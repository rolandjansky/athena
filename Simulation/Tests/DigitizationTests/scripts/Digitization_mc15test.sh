#!/bin/sh


# how do I check if have run "voms-proxy-init --voms atlas" and "asetup" 

echo 'Ensure you run the correctr asetup command for your build' 
echo 'You will also need to run voms-proxy-init --voms atlas' 


Digi_tf.py --AMIConfig d1364 \
 --inputHITSFile root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc15a/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._001041.pool.root.1 \
 --inputHighPtMinbiasHitsFile root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc15a/mc15_valid.361035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3581_s2578_s2169_tid05098387_00/HITS.05098387._00067?.pool.root.1 \
 --inputLowPtMinbiasHitsFile root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc15a/mc15_valid.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2169_tid05098374_00/HITS.05098374.*.pool.root.1 \
 --outputRDOFile mc15_2015_ttbar.RDO.pool.root  

diffPoolFiles.py -rroot://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/ReferenceFiles/dev/x86_64-slc6-gcc48-opt/mc15_2015_ttbar_50ns_pileup.RDO.pool.root -f mc15_2015_ttbar.RDO.pool.root 

acmd.py diff-root root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/ReferenceFiles/dev/x86_64-slc6-gcc48-opt/mc15_2015_ttbar_50ns_pileup.RDO.pool.root  mc15_2015_ttbar.RDO.pool.root --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings


