# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ispy


def GetAtlasReady():
   from ispy import ISObject, IPCPartition
   try:
      r4p = ISObject(IPCPartition("ATLAS"), 'RunParams.Ready4Physics', 'RunParams')
      r4p.checkout()
      return r4p.ready4physics
   except Exception:
      print ("#### Failed to determine if we are ready for physics")
      raise

def GetRunType():
  """Get the run type by reading the run-type setting in the partition from IS """

  #Get a logger so we can show formated messages
  from AthenaCommon.Logging import logging
  mlog = logging.getLogger('RecExOnline')

  #Try to get the partition name
  try:
    import os
    partition = os.environ['TDAQ_PARTITION']
    if partition == "EventDisplays":
      partition = "ATLAS"
  except KeyError:
    partition = "ATLAS"
    mlog.warning("TDAQ_PARTITION not defined in environment, using %s as default", partition)

  #now try and read the information from IS
  try:
    from ipc import IPCPartition
    from ispy import ISObject
    ipcPart = IPCPartition(partition)
    if not ipcPart.isValid():
      raise UserWarning("Partition %s invalid - cannot access run type settings" % partition)
    runparams = ISObject(ipcPart, 'RunParams.RunParams', 'RunParams')
    runparams.checkout()
    beamEnergy = runparams.beam_energy
    projectTag = runparams.T0_project_tag
  except UserWarning as err:
    mlog.error(err)
    beamEnergy = None
    projectTag = None

  mlog.info("Setting project tag to %s", projectTag)
  return (None, beamEnergy, projectTag) # the BeamType in the IS RunParams is not useful for auto-configuration

def GetBFields():
  #Get a logger so we can show formated messages
  from AthenaCommon.Logging import logging
  mlog = logging.getLogger('RecExOnline')

  #BFields are read from initial partition
  partition = 'initial'
  mlog.debug("Trying to read magnetic field configuration from partition %s", partition)

  #now try and read the information from IS
  try:
    from ipc import IPCPartition
    ipcPart = IPCPartition(partition)
    if not ipcPart.isValid():
      raise UserWarning("Partition %s invalid - cannot access magnetic field setting" % partition)
    #Get the current and valid status
    #     from ispy import ISObject
    #     torCurrent = ISObject(ipcPart, 'DCS_GENERAL.MagnetToroidsCurrent.value', 'DdcFloatInfo')
    #     solCurrent = ISObject(ipcPart, 'DCS_GENERAL.MagnetSolenoidCurrent.value', 'DdcFloatInfo')
    #     torInvalid = ISObject(ipcPart, 'DCS_GENERAL.MagnetToroidsCurrent.invalid', 'DdcIntInfo')
    #     solInvalid = ISObject(ipcPart, 'DCS_GENERAL.MagnetSolenoidCurrent.invalid', 'DdcIntInfo')
    #     torCurrent.checkout()
    #     solCurrent.checkout()
    #     torInvalid.checkout()
    #     solInvalid.checkout()
    
    # AL playing around: start
    torCurrent   = ispy.ISInfoDynAny(ipcPart, 'DdcFloatInfo')
    solCurrent   = ispy.ISInfoDynAny(ipcPart, 'DdcFloatInfo')
    torInvalid   = ispy.ISInfoDynAny(ipcPart, 'DdcIntInfo')
    solInvalid   = ispy.ISInfoDynAny(ipcPart, 'DdcIntInfo')
    
    torInvalid.value=1
    solInvalid.value=1
    # AL playing around: end
    
    mlog.info("toroidCurrent = %f", torCurrent.value)
    mlog.info("toroidInvalid = %f", torInvalid.value)
    mlog.info("solenoidCurrent = %f", solCurrent.value)
    mlog.info("solenoidInvalid = %f", solInvalid.value)
    
    
    #And calculate the flags
    solOn = ((solCurrent.value > 1000.) and (solInvalid.value == 0))
    torOn = ((torCurrent.value > 1000.) and (torInvalid.value == 0))
  except UserWarning as err:
    mlog.error(err)
    #Should always be able to access initial parititon
    mlog.fatal("Failed to read magnetic field configuration from IS, aborting")
    import sys
    sys.exit(1)

  #print the result
  mlog.info("Magnetic field in solenoid is %s", ((solOn and "ON") or "OFF"))
  mlog.info("Magnetic field in toroid is %s",   ((torOn and "ON") or "OFF"))

  #finally return our values
  return (solCurrent, torCurrent)

if __name__ == "__main__":
  runType = GetRunType()
  print ("(BeamType, BeamEnergy, ProjectTag): ", runType)
  bFields = GetBFields()
  print ("(SolCurrent, TorCurrent):", (bFields[0].value, bFields[1].value))
