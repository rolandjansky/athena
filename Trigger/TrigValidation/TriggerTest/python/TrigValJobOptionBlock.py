# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

class TrigValJobOptionBlock:
  def __init__(self, defaultOption, allOptions ):
     self._list = allOptions
     self._current = defaultOption
     if( defaultOption not in self._list ) :
        self._list += defaultOption
     
     self.name              = "NotSet"
     self.jobProperties     = ""
     self.jobFlags          = ""
     self.extraTopOptions   = ""

     self.isJobOptionBlock  = True

  def help(self) :
     print "=== JobO block "+self.name+" has options"
     print self._list
     print "    current default is "+self.current()

  def current(self) :
     return self._current

  def setCurrent(self, option) :
     if( self.has_option(option) ):
        self._current = option
        self.initializeBlocks()

  def has_option(self, option) :
     if( option in self._list ): 
        return True
     else :
        print "jobOptionDict "+self.name+" ERROR : uknown option "+option+", possible candidates are :"
        print self._list
        return False
 

  def initializeBlocks():  # by default do nothing, but derived classes should overwrite this
        return None

  def die(self) :
     print " TrigValJobOptionBlock kills the job"
     import sys 
     sys.exit(2)

  def  includeJobProperties(self, jobO) :
     try :
        jobO.write(self.jobProperties )  
     except :
        print " TrigValJobOptionBlock   ERROR cannot write block to "+jobO.name
        self.die()

  def  modifyJobFlags(self, jobO) :
     try :
        jobO.write(self.jobFlags )  
     except :
        print " TrigValJobOptionBlock   ERROR cannot write block to "+jobO.name
        self.die()

  def  addExtraTopOption(self, jobO) :
     try :
        jobO.write(self.extraTopOptions )  
     except :
        print " TrigValJobOptionBlock   ERROR cannot write block to "+jobO.name
        self.die()

