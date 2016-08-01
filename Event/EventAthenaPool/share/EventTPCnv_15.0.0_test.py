infile = 'aod/AOD-15.0.0/AOD-15.0.0-full.pool.root'
keys = [
    #EventInfo_p2
    'McEventInfo',

    #EventStreamInfo_p1
    'InputMetaDataStore/StreamAOD',
         ]

include ('EventAthenaPool/EventAthenaPoolTestDump.py')
include ('AthenaPoolUtilities/TPCnvTest.py')
