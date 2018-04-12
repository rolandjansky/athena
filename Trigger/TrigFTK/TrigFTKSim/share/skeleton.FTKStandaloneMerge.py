# FTK Simulation Merge Transform Skeleton Job Options
# $Id:

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKStandaloneMerging')
#ftkLog.propagate = False
ftkLog.info( '********** STARTING FTKStandaloneMerge **********' )

from AthenaCommon.AthenaCommonFlags import jobproperties as jp

from PerfMonComps.PerfMonFlags import jobproperties as pmjp;
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True
###pmjp.PerfMonFlags.doFullMon = True

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

FTKMerger = FTKMergerAlgo( "FTKMergerAlgo" , OutputLevel=VERBOSE)
FTKMerger.doMerging = True # this enables the behavior of the FTKMergerAlgo as FTK streams merger

runArgsMandatory =  ['NBanks', 'NSubRegions', 'pmap_path', 'loadHWConf_path']

runArgsOptional = {'FirstRegion': 0, 'FirstSubreg': 0, 'MergeRegion': -1, 'HWNDiff': 6, 'HitWarriorMerger': 2}

nb=64

# prestore some common configurations that can be called through the option FTKSetupTag
FTKTagOptions = {}

FTKTagOptions['SaP_Reb64_12L'] = \
     {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap',
      'loadHWConf_path': 'raw_12L.hw', 'MergeRoads': False, 'MergeRoadsDetailed': False}
