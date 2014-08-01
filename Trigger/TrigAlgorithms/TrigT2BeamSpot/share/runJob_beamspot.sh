#!/bin/bash

echo
echo "Running single job"
echo

##--------------
## Seed choices
##--------------
# beamspot_vertex_list
# beamspot_vertex_jet10 
# beamspot_vertex_bjet18
# beamspot_vertex_FSTracks
# beamspot_vertex_tau16i_loose
# beamspot_vertex_e10_medium
# beamspot_vertex_e10_medium_SiTrk 
# beamspot_vertex_e10_medium_SiTrk_robust
# beamspot_vertex_trk9i_calib


mydate=`date '+%Y-%m-%d-%HH-%MM-%SS'`
evtMax=1000
datatype=EnhancedBias
L2Seed=beamspot_vertex_FSTracks
trktype=IdScan
cutstype=${mydate}
jobOptions=myNewTrigger_jobOptions.py
logDir=/u1/fizisist/beamspot
Output=DEBUG

athena.py -c "EvtMax=${evtMax};OutputLevel=${Output};Data='${datatype}';Seed='${L2Seed}';TrackType='${trktype}';SiTrackOpt='${cutstype}';" ${jobOptions} >& ${logDir}/beamspot.${datatype}.${evtMax}evt.${trktype}.${L2Seed}.${cutstype}.${mydate}.log & 

#mydate=`date '+%Y-%m-%d-%HH-%MM-%SS'`
#athenaMT.py -M -W -c isOnline=True -l INFO -f Raw.data -n 10000 testHLT_standalone.py >& /u1/fizisist/beamspot/testMT.raw.withOH.${mydate}.log &


