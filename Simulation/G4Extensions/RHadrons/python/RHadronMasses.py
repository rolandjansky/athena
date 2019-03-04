# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# This file contains a number of helper functions for defining R-hadron mass spectra
# A large data table at the top is then used in several of the helper functions

"""
The offset options.  Dictionary of PDG IDs with offsets.
Anti-particles by definition have the same mass as standard particles.
Columns are:
   PDG ID
   PDG ID of SUSY constituent
   Has an anti-particle (i.e. need to define a particle with an opposite-signed PDG ID)
   Name (for PDG table)
   Charge (for PDG table)

   Mass Offset in the Pythia6 simulation configuration
   8 mass options taken from slides 4 and 5 of ESK's presentation here:
https://indico.cern.ch/event/748024/contributions/3094005/attachments/1701409/2740180/GlRhadMasses_20180816.pdf
   First 3 are as shown on slide 4; 4th is the first specrum shifted to make the gluino R-glueball heaver 
    than the lightest R-mesons, but with the same relative splitting otherwise.
   Second set of four follow the same pattern, but for the splittings shown on slide 5.

The list of possible R-hadrons comes from the Pythia8 code, for example:
/afs/cern.ch/sw/lcg/external/MCGenerators_lcgcmt67c/pythia8/235/share/sources/src/RHadrons.cc 
"""

