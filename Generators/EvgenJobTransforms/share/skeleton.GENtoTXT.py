#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
"""Functionality core of the Generate_tf transform"""

##==============================================================
## Basic configuration
##==============================================================

## Create sequences for generators, clean-up algs, filters and analyses
## and import standard framework objects with standard local scope names
import ast
import os, re, string, subprocess
import AthenaCommon.AlgSequence as acas
import AthenaCommon.AppMgr as acam
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AthenaCommonFlags import jobproperties
theApp = acam.theApp
acam.athMasterSeq += acas.AlgSequence("EvgenGenSeq")
genSeq = acam.athMasterSeq.EvgenGenSeq
acam.athMasterSeq += acas.AlgSequence("EvgenFixSeq")
fixSeq = acam.athMasterSeq.EvgenFixSeq
acam.athMasterSeq += acas.AlgSequence("EvgenPreFilterSeq")
prefiltSeq = acam.athMasterSeq.EvgenPreFilterSeq
acam.athFilterSeq += acas.AlgSequence("EvgenTestSeq")
testSeq = acam.athFilterSeq.EvgenTestSeq
## NOTE: LogicalExpressionFilter is an algorithm, not a sequence
from EvgenProdTools.LogicalExpressionFilter import LogicalExpressionFilter
acam.athFilterSeq += LogicalExpressionFilter("EvgenFilterSeq")
filtSeq = acam.athFilterSeq.EvgenFilterSeq
topSeq = acas.AlgSequence()
anaSeq = topSeq
topSeq += acas.AlgSequence("EvgenPostSeq")
postSeq = topSeq.EvgenPostSeq
##==============================================================
## Configure standard Athena services
##==============================================================

## Special setup for event generation
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")
include("PartPropSvc/PartPropSvc.py")

## Run performance monitoring (memory logging)
from PerfMonComps.PerfMonFlags import jobproperties as perfmonjp
perfmonjp.PerfMonFlags.doMonitoring = True
perfmonjp.PerfMonFlags.doSemiDetailedMonitoring = True

## Jobs should stop if an include fails.
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError = False

## Set up a standard logger
from AthenaCommon.Logging import logging
evgenLog = logging.getLogger('Generate_tf_txt')


##==============================================================
## Run arg handling
##==============================================================

## Announce arg checking
evgenLog.debug("****************** CHECKING EVENT GENERATION ARGS *****************")
evgenLog.debug(str(runArgs))
print ("****************** CHECKING EVENT GENERATION ARGS *****************")

## Announce start of job configuration
evgenLog.debug("****************** CONFIGURING EVENT GENERATION *****************")
print ("****************** CONFIGURING EVENT GENERATION *****************")
## Functions for operating on generator names
## NOTE: evgenConfig, topSeq, svcMgr, theApp, etc. should NOT be explicitly re-imported in JOs
from EvgenJobTransforms.EvgenConfig import evgenConfig
from EvgenJobTransforms.EvgenConfig import gens_known, gens_lhef, gen_sortkey, gens_testhepmc, gens_notune, gen_require_steering


## Ensure that an output name has been given
# TODO: Allow generation without writing an output file (if outputEVNTFile is None)?
#if not hasattr(runArgs, "outputEVNTFile") and not hasattr(runArgs, "outputEVNT_PreFile"):
#    raise RuntimeError("No output evgen EVNT or EVNT_Pre file provided.")

##==============================================================
## Configure standard Athena and evgen services
##==============================================================

## Announce start of job configuration
evgenLog.debug("****************** CONFIGURING MATRIX ELEMENT GENERATION *****************")
print ("****************** CONFIGURING MATRIX ELEMENT GENERATION *****************")
evgenLog.debug("****************** CONFIGURING EVENT GENERATION *****************")

## Functions for operating on generator names
## NOTE: evgenConfig, topSeq, svcMgr, theApp, etc. should NOT be explicitly re-imported in JOs
from EvgenJobTransforms.EvgenConfig import evgenConfig
from EvgenJobTransforms.EvgenConfig import gens_known, gens_lhef, gen_sortkey, gens_testhepmc, gens_notune, gen_require_steering

