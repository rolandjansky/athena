#!/usr/bin/env python

# art-description: art job for all_ttbar_tier0_pu40
# art-type: grid
# art-include: master/Athena
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


Slices  = ['electron-tnp']
Events  = 4000
Threads = 8 
Slots   = 8
Release = "current"

ExtraAna = " -c doTIDATier0=True "

Input   = 'Zee_pu40'    # defined in TrigValTools/share/TrigValInputs.json  
preinclude_file = 'all:TrigInDetValidation/TIDV_cond_fix.py' #conditions fix for ATR-23982. In future find a more recent RDO 

Jobs = [] 

Comp = [ ( "L2electron",   "L2electronTier0TnP",  "data-hists-tier0.root", " -b HLT/TRIDT/Egamma/Expert -s '_HLT_IDTrack' '/HLT_IDTrack' -c TIDAhisto-tier0-TnP.dat -l e26_e14_etcut_idperf_50invmAB130_FTF_FE_el1_probe_1 e26_e14_etcut_idperf_gsf_50invmAB130_FTF_FE_el2_probe_1 --ncols 3 -d HLTL2-plots-electron " ),
         ( "EFelectron",   "EFelectronTier0TnP",  "data-hists-tier0.root", " -b HLT/TRIDT/Egamma/Expert -s '_HLT_IDTrack' '/HLT_IDTrack' -c TIDAhisto-tier0-TnP.dat -l e26_e14_etcut_idperf_50invmAB130_FTF_FE_el1_probe_1 e26_e14_etcut_idperf_50invmAB130_IDTrig_FE_el1_probe_1 e26_e14_etcut_idperf_gsf_50invmAB130_IDTrig_FE_el2_probe_1  --ncols 3 -d HLTEF-plots-electron " ) ]
   
from AthenaCommon.Include import include 
include("TrigInDetValidation/TrigInDetValidation_Base.py")

