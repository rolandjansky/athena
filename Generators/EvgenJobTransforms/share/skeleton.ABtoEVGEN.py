"""Functionality core of the Generate_tf transform"""

##==============================================================
## Basic configuration
##==============================================================

## Create sequences for generators, clean-up algs, filters and analyses
## and import standard framework objects with standard local scope names
import os, re, string
import AthenaCommon.AlgSequence as acas
import AthenaCommon.AppMgr as acam
from AthenaCommon.AthenaCommonFlags import jobproperties
theApp = acam.theApp
acam.athMasterSeq += acas.AlgSequence("EvgenGenSeq")
genSeq = acam.athMasterSeq.EvgenGenSeq
#acam.athMasterSeq += acas.AlgSequence("EvgenFixSeq")
#fixSeq = acam.athMasterSeq.EvgenFixSeq
acam.athMasterSeq += acas.AlgSequence("EvgenPreFilterSeq")
prefiltSeq = acam.athMasterSeq.EvgenPreFilterSeq
acam.athMasterSeq += acas.AlgSequence("EvgenTestSeq")
testSeq = acam.athMasterSeq.EvgenTestSeq
## NOTE: LogicalExpressionFilter is an algorithm, not a sequence
from EvgenProdTools.LogicalExpressionFilter import LogicalExpressionFilter
acam.athMasterSeq += LogicalExpressionFilter("EvgenFilterSeq")
filtSeq = acam.athMasterSeq.EvgenFilterSeq
topSeq = acas.AlgSequence()
anaSeq = topSeq
topSeq += acas.AlgSequence("EvgenPostSeq")
postSeq = topSeq.EvgenPostSeq
#topAlg = topSeq #< alias commented out for now, so that accidental use throws an error


##==============================================================
## Configure standard Athena services
##==============================================================

## Special setup for event generation
include("AthenaCommon/Atlas.UnixStandardJob.py")
include("PartPropSvc/PartPropSvc.py")

## Run performance monitoring (memory logging)
from PerfMonComps.PerfMonFlags import jobproperties as perfmonjp
perfmonjp.PerfMonFlags.doMonitoring = True
perfmonjp.PerfMonFlags.doSemiDetailedMonitoring = True

## Random number services
from AthenaServices.AthenaServicesConf import AtRndmGenSvc, AtRanluxGenSvc
svcMgr += AtRndmGenSvc()
svcMgr += AtRanluxGenSvc()

## Jobs should stop if an include fails.
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError = False

## Set up a standard logger
from AthenaCommon.Logging import logging
evgenLog = logging.getLogger('Generate')


##==============================================================
## Run arg handling
##==============================================================

## Announce arg checking
evgenLog.debug("****************** CHECKING EVENT GENERATION ARGS *****************")
evgenLog.debug(str(runArgs))

## Ensure that an output name has been given
# TODO: Allow generation without writing an output file (if outputEVNTFile is None)?
if not hasattr(runArgs, "outputEVNTFile") and not hasattr(runArgs, "outputEVNT_PreFile"):
    raise RuntimeError("No output evgen EVNT or EVNT_Pre file provided.")

## Ensure that mandatory args have been supplied (complain before processing the includes)
if not hasattr(runArgs, "ecmEnergy"):
    raise RuntimeError("No center of mass energy provided.")
if not hasattr(runArgs, "randomSeed"):
    raise RuntimeError("No random seed provided.")
if not hasattr(runArgs, "runNumber"):
    raise RuntimeError("No run number provided.")
    # TODO: or guess it from the JO name??
if not hasattr(runArgs, "firstEvent"):
    raise RuntimeError("No first number provided.")


##==============================================================
## Configure standard Athena and evgen services
##==============================================================

## Announce start of job configuration
evgenLog.debug("****************** CONFIGURING EVENT GENERATION *****************")

## Functions for operating on generator names
## NOTE: evgenConfig, topSeq, svcMgr, theApp, etc. should NOT be explicitly re-imported in JOs
from EvgenJobTransforms.EvgenConfig import evgenConfig
from EvgenJobTransforms.EvgenConfig import gens_known, gens_lhef, gen_sortkey, gens_testhepmc, gens_notune

## Fix non-standard event features
#from EvgenProdTools.EvgenProdToolsConf import FixHepMC
#if not hasattr(fixSeq, "FixHepMC"):
#    fixSeq += FixHepMC()

## Sanity check the event record (not appropriate for all generators)
from EvgenProdTools.EvgenProdToolsConf import TestHepMC
testSeq += TestHepMC(CmEnergy=runArgs.ecmEnergy*Units.GeV)
if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["TestHepMCname DATAFILE='TestHepMC.root' OPT='RECREATE'"]

