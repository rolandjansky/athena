from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecAlgsFlags  import recAlgs

#rec.ScopingLevel.set_Value_and_Lock(1)

if not 'Input' in dir():
    # uncomment if input is real data
    #Input='data'
    # uncomment if input is overlay of simulated Hits onto real data
    Input='overlay'
    # uncomment if input is simulation BS
    # Input='simbs'
    # uncomment if input is simulation RDO
    # Input='simrdo'

# uncomment if reading from TAG
#rec.readTAG=True


# input configuration (lock to prevent override)
if Input=='overlay':

    jobproperties.Beam.beamType.set_Value_and_Lock('collisions')


    TileFrameLength=7

    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.useShape=True
    larCondFlags.OFCShapeFolder = ""

    from LArROD.LArRODFlags import larRODFlags
    larRODFlags.readDigits = True

    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("")
    larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([
            'rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/pileup/MC_RandomTrigger.RDO.pool.root'])
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
    globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    globalflags.InputFormat.set_Value_and_Lock('pool')
    globalflags.DataSource.set_Value_and_Lock('data')
else:
    raise RuntimeError,"Wrong Input value"

athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
#athenaCommonFlags.SkipEvents = 0

doESD=False
doWriteESD=False
doCBNT=False
doHist=False
doAOD=False
doWriteAOD=False
doWriteBS=True
doMonitoring=False
doWriteTAG=False
doWriteTAGCOM=False
doNameAuditor=True

BSRDOOutput='AppName=Athena, OutputDirectory=./, FileTag=Release14.2.25.9, Run=0091890'

# the main jobOpt
include("RecExCommission/RecExCommission.py")                

# filtered ESD if not collisions

include ("RecExCommon/RecExCommon_topOptions.py")

ServiceMgr.MboySvc.UseAllSegments=0 # standard way to run Muonboy on collisions
ServiceMgr.TileInfoLoader.filenameDeltaPhysicsSuffix="of2_Delta_Phys_7Samples"

