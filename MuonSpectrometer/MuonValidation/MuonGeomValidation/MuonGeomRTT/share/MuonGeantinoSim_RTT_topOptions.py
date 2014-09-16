############################################################
#
# MuonHitRelocation_RTT_topOptions.py
#
############################################################

DetDescrVersion = "ATLAS-GEO-10-00-01"
GlobalCondTag = "OFLCOND-DR-BS900-A1-01" 

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
