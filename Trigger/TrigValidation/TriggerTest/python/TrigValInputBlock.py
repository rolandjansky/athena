# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValInputBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, 'RDO', ['RDO','BS', 'ESD','AOD','BStmp'])

     self.name = "Input"
     if(current  == "" ) : current = "RDO"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
      return self.current()


   def  initializeBlocks(self) :

     ####################    RDO     ###############################
     if(    self.current() == 'RDO' ) :
         self.jobProperties    = """
### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]
"""
         self.jobFlags = """
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
"""

     ####################    ESD     ###############################
     elif(    self.current() == 'ESD' ) :
         self.jobProperties    = """
acf.PoolESDInput=[\"../trigger.ESD.pool.root\"]
rec.readESD=True
"""
         self.jobFlags = """
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
"""
     ####################    AOD     ###############################
     elif(    self.current() == 'AOD' ) :
         self.jobProperties    = """
acf.PoolAODInput=[\"../trigger.AOD.pool.root\"]
rec.readAOD=True
"""
         self.jobFlags     += """
TriggerFlags.readHLTconfigFromXML=True
TriggerFlags.readLVL1configFromXML=True
# WARNING: please don't try running from ./outputLVL1/HLTconfig.xml, since Athena will try writing to that file
TriggerFlags.inputLVL1configFile = \"../outputLVL1config.xml\"
TriggerFlags.inputHLTconfigFile = \"../outputHLTconfig.xml\"
#### to avoid segmentation violation in 12.0.6
DetFlags.Muon_setOff()
"""
     ####################    BS     ###############################
     elif(  self.current() == 'BS' ) :
         self.jobProperties   = """
from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.InputFormat.set_bytestream()
    acf.BSRDOInput=[\"/afs/cern.ch/atlas/project/trigger/pesa-sw/releases/data/daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data\"]
    DetDescrVersion = \"ATLAS-CSC-01-02-00\"

"""
         self.jobFlags     += """
TriggerFlags.doHLTpersistency=False
TriggerFlags.readLVL1configFromXML=True
# WARNING: please don't try running from ./outputLVL1config.xml, since Athena will try writing to that file
TriggerFlags.inputLVL1configFile = \"../outputLVL1config.xml\"

"""
     ####################    BStmp     ###############################
     elif(  self.current() == 'BStmp' ) :
         self.jobProperties   = """
from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.InputFormat.set_bytestream()
    acf.BSRDOInput=[\"../trigger.BS.data\"]
    DetDescrVersion = \"ATLAS-CSC-01-02-00\"

"""
         self.jobFlags     += """
TriggerFlags.doHLTpersistency=False
TriggerFlags.readLVL1configFromXML=True
# WARNING: please don't try running from ./outputLVL1config.xml, since Athena will try writing to that file
TriggerFlags.inputLVL1configFile = \"../outputLVL1config.xml\"

"""
