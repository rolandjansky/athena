from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

if not 'RTTdata' in dir():
    RTTdata=False

if not 'RTTsim' in dir():
    RTTsim=False

if not 'ATNsim' in dir():
        ATNsim=False

if ATNsim:
    RTTsim=True
        
rec.readRDO=False



#uncomment if read BS
rec.readRDO=True
#uncomment if read AOD
#rec.readAOD=True
#uncomment if read ESD (note that cosmics DPD are best treated as ESD)
#rec.readESD=True

# uncomment if reading from TAG (for commissioning one of rec.readESD or rec.readRDO should be true)
#rec.readTAG=True
# note that a PoolFileCatalog with the files to navigate to should be available in the run directory
# note that there is problem 


#uncomment if want JiveXML file
#rec.doJiveXML=True


if RTTdata:
    Input = 'data'
    
if RTTsim:    
    Input = 'simrdo'

if RTTdata or RTTsim:
    Debug = False




if not 'Input' in dir():
    # uncomment if input is real data
    Input='data'
    # uncomment if input is simulation BS
    # Input='simbs'
    # uncomment if input is simulation RDO
    # Input='simrdo'

if not 'Debug' in dir():
    Debug=True  # change to True to have printout information


#specify beam type (default is "collisions", other values "cosmics" "singlebeam"
# pick the correct one
jobproperties.Beam.beamType = 'cosmics'
# singlebeam not tested recently
#jobproperties.Beam.beamType = 'singlebeam'






# input configuration (lock to prevent override)
# note that if reading from TAG, the input file spec is ignored,
#   however the flag settings should still be consistent 
if Input=='data':
    if rec.readTAG():
        # read TAG, navigate to ESD
        athenaCommonFlags.PoolTAGInput=['../cosm/myTAGCOMM.root']
        athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("TRTTracks>0")
    if rec.readRDO(): 
       if jobproperties.Beam.beamType() == 'cosmics':
           if RTTdata:
               # this is ignored if TAG are read
               athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-2._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-2._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-2._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-3._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-3._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-3._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-4._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-4._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-4._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-5._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-5._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-5._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-1._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-1._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-1._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-2._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-2._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-2._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-3._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-3._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-3._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-4._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-4._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-4._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-5._0001.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-5._0002.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0002.SFO-5._0003.data',
                                                                '/castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0003.SFO-1._0001.data'])
           else:
               athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data'])

           globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
           globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-00')
           # for running over data with field on need this to be setto true
           from AthenaCommon.BFieldFlags import jobproperties
           jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
           jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
           jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)

        
       else:
           # this is ignored if tag are read
           athenaCommonFlags.BSRDOInput.set_Value_and_Lock([
            '/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
           # Detector description #jamie, use version with field (even though solenoid is off)

           globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
           globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-00')
           # for running over data with field on need this to be setto true
           from AthenaCommon.BFieldFlags import jobproperties
           jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
           jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
           jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
        
       globalflags.InputFormat.set_Value_and_Lock('bytestream')



    if rec.readAOD():    
       if rec.readTAG():
           raise "Cannot have both readAOD and readTAG true"
       athenaCommonFlags.PoolAODInput.set_Value_and_Lock(["../cosm/myAOD.pool.root"])
       rec.doWriteAOD=False
       rec.doWriteESD=False 

    if rec.readESD():    #lxplus225
        ####L1CaloEM
        # this is ignored if tags are read
        athenaCommonFlags.PoolESDInput.set_Value_and_Lock(["../cosm/myESD.pool.root"])
        rec.doWriteESD=False

    if rec.readESD() or rec.readAOD():
        globalflags.InputFormat.set_Value_and_Lock('pool')
        globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
        globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-00')
        from AthenaCommon.BFieldFlags import jobproperties
        jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
        jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
        jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)



elif Input=='overlay':
    # not tested recently
    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.useShape=True

    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([
            'singleMuon100plusCosmics.RDO.020162._00002.pool.root'])

    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-03-00-00')
    globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-REPC-001-00')

    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
    jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
    jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)

    globalflags.InputFormat.set_Value_and_Lock('pool')
    
