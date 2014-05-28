#! /usr/bin/env sh
###################################################################
#   ATLAS setup environment script
#   Wrapper script to submit MuonDQA jobs to LXBATCH
#   - using Rel. = AtlasPoint1  
###################################################################
#   (C) Nektarios.Chr.Benekos-Illinois
################################################################### 
#--- the following variables must (!!! TO BE ADJUSTED !!!) 
#numevents per partition file
export num=$1
export evtskip=$2
################################################################### 
source monvariables.sh
echo "what are the input variables?" $process $Mn $datatype $numevents $dstnum $rel $prefix $type $sample 

## FOR OUTPUT
export castoutdir=/castor/cern.ch/user/m/muondqa
export OUTPUTDIR=$castoutdir/$datatype/$rel/$dstnum
export outrootfile=$datatype.00$dstnum.$num.mon.root
export outcpurootfile=$datatype.00$dstnum.$num.perfmon.pmon.gz
export outlogfile=$datatype.00$dstnum.$num.log   
 
#--------------------------------------------------------------------------
#          package dependencies - setting up the environment 
#-------------------------------------------------------------------------- 
export T_RELEASE=13.2.0.Y
export T_NIGHTLY=rel_2
export ATLAS_ROOT=/afs/cern.ch/atlas 
export T_DISTREL=$ATLAS_ROOT/software/builds/nightlies/$T_RELEASE/AtlasPoint1/$T_NIGHTLY  
export ATLASDATA=$ATLAS_ROOT/offline/data 

#--- environment variables, dependent on Linux version (!!! TO BE ADJUSTED !!!)    
## ... for SLC4
export ATLASLIB=$T_DISTREL/InstallArea/i686-slc4-gcc34-opt/lib
   
export CMTVERSION=v1r20p20070720
export CMTROOT=/afs/cern.ch/sw/contrib/CMT/$CMTVERSION 
. ${CMTROOT}/mgr/setup.sh 
  
#--- set up home 
cat > requirements <<EOF
set   CMTSITE  CERN
set   SITEROOT /afs/cern.ch
set CMTSTRUCTURINGSTYLE "with_version_directory"
# if you don't want prefixes such as 'AtlasOffline-'
apply_tag simpleTest
apply_tag 32
apply_tag setupCMT
apply_tag setup
macro ATLAS_DIST_AREA /afs/cern.ch/atlas/software/dist
macro ATLAS_TEST_AREA `pwd`
use AtlasLogin AtlasLogin-* \$(ATLAS_DIST_AREA)
EOF
   
cmt config 
source setup.sh -tag=AtlasPoint1,$T_NIGHTLY,$T_RELEASE,opt,32,oneTest,runtime
shift;shift;shift;shift
   
echo "## ... CMTCONFIG=$CMTCONFIG"
echo "## ... CMTPATH set to $CMTPATH"
 
echo "##sourcing AtlasPoint1Release/cmt/setup.sh" 
. $T_DISTREL/AtlasPoint1RunTime/cmt/setup.sh"" 

echo "...My PATH" $PATH

#-------------------------------------------------------------------------- 
export LOCALRUNDIR=`pwd`
echo "LOCALRUNDIR is:" $LOCALRUNDIR

#--------------------------------------------------------------------------
# co the packages in the working directory
#--------------------------------------------------------------------------
cmt co MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring 
#--------------------------------------------------------------------------
# get appropriate tag
#--------------------------------------------------------------------------
LOCALREC=`/bin/ls MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring`
export LOCALREC=MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring/$LOCALREC
echo $LOCALREC

cd $LOCALREC/cmt
cmt broadcast cmt config

# H... knows why -tag=... is necessary here, but CMT doesn't set up the 
# right PATH and LD_LIBRARY_PATH without it, and athena can't be run.

source setup.sh -tag=$CMTCONFIG
#source setup.sh
cmt broadcast gmake -s

export PATH=`/afs/cern.ch/atlas/scripts/pathclean.pl`
echo "PATH length =            " `echo $PATH | wc -c`

export LD_LIBRARY_PATH=${LOCALRUNDIR}/InstallArea/i686-slc4-gcc34-opt:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`/afs/cern.ch/atlas/scripts/pathclean.pl LD_LIBRARY_PATH ":"`
echo "LD_LIBRARY_PATH length = " `echo $LD_LIBRARY_PATH | wc -c` 

#--------------------------------------------------------------------------
# power of the processor
echo "## ... processor specifications:"
grep MHz /var/log/dmesg
cat /proc/meminfo
export system="`uname -a`"
export userid="`   id   `"
export HOSTMACHINE=`hostname -f`
printenv | grep -v 'CONFIG=' | grep -v "ROOT=$DISTREL"
echo
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo
#--------------------------------------------------------------------------

