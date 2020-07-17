#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Run transforms and compare in-file metadata

This helper script runs the the q431 transfor step by step.  The input
bytestream and ESD files may optionally be specified. Each step is first run
serial then in threaded mode. If both succeed the in-file metadata of the two
transform ouputs are compared. Any differences are reported. If you specify
input By default inputs are from three different runs with different
conditions. The files reside in CVMFS. You can specify any other input file
that a transform would understand.


    Example

        $ MetaDataTest.py -h

    Attributes:
        STEPS (list): The transform steps to execute,
                      e.g ["RAWtoRAW", "RAWtoESD"]
        MSG   (logger): progress of the script is reported here

    Todo:
        * Support other q-tests, e.g. q221
"""
#
# art-description: Validate in-file metadata in MT q431
# art-type: grid
# art-athena-mt: 8
#
import argparse
import copy
import json
import os
import shutil
from subprocess import Popen, STDOUT
import sys
import textwrap

from AthenaCommon.Logging import logging
from PyUtils.MetaDiff import meta_diff


MSG = logging.getLogger("MetaDataTest")

STEPS = ["RAWtoRAW", "RAWtoESD", "ESDtoAOD"]


class TestConfiguration:
    """configuration information and setup routines for metadata test
    """

    def __init__(self):
        self.options = {
            "execOnly": True,
            "outputRAWFile": "../draw.root",
            "outputESDFile": "../esd.root",
            "outputAODFile": "../aod.root",
            "conditionsTag": "CONDBR2-BLKPA-2017-10",
            "geometryVersion": "ATLAS-R2-2016-01-00-01",
        }
        self.preIncludeContent = textwrap.dedent("""\
        from AthenaCommon.JobProperties import jobproperties
        from AthenaMonitoring.DQMonFlags import DQMonFlags;
        from ParticleBuilderOptions.AODFlags import AODFlags
        from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties


        jobproperties.PrimaryDPDFlags.WriteDRAW_BCID1.RejectBCIDs = [0]

        rec.doTrigger = False
        rec.doMonitoring.set_Value_and_Lock(True);

        DQMonFlags.doStreamAwareMon=False;
        DQMonFlags.doMuonRawMon=False;
        DQMonFlags.doMuonSegmentMon=False;
        DQMonFlags.doMuonTrackMon=False;
        DQMonFlags.doMuonAlignMon=False;
        DQMonFlags.doMuonTrkPhysMon=False;
        DQMonFlags.doMuonPhysicsMon=False;
        DQMonFlags.doGlobalMon.set_Value_and_Lock(False);
        DQMonFlags.doMonitoring.set_Value_and_Lock(False);

        AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)

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
        """)

    def setupWorkDir(self):
        """write pre_include and set up directories to run tests in"""
        self._writePreInclude()

    def getCmdConfig(self, step, threaded=True):
        """build dictionary construct transform command from"""
        assert step in STEPS, "{} not one of {}".format(step, STEPS)

        common_opts = ["AMI", "preInclude", "execOnly", "conditionsTag",
                       "geometryVersion"]
        cmd_config = {
            k: self.options[k] for k in common_opts if k in self.options
        }

        if threaded:
            cmd_config["multithreaded"] = True

        if step == "ESDtoAOD":
            cmd_config["inputESDFile"] = self.options["inputESDFile"]
        else:
            cmd_config["inputBSFile"] = self.options["inputBSFile"]

        if step == "RAWtoRAW":
            cmd_config["outputDRAW_BCID1File"] = self.options["outputRAWFile"]
        elif step == "RAWtoESD":
            cmd_config["outputESDFile"] = self.options["outputESDFile"]
        elif step == "ESDtoAOD":
            cmd_config["outputAODFile"] = self.options["outputAODFile"]

        return cmd_config

    def _writePreInclude(self):
        """write pre include content to file"""
        file_name = "{}/pre_include.py".format(os.getcwd())
        try:
            self.options["preInclude"] = [file_name]
            with open(file_name, "w") as handle:
                handle.write(self.preIncludeContent)
        except IOError:
            MSG.error("failed to write {} ... aborting".format(file_name))
            self.options["preInclude"] = None
            sys.exit(1)


def getCmdEnv(threads=4):
    """add ATHENA_CORE_NUMBER to environment if needed"""
    env = os.environ.copy()
    if "ATHENA_CORE_NUMBER" not in env:
        env["ATHENA_CORE_NUMBER"] = str(threads)
    return env


def getCmdPath(step, threaded):
    """make directory to execute command in and return it"""
    parent = "thread" if threaded else "serial"
    if not os.path.exists(parent):
        os.mkdir(parent)

    path = "{}/{}".format(parent, step)
    if os.path.exists(path):
        shutil.rmtree(path)
    os.mkdir(path)

    return path


def buildTransformCommand(config):
    """build command to hand to subproccess from configuration

    Appends options supplied in config to Reco_tf.py in a list formatted
    to be handed to subprocess' run or Popen.

    Args:
        config  (dict): Key, value paris of options for Reco_tf.py

    Returns:
        list: List containing Reco_tf.py command to execute
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


