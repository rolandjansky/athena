infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #EventInfo_p4
    'McEventInfo',

    #EventStreamInfo_p2
    'InputMetaDataStore/StreamAOD',
         ]

include ('EventAthenaPool/EventAthenaPoolTestDump.py')
include ('AthenaPoolUtilities/TPCnvTest.py')