cd ../run  
cat > $prefix.00$dstnum.physics.cosmics_batch.py << EOF 
#======================================================================
# This is an example steering jobOption file for MuonDQAMonitoring 
# Nektarios Chr. Benekos (UIUC)
#======================================================================
import os
 
os.system('rm -rf sqlite200')

#----------------------------------------------------------------------
# Some global variables, to make MuonDQAMonitoring happy, 
# for new configurable stuff
#---------------------------------------------------------------------- 
# Setup Athena common flags
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags

# import AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import ToolSvc
from AthenaCommon.AppMgr import ToolSvc
 
#--------------------------------------------------------------
# Number of events to read back
# import theApp
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp

#import ServiceMgr
from AthenaCommon.AppMgr import ServiceMgr  
svcMgr = theApp.serviceMgr()

#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool
#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

from AthenaCommon import CfgMgr
from RecExConfig.RecFlags import rec as recFlags

###############################################################
#
#  Configuration jobOptions file
#
###############################################################
#-----------------------------------------------------------------
# Setup Internal flags for the Muon DQA Monitoring (defaults values)
#----------------------------------------------------------------- 
from MuonDQAMonitoring.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

# --- Callgrind Tool
MuonDQAFlags.doCallgrind = False

# --- CPU and txt files
MuonDQAFlags.doCpuMuonMon = True
MuonDQAFlags.doMuonDQAPerfMonitoring = True
MuonDQAFlags.doAuditor = True
MuonDQAFlags.doFinalCPU = False  
MuonDQAFlags.doDumpFile = False
MuonDQAFlags.doSimpleFile = True

# --- Muon Hit Monitoring options
MuonDQAFlags.doMuonMonitoring = True
MuonDQAFlags.doMuonHitMon = True
# --- for each subdetector
MuonDQAFlags.doMDTMon = True
MuonDQAFlags.doRPCMon = True
MuonDQAFlags.doRPCL1Mon = True
MuonDQAFlags.doMDTRPCMon = True
MuonDQAFlags.doCSCMon = False
MuonDQAFlags.doTGCMon = True
MuonDQAFlags.doCSCMonStdAlone=False

# --- Muon Segment Monitoring options
MuonDQAFlags.doMuonSegmMon = True

# --- MuonTrk Monitoring options
MuonDQAFlags.doMuonTrackMon = True

# --- VP1 event display
MuonDQAFlags.doVP1 = False
#-----------------------------------------------------------------
# Setup Internal flags for the Muon Spectrometer (defaults values)
#-----------------------------------------------------------------
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample import ConfiguredMuonRec
ConfiguredMuonRec.defaultPackage = 'MuonCommRecExample'
#-----------------------------------------------------------------
# Setup Internal flags for the MuonRecFlags
#-----------------------------------------------------------------  
# initial flags for calibration constants loading and PRDs
#MuDetCosmicFlags.doM4 = False
#MuDetCosmicFlags.doM5 = True
#MuDetCosmicFlags.doP2 = False
# input 
#MuDetCosmicFlags.doSim   = False
#MuDetCosmicFlags.doMoore = True
#MuDetCosmicFlags.doMuonboy = True
#MuDetCosmicFlags.doMDT_TBCabling = True

muonRecFlags.doMoore = True
muonRecFlags.doMuonboy = True
 
#Steering flags
#MuDetCosmicFlags.doESD = False
#MuDetCosmicFlags.doGraphics = False
#MuDetCosmicFlags.doCalibFillNt = False
#MuDetCosmicFlags.doNtuple = False
#-----------------------------------------------------------------  
# setup Mx/Px data
#-----------------------------------------------------------------  
#if MuDetCosmicFlags.doM4:
#    ###FOR M4 --> list of good M4 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/M4OnlineRunList
#    MuonDQAFlags.RunNumber = $dstnum
#    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/'
#    MuonDQAFlags.InputDir = inDir
#    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep 00'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics.'
#    sampletype='daq.m4_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  
#
#if MuDetCosmicFlags.doM5:
#    ###FOR M5 --> list of good M5 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/M5MuonRunList
#    MuonDQAFlags.RunNumber = $dstnum
#    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5'
#    MuonDQAFlags.InputDir = inDir
#    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep daq.m5_combined.00'+str(MuonDQAFlags.RunNumber)
#    sampletype='daq.m5_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  
#                
#if MuDetCosmicFlags.doP2:
#    ###FOR P2 --> list of good P2 data-->https://twiki.cern.ch/twiki/bin/view/Atlas/P2MuonRunList
#    MuonDQAFlags.RunNumber = $dstnum
#    inDir='/castor/cern.ch/grid/atlas/t0/perm/DAQ/MuonWeekDec07'
#    MuonDQAFlags.InputDir = inDir
#    datafiles='nsls ' +str(MuonDQAFlags.InputDir)+' | grep daq.muon_combined.00'+str(MuonDQAFlags.RunNumber)
#    sampletype='daq.P2_combined.'+str(MuonDQAFlags.RunNumber)+'.physics.cosmics'  
      
