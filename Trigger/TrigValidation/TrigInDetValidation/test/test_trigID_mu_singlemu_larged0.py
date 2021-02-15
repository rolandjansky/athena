#!/usr/bin/env python

# art-description: art job for mu_singlemu_larged0
# art-type: grid
# art-include: master/Athena
# art-input: mc15_13TeV.107237.ParticleGenerator_mu_Pt4to100_vertxy20.recon.RDO.e3603_s2726_r7728
# art-input-nfiles: 10
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


Slices  = ['muon']
Events  = 20000 
Threads = 8 
Slots   = 8
Input   = 'Single_mu_larged0'    # defined in TrigValTools/share/TrigValInputs.json
GridFiles=True

Jobs = [ ( "Truth",       " TIDAdata-run3-larged0.dat                    -o data-hists.root -p 13",   "Test_bin_larged0.dat" ),
         ( "Offline",     " TIDAdata-run3-offline-larged0.dat -r Offline -o data-hists-offline.root", "Test_bin_larged0.dat" ) ]

Comp = [ ( "L2muon",              "L2muon",      "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTL2-plots " ),
         ( "L2muon-lowpt",        "L2muonLowpt", "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTL2-plots-lowpt " ),
         ( "L2muonoffline",       "L2muon",      "data-hists-offline.root", " -c TIDAhisto-panel.dat  -d HLTL2-plots-offline " ),
         ( "L2muonoffline-lowpt", "L2muonLowpt", "data-hists-offline.root", " -c TIDAhisto-panel.dat  -d HLTL2-plots-lowpt-offline " ),
         ( "EFmuon",              "EFmuon",      "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTEF-plots " ),
         ( "EFmuon-lowpt",        "EFmuonLowpt", "data-hists.root",         " -c TIDAhisto-panel.dat  -d HLTEF-plots-lowpt " ),
         ( "EFmuonoffline",       "EFmuon",      "data-hists-offline.root", " -c TIDAhisto-panel.dat  -d HLTEF-plots-offline " ),
         ( "EFmuonoffline-lowpt", "EFmuonLowpt", "data-hists-offline.root", " -c TIDAhisto-panel.dat  -d HLTEF-plots-lowpt-offline " ) ]


from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")


 
