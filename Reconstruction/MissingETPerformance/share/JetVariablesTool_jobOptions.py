include.block( "MissingETPerformance/JetVariablesTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     JetVariablesTool as ConfiguredJetVariablesTool
JetVariablesTool = ConfiguredJetVariablesTool(

    JetCollectionKey            =    'AntiKt6LCTopoJets',
    JetTruthCollectionKey       =    'AntiKt6TruthJets',
    TrackParticleContainerKey   =    'TrackParticleCandidate',

    )

ToolSvc += JetVariablesTool
print      JetVariablesTool
