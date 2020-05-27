#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# art-description: Validate in-file metadata in MT q431
# art-type: grid
# art-athena-mt: 8
#
import argparse
import json
import os
from subprocess import Popen, STDOUT
import sys

from AthenaCommon.Logging import logging
from PyUtils.MetaDiff import meta_diff


MSG = logging.getLogger('RecoMetadata_test')

STEPS = ["BStoRAW", "BStoESD", "ESDtoAOD"]

CONFIG = {
    "execOnly": True,
    "preInclude": ["mt.py"],
    "AMI": "q431",
    "inputBSFile": [
        "/eos/user/g/goetz/data/data17/data17_13TeV.00330470.physics_Main.daq.RAW.mix._0001.data",
        "/eos/user/g/goetz/data/data17/data17_13TeV.00337215.physics_Main.daq.RAW.mix._0001.data",
        "/eos/user/g/goetz/data/data18/data18_13TeV.00363979.physics_Main.daq.RAW.0250._0001.data"]
}

SERIAL = {step: CONFIG.copy() for step in STEPS}
for k, v in SERIAL.items():
    if k == "BStoRAW":
        v.update(outputDRAW_ZMUMUFile=["serial_DRAW"])
    if k == "BStoESD":
        v.update(outputESDFile=["serial_ESD.pool.root"])
    if k == "ESDtoAOD":
        v.update(
            inputESDFile=SERIAL["BStoESD"]["outputESDFile"],
            outputAODFile=["serial_AOD.pool.root"])
        del v["inputBSFile"]

THREAD = {step: CONFIG.copy() for step in STEPS}
for k, v in THREAD.items():
    v["multithreaded"] = True
    if k == "BStoRAW":
        v.update(outputDRAW_ZMUMUFile=["threaded_DRAW"])
    if k == "BStoESD":
        v.update(outputESDFile=["threaded_ESD.pool.root"])
    if k == "ESDtoAOD":
        v.update(
            inputESDFile=SERIAL["BStoESD"]["outputESDFile"],
            outputAODFile=["threaded_AOD.pool.root"])
        del v["inputBSFile"]


PRE_INCLUDE = '''\
rec.doMonitoring.set_Value_and_Lock(True);
from AthenaMonitoring.DQMonFlags import DQMonFlags;
DQMonFlags.doStreamAwareMon=False;
DQMonFlags.doMuonRawMon=False;
DQMonFlags.doMuonSegmentMon=False;
DQMonFlags.doMuonTrackMon=False;
DQMonFlags.doMuonAlignMon=False;
DQMonFlags.doMuonTrkPhysMon=False;
DQMonFlags.doMuonPhysicsMon=False;
DQMonFlags.doGlobalMon.set_Value_and_Lock(False);
DQMonFlags.doMonitoring.set_Value_and_Lock(False);

rec.doTrigger = False

# rec.doMuon = True
# rec.doMuonCombined = True
# from DiTauRec.DiTauRecFlags import jobproperties
# jobproperties.DiTauRecFlags.doDiTauRec = True

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)

if True :
    from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doFastMon.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoringFullPrint.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doFullMon.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doMallocMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doExtraPrintouts.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doHephaestusMon.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doDsoMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doPersistencyMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doDataProxySizeMonitoring.set_Value_and_Lock(False)
    pmon_properties.PerfMonFlags.doPostProcessing.set_Value_and_Lock(False)
'''


def build_transform_command(config):
    """
    build command to hand to subproccess from configuration
    """
    command = ["Reco_tf.py"]

    for key, value in config.items():
        MSG.debug("adding %s and %s to command", key, value)

        if isinstance(value, bool):
            if value:
                command.append("--{}".format(key))
        elif isinstance(value, list):
            command.append("--{}".format(key))
            command += value
        else:
            command.append("--{}".format(key))
            command.append(value)

        MSG.debug("command list: %s", command)

    return command