def serial(config, path=None, verbose=False):
    """Run athena in inherited environmnt

    Transparently passes everything on to runTransform

    Args:
        config  (dict): Key, value paris of options for Reco_tf.py
        path    (str):  Path where to execute transform
        verbose (bool): Whether to be terse

    Returns:
        (int, str) transform exit code and output file name
    """
    return runTransform(config, path=path, env=os.environ, verbose=verbose)


def thread(config, path=None, threads=4, verbose=False):
    """Adjust environment to run athenaMT

    Add ATHENA_CORE_NUMBER to the environment if needed. Then run the transform
    in the adjusted environment

    Args:
        config  (dict): Key, value paris of options for Reco_tf.py
        path    (str):  Path where to execute transform
        threads (int):  Number of threads to use
        verbose (bool): Whether to be terse

    Returns:
        (int, str) transform exit code and output file name
    """
    env = getCmdEnv(threads=threads)

    return runTransform(config, path=path, env=env, verbose=verbose)


def runTransform(config, path=None, env=None, verbose=False):
    """run Reco_tf.py with given configuration

    Assemble command from config print it as informational message and run it.
    The transform will be executed in the directory specified by path with the
    environment given in env.

    Args:
        config  (dict): Key, value paris of options for Reco_tf.py
        path    (str):  Path where to execute transform
        env     (dict): Key, value pairs of evars for transform
        verbose (bool): whether to be terse

    Retruns:
        (int, str) return value of transform, output file name
    """
    # MSG.debug("job environment: %s", json.dumps(env, indent=2))
    MSG.debug("job config: %s", json.dumps(config))
    MSG.debug("job config: %s", json.dumps(config))

    command = buildTransformCommand(config)
    MSG.info("running: %s", " ".join(command))

    MSG.debug("in directory: %s", path)

    try:
        for key, value in config.items():
            if "output" in key.lower():
                out_file = value
        out_file = "{}/{}".format(path, out_file)
    except IndexError:
        MSG.error("no output file specified")
        return 1, ""

    try:
        if verbose:
            proc = Popen(command, cwd=path, env=env)
        else:
            with open(os.devnull, "w") as dev_null:
                proc = Popen(
                    command, cwd=path, env=env, stdout=dev_null, stderr=STDOUT
                )

        return proc.wait(), out_file

    except KeyboardInterrupt:
        proc.kill()
        MSG.debug("comparison cancelled")
        sys.exit(0)


def getArgs():
    """handle command line arguments"""
    parser = argparse.ArgumentParser(
        description="""\
            Run q431 reconstruction transform on the given input (defaults are
            single event bytestream files on CVMFS). Compare the in-file
            metadata of the output files from serial and threaded running.
            """
    )

    parser.add_argument(
        "-v", "--verbose", action="store_true", help="do not be laconic"
    )

    input_bs = [
        ("/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MetadataTests/"
         "data17/data17_13TeV.00330470.physics_Main.daq.RAW.mix._0001.data"),
        ("/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MetadataTests/"
         "data17/data17_13TeV.00337215.physics_Main.daq.RAW.mix._0001.data"),
        ("/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MetadataTests/"
         "data18/data18_13TeV.00363979.physics_Main.daq.RAW.0250._0001.data"),
    ]
    parser.add_argument(
        "--inputBSFile",
        metavar="BS",
        type=str,
        nargs="*",
        help="bytestream file(s) to use as input",
        default=input_bs,
    )

    parser.add_argument(
        "--inputESDFile",
        metavar="ESD",
        type=str,
        nargs="*",
        help="bytestream file(s) to use as input",
        # default=input_esds,
    )

    parser.add_argument(
        "--AMI",
        metavar="TAG",
        type=str,
        nargs=1,
        help="transform to run (e.g. q431)",
        default="q431",
    )

    args = parser.parse_args()
    MSG.setLevel(logging.DEBUG if args.verbose else logging.INFO)

    return args


