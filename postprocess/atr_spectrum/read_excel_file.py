#!/usr/bin/env python 

import numpy as np
import pandas as pd 
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams.update({'font.size': 18})
import sys
import csv 

#-----------------------------
def get_spectrum(xls_file,energy_cutoff_list,position,writer):
    '''read excel spreadsheet and return energy bin & spectrum'''
    xls = pd.ExcelFile(xls_file)
    df = pd.read_excel(xls,'Aluminum')
    energy_bin_edge = df['Unnamed: 0']
    spectrum = df['percent_shield_thickness_001'].to_numpy(dtype=np.float32)
    #insert 1e-11 at beginning to use with step function for plotting 
    energy_bin_edge = np.insert(energy_bin_edge,0,1e-11)
    #spectrum = np.insert(spectrum,0,0.)
    # thermal, epithermal, fast energy in relative fraction
    total_flux = sum(spectrum)
    row = [position]
    n = sum(df[df['Unnamed: 0'] <= energy_cutoff_list[0] ]['percent_shield_thickness_001'])
    row.append(n/total_flux)
    for e0,e1 in zip(energy_cutoff_list,energy_cutoff_list[1:]):
        n = sum(df[ (df['Unnamed: 0'] > e0) & ( df['Unnamed: 0']<= e1) ]['percent_shield_thickness_001'])
        row.append(n/total_flux)
    n = sum(df[df['Unnamed: 0'] > energy_cutoff_list[-1] ]['percent_shield_thickness_001'])
    row.append(n/total_flux)
    row.append(total_flux)
    writer.writerow(row)
    return energy_bin_edge,spectrum
#-----------------------------


