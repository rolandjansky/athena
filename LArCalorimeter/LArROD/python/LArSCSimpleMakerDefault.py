# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from LArROD.LArRODConf import LArSCSimpleMaker as _LArSCSimpleMaker
from LArROD.LArRODConf import LArSuperCellBCIDEmAlg as _LArSuperCellBCIDEmAlg

class LArSCSimpleMaker(_LArSCSimpleMaker):
   __slots__ = []
   def __init__(self,name="LArSCSimpleMaker"):
     super( LArSCSimpleMaker,self).__init__(name)
     # bunch of needed stuff to make SuperCells from cells (conditions)
     from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
     from AthenaCommon.AlgSequence import AthSequencer
     CaloNoiseCondAlg(noisetype="electronicNoise")
     condSeq = AthSequencer("AthCondSeq")
     from CaloRec.CaloRecConf import CaloNoiseSigmaDiffCondAlg
     if not hasattr(condSeq,"CaloNoiseSigmaDiffCondAlg"):
       condSeq+=CaloNoiseSigmaDiffCondAlg()
     from CaloRec.CaloBCIDAvgAlgDefault import CaloBCIDAvgAlgDefault
     CaloBCIDAvgAlgDefault()
     self.SCellContainer="SimpleSCellNoBCID"

class LArSuperCellBCIDEmAlg(_LArSuperCellBCIDEmAlg):
   __slots__ = []
   def __init__(self,name="LArSuperCellBCIDEmAlg"):
     super( LArSuperCellBCIDEmAlg,self).__init__(name)
     # needed conditions to compensate cells from Super-Cells
     from CaloRec.CaloBCIDAvgAlgDefault import CaloBCIDAvgAlgDefault
     CaloBCIDAvgAlgDefault()
     self.SCellContainerIn="SimpleSCellNoBCID"

