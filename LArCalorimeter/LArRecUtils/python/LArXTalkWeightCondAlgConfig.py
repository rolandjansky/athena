
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArXTalkWeightCondAlg_stripCfg(flags,name="LArXTalkWeightCondAlg_strip", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_strip"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_strip(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_strip_ecCfg(flags,name="LArXTalkWeightCondAlg_strip_ec", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_strip_ec"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_strip_ec(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middlebackCfg(flags,name="LArXTalkWeightCondAlg_middleback", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middleback"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middleback(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middleback_ecowCfg(flags,name="LArXTalkWeightCondAlg_middleback_ecow", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middleback_ecow"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middleback_ecow(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middleback_eciwCfg(flags,name="LArXTalkWeightCondAlg_middleback_eciw", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middleback_eciw"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middleback_eciw(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_stripmiddleCfg(flags,name="LArXTalkWeightCondAlg_stripmiddle", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_stripmiddle"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_stripmiddle(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_stripmiddle_ecCfg(flags,name="LArXTalkWeightCondAlg_stripmiddle_ec", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_stripmiddle_ec"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_stripmiddle_ec(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_2stripCfg(flags,name="LArXTalkWeightCondAlg_2strip", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_2strip"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_2strip(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_2strip_ecCfg(flags,name="LArXTalkWeightCondAlg_2strip_ec", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_2strip_ec"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_2strip_ec(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middle1Cfg(flags,name="LArXTalkWeightCondAlg_middle1", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middle1"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middle1(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middle2Cfg(flags,name="LArXTalkWeightCondAlg_middle2", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middle2"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middle2(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middle1_ecCfg(flags,name="LArXTalkWeightCondAlg_middle1_ec", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middle1_ec"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middle1_ec(name, **kwargs))
   return acc

def LArXTalkWeightCondAlg_middle2_ecCfg(flags,name="LArXTalkWeightCondAlg_middle2_ec", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightCondAlg_middle2_ec"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightCondAlg_middle2_ec(name, **kwargs))
   return acc

def LArXTalkWeightGlobalCondAlgCfg(flags,name="LArXTalkWeightGlobalCondAlg", **kwargs):
   """Return ComponentAccumulator with configured LArXTalkWeightGlobalCondAlg"""
   acc = ComponentAccumulator()
   acc.addCondAlgo(CompFactory.LArXTalkWeightGlobalCondAlg(name, **kwargs))
   return acc

def LArXTalkWeightCondAlgCfg(flags):
  # make sure we start everything
  acc = ComponentAccumulator()
  acc.merge(LArXTalkWeightCondAlg_stripCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_strip_ecCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middlebackCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middleback_ecowCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middleback_eciwCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_stripmiddleCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_stripmiddle_ecCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_2stripCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_2strip_ecCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middle1Cfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middle2Cfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middle1_ecCfg(flags))
  acc.merge(LArXTalkWeightCondAlg_middle2_ecCfg(flags))
  acc.merge(LArXTalkWeightGlobalCondAlgCfg(flags))
  return acc
