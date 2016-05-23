# Original author: Goetz Gaycken
# Uploaded by shaun roe, 19 Jan 2016
# The actual reco tf below pre-includes some python snippits 
# which are generated in the first few lines.


# create a python script which switches off everything which is not strictly
# needed for InDetPhysValMonitoring or the InDetStandardPlots
# It seems that the main PhysVal monitoring will fail if standard objects 
# are not available in 20.1.8.5, so only the trigger can be disabled to speed up
# processing
cat - > switch_off_non_id.py <<EOF
# print rec
# rec.doCalo = False
# rec.doEgamma = False
# rec.doForwardDet = False
# rec.doInDet = True
# rec.doJetMissingETTag = False
# rec.doLArg = False
# rec.doLucid = False
# rec.doMuon = False
# rec.doMuonCombined = False
# rec.doRecoTiming = True
# rec.doSemiDetailedPerfMon = True
# rec.doTau = False
# rec.doTile = False
rec.doTrigger = False
EOF

# switch of track slimming so that residuals can be plotted
# this is needed.
cat - > slimming.py <<EOF
from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doSlimming.set_Value_and_Lock(False)
EOF

# switch on the indet standard plots
# this is needed to get the plots from the InDetPerformanceRTT
cat - > indet_std_plots <<EOF
# from AthenaMonitoring import DQMonFlags;
# DQMonFlags.doStreamAwareMon=False
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doStandardPlots.set_Value_and_Lock(True);
EOF


# run the reco transform
Reco_tf.py   --checkEventCount  False --ignoreErrors True --maxEvents 5 --valid True --inputRDOFile root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366/RDO.197091._000015.pool.root.1  --outputNTUP_PHYSVALFile physval.root  --validationFlags doInDet --outputAODFile physval.AOD.root --preInclude switch_off_non_id.py r2e:slimming.py,indet_std_plots.py


# --checkEventCount  False  --ignoreErrors True   : to prevent the Reco_tf to abort after a stage due to errors showing up in the log file.
# --maxEvents 5 :       to only run on 5 events
# --inputRDOFile ...  : specify an RDO input file


# --valid True  :                         needed for InDetPhysValMonitoring. Otherwise the decorator be run too late
# --outputNTUP_PHYSVALFile physval.root : request the PhysVal monitoring histograms and set the histogram file name
# --validationFlags doInDet :             request InDet PhysVal monitoring histograms
# --outputAODFile ... :                   [NOT NECESSARY] request an AOD which includes the decorations by the decrorator of the InDetPhysValMonitoring 
# --preInclude r2e:switch_off_non_id.py,slimming.py,indet_std_plots.py e2a:switch_off_non_id.py :
#     run these python scripts before processing the normal job options of a certain processing staage 
#     r2e: after this label the python scripts which are to be run at the very beginning of the RAWtoESD stage (or RDO to ESD)
#     a2e: after this label the python scripts which are to be run at the very beginning of the ESDtoAOD stage.