###############################################################   
# ------------------------------------
# import GlobalFlags and DetFlags
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Print()

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.Muon_setOn()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.Print()
   
# -----------------------------------------
# Set Data Inputs (real data or simulation)
# -----------------------------------------
#if MuDetCosmicFlags.doSim:
#    MuDetCosmicFlags.doTruth = True
##reading simulated data from pool     
#    GlobalFlags.DataSource.set_geant4()
#    GlobalFlags.InputFormat.set_pool()
#    include ("MuonCommRecExample/MuDetCosmicDigitization_jobOptions.py")
#else:
#    MuDetCosmicFlags.doTruth = False  
##reading real data from ByteStream
#    GlobalFlags.DataSource.set_data()
#    GlobalFlags.InputFormat.set_bytestream()

# synchronise new-style flags
recFlags.doTruth = MuDetCosmicFlags.doTruth

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
if MuDetCosmicFlags.doSim:
    print 'Run on simulated data '
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    # Read Pool data
    ServiceMgr.EventSelector.InputCollections += [
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000000.root",
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000001.root",    
    "rfio:/castor/cern.ch/user/l/lytken/cosmic_1250/digitization/MuonVolume/withEndCaps/dig.05AprProd-10000099.root"
    ] 
    ServiceMgr.PoolSvc.AttemptCatalogPatch = True

else:
    print 'Run on Real data '   
    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    svcMgr.ROBDataProviderSvc.filterEmptyROB = True
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
    # Specify input file
    ByteStreamInputSvc = svcMgr.ByteStreamInputSvc 
    # Input directory
    ByteStreamInputSvc.InputDirectory = [ MuonDQAFlags.InputDir ]
    # Prefix 
    ByteStreamInputSvc.FilePrefix     = [ MuonDQAFlags.FilePrefix ]
    # Run number 
    ByteStreamInputSvc.RunNumber      = [ MuonDQAFlags.RunNumber ]
    if MuonDQAFlags.doSimpleFile:
        fileNames = os.popen(datafiles).readlines()
        fileNames.sort()
        ByteStreamInputSvc.FullFileName +=  [ os.path.join(inDir,f).strip() for f in fileNames ]
        print "Input files:"
        print ByteStreamInputSvc.FullFileName       
       
# ------------------------------------
# Detector Description /IOV Svc
# ------------------------------------
include ("MuonCommRecExample/MuDetCosmicDetDescr_jobOptions.py" )
#--------------------------------------------------------------
# include joboptions to read Muon PREPRAWDATA,  cabling maps
#--------------------------------------------------------------
include ("MuonCommRecExample/MuDetCosmicPRD_jobOptions.py")
#--------------------------------------------------------------
#initialize MuCTPI converter
#--------------------------------------------------------------
if MuDetCosmicFlags.doMUCTPI or MuDetCosmicFlags.doCTP:
    include ("MuonCommRecExample/ReadMuCTPI_jobOptions.py")
#--------------------------------------------------------------
# Conditions database access (only for real data for the moment)
#--------------------------------------------------------------          
if not MuDetCosmicFlags.doSim:
    include ("MuonCommRecExample/MuDetCosmicConDB_jobOptions.py")
#--------------------------------------------------------------
# Setup reconstruction
#--------------------------------------------------------------
include("MuonCommRecExample/MuonCommRec_jobOptions.py")

print '\n************************************************************\n'
print "   ******** NOW PRINTING DIR ******** "  
print '\n************************************************************\n'
print dir()

#--------------------------------------------------------------
# Turn on EDM monitors
#--------------------------------------------------------------
import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  if not 'doEdmMonitor' in dir(): doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  if not 'doNameAuditor' in dir(): doNameAuditor   = True
else:
  if not 'doEdmMonitor' in dir(): doEdmMonitor    = False
  if not 'doNameAuditor' in dir(): doNameAuditor   = False 
 
