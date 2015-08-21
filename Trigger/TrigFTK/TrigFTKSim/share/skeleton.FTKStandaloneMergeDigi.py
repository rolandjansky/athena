# FTK Simulation Merge Transform Skeleton Job Options
# $Id: 

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKStandaloneMerging')
#ftkLog.propagate = False
ftkLog.info( '********** STARTING FTKStandaloneMerge **********' )

from AthenaCommon.AthenaCommonFlags import jobproperties as jp

from PerfMonComps.PerfMonFlags import jobproperties as pmjp;
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True

from RecExConfig.RecFlags import rec
rec.doWriteRDO.set_Value_and_Lock(True)
rec.OutputLevel.set_Value_and_Lock(DEBUG)


if hasattr(runArgs,"maxEvents"):
    ftkLog.info("Setting number of events: %d" % runArgs.maxEvents)
    theApp.EvtMax = runArgs.maxEvents
else:
    ftkLog.info("Running on all the events")
    theApp.EvtMax = -1 

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile

# this dictionary describe the standard position for common files, this allow
# a shorter command line when standard files are used
ftkBaseConfigDir = findFile(os.environ['DATAPATH'], 'ftk_configuration')
if ftkBaseConfigDir is None:
    ftkLog.warning('Failed to find ftk_configuration directory searching along $DATAPATH - all paths must now be given explicitly')
    standardConfBaseDir = {}
else:
    ftkLog.info('Found ftk_configuration directory {0}'.format(ftkBaseConfigDir))
    standardConfBaseDir = {'loadHWConf_path': os.path.join(ftkBaseConfigDir, 'hwsdev_file'),
                           'pmap_path': os.path.join(ftkBaseConfigDir, 'map_files')
                           }

# --------------------------------------------------------------
# FTK algorithm inclusions
# --------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import FTKMergerAlgo
import os
import sys

FTKMerger = FTKMergerAlgo( "FTKMergerAlgo" , OutputLevel=DEBUG)
FTKMerger.doMerging = True # this enables the behavior of the FTKMergerAlgo as FTK streams merger

runArgsMandatory =  ['NBanks', 'NSubRegions', 'pmap_path', 'loadHWConf_path']

runArgsOptional = {'FirstRegion': 0, 'FirstSubreg': 0, 'MergeRegion': -1, 'HWNDiff': 6, 'HitWarrior': 2, 'TruthFileNames': [], 'TruthTrackTreeName': '', 'EvtInfoTreeName': ''}

# prestore some common configurations that can be called through the option FTKSetupTag
FTKTagOptions = {}
FTKTagOptions["TDRv0"] = \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_11L.pmap', \
         'loadHWConf_path': 'raw_11L.hw'}
