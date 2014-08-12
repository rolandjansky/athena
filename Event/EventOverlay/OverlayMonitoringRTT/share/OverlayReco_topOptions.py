from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags



if not 'Input' in dir():
    # uncomment if input is real data
    #Input='data'
    # uncomment if input is overlay of simulated Hits onto real data
    Input='overlay'
    # uncomment if input is simulation BS
    # Input='simbs'
    # uncomment if input is simulation RDO
    # Input='simrdo'

if not 'Debug' in dir():
    Debug=False  # change to True to have printout information

# uncomment if reading from TAG
#rec.readTAG=True

if rec.readTAG():
    athenaCommonFlags.PoolTAGInput=['../runtrf/myTAGCOMM.root']
    athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("TRTTracks>0")
#specify beam type (default is "collisions", other values "cosmics" "singlebeam"
#jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")

# input configuration (lock to prevent override)
# note that if reading from TAG, the input file spec is ignored,
#   however the flag settings should still be consistent 
if Input=='data':


    athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
            '/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
    # Detector description #jamie, use version with field (even though solenoid is off)

    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
    globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
    # for running over data with field on need this to be setto true
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
elif Input=='overlay':

    #from LArConditionsCommon.LArCondFlags import larCondFlags
    #larCondFlags.useShape=True

    from LArROD.LArRODFlags import larRODFlags
    larRODFlags.readDigits = False

    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([
            '/afs/cern.ch/user/k/ketevi/w0/data/singleMuon100plusCosmics.RDO.020162._00001.pool.root'])
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
    globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    globalflags.InputFormat.set_Value_and_Lock('pool')
    globalflags.DataSource.set_Value_and_Lock('data')
else:
    if Input=='simbs':
        #beware there might be sthg wrong in these BS files wrt ID cabling
        athenaCommonFlags.BSRDOInput.set_Value_and_Lock(
            ["/afs/cern.ch/user/g/gencomm/w0/jboyd/900Gev/BS/daq.0951001._0001.data",
             "/afs/cern.ch/user/g/gencomm/w0/jboyd/900Gev/BS/daq.0951001._0002.data"])
        globalflags.InputFormat.set_Value_and_Lock('bytestream')
    elif Input=='simrdo':
        athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(
        ["castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00001.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00002.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00003.pool.root.1",    
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00004.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00005.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00006.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00007.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00008.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00009.pool.root.1",
         "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._000010.pool.root.1"])         

        globalflags.InputFormat.set_Value_and_Lock('pool')
        # crashing :  No valid proxy for object TileTTL1CosmicsCnt  of type TileTTL1Container(CLID 2933)
        #doTileCosmicsTrigger=False

    else:
      raise RuntimeError,"Wrong Input value"

    globalflags.DataSource.set_Value_and_Lock('geant4')    
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-00')


#hardcoded output specification (corresponds to optional flags from runArgs in transform skeleton)
rec.RootNtupleOutput.set_Value_and_Lock("commission.ntuple.root")
athenaCommonFlags.PoolESDOutput.set_Value_and_Lock("commissionESD.pool.root")
rec.RootHistoOutput.set_Value_and_Lock("monitoring.root")
athenaCommonFlags.EvtMax.set_Value_and_Lock(50)
#athenaCommonFlags.SkipEvents = 0

rec.doHist=True
rec.doWriteTAGCOM.set_Value_and_Lock(True)
rec.doWriteTAG.set_Value_and_Lock(False)	
rec.doWriteAOD=True
rec.doAOD=True
if Input=='overlay':
   rec.doMonitoring.set_Value_and_Lock(False)
else:
   rec.doMonitoring.set_Value_and_Lock(True)

if Debug:
    # DEBUG/VERBOSE options
    rec.doNameAuditor=True
    rec.doPerfMon=True
    rec.doDetailedPerfMon=True
    rec.doDumpProperties=True
    rec.doDumpTES=True
    # rec.OutputLevel = INFO

# the main jobOpt
include("RecExCommission/RecExCommission.py")                

# filtered ESD if not collisions
if jobproperties.Beam.beamType=="collisions":
    ATLASCosmicFlags.doFilteredESD=False
else:
    ATLASCosmicFlags.doFilteredESD=True    
ATLASCosmicFlags.FilteredESDOutputFile = "cosmicsESD.filtered.pool.root"

#Muon calib ntuple
ATLASCosmicFlags.doCalibFillNt = True

# tracking ntuple
ATLASCosmicFlags.doTrkNtuple = False

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.histogramFile.set_Value_and_Lock( "myMonitoring.root" )
include ("RecExCommon/RecExCommon_topOptions.py")

if Debug:
    from IOVDbSvc.CondDB import conddb
    conddb.dumpFolderTags('iovfolderlist.txt',True)

if Input=='overlay':
   topSequence.CBNT_AthenaAware.CBNTAA_LArDigits.ContainerKey = "LArDigitContainer_MC_Thinned"
   topSequence.LArDigitThinner.InputContainerName = "LArDigitContainer_MC_Thinned"
   ServiceMgr.MboySvc.UseAllSegments=0 # standard way to run Muonboy on collisions
   topSequence.TileDigitsFilter.Enable = False

# svcMgr.ByteStreamNavigationProviderSvc.OutputLevel = DEBUG