#--------------------------------------------------------------
# Write out ESD  
#--------------------------------------------------------------
if MuDetCosmicFlags.doESD:
    include( "MuonCommRecExample/MuDetCosmicPOOL_jobOptions.py")

#--------------------------------------------------------------
# VP1 event display
#--------------------------------------------------------------
if MuonDQAFlags.doVP1:
   from VP1Algs.VP1AlgsConf import VP1Alg
   topSequence += VP1Alg()

#--------------------------------------------------------------
# Monitoring Histogram  
#--------------------------------------------------------------
if MuonDQAFlags.doMuonMonitoring:
    ###MuonHit Monitoring
    if MuonDQAFlags.doMuonHitMon:
        include("MuonRawDataMonitoring/MuonRawDataMonitoring_options.py") 
    ###MuonSegm Monitoring
    if MuonDQAFlags.doMuonSegmMon:
        include("MuonSegmMonitoring/MuonSegmMonitoring_options.py")
        if MuDetCosmicFlags.doMoore and not MuDetCosmicFlags.doMuonboy:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [1,0]
        if MuDetCosmicFlags.doMuonboy and not MuDetCosmicFlags.doMoore:
            ToolSvc.MuonSegmValidation.WhichSegmCollections = [0,1] 
    ###MuonTrk Monitoring
    if MuonDQAFlags.doMuonTrackMon:
        include("MuonTrackMonitoring/MuonTrackMonitoring_options.py")
        if MuDetCosmicFlags.doMoore and not MuDetCosmicFlags.doMuonboy:
            ToolSvc.MuonTrackValidation.WhichTrackCollections = [1,0]
            ToolSvc.MuonAlignMonGenericTracks.WhichTrackCollections = [1,0]
        if MuDetCosmicFlags.doMuonboy and not MuDetCosmicFlags.doMoore:
            ToolSvc.MuonTrackValidation.WhichTrackCollections = [0,1]
            ToolSvc.MuonAlignMonGenericTracks.WhichTrackCollections = [0,1] 
	    
### Histograms Service
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr +=THistSvc()
    atlaspak='MuonDQAMonitoring'
    RootHistOutputFileName = str(atlaspak)+'.'+str(sampletype)+'.root' 

# Writing the histogram file
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    # Writing the histogram files from each sub-algorithm
    svcMgr.THistSvc.Output  += ["GLOBAL DATAFILE='%s' OPT='NEW'" % RootHistOutputFileName ]
          
#--------------------------------------------------------------
# Graphics  
#--------------------------------------------------------------
if MuDetCosmicFlags.doGraphics:
    include( "MuonCommRecExample/MuDetCosmicGraphics_jobOptions.py" )

#--------------------------------------------------------------
# Performace monitoring (CPU and Memory)
#--------------------------------------------------------------
if MuonDQAFlags.doCpuMuonMon:
    include("MuonDQAMonitoring/MuonDQAMonitoring_CpuPerfMon.py")
#--------------------------------------------------------------
# Callgrind
#--------------------------------------------------------------
if MuonDQAFlags.doCallgrind:
     from Valkyrie.ValkyrieConf import ValgrindSvc
     valgrindSvc = ValgrindSvc( OutputLevel = VERBOSE )
     if MuonDQAFlags.doMuonHitMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindHitAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
     if MuonDQAFlags.doMuonTrackMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindTrkAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
     if MuonDQAFlags.doMuonSegmMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindSegmAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
          
     # Add service and turn on auditing
     svcMgr += valgrindSvc
     theApp.CreateSvc += [svcMgr.ValgrindSvc.getFullJobOptName()]
     theApp.AuditAlgorithms = True
     theApp.AuditTools      = True
     theApp.AuditServices   = True

#--------------------------------------------------------------
# write out a summary of the time spent
#--------------------------------------------------------------
#if MuDetCosmicFlags.doAuditor:
#    theAuditorSvc += CfgMgr.ChronoAuditor()
#    theAuditorSvc += CfgMgr.NameAuditor()
#    theApp.AuditServices=True
#    theApp.AuditAlgorithms=True

# -------------------------------------------------------------
# Number of events to be processed (default is 10)
# -------------------------------------------------------------
# get EventSelector
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"
# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"; 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
# -------------------------------------------------------------
# Number of events to be processed (default is 10)
MuDetCosmicFlags.EvtMax = $numevents
theApp.EvtMax = MuDetCosmicFlags.EvtMax
# Number of events to be skipped
MuDetCosmicFlags.SkipEvents = $evtskip
svcMgr.EventSelector.SkipEvents=MuDetCosmicFlags.SkipEvents 
# -------------------------------------------------------------
#Output level
# -------------------------------------------------------------
ServiceMgr += CfgMgr.MessageSvc( Format = "% F%50W%S%7W%R%T %0W%M",
                                 defaultLimit=1000000 , 
				 OutputLevel = INFO,
                                 useColors = False # use color output text
				 ) 
