#!/usr/bin/env python

# art-description: art job for mu_Zmumu_pu40_grid_mt
# art-type: grid
# art-include: master/Athena
# art-input: mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO.e3601_s2576_s2132_r7143
# art-input-nfiles: 4
# art-athena-mt: 4
# art-memory: 4096
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.check*
# art-output: HLT*
# art-output: times*
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


Slices  = ['muon']
RunEF   = True
Events  = 8000 
Threads = 4 
Slots   = 4 # what about the mt: 4 art directive ? nfiles: 3 ?
Input   = 'Zmumu_pu40'    # defined in TrigValTools/share/TrigValInputs.json
GridFiles=True

Args = " -p 13 "
TrackReference = [ 'Truth', 'Offline' ]
Lowpt          = [ False, True ] 

from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
