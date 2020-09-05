#!/usr/bin/env python


# art-description: art job for minbias_mt
# art-type: grid
# art-include: master/Athena
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
# art-athena-mt: 4
# art-memory: 4096
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


Slices  = ['minbias']
RunEF   = False
Events  = 8000 
Threads = 4 
Slots   = 4 # what about the mt: 4 art directive ? nfiles: 3 ?
Input   = 'minbias'    # defined in TrigValTools/share/TrigValInputs.json  

TrackReference = 'Truth'


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
