#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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


## Prodsys1 hack...
# TODO: Remove!
ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--checkEventCount', '--command', '--dumpJSON', '--dumpPickle', '--ecmEnergy', '--env', '--eventAcceptanceEfficiency', '--evgenJobOpts', '--execOnly', '--fileValidation', '--firstEvent', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputEVNT_PreFile', '--inputFileValidation', '--inputGenConfFile', '--inputGeneratorFile', '--jobConfig', '--jobid', '--maxEvents', '--orphanKiller', '--outputEVNTFile', '--outputEVNT_PreFile', '--outputFileValidation', '--outputNTUP_TRUTHFile', '--outputTXTFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--printEvts', '--randomSeed', '--reportName', '--reportType', '--rivetAnas', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts']

class EvgenExecutor(athenaExecutor):
    "Specialised trf executor class for event generation jobs"

    def __init__(self, name="generate", skeleton="EvgenJobTransforms/skeleton.GENtoEVGEN.py", substep=None, inData=["inNULL"], outData=["EVNT", "EVNT_Pre", "TXT"]):
        athenaExecutor.__init__(self, name=name, skeletonFile=skeleton, substep=substep, tryDropAndReload=False, inData=inData, outData=outData)

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

        def get_immediate_subdirectories(a_dir):
            return [name for name in os.listdir(a_dir)
                    if os.path.isdir(os.path.join(a_dir, name))]                

        def mk_jo_proxy(targetbasepath, pkgname, proxypath, addtosearch=True):
            "Make a JO proxy dir such that the MCxxJobOptions/dddd dirs contents are found via include(MCxxJobOptions/yyyy)"
            if proxypath:
                if os.path.exists(proxypath):
                    shutil.rmtree(proxypath)
                os.mkdir(proxypath)
            os.environ['LOCAL_INSTALL_DIR'] = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
            os.environ['LOCAL_DATA_DIR'] = (os.environ['DATAPATH']).split(":")[0]

            dirlist =  get_immediate_subdirectories(targetbasepath)
            subdirlist=dirlist;
            for dd in (dirlist):
              if (('.svn' not in dd) and ('cmt' not in dd) and ('_joproxy' not in dd)):
                deepdir = os.path.join(targetbasepath, dd)
                subdirlist1 = get_immediate_subdirectories(deepdir)
                subdirlist = subdirlist+["%s" % dd+"/%s" % item for item in subdirlist1]
            for d in (subdirlist):
                # TODO: we could _maybe_ add the appropriate share/DSIDxxxx/ dir to the path based on the jobConfig arg... too much magic?
                if (('.svn' not in d) and ('cmt' not in d) and ('_joproxy' not in d) and ('share/' not in d)):
                  dpath = os.path.join(proxypath, d)
                  if proxypath:
                           os.mkdir(dpath)

                  if ('nonStandard' in dpath):
                        dpath_ex = os.path.join(dpath,pkgname)
                        if proxypath:
                           os.mkdir(dpath_ex)
                            
                        os.symlink(os.path.join(targetbasepath, d), os.path.join(dpath_ex, "nonStandard"))
                  else :      
                        os.symlink(os.path.join(targetbasepath, d), os.path.join(dpath, pkgname))
                  if addtosearch:
                    os.environ["JOBOPTSEARCHPATH"] = dpath+":"+os.environ["JOBOPTSEARCHPATH"]
                    os.environ["DATAPATH"] =os.path.join(targetbasepath, d)+":"+os.environ["DATAPATH"]

            os.environ["JOBOPTSEARCHPATH"] = os.environ['LOCAL_INSTALL_DIR']+":"+os.environ["JOBOPTSEARCHPATH"]
            os.environ["DATAPATH"] = os.environ['LOCAL_DATA_DIR']+":"+os.environ["DATAPATH"]
        

        ## Handle locating of evgen job options / fragments, either from a tarball or CVMFS
        # read the JO used
        joparam = self._trf.argdict["jobConfig"].value
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
            if "MC14" in str(joparam):
                mk_jo_proxy(os.getcwd(), "MC14JobOptions", "_joproxy14")
            else :
                mk_jo_proxy(os.getcwd(), "MC15JobOptions", "_joproxy15")
            ## Source setup script (requires some shenanigans to update the Python env robustly)
            # TODO: trf framework now bans use of exec()...
            #import subprocess
            #pipe = subprocess.Popen(". ./setupevprod.sh; python -c 'import os; print \"newenv = %r\" % os.environ'", stdout=subprocess.PIPE, shell=True)
            #exec(pipe.communicate()[0])
            #os.environ.update(newenv)

        else: ## Use the CVMFS copy of the latest MC14 JOs tag
            sw_base =  os.popen("echo $ATLAS_SW_BASE").read()
