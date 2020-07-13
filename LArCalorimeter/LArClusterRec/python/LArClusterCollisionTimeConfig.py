# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# python fragment to configure LAr cluster collision time algo

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArClusterCollisionTimeCfg(configFlags):

   result=ComponentAccumulator()

   LArClusterCollisionTimeAlg=CompFactory.LArClusterCollisionTimeAlg
   result.addEventAlgo(LArClusterCollisionTimeAlg("LArClusterCollisionTimeAlg", 
                                                  OutputName="ClusterCollTime",InputName="LArClusterEM"))

   return result   

