#!/usr/bin/env python 

import sys
import numpy as np 
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams.update({'font.size': 18})
import pandas as pd 
import glob 
from pathlib import Path,PurePath
import math
from mpl_toolkits.axes_grid1.inset_locator import (inset_axes, InsetPosition,mark_inset) 
#-----------------
def get_total_neutrons(bins,csv_list,energy_cutoff):
    ''' return the total number of entries in csv files for E>energy_cutoff'''
    neutron_yield = []
    for csv_file in csv_list:
        df = pd.read_csv(csv_file,skiprows=6)
        indices = np.where(bins>=energy_cutoff)
        n = df['entries'].iloc[indices].sum()
        neutron_yield.append(n)
    return np.asfarray(neutron_yield)
#------------------------
def get_frac_spectrum_in_target(bins,spectrum,energy_cutoff):
    ''' get fraction of spectrum in target  - spectrum already normalized to 1'''
    indices = np.where(bins>=energy_cutoff) 
    return sum(spectrum[indices])
#------------------------
def get_spectrum_from_csv(csv_list):
    '''return spectrum (normalized to 1) and energy bins from a list of csv files'''
    #get bins - read first csv since all identical 
    csv_file = csv_list[0]
    axis_fixed = False 
    for line in open(csv_file):
        if not "#" in line: continue
        line_split = line.split()
        if "axis fixed" in line:
            axis_fixed = True 
            vmin = float(line_split[3])
            vmax = float(line_split[4])
        if "edges" in line:
            bins = [1e-11]
            for e in line_split[2:]:
                bins.append(float(e))
        if "bin_number" in line:
            nbins = int(line_split[1])
    if axis_fixed:
        bins = np.linspace(vmin,vmax,nbins)
        #bins = np.insert(bins,0,0)
        bar_width = (vmax-vmin) / nbins
    # dataframe 
    df = pd.read_csv(csv_file,skiprows=6)
    for csv_file in csv_list[1:]:
        df_ = pd.read_csv(csv_file,skiprows=6)
        df['entries'] += df_['entries']
    spectrum = df['entries'].to_numpy(dtype=np.float32)
    sum_spectrum = sum(spectrum)
    if sum_spectrum!=0.:
        spectrum /=sum(spectrum)
    bins = np.array(bins)
    return spectrum,bins
#------------------------
def get_lethargy(bins):
    ''' computes lethargy per group'''
    i = 0
    lethargy = np.full_like(bins,0.)
    for e0,e1 in zip(bins,bins[1:]):
        lethargy[i] = np.log(e1/e0)
        i+=1
    return lethargy
#------------------------
def plot_spectrum(rundir,number_of_primary_neutrons,energy_cutoff,position,tot_flux):
    ''' 
    input: rundir : directory where csv files are present
           number_of_primary_neutrons*n_batch
           energy_cutoff: threshold for "fast" neutrons in MeV
           position: name of simulation / material chosen
    generate triton_spectrum_{}.png,
        neutron_spectrum_secondary_{}.png, 
        neutron_spectrum_in_target_{}.png, 
        neutron_spectrum_emitted_{}.png '''