## Configure the event counting (AFTER all filters)
# TODO: Rewrite in Python?
from EvgenProdTools.EvgenProdToolsConf import CountHepMC
svcMgr.EventSelector.FirstEvent = runArgs.firstEvent
theApp.EvtMax = -1

#evgenConfig.minevents = 1
if not hasattr(postSeq, "CountHepMC"):
    postSeq += CountHepMC()

postSeq.CountHepMC.FirstEvent = runArgs.firstEvent
postSeq.CountHepMC.CorrectHepMC = True
postSeq.CountHepMC.CorrectEventID = True

from EvgenProdTools.EvgenProdToolsConf import TestLHE
if not hasattr(testSeq, "TestLHE")
    testSeq += TestLHE()

##==============================================================
## Pre- and main config parsing
##==============================================================

## Announce JO loading
evgenLog.debug("****************** LOADING PRE-INCLUDES AND JOB CONFIG *****************")
print ("****************** LOADING PRE-INCLUDES AND JOB CONFIG *****************")

## Pre-include
if hasattr(runArgs, "preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

## Pre-exec
if hasattr(runArgs, "preExec"):
    evgenLog.info("Transform pre-exec")
    for cmd in runArgs.preExec:
        evgenLog.info(cmd)
        exec(cmd)

# TODO: Explain!!!
def OutputTXTFile():
    outputTXTFile = None
    if hasattr(runArgs,"outputTXTFile"): outputTXTFile=runArgs.outputTXTFile
    return outputTXTFile

## Main job option include
## Only permit one jobConfig argument for evgen: does more than one _ever_ make sense?
if len(runArgs.jobConfig) != 1:
    evgenLog.error("You must supply one and only one jobConfig file argument")
    sys.exit(1)
jo = runArgs.jobConfig[0]
print jo
jofile = os.path.basename(jo)
joparts = jofile.split(".")
## Perform some consistency checks if this appears to be an "official" production JO
officialJO = False
if joparts[0].startswith("MC") and all(c in string.digits for c in joparts[0][2:]):
    officialJO = True
    ## Check that the JO does not appear to be an old one, since we can't use those
    if int(joparts[0][2:]) < 14:
        evgenLog.error("MC14 (or later) job option scripts are needed to work with Generate_tf!")
        evgenLog.error(jo + " will not be processed: please rename or otherwise update to a >= MC14 JO.")
        sys.exit(1)
    ## Check that there are exactly 4 name parts separated by '.': MCxx, DSID, physicsShort, .py
    if len(joparts) != 4:
        evgenLog.error(jofile + " name format is wrong: must be of the form MC<xx>.<yyyyyy>.<physicsShort>.py: please rename.")
        sys.exit(1)
    ## Check the DSID part of the name
    jo_dsidpart = joparts[1]
    try:
        jo_dsidpart = int(jo_dsidpart)
        if runArgs.runNumber != jo_dsidpart:
            raise Exception()
    except:
        evgenLog.error("Expected dataset ID part of JO name to be '%s', but found '%s'" % 
(str(runArgs.runNumber), jo_dsidpart))
        sys.exit(1)
    ## Check the length limit on the physicsShort portion of the filename
    jo_physshortpart = joparts[2]
    if len(jo_physshortpart) > 60:
        evgenLog.error(jofile + " contains a physicsShort field of more than 60 characters: please rename.")
        sys.exit(1)
    ## There must be at least 2 physicsShort sub-parts separated by '_': gens, (tune)+PDF, and process
    jo_physshortparts = jo_physshortpart.split("_")
    if len(jo_physshortparts) < 2:
        evgenLog.error(jofile + " has too few physicsShort fields separated by '_': should contain <generators>(_<tune+PDF_if_available>)_<process>. Please rename.")
        sys.exit(1)
    ## NOTE: a further check on physicsShort consistency is done below, after fragment loading

## Include the JO fragment
include(jo)
include('EvgenJobTransforms/LHEonly.py')

##==============================================================
## Config validation and propagation to services, generators, etc.
##==============================================================

## Announce start of JO checking
evgenLog.debug("****************** CHECKING EVGEN CONFIGURATION *****************")
print ("****************** CHECKING EVGEN CONFIGURATION *****************")

## Print out options
for opt in str(evgenConfig).split(os.linesep):
    evgenLog.info(opt)
evgenLog.info(".transform =               Generate_tf_txt")

## Sort and check generator name / JO name consistency
##
## Check that the generators list is not empty:
if not evgenConfig.generators:
    evgenLog.error("No entries in evgenConfig.generators: invalid configuration, please check your JO")
    sys.exit(1)
## Check for duplicates:
if len(evgenConfig.generators) > len(set(evgenConfig.generators)):
    evgenLog.error("Duplicate entries in evgenConfig.generators: invalid configuration, please check your JO")
    sys.exit(1)
## Sort the list of generator names into standard form
gennames = sorted(evgenConfig.generators, key=gen_sortkey)
## Check that the actual generators, tune, and main PDF are consistent with the JO name
if joparts[0].startswith("MC"): #< if this is an "official" JO
    genpart = jo_physshortparts[0]
#    genpart = genpart.replace("Py8", "Pythia8").replace("MG","MadGraph").replace("Ph","Powheg").replace("Hpp",Herwigpp").replace("H7",Herwig7").replace("Sh","Sherpa").replace("Ag","Alpgen").replace("Py","Pythia").replace("EG","EvtGen").replace("PG","ParticleGun")
    expectedgenpart = ''.join(gennames)
    ## We want to record that HERWIG was used in metadata, but in the JO naming we just use a "Herwig" label
    expectedgenpart = expectedgenpart.replace("HerwigJimmy", "Herwig")
    def _norm(s):
        # TODO: add EvtGen to this normalization for MC14?
        return s.replace("Photospp", "").replace("Photos", "").replace("TauolaPP", "").replace("Tauolapp", "").replace("Tauola", "")
    def _norm2(s):
        return s.replace("Py", "Pythia").replace("MG","MadGraph").replace("Ph","Powheg").replace("Hpp","Herwigpp").replace("H7","Herwig7").replace("Sh","Sherpa").replace("Ag","Alpgen").replace("EG","EvtGen").replace("PG","ParticleGun").replace("Gva","Geneva")
        
    def _short2(s):
         return s.replace("Pythia","Py").replace("MadGraph","MG").replace("Powheg","Ph").replace("Herwigpp","Hpp").replace("Herwig7","H7").replace("Sherpa","Sh").replace("Alpgen","Ag").replace("EvtGen","EG").replace("PG","ParticleGun").replace("Geneva","Gva")
     
#    if genpart != expectedgenpart and _norm(genpart) != _norm(expectedgenpart) and _norm2(genpart) != expectedgenpart and _norm2(genpart) != _norm(expectedgenpart):
#        evgenLog.error("Expected first part of JO name to be '%s' or '%s' or '%s', but found '%s'" % (_norm(expectedgenpart), expectedgenpart, _short2(expectedgenpart), genpart))
#        evgenLog.error("gennames '%s' " %(expectedgenpart))
#        sys.exit(1)

    if genpart != _norm(expectedgenpart)  and _norm2(genpart) != _norm(expectedgenpart):
        evgenLog.error("Expected first part of JO name to be '%s' or '%s', but found '%s'" % (_norm(expectedgenpart), _norm(_short2(expectedgenpart)), genpart))
        evgenLog.error("gennames '%s' " %(expectedgenpart))
        sys.exit(1)

    del _norm
    ## Check if the tune/PDF part is needed, and if so whether it's present
    if not gens_notune(gennames) and len(jo_physshortparts) < 3:
        evgenLog.error(jofile + " with generators " + expectedgenpart +
                       " has too few physicsShort fields separated by '_'." +
                       " It should contain <generators>_<tune+PDF_<process>. Please rename.")
        sys.exit(1)

## Check the "--steering=afterburn" command line argument has been set if EvtGen is in the JO name
# Dont't have access to steering flag so check it's effect on output files
if gen_require_steering(gennames):
    if hasattr(runArgs, "outputEVNTFile") and not hasattr(runArgs, "outputEVNT_PreFile"):
        raise RuntimeError("'EvtGen' found in job options name, please set '--steering=afterburn'")


## Check that the evgenConfig.minevents setting is acceptable
## minevents defines the production event sizes and must be sufficiently "round"    
rounding = 0
if hasattr(runArgs,'inputGeneratorFile') and ',' in runArgs.inputGeneratorFile:   multiInput = runArgs.inputGeneratorFile.count(',')+1
else:
   multiInput = 0
   
if evgenConfig.minevents < 0:
    raise RunTimeError("evgenConfig.minevents must be at least 0")
else:
    allowed_minevents_lt1000 = [0, 1, 2, 5, 10, 20, 25, 50, 100, 200, 500, 1000]
    msg = "evgenConfig.minevents = %d: " % evgenConfig.minevents
# introduced due to PRODSYS-788, commented out on 06.07.18 obo Dominic
#    if multiInput !=0 :
#        dummy_minevents = evgenConfig.minevents*(multiInput)
#        evgenLog.info('Replacing input minevents '+str(evgenConfig.minevents)+' with calculated '+str(dummy_minevents))
#        evgenConfig.minevents = dummy_minevents

    if evgenConfig.minevents >= 1000 and evgenConfig.minevents % 1000 != 0:
# introduced due to PRODSYS-788, commented out on 06.07.18 obo Dominic
#        rest1000 = evgenConfig.minevents % 1000
#        if multiInput !=0 :
#            rounding=1
#            if rest1000 < 1000-rest1000:
#                evgenLog.info('Replacing minevents '+str(evgenConfig.minevents)+' with roundeded '+str(evgenConfig.minevents-rest1000))
#                evgenConfig.minevents = evgenConfig.minevents-rest1000
#            else:
#                evgenLog.info('Replacing input minevents '+str(evgenConfig.minevents)+' with calculated '+str(evgenConfig.minevents-rest1000+1000))
#                evgenConfig.minevents = evgenConfig.minevents-rest1000+1000
#        else:    
           msg += "minevents in range >= 1000 must be a multiple of 1000"
           raise RuntimeError(msg)
    elif evgenConfig.minevents < 1000 and evgenConfig.minevents not in allowed_minevents_lt1000:
# introduced due to PRODSYS-788, commented out on 06.07.18 obo Dominic
#        if multiInput !=0:
#           rounding=1
#           round_minevents=min(allowed_minevents_lt1000,key=lambda x:abs(x-evgenConfig.minevents))
#           evgenLog.info('Replacing minevents lt 1000 '+str(evgenConfig.minevents)+' with rounded '+str(round_minevents))
#           evgenConfig.minevents=round_minevents
#        else:
           msg += "minevents in range <= 1000 must be one of %s" % allowed_minevents_lt1000
           raise RuntimeError(msg)
#    else:
#    postSeq.CountHepMC.RequestedOutput = evgenConfig.minevents if runArgs.maxEvents == -1 or rounding==1 else runArgs.maxEvents
    postSeq.CountHepMC.RequestedOutput = evgenConfig.minevents if runArgs.maxEvents == -1  else runArgs.maxEvents
    evgenLog.info('Requested output events '+str(postSeq.CountHepMC.RequestedOutput))

## Check that the keywords list is not empty:
if not evgenConfig.keywords:
    evgenLog.warning("No entries in evgenConfig.keywords: invalid configuration, please check your JO !!")    

## Check that the keywords are in the list of allowed words (and exit if processing an official JO)
if evgenConfig.keywords:
    ## Get the allowed keywords file from the JO package if possibe
    # TODO: Make the package name configurable
    kwfile = "MC15JobOptions/evgenkeywords.txt"
    kwpath = None
    for p in os.environ["JOBOPTSEARCHPATH"].split(":"):
        kwpath = os.path.join(p, kwfile)
        if os.path.exists(kwpath):
            break
        kwpath = None
    ## Load the allowed keywords from the file
    allowed_keywords = []
    if kwpath:
        kwf = open(kwpath, "r")
        for l in kwf:
            allowed_keywords += l.strip().lower().split()
#        ## add also categories if in a separate file
#        kwfile1 = "MC15JobOptions/CategoryList.txt"
#        kwpath1 = None
#        for p in os.environ["JOBOPTSEARCHPATH"].split(":"):
#            kwpath1 = os.path.join(p, kwfile1)
#            if os.path.exists(kwpath1):
#               break
#            kwpath1 = None
#    ## Continue loading the categories to allowed keywords list
#            if kwpath1:
#               kwf1 = open(kwpath1, "r")
#               for l in kwf1:
#                  allowed_keywords += l.strip().lower().split()        
        #allowed_keywords.sort()
        ## Check the JO keywords against the allowed ones
        evil_keywords = []
        ll1 = 0
        ll2 = 0
        for k in evgenConfig.keywords:
            if k.lower() not in allowed_keywords:
                evil_keywords.append(k)
#            if "l1." in k.lower():
#                ll1 +=1
#            if "l2." in k.lower():    
#                ll2 +=1        
        if evil_keywords:
            msg = "evgenConfig.keywords contains non-standard keywords: %s. " % ", ".join(evil_keywords)
            msg += "Please check the allowed keywords list and fix."
            evgenLog.error(msg)
            if officialJO:
                sys.exit(1)
#        if ll1 == 0:
#            msg = "evgenConfig.keywords do not contain L1. keyword"
#            evgenLog.warning(msg)
#        if ll2 == 0:
#            msg = "evgenConfig.keywords do not contain L2. keyword"
#            evgenLog.warning(msg) 
    else:
        evgenLog.warning("Could not find evgenkeywords.txt file %s in $JOBOPTSEARCHPATH" % kwfile)

## Check that the categories list is not empty:
if not evgenConfig.categories:
    evgenLog.warning("No entries in evgenConfig.categories: invalid configuration, please check your JO !!")        

## Check that the L1 and L2 keywords pairs are in the list of allowed words pairs (and exit if processing an official JO)
if evgenConfig.categories:
    ## Get the allowed categories file from the JO package if possibe
    # TODO: Make the package name configurable
    lkwfile = "MC15JobOptions/CategoryList.txt"
    lkwpath = None
    for p in os.environ["JOBOPTSEARCHPATH"].split(":"):
        lkwpath = os.path.join(p, lkwfile)
        if os.path.exists(lkwpath):
            break
        lkwpath = None
## Load the allowed categories names from the file
    allowed_cat = []
    if lkwpath:
        with open(lkwpath, 'r') as catlist:
            for line in catlist:
               allowed_list = ast.literal_eval(line)
               allowed_cat.append(allowed_list)
# Print allowed categories and categories read from the JOs
#            print "allowed categories", allowed_list
#            print "evgenConfig.categories", evgenConfig.categories

        ## Check the JO categories against the allowed ones
        bad_cat =[]
        it = iter(evgenConfig.categories)
        for x in it:
           l1 = x
           l2 = next(it)
           if "L1:" in l2 and "L2:" in l1:
               l1, l2 = l2, l1
           print "first",l1,"second",l2
           bad_cat.extend([l1, l2])
           for a1,a2 in allowed_cat:
#               print "a1 ",a1,"l1 ",l1, "a2 ",a2,"l2 ",l2
               if l1.strip().lower()==a1.strip().lower() and l2.strip().lower()==a2.strip().lower():
                 bad_cat=[]
           if bad_cat:
               msg = "evgenConfig.categories contains non-standard category: %s. " % ", ".join(bad_cat)
               msg += "Please check the allowed categories list and fix."
               evgenLog.error(msg)
               if officialJO:
                   sys.exit(1)
    else:
        evgenLog.warning("Could not find CategoryList.txt file %s in $JOBOPTSEARCHPATH" % lkwfile)

## Configure and schedule jet finding algorithms
## NOTE: This generates algorithms for jet containers defined in the user's JO fragment
if evgenConfig.findJets:
    include("EvgenJobTransforms/Generate_TruthJets.py")

## Propagate energy argument to the generators
# TODO: Standardise energy setting in the GenModule interface
include("EvgenJobTransforms/Generate_ecmenergies.py")

## Process random seed arg and pass to generators
#include("EvgenJobTransforms/Generate_randomseeds.py")


##==============================================================
## Handling of a post-include/exec args at the end of standard configuration
##==============================================================

if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

if hasattr(runArgs, "postExec"):
    evgenLog.info("Transform post-exec")
    for cmd in runArgs.postExec:
        evgenLog.info(cmd)
        exec(cmd)


##==============================================================
## Input file arg handling
##==============================================================

## Announce start of input file handling
evgenLog.debug("****************** HANDLING EVGEN INPUT FILES *****************")
print("****************** HANDLING EVGEN INPUT FILES *****************")
## Dat files
datFile = None
if "McAtNlo" in evgenConfig.generators and "Herwig" in evgenConfig.generators:
    datFile = "inparmMcAtNlo.dat"
elif "Alpgen" in evgenConfig.generators:
    datFile = "inparmAlpGen.dat"
elif "Protos" in evgenConfig.generators:
    datFile = "protos.dat"
elif "ProtosLHEF" in evgenConfig.generators:
    datFile = "protoslhef.dat"
elif "AcerMC" in evgenConfig.generators:
    datFile = "inparmAcerMC.dat"
elif "CompHep" in evgenConfig.generators:
    datFile = "inparmCompHep.dat"

#if hasattr(runArgs,"outputTXTFile"): outputTXTFile=runArgs.outputTXTFile
## Events files
eventsFile = None
if "Alpgen" in evgenConfig.generators:
    eventsFile = "alpgen.unw_events"
elif "Protos" in evgenConfig.generators: 
    eventsFile = "protos.events"
elif "ProtosLHEF" in evgenConfig.generators:
    eventsFile = "protoslhef.events"
elif "BeamHaloGenerator" in evgenConfig.generators:
    eventsFile = "beamhalogen.events"
elif "HepMCAscii" in evgenConfig.generators:
    eventsFile = "events.hepmc"
elif gens_lhef(evgenConfig.generators):
    #eventsFile = outputTXTFile
    eventsFile = "events.lhe"


## Helper functions for input file handling
def find_unique_file(pattern):
    "Return a matching file, provided it is unique"
    import glob
    files = glob.glob(pattern)
    ## Check that there is exactly 1 match
    if not files:
        raise RuntimeError("No '%s' file found" % pattern)
    elif len(files) > 1:
        raise RuntimeError("More than one '%s' file found" % pattern)
    return files[0]

# This function merges a list of input LHE file to make one outputFile.  The header is taken from the first
# file, but the number of events is updated to equal the total number of events in all the input files
def merge_lhe_files(listOfFiles,outputFile):
    if(os.path.exists(outputFile)):
      print "outputFile ",outputFile," already exists.  Will rename to ",outputFile,".OLD"
      os.rename(outputFile,outputFile+".OLD")
    output = open(outputFile,'w')
    holdHeader = ""
    nevents=0
    for file in listOfFiles:
       cmd = "grep /event "+file+" | wc -l"
       nevents+=int(subprocess.check_output(cmd,stderr=subprocess.STDOUT,shell=True))

    for file in listOfFiles:
       inHeader = True
       header = ""
       print "*** Starting file ",file
       for line in open(file,"r"):
##        Reading first event signals that we are done with all the header information
##        Using this approach means the script will properly handle any metadata stored
##        at the beginning of the file.  Note:  aside from the number of events, no metadata
##        is updated after the first header is read (eg the random number seed recorded will be
##        that of the first file.
          if("<event" in line and inHeader):
             inHeader = False
             if(len(holdHeader)<1):
                holdHeader = header
                output.write(header)
             output.write(line)
##        each input file ends with "</LesHouchesEvents>".  We don't want to write this out until all
##        the files have been read.  The elif below writes out all the events.
          elif(not inHeader and not ("</LesHouchesEvents>" in line)):
              output.write(line)
          if(inHeader):
##           Format for storing number of events different in MG and Powheg 
             if("nevents" in line):
##              MG5 format is "n = nevents"
                tmp = line.split("=")
                line = line.replace(tmp[0],str(nevents))
             elif("numevts" in line):
##              Powheg format is numevts n
                tmp = line.split(" ")
                nnn = str(nevents)
                line = line.replace(tmp[1],nnn)
             header+=line
    output.write("</LesHouchesEvents>\n")
    output.close()


def mk_symlink(srcfile, dstfile):
    "Make a symlink safely"
    if dstfile:
        if os.path.exists(dstfile) and not os.path.samefile(dstfile, srcfile):
            os.remove(dstfile)
        if not os.path.exists(dstfile):
            evgenLog.info("Symlinking %s to %s" % (srcfile, dstfile))
            print "Symlinking %s to %s" % (srcfile, dstfile)
            os.symlink(srcfile, dstfile)
        else:
            evgenLog.debug("Symlinking: %s is already the same as %s" % (dstfile, srcfile))

## Find and symlink dat and event files, so they are available via the name expected by the generator
if eventsFile or datFile:
    if not hasattr(runArgs, "inputGeneratorFile") or runArgs.inputGeneratorFile == "NONE":
        raise RuntimeError("%s needs input file (argument inputGeneratorFile)" % runArgs.jobConfig)
    if evgenConfig.inputfilecheck and not re.search(evgenConfig.inputfilecheck, runArgs.inputGeneratorFile):
        raise RuntimeError("inputGeneratorFile=%s is incompatible with inputfilecheck '%s' in %s" %
                           (runArgs.inputGeneratorFile, evgenConfig.inputfilecheck, runArgs.jobConfig))
#    inputroot = os.path.basename(runArgs.inputGeneratorFile).split("._")[0]
    if datFile:
      if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
        inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
      else:  
        inputroot = os.path.basename(runArgs.inputGeneratorFile).split("._")[0]

      realDatFile = find_unique_file('*%s*.dat' % inputroot)
      mk_symlink(realDatFile, datFile)
    if eventsFile:
#        realEventsFile = find_unique_file('*%s.*.ev*ts' % inputroot)
#        mk_symlink(realEventsFile, eventsFile)
        myinputfiles = runArgs.inputGeneratorFile
        genInputFiles = myinputfiles.split(',')
        numberOfFiles = len(genInputFiles)
        # if there is a single file, make a symlink.  If multiple files, merge them into one output eventsFile
        if(numberOfFiles<2):
           if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
             inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
           else:  
             inputroot = os.path.basename(runArgs.inputGeneratorFile).split("._")[0]

           if "events" in inputroot :
               inputroot = inputroot.replace(".events","")
           realEventsFile = find_unique_file('*%s.*ev*ts' % inputroot)
           mk_symlink(realEventsFile, eventsFile)
        else:
           allFiles = []
           for file in genInputFiles:
#             Since we can have multiple files from the same task, inputroot must include more of the filename
#             to make it unique
              if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
                inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
              else:  
                input0 = os.path.basename(file).split("._")[0]
                input1 = (os.path.basename(file).split("._")[1]).split(".")[0]
                inputroot = input0+"._"+input1
              print "inputroot ",inputroot
              realEventsFile = find_unique_file('*%s.*ev*ts' % inputroot)
#             The only input format where merging is permitted is LHE
              with open(realEventsFile, 'r') as f:
                 first_line = f.readline()
                 if(not ("LesHouche" in first_line)):
                    raise RuntimeError("%s is NOT a LesHouche file" % realEventsFile)
                 allFiles.append(realEventsFile)
           merge_lhe_files(allFiles,eventsFile)

else:
    if hasattr(runArgs, "inputGeneratorFile") and runArgs.inputGeneratorFile != "NONE":
        raise RuntimeError("inputGeneratorFile arg specified for %s, but generators %s do not require an input file" %
                           (runArgs.jobConfig, str(gennames)))
    if evgenConfig.inputfilecheck:
        raise RuntimeError("evgenConfig.inputfilecheck specified in %s, but generators %s do not require an input file" %
                           (runArgs.jobConfig, str(gennames)))

## Check conf files, as above but for a different command line arg, and with omission allowed
if hasattr(runArgs, "inputGenConfFile") and runArgs.inputGenConfFile != "NONE":
    if evgenConfig.inputconfcheck and not re.search(evgenConfig.inputconfcheck, runArgs.inputGenConfFile):
        raise RuntimeError("inputGenConfFile=%s is incompatible with inputconfcheck (%s) in %s" %
                           (runArgs.inputGenConfFile, evgenConfig.inputconfcheck, runArgs.jobConfig))

## Do the aux-file copying
if evgenConfig.auxfiles:
    from PyJobTransformsCore.trfutil import get_files
    get_files(evgenConfig.auxfiles, keepDir=False, errorIfNotFound=True)


##==============================================================
## Write out metadata for reporting to AMI
##==============================================================

def _checkattr(attr, required=False):
    if not hasattr(evgenConfig, attr) or not getattr(evgenConfig, attr):
        msg = "evgenConfig attribute '%s' not found." % attr
        if required:
            raise RuntimeError("Required " + msg)
        return False
    return True

if _checkattr("description", required=True):
    msg = evgenConfig.description
    if _checkattr("notes"):
        msg += " " + evgenConfig.notes
    print "MetaData: %s = %s" % ("physicsComment", msg)
if _checkattr("generators", required=True):
#    print "MetaData: %s = %s" % ("generatorName", "+".join(gennames))
    gennamesvers=[]
    for item in gennames:
       genera = item.upper()
       generat = genera+"VER"
       if (generat in os.environ):
           gennamesvers.append(item+"(v."+os.environ[generat]+")")
#           gennamesvers.append(item+"."+os.environ[generat])
       else:
           gennamesvers.append(item)
    print "MetaData: %s = %s" % ("generatorName", "+".join(gennamesvers))    
if _checkattr("process"):
    print "MetaData: %s = %s" % ("physicsProcess", evgenConfig.process)
if _checkattr("tune"):
    print "MetaData: %s = %s" % ("generatorTune", evgenConfig.tune)
if _checkattr("hardPDF"):
    print "MetaData: %s = %s" % ("hardPDF", evgenConfig.hardPDF)
if _checkattr("softPDF"):
    print "MetaData: %s = %s" % ("softPDF", evgenConfig.softPDF)
if _checkattr("keywords"):
    print "MetaData: %s = %s" % ("keywords", ", ".join(evgenConfig.keywords).lower() ),
if _checkattr("categories"):
    print ( ", " + ", ".join(evgenConfig.categories))
else:
    print (" ")

#if _checkattr("categories"):
#    print "MetaData: %s = %s" % ("categories", ", ".join(evgenConfig.categories))
if _checkattr("specialConfig"):
   print "MetaData: %s = %s" % ("specialConfig", evgenConfig.specialConfig)
# TODO: Require that a contact / JO author is always set
if _checkattr("contact"):
    print "MetaData: %s = %s" % ("contactPhysicist", ", ".join(evgenConfig.contact))
#if _checkattr( "randomSeed") :
print "MetaData: %s = %s" % ("randomSeed", str(runArgs.randomSeed))
 
    
    

# Output list of generator filters used
filterNames = [alg.getType() for alg in acas.iter_algseq(filtSeq)]
excludedNames = ['AthSequencer', 'PyAthena::Alg', 'TestHepMC']
filterNames = list(set(filterNames) - set(excludedNames))
print "MetaData: %s = %s" % ("genFilterNames", ", ".join(filterNames))


##==============================================================
## Dump evgenConfig so it can be recycled in post-run actions
##==============================================================

from PyJobTransformsCore.runargs import RunArguments
runPars = RunArguments()
runPars.minevents = evgenConfig.minevents
runPars.maxeventsstrategy = evgenConfig.maxeventsstrategy
with open("config.pickle", 'w') as f:
    import cPickle
    cPickle.dump(runPars, f)


##==============================================================
## Get ready to run...
##==============================================================
 
evgenLog.debug("****************** STARTING EVENT GENERATION *****************")
print ("****************** STARTING EVENT GENERATION *****************")
print ("**************************************************************")
print ("****************** PLEASE IGNORE THE LOG FROM PYTHIA ************")
print ("****************** GENERATION OF ONE PYTHIA EVENT ***************")
print ("******************** IS NEEDED TO MAKE *************")
print ("****************** THE TRANSFORM WORK ***************************")
print ("**************************************************************")

