############################################################
#
# MuonHitRelocation_RTT_topOptions.py
#
############################################################

DetDescrVersion = "ATLAS-R2-2015-02-01-00"
GlobalCondTag = "OFLCOND-RUN12-SDR-22" 

include( "MuonGeoModelTest/simulGeantinoHits_misal.py" )

#--- AthenaCommon flags ---------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.unlock_JobProperties()

#it looks like the following line is not really able to reset the output filename  
#athenaCommonFlags.PoolHitsOutput='MuonHits.root'

if not 'EvtMax' in dir():
    EvtMax=20000
    print 'Setting now EvtMax to ', EvtMax
else:
    print 'EvtMax already set to ', EvtMax
athenaCommonFlags.EvtMax=EvtMax
