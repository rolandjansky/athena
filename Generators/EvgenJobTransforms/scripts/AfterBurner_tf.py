#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#! /usr/bin/env python

"""
Run event simulation and produce an EVNT file.
"""

import os, sys, time, shutil
from PyJobTransforms.trfLogger import msg
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from EvgenJobTransforms.evgenTrfArgs import addStdEvgenArgs
import PyJobTransforms.trfArgClasses as trfArgClasses
from EvgenJobTransforms.jo_proxy import mk_jo_proxy


## Prodsys1 hack...
# TODO: Remove!
ListOfDefaultPositionalKeys=['--AMI', '--AMITag', '--DBRelease', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--ecmEnergy', '--eventAcceptanceEfficiency', '--evgenJobOpts', '--execOnly', '--firstEvent', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputGenConfFile', '--inputGeneratorFile', '--jobConfig', '--maxEvents', '--omitFileValidation', '--outputEVNTFile', '--postExec', '--postInclude', '--preExec', '--preInclude', '--randomSeed', '--reportName', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--uploadtoami', '--validation', '--outputTXTFile']


class EvgenExecutor(athenaExecutor):
    "Specialised trf executor class for event generation jobs"

    def __init__(self, skeleton):
        athenaExecutor.__init__(self, name="generate", skeletonFile=skeleton, substep="gen2evnt", tryDropAndReload=False, inData=["EVNT_Pre"], outData=["EVNT"])

    def preExecute(self, input=set(), output=set()):
        "Get input tarball, unpack and set up env if an evgenJobOpts arg was provided."

        ## First call the base class preExecute
        super(EvgenExecutor, self).preExecute(input, output)

        def expand_if_archive(filename):
            "Function to expand a file if it is a zip archive or tarball"
            if ".tar" in filename:
                import tarfile
                tf = tarfile.open(filename)
                tf.extractall()
                tf.close()
            elif filename.endswith(".zip"):
                import zipfile
                zf = zipfile.ZipFile(filename)
                zf.extractall()
                zf.close()

        ## Handle locating of evgen job options / fragments, either from a tarball or CVMFS
        if "evgenJobOpts" in self._trf.argdict: ## Use a specified JO tarball
            tarball = self._trf.argdict["evgenJobOpts"].value
            ## Prepend the standard tarball URL prefix if the arg is not a full URL
            # TODO: also allow a copy from a local filesystem location if it starts with a / (or can be found with os.path)?
            if tarball.startswith("http"):
                url = tarball
                tarball = os.basename(tarball)
            else:
                url = "http://cern.ch/atlas-computing/links/kitsDirectory/EvgenJobOpts/" + tarball
            ## Download the tarball in a Grid-safe way (if it doesn't already exist)
            if not os.path.exists(tarball):
                from EvgenJobTransforms.download import downloadUsingProxy
                status, output = downloadUsingProxy(url)
                if status != 0:
                    raise EnvironmentError('Error downloading tarball %s. Downloader reports: %s' % (tarball, output))
                msg.info('Evgen tarball download success: %s' % output)
            ## Expand tarball
            expand_if_archive(tarball)
            mk_jo_proxy(os.getcwd(), "MC14JobOptions", "_joproxy14")
            mk_jo_proxy(os.getcwd(), "MC15JobOptions", "_joproxy15")
            ## Source setup script (requires some shenanigans to update the Python env robustly)
            # TODO: trf framework now bans use of exec()...
            #import subprocess
            #pipe = subprocess.Popen(". ./setupevprod.sh; python -c 'import os; print \"newenv = %r\" % os.environ'", stdout=subprocess.PIPE, shell=True)
            #exec(pipe.communicate()[0])
            #os.environ.update(newenv)

        else: ## Use the CVMFS copy of the latest MC14 JOs tag
            if os.path.exists("/cvmfs/atlas.cern.ch"):
                # TODO: Make the package name configurable
                mk_jo_proxy("/cvmfs/atlas.cern.ch/repo/sw/Generators/MC14JobOptions/latest/", "MC14JobOptions","_joproxy14")
                mk_jo_proxy("/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/", "MC15JobOptions","_joproxy15")
                msg.info("No evgenJobOpts tarball specified, using JOBOPTSEARCHPATH = '%s'" % os.environ["JOBOPTSEARCHPATH"])
            elif os.path.exists("/afs/cern.ch/atlas/groups/Generators"):
                mk_jo_proxy("/afs/cern.ch/atlas/groups/Generators/MC14JobOptions/latest/", "MC14JobOptions","_joproxy14")
                mk_jo_proxy("/afs/cern.ch/atlas/groups/Generators/MC15JobOptions/latest/", "MC15JobOptions","_joproxy15")
                msg.info("No evgenJobOpts tarball specified, no cvmfs, using JOBOPTSEARCHPATH = '%s'" % os.environ["JOBOPTSEARCHPATH"])

        ## Expand tarball input event and generator conf files, if provided
        if "inputGeneratorFile" in self._trf.argdict:
            expand_if_archive(self._trf.argdict["inputGeneratorFile"].value)
        if "inputGenConfFile" in self._trf.argdict:
            expand_if_archive(self._trf.argdict["inputGenConfFile"].value)


def getTransform():
    exeSet = set()
    exeSet.add(EvgenExecutor("EvgenJobTransforms/skeleton.ABtoEVGEN.py"))
    exeSet.add(athenaExecutor(name = "AODtoDPD", skeletonFile = "PATJobTransforms/skeleton.AODtoDPD_tf.py",
                              substep = "a2d", inData = ["EVNT"], outData = ["NTUP_TRUTH"], perfMonFile = "ntuple_AODtoDPD.pmon.gz"))
    trf = transform(executor=exeSet)
    addAthenaArguments(trf.parser)
    addStdEvgenArgs(trf.parser)
    return trf


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    msg.info("This is %s" % sys.argv[0])
    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()
    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


# TODO: Open resulting EVNT file to extract cross-section, generator names+versions, etc. from the HepMC::GenRun or whatever... in an executor postExecute?


if __name__ == "__main__":
    main()
