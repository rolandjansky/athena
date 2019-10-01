# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from LArCellRec.LArNoisyROFlags import larNoisyROFlags
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArBadChannelTool.LArBadFebsConfig import LArKnownBadFebCfg, LArKnownMNBFebCfg

def LArNoisyROSummaryCfg(configFlags):

   result=ComponentAccumulator()

   result.merge(LArKnownBadFebCfg(configFlags))
   result.merge(LArKnownMNBFebCfg(configFlags))

   # now configure the algorithm
   try:        
       from LArCellRec.LArCellRecConf import LArNoisyROAlg,LArNoisyROTool
   except Exception:
       import traceback
       print(traceback.format_exc())
       return result

   theLArNoisyROTool=LArNoisyROTool(CellQualityCut=larNoisyROFlags.CellQualityCut(),
                                    BadChanPerFEB=larNoisyROFlags.BadChanPerFEB(),
                                    BadFEBCut=larNoisyROFlags.BadFEBCut(),
                                    MNBLooseCut=larNoisyROFlags.MNBLooseCut(),
                                    MNBTightCut=larNoisyROFlags.MNBTightCut(),
                                    MNBTight_PsVetoCut=larNoisyROFlags.MNBTight_PsVetoCut()
                                    )

   result.setPrivateTools(theLArNoisyROTool)
   theLArNoisyROAlg=LArNoisyROAlg()
   theLArNoisyROAlg.Tool=result.popPrivateTools()
   result.addEventAlgo(theLArNoisyROAlg)

   return result

