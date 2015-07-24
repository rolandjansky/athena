#--------------------------------------------------------------
# Powheg ggH_quark_mass_effects setup
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_ggF_H_Common.py')

# Set Powheg variables, overriding defaults
# Note: width_H will be overwritten in case of CPS.
PowhegConfig.mass_H  = 125.
PowhegConfig.width_H = 0.00407

# Turn on the heavy quark effect
PowhegConfig.use_massive_b = True
PowhegConfig.use_massive_c = True

# Complex pole scheme or not (1 for NWA and 3(CPS) for SM)
PowhegConfig.bwshape = 3

# Dynamical scale (sqrt(pT(H)^2+mH^2) real emission contributions)
# Note: r2330 does not support this option. please use newer versions.
PowhegConfig.runningscale = 2

# EW correction
if PowhegConfig.mass_H <= 1000.:
  PowhegConfig.ew = 1
else:
  PowhegConfig.ew = 0

# Set scaling and masswindow parameters
hfact_scale    = 1.2
masswindow_max = 30.

# Calculate an appropriate masswindow and hfact
masswindow = masswindow_max
if PowhegConfig.mass_H <= 700.:  
  masswindow = min( (1799.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
else:
  masswindow = min( (1999.9 - PowhegConfig.mass_H) / PowhegConfig.width_H, masswindow )
PowhegConfig.masswindow = masswindow
PowhegConfig.hfact = PowhegConfig.mass_H / hfact_scale

# Increase number of events requested to compensate for potential Pythia losses
PowhegConfig.nEvents *= 1.02

PowhegConfig.generateRunCard()
PowhegConfig.generateEvents()

#--------------------------------------------------------------
# configure Prophecy
#--------------------------------------------------------------
include('Prophecy4fControl/ProphecyPowhegCommon.py')

#ProphecyConfig.gfermi = PowhegConfig.gfermi
ProphecyConfig.mass_t = PowhegConfig.tmass
ProphecyConfig.mass_b = PowhegConfig.mass_b
ProphecyConfig.mass_c = PowhegConfig.mass_c
ProphecyConfig.mass_H = PowhegConfig.mass_H
ProphecyConfig.nEvents = PowhegConfig.nEvents
ProphecyConfig.channel = "e anti-e mu anti-mu"

ProphecyConfig.generateRunCard()
ProphecyConfig.generateEvents()

#--------------------------------------------------------------
# configure ProphecyPowhegMerger
#--------------------------------------------------------------

include('Prophecy4fControl/ProphecyPowhegMergeCommon.py')

ProphecyPowhegMerger.input_powhegLHE = PowhegConfig.output_events_file_name
ProphecyPowhegMerger.input_prophecyLHE = ProphecyConfig.output_events_file_name

ProphecyPowhegMerger.merge()

#--------------------------------------------------------------
# final state randomizer
#--------------------------------------------------------------

import shutil, random

fname_in = ProphecyPowhegMerger.output_events_file_name
fname_tmp = fname_in+".tmp"

id_replace_one = '11'
id_replace_two = '13'
id_list_one = ['11', '13', '15']
id_list_two = ['11', '13', '15']

file_out = open(fname_tmp, 'w')

counter = 1
id_random_one = id_replace_one
id_random_two = id_replace_two
# PDG IDs end in Prophecy4f LHE files at position 9
for line in open(fname_in, 'r') :
  if ( line.find(id_replace_one, 0, 9) != -1 or line.find(id_replace_two, 0, 9) != -1 ) and line.find("<", 0, 9) == -1 :
    keep = line[9:]
    change = line[:9]
    if counter == 1 :
      id_random_one = random.choice(id_list_one)
      id_list_two = ['11', '13', '15']
      id_list_two.remove(id_random_one)
      id_random_two = random.choice(id_list_two)
      counter = 4
    else :
      counter -= 1
    modline = ""
    if line.find(id_replace_one, 0, 9) != -1 :
      modline = string.replace(change, id_replace_one, id_random_one) + keep
    else :
      modline = string.replace(change, id_replace_two, id_random_two) + keep
    file_out.write(modline)
  else :
    file_out.write(line)

file_out.close()

shutil.move(fname_tmp, fname_in)

#--------------------------------------------------------------
# Pythia8 showering
# note: Main31 is set in Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_AZNLO_CTEQ6L1_EvtGen_Common.py')

#--------------------------------------------------------------
# Pythia8 main31 update
#--------------------------------------------------------------
genSeq.Pythia8.UserModes += [ 'Main31:NFinal = 1' ]

#--------------------------------------------------------------
# Higgs at Pythia8
#--------------------------------------------------------------
genSeq.Pythia8.Commands += [ '25:onMode = off',    # decay of Higgs
                             '23:onMode = off' ]    # decay of Z

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = "POWHEG+PROPHECY+PYTHIA8+EVTGEN, ggH H->llll+X mh=125 GeV CPS"
evgenConfig.keywords    = [ "Higgs", "SMHiggs", "ZZ" ]
evgenConfig.contact     = [ 'jochen.meyer@cern.ch' ]