FTKTagOptions['SaP_Reb64_8L'] = \
     {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl3D123.pmap',
      'loadHWConf_path': 'raw_8Lc.hw', 'MergeRoads': False, 'MergeRoadsDetailed': False}
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
FTKTagOptions["Run2v0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl3D.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}
FTKTagOptions["Run2v1"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl3D.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}
FTKTagOptions["Run2TempMapv0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_12Libl3D.pmap', \
         'loadHWConf_path': 'raw_12L.hw'}
FTKTagOptions['SectorsAsPatterns32Tower12L3D'] = \
    {'NBanks': 32, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap',
     'loadHWConf_path': 'raw_12L.hw', 'MergeRoads': False, 'MergeRoadsDetailed': False}
FTKTagOptions['SectorsAsPatterns32Tower8L3D'] = \
    {'NBanks': 32, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl3D123.pmap',
     'loadHWConf_path': 'raw_8Lc.hw', 'MergeRoads': False, 'MergeRoadsDetailed': False}
FTKTagOptions["HWMode2Test32Tower"] = \
   {'NBanks': 32, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}
FTKTagOptions["HWMode2Test64Tower"] = \
   {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}
FTKTagOptions["64Tower2017.v1.ECFix"] = \
   {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}
FTKTagOptions["64Tower2018.v1.ECFix"] = \
   {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}
FTKTagOptions["64Tower2017.v1"] = \
   {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}
FTKTagOptions["64Tower2017.v2"] = \
   {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW3D.pmap', \
         'loadHWConf_path': 'raw_8LcIbl123.hw'}

FTKTagOptions['FitITk'] = {
    'MergeRoads' : True ,
    'HitWarriorMerger' : 0 ,
}
FTKTagOptions['FitITkSaP'] = {
    'MergeRoads' : True ,
    'HitWarriorMerger' : 0 ,
}
FTKTagOptions['FitITkDC'] = {
    'MergeRoads' : True ,
    'HitWarriorMerger' : 0 ,
}


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


if hasattr(runArgs, 'MergeRoads'):
  FTKMerger.MergeRoads = runArgs.MergeRoads

if hasattr(runArgs, 'MergeRoadsDetailed'):
  FTKMerger.MergeRoadsDetailed = runArgs.MergeRoadsDetailed

# The presence of the option FinalMerge
if hasattr(runArgs, 'MergeFromTowers') :
    # overwrite the number of sub-regions
    if runArgs.MergeFromTowers :
      runArgs.NSubRegions = 1
      FTKMerger.FTKUnmergedFormatName = "FTKMergedTracksStream%u"
      FTKMerger.FTKUnmergedRoadFormatName = "FTKMergedRoadsStream%u"

### But overwrite the names if provided, no matter what
if hasattr(runArgs, 'UnmergedFormatName'):
  FTKMerger.FTKUnmergedFormatName = runArgs.UnmergedFormatName

### But overwrite the names if provided, no matter what
if hasattr(runArgs, 'UnmergedRoadFormatName'):
  FTKMerger.FTKUnmergedRoadFormatName = runArgs.UnmergedRoadFormatName

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

if hasattr(runArgs, 'HitWarriorMerger'):
  FTKMerger.HitWarriorMerger = runArgs.HitWarriorMerger  


# set a meaningful name for the PerfMon file
if FTKMerger.MergeRegion < 0:
  pmjp.PerfMonFlags.OutputFile = 'ntuple_FTKMerge.pmon.gz'
else :
  pmjp.PerfMonFlags.OutputFile = 'ntuple_FTKMergeTower%d.pmon.gz' % FTKMerger.MergeRegion

### overwrite the merger if we need to
if hasattr(runArgs,'UberFinalMerge') and FTKMerger.MergeRegion < 0: ### only do if it's a final merge
  if (runArgs.UberFinalMerge):
    FTKMerger.MergeRegion = -1 ### put all in one stream
  else:
    FTKMerger.MergeRegion = -2 ### put all in one file, but keep streams separate


### truth output copying
if hasattr(runArgs,'EvtInfoTreeName'):
    FTKMerger.EvtInfoTreeName = runArgs.EvtInfoTreeName
if hasattr(runArgs,'TruthTrackTreeName'):
    FTKMerger.TruthTrackTreeName = runArgs.TruthTrackTreeName

if hasattr(runArgs, 'SaveOfflineTree'):
    if runArgs.SaveOfflineTree == False:
        print "SaveOfflineTrees False"
        FTKMerger.OfflineTreeName = '' ### be safe
    else: ## use default
        print "SaveOfflineTrees True and setting name to default = offline_cluster_tree"
        FTKMerger.OfflineTreeName = 'offline_cluster_tree'

if hasattr(runArgs, 'SaveTruthTree'):
    if runArgs.SaveTruthTree:
      if hasattr(runArgs, 'inputNTUP_FTKIPFile'):
        FTKMerger.TruthFileNames = runArgs.inputNTUP_FTKIPFile
      else: ### be safe otherwise
        FTKMerger.TruthTrackTreeName = ''
        FTKMerger.EvtInfoTreeName = ''
    else:
      FTKMerger.TruthTrackTreeName = ''
      FTKMerger.EvtInfoTreeName = ''
else:
    FTKMerger.TruthTrackTreeName = ''
    FTKMerger.EvtInfoTreeName = ''

# Set output file
if hasattr(runArgs, 'FTKUnmergedInputPath'):
    doGrid = False
    if hasattr(runArgs, 'FTKDoGrid'):
        doGrid = runArgs.FTKDoGrid
    if doGrid:
        ftkLog.info('Searching for input files in: %s' % runArgs.FTKUnmergedInputPath)
        FTKMerger.FTKUnmergedInputPath = runArgs.FTKUnmergedInputPath
        import glob
        filelist = glob.glob(runArgs.FTKUnmergedInputPath+"/NTUP_FTKTMP*.root*")
        filelist.sort() # sort the files by name

        if len(filelist) == 0:
            raise RuntimeError, "No input files in: " + runArgs.FTKUnmergedInputPath
        for curfile in filelist :
            ftkLog.info('Adding files to the list: %s' % curfile)
        FTKMerger.FTKToMergePaths = filelist # assign the list of files to the algorithms
    else:
        FTKMerger.FTKUnmergedInputPath = runArgs.FTKUnmergedInputPath
        dirList = os.listdir(runArgs.FTKUnmergedInputPath)
        fileRoot = "ftkoutput_unmerged"
        for fname in dirList:
            if fname.find(".root") != -1:
                keys = fname.split("_")
                nKeys = len(keys)
                if nKeys > 1:
                    fileRoot = fname.replace("_"+keys[nKeys-2]+"_"+keys[nKeys-1],"")
        print "Using file root", fileRoot
        FTKMerger.FTKUnmergedFileRoot = fileRoot
elif hasattr(runArgs, 'inputNTUP_FTKTMP_00File'):
    # tower files specified separately
    FTKMerger.FTKToMergePaths = [
        getattr(runArgs, 'inputNTUP_FTKTMP_{0:02d}File'.format(tower))[0]
        for tower in range(runArgs.NBanks)] 
elif hasattr(runArgs, 'inputNTUP_FTKTMPFile'):
    FTKMerger.FTKToMergePaths = runArgs.inputNTUP_FTKTMPFile
else:
    ftkLog.warning('no input files to merge')

# Set output file
if hasattr(runArgs,'FTKForceAllInput'):
    FTKMerger.FTKForceAllInput = runArgs.FTKForceAllInput

# enable an internal flag for grid jobs naming conventions
if hasattr(runArgs,'FTKDoGrid'):
    FTKMerger.FTKDoGrid = runArgs.FTKDoGrid

if hasattr(runArgs,'outputNTUP_FTKFile') :
    FTKMerger.FTKMergedOutput = runArgs.outputNTUP_FTKFile

elif hasattr(runArgs,'outputNTUP_FTKTMPFile') :
    FTKMerger.FTKMergedOutput = runArgs.outputNTUP_FTKTMPFile

elif hasattr(runArgs,'outputRDO_FTKFile') :
  if hasattr(runArgs, 'inputRDOFile') :
    # Merge an existing RDO with the FTK tracks
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    # this initialize the input service
    athenaCommonFlags.FilesInput.set_Value_and_Lock( runArgs.inputRDOFile )
    from RecJobTransforms.RDOFilePeeker import RDOFilePeeker
    RDOFilePeeker(runArgs, ftkLog) # not sure what it does
    # enable the detector flag
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOn()
    DetFlags.ALFA_setOff()
    DetFlags.ZDC_setOff()
    # other services
    from AthenaCommon.AppMgr import ServiceMgr
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    ServiceMgr += AthenaPoolCnvSvc()
    import AthenaPoolCnvSvc.ReadAthenaPool
    ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
    # Add the FTK collections to the RDO
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    StreamRDO = AthenaPoolOutputStream( "StreamRDO", runArgs.outputRDO_FTKFile)
    #StreamRDO.TakeItemsFromInput=True
    StreamRDO.ForceRead=TRUE
    StreamRDO.ItemList+=["FTK_RawTrackContainer#*"]
  else :
    # generate RDO file from scratch, this represents an RDO
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    StreamRDO=MSMgr.NewPoolStream("StreamRDO",runArgs.outputRDO_FTKFile)
    StreamRDO.AddItem( ["FTK_RawTrackContainer#*"] )
    StreamRDO.AddItem( ["TrigInDetTrackCollection#*"] )
else:
    ftkLog.error('No output file for merge given')
    raise RuntimeError, 'No output file for merge given'
alg += FTKMerger
