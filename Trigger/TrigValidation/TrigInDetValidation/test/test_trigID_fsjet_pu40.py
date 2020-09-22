#!/usr/bin/env python

# art-description: art job for fsjet_pu40
# art-type: grid
# art-include: master/Athena
# art-input: valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315
# art-input-nfiles: 3
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


Slices  = ['fsjet']
RunEF   = False
Events  = 2000 
Threads = 1 
Slots   = 1 # what about the mt: 4 art directive ? nfiles: 3 ?
Input   = 'ttbar'    # defined in TrigValTools/share/TrigValInputs.json  

TrackReference = [ 'Truth', 'Offline' ]


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
