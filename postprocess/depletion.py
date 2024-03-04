#!/usr/bin/env python

'''computes the lifetime of LiD (or other breeder material)'''

import numpy as np 

lid_density = 0.8 # g/cm3
lid_inner_radius = 0.6 # cm 
lid_outer_radius = 0.7 # cm
lid_length = 5 # cm
lid_volume = np.pi*(lid_outer_radius**2-lid_inner_radius**2)*lid_length  # cm3 

lid_mass = lid_volume * lid_density # g

lid_molar_mass = 8.954 # g/mol

lid_mol = lid_mass / lid_molar_mass

print ('moles of LiD = {:.3e}'.format(lid_mol))

Na = 6.022e23 # mol-1

NLi = lid_mol*Na 
print ('number of Li6 or D atoms = {:.3e}'.format(NLi))

# assume flux of 10^13 
phi = 1e13

reaction_rate_Li = 0.3*phi # 0.3 = number of neutrons absorber in Li6 per source neutron

t = NLi / reaction_rate_Li # s
seconds_to_year = 3600*24*365.25

print ('estimated duration = {:.2e} year'.format(t/seconds_to_year))
