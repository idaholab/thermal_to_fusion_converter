#!/usr/bin/env python


import numpy as np


'''
computes source intensity for (1) cylinder (2-3) bot/top surfaces
using surface probabilities
'''
radius = 0.8 # cm 
length = 2.8 # cm 

cyl_surf = 2*np.pi*radius*length 
circle_surf = np.pi*radius**2 
tot_surf = cyl_surf+2*circle_surf 

proba_cyl = cyl_surf / tot_surf 
proba_circle = circle_surf / tot_surf 

print ("intensity for cylinder = {:.2f}".format(proba_cyl))
print ("intensity for circle (bot/top) = {:.2f}".format(proba_circle))
