# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

def RoiWriterCfg(flags):
   """Configure RoiWriter to convert AOD RoI descriptors to xAOD"""

   from TrigEDMConfig.TriggerEDMRun2 import TriggerRoiList
   from OutputStreamAthenaPool.OutputStreamConfig import addToAOD

   acc = ComponentAccumulator()

   roiWriter = CompFactory.RoiWriter()
   if flags.Input.Format is Format.BS:
      # If TrigBSExtraction is running, enforce order via fictional data dependency
      roiWriter.ExtraInputs = [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]

   acc.addEventAlgo( roiWriter )
   acc.merge( addToAOD(flags, TriggerRoiList) )

   return acc
