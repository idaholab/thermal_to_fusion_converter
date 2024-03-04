
#!/usr/bin/env python
'''utility for reading geant4 output from terminal'''


#-----------------
def get_fusion_neutron_yield(file):
    n=0
    q=0
    for line in open(file):
        if 'triton + Deuterium --> neutron + alpha:' in line:
            line_split = line.split()
            n = int(line_split[7])
            q  = float(line_split[10])
            break 
    return n,q
#--------------------
def get_fast_neutron_yield(file):
    n = 0
    q = 0 
    n_reaction = 0
    for line in open(file):
        if 'triton + Li6 --> neutron + 2 alpha:' in line:
            line_split = line.split()
            n  += int(line_split[8])
            q  += float(line_split[11])  
            n_reaction += 1 
        if 'triton + Li7 --> 2 neutron + 2 alpha:' in line:
            line_split = line.split()
            n  += int(line_split[9])
            q  += float(line_split[12])
            n_reaction += 1 
        if  'triton + Li7 --> Be9 + neutron:' in line:
            line_split = line.split()
            n  += int(line_split[7])
            q  += float(line_split[10])
            n_reaction += 1 
        if  'triton + Li7 --> neutron + Be9:' in line:
            line_split = line.split()
            n  += int(line_split[7])
            q  += float(line_split[10])
            n_reaction += 1 
    if n_reaction>0:
        q/=n_reaction
    return n,q
#-----------------
    
def get_edep(file):
    for line in open(file):
        if ' Edep in absorber 1 =' in line:
            line_split = line.split()
            edep = float(line_split[5])
            if line_split[6] == 'keV':
                edep *= 1e-3
            return edep
#-----------------
def get_triton_yield(file):
    for line in open(file):
        if 'neutron + Li6 --> triton + alpha' in line:
            line_split = line.split()
            n = int(line_split[7])
            q = float(line_split[10])
            return n,q
        if 'neutron + Helium3 --> proton + triton:' in line:
            line_split = line.split()
            n = int(line_split[7])
            q = float(line_split[10])
            return n,q
#-----------------