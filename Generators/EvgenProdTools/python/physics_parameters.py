
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------------------------------
#       
#       Script to build dictionary with physics parameters from PDG API:
#       Dictionary is exported to file (offline_dict.py).
#       Once a new PDG API is availabe, this file needs to be updated with
#       the right year and run again with python physics_parameters.py 
#       to update the file offline_dict.py.
#       All other times, the dictionary created by this file can simply be 
#       integrated in the generator interfaces with 
#       from EvgenProdTools.offline_dict import parameters.
#
#-----------------------------------------------------------------------------
import os

# Check if file with PDG values already exists:
# The name of the file needs to be altered once a new PDG API is available
if os.path.isfile('./mass_width_2019.mcd'):
    # Open the existing file (
    f = open('mass_width_2019.mcd', "r")
    print 'File mass_width_2019.mcd already exists.'
else: 
    # Load the particle data from the PDG API:
    # The name of the file needs to be altered once a new PDG API is available
    import urllib2
    url = 'http://pdg.lbl.gov/2019/mcdata/mass_width_2019.mcd'
    response = urllib2.urlopen(url)
    html = response.read()

    # Write the data in a file 
    file_name = url.split('/')[-1]
    f = open(file_name, 'wb')
    f.write(html)
    f = open(file_name, "r")
    print 'Particle data was fetched from PDG API and put into %s.' % file_name

# Method to extract nonblank lines
def nonblank_lines(f):
    for l in f:
        line = l.rstrip()
        if line:
            yield line

# Loop over lines and strip to extract the parameters in a list
stripped_list = []

for line in nonblank_lines(f):
    stripped_line = line.strip()
    if not stripped_line.startswith('*'):
        split_line = line.strip().split(" ")
        if len(split_line) > 1 :
            for i in split_line:
                if i != "":
                    stripped_list.append(i)

# Function to detect integer
def isint(value):
    try:
        int(value)
        if (int(value) != 12 and int(value) != 14 and int(value) != 16):
            return value
    except ValueError:
        return False

# Function to detect float
def isfloat(value):
    try:
        float(value)
        return value
    except ValueError:
        return False

# Write particle dictionary from PDG API:
# differenciate between particles with widths and without

p_dict = dict([])

for i in range(0,len(stripped_list)):
    if (isint(stripped_list[i]) and stripped_list[i] != '0'):
            # Particle with 4 PIDs      
        if (isint(stripped_list[i]) and isint(stripped_list[i+1]) and isint(stripped_list[i+2]) and isint(stripped_list[i+3])):
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+10]),('mass',stripped_list[i+4]),('width',stripped_list[i+7])])
            # Particle with 3 PIDs
        elif (isint(stripped_list[i]) and isint(stripped_list[i+1]) and isint(stripped_list[i+2])):
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+9]),('mass',stripped_list[i+3]),('width',stripped_list[i+6])])
            # Particle with 2 PIDS and width
        elif (isint(stripped_list[i]) and isint(stripped_list[i+1]) and isfloat(stripped_list[i+5])):
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+8]),('mass',stripped_list[i+2]),('width',stripped_list[i+5])])
            # Particle with 2 PIDS and no width
        elif (isint(stripped_list[i]) and isint(stripped_list[i+1]) and not isfloat(stripped_list[i+5])):
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+5]),('mass',stripped_list[i+2]),('width','0.E+00')])
            # Particle with 1 PID and no width
        elif (isint(stripped_list[i]) and not isfloat(stripped_list[i+4])):
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+4]),('mass',stripped_list[i+1]),('width','0.E+00')])
            # Particle with 1 PID and width
        else:  
            p_dict[stripped_list[i]]=dict([('name',stripped_list[i+7]),('mass',stripped_list[i+1]),('width',stripped_list[i+4])])
        
# Write EW parameter dictionary 

EW_dict = dict([])

EW_dict[('SIN2THETAW','sin2thetaW','Sin2ThetaW')]='0.23113'
EW_dict[('SIN2THETAWbar','sin2thetaWbar','Sin2ThetaWbar')]='0.23146'

EW_dict[('SIN2THETAWbar','sin2thetaWbar','Sin2ThetaWbar')]='0.23146'

# All the following values are taken from and moved here from the file 'atlas_common.py' from Powheg
# Branching ratios

EW_dict['W_to_enu'] = 0.1082
EW_dict['W_to_leptons'] = 3*EW_dict['W_to_enu']
EW_dict['W_to_hadrons'] = 1.0 - EW_dict['W_to_leptons']
EW_dict['t_to_Wb'] = 1.0
EW_dict['t_to_Ws'] = 0.0
EW_dict['t_to_Wd'] = 0.0

# CKM matrix
EW_dict['Vud'] = 0.97428
EW_dict['Vus'] = 0.2253
EW_dict['Vub'] = 0.00347
EW_dict['Vcd'] = 0.2252
EW_dict['Vcs'] = 0.97345
EW_dict['Vcb'] = 0.041
EW_dict['Vtd'] = 0.00862
EW_dict['Vts'] = 0.0403
EW_dict['Vtb'] = 0.999152
	
# Coupling constants
# EM coupling
EW_dict['alphaem_0'] = 0.00729735252  # 1/137.036
# EM coupling
EW_dict['alphaem'] = 0.00781653039  # 1/127.934
# Strong coupling
EW_dict['alphaqcd'] = 0.1185
# Fermi constant
EW_dict['G_F'] = 0.00001166397
# sin(theta_Cabibbo) ^ 2
EW_dict['sin2cabibbo'] = 0.051 

# Write combined dictionary

parameters = dict([])
parameters['particles']=p_dict
parameters['EW_parameters']=EW_dict


# Write dictionary in file 'offline_dict.py' 
with open("offline_dict.py", 'wb') as file:
    file.write("parameters = { \n")
    for k in sorted (parameters.keys()):
        #if k == 'particles':
        file.write("'%s':{ \n" % k)
        for key in sorted (parameters[k].keys()):
            lastkey = sorted (parameters[k].keys())[-1]
            secondtolastkey = sorted (parameters[k].keys())[-2]
            if key == lastkey:
                if k == 'EW_parameters':
                    file.write("%s:%s}, \n" % (key, parameters[k][key]))
                else:
                    file.write("'%s':%s} \n" % (key, parameters[k][key]))
            elif key == secondtolastkey:
                if k == 'EW_parameters':
                    file.write("%s:%s, \n" % (key, parameters[k][key]))
            else:
                file.write("'%s':%s, \n" % (key, parameters[k][key]))
    file.write("}")

f.close()
