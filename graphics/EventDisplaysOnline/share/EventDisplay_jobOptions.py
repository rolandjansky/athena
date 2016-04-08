#########################################
#
# Setup for running event displays at point1
# 
#########################################

#Define the general output directory for VP1 and Atlantis
#All events will be stored in subdirectories of that directory
if not 'OutputDirectory' in dir():
  OutputDirectory="/EventDisplayEvents/"
#Make sure it ends with a "/"
if not OutputDirectory.endswith("/"):
  OutputDirectory += "/"

#Make sure we run infintly if not otherwise specified on the command line
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
if 'EvtMax' in dir():
  athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)
else:
  athenaCommonFlags.EvtMax.set_Value_and_Lock(-1)

#Get a logger so we can show formated messages
from AthenaCommon.Logging import logging                                                                                                                
mlog = logging.getLogger( 'EventDisplays' )                                                                                       

#Make sure we have a stream name defined
if not 'Stream' in dir():
  Stream="Default"

#Make sure the Splash flag exists
if not 'Splash' in dir():
  Splash=False

#Create phony stream name
if not 'StreamName' in dir():
  StreamName = Stream+(Splash and '-Splash' or '')

#Assemble final output directory
OutputDirectory += StreamName

#Make sure the output directory exists and is writable
import os, stat
if os.access(OutputDirectory, os.F_OK):
  if os.path.isdir(OutputDirectory) and os.access(OutputDirectory, os.W_OK):
    mlog.info("using existing output directory \'%s\' for stream \'%s\'" % (OutputDirectory, StreamName))
  else:
    mlog.fatal("cannot write to directory \'%s\'" % OutputDirectory)
    raise OSError("cannot write to directory \'%s\'" % OutputDirectory)
else:
  try:
    os.mkdir(OutputDirectory);
    os.chmod(OutputDirectory, stat.S_IRWXU | stat.S_IRWXG | stat.S_IROTH | stat.S_IXOTH)
    mlog.info("created output directory \'%s\' for stream \'%s\'" % (OutputDirectory, StreamName))
  except OSError, err:
    mlog.fatal("failed to create output directory \'%s\' for stream \'%s\': %s", (OutputDirectory, StreamName, err.strerror))
    raise err

#Bytestream input is configured here
include('EventDisplaysOnline/ByteStreamInput_jobOptions.py')

#Add reconstruction on top of that
if not Splash:
  include('EventDisplaysOnline/RecoCommon_jobOptions.py')
else :
  include('EventDisplaysOnline/SplashEvent_jobOptions.py')

#Add Atlantis-specific jobOptions
include('EventDisplaysOnline/Atlantis_jobOptions.py')

#Add VP1-specific jobOptions
include('EventDisplaysOnline/VP1_jobOptions.py')

#Finally configure the ByteStreamController (after RecoCommon!)
#include('EventDisplaysOnline/ByteStreamController_jobOptions.py')
