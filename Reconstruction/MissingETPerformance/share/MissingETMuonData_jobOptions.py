include.block( "MissingETPerformance/MissingETMuonData_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     MissingETMuonData as ConfiguredMissingETMuonData
MissingETMuonData = ConfiguredMissingETMuonData(

    McEventCollectionKey        = 'GEN_AOD', #set to '' to turn off truth retreival, AOD container is called 'GEN_AOD',ESD container is called 'TruthEvent'
    MuonContainerKey            = 'StacoMuonCollection',
    CaloMuonContainerKey        = 'CaloMuonCollection',
    JetKey                      = 'AntiKt6LCTopoJets',
    )

ToolSvc += MissingETMuonData
print      MissingETMuonData
