# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import ConfigurableAlgorithm
from AthenaCommon.Constants import *   # for "INFO"

class FileStagerAlg( ConfigurableAlgorithm ):
   # slots is a sequence of properties that the use is allowed to set
   # either provide a tuple/list with names, like so:
   #
   #  __slots__ = [ 'MyInt', 'MyBool', 'MyDouble', 'MyStringVec' ]
   #
   # or provide a dictionary with names and initial (error) values:
   __slots__ = [ 'PipeLength', 'VerboseStager', 'VerboseWaiting', 'FirstFileAlreadyStaged'
                 'TreeName', 'InfilePrefix', 'OutfilePrefix',
                 'CpCommand', 'BaseTmpdir', 'CpArguments',
                 'InputCollections', 'OutputCollections',
                 'LogfileDir', 'KeepLogfiles', 'StoreStatistics' ]
   
   def __init__( self, name = "FileStagerAlg" ):
    # Base class init MUST be called
      super( FileStagerAlg, self ).__init__( name )

   def getServices( self ):
    # this method is required: it tells the framework which
    # services are needed to run this algorithm successfully
    # Note: the following services are not really needed, but
    # it does allow to show how a couple of parameters can be
    # set for these services (see below, configureXYZ)
      return [ "MessageSvc", "ApplicationMgr" ]

   def getDlls( self ):
    # this method is required: it tells the framework where the
    # FileStagerAlg algorithm is located (in libFileStager.so)
      return "FileStager"

   def getType( cls ):
      return "FileStagerAlg"

   def setDefaults( cls, hello ):
    # this method is required: it sets the defaults for the
    # FileStagerAlg algorithm
    # Note: if the user has already specified values for any of
    # these properties, than those values are not overwritten
    hello.PipeLength = 1
    hello.VerboseStager = False
    hello.VerboseWaiting = True
    hello.FirstFileAlreadyStaged = True
    hello.TreeName = "CollectionTree"
    hello.InfilePrefix = "gridcopy://"
    hello.OutfilePrefix = "file:"
    hello.CpCommand = "lcg-cp"
    hello.CpArguments = "-v"
    hello.CpArguments = "--vo"
    hello.CpArguments = "atlas"
    hello.LogfileDir = ""
    hello.KeepLogfiles = False
    hello.StoreStatistics = False

   def configureMessageSvc( self, msgsvc ):
    # this method is optional: it is used to configure the
    # MessageSvc, and will only be called if "MessageSvc" is in
    # the list obtained from getServices
    # Note: in general you don't want to touch top level svcs
    # here, and you should configure FileStagerAlg.OutputLevel in
    # setDefaults() instead
      msgsvc.OutputLevel = WARNING