FTKTagOptions["TDRv1"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_11L.pmap', \
         'loadHWConf_path': 'raw_11L.hw'}
FTKTagOptions["TDAQTDRv0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}
FTKTagOptions["TDAQTDRv1"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}
FTKTagOptions["TDAQTDRv2"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}


# enable the "Scenario" runarg that sets other runarg values as consequence
if hasattr(runArgs, 'FTKSetupTag'):
    ftktag = getattr(runArgs,'FTKSetupTag')
    
    ftkLog.info("A FTKSetupTag=%s has been requested, default values will be set, or forced, for specific runArgs" % ftktag)

    # loop over the keys to be set
    for k, v in FTKTagOptions[ftktag].iteritems() :     
        if hasattr(runArgs,k) :
            info = "User value preferred on default (%s) for the key %s" % (str(v), k)
        else :
            info = "Running arguments set: %s = %s" %(k, str(v))
            setattr(runArgs,k,v)
        ftkLog.info(info)
        

# The presence of the option FinalMerge
if hasattr(runArgs, 'MergeFromTowers') :
    # overwrite the number of sub-regions
    if runArgs.MergeFromTowers :
      runArgs.NSubRegions = 1
      FTKMerger.FTKUnmergedFormatName = "FTKMergedTracksStream%u"

# set the FTKRoadMerger properties
for runArgName in runArgsMandatory + runArgsOptional.keys() :
   ftkLog.info('Setting FTKMerger argument %s' % runArgName)
   if hasattr(runArgs, runArgName):
       if runArgName in standardConfBaseDir:
           ftkLog.info("Using preferential path \"%s\" for %s" % (standardConfBaseDir[runArgName],runArgName))
           setattr(FTKMerger, runArgName, os.path.join(standardConfBaseDir[runArgName], getattr(runArgs, runArgName)))
       else:
           setattr(FTKMerger, runArgName, getattr(runArgs, runArgName))
   elif runArgName in runArgsOptional :
      setattr(FTKMerger, runArgName, runArgsOptional[runArgName])
   else:
       raise RuntimeError, 'Failed to find mandatory FTKMerger runtime argument for transform %s' % runArgName

# set a meaningful name for the PerfMon file
if FTKMerger.MergeRegion == -1 : 
  pmjp.PerfMonFlags.OutputFile = 'ntuple_FTKMerge.pmon.gz'
else :
  pmjp.PerfMonFlags.OutputFile = 'ntuple_FTKMergeTower%d.pmon.gz' % FTKMerger.MergeRegion

# Set output file
if hasattr(runArgs,'FTKUnmergedInputPath'):
    doGrid = False 
    if hasattr(runArgs,'FTKDoGrid') :
        doGrid = runArgs.FTKDoGrid

    if doGrid :
        ftkLog.info('Searching for input files in: %s' % runArgs.FTKUnmergedInputPath)
        FTKMerger.FTKUnmergedInputPath = runArgs.FTKUnmergedInputPath
        import glob
        filelist = glob.glob(runArgs.FTKUnmergedInputPath+"/NTUP_FTKTMP*.root*")
        filelist.sort() # sort the files by name
        
        if len(filelist)==0 :
            raise RuntimeError, "No input files in: " + runArgs.FTKUnmergedInputPath
        for curfile in filelist :
            ftkLog.info('Adding files to the list: %s' % curfile)
        FTKMerger.FTKToMergePaths = filelist # assign the list of files to the algorithms
    else :
        FTKMerger.FTKUnmergedInputPath = runArgs.FTKUnmergedInputPath
        import os
        dirList=os.listdir(runArgs.FTKUnmergedInputPath)
        fileRoot = "ftkoutput_unmerged"
        for fname in dirList:
            if fname.find(".root") != -1:
                keys = fname.split("_")
                nKeys = len(keys)
                if nKeys > 1:            
                    fileRoot =fname.replace("_"+keys[nKeys-2]+"_"+keys[nKeys-1],"")


        print "Using file root",fileRoot
        FTKMerger.FTKUnmergedFileRoot = fileRoot
elif hasattr(runArgs,'inputNTUP_FTKTMPFile'):
   FTKMerger.FTKToMergePaths = runArgs.inputNTUP_FTKTMPFile
else:
    ftkLog.warning('no input files to merge')
    

# Set output file
if hasattr(runArgs,'FTKForceAllInput'):
    FTKMerger.FTKForceAllInput = runArgs.FTKForceAllInput

if hasattr(runArgs,'FTKDoGrid'):
    FTKMerger.FTKDoGrid = runArgs.FTKDoGrid    

alg += FTKMerger  

#StreamRDO            = AthenaPoolOutputStream ( "StreamRDO" )
#StreamRDO.OutputFile = "InDetRecRDO.root"
#StreamRDO.ItemList   +=  ['FTK_RawTrackContainer#*']

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

StreamRDO=MSMgr.NewPoolStream("StreamRDO",runArgs.outputRDO_FTKFile)
StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] ) 
StreamRDO.AddItem( ["TrigInDetTrackCollection#*"] )
#StreamRDO.AddItem( ["*#*"] )
#objKeyStore.addStreamESD("TrigVertexCollection", "HLT_T2VertexBeamSpot_L2StarF")
#StreamRDO.AddItem( ["TrigInDetTrackCollection#*"] )
#StreamRDO.AddItem( ["TrackCollection#*"] )
#StreamRDO.AddItem( ["FTKAthTrackContainer#*"])
#StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] ) 
#StreamRDO.AddMetaDataItem(["IOVMetaDataContainer#*"])

#StreamRDO.AddItem( ["TrackParticleContainer#*"] )
#StreamRDO.AddItem( ["*#*"] ) 
#StreamRDO.AddItem( ["TrackCollectiom#FTK_Trk_Tracks"] )