def serial(config, verbose=False):
    """
    Run threaded Reconstruction transform based on parameters provided in
    config parameter
    :param: config: configuration dictionary to hand to transform
    :param: threads: number of threads to use
    :param: verbose: whether to be laconic or not
    :return: transform exit code
    """
    return run_transform(config, env=os.environ, verbose=verbose)


def thread(config, threads=8, verbose=False):
    """
    Run threaded Reconstruction transform based on parameters provided in
    config parameter
    :param: config: configuration dictionary to hand to transform
    :param: threads: number of threads to use
    :param: verbose: whether to be laconic or not
    :return: transform exit code
    """
    env = os.environ.copy()
    env["ATHENA_CORE_NUMBER"] = str(threads)

    return run_transform(config, env=env, verbose=verbose)


def run_transform(config, env, verbose=False):
    """
    run Reco_tf.py with given configuration

    :return: return value of transform
    """
    # MSG.debug("job environment: %s", json.dumps(env, indent=2))
    MSG.debug("job config: %s", json.dumps(config))
    MSG.debug("job config: %s", json.dumps(config))

    command = build_transform_command(config)
    MSG.info('running: %s', ' '.join(command))

    try:
        if verbose:
            proc = Popen(command, env=env)
        else:
            with open(os.devnull, 'w') as dev_null:
                proc = Popen(command, env=env, stdout=dev_null, stderr=STDOUT)

        return proc.wait()

    except KeyboardInterrupt:
        proc.kill()
        MSG.debug('comparison cancelled')
        sys.exit(0)


def get_args():
    """
    handle command line arguments
    """
    parser = argparse.ArgumentParser(description='''\
            Run q431 reconstruction transform on the given input (default are
            some of Goetz's files on EOS). Compare the in-file metadata of the
            output files.
            ''')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='do not be laconic')
    parser.add_argument('inputBS', metavar='FILE', type=str, nargs='*',
                        help='bytestream file(s) to use as input')

    args = parser.parse_args()
    MSG.setLevel(logging.DEBUG if args.verbose else logging.INFO)

    input_files = args.inputBS if args.inputBS else CONFIG['inputBSFile']
    MSG.info("input files:")
    MSG.info(' '.join(input_files))

    for step, config in SERIAL.items():
        for key in config:
            if key == "inputBSFile":
                SERIAL[step][key] = input_files

    for step, config in THREAD.items():
        for key in config:
            if key == "inputBSFile":
                THREAD[step][key] = input_files

    return args


def main():
    """write pre-include to file, run transforms, compare results, report
    """
    args = get_args()

    for file_name in CONFIG["preInclude"]:
        with open(file_name, 'w') as handle:
            handle.write(PRE_INCLUDE)

    for step in STEPS:
        MSG.info('%s in serial mode', step)
        s_status = serial(SERIAL[step], verbose=args.verbose)
        print("art-result: {} {}_serial_transform".format(s_status, step))

        MSG.info('%s in threaded mode', step)
        t_status = thread(THREAD[step], verbose=args.verbose)
        print("art-result: {} {}_threaded_transform".format(t_status, step))

        try:
            s_file = [value for key, value in SERIAL[step].items() if "output" in key.lower()]
            s_file = s_file[0][0]

            t_file = [value for key, value in THREAD[step].items() if "output" in key.lower()]
            t_file = t_file[0][0]
        except IndexError:
            MSG.error("missing output found for steo %s", step)
            print("art-result: 1 can_read_metadata")
            print("art-result: 1 {}".format(step))
            continue

        MSG.info('Comparing metadata in %s and %s', s_file, t_file)
        try:
            diff = meta_diff([s_file, t_file], mode='full', drop=['file_guid'])

            print("art-result: 0 can_read_metadata")
            if diff:
                MSG.warning(''.join(diff))
            print("art-result: {} {}".format(0 if len(diff) < 2 else 1, step))
        except (ReferenceError, StopIteration):
            MSG.error('failed to read metadata from: %s and/or %s', s_file, t_file)
            print("art-result: 1 can_read_metadata")
            print("art-result: 1 {}".format(step))
            continue


if __name__ == "__main__":
    main()