else:
    if Input=='simbs':
        #beware there might be sthg wrong in these BS files wrt ID cabling
        athenaCommonFlags.BSRDOInput.set_Value_and_Lock(
            ["/afs/cern.ch/user/g/gencomm/w0/jboyd/900Gev/BS/daq.0951001._0001.data",
             "/afs/cern.ch/user/g/gencomm/w0/jboyd/900Gev/BS/daq.0951001._0002.data"])
        globalflags.InputFormat.set_Value_and_Lock('bytestream')
        globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
        globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-00')
        doTileCosmicsTrigger=False
    elif Input=='simrdo':

        if jobproperties.Beam.beamType() == 'cosmics':
            globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-Comm-02-00-00')
            athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( ["/afs/cern.ch/atlas/maxidisk/d17/dig.trtbarrel_ma_B100.root"])
            from AthenaCommon.BFieldFlags import jobproperties
            jobproperties.BField.solenoidOn=True
            jobproperties.BField.barrelToroidOn=True
            jobproperties.BField.endcapToroidOn=True
            SpecialTileConfigHack=True
            globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-CSC-01-00-00')

        if jobproperties.Beam.beamType() == 'singlebeam':
            athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( ["/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/BeamGas/hijing.O.digit._00430.pool.root"])
            globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')
            from AthenaCommon.BFieldFlags import jobproperties
            jobproperties.BField.solenoidOn=True
            jobproperties.BField.barrelToroidOn=True
            jobproperties.BField.endcapToroidOn=True
            
        else:
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
#            globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
#            globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-00')
        # crashing :  No valid proxy for object TileTTL1CosmicsCnt  of type TileTTL1Container(CLID 2933)
            doTileCosmicsTrigger=False

    else:
      raise RuntimeError,"Wrong Input value"

    globalflags.DataSource.set_Value_and_Lock('geant4')    
    #doTileCosmicsTrigger=False

#hardcoded output specification (corresponds to optional flags from runArgs in transform skeleton)
rec.RootNtupleOutput.set_Value_and_Lock("commission.ntuple.root")
athenaCommonFlags.PoolESDOutput.set_Value_and_Lock("commissionESD.pool.root")
rec.RootHistoOutput.set_Value_and_Lock("monitoring.root")
if RTTdata or RTTsim:
    if not ATNsim:
        athenaCommonFlags.EvtMax.set_Value_and_Lock(500)
    else:
        athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
        
else:
    athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
#athenaCommonFlags.SkipEvents = 0


if RTTsim:
    rec.doHist=False
else:
    rec.doHist = True

rec.doWriteTAG.set_Value_and_Lock(False)	
rec.doWriteAOD=False
rec.doAOD=False
if RTTsim or Input=='overlay' or rec.readAOD():
    rec.doMonitoring.set_Value_and_Lock(False)
else:
    rec.doMonitoring.set_Value_and_Lock(True)    

if Input=='overlay':
   topSequence.LArRawChannelBuilder.DataLocation = "LArDigitContainer_MC_Thinned"
   topSequence.CBNT_AthenaAware.CBNTAA_LArDigits.ContainerKey = "LArDigitContainer_MC_Thinned"
   ServiceMgr.MboySvc.UseAllSegments=0 # standard way to run Muonboy on collisions
   topSequence.LArDigitThinner.Enable = False
   topSequence.TileDigitsFilter.Enable = False

if Debug:
    # DEBUG/VERBOSE options
    rec.doNameAuditor=True
    rec.doPerfMon=True
    rec.doDetailedPerfMon=True
    rec.doDumpProperties=True
    rec.doDumpTES=True
    # rec.OutputLevel = INFO


include("RecExCommission/RecExCommissionRepro.py")                

# filtered ESD calib and tracking ntuple if from RDO
if rec.readRDO():
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    DQMonFlags.histogramFile.set_Value_and_Lock( "myMonitoring.root" )

    #crashing in 14.5.X VAL 2 3 Feb
    include("TrigT1CTMonitoring/CTPFlags.py")
    CTPFlags.doCTRDO=False

if RTTsim:
    rec.doTrigger=False

#FIXME very temporary, crashing
#if rec.doJiveXML():
#    include.block("TrigJiveXML/TrigJiveXML_DataTypes.py")


include("RecExCommon/RecExCommon_topOptions.py")                

if Debug:
    from IOVDbSvc.CondDB import conddb
    conddb.dumpFolderTags('iovfolderlist.txt',True)


# svcMgr.ByteStreamNavigationProviderSvc.OutputLevel = DEBUG