if __name__ == "__main__":
    position_list = ['A11','NEFT','B5','B11','I14','I11','OS5','OS15']
    # spectrum for use in geant4
    spectrum = {} # absolute spectrum in n/cm2/s
    spectrum_normalized = {}  # spectrum normalized to 1
    # spectrum divided per unit lethargy, for plotting purpose only 
    spectrum_du = {} # spectrum divided per unit lethargy
    spectrum_du_normalized = {} # spectrum divided per unit lethargy normalized to 1



    # csv table for energy breakdown 
    energy_cutoff_list = [0.625e-6,0.5,10] # energy group boundaries in MeV
    csv_file = open('energy_breakdown_atr.csv','w')
    writer = csv.writer(csv_file)
    columns = ['position']
    columns.append('< {:.2e} MeV'.format(energy_cutoff_list[0]))
    for e0,e1 in zip(energy_cutoff_list,energy_cutoff_list[1:]):
        columns.append('[{:.2e},{:.2e}] MeV'.format(e0,e1))
    columns.append('> {:.2e} MeV'.format(energy_cutoff_list[-1]))
    columns.append('total flux')
    writer.writerow(columns)


    for position in position_list:
        xls_file = '{}_Spectrum_Mod_Scope.xlsx'.format(position)
        energy_bin_mcnp,spectrum[position] = get_spectrum(xls_file,energy_cutoff_list,position,writer)

    csv_file.close()

    # computes bin center and width for each energy group
    for position in position_list:
        energy_bin_center = np.full_like(spectrum[position],0.)
        energy_bin_width = np.full_like(spectrum[position],0.)
        lethargy = np.full_like(spectrum[position],0.)
        break 
    # computes lethargy
    i = 0
    for e0,e1 in zip(energy_bin_mcnp,energy_bin_mcnp[1:]):
        energy_bin_center[i] = (e0+e1)/2 
        energy_bin_width[i] = e1-e0
        lethargy[i] = np.log(e1/e0)
        i+=1

    # computes spectrum (normalized to 1), spectrum divided by lethargy
    for position in position_list:
        spectrum_normalized[position] = spectrum[position] / sum(spectrum[position])
        spectrum_du[position] = np.divide(spectrum[position], lethargy, out=np.zeros_like(spectrum[position]), where=lethargy!=0)
        spectrum_du_normalized[position] = spectrum_du[position] / sum(spectrum_du[position]) 
        #save it for comparing against what geant samples 
        np.savetxt('spectrum_normalized_{}.txt'.format(position),spectrum_du_normalized[position])



    #-----
    # verification of gps: plot the spectrum and a sampled case 
    #------
    debug  = False
    if debug:
        N = 10000000
        fig,ax = plt.subplots(figsize=(15,8))
        #individual regions normalized to 1 
        for position in position_list:
            ax.stairs(spectrum_du_normalized[position],energy_bin_mcnp,label='MCNP')
            #add a sampled case for testing only
            sample = np.random.choice(energy_bin_center,N,p=spectrum_normalized[position])
            sample_bin = np.histogram(sample,bins=energy_bin_mcnp)
            spectrum_sample_du = np.divide(sample_bin[0], lethargy, out=np.zeros_like(spectrum[position]), where=lethargy!=0)
            spectrum_sample_du_normalized = spectrum_sample_du/sum(spectrum_sample_du)
            ax.stairs(spectrum_sample_du_normalized,energy_bin_mcnp,label='sampled')
            ax.set_xscale('log')
            ax.set_xlim([min(energy_bin_mcnp),max(energy_bin_mcnp)])
            ax.set_xlabel('Energy (MeV)')
            ax.set_ylabel('Neutron flux per unit lethargy (normalized to 1)')
            ax.set_title('Neutron spectrum for position {} in ATR'.format(position))
            plt.grid(True)
            plt.legend()
            plt.savefig('phi_du_loglin_debug_{}.png'.format(position))
            plt.close() 


    for position in position_list:
        # write hist/point for geant
        with open('hist_ene_{}.txt'.format(position),'w') as f:
            for e,s in zip(energy_bin_mcnp[1:],spectrum_normalized[position]):
                f.write('/gps/hist/point {:.3e} {:.3e}\n'.format(e,s))



    #-------
    # plot the different spectrum normalized per unit lethargy
    #-------
        
    fig,ax = plt.subplots(figsize=(15,10))
    for position in position_list:
        ax.stairs(spectrum_du[position],energy_bin_mcnp,label=position)
    ax.set_xscale('log')
    ax.set_xlim([min(energy_bin_mcnp),max(energy_bin_mcnp)])
    ax.set_xlabel('Energy (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (cm$^{-2}$.s$^{-1}$)')
    ax.set_title('Neutron spectrum for different locations in ATR')
    plt.grid(True)
    plt.legend()
    plt.savefig('phi_du_loglin.png')
    plt.close() 

    fig,ax = plt.subplots(figsize=(15,10))
    for position in position_list:
        ax.stairs(spectrum_du[position],energy_bin_mcnp,label=position)
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlim([min(energy_bin_mcnp),max(energy_bin_mcnp)])
    ax.set_xlabel('Energy (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (cm$^{-2}$.s$^{-1}$)')
    ax.set_title('Neutron spectrum for different locations in ATR')
    plt.grid(True)
    plt.legend()
    plt.savefig('phi_du_loglog.png')
    plt.close() 

    fig,ax = plt.subplots(figsize=(15,10))
    for position in position_list:
        ax.stairs(spectrum_du_normalized[position],energy_bin_mcnp,label=position)
    ax.set_xscale('log')
    ax.set_xlim([min(energy_bin_mcnp),max(energy_bin_mcnp)])
    ax.set_xlabel('Energy (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (normalized to 1)')
    ax.set_title('Neutron spectrum for different locations in ATR')
    plt.grid(True)
    plt.legend()
    plt.savefig('phi_du_normalized_loglin.png')
    plt.close() 




    # #write C++ array for use in geant 
    # f = open('scale252.txt','w')
    # n=1
    # for e in energy_bin_mcnp[1:]:
    #     f.write('{:.3e}, '.format(e))
    #     if n%10==0:
    #         f.write('\n')
    #     n+=1
    # f.close()

