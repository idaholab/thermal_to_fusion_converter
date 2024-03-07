#!/usr/bin/env python




P_he3 = 5e5 # Pascal 
T = 344 # K 
R = 8.314 # J/mol/K 
M_he3 = 3.016 # g/mol
# volume of one mole of He3 
# rho = PM/(RT)

rho_he3 = P_he3*M_he3/ (R*T) # g/m3
rho_he3/=1e6 # g/cm3
print ('Pressure = {} Pa, Temperature = {} K'.format(P_he3,T))
print ('Helium3 density = {:.3e} g/cm3'.format(rho_he3))

P_d2 = 5e5 # Pa
M_d2 = 4.08 # g/mol 
rho_d2 = P_d2*M_d2/ (R*T) # g/m3
rho_d2/=1e6 # g/cm3
print ('D2 density = {:.3e} g/cm3'.format(rho_d2))


dens_mix_1_1 =  1e-6* (P_he3*M_he3+P_d2*M_d2) / (R*T)
dens_mix_1_2 =  1e-6* (P_he3*M_he3+2*P_d2*M_d2) / (R*T)
dens_mix_1_3 =  1e-6* (P_he3*M_he3+3*P_d2*M_d2) / (R*T)

print ('density for mixture 1:1  = {:.3e} g/cm3'.format(dens_mix_1_1))
print ('density for mixture 1:2  = {:.3e} g/cm3'.format(dens_mix_1_2))
print ('density for mixture 1:3  = {:.3e} g/cm3'.format(dens_mix_1_3))

