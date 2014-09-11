# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.AppMgr import ToolSvc

from JetCalibTools.JetCalibToolsConf import JetSequenceManager
from JetCalibTools.MakeCalibSequences import getStandardCalibSequence


defaultJetColl = [
    ("Cone",0.4,"Tower"),
    ("Cone",0.4,"Topo"),
    ("Cone",0.4,"LCTopo"),
    ("AntiKt",0.4,"Tower"),
    ("AntiKt",0.4,"Topo"),
    ("AntiKt",0.4,"LCTopo"),
    ("AntiKt",0.6,"Tower"),
    ("AntiKt",0.6,"Topo"),
    ("AntiKt",0.6,"LCTopo"),
]

defaultCalib = [
    "StandardH1Calib",
    "H1NumInv",
    "NumInv",
    "LayerFracs1",
    "LayerFracs2",
    "LayerFracs3",
    "LayerFracsDefault",
]

defaultManager = JetSequenceManager("DefaultManager")

for jcoll in defaultJetColl :
    option = {
        'finder' : jcoll[0], 'mainParam' : jcoll[1], 'input' : jcoll[2]
        }
    for calib in defaultCalib :
        defaultManager.SequenceList += [ getStandardCalibSequence(calib, useJetCollName=True, **option) ]
        

ToolSvc += defaultManager