## Copy the event weight from HepMC to the Athena EventInfo class
# TODO: Rewrite in Python?
from EvgenProdTools.EvgenProdToolsConf import CopyEventWeight
  #if not hasattr(postSeq, "CopyEventWeight"):
#    postSeq += CopyEventWeight()

## Configure the event counting (AFTER all filters)
# TODO: Rewrite in Python?
from EvgenProdTools.EvgenProdToolsConf import CountHepMC

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.FirstEvent = runArgs.firstEvent
theApp.EvtMax = -1
if not hasattr(postSeq, "CountHepMC"):
    postSeq += CountHepMC()
postSeq.CountHepMC.RequestedOutput = evgenConfig.minevents if runArgs.maxEvents == -1 else runArgs.maxEvents
postSeq.CountHepMC.FirstEvent = runArgs.firstEvent
postSeq.CountHepMC.CorrectHepMC = False
postSeq.CountHepMC.CorrectEventID = True

## Print out the contents of the first 5 events (after filtering)
# TODO: Allow configurability from command-line/exec/include args
if hasattr(runArgs, "printEvts") and runArgs.printEvts > 0:
    from TruthIO.TruthIOConf import PrintMC
    postSeq += PrintMC()
    postSeq.PrintMC.McEventKey = "GEN_EVENT"
    postSeq.PrintMC.VerboseOutput = True
    postSeq.PrintMC.PrintStyle = "Barcode"
    postSeq.PrintMC.FirstEvent = 1
    postSeq.PrintMC.LastEvent  = runArgs.printEvts

## Add Rivet_i to the job
# TODO: implement auto-setup of analyses triggered on evgenConfig.keywords (from T Balestri)
if hasattr(runArgs, "rivetAnas"):
    from Rivet_i.Rivet_iConf import Rivet_i
    anaSeq += Rivet_i()
    anaSeq.Rivet_i.Analyses = runArgs.rivetAnas
    anaSeq.Rivet_i.DoRootHistos = True


##==============================================================
## Pre- and main config parsing
##==============================================================

## Announce JO loading
evgenLog.debug("****************** LOADING PRE-INCLUDES AND JOB CONFIG *****************")

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
        evgenLog.error("Expected dataset ID part of JO name to be '%s', but found '%s'" % (str(runArgs.runNumber), jo_dsidpart))
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


##==============================================================
## Config validation and propagation to services, generators, etc.
##==============================================================

## Announce start of JO checking
evgenLog.debug("****************** CHECKING EVGEN CONFIGURATION *****************")

## Print out options
for opt in str(evgenConfig).split(os.linesep):
    evgenLog.info(opt)

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
    expectedgenpart = ''.join(gennames)
    ## We want to record that HERWIG was used in metadata, but in the JO naming we just use a "Herwig" label
    expectedgenpart = expectedgenpart.replace("HerwigJimmy", "Herwig")
    def _norm(s):
        # TODO: add EvtGen to this normalization for MC14?
        return s.replace("Photospp", "").replace("Photos", "").replace("TauolaPP", "").replace("Tauolapp", "").replace("Tauola", "")
    def _norm2(s):
        return s.replace("Py", "Pythia").replace("MG","MadGraph").replace("Ph","Powheg").replace("Hpp","Herwigpp").replace("H7","Herwig7").replace("Sh","Sherpa").replace("Ag","Alpgen").replace("EG","EvtGen").replace("PG","ParticleGun")

    def _short2(s):
         return s.replace("Pythia","Py").replace("MadGraph","MG").replace("Powheg","Ph").replace("Herwigpp","Hpp").replace("Herwig7","H7").replace("Sherpa","Sh").replace("Alpgen","Ag").replace("EvtGen","EG").replace("PG","ParticleGun")
 
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

## Check that the evgenConfig.minevents setting is acceptable
## minevents defines the production event sizes and must be sufficiently "round"
if evgenConfig.minevents < 1:
    raise RunTimeError("evgenConfig.minevents must be at least 1")
else:
    allowed_minevents_lt1000 = [1, 2, 5, 10, 20, 25, 50, 100, 200, 500]
    msg = "evgenConfig.minevents = %d: " % evgenConfig.minevents
    if evgenConfig.minevents >= 1000 and evgenConfig.minevents % 1000 != 0:
        msg += "minevents in range >= 1000 must be a multiple of 1000"
        raise RuntimeError(msg)
    elif evgenConfig.minevents < 1000 and evgenConfig.minevents not in allowed_minevents_lt1000:
        msg += "minevents in range < 1000 must be one of %s" % allowed_minevents_lt1000
        raise RuntimeError(msg)

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
            allowed_keywords += l.strip().split()
        #allowed_keywords.sort()
        ## Check the JO keywords against the allowed ones
        evil_keywords = []
        for k in evgenConfig.keywords:
            if k not in allowed_keywords:
                evil_keywords.append(k)
        if evil_keywords:
            msg = "evgenConfig.keywords contains non-standard keywords: %s. " % ", ".join(evil_keywords)
            msg += "Please check the allowed keywords list and fix."
            evgenLog.error(msg)
            if officialJO:
                sys.exit(1)
    else:
        evgenLog.warning("Could not find evgenkeywords.txt file %s in $JOBOPTSEARCHPATH" % kwfile)

