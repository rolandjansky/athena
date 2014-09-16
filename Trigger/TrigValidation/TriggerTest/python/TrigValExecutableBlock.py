# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock import   TrigValJobOptionBlock

class TrigValExecutableBlock(TrigValJobOptionBlock):
   def __init__(self, current) :
     TrigValJobOptionBlock.__init__(self, "Athena",['Athena','AthenaAllAlgsTrue','AthenaNoTrigger',  #RecExCommon topO
                                                    'AthenaL1Only','AthenaStandalone', 'AthenaMT','AthenaPT']) #Standalone topO

     self.name = "Executable"
     if(current  == "" ) : current = "Athena"
     self.setCurrent(current)  # this calls initializeBlocks()


   def jobNameBlock(self) :
     return self.current()
  
   ######## RecExCommon vs Standalone ###############################
   def includeRecExCommon_flags(self) :
     if( self.current() ==  'AthenaStandalone' or  self.current() == 'AthenaMT' or self.current() == 'AthenaPT') :
       tmpStr="""
rec.doTruth=False       
#-----------------------------------------------------------
include(\"RecExCommon/RecExCommon_flags.py\")
#-----------------------------------------------------------
"""
     else :
       tmpStr="""
#-----------------------------------------------------------
include(\"RecExCommon/RecExCommon_flags.py\")
#-----------------------------------------------------------
"""
     return tmpStr


   def includeRecExCommon_topOptions(self) :
     if( self.current() ==  'AthenaStandalone' or  self.current() == 'AthenaMT' or self.current() == 'AthenaPT') :
       tmpStr="""
#-----------------------------------------------------------       
include(\"TriggerRelease/Trigger_topOptions_standalone.py\")
#-----------------------------------------------------------
"""
     else :
       tmpStr="""
#-----------------------------------------------------------
include(\"RecExCommon/RecExCommon_topOptions.py\")
#-----------------------------------------------------------
"""
     return tmpStr


   def  initializeBlocks(self) :

     ######## Common settings ###############################
     self.jobProperties     = """

if not ('EvtMax' in dir()):
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

###############################
rec.doCBNT=False

"""
     if( self.current() != 'AthenaNoTrigger' ) :
        self.jobProperties     += """
rec.doTrigger=True
"""
     else :
        self.jobProperties     += """
rec.doTrigger=False
"""
        
     ####################     Athena    ###############################
     if(  self.current() != 'AthenaAllAlgsTrue' and self.current() != 'AthenaNoTrigger' ) :
           self.jobProperties     += "doESD=False\n"
     else :
           self.jobProperties     += "doESD=True \n"

     if(    self.current() == 'Athena' or  self.current() == 'AthenaAllAlgsTrue' or self.current() == 'AthenaStandalone' ) :
        self.extraTopOptions = """
jobproperties.PerfMonFlags.doMonitoring = True

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr

"""



     ####################     AthenaAllFlagsTrue    ###############################
     else :
        self.jobProperties     = "## WARNING jobO are not yet setup for "+self.current()