#--------------------------------------------------------------------------- 
# THIS IS TEMPORY HACK TO GET RID OF MUON ERRORS BEING PRINTED FOR EVERY EVENT
# NEED A REAL FIX 
#--------------------------------------------------------------------------- 
if MuonDQAFlags.doMuonHitMon:
    ToolSvc += CfgMgr.MDTRawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.RPCRawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.MDTvsRPCRawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.RPCLV1RawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.TGCRawDataValAlg(OutputLevel = INFO)
    if MuonDQAFlags.doCSCMon:
        ToolSvc += CfgMgr.CSCPRDMonTool(OutputLevel = INFO)
        ToolSvc += CfgMgr.CSCRDOMonTool(OutputLevel = INFO)
    
if MuonDQAFlags.doMuonSegmMon:
    ToolSvc += CfgMgr.MuonSegmValidation(OutputLevel = INFO)

if MuonDQAFlags.doMuonTrackMon:
    ToolSvc += CfgMgr.MuonTrackValidation(OutputLevel = INFO)
    ToolSvc += CfgMgr.MuonAlignMonGenericTracks(OutputLevel = INFO) 
     
Service( "THistSvc" ).OutputLevel = WARNING 
#--------------------------------------------------------------------------- 
print "List all DLL"
print theApp.Dlls
print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg
#--------------------------------------------------------------------------- 
print topSequence
print ServiceMgr
print ToolSvc
#======================================================================
             
EOF

echo "JobOPt used --> $prefix.00$dstnum.physics.cosmics_batch.py <-- whose content is:"
cat $prefix.00$dstnum.physics.cosmics_batch.py 
export here=`pwd`

#########################################################################
# run the job ...
#
echo " "
echo " "
echo "##################################################################"
echo "## STEP 4: running athena ..."
echo "##################################################################"
echo "##"
echo "##... use the following athena.py script:"
which athena.py
echo "## PATH = $PATH"
echo "## LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
echo "######Ok, ready for running: files in local dir are"
echo `pwd`
ls -l

time athena.py -b $prefix.00$dstnum.physics.cosmics_batch.py >& $outlogfile

mv MuonDQAMonitoring.daq.*combined.$dstnum.physics.cosmics.root $outrootfile 
mv muondqa.perfmon.pmon.gz $outcpurootfile

#--------------------------------------------------------------------------
# paste output-file size to the end of the logfile...
#
#
[ -f "$outlogfile" ] && echo outputlog file SIZE: `ls -l $outlogfile`
[ -f "$outrootfile" ] && echo output ROOT file SIZE: `ls -l $outrootfile` 
[ -f "$outcpurootfile" ] && echo output ROOT file SIZE: `ls -l $outcpurootfile` 

ls -l $here 
 
#--------------------------------------------------------------------------
#          output log file copy
#--------------------------------------------------------------------------
if [[ -e $outlogfile ]] ; then
 echo "md5sum $outlogfile `md5sum $outlogfile `"
 rfmkdir -p $OUTPUTDIR/logfiles
 nschmod 775 $OUTPUTDIR/logfiles
 rfcp $outlogfile $OUTPUTDIR/logfiles/$outlogfile
 echo "where the logfile is copied?" $OUTPUTDIR/logfiles
 echo "rfcp result code = $?"
fi
#--------------------------------------------------------------------------
#          output ROOT file copy
#--------------------------------------------------------------------------
if [[ -e $outrootfile ]] ; then
 echo "md5sum $outrootfile `md5sum $outrootfile `"
 rfmkdir -p $OUTPUTDIR
 nschmod 775 $OUTPUTDIR
 rfcp $outrootfile $OUTPUTDIR/$outrootfile
 echo "where the $outrootfile is copied?" $OUTPUTDIR
 echo "rfcp result code = $?"
fi
#--------------------------------------------------------------------------
#          output ROOT file copy
#--------------------------------------------------------------------------
if [[ -e $outcpurootfile ]] ; then
 echo "md5sum $outcpurootfile `md5sum $outcpurootfile `"
 rfmkdir -p $OUTPUTDIR
 nschmod 775 $OUTPUTDIR
 rfcp $outcpurootfile $OUTPUTDIR/$outcpurootfile
 echo "where the $outcpurootfile is copied?" $OUTPUTDIR
 echo "rfcp result code = $?"
fi
 