def makeConfig(args):
    """build job configuration"""
    config = TestConfiguration()

    config.options["AMI"] = args.AMI
    config.options["inputBSFile"] = args.inputBSFile

    MSG.debug("input bytestream files:")
    MSG.debug(" ".join(config.options["inputBSFile"]))

    config.setupWorkDir()

    if args.inputESDFile:
        config.options["inputESDFile"] = args.inputESDFile
    else:
        MSG.info("generating ESDtoAOD input")
        for input_config in makeInputConfig(config):
            status, esd_file = serial(
                input_config.getCmdConfig("RAWtoESD", threaded=False),
                path=os.getcwd(),
            )
            if status == 0:
                MSG.debug("Generated ESDtoAOD input %s", esd_file)
                try:
                    config.options["inputESDFile"] += [esd_file]
                except KeyError:
                    config.options["inputESDFile"] = [esd_file]

    try:
        MSG.debug("input ESD files:")
        MSG.debug(" ".join(config.options["inputESDFile"]))
    except KeyError:
        MSG.info("no input for ESDtoAOD avialable")
        STEPS.remove('ESDtoAOD')

    return config


def makeInputConfig(test_config):
    """build configuration to generate ESD inputs from test configuration"""
    result = []
    for input_file in test_config.options["inputBSFile"]:
        input_config = copy.deepcopy(test_config)
        input_config.options["inputBSFile"] = [input_file]
        input_config.options["outputESDFile"] = None
        # TODO: make this less fugly
        output_file = os.path.basename(input_file)
        array = output_file.split('.')
        array[-1] = 'pool.root'
        output_file = '.'.join(array)
        output_file = output_file.replace('RAW', 'ESD')
        input_config.options["outputESDFile"] = output_file
        result += [input_config]
    return result


def compareMetadata(step, target, reference):
    """Compare metadata in target to reference

    Compare metadata in target file produce in transform step to reference
    specified in job configuration. Report the result.

    Args:
        step      (str):  The transform step to run, e.g. ESDtoAOD
        target    (str):  File to compare to reference
        reference (str):  Reference file
    """
    try:
        diff = meta_diff(
            [target, reference], mode="full", drop=["file_guid", "file_size"],
            diff_format="diff"
        )

        if diff:
            MSG.info("".join(diff))

        print("art-result: {} {}".format(1 if diff else 0, step))

    except (ReferenceError, StopIteration):
        MSG.error(
            "failed to read metadata from: %s and/or %s", target, reference
        )
        print("art-result: 1 {}".format(step))


def runTest(config, step, verbose):
    """Run a transform step and compare metadata of the output

    Run the transform step as set up in the config, either in serial or threaded
    mode. If the transorm reports success compare the metadata of the resulting
    file to the supplied referenc files.

    Args:
        config    (TestConfiguration): The config object set up for the test
        step      (str):               The transform step to run, e.g. ESDtoAOD
        verbose   (bool):              Whether to print out debug info
    """
    MSG.info("%s in serial mode", step)
    serial_status, serial_file = serial(
        config.getCmdConfig(step, threaded=False),
        path=getCmdPath(step=step, threaded=False),
        verbose=verbose,
    )

    if not serial_status == 0:
        print("art-result: {} {}_transform".format(serial_status, step))
        MSG.info(("serial transform failed ... skipping threaded transform "
                  "and test"))
        return

    MSG.info("%s in threaded mode", step)
    thread_status, thread_file = thread(
        config.getCmdConfig(step, threaded=True),
        path=getCmdPath(step=step, threaded=True),
        verbose=verbose,
    )

    print("art-result: {} {}_transform".format(thread_status, step))

    if thread_status == 0:
        compareMetadata(step, thread_file, serial_file)
    else:
        MSG.info("transform failed ... skipping metadata comparison")


def main():
    """fetch arguments, set up for the tests, then run the requested tests."""
    args = getArgs()
    config = makeConfig(args)

    for step in STEPS:
        runTest(
            config=config, step=step, verbose=args.verbose
        )


if __name__ == "__main__":
    main()
