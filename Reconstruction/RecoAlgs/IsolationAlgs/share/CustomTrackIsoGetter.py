# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.RecFlags import rec

# Import the xAOD isolation parameters.
from xAODPrimitives.xAODIso import xAODIso as isoPar
ptconeList = [[isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20]]

## Put the new algs before egamma locker... (so right after default IsolationBuilder)
theAlgBeforeWhereToPutIsoAlg = 'egLocker'
topSequence = AlgSequence()
index = -1
if not hasattr(topSequence,theAlgBeforeWhereToPutIsoAlg):
  theAlgBeforeWhereToPutIsoAlg = 'StreamESD'
if not hasattr(topSequence,theAlgBeforeWhereToPutIsoAlg):
  theAlgBeforeWhereToPutIsoAlg = 'StreamAOD'
if not hasattr(topSequence,theAlgBeforeWhereToPutIsoAlg):
  print 'no egLocker, no output stream ! put the new algs at the end of the sequence'
  index = len(topSequence)
else:
  for i,comp in enumerate(topSequence):
    print i, comp
    if comp.getName() == theAlgBeforeWhereToPutIsoAlg:
      index = i
      break
  print 'index of algo ',theAlgBeforeWhereToPutIsoAlg,' = ',index,', putting custom track isol algs just after...'

from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool

#Customisation
CustomCuts = [ [ 3.0,  400, 'TightPrimary' ], [ 3.0,  500, 'TightPrimary' ] , [ 3.0,  800, 'TightPrimary' ], [ 3.0, 1200, 'TightPrimary' ], [ 3.0, 1000, 'Loose' ], 
               [ 1.5, 1000, 'TightPrimary' ], [ 5.0, 1000, 'TightPrimary' ] ]
for icut in CustomCuts:

  CustomType = icut[2]+'z0'+str(int(10*icut[0]))+'Pt'+str(icut[1])+'MeV'
  print 'Building track isolation tool for type ',CustomType
  TrackIsolationCustom = xAOD__TrackIsolationTool(name = 'TrackIsolationTool'+CustomType)
  TrackIsolationCustom.TrackSelectionTool.maxZ0SinTheta = float(icut[0])
  TrackIsolationCustom.TrackSelectionTool.minPt         = float(icut[1])
  TrackIsolationCustom.TrackSelectionTool.CutLevel      = icut[2]
  ToolSvc += TrackIsolationCustom
  isoBuilderCustom = IsolationBuilder(
    name                   = "IsolationBuilder"+CustomType,
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationCustom, 
    EgIsoTypes             = [[]] if not rec.doEgamma() else ptconeList,
    MuIsoTypes             = [[]] if not rec.doMuon() else ptconeList,
    CustomConfgurationName = CustomType,
    OutputLevel            = 3)
  topSequence.insert(index,isoBuilderCustom)

##  --postInclude 'RecJobTransforms/UseFrontier.py' RAWtoESD:'IsolationAlgs/CustomTrackIsoGetter.py'
