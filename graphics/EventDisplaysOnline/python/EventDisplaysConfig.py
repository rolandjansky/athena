# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


def GetRunType():
  """Get the run type by reading the run-type setting in the partition from IS """

  #Get a logger so we can show formated messages
  from AthenaCommon.Logging import logging
  mlog = logging.getLogger( 'EventDisplays' )

  #Try to get the partition name
  try :
    import os
    partition = os.environ['TDAQ_PARTITION']
  except KeyError :
    partition = "ATLAS"
    mlog.warning("TDAQ_PARTITION not defined in environment, using %s as default"%partition)
  
  mlog.debug('Probing partition %s for RunType'%partition)

  #now try and read the information from IS
  try :
    from ipc import IPCPartition
    from ispy import ISObject
    runparams = ISObject(IPCPartition(partition), 'RunParams.RunParams','RunParams')
    runparams.checkout()
    runType = runparams.run_type
  except UserWarning, err:
    mlog.error(err)
    #Set the default runtype
    runType="collisions"
    mlog.warning("Failed to read run type from IS, using %s as default"%runType)
  finally :
    if not runType in ['collisions','singlebeam','cosmics']:
      mlog.fatal("Invalid run type: %s"%runType)
      import sys
      sys.exit(1)

  mlog.info("Setting run type to: %s"%runType)
  return runType

def GetBFields():

  #Get a logger so we can show formated messages
  from AthenaCommon.Logging import logging
  mlog = logging.getLogger( 'EventDisplays' )

  #BFields are read from initial partition
  partition='initial'
  mlog.debug("Trying to read magnetic field configuration from partition %s"%partition)

  #now try and read the information from IS
  try :
    from ipc import IPCPartition
    from ispy import ISObject
    #Get hold of the initial partition
    ipcPart = IPCPartition(partition);
    if not ipcPart.isValid():
      raise UserWarning("Partition %s invalid - cannot access magnetic field setting"%partition);
    #Get the current and valid status
    toroidCurrent = ISObject(ipcPart,'DCS_GENERAL.MagnetToroidsCurrent.value','DdcFloatInfo')
    solenoidCurrent = ISObject(ipcPart,'DCS_GENERAL.MagnetSolenoidCurrent.value','DdcFloatInfo')
    toroidInvalid = ISObject(ipcPart,'DCS_GENERAL.MagnetToroidsCurrent.invalid','DdcIntInfo')
    solenoidInvalid = ISObject(ipcPart,'DCS_GENERAL.MagnetSolenoidCurrent.invalid','DdcIntInfo')
    toroidCurrent.checkout()
    solenoidCurrent.checkout()
    toroidInvalid.checkout()
    solenoidInvalid.checkout()
    #And calculate the flags
    solenoidOn=((solenoidCurrent.value > 1000.) and (solenoidInvalid.value == 0))
    toroidOn=((toroidCurrent.value > 1000.) and (toroidInvalid.value == 0))
  except UserWarning, err:
    mlog.error(err)
    #Should always be able to access initial parititon
    mlog.fatal("Failed to read magnetic field configuration from IS, aborting")
    import sys
    sys.exit(1)

  #print the result
  mlog.info("Magnetic field in solenoid is %s" % ((solenoidOn and "ON") or "OFF"))
  mlog.info("Magnetic field in toroid is %s" % ((toroidOn and "ON") or "OFF"))

  #finally return our values
  return (solenoidOn,toroidOn)

if __name__ == "__main__":
  runType=GetRunType()
  print "RunType: %s"%runType
  bFields = GetBFields()
  print "BFields (Sol,Tor):",bFields   
