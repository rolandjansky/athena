
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_strip
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_strip_ec
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middleback
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middleback_ecow
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middleback_eciw
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_stripmiddle
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_stripmiddle_ec
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_2strip
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_2strip_ec
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middle1
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middle2
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middle1_ec
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightCondAlg_middle2_ec
from LArRecUtils.LArRecUtilsConf import LArXTalkWeightGlobalCondAlg
from AthenaCommon.AlgSequence import AthSequencer

def LArXTalkWeightCondAlg_stripCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_strip"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_strip") # return existing one
   theLArXTalkWeightCondAlg_strip=LArXTalkWeightCondAlg_strip(name="LArXTalkWeightCondAlg_strip")
   condSeq+=theLArXTalkWeightCondAlg_strip
   return theLArXTalkWeightCondAlg_strip

def LArXTalkWeightCondAlg_strip_ecCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_strip_ec"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_strip_ec") # return existing one
   theLArXTalkWeightCondAlg_strip_ec=LArXTalkWeightCondAlg_strip_ec(name="LArXTalkWeightCondAlg_strip_ec")
   condSeq+=theLArXTalkWeightCondAlg_strip_ec
   return theLArXTalkWeightCondAlg_strip_ec

def LArXTalkWeightCondAlg_middlebackCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middleback"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middleback") # return existing one
   theLArXTalkWeightCondAlg_middleback = LArXTalkWeightCondAlg_middleback(name="LArXTalkWeightCondAlg_middleback")
   condSeq+=theLArXTalkWeightCondAlg_middleback
   return

def LArXTalkWeightCondAlg_middleback_ecowCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middleback_ecow"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middleback_ecow") # return existing one
   theLArXTalkWeightCondAlg_middleback_ecow=LArXTalkWeightCondAlg_middleback_ecow(name="LArXTalkWeightCondAlg_middleback_ecow")
   condSeq+=theLArXTalkWeightCondAlg_middleback_ecow
   return theLArXTalkWeightCondAlg_middleback_ecow

def LArXTalkWeightCondAlg_middleback_eciwCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middleback_eciw"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middleback_eciw") # return existing one
   theLArXTalkWeightCondAlg_middleback_eciw=LArXTalkWeightCondAlg_middleback_eciw(name="LArXTalkWeightCondAlg_middleback_eciw")
   condSeq+=theLArXTalkWeightCondAlg_middleback_eciw
   return theLArXTalkWeightCondAlg_middleback_eciw

def LArXTalkWeightCondAlg_stripmiddleCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_stripmiddle"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_stripmiddle") # return existing one
   theLArXTalkWeightCondAlg_stripmiddle=LArXTalkWeightCondAlg_stripmiddle(name="LArXTalkWeightCondAlg_stripmiddle")
   condSeq+=theLArXTalkWeightCondAlg_stripmiddle
   return theLArXTalkWeightCondAlg_stripmiddle

def LArXTalkWeightCondAlg_stripmiddle_ecCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_stripmiddle_ec"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_stripmiddle_ec") # return existing one
   theLArXTalkWeightCondAlg_stripmiddle_ec=LArXTalkWeightCondAlg_stripmiddle_ec(name="LArXTalkWeightCondAlg_stripmiddle_ec")
   condSeq+=theLArXTalkWeightCondAlg_stripmiddle_ec
   return theLArXTalkWeightCondAlg_stripmiddle_ec

def LArXTalkWeightCondAlg_2stripCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_2strip"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_2strip") # return existing one
   theLArXTalkWeightCondAlg_2strip=LArXTalkWeightCondAlg_2strip(name="LArXTalkWeightCondAlg_2strip")
   condSeq+=theLArXTalkWeightCondAlg_2strip
   return theLArXTalkWeightCondAlg_2strip

def LArXTalkWeightCondAlg_2strip_ecCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_2strip_ec"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_2strip_ec") # return existing one
   theLArXTalkWeightCondAlg_2strip_ec=LArXTalkWeightCondAlg_2strip_ec(name="LArXTalkWeightCondAlg_2strip_ec")
   condSeq+=theLArXTalkWeightCondAlg_2strip_ec
   return theLArXTalkWeightCondAlg_2strip_ec

def LArXTalkWeightCondAlg_middle1Cfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middle1"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middle1") # return existing one
   theLArXTalkWeightCondAlg_middle1=LArXTalkWeightCondAlg_middle1(name="LArXTalkWeightCondAlg_middle1")
   condSeq+=theLArXTalkWeightCondAlg_middle1
   return theLArXTalkWeightCondAlg_middle1

def LArXTalkWeightCondAlg_middle2Cfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middle2"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middle2") # return existing one
   theLArXTalkWeightCondAlg_middle2=LArXTalkWeightCondAlg_middle2(name="LArXTalkWeightCondAlg_middle2")
   condSeq+=theLArXTalkWeightCondAlg_middle2
   return theLArXTalkWeightCondAlg_middle2

def LArXTalkWeightCondAlg_middle1_ecCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middle1_ec"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middle1_ec") # return existing one
   theLArXTalkWeightCondAlg_middle1_ec=LArXTalkWeightCondAlg_middle1_ec(name="LArXTalkWeightCondAlg_middle1_ec")
   condSeq+=theLArXTalkWeightCondAlg_middle1_ec
   return theLArXTalkWeightCondAlg_middle1_ec

def LArXTalkWeightCondAlg_middle2_ecCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightCondAlg_middle2_ec"):
      return getattr(condSeq,"LArXTalkWeightCondAlg_middle2_ec") # return existing one
   theLArXTalkWeightCondAlg_middle2_ec=LArXTalkWeightCondAlg_middle2_ec(name="LArXTalkWeightCondAlg_middle2_ec")
   condSeq+=theLArXTalkWeightCondAlg_middle2_ec
   return theLArXTalkWeightCondAlg_middle2_ec

def LArXTalkWeightGlobalCondAlgCfg():
   condSeq = AthSequencer("AthCondSeq")
   if hasattr(condSeq,"LArXTalkWeightGlobalCondAlg"):
      return getattr(condSeq,"LArXTalkWeightGlobalCondAlg") # return existing one
   theLArXTalkWeightGlobalCondAlg=LArXTalkWeightGlobalCondAlg(name="LArXTalkWeightGlobalCondAlg")
   condSeq+=theLArXTalkWeightGlobalCondAlg
   return theLArXTalkWeightGlobalCondAlg


def LArXTalkWeightCondAlgDefault():
  # make sure we start everything
  LArXTalkWeightCondAlg_stripCfg()
  LArXTalkWeightCondAlg_strip_ecCfg()
  LArXTalkWeightCondAlg_middlebackCfg()
  LArXTalkWeightCondAlg_middleback_ecowCfg()
  LArXTalkWeightCondAlg_middleback_eciwCfg()
  LArXTalkWeightCondAlg_stripmiddleCfg()
  LArXTalkWeightCondAlg_stripmiddle_ecCfg()
  LArXTalkWeightCondAlg_2stripCfg()
  LArXTalkWeightCondAlg_2strip_ecCfg()
  LArXTalkWeightCondAlg_middle1Cfg()
  LArXTalkWeightCondAlg_middle2Cfg()
  LArXTalkWeightCondAlg_middle1_ecCfg()
  LArXTalkWeightCondAlg_middle2_ecCfg()
  LArXTalkWeightGlobalCondAlgCfg()
