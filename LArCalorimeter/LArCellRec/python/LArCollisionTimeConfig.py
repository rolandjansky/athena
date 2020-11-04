# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# python fragment to configure LAr collision time algo

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArCollisionTimeCfg(configFlags, cutIteration=False):

   result=ComponentAccumulator()

   LArCollisionTimeAlg=CompFactory.LArCollisionTimeAlg
   result.addEventAlgo(LArCollisionTimeAlg("LArCollisionTimeAlg", 
                       isMC=configFlags.Input.isMC,
                       cutIteration=cutIteration))

   return result   

