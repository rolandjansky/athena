# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV, TeV
from AthenaCommon.Logging import logging
from AthenaConfiguration.Enums import BeamType, Format

log = logging.getLogger('OnlineRecoDefaults')


# Online reconstruction at P1 (Event Display, Global Monitoring) reads events
# directly from DCM and has no input file where metadata could be read from.
# Set all flags that would otherwise be auto-configured from input metadata.
def setDefaultOnlineRecoFlags(flags):
    '''
    Set all input-dependent flags explicitly to avoid attempts to read an input
    file when there is none
    '''

    flags.Common.isOnline = True

    flags.Input.Files = []
    flags.Input.isMC = False
    flags.Input.RunNumber = [-1]
    flags.Input.LumiBlockNumber = [-1]
    flags.Input.TimeStamp = [-1]
    flags.Input.ProjectName = 'data_test'
    flags.Input.Format = Format.BS
    flags.Input.ProcessingTags = []

    flags.Beam.Type = BeamType.Collisions
    flags.Beam.Energy = 7*TeV

    flags.IOVDb.GlobalTag = 'CONDBR2-HLTP-2018-01'
    flags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'

    flags.Trigger.EDMVersion = 3


# When running the job at P1 while the partition is running, the metadata can be read from IS
def trySetFlagsFromIS(flags, partition=None):
    '''Attempt to fill in run metadata from IS if available'''
    from PyUtils.moduleExists import moduleExists
    if not moduleExists('ipc') or not moduleExists('ispy'):
        log.warning('TDAQ python modules to read from IS (ipc, ispy) are unavailable')
        return

    from ipc import IPCPartition
    from ispy import ISObject

    if not partition:
        import os
        partition = os.getenv('TDAQ_PARTITION') or 'ATLAS'
    log.debug('Trying to read IS data for partition %s', partition)

    def tryReadISObject(obj_name, type_name, part_name=partition):
        try:
            obj = ISObject(IPCPartition(part_name), obj_name, type_name)
            obj.checkout()
            return obj
        except Exception as e:
            log.warning('Cannot read %s from partition %s, exception: %s', obj_name, part_name, e)
            return None

    def tryReadFromISObject(obj, attr, default=None):
        try:
            return obj.getAttributeValue(attr)
        except Exception as e:
            log.warning('Cannot read attribute %s from ISObject %s, exception: %s', attr, obj.name(), e)
            return default

    # Run parameters
    runparams = tryReadISObject('RunParams.RunParams', 'RunParams')
    if runparams:
        # Run number
        run_number = tryReadFromISObject(runparams, 'run_number')
        if run_number is not None:
            flags.Input.RunNumber = [run_number]
        # Tier-0 project name
        project_name = tryReadFromISObject(runparams, 'T0_project_tag')
        if project_name is not None:
            flags.Input.ProjectName = project_name
        # Beam type
        beam_type_num = tryReadFromISObject(runparams, 'beam_type')
        if beam_type_num is not None:
            flags.Beam.Type = BeamType.Collisions if beam_type_num > 0 else BeamType.Cosmics
        # Beam energy
        beam_energy = tryReadFromISObject(runparams, 'beam_energy')
        if beam_energy is not None:
            flags.Beam.Energy = beam_energy*GeV

    # Trigger configuration
    smk_is = tryReadISObject('RunParams.TrigConfSmKey', 'TrigConfSmKey')
    l1psk_is = tryReadISObject('RunParams.TrigConfL1PsKey', 'TrigConfL1PsKey')
    hltpsk_is = tryReadISObject('RunParams.TrigConfHltPsKey', 'TrigConfHltPsKey')
    bgk_is = tryReadISObject('RunParams.TrigConfL1BgKey', 'TrigConfL1BgKey')
    if smk_is and l1psk_is and hltpsk_is and bgk_is:
        smk = tryReadFromISObject(smk_is, 'SuperMasterKey')
        l1psk = tryReadFromISObject(l1psk_is, 'L1PrescaleKey')
        hltpsk = tryReadFromISObject(hltpsk_is, 'HltPrescaleKey')
        bgk = tryReadFromISObject(bgk_is, 'L1BunchGroupKey')
        if all([v is not None for v in [smk, l1psk, hltpsk, bgk]]):
            dbname = 'TRIGGERDB_RUN3'  # Run3 DB name (not listed in IS)
            flags.Trigger.triggerConfig = 'DB:{:s}:{:d},{:d},{:d},{:d}'.format(
                dbname, smk, l1psk, hltpsk, bgk)

    # LumiBlock information
    lbinfo = tryReadISObject('RunParams.LumiBlock', 'LumiBlock')
    if lbinfo:
        # LB number
        lb_number = tryReadFromISObject(lbinfo, 'LumiBlockNumber')
        if lb_number is not None:
            flags.Input.LumiBlockNumber = [lb_number]
        # Timestamp
        time_ns = tryReadFromISObject(lbinfo, 'Time')
        if time_ns is not None:
            flags.Input.TimeStamp = [int(time_ns / 1e9)]

    # Solenoid status
    solenoid_curr_is = tryReadISObject('DCS_GENERAL.MagnetSolenoidCurrent.value', 'DdcFloatInfo', part_name='initial')
    if solenoid_curr_is:
        solenoid_curr = tryReadFromISObject(solenoid_curr_is, 'value')
        if solenoid_curr is not None:
            flags.BField.solenoidOn = (solenoid_curr > 70) # threshold of 70 copied from RecExOnline_globalconfig.py

    # Toroid status
    toroid_curr_is = tryReadISObject('DCS_GENERAL.MagnetToroidsCurrent.value', 'DdcFloatInfo', part_name='initial')
    if toroid_curr_is:
        toroid_curr = tryReadFromISObject(toroid_curr_is, 'value')
        if toroid_curr is not None:
            flags.BField.barrelToroidOn = (toroid_curr > 70) # threshold of 70 copied from RecExOnline_globalconfig.py
            flags.BField.endcapToroidOn = (toroid_curr > 70) # threshold of 70 copied from RecExOnline_globalconfig.py


def autoConfigOnlineRecoFlags(flags, partition=None):
    # Set the flags to default
    setDefaultOnlineRecoFlags(flags)
    # Try updating the flags from IS if the information is available
    trySetFlagsFromIS(flags, partition)