first_mass_set = 4
offset_options = {
# Fundamental SUSY particles
        1000021 : [       0 , False , '~g          ' ,  0 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 ] ,
        1000022 : [       0 , False , '~chi10      ' ,  0 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 ] ,
        1000039 : [       0 , False , '~Gr         ' ,  0 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 , 0.000 ] ,
# Gluino R-glueball
        1000993 : [ 1000021 , False , '~g_glueball ' ,  0 , 0.700 , 0.700 , 0.700 , 0.700 , 0.700 , 0.700 , 0.700 , 0.700 , 0.700 ] ,
# Gluino q-qbar R-mesons
        1009113 : [ 1000021 , False , '~g_pi0      ' ,  0 , 0.650 , 0.682 , 0.551 , 0.770 , 0.000 , 0.770 , 0.551 , 0.770 , 0.000 ] ,
        1009223 : [ 1000021 , False , '~g_omega    ' ,  0 , 0.650 , 0.609 , 0.402 , 0.716 , 0.000 , 0.716 , 0.402 , 0.716 , 0.000 ] ,
        1009333 : [ 1000021 , False , '~g_phi      ' ,  0 , 1.800 , 0.923 , 0.827 , 1.073 , 0.000 , 1.053 , 0.827 , 1.073 , 0.000 ] ,
        1009443 : [ 1000021 , False , '~g_eta_c    ' ,  0 , 3.400 , 3.262 , 3.102 , 3.421 , 0.000 , 3.401 , 3.102 , 3.421 , 0.000 ] ,
        1009553 : [ 1000021 , False , '~g_eta_b    ' ,  0 ,10.000 ,10.000 ,10.000 ,10.000 ,10.000 ,10.000 ,10.000 ,10.000 ,10.000 ] ,
# Light-flavor Gluino R-mesons
        1009213 : [ 1000021 ,  True , '~g_rho+     ' ,  1 , 0.650 , 0.682 , 0.551 , 0.770 , 0.000 , 0.770 , 0.551 , 0.770 , 0.000 ] ,
# Strange Gluino R-mesons
        1009313 : [ 1000021 ,  True , '~g_K*0      ' ,  0 , 0.825 , 0.816 , 0.697 , 0.927 , 0.000 , 0.922 , 0.697 , 0.927 , 0.000 ] ,
        1009323 : [ 1000021 ,  True , '~g_K*+      ' ,  1 , 0.825 , 0.816 , 0.697 , 0.927 , 0.000 , 0.922 , 0.697 , 0.927 , 0.000 ] ,
# Charm Gluino R-mesons
        1009413 : [ 1000021 ,  True , '~g_D+       ' ,  1 , 2.000 , 1.954 , 1.863 , 2.078 , 0.000 , 2.073 , 1.863 , 2.078 , 0.000 ] ,
        1009423 : [ 1000021 ,  True , '~g_D0       ' ,  0 , 2.000 , 1.954 , 1.863 , 2.078 , 0.000 , 2.073 , 1.863 , 2.078 , 0.000 ] ,
        1009433 : [ 1000021 ,  True , '~g_Ds+      ' ,  1 , 2.200 , 2.103 , 2.043 , 2.255 , 0.000 , 2.235 , 2.043 , 2.255 , 0.000 ] ,
# Bottom Gluino R-mesons
        1009513 : [ 1000021 ,  True , '~g_B0       ' ,  0 , 2.000 , 5.317 , 5.224 , 5.367 , 0.000 , 5.367 , 5.224 , 5.367 , 0.000 ] ,
        1009523 : [ 1000021 ,  True , '~g_B+       ' ,  1 , 2.000 , 5.317 , 5.224 , 5.367 , 0.000 , 5.367 , 5.224 , 5.367 , 0.000 ] ,
        1009533 : [ 1000021 ,  True , '~g_Bs0      ' ,  0 , 2.200 , 5.468 , 5.421 , 5.541 , 0.000 , 5.532 , 5.421 , 5.541 , 0.000 ] ,
        1009543 : [ 1000021 ,  True , '~g_Bc+      ' ,  1 , 2.200 , 6.630 , 6.550 , 6.710 , 0.000 , 6.700 , 6.550 , 6.710 , 0.000 ] ,
# Light-flavor singlet Gluino R-baryons
        1093214 : [ 1000021 ,  True , '~g_Lambda0  ' ,  0 , 1.150 , 0.562 , 0.280 , 0.715 , 0.000 , 0.715 , 0.280 , 0.715 , 0.000 ] ,
# Charm singlet Gluino R-baryons
        1094214 : [ 1000021 ,  True , '~g_Sigmac*+ ' ,  1 , 2.300 , 1.726 , 1.489 , 1.885 , 0.000 , 1.885 , 1.489 , 1.885 , 0.000 ] ,
        1094314 : [ 1000021 ,  True , '~g_Xic*0    ' ,  0 , 2.300 , 1.878 , 1.688 , 2.050 , 0.000 , 2.050 , 1.688 , 2.050 , 0.000 ] ,
        1094324 : [ 1000021 ,  True , '~g_Xic*+    ' ,  1 , 2.300 , 1.878 , 1.688 , 2.050 , 0.000 , 2.050 , 1.688 , 2.050 , 0.000 ] ,
# Bottom singlet Gluino R-baryons
        1095214 : [ 1000021 ,  True , '~g_Sigmab*0 ' ,  0 , 5.600 , 5.096 , 4.861 , 5.185 , 0.000 , 5.185 , 4.861 , 5.185 , 0.000 ] ,
        1095314 : [ 1000021 ,  True , '~g_Xib*-    ' , -1 , 5.750 , 5.248 , 5.060 , 5.350 , 0.000 , 5.350 , 5.060 , 5.350 , 0.000 ] ,
        1095324 : [ 1000021 ,  True , '~g_Xib*0    ' ,  0 , 5.750 , 5.248 , 5.060 , 5.350 , 0.000 , 5.350 , 5.060 , 5.350 , 0.000 ] ,
# Light flavor Gluino R-baryons
        1091114 : [ 1000021 ,  True , '~g_Delta-   ' , -1 , 0.975 , 0.812 , 0.530 , 0.965 , 0.000 , 0.965 , 0.530 , 0.965 , 0.000 ] ,
        1092114 : [ 1000021 ,  True , '~g_Delta0   ' ,  0 , 0.975 , 0.812 , 0.530 , 0.965 , 0.000 , 0.965 , 0.530 , 0.965 , 0.000 ] ,
        1092214 : [ 1000021 ,  True , '~g_Delta+   ' ,  1 , 0.975 , 0.812 , 0.530 , 0.965 , 0.000 , 0.965 , 0.530 , 0.965 , 0.000 ] ,
        1092224 : [ 1000021 ,  True , '~g_Delta++  ' ,  2 , 0.975 , 0.812 , 0.530 , 0.965 , 0.000 , 0.965 , 0.530 , 0.965 , 0.000 ] ,
# Strange Gluino R-baryons
        1093114 : [ 1000021 ,  True , '~g_Sigma*-  ' , -1 , 1.150 , 1.094 , 0.858 , 1.260 , 0.000 , 1.260 , 0.858 , 1.260 , 0.000 ] ,
        1093224 : [ 1000021 ,  True , '~g_Sigma*+  ' ,  1 , 1.150 , 1.094 , 0.858 , 1.260 , 0.000 , 1.260 , 0.858 , 1.260 , 0.000 ] ,
        1093314 : [ 1000021 ,  True , '~g_Xi*-     ' , -1 , 1.300 , 1.246 , 1.058 , 1.440 , 0.000 , 1.425 , 1.058 , 1.440 , 0.000 ] ,
        1093324 : [ 1000021 ,  True , '~g_Xi*0     ' ,  0 , 1.300 , 1.246 , 1.058 , 1.440 , 0.000 , 1.425 , 1.058 , 1.440 , 0.000 ] ,
        1093334 : [ 1000021 ,  True , '~g_Omega-   ' , -1 , 1.600 , 1.398 , 1.257 , 1.620 , 0.000 , 1.590 , 1.257 , 1.620 , 0.000 ] ,
# Charm Gluino R-baryons
        1094114 : [ 1000021 ,  True , '~g_Sigmac*0 ' ,  0 , 2.300 , 2.258 , 2.068 , 2.430 , 0.000 , 2.430 , 2.068 , 2.430 , 0.000 ] ,
        1094224 : [ 1000021 ,  True , '~g_Sigmac*++' ,  2 , 2.300 , 2.258 , 2.068 , 2.430 , 0.000 , 2.430 , 2.068 , 2.430 , 0.000 ] ,
        1094334 : [ 1000021 ,  True , '~g_Omegac*0 ' ,  0 , 2.300 , 2.562 , 2.466 , 2.790 , 0.000 , 2.760 , 2.466 , 2.790 , 0.000 ] ,
# Bottom Gluino R-baryons
        1095114 : [ 1000021 ,  True , '~g_Sigmab*- ' , -1 , 5.600 , 5.628 , 5.440 , 5.730 , 0.000 , 5.730 , 5.440 , 5.730 , 0.000 ] ,
        1095224 : [ 1000021 ,  True , '~g_Sigmab*+ ' ,  1 , 5.600 , 5.628 , 5.440 , 5.730 , 0.000 , 5.730 , 5.440 , 5.730 , 0.000 ] ,
        1095334 : [ 1000021 ,  True , '~g_Omegab*- ' , -1 , 5.900 , 5.932 , 5.838 , 6.080 , 0.000 , 6.060 , 5.838 , 6.080 , 0.000 ] ,

# Sbottom R-mesons
        1000512 : [ 1000005 , True , '~B0          ' ,  0 , 0.325 , 0.314 , 0.220 , 0.365 , 0.000 , 0.365 , 0.220 , 0.365 , 0.000 ] ,
        1000522 : [ 1000005 , True , '~B-          ' , -1 , 0.325 , 0.314 , 0.220 , 0.365 , 0.000 , 0.365 , 0.220 , 0.365 , 0.000 ] ,
        1000532 : [ 1000005 , True , '~Bs0         ' ,  0 , 0.500 , 0.466 , 0.419 , 0.540 , 0.000 , 0.530 , 0.419 , 0.540 , 0.000 ] ,
        1000542 : [ 1000005 , True , '~Bc-         ' , -1 , 1.500 , 1.630 , 1.550 , 1.710 , 0.000 , 1.700 , 1.550 , 1.710 , 0.000 ] ,
        1000552 : [ 1000005 , True , '~etab0       ' ,  0 , 4.800 , 5.000 , 5.000 , 5.000 , 0.000 , 5.000 , 5.000 , 5.000 , 0.000 ] ,
# Sbottom R-baryons
        1005113 : [ 1000005 , True , '~Sigmab-     ' , -1 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1005211 : [ 1000005 , True , '~Sigmab0     ' ,  0 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.632 , 0.171 , 0.632 , 0.000 ] ,
        1005213 : [ 1000005 , True , '~Sigmab*0    ' ,  0 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1005223 : [ 1000005 , True , '~Sigmab+     ' ,  1 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1005311 : [ 1000005 , True , '~Xib-        ' , -1 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.827 , 0.497 , 0.833 , 0.000 ] ,
        1005313 : [ 1000005 , True , '~Xib*-       ' , -1 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.917 , 0.686 , 0.922 , 0.000 ] ,
        1005321 : [ 1000005 , True , '~Xib0        ' ,  0 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.827 , 0.497 , 0.833 , 0.000 ] ,
        1005323 : [ 1000005 , True , '~Xib*0       ' ,  0 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.917 , 0.686 , 0.922 , 0.000 ] ,
        1005333 : [ 1000005 , True , '~Omegab-     ' , -1 , 1.000 , 0.466 , 0.419 , 0.540 , 0.000 , 1.075 , 0.862 , 1.094 , 0.000 ] ,
# Stop R-mesons
        1000612 : [ 1000006 , True , '~T+          ' ,  1 , 0.325 , 0.314 , 0.220 , 0.365 , 0.000 , 0.365 , 0.220 , 0.365 , 0.000 ] ,
        1000622 : [ 1000006 , True , '~T0          ' ,  0 , 0.325 , 0.314 , 0.220 , 0.365 , 0.000 , 0.365 , 0.220 , 0.365 , 0.000 ] ,
        1000632 : [ 1000006 , True , '~Ts+         ' ,  1 , 0.500 , 0.466 , 0.419 , 0.540 , 0.000 , 0.530 , 0.419 , 0.540 , 0.000 ] ,
        1000642 : [ 1000006 , True , '~Tc0         ' ,  0 , 1.500 , 1.630 , 1.550 , 1.710 , 0.000 , 1.700 , 1.550 , 1.710 , 0.000 ] ,
        1000652 : [ 1000006 , True , '~etat+       ' ,  1 , 4.800 , 5.000 , 5.000 , 5.000 , 0.000 , 5.000 , 5.000 , 5.000 , 0.000 ] ,
# Stop R-baryons
        1006113 : [ 1000006 , True , '~Sigmat0     ' ,  0 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1006211 : [ 1000006 , True , '~Sigmat+     ' ,  1 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.632 , 0.171 , 0.632 , 0.000 ] ,
        1006213 : [ 1000006 , True , '~Sigmat*+    ' ,  1 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1006223 : [ 1000006 , True , '~Sigmat++    ' ,  2 , 0.650 , 0.314 , 0.220 , 0.365 , 0.000 , 0.762 , 0.529 , 0.762 , 0.000 ] ,
        1006311 : [ 1000006 , True , '~Xit0        ' ,  0 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.827 , 0.497 , 0.833 , 0.000 ] ,
        1006313 : [ 1000006 , True , '~Xit*0       ' ,  0 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.917 , 0.686 , 0.922 , 0.000 ] ,
        1006321 : [ 1000006 , True , '~Xit+        ' ,  1 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.827 , 0.497 , 0.833 , 0.000 ] ,
        1006323 : [ 1000006 , True , '~Xit*+       ' ,  1 , 0.825 , 0.314 , 0.220 , 0.365 , 0.000 , 0.917 , 0.686 , 0.922 , 0.000 ] ,
        1006333 : [ 1000006 , True , '~Omegat0     ' ,  0 , 1.000 , 0.466 , 0.419 , 0.540 , 0.000 , 1.075 , 0.862 , 1.094 , 0.000 ] ,
   }


def charge( c ):
    """ Function to return a PDG table formatted charge given an integer charge
        Input is the charge either as a string or number
    """
    n = int(c)
    if n==0: return ' 0'
    if n==1: return ' +'
    if n==2: return '++'
    if n==-1: return ' -'
    if n==-2: return '--'
    raise RuntimeError('Unexpected charge: '+str(n))


def get_quarks( x ):
    """ Function to return a list of quarks in a hadron
    """
    # For mesons, just two quarks
    if '00' in str(x): return str(x)[4:5]
    # For baryons, three quarks
    return str(x)[3:5]


def is_baryon( x ):
    if '009' in str(x): return 0 # gluino meson
    elif '09' in str(x): return 1 # gluino baryon
    elif '006' in str(x): return 1 # stop baryon
    elif '005' in str(x): return 1 # sbottom baryon
    # Otherwise it's a meson
    return 0


def anti_name( x ):
    """ Function to turn a particle name into an anti-particle name (mostly charge issues)
    """
    # These look a little funny to get both + and ++
    if '*+' in x: return x.replace('*','bar*').replace('+','-')
    if '*-' in x: return x.replace('*','bar*').replace('-','+')
    if '++' in x: return x.replace('++','bar--')
    if '--' in x: return x.replace('--','bar++')
    if '+' in x:  return x.replace('+','bar-')
    if '-' in x:  return x.replace('-','bar+')
    if '*0' in x: return x.replace('*0','bar*0')
    return x.replace('0','bar0')


# Now programmatically calculate the missing spectra
for pid in offset_options:
    # Skip fundamental SUSY particles and R-glueball
    if offset_options[pid][0] == 0 or pid == 1000993: continue
    # Setup #4 to be mass set #1 but with 1009113 matching mass set 5
    offset_options[pid][first_mass_set+4] = offset_options[pid][first_mass_set+1]+0.088
    # Setup #8 to be mass set #5 but with 1009113 matches mass set 2
    offset_options[pid][first_mass_set+8] = offset_options[pid][first_mass_set+5]-0.088


def get_gluino_Rhadron_masses(input_file, mass_spectrum=1):
    """ Function to return a dictionary of PDG IDs and masses based on an input param/SLHA/LHE file
        First parameter: input file (string or file handle)
        Second parameter: mass spectrum (enumeration value)
    """
    # Expect a string file name or file handle
    if type(input_file) not in [str,file]:
        raise RuntimeError('Incorrect input file type: '+str(type(input_file)))
    in_file = input_file if type(input_file)==file else open(input_file,'r')

    # Expect SLHA file format.  Read for mass block, then look for relevant masses, then exit
    masses = {}
    mass_block = False
    for l in in_file.readlines():
        # Are we entering the mass block?
        if 'BLOCK' in l.upper().split('#')[0].split() and 'MASS' in l.upper().split('#')[0].split():
            mass_block = True
            continue
        # Otherwise, if we've not yet entered the mass block, keep reading
        elif not mass_block: continue
        # If we're past the mass block, then stop reading
        if 'BLOCK' in l.upper().split('#')[0].split(): break
        # Skip empty lines, comments, etc
        if len(l.split('#')[0].split())<2: continue
        pdg_id = int(l.split('#')[0].split()[0])
        # Set the input masses
        if pdg_id in offset_options:
            mass = float(l.split('#')[0].split()[1])
            # If we have decoupled the thing, don't include it!
            if mass > 7e3: continue
            # Otherwise, it goes in the list
            masses[pdg_id] = mass
        # Not an ID we care about otherwise!  Skip!
    # Done reading file; close if it's our responsibility
    if type(input_file)==str: in_file.close()

    # Set the remainder of the masses
    had_rhadron=False
    for pid in offset_options:
        # Skip fundamental particles - they should be read from the input file!
        if offset_options[pid][0] == 0: continue
        # Check if the constituent is in there (e.g. skip stop R-hadrons for gluino production)
        if offset_options[pid][0] not in masses: continue
        # Check if the mass spectrum is available
        if mass_spectrum<0 or first_mass_set+mass_spectrum>len(offset_options[pid]):
            raise RuntimeError("Unknown mass set requested: "+str(mass_spectrum)+" > number of options ("+str(len(offset_options[pid])-first_mass_set+1)+") for PID "+str(pid))
        # Add 'normal' R-hadron
        masses[pid] = masses[ offset_options[pid][0] ] + offset_options[pid][first_mass_set+mass_spectrum]
        # If needed, add anti-R-hadron
        if offset_options[pid][1]:
            masses[-pid] = masses[ offset_options[pid][0] ] + offset_options[pid][first_mass_set+mass_spectrum]
        had_rhadron = True

    # Make sure we generated some R-hadrons
    if not had_rhadron:
        raise RuntimeError('No R-hadrons generated!')

    # Return the dictionary
    return masses


def update_PDG_table(input_file, pdg_table, mass_spectrum=1):
    """ Function to update a PDG table with R-hadron masses
        First input parameter: input file (string or file handle)
        Second input parameter: output PDG table (string or file handle)
        Third input parameter: mass spectrum (enumeration value)
        Gets R-hadron masses based on get_gluino_Rhadron_masses()
    """
    # Check that we had the right output file type
    if type(pdg_table) not in [str,file]:
        raise RuntimeError('Incorrect output file type: '+str(type(pdg_table)))
    # Get the masses that we need
    masses = get_gluino_Rhadron_masses(input_file,mass_spectrum)
    # Get the output file ready
    # Open for appending (assume that's what was done if given a file handle)
    out_file = pdg_table if type(pdg_table)==file else open(pdg_table,'a')
    # Add all our R-hadrons to the table!
    for pid in masses:
        # For the PDG table, we only write positive-signed PDG ID particles
        if pid<0: continue
        # Note that we follow the Pythia6 convention of *including* fundamental SUSY particles
        # The format is VERY specific; needs mass and width (we always set the width to 0)
        # Mass is in MeV here, rather than GeV as in the dictionary
        out_file.write('\nM %i                          %11.5E    +0.0E+00 -0.0E+00 %s       %s'%(pid,masses[pid]*1000.,offset_options[pid][2],charge(offset_options[pid][3])))
        out_file.write('\nW %i                          %11.5E    +0.0E+00 -0.0E+00 %s       %s'%(pid,0.E+00,offset_options[pid][2],charge(offset_options[pid][3])))

    # Done writing all the lines!  Clean up if necessary
    if type(pdg_table) is str: out_file.close()

    # Nothing to return


def update_particle_table(input_file, particle_table='particles.txt', mass_spectrum=1):
    """ Function to update a particle table with R-hadron masses
        First input parameter: input file (string or file handle)
        Second input parameter: output particle table (string or file handle)
        Third input parameter: mass spectrum (enumeration value)
        Gets R-hadron masses based on get_gluino_Rhadron_masses()
    """
    # Check that we had the right output file type
    if type(particle_table) not in [str,file]:
        raise RuntimeError('Incorrect output file type: '+str(type(particle_table)))
    # Get the masses that we need
    masses = get_gluino_Rhadron_masses(input_file,mass_spectrum)
    # Get the output file ready
    # Open for appending (assume that's what was done if given a file handle)
    out_file = particle_table if type(particle_table)==file else open(particle_table,'a')
    # Add all our R-hadrons to the table!
    # Note that we MUST write the primary first, followed by the compound particles
    primaries = []
    extras = []
    for pid in masses:
        if offset_options[abs(pid)][0]==0: extras += [pid]
        elif not offset_options[abs(pid)][0] in primaries: primaries += [ offset_options[abs(pid)][0] ]

    # Rounds per primary
    for p in primaries:
        # Note that we follow the old convention of *including* fundamental SUSY particles
        # The format is VERY specific; needs mass and width (we always set the width to 0)
        # Mass is in MeV here, rather than GeV as in the dictionary
        if p>0: out_file.write('     %i  %04.3f   # %s\n'%(p,masses[p],offset_options[abs(p)][2]))
        # For the anti-particle, also need the anti-name
        else:     out_file.write('    %i  %04.3f   # %s\n'%(p,masses[p],anti_name(offset_options[abs(p)][2])))
        # Now include the secondaries
        for pid in masses:
            if offset_options[abs(pid)][0]!=p: continue
            # Note that we follow the old convention of *including* fundamental SUSY particles
            # The format is VERY specific; needs mass and width (we always set the width to 0)
            # Mass is in MeV here, rather than GeV as in the dictionary
            if pid>0: out_file.write('     %i  %04.3f   # %s\n'%(pid,masses[pid],offset_options[abs(pid)][2]))
            # For the anti-particle, also need the anti-name
            else:     out_file.write('    %i  %04.3f   # %s\n'%(pid,masses[pid],anti_name(offset_options[abs(pid)][2])))
        # Done with secondaries for this primary

    for p in extras:
        if p in primaries: continue
        if p>0: out_file.write('     %i  %04.3f   # %s\n'%(p,masses[p],offset_options[abs(p)][2]))
        # For the anti-particle, also need the anti-name
        else:   out_file.write('    %i  %04.3f   # %s\n'%(p,masses[p],anti_name(offset_options[abs(p)][2])))

    # Done writing all the lines!  Clean up if necessary
    if type(particle_table) is str: out_file.close()

    # Nothing to return


def get_Pythia8_commands(input_file, mass_spectrum=1):
    """ Function to return a list of Pythia8 commands to set up an R-hadron mass spectrum.
        First input parameter: input file (string or file handle)
        Second input parameter: mass spectrum (enumeration value)
    """
    # Get the masses for this configuration
    masses = get_gluino_Rhadron_masses(input_file,mass_spectrum)
    # Tell Pythia8 we are going to use our own masses
    commands = ['RHadrons:setMasses = off']
    
    # Add commands to set all the masses
    for pid in masses:
        # Only set masses for particles (not anti-particles)
        if pid<0: continue
        # Actual command takes the form PDGID:m0 = somemass
        commands += [ str(pid)+':m0 = '+str(masses[pid]) ]

    # All done!
    return commands


def get_interaction_list(input_file, interaction_file='ProcessList.txt', mass_spectrum=1):
    """ Function to write all possible interactiosn that we need
        First input parameter: input file (string or file handle)
        Second input parameter: output PDG table (string or file handle)
        Third input parameter: mass spectrum (enumeration value)
        Gets R-hadron masses based on get_gluino_Rhadron_masses()
    """
    # Check that we had the right output file type
    if type(interaction_file) not in [str,file]:
        raise RuntimeError('Incorrect output file type: '+str(type(interaction_file)))
    # Get the masses that we need. Note that we don't really need masses, just PDG IDs
    masses = get_gluino_Rhadron_masses(input_file,mass_spectrum)
    # Get the output file ready
    # Open for appending (assume that's what was done if given a file handle)
    out_file = interaction_file if type(interaction_file)==file else open(interaction_file,'a')

    # Helpful lists to move us along
    sm_particles = {
      # Name  :  Charge , Baryon # , Strangeness
        'pi0' : [  0 , 0 ,  0 ],
        'pi+' : [  1 , 0 ,  0 ],
        'pi-' : [ -1 , 0 ,  0 ],
    'neutron' : [  0 , 1 ,  0 ],
     'proton' : [  1 , 1 ,  0 ],
      'kaon0' : [  0 , 0 ,  1 ],
 'anti_kaon0' : [  0 , 0 , -1 ],
      'kaon+' : [  1 , 0 ,  1 ],
      'kaon-' : [ -1 , 0 , -1 ]
                   }
    targets = [ 'proton' , 'neutron' ]

    incoming_rhadrons = {}
    outgoing_rhadrons = {}
    for pid in masses:
        # Only for bound states
        if offset_options[abs(pid)][0]==0: continue
        # All of them are on the list of incoming RHadrons
        # Deal with strangeness
        # Approximation! Bottom number -> -Charm number -> Strangeness
        # Approximation needed because I don't know how outgoing SM charms are treated in G4 at the moment
        s_number = 0
        if '3' in get_quarks(pid) or '4' in get_quarks(pid) or '5' in get_quarks(pid):
            if len(get_quarks(pid))>2:
                s_number = -(get_quarks(pid).count('3')+get_quarks(pid).count('4')+get_quarks(pid).count('5')) if pid>0 else get_quarks(pid).count('3')+get_quarks(pid).count('4')+get_quarks(pid).count('5')
            else:
                if get_quarks(pid).count('3') + get_quarks(pid).count('4') + get_quarks(pid).count('5')>1: s_number=0
                elif offset_options[abs(pid)][3]==0 and ('3' in get_quarks(pid) or '5' in get_quarks(pid)): s_number=1 if pid>0 else -1
                elif offset_options[abs(pid)][3]==0 and '4' in get_quarks(pid): s_number=1 if pid<0 else -1
                elif '3' in get_quarks(pid) or '5' in get_quarks(pid): s_number=offset_options[abs(pid)][3]
                elif '4' in get_quarks(pid): s_number=-offset_options[abs(pid)][3]
        else: s_number=0
        # Build the dictionary
        pid_name = offset_options[pid][2].strip() if pid>0 else anti_name(offset_options[abs(pid)][2]).strip()
        incoming_rhadrons[pid_name] = [ offset_options[abs(pid)][3] , is_baryon(pid) , s_number ]
        # Smaller list of outgoing rhadrons.
        # No charm or bottom
        if '4' in get_quarks(pid) or '5' in get_quarks(pid): continue
        outgoing_rhadrons[pid_name] = [ offset_options[abs(pid)][3] , is_baryon(pid) , s_number ]

    # Add all our R-hadrons to the table
    for proj in incoming_rhadrons:
        # Loop over targets
        for t in targets:
            # Loop over possible outgoing R-hadrons
            for orhad in outgoing_rhadrons:
                # Possible 2>2 reactions
                for osm1 in sm_particles:
                    # Check for charge conservation
                    total_charge = incoming_rhadrons[proj][0]+sm_particles[t][0]-outgoing_rhadrons[orhad][0]-sm_particles[osm1][0]
                    # Check for baryon number conservation
                    total_bnumber = incoming_rhadrons[proj][1]+sm_particles[t][1]-outgoing_rhadrons[orhad][1]-sm_particles[osm1][1]
                    # Check for strangeness conservation
                    total_snumber = incoming_rhadrons[proj][2]+sm_particles[t][2]-outgoing_rhadrons[orhad][2]-sm_particles[osm1][2]
                    # Check if it's an allowed reaction
                    if total_charge==0 and total_bnumber==0 and total_snumber==0:
                        out_file.write( ' # '.join([str(proj),str(t),str(orhad),str(osm1)])+'\n' )
                    # Now loop over possible 2>3 reactions
                    for osm2 in sm_particles:
                        # Check for charge conservation
                        total_charge = incoming_rhadrons[proj][0]+sm_particles[t][0]-outgoing_rhadrons[orhad][0]-sm_particles[osm1][0]-sm_particles[osm2][0]
                        # Check for baryon number conservation
                        total_bnumber = incoming_rhadrons[proj][1]+sm_particles[t][1]-outgoing_rhadrons[orhad][1]-sm_particles[osm1][1]-sm_particles[osm2][1]
                        # Check for strangeness conservation
                        total_snumber = incoming_rhadrons[proj][2]+sm_particles[t][2]-outgoing_rhadrons[orhad][2]-sm_particles[osm1][2]-sm_particles[osm2][2]
                        # Check if it's an allowed reaction
                        if total_charge==0 and total_bnumber==0 and total_snumber==0:
                            out_file.write( ' # '.join([str(proj),str(t),str(orhad),str(osm1),str(osm2)])+'\n' )
                        # Wrote out the reaction
                    # Loop over 2>3
                # Loop over 2>2
            # Loop over outgoing RHadrons
        # Loop over targets
    # Loop over projectiles

    # Done writing all the lines!  Clean up if necessary
    if type(interaction_file) is str: out_file.close()

    # Nothing to return


def print_masses(spectrum=-1):
    """ Print the mass spectra.
    Input parameter: spectrum number.  If -1, print all spectra.
    """
    for i in sorted(offset_options.keys()):
        s= str(offset_options[i][2])+' '+str(i)
        if spectrum<0:
            for j in xrange(first_mass_set,len(offset_options[i])): s+=' '+str(offset_options[i][j])
        else:
            if first_mass_set+spectrum>len(offset_options[i]):
                raise RuntimeError('Spectrum #'+str(spectrum)+' not known for PID '+str(i))
            else:
                s+=' '+str(offset_options[i][spectrum+first_mass_set])
        print s
    # Done!
