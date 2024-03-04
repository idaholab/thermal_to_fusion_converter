#!/usr/bin/env python



P = 5 # bars

P*= 1e5 # Pascal 
T = 344 # K 
R = 8.314 # J/mol/K 
M_he3 = 3.016 # g/mol
# volume of one mole of He3 
# rho = PM/(RT)

rho_he3 = P*M_he3/ (R*T) # g/m3
rho_he3/=1e6 # g/cm3
print ('Pressure = {} Pa, Temperature = {} K'.format(P,T))
print ('Helium3 density = {:.3e} g/cm3'.format(rho_he3))

M_d2 = 4.08 # g/mol 
rho_d2 = P*M_d2/ (R*T) # g/m3
rho_d2/=1e6 # g/cm3
print ('D2 density = {:.3e} g/cm3'.format(rho_d2))


dens_mix_1_1 = rho_d2 * (1 + 1   * M_he3/M_d2) 
dens_mix_1_2 = rho_d2 * (1 + 1/2 * M_he3/M_d2) 
dens_mix_1_3 = rho_d2 * (1 + 1/3 * M_he3/M_d2)

print ('density for mixture 1:1  = {:.3e} g/cm3'.format(dens_mix_1_1))
print ('density for mixture 1:2  = {:.3e} g/cm3'.format(dens_mix_1_2))
print ('density for mixture 1:3  = {:.3e} g/cm3'.format(dens_mix_1_3))

