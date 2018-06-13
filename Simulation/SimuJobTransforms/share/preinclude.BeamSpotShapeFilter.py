from AthenaCommon.AlgSequence import AthSequencer

from AthenaCommon.AppMgr import ServiceMgr

from SimuJobTransforms.SimBeamSpotShapeFilter import *

include("InDetBeamSpotService/BeamCondSvc.py")
if not hasattr(ServiceMgr, 'BeamCondSvc'):
  from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
  beamcondsvc = BeamCondSvc('BeamCondSvc')
  ServiceMgr += beamcondsvc

bsFilter = SimBeamSpotShapeFilter()
if 'targetSigmaX' in locals():
  bsFilter.targetSigmaX = targetSigmaX
if 'targetSigmaY' in locals():
  bsFilter.targetSigmaY = targetSigmaY
if 'targetSigmaZ' in locals():
  bsFilter.targetSigmaZ = targetSigmaZ
if 'initialBStag' in locals():
  bsFilter.initialBStag = initialBStag
if 'targetBStag' in locals():  
  bsFilter.targetBStag = targetBStag

topSequence += bsFilter
AcceptList = [ bsFilter.name() ]

