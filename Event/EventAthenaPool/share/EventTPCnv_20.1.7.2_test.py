infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #EventStreamInfo_p3
    'InputMetaDataStore/StreamESD',

    #PileUpEventInfo_p5
    'OverlayEvent',
         ]

include ('EventAthenaPool/EventAthenaPoolTestDump.py')
include ('AthenaPoolUtilities/TPCnvTest.py')
