#!/usr/bin/env python

# art-description: art job for mu_Zmumu_pu40
# art-type: grid
# art-include: master/Athena
# art-input-nfiles: 4
# art-athena-mt: 8
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


Slices  = ['L2muonLRT']
Events  = 8000 
Threads = 8 
Slots   = 8
Input   = 'StauStau'    # defined in TrigValTools/share/TrigValInputs.json
GridFiles = False
Release = "current"

ExtraAna = ' -c LRT="True" '

preinclude_file = 'TrigInDetValidation/TIDAsiSPlrt_preinclude.py'


Jobs = [ ( "Truth",  " TIDAdata-run3-lrt.dat -o data-hists.root -p 13", "Test_bin_lrt.dat" ),
         ( "Offline",    " TIDAdata-run3-offline-lrt.dat -r Offline -o data-hists-offline.root", "Test_bin_lrt.dat" ) ]

Comp = [ ( "L2muonLRT",  "L2muonLRT",  "data-hists.root",  " -c TIDAhisto-panel.dat -d HLTL2-plots -sx Reference Truth " ),
         ( "EFmuonLRT",  "EFmuonLRT", "data-hists.root",   " -c TIDAhisto-panel.dat -d HLTEF-plots -sx Reference Truth   " ),
         ( "L2muonLRToffline",   "L2muonLRT","data-hists-offline.root",   " -c TIDAhisto-panel.dat -d HLTL2-plots-offline -sx Reference Offline " ),
         ( "EFmuonLRToffline",   "EFmuonLRT", "data-hists-offline.root",   " -c TIDAhisto-panel.dat -d HLTEF-plots-offline -sx Reference Offline " )
       ]


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")
