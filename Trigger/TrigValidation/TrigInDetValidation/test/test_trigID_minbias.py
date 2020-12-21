#!/usr/bin/env python


# art-description: art job for minbias
# art-type: grid
# art-include: master/Athena
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
# art-athena-nfiles: 4
# art-athena-mt: 8
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
Events  = 8000 
Threads = 8 
Slots   = 8
Input   = 'minbias'    # defined in TrigValTools/share/TrigValInputs.json  

Jobs = [ ( "Truth",       " TIDAdata-run3.dat                    -o data-hists.root" ),
         ( "Offline",     " TIDAdata-run3-offline.dat -r Offline -o data-hists-offline.root" ) ]

Comp = [ ( "L2minbias",        "L2minbias", "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTL2-plots " ),
         ( "L2minbiasoffline", "L2minbias", "data-hists-offline.root", " -c TIDAhisto-panel.dat  -d HLTL2-plots-offline " ) ]


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
