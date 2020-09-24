#! /usr/bin/env python

#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
"""
# Run event generation and produce an EVNT file.
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
ListOfDefaultPositionalKeys=['--AMIConfig', '--AMITag', '--argJSON', '--asetup', '--athena', '--athenaMPMergeTargetSize', '--athenaopts', '--attempt', '--checkEventCount', '--command', '--dumpJSON', '--dumpPickle', '--ecmEnergy', '--env', '--eventAcceptanceEfficiency', '--evgenJobOpts', '--execOnly', '--fileValidation', '--firstEvent', '--ignoreErrors', '--ignoreFiles', '--ignorePatterns', '--imf', '--inputEVNT_PreFile', '--inputFileValidation', '--inputGenConfFile', '--inputGeneratorFile', '--jobConfig', '--jobid', '--maxEvents', '--orphanKiller', '--outputEVNTFile', '--outputEVNT_PreFile', '--outputFileValidation', '--outputNTUP_TRUTHFile', '--outputTXTFile', '--parallelFileValidation', '--postExec', '--postInclude', '--preExec', '--preInclude', '--printEvts', '--randomSeed', '--reportName', '--reportType', '--rivetAnas', '--runNumber', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--skipFileValidation', '--skipInputFileValidation', '--skipOutputFileValidation', '--steering', '--taskid', '--tcmalloc', '--valgrind', '--valgrindbasicopts', '--valgrindextraopts', '--lheOnly', '--localPath', '--cleanOut', '--saveList']

class EvgenExecutor(athenaExecutor):
  "Specialised trf executor class for event generation jobs"
  def __init__(self, name="generate", skeleton=None, substep=None, inData=set(), outData=set()):
        athenaExecutor.__init__(self, name=name, skeletonFile=skeleton, substep=substep, inputEventTest=False, tryDropAndReload=False, inData=inData, outData=outData)

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

        ## Handle locating of evgen job options / fragments, either from a tarball or CVMFS
        # read the JO directory
        os.environ['LOCAL_INSTALL_DIR'] = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
        CommonCvmfsDir = '/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/common'
        os.environ["JOBOPTSEARCHPATH"] = CommonCvmfsDir+":"+os.environ["JOBOPTSEARCHPATH"]
        if os.path.exists('/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/common/MadGraphControl/dat/'):
            datCvmfsDir = '/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/common/MadGraphControl/dat/'
            os.environ["DATAPATH"] = datCvmfsDir+":"+os.environ["DATAPATH"]
#        dsidparam = (self._trf.argdict["jobConfig"].value).values()[0][0]
        dsidpar = (self._trf.argdict["jobConfig"].value).values()
        dsidparam = list(dsidpar)[0][0]
        # Adding cvmfs path to JOBOPTSEARCHPATH
        BaseCvmfsPath = "/cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/"

        if len(dsidparam)==6 and dsidparam.isdigit(): #only dsid is provided, add cvmfs folder like 123xxx to JOBOPTSEARCHPATH
            Jodir = dsidparam[:3]+'xxx'
            cwdir = os.getcwd()
            cwd_ful = os.path.join(cwdir, dsidparam)
            if (os.path.isdir(cwd_ful)):
               os.environ["JOBOPTSEARCHPATH"] = cwd_ful+":"+os.environ["JOBOPTSEARCHPATH"]
               os.environ["DATAPATH"] = cwd_ful+":"+os.environ["DATAPATH"]
            else:               
               cwd_Jodir = os.path.join(cwdir,Jodir)
               cwd_Jodir_ful = os.path.join(cwd_Jodir,dsidparam)
               if (os.path.isdir(cwd_Jodir_ful)):
                  os.environ["JOBOPTSEARCHPATH"] = cwd_Jodir_ful+":"+os.environ["JOBOPTSEARCHPATH"]
                  os.environ["DATAPATH"] = cwd_Jodir_ful+":"+os.environ["DATAPATH"]
               else:
                  JoCvmfsPath = os.path.join(BaseCvmfsPath, Jodir)
                  JoCvmfsPath_ful = os.path.join(JoCvmfsPath, dsidparam)
                  os.environ["JOBOPTSEARCHPATH"] = JoCvmfsPath_ful+":"+os.environ["JOBOPTSEARCHPATH"]
                  os.environ["DATAPATH"] = JoCvmfsPath_ful+":"+os.environ["DATAPATH"]
           
            
        else:  #Suppose full path of dsid folder is provided(/afs/.../123xxx/123456), add cvmfs floder and local path(/afs/.../123xxx) to JOBOPTSEARCHPATH
            if (os.path.isdir(dsidparam)):
               os.environ["JOBOPTSEARCHPATH"] = dsidparam+":"+os.environ["JOBOPTSEARCHPATH"]
               os.environ["DATAPATH"] = dsidparam+":"+os.environ["DATAPATH"]
            else:
                msg.error("JOs not found, please check = '%s'" % dsidparam) 

                

        msg.info("Using JOBOPTSEARCHPATH = '%s'" % os.environ["JOBOPTSEARCHPATH"])
        msg.info("Using DATAPATH = '%s'" % os.environ["DATAPATH"])
                
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
            ## Source setup script (requires some shenanigans to update the Python env robustly)
            # TODO: trf framework now bans use of exec()...

        # copy config files to cwd
        FIRST_DIR = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]
        configFiles = [f for f in os.listdir(FIRST_DIR) if ( "GRID" in f)]
        if len(configFiles) == 1:
            confFile =  os.path.join(FIRST_DIR, configFiles[0])
            expand_if_archive(confFile)
            msg.info('Configuration input found: %s' % confFile)
        elif len(configFiles) >1:
            msg.error("Too many *GRID* config files, please check = '%s'" % dsidparam) 

        #Expand if a tarball is found in local directory
        loc_files = os.listdir(os.getcwd())
        for loc_file in loc_files: 
            expand_if_archive(loc_file)
            
            
        ## Expand tarball input event and generator conf files, if provided
        if "inputGeneratorFile" in self._trf.argdict:
#           expand_if_archive(self._trf.argdict["inputGeneratorFile"].value)
            myinputfiles = self._trf.argdict["inputGeneratorFile"].value
            genInputFiles = myinputfiles.split(',')
            for file in genInputFiles:
               expand_if_archive(file)
        if "inputGenConfFile" in self._trf.argdict:
            expand_if_archive(self._trf.argdict["inputGenConfFile"].value)

def move_files(main_dir,tmp_dir,whitelist):
    files = os.listdir(tmp_dir)
    files.sort()
    for f in files:
       for i in whitelist:
            if i in f:
                src = tmp_dir+"/"+f
                dest = main_dir+"/"+f
                os.rename(src,dest)
                break


def getTransform():
    exeSet = set()
    msg.info("Transform arguments %s" % sys.argv[1:])
    if "--outputEVNTFile" in str(sys.argv[1:]):
       exeSet.add(EvgenExecutor(name="generate", skeleton="EvgenJobTransforms/skel.GENtoEVGEN.py", inData=["inNULL"], outData=["EVNT", "EVNT_Pre", "TXT" ]))
       msg.info("Output EVNT file")
    elif "--outputTXTFile" in str(sys.argv[1:]):
       exeSet.add(EvgenExecutor(name="generate", skeleton="EvgenJobTransforms/skel.GENtoTXT.py", inData=["inNULL"], outData=["TXT"]))
       msg.info("Output TXT file")
    else:
       msg.error("Output cannot be recognised")

    exeSet.add(EvgenExecutor(name="afterburn", skeleton="EvgenJobTransforms/skel.ABtoEVGEN.py", inData=["EVNT_Pre"], outData=["EVNT"]))
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

    main_dir = os.getcwd()
    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    if (("cleanOut" in trf.argdict) and (trf.argdict["cleanOut"].value != 0)):
      name_tmpdir = "tmprun"
      tmp_dir = os.path.join(main_dir, name_tmpdir)
      if os.path.isdir(tmp_dir):
         shutil.rmtree(tmp_dir, ignore_errors=True)
      os.mkdir("tmprun")
      os.chdir("tmprun")
      tmp_dir = os.getcwd()
      whitelist_in = ['MC','group','TXT']
      move_files(tmp_dir,main_dir,whitelist_in)
      
    trf.execute()
    trf.generateReport()
    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))


# read files/dirs that should be saved and if present in cwd - remove

    if (("cleanOut" in trf.argdict) and (trf.argdict["cleanOut"].value!=0)):
       whitelist_out = ['log.generate','.root']
       if "outputTXTFile" in trf.argdict:
         whitelist_out.append('TXT')
       if "saveList" in trf.argdict:
         saveList_dic= trf.argdict["saveList"].value
         saveList_str= str(saveList_dic)
         saveList_str=saveList_str[10:-3]
         saveList= saveList_str.split(",")
         for item in saveList:
           test_ex = os.path.join(main_dir,str(item))
           if os.path.isdir(test_ex):
             shutil.rmtree(test_ex, ignore_errors=True)
           elif os.path.isfile(test_ex):
             os.remove(test_ex)
         if not saveList[0].isdigit():
             whitelist_out=whitelist_out+saveList 

       move_files(main_dir,tmp_dir,whitelist_out)
       os.chdir(main_dir)
       if "saveList" not in trf.argdict:
         shutil.rmtree(tmp_dir, ignore_errors=True)
       elif not saveList[0].isdigit():
         shutil.rmtree(tmp_dir, ignore_errors=True)
# if cleanOut is not defined and multipleinput preset, remove the merged file
    elif ("inputGeneratorFile" in trf.argdict):
        myinputfiles = trf.argdict["inputGeneratorFile"].value
        genInputFiles = myinputfiles.split(',')
        numberOfFiles = len(genInputFiles)
        merge_file = 'merged_lhef._0.events'
        if((numberOfFiles>1) and (os.path.exists(merge_file))):
           os.remove(merge_file)
#           
    if (("lheOnly" in trf.argdict ) and (trf.argdict["lheOnly"].value == 1)):
          outputName = ''.join(trf.argdict["outputEVNTFile"].value)
          os.remove(outputName)
    sys.exit(trf.exitCode)


# TODO: Open resulting EVNT file to extract cross-section, generator names+versions, etc. from the HepMC::GenRun or whatever... in an executor postExecute?


if __name__ == "__main__":
    main()
