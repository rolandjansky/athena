# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# python fragment to configure LAr collision time algo

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArCollisionTimeCfg(configFlags, cutIteration=False):

   result=ComponentAccumulator()

   from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
   result.merge(BunchCrossingCondAlgCfg(configFlags))
   from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
   result.merge(CaloNoiseCondAlgCfg(configFlags,"totalNoise"))

   LArCollisionTimeAlg=CompFactory.LArCollisionTimeAlg
   result.addEventAlgo(LArCollisionTimeAlg("LArCollisionTimeAlg", 
                       isMC=configFlags.Input.isMC,
                       cutIteration=cutIteration))

   return result   