#------------------------
    

    #-------
    # index 14: Triton emission spectrum
    #-------

    csv_list = glob.glob("{}/spectrum_*_h1_14.csv".format(rundir))
    triton_spectrum,bins = get_spectrum_from_csv(csv_list)
    fig,ax = plt.subplots(figsize=(15,8))
    ax.step(bins,triton_spectrum,where='post')
    ax.set_xlabel('Triton Spectrum (MeV)')
    ax.set_ylabel('Triton flux (normalized to 1)')
    #ax.set_xscale('log')
    ax.set_xlim([min(bins),max(bins)])
    plt.title('Triton Energy (MeV)')
    plt.grid(True)
    #plt.legend()
    plt.savefig('{}/triton_spectrum_{}.png'.format(rundir,position))
    plt.close()




    #-------
    # index 15: source / primary neutron emission spectrum to verify gps
    #-------

    csv_list = glob.glob("{}/spectrum_*_h1_15.csv".format(rundir))
    source_neutrons_geant,bins = get_spectrum_from_csv(csv_list)
    lethargy = get_lethargy(bins)
    source_neutrons_geant_du = np.divide(source_neutrons_geant, lethargy, out=np.zeros_like(source_neutrons_geant), where=lethargy!=0)
    source_neutrons_geant_du /= sum(source_neutrons_geant_du)
    fig,ax = plt.subplots(figsize=(15,8))
    source_neutrons_mcnp_du = np.loadtxt('atr_spectrum/spectrum_normalized_{}.txt'.format(position))
    ax.step(bins,source_neutrons_geant_du,linestyle='--',where='post',label='GEANT')
    ax.stairs(source_neutrons_mcnp_du,bins,label='MCNP')
    ax.set_xlabel('Energy (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (normalized to 1)')
    ax.set_xscale('log')
    ax.set_xlim([min(bins),max(bins)])
    plt.title('Primary Neutron Energy (MeV) in position {}'.format(position))
    plt.grid(True)
    plt.legend()
    plt.savefig('{}/phi_du_primary_loglin_{}.png'.format(rundir,position))
    plt.close()


    #-------
    # index 16 & 17 : secondary neutron spectrum emitted in LiD due to D-T and Li-T reactions
    #-------

    csv_list = glob.glob("{}/spectrum_*_h1_16.csv".format(rundir))
    phi_DT,bins = get_spectrum_from_csv(csv_list)
    phi_DT_du = np.divide(phi_DT, lethargy, out=np.zeros_like(phi_DT), where=lethargy!=0)
    phi_DT_du_normalized = phi_DT_du / sum(phi_DT_du)
    n_neutrons_DT  = get_total_neutrons(bins,csv_list,energy_cutoff)

    csv_list = glob.glob("{}/spectrum_*_h1_17.csv".format(rundir))
    phi_TLi,bins = get_spectrum_from_csv(csv_list)

    phi_TLi_du = np.divide(phi_TLi, lethargy, out=np.zeros_like(phi_TLi), where=lethargy!=0)
    if sum(phi_TLi)!=0.:#happens for He3D2
        phi_TLi_du_normalized = phi_TLi_du / sum(phi_TLi_du)
    else:
        phi_TLi_du_normalized = phi_TLi_du

    n_neutrons_TLi  = get_total_neutrons(bins,csv_list,energy_cutoff)

    fig,ax = plt.subplots(figsize=(15,8))
    phi_DT_du = phi_DT_du_normalized*tot_flux
    phi_TLi_du = phi_TLi_du_normalized*tot_flux
    ax.step(bins,phi_DT_du,where='post',label='T-D neutrons')
    ax.step(bins,phi_TLi_du,where='post',label='T-Li neutrons')
    ax.set_xlabel('Secondary Neutron Spectrum (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (cm$^2$.s$^{-1}$)')
    #ax.set_yscale('log')
    ax.set_xscale('log')
    ax.set_xlim([0.1,20])
    ax.set_xticks([0.1,1,10,20],[0.1,1,10,20])
    #ax.set_ylim([min(phi_DT_du),max(phi_DT_du)*100])

    plt.legend()
    plt.title('Neutron Energy (MeV)')
    plt.grid(True)
    plt.savefig('{}/phi_du_secondary_loglin_{}.png'.format(rundir,position))
    plt.close()

    fig,ax = plt.subplots(figsize=(15,8))
    ax.step(bins,phi_DT_du,where='post',label='T-D neutrons')
    ax.step(bins,phi_TLi_du,where='post',label='T-Li neutrons')
    ax.set_xlabel('Secondary Neutron Spectrum (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (cm$^2$.s$^{-1}$)')
    ax.set_yscale('log')
    ax.set_xscale('log')
    ax.set_xlim([0.1,20])
    plt.title('Neutron Energy (MeV)')
    plt.legend()
    plt.grid(True)
    plt.savefig('{}/phi_du_secondary_loglog_{}.png'.format(rundir,position))
    plt.close()

    #-------
    # total neutrons reaching target 
    #-------

    # neutrons in target
    csv_list = glob.glob("{}/spectrum_*_h1_18.csv".format(rundir))
    phi_target,bins = get_spectrum_from_csv(csv_list)
    #hack to remove odd thermal flux 
    #phi_target[0:2] = 0.
    phi_du_target = np.divide(phi_target, lethargy, out=np.zeros_like(phi_target), where=lethargy!=0)
    phi_du_target /= sum(phi_du_target)

    # #debug 
    # df = pd.DataFrame()
    # df['energy'] = pd.Series(bins)
    # df['converter'] = pd.Series(phi_target)
    # df['atr'] = pd.Series(source_neutrons_geant)
    # df.to_csv('debug.csv')
    #absolute flux - log-log
    fig,ax = plt.subplots(figsize=(20,10))
    ax.step(bins,source_neutrons_geant_du*tot_flux,where='post',linestyle='--',label='ATR unperturbed spectrum')
    ax.step(bins,phi_du_target*tot_flux,where='post',label='neutrons in target with converter')
    ax.set_xlabel('Neutron Spectrum (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (cm$^2$.s$^{-1}$)')
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlim([min(bins),max(bins)])
    ax.grid(True)
    if position == 'A11':
        ax.legend(loc=2)
        ax2 = inset_axes(ax,width="125%",height="125%",bbox_to_anchor=(.5,.3,.4,.4),bbox_transform=ax.transAxes)
        ax2.step(bins,source_neutrons_geant_du*tot_flux,where='post',linestyle='--')
        ax2.step(bins,phi_du_target*tot_flux,where='post')
        #ax2.set_xscale('log')
        ax2.set_yscale('log')
        ax2.set_xticks([])
        ax2.set_xlim([1,20])
        ax2.set_ylim([1.26e7,3e13])
        ax2.set_xticks([1,5,10,15,20],[1,5,10,15,20])
        ax2.grid(True)
        box,c = ax.indicate_inset_zoom(ax2, edgecolor="black")
        c[0].set_visible(True)
        c[1].set_visible(True)  
        c[2].set_visible(True)  
        c[3].set_visible(True)
        plt.setp([c[:]], linestyle=":", lw=0.7)
    elif position == 'OS15':
        ax.legend(loc=2)
        ax2 = inset_axes(ax,width="125%",height="125%",bbox_to_anchor=(.5,.2,.4,.4),bbox_transform=ax.transAxes)
        ax2.step(bins,source_neutrons_geant_du*tot_flux,where='post',linestyle='--')
        ax2.step(bins,phi_du_target*tot_flux,where='post')
        #ax2.set_xscale('log')
        ax2.set_yscale('log')
        ax2.set_xticks([])
        ax2.set_xlim([1,20])
        ax2.set_ylim([1e6,1e10])
        ax2.set_xticks([1,5,10,15,20],[1,5,10,15,20])
        ax2.grid(True)
        box,c = ax.indicate_inset_zoom(ax2, edgecolor="black")
        c[0].set_visible(True)
        c[1].set_visible(True)  
        c[2].set_visible(True)  
        c[3].set_visible(True)
        plt.setp([c[:]], linestyle=":", lw=0.7)
    ax.set_title('Neutron Energy (MeV)')
    plt.savefig('{}/spectrum_in_target_loglog_{}.png'.format(rundir,position))
    plt.close()

    fig,ax = plt.subplots(figsize=(15,8))
    ax.step(bins,source_neutrons_geant_du,where='post',linestyle='--',label='ATR unperturbed spectrum')
    ax.step(bins,phi_du_target,where='post',label='neutrons in target with converter')
    ax.set_xlabel('Neutron Spectrum (MeV)')
    ax.set_ylabel('Neutron flux per unit lethargy (normalized)')
    #ax.set_yscale('log')
    ax.set_xscale('log')
    ax.set_xlim([min(bins),max(bins)])
    ax.set_title('Neutron Energy (MeV)')
    ax.grid(True)
    ax.legend()
    if position == "A11":
        ax2 = inset_axes(ax,width="50%",height="50%",bbox_to_anchor=(.3,.3,.4,.4),bbox_transform=ax.transAxes)
        ax2.step(bins,source_neutrons_geant_du,where='post',linestyle='--')
        ax2.step(bins,phi_du_target,where='post')
        #ax2.set_xscale('log')
        ax2.set_yscale('log')
        ax2.set_xticks([])
        ax2.set_xlim([10,20])
        ax2.set_ylim([1e-9,1e-3])
        ax2.set_xticks([10,15,20],[10,15,20])
        ax2.grid(True)
        box,c = ax.indicate_inset_zoom(ax2, edgecolor="black")
        c[0].set_visible(True)
        c[1].set_visible(True)  
        c[2].set_visible(True)  
        c[3].set_visible(True)
        plt.setp([c[:]], linestyle=":", lw=0.7)
    if position == "OS15":
        ax2 = inset_axes(ax,width="50%",height="50%",bbox_to_anchor=(.6,.2,.4,.4),bbox_transform=ax.transAxes)
        ax2.step(bins,source_neutrons_geant_du,where='post',linestyle='--')
        ax2.step(bins,phi_du_target,where='post')
        #ax2.set_xscale('log')
        ax2.set_yscale('log')
        ax2.set_xticks([])
        ax2.set_xlim([5,20])
        ax2.set_ylim([1e-9,1e-3])
        ax2.set_xticks([5,10,15,20],[5,10,15,20])
        ax2.grid(True)
        box,c = ax.indicate_inset_zoom(ax2, edgecolor="black")
        c[0].set_visible(True)
        c[1].set_visible(True)  
        c[2].set_visible(True)  
        c[3].set_visible(True)
        plt.setp([c[:]], linestyle=":", lw=0.7)
    plt.savefig('{}/spectrum_in_target_normalized_loglin_{}.png'.format(rundir,position))
    plt.close()
    
    spectrum_in_target  = get_frac_spectrum_in_target(bins,phi_target,energy_cutoff)

    return n_neutrons_DT,n_neutrons_TLi,spectrum_in_target

