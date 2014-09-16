
#MuonGeantinoSim
athena -c 'EvtMax=10000; ConditionsTag="OFLCOND-DR-BS900-A1-01";DetDescrVersion="ATLAS-GEO-16-00-01"' MuonGeomRTT/MuonGeantinoSim_RTT_topOptions.py
mv MuonHits.root myGeantinoHits_misal.pool.root
athena -c 'ConditionsTag="OFLCOND-DR-BS900-A1-01";DetDescrVersion="ATLAS-GEO-16-00-01"' MuonGeomRTT/MuonHitRelocation_RTT_topOptions.py
root -l -b <RelocatedMuonHits_Plot.C 
#MuonGMCheckReference
athena -c 'DetDescrVersion="ATLAS-GEO-16-00-01"' MuonGeomRTT/MuonGMCheck_RTT_topOptions.py
#
athena -c 'DetDescrVersion="ATLAS-GEO-18-00-00"' MuonGeomRTT/MuonGMCheck_RTT_topOptions.py
#
# Amdc MGM 
athena -c 'DetDescrVersion="ATLAS-GEO-16-00-01"' MuonGeomRTT/AmdcMGM_RTT_topOptions.py
#
athena -c 'useAlignData=True;DetDescrVersion="ATLAS-GEO-18-00-00"' MuonGeomRTT/AmdcMGM_RTT_topOptions.py
#


