# steering file for RDO->ESD step without trigger
# see myTopOptions.py for more info

#doCBNT=False
doTrigger=False
doWriteRDO=False
if not 'doWriteESD' in dir():
    doWriteESD=True
doWriteAOD=False
doAOD=False 
doWriteTAG=False 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from D3PDMakerCoreComps.MakerAlg import *
from JetD3PDMaker.JetD3PDObject import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *

# Configure a very basic JetD3PDMaker
alg = MakerAlg("MyTuple", topSequence, file = "JetD3PD.root", D3PDSvc = 'D3PD::RootD3PDSvc')

alg += EventInfoD3PDObject (10)

# AntiKt4H1TowerJets as a default
alg += JetD3PDObject (3, prefix='AntiKt4H1TowerJets_', sgkey='AntiKt4H1TowerJets')
# AntiKt4TruthJets - note that the detail level is chosen to be 0
# because all quantities at other levels are reco-only
alg += JetD3PDObject (0, prefix='AntiKt4TruthJets_',   sgkey='AntiKt4TruthJets')

topSequence += alg
                

