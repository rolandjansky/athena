#!/usr/bin/env python

# art-description: art job for tau_ztautau_pu46
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
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


Slices  = ['tauLRT']
Events  = 5000
Threads = 8
Slots   = 8
Release = "current"
Input   = 'StauStau'    # defined in TrigValTools/share/TrigValInputs.json

ExtraAna = " -c 'LRT=True;parentpdgid=15' "

preinclude_file = 'all:TrigInDetValidation/TIDAlrt_preinclude.py'

Jobs = [ ( "Offline",  " TIDAdata-run3-offline-lrt.dat -r Offline -o data-hists-offline-lrt.root", "Test_bin_lrt.dat" ),
         ( "Truth",    " TIDAdata-run3-lrt.dat                    -o data-hists-lrt.root",         "Test_bin_lrt.dat" ) ]


Comp = [ ( "L2tau",        "L2tau",       "data-hists-lrt.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-lrt " ),
         ( "EFtau",        "EFtau",       "data-hists-lrt.root",          " -c TIDAhisto-panel.dat  -d HLTEF-plots-lrt " ),
         ( "L2tauLRT",     "L2tau",       "data-hists-lrt.root",          " -c TIDAhisto-panel.dat  -d HLTL2-plots-lrt " ),
         ( "L2tauOff",     "L2tau",       "data-hists-offline-lrt.root",  " -c TIDAhisto-panel.dat  -d HLTL2-plots-offline-lrt " ),
         ( "EFtauOff",     "EFtau",       "data-hists-offline-lrt.root",  " -c TIDAhisto-panel.dat  -d HLTEF-plots-offline-lrt " ),
         ( "L2tauLRTOff",  "L2tau",       "data-hists-offline-lrt.root",  " -c TIDAhisto-panel.dat  -d HLTL2-plots-offline-lrt " ),
         ( "EFvtx",        "EFtauvtx",    "data-hists-offline-lrt.root",  " -c TIDAhisto-panel-vtx.dat  -d HLTL2-plots-vtx  --ncols 3" ) ]


from AthenaCommon.Include import include
include("TrigInDetValidation/TrigInDetValidation_Base.py")