#            print subprocess.Popen("echo Hellllo World", shell=True, stdout=subprocess.PIPE).stdout.read()
#            sw_base = print subprocess.Popen("echo $ATLAS_LOCAL_ROOT_BASE", shell=True, stdout=subprocess.PIPE)
            sw_base = sw_base.strip()
            if (sw_base == ""):
                msg.info('$ATLAS_SW_BASE not defined, trying explicite /cvmfs path')
                sw_base = '/cvmfs'
#                msg.info('sw_base path %s ' %sw_base)
            else:    
                msg.info('ATLAS_SW_BASE path: %s' %sw_base)

            cvmfs_path = os.path.join(sw_base, "atlas.cern.ch")
            
            if os.path.exists(cvmfs_path):
                # TODO: Make the package name configurable
                if "MC14" in str(joparam):
                    cvmfs_mc14 = os.path.join(cvmfs_path, "repo/sw/Generators/MC14JobOptions/latest/")
                    mk_jo_proxy(cvmfs_mc14, "MC14JobOptions","_joproxy14")
#                if "MC15" in str(joparam):
                else :
                    cvmfs_mc15 = os.path.join(cvmfs_path, "repo/sw/Generators/MC15JobOptions/latest/")
#                    msg.info('cvmfs_mc15 path: %s ' %cvmfs_mc15)
                    mk_jo_proxy(cvmfs_mc15, "MC15JobOptions","_joproxy15")
                msg.info("No evgenJobOpts tarball specified, using JOBOPTSEARCHPATH = '%s'" % os.environ["JOBOPTSEARCHPATH"])
            elif os.path.exists("/afs/cern.ch/atlas/groups/Generators"):
                if "MC14" in str(joparam):
                    mk_jo_proxy("/afs/cern.ch/atlas/groups/Generators/MC14JobOptions/latest/", "MC14JobOptions","_joproxy14")
                else :
                    mk_jo_proxy("/afs/cern.ch/atlas/groups/Generators/MC15JobOptions/latest/", "MC15JobOptions","_joproxy15")
                msg.info("No evgenJobOpts tarball specified, no cvmfs, using JOBOPTSEARCHPATH = '%s'" % os.environ["JOBOPTSEARCHPATH"])

        ## Expand tarball input event and generator conf files, if provided
        if "inputGeneratorFile" in self._trf.argdict:
            expand_if_archive(self._trf.argdict["inputGeneratorFile"].value)
        if "inputGenConfFile" in self._trf.argdict:
            expand_if_archive(self._trf.argdict["inputGenConfFile"].value)


def getTransform():
    exeSet = set()
    exeSet.add(EvgenExecutor(name="generate"))
    exeSet.add(EvgenExecutor(name="afterburn", skeleton="EvgenJobTransforms/skeleton.ABtoEVGEN.py", inData=["EVNT_Pre"], outData=["EVNT"]))
    exeSet.add(athenaExecutor(name = "AODtoDPD", skeletonFile = "PATJobTransforms/skeleton.AODtoDPD_tf.py",
                              substep = "a2d", inData = ["EVNT"], outData = ["NTUP_TRUTH"], perfMonFile = "ntuple_AODtoDPD.pmon.gz"))
    trf = transform(executor=exeSet)
    addAthenaArguments(trf.parser, maxEventsDefaultSubstep='all')
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