## Configure and schedule jet finding algorithms
## NOTE: This generates algorithms for jet containers defined in the user's JO fragment
if evgenConfig.findJets:
    include("EvgenJobTransforms/Generate_TruthJets.py")

## Configure POOL streaming to the output EVNT format file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
if hasattr(runArgs, "outputEVNTFile"):
  poolFile = runArgs.outputEVNTFile
elif hasattr(runArgs, "outputEVNT_PreFile"):
  poolFile = runArgs.outputEVNT_PreFile
else:
  raise RuntimeError("Output pool file, either EVNT or EVNT_Pre, is not known.")

#StreamEVGEN = AthenaPoolOutputStream("StreamEVGEN", runArgs.outputEVNTFile)
StreamEVGEN = AthenaPoolOutputStream("StreamEVGEN", poolFile)
if hasattr(runArgs, "inputEVNT_PreFile") :
  #  import AthenaPoolCnvSvc.ReadAthenaPool
  #  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  #  athenaCommonFlags.PoolEvgenInput.set_Value_and_Lock( runArgs.inputEVNT_PreFile )
  svcMgr.EventSelector.InputCollections = runArgs.inputEVNT_PreFile
  StreamEVGEN.TakeItemsFromInput = True

StreamEVGEN.ForceRead = True
StreamEVGEN.ItemList += ["EventInfo#*", "McEventCollection#*"]
StreamEVGEN.RequireAlgs += ["EvgenFilterSeq"]
## Used for pile-up (remove dynamic variables except flavour labels)
if evgenConfig.saveJets:
    StreamEVGEN.ItemList += ["xAOD::JetContainer_v1#*"]
    StreamEVGEN.ItemList += ["xAOD::JetAuxContainer_v1#*.TruthLabelID.PartonTruthLabelID"]

## Set the run numbers
svcMgr.EventSelector.RunNumber = runArgs.runNumber
# TODO: set EventType::mc_channel_number = runArgs.runNumber

## Handle beam info
import EventInfoMgt.EventInfoMgtInit
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_energy", str(int(runArgs.ecmEnergy*Units.GeV/2.0))]
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_type", 'collisions']

## Propagate energy argument to the generators
# TODO: Standardise energy setting in the GenModule interface
include("EvgenJobTransforms/Generate_ecmenergies.py")

## Process random seed arg and pass to generators
include("EvgenJobTransforms/Generate_randomseeds.py")

## Add special config option (extended model info for BSM scenarios)
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["specialConfiguration", evgenConfig.specialConfig ]

## Remove TestHepMC if it's inappropriate for this generator combination
# TODO: replace with direct del statements in the generator common JO fragments?
if hasattr(testSeq, "TestHepMC") and not gens_testhepmc(evgenConfig.generators):
    evgenLog.info("Removing TestHepMC sanity checker")
    del testSeq.TestHepMC


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
## Show the algorithm sequences and algs now that config is complete
##==============================================================
acas.dumpMasterSequence()


##==============================================================
## Input file arg handling
##==============================================================

## Announce start of input file handling
evgenLog.debug("****************** HANDLING EVGEN INPUT FILES *****************")

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

## Events files
eventsFile = None

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
    print "MetaData: %s = %s" % ("generatorName", "+".join(gennames))
if _checkattr("process"):
    print "MetaData: %s = %s" % ("physicsProcess", evgenConfig.process)
if _checkattr("tune"):
    print "MetaData: %s = %s" % ("generatorTune", evgenConfig.tune)
if _checkattr("hardPDF"):
    print "MetaData: %s = %s" % ("hardPDF", evgenConfig.hardPDF)
if _checkattr("softPDF"):
    print "MetaData: %s = %s" % ("softPDF", evgenConfig.softPDF)
if _checkattr("keywords"):
    print "MetaData: %s = %s" % ("keywords", ", ".join(evgenConfig.keywords).lower())
if _checkattr("specialConfig"):
   print "MetaData: %s = %s" % ("specialConfig", evgenConfig.specialConfig)
# TODO: Require that a contact / JO author is always set
if _checkattr("contact"):
    print "MetaData: %s = %s" % ("contactPhysicist", ", ".join(evgenConfig.contact))

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
