infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #EventBookkeeperCollection_p2
    'InputMetaDataStore/IncompleteEventBookkeepers',
    'InputMetaDataStore/EventBookkeepers@4',

    #SkimDecisionCollection_p1
    'StreamESD_SkimDecisionsContainer',
         ]

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

