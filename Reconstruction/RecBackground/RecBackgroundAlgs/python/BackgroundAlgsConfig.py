# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BackgroundAlgsCfg(flags):

  result=ComponentAccumulator()

  haveCSC=flags.Detector.GeometryCSC 

  result.addEventAlgo(CompFactory.BeamBackgroundFiller(cscSegmentContainerKey=("NCB_TrackMuonSegments" if haveCSC else "")))

  result.addEventAlgo(CompFactory.BcmCollisionTimeAlg())

  result.addEventAlgo(CompFactory.BackgroundWordFiller(IsMC=flags.Input.isMC))

  return result

#No self-test __main__ here because these algos depend on many event data objects not stored in ESD, therefore they can't run stand-alone. 
