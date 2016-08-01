infile = 'aod/AOD-17.5.0/AOD-17.5.0-full.pool.root'
keys = [
    #EventInfo_p3
    'McEventInfo',

    #EventStreamInfo_p2
    'InputMetaDataStore/StreamAOD',
         ]

include ('EventAthenaPool/EventAthenaPoolTestDump.py')
include ('AthenaPoolUtilities/TPCnvTest.py')
