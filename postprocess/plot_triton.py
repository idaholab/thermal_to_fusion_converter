#!/usr/bin/env python 

import sys
import numpy as np 
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams.update({'font.size': 18})




distance_He3D2 = [0,0.0555,0.0996,0.1353,0.1638,0.187,0.2059,0.221,0.2338,0.2436,
            0.2517,0.2579,0.2634,0.2682,0.2723,0.2763]

energy_He3D2 = [0.1914,0.1362,0.09748,0.06525,0.04227,0.02663,
          0.01574,0.009153,0.006282,0.004162,0.002241,0.002038,
          0.001606,0.001295,0,0]


distance_LiD = [
0,
0.001399,
0.002532,
0.003439,
0.00416,
0.004739,
0.005204,
0.00558,
0.005882,
0.006124,
0.006318,
0.006474,
0.0066,
0.0067,
0.006781,
0.006845,
0.006896,
0.006936,
0.006967,
0.006991,
0.007011,
0.007027,
0.00704,
0.007051,
0.007061,
0.007061]

energy_LiD = [
2.73,
2.392,
2.075,
1.787,
1.545,
1.327,
1.141,
0.9694,
0.8096,
0.6735,
0.5462,
0.4353,
0.3335,
0.2468,
0.1762,
0.1176,
0.07501,
0.04549,
0.02744,
0.01656,
0.01101,
0.007264,
0.005292,
0.003538,
0,
0
]


fig,ax = plt.subplots(figsize=(15,8))
ax.plot(distance_LiD,energy_LiD,linewidth=4,marker='d',markersize=12,label='LiD')
ax.plot(distance_He3D2,energy_He3D2,linewidth=4,marker='s',markersize=12,label='$^3$HeD$_2$')

ax.set_xlabel('Length (cm)')
ax.set_ylabel('Energy (MeV)')
#ax.set_xscale('log')
ax.set_ylim([0,max(energy_LiD)])
ax.set_xticks([0.,0.025,0.05,0.10,0.15,0.20,0.25,0.3])
ax.set_xlim([0,max(distance_He3D2)])
plt.title('Triton range in LiD and He3D2')
plt.grid(True)
plt.legend()
plt.savefig('triton_slowing_down.png')
plt.close()
