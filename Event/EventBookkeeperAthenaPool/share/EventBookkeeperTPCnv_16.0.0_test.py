infile = 'aod/AOD-16.0.0/AOD-16.0.0-full.pool.root'
keys = [
    #EventBookkeeperCollection_p1
    'InputMetaDataStore/EventBookkeepers@4',
    'InputMetaDataStore/EventSelector.Counter',

    #SkimDecisionCollection_p1
    'StreamAOD_SkimDecisionsContainer',
    'StreamESD_SkimDecisionsContainer',
         ]

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

