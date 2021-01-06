#!/usr/bin/env python

# art-description: art job for mu_Zmumu_pu40_build
# art-type: build
# art-include: master/Athena


Slices  = ['muon']
Events  = 100 
Threads = 1 
Slots   = 1
Input   = 'Zmumu_pu40'    # defined in TrigValTools/share/TrigValInputs.json
Art_type= 'build'

Jobs = [ ( "Truth",       " TIDAdata-run3.dat                    -o data-hists.root -p 13" ) ]

Comp = [ ( "L2muon",              "L2muon",      "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTL2-plots " ),
         ( "EFmuon",              "EFmuon",      "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTEF-plots " ) ]


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
