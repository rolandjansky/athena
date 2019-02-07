#**************************************************************
#
# jopOptions file for TileCal commissioning analysis
#
#==============================================================

from os import system
from subprocess import check_output
from subprocess import CalledProcessError

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

from AthenaCommon.Logging import logging
log = logging.getLogger( 'jobOptions_TileCalibRec.py' )

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

if 'ReadRDO' in dir() and ReadRDO:
    doSim = True
    ReadPool = True
    ReadRch = True
if ('ReadESD' in dir() and ReadESD) or ('ReadAOD' in dir() and ReadAOD):
    doSim = True
    ReadPool = True
    ReadRch = False
    doCaloCell = False
if 'doRecoESD' in dir() and doRecoESD:
    doSim = False
    ReadESD = True
    ReadPool = True
    ReadDigits = True
    ReadRch = True
    if not 'useRODReco' in dir():
        useRODReco = True # enable DSP results in ntuple
    if not 'TileUseDCS' in dir():
        TileUseDCS = True # setup for real data
else:
    doRecoESD=False
if not 'doSim' in dir():
    doSim = False
if not 'ReadPool' in dir():
    ReadPool = doSim
if not 'ReadDigits' in dir():
    ReadDigits = (not ReadPool)
if not 'ReadRch' in dir():
    ReadRch = ReadPool
Year = 0

if ReadPool:
    OldRun = False
    if not 'RunNumber' in dir():
        RunNumber = 0
    if not 'TilePhysRun' in dir():
        TilePhysRun = True
        TileRunType = 1

    # if FileName is set all other parameters below are not needed
    #if not 'FileName' in dir():
    #    FileName='DigitizationOutput.pool.root'

    # these parameters are tuned to read ESD or AOD for real data from CASTOR
    if not 'RunStream' in dir():
        RunStream = "physics_MinBias"
    if not 'DataProject' in dir():
        DataProject = "data15_cos"
    if not 'DirectorySuffix' in dir():
        DirectorySuffix = ""
    if not 'InputDirectory' in dir():
        if RunNumber < 171194:
            InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/0%(run)s/%(project)s.00%(run)s.%(stream)s.%(suff)s" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber, 'suff': DirectorySuffix })
        elif RunNumber < 254945:
            InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.%(suff)s" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber, 'suff': DirectorySuffix })
        else:
            InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.%(suff)s" % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber, 'suff': DirectorySuffix })


        RunFromLocal=False
    if not 'FileFilter' in dir():
        if 'ReadESD' in dir() and ReadESD:
            FileFilter = "ESD"
        elif 'ReadAOD' in dir() and ReadAOD:
            FileFilter = "AOD"
        else:
            FileFilter = "pool"

    if not 'doTileMon' in dir():
        doTileMon = False
    if not 'useRODReco' in dir():
        useRODReco = False
    if not 'TileUseDCS' in dir():
        TileUseDCS = False

    if 'ReadAOD' in dir() and ReadAOD:
        if not 'doTileMuId' in dir():
            doTileMuId = False
        if not 'doTileRODMuId' in dir():
            doTileRODMuId = False
        if not 'doTileMuonFitter' in dir():
            doTileMuonFitter = False
else:
    #=============================================================
    #=== ByteStream Input
    #=============================================================
    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 
    # frag to ROD mapping and additional options in external file
    svcMgr.ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ]
    if 'TileMap' in dir():
        include( TileMap )

    if not 'RunNumber' in dir():
        RunNumber = 0
    # runs with 9 samples
    OldRun = (RunNumber > 10 and RunNumber < 40000)

    if not 'TilePhysRun' in dir():
        TilePhysRun = False

    if TilePhysRun:
        TileRunType = 1
        if not 'RunStream' in dir():
           RunStream = "physics_L1Calo"
           log.warning('RunStream is not set up and will be used: %(stream)s' % {'stream': RunStream})
        if not 'DataProject' in dir():
           DataProject = "data15_cos"
           log.warning('DataProject is not set up and will be used: %(project)s' % {'project': DataProject})
        if not 'InputDirectory' in dir():
           if RunNumber < 10:
                InputDirectory = "."
                RunFromLocal = True
           elif RunNumber < 10800:
                InputDirectory = "/castor/cern.ch/atlas/P1commisioning/phaseII"
           elif RunNumber < 20000:
                InputDirectory = "/castor/cern.ch/atlas/P1commisioning/phaseIII"
           elif RunNumber < 28000:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ"
           elif RunNumber < 32053:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5"
           elif RunNumber < 32946:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/CaloWeekNov07"
           elif RunNumber < 39801:
                InputDirectory = "/castor/cern.ch/grid/atlas/t0/perm/DAQ/CaloWeekFeb08"
           elif RunNumber < 44520:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/M6"
           elif RunNumber < 54063:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/CaloWeekApr08"
           elif RunNumber < 75354:
                InputDirectory = "/castor/cern.ch/grid/atlas/DAQ/M7"
           elif RunNumber < 92227:
                InputDirectory = ("/castor/cern.ch/grid/atlas/DAQ/2008/%(run)s/%(stream)s" % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber < 100000:
                InputDirectory = ("/castor/cern.ch/grid/atlas/DAQ/2008/00%(run)s/%(stream)s" % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber < 142407:
                InputDirectory = ("/castor/cern.ch/grid/atlas/DAQ/2009/00%(run)s/%(stream)s" % { 'run': RunNumber, 'stream': RunStream })
           elif RunNumber < 171194:
                InputDirectory = ("/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/0%(run)s/%(project)s.00%(run)s.%(stream)s.merge.RAW" 
                                  % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
           elif RunNumber < 254945:
                InputDirectory = ( "/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.merge.RAW" 
                                   % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
           else:
               if not 'DirectorySuffix' in dir():
                   if RunStream.startswith('calibration'): DirectorySuffix = 'daq.RAW'
                   else: DirectorySuffix = 'merge.RAW'
                   log.warning('DirectorySuffix is not set up and will be used: %(suff)s' % {'suff': DirectorySuffix})

               InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.%(suff)s" 
                                  % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber, 'suff': DirectorySuffix })


    if not 'InputDirectory' in dir():
        if RunNumber < 10:
            InputDirectory = "."
            RunFromLocal = True
        elif RunNumber < 36127:
            InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2007/daq"
        elif RunNumber < 100000:
            InputDirectory = "/castor/cern.ch/atlas/testbeam/tilecal/2008/daq"
        else:
            if RunNumber < 142682:
                Year = 2009
            elif RunNumber < 171194:
                Year = 2010
            elif RunNumber < 194688:
                Year = 2011
            elif RunNumber < 216816:
                Year = 2012            
            elif RunNumber < 224305:
                Year = 2013            
            elif RunNumber < 248584:
                Year = 2014
            elif RunNumber < 287952:
                Year = 2015
            elif RunNumber < 314450:
                Year = 2016
            elif RunNumber < 342540:
                Year = 2017
            else:
                Year = 2018


            if 'RunStream' in dir():
                if RunStream == 'l1calo' or RunStream == 'L1Calo':
                    InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/l1calo/00%(run)s" % { 'run': RunNumber })
                elif RunStream.startswith('calibration_L1Calo'):
                    if not 'DataProject' in dir(): 
                        DataProject = 'data15_calib'
                        log.warning('DataProject is not set up and will be used: %(project)s' % {'project': DataProject})

                    InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s" 
                                       % {'project': DataProject, 'stream': RunStream, 'run': RunNumber })
                else:
                    if RunNumber < 254372:
                        InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/%(year)s/00%(run)s/%(stream)s" 
                                           % { 'year': Year, 'run': RunNumber, 'stream': RunStream })
                    else:
                        if not 'DataProject' in dir():
                            DataProject = 'data15_cos'
                            log.warning('DataProject is not set up and will be used: %(project)s' % {'project': DataProject})
                        
                        InputDirectory = ( "/eos/atlas/atlastier0/rucio/%(project)s/%(stream)s/00%(run)s/%(project)s.00%(run)s.%(stream)s.daq.RAW" 
                                           % { 'project': DataProject, 'stream': RunStream, 'run': RunNumber })
                        
            else:
                if RunNumber < 270317:
                    InputDirectory = ( "/castor/cern.ch/grid/atlas/DAQ/tile/%(year)s/daq" % { 'year': Year })
                else:
                    InputDirectory = ( '/eos/atlas/atlascerngroupdisk/det-tile/online/%(year)s/daq' % { 'year': Year })


    if not 'FileFilter' in dir():
        FileFilter = ".data"

if not 'RunFromLocal' in dir():
    if InputDirectory == ".":
        RunFromLocal = True
    else:
        RunFromLocal = False

def FindFile(path, runinput, filter):

    run = str(runinput).zfill(7) if int(runinput) > 0 else str(runinput)
    if len(filter) < 1: filter = '.'

    files = []
    fullname = []

    try:
        if RunFromLocal:
            files = check_output('ls %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter}, shell = True).splitlines()
        elif (path.startswith('/eos/')):
            files = check_output('xrdfs eosatlas ls -l %(path)s | grep %(run)s | grep -v "#" | grep -v -e "         [ 0-9][ 0-9][0-9] " | grep %(filt)s | sed "s|^.*/||" ' % {'path':path, 'run':run, 'filt':filter}, shell = True).splitlines()
        else:
            if (TilePhysRun and RunNumber > 28000 and RunNumber < 40000):
                files = check_output('nsls -l %(path)s | grep %(run)s | grep -v -e "               [ 0-9][ 0-9][0-9] " -e "hlterror" | grep %(filt)s | grep -e "b0000[01][01][01][01]" -e "physics.cosmics" | cut -c66- ' % {'path': path, 'run':run, 'filt':filter  }, shell = True).splitlines()
            elif (TilePhysRun and RunNumber > 40000 and RunNumber < 75354):
                files = check_output('nsls -l %(path)s | grep %(run)s | grep -v -e "               [ 0-9][ 0-9][0-9] " -e "hlterror" | grep %(filt)s | grep -e "NIM0" -e "[pP]hysics" | cut -c66- ' % {'path': path, 'run':run, 'filt':filter}, shell = True).splitlines()
            else:
                files = check_output('nsls %(path)s | grep %(run)s | grep -v -e "               [ 0-9][ 0-9][0-9] " | grep %(filt)s ' % {'path': path, 'run':run, 'filt':filter  }, shell = True).splitlines()
    
    except CalledProcessError:
        files = []
        log.warn('Seems there are no such directory: ' + path)

    for file_name in (files):
        try:
            good=(not file_name in open('/afs/cern.ch/user/t/tilebeam/ARR/bad_data_files').read())
            if good: good=(not file_name in open('/afs/cern.ch/user/t/tiledaq/public/bad_data_files').read())
        except:
            good=True
        if good:
            if (path.startswith('/eos/')):
                fullname.append('root://eosatlas/' + path + '/' + file_name)
            elif ReadPool and not RunFromLocal:
                fullname.append('rfio:' + path + '/' + file_name) # maybe castor: ?
            elif (path.startswith('/castor/')):
                fullname.append('root://castoratlas/' + path + '/' + file_name)
            else:
                fullname.append(path + '/' + file_name)
        else:
            print "Excluding known bad data file",file_name

    return [fullname, run]


if not 'FileNameVec' in dir():
    if not 'FileName' in dir() or FileName == "":    
        tmp = FindFile(InputDirectory, RunNumber, FileFilter)
        FileNameVec = tmp[0]
        FormattedRunNumber = tmp[1]
    else:
        FileNameVec = [ FileName ]
        FormattedRunNumber = RunNumber
else:
    FormattedRunNumber = RunNumber

log.info("InputDirectory is " + str(InputDirectory))
log.info("RunNumber was " + str(RunNumber))
log.info("RunNumber is " + str(FormattedRunNumber))
log.info("FullFileName is " + str(FileNameVec))

if len(FileNameVec) < 1:
    log.fatal("Input file not found")
    sys.exit(1)

if not 'Version' in dir():
    Version = "0"

if not 'OutputDirectory' in dir():
    OutputDirectory = "/tmp/Reco-" + str(RunNumber) + "-" + Version + "/"

system('mkdir -p %s' % (OutputDirectory))

if not 'EvtMin' in dir():
    EvtMin = 0
    # if (not 'doTileMon' in dir() or doTileMon):
    #    EvtMin = 1
    # else:
    #    EvtMin = 0
    EvtMinNotSet = True

if not 'EvtMax' in dir():
    EvtMax = 5000000

#-----------------------------------
# TileCal reconstruction for commissioning
#-----------------------------------

# if run type is set to non-zero value, it overrides event trig type
if not 'TileRunType' in dir():
    TileRunType = 0; # 1 - physics, 2 - laser, 4 - pedestal, 8 - CIS run 

# if noise filter type is not set - disable it
if not 'TileNoiseFilter' in dir():
    TileNoiseFilter = 0; # 0 - no filter, 1 - Shimpei filter

# override run type with one of these flags
if not 'TileCisRun' in dir():
    TileCisRun = False
if not 'TileMonoRun' in dir():
    TileMonoRun = False
if not 'TilePedRun' in dir():
    TilePedRun = False
if not 'TileLasRun' in dir():
    TileLasRun = False
if not 'TilePhysRun' in dir():
    TilePhysRun = False
if not 'TileRampRun' in dir():
    TileRampRun = False
if not 'TileL1CaloRun' in dir():
    TileL1CaloRun = False

TileLasPulse = TileLasRun
TileCisPulse = (TileCisRun or TileMonoRun or TileRampRun or TileL1CaloRun)

if not 'TilePhysTiming' in dir():
    TilePhysTiming = False

if TileCisRun:
    TileRunType = 8
    TileBiGainRun = True

    # do not convert ADC counts to pCb
    if not 'TileCalibrateEnergy' in dir():
        TileCalibrateEnergy = False

    # skip all events when just one channel is fired (4*48)
    if 'EvtMinNotSet' in dir():
        EvtMin = 192

elif TileMonoRun:
    TileRunType = 8
    TileBiGainRun = False

    # use pCb units for ntuple
    if not 'TileOfflineUnits' in dir():
        TileOfflineUnits = 1

elif TileRampRun:
    TileRunType = 8
    TileBiGainRun = False

    # use pCb units for ntuple
    if not 'TileOfflineUnits' in dir():
        TileOfflineUnits = 1

elif TileL1CaloRun:
    TileRunType = 8
    TileBiGainRun = False

    # use pCb units for ntuple
    if not 'TileOfflineUnits' in dir():
        TileOfflineUnits = 1

elif TilePedRun:
    TileRunType = 4
    TileBiGainRun = True

elif TileLasRun:
    TileRunType = 2
    TileBiGainRun = False

    # use pCb units for ntuple
    if not 'TileOfflineUnits' in dir():
        TileOfflineUnits = 1

else:
    if TilePhysRun:
        TileRunType = 1
    TileBiGainRun = False

    if not 'doLab' in dir():
        doLab = False

    if not 'doTileMuId' in dir():
        doTileMuId = doLab

    if not 'doTileRODMuId' in dir():
        doTileRODMuId = False

    if not 'doTileMuonFitter' in dir():
        doTileMuonFitter = doLab

    if not 'doCaloNtuple' in dir():
        doCaloNtuple = True

    if not 'doCaloTopoCluster' in dir():
        doCaloTopoCluster = False

# Compare mode to compare frag5 with frag0 and frag4
if not 'TileCompareMode' in dir():
    TileCompareMode = False

# emulate DSP reco offline and use those results in noise algorithm
if not 'TileEmulateDSP' in dir():
   TileEmulateDSP = False

# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder
# by default we use 3 methods - Fit, Opt2 and OptAtlas

if not 'doTileFlat' in dir():
    doTileFlat = False

if not 'doTileOpt' in dir():
    doTileOpt = False

if not 'doTileOpt2' in dir():
    doTileOpt2 = not TileCompareMode and ReadDigits

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = not TileCompareMode and ReadDigits

if not 'doTileMF' in dir():
    doTileMF = False

if not 'doTileOF1' in dir():
    doTileOF1 = False

if not 'doTileFit' in dir():
    doTileFit = not TileCompareMode and ReadDigits

if not 'doTileFitCool' in dir():
    doTileFitCool = False

if not 'TileOF1Ped' in dir():
    TileOF1Ped = -1

# for the moment enable TMDB only for data
if not 'useTMDB' in dir():
    useTMDB = not doSim

if TileBiGainRun:
    # only 7 samples are expected
    if not 'TileFrameLength' in dir():
        TileFrameLength = 7

    # do not put DSP resutls to ntuple
    if not 'useRODReco' in dir():
        useRODReco = False
else:
    # select correct number of samples
    if not 'TileFrameLength' in dir():
        if OldRun: TileFrameLength = 9
        else:      TileFrameLength = 7

    # put DSP resutls to ntuple
    if not 'useRODReco' in dir():
        useRODReco = True

if not 'useDSPUnits' in dir():
    useDSPUnits = False
elif useDSPUnits:
    TileCalibrateEnergy = False

if not 'PhaseFromCOOL' in dir():
    PhaseFromCOOL = True

if not 'OfcFromCOOL' in dir():
    if TilePhysRun or TilePedRun:
        OfcFromCOOL = True
    else:
        OfcFromCOOL = False

if useRODReco or doTileOpt2 or doTileMF or doTileOF1 or doTileOptATLAS or doTileFitCool or TileCompareMode or not 'TileUseCOOL' in dir():
    TileUseCOOL = True
    TileUseCOOLOFC = not ReadPool or OfcFromCOOL

if not 'TileUseDCS' in dir():
    TileUseDCS = True

# create monitoring histograms
if not 'doTileMon' in dir():
    doTileMon = False

# calculate calibration constants and store them in ROOT file
if not 'doTileCalib' in dir():
    doTileCalib = False

# all other parameters which can be set to True or False
# from the command line 

# use PMT ordering in ntuple (convert channel to PMT number) 
if not 'doTileCable' in dir():
    doTileCable = False

# convert ADC counts to MeV in output ntuple by default
if not 'TileCalibrateEnergy' in dir():
    TileCalibrateEnergy = True

# create TileRec/h2000 ntuple
if not 'doTileNtuple' in dir():
    doTileNtuple=True

# store results of reconstruction in POOL file
if not 'doCreatePool' in dir():
    doCreatePool=False

# run v-atlas event display
if not 'doEventDisplay' in dir():
    doEventDisplay=False

# prepare XML files for Atlantis
if not 'doAtlantis' in dir():
    doAtlantis=False

# create CALO D3PD with cell by cell info
if not 'doCaloNtuple' in dir():
    doCaloNtuple = False

# create TileTowers
if not 'doTileTower' in dir():
    doTileTower=False

# run TileMuId
if not 'doTileMuId' in dir():
    doTileMuId = False

# run TileRODMuId
if not 'doTileRODMuId' in dir():
    doTileRODMuId = False

# run TileMuonFitter
if not 'doTileMuonFitter' in dir():
    doTileMuonFitter = False

# run TopoClustering
if not 'doCaloTopoCluster' in dir():
    doCaloTopoCluster = False

# check if we want to create D3PD
if (doCaloNtuple or doTileMuId or doTileRODMuId or doTileMuonFitter or doCaloTopoCluster):
    doD3PD = True
else:
    doD3PD = False

# check if we want to create noise monitoring plots
if not 'doTileCellNoiseMon' in dir(): 
    doTileCellNoiseMon = False
if not 'doTileDigiNoiseMon' in dir():
    doTileDigiNoiseMon = doTileCellNoiseMon or TilePedRun
if not 'doTileRawChannelNoiseMon' in dir():
    doTileRawChannelNoiseMon = doTileCellNoiseMon or TilePedRun

# check if we want to create TMDB monitoring plots
if not 'doTileTMDBRawChannel' in dir():
    doTileTMDBRawChannel = useTMDB

if not 'doTileTMDBDigitsMon' in dir():
    doTileTMDBDigitsMon = useTMDB

if not 'doTileTMDBRawChannelMon' in dir():
    doTileTMDBRawChannelMon = useTMDB and doTileTMDBRawChannel

# check if we need to create TileCells
if not 'doCaloCell' in dir():
    if (doD3PD or doCaloNtuple or doCreatePool or doEventDisplay or doAtlantis or (doTileMon and (TilePhysRun or doTileCellNoiseMon))):
        doCaloCell = True
    else:
        doCaloCell = False

if not 'TileD3PDSavePosition' in dir():
    TileD3PDSavePosition = False

#---------------
# end of options
#---------------

#=============================================================
#=== init Det Descr
#=============================================================

from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
if ReadPool:
    if TileUseDCS:
        globalflags.DataSource.set_Value_and_Lock('data')
    else:
        globalflags.DataSource.set_Value_and_Lock('geant4')
    globalflags.InputFormat.set_Value_and_Lock('pool')
else:
    globalflags.DataSource.set_Value_and_Lock('data')
    globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.BeamFlags import jobproperties
#jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')
jobproperties.Beam.beamType.set_Value_and_Lock('collisions')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(FileNameVec)

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()
if TileL1CaloRun:
    DetFlags.detdescr.LVL1_setOn()
if ReadPool:
    DetFlags.readRDOPool.Tile_setOn()
    if TileL1CaloRun:
        DetFlags.readRDOPool.LVL1_setOn()
else:
    DetFlags.readRDOBS.Tile_setOn()
    if TileL1CaloRun:
        DetFlags.readRDOBS.LVL1_setOn()
DetFlags.Print()

from RecExConfig.RecFlags import rec
rec.doLArg = False

# Get project name from file name and use it in RecFlags
# in order to set up right database instance  in condb
projectName = FileNameVec[0].split('/').pop().split('.')[0]
rec.projectName = projectName
rec.RunNumber = int(RunNumber)

if not 'RUN2' in dir(): 
    RUN2 = False
if globalflags.DataSource() == 'data':
    if Year > 2014 or RunNumber > 232000 or projectName.startswith("data15_") or RUN2:
        RUN2 = True
    else: 
        # use RUN1 DB for runs taken before Jul-2014
        if projectName.startswith("data14_"): rec.projectName = "data13_tilecomm"
        globalflags.DatabaseInstance = "COMP200"


if not 'doTileRawChannelTimeMonTool' in dir():
    doTileRawChannelTimeMonTool = (TileRunType == 2) and TilePhysTiming and RUN2 and doTileFit


from IOVDbSvc.CondDB import conddb
from AthenaCommon.GlobalFlags import jobproperties
if ReadPool:
    #---  Load PartProp into the Detector store ---------------
    if not hasattr(svcMgr, 'PartPropSvc'):
        from PartPropSvc.PartPropSvcConf import PartPropSvc
        svcMgr += PartPropSvc()
    #--- Pool specific --------------------------------------------
    # - General Pool converters
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    # - Pool input 
    svcMgr.EventSelector.InputCollections = FileNameVec
    # Set Geometry version
    if not 'DetDescrVersion' in dir():
        if RUN2:
            DetDescrVersion = 'ATLAS-R2-2015-04-00-00'
        else:
            DetDescrVersion = 'ATLAS-R1-2012-02-00-00'
else:
    # - ByteStream input 
    svcMgr.EventSelector.Input = FileNameVec
    # Set Global tag for IOVDbSvc
    if not 'CondDbTag' in dir():
        if RUN2:
            if 'UPD4' in dir() and UPD4: CondDbTag = 'CONDBR2-BLKPA-2018-03'
            else:                        CondDbTag = 'CONDBR2-ES1PA-2018-02'
        else:
            if 'UPD4' in dir() and UPD4 and RunNumber > 141066: CondDbTag = 'COMCOND-BLKPA-RUN1-06'
            else:                                               CondDbTag = 'COMCOND-ES1PA-006-05'

    jobproperties.Global.ConditionsTag = CondDbTag        
    conddb.setGlobalTag(CondDbTag)
    # Set Geometry version
    if not 'DetDescrVersion' in dir():
        if RUN2:
            DetDescrVersion = 'ATLAS-R2-2015-04-00-00'
        else:
            DetDescrVersion = 'ATLAS-R1-2012-02-00-00'
jobproperties.Global.DetDescrVersion = DetDescrVersion 
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion()) )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )
#GeoModelSvc.TileVersionOverride = "TileCal-GEO-08"
#log.info( "GeoModelSvc.TileVersionOverride = %s" % (GeoModelSvc.TileVersionOverride) )


#=============================================================
#=== setup all options for optimal filter
#=============================================================

if not 'TileCorrectAmplitude' in dir():
    TileCorrectAmplitude = False;  # don't do parabolic correction in OptATLAS

if not 'TileCorrectTime' in dir():
    if TilePhysRun or TilePhysTiming:
        TileCorrectTime = True;  # APPLY time correction in physics runs
    else:
        TileCorrectTime = False;  # do not apply time correction - to be compatible with DSP reco

if not 'doTileOverflowFit' in dir():
    doTileOverflowFit = False
        
include( "TileRec/TileDefaults_jobOptions.py" )

from TileRecUtils.TileRecFlags import jobproperties

jobproperties.TileRecFlags.readDigits = ReadDigits
jobproperties.TileRecFlags.noiseFilter = TileNoiseFilter
jobproperties.TileRecFlags.TileRunType = TileRunType
jobproperties.TileRecFlags.calibrateEnergy = False;  # don't need pC in raw channels, keep ADC counts
if 'BunchSpacing' in dir():
    halfBS = BunchSpacing/2.
    if halfBS > 25.1 or halfBS < 0.1:
        log.info("Bad bunch spacing %s, keeping default limits for parabolic correction" % BunchSpacing)
    else:
        jobproperties.TileRecFlags.TimeMinForAmpCorrection = -halfBS
        jobproperties.TileRecFlags.TimeMaxForAmpCorrection = halfBS

jobproperties.TileRecFlags.doTileOverflowFit = doTileOverflowFit

# if those two parameters are set to true, everything is loaded correctly,
# but only for physics runs, i.e. it will be wrong for cis or laser runs.
# so, we set them to false here, but then load proper OFC and phases
# for every type of run
jobproperties.TileRecFlags.OfcFromCOOL = OfcFromCOOL
jobproperties.TileRecFlags.BestPhaseFromCOOL = PhaseFromCOOL

jobproperties.print_JobProperties('tree&value')

#=============================================================
#=== setup TileConditions
#=============================================================
include( "TileConditions/TileConditions_jobOptions.py" )
tileInfoConfigurator.OutputLevel = OutputLevel
# use correct timing constants for different run types
from TileConditions.TileCondToolConf import *
if TilePhysTiming:
    if RUN2 and TileLasPulse: tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','GAPLAS')
    else:                     tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','PHY')
elif TileLasPulse:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','LAS')
elif TileCisPulse:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','CIS')
else:
    tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','PHY')
print tileInfoConfigurator

#============================================================
#=== configure TileCondToolOfcCool
#============================================================
OfcFromCoolOF1 = doTileOF1 and OfcFromCOOL and (conddb.GetInstance() == 'CONDBR2') # there are OFCs for OF1 only in CONDBR2

#============================================================
#=== configure TileCondToolOfc
#============================================================
if not OfcFromCOOL and (doTileOpt2 or doTileOptATLAS or doTileOF1):
    from TileConditions.TileConditionsConf import TileCondToolOfc
    tileCondToolOfc = TileCondToolOfc()
    tileCondToolOfc.nSamples = TileFrameLength # default = 7
    tileCondToolOfc.OptFilterDeltaCorrelation = False # False - use matrix from DB
    tileCondToolOfc.CacheSize = 0 # (max phases per channel) 0-no cache (default)
    tileCondToolOfc.OutputLevel = OutputLevel

    ToolSvc += tileCondToolOfc

    #  'LAS' or "CIS" or 'PHY' pulse shape

    print tileCondToolOfc

#============================================================
#=== adding Event Info
#============================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if not 'newRDO' in dir() or newRDO is None:
    if 'ReadRDO' in dir() and ReadRDO:
        from RecExConfig.InputFilePeeker import inputFileSummary
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])
        newRDO = objKeyStore.isInInput( "xAOD::EventInfo" )
    else:
        newRDO = True

if ReadPool and newRDO:
    topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
else:
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence+=xAODMaker__EventInfoCnvAlg()

#=============================================================
#=== read ByteStream and reconstruct data
#=============================================================
if not ReadPool:
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )
    include( "TileRec/TileRec_jobOptions.py" )

    from TileByteStream.TileByteStreamConf import TileROD_Decoder
    ToolSvc += TileROD_Decoder()
    ToolSvc.TileROD_Decoder.TileCondToolTiming = tileInfoConfigurator.TileCondToolTiming
    if TileCompareMode:
        ToolSvc.TileROD_Decoder.useFrag5Raw  = True
        ToolSvc.TileROD_Decoder.useFrag5Reco = True

        TilePulseTypes = {0 : 'PHY', 1 : 'PHY', 2 : 'LAS', 4 : 'PHY', 8 : 'CIS'}
        TilePulse = TilePulseTypes[jobproperties.TileRecFlags.TileRunType()]

        tileInfoConfigurator.setupCOOLOFC(type = TilePulse)
        ToolSvc.TileROD_Decoder.TileCondToolOfcCool = ToolSvc.TileCondToolOfcCool
else:
    if ReadDigits:
        from TileRecUtils.TileRawChannelGetter import *
        theTileRawChannelGetter=TileRawChannelGetter()
        if doRecoESD:
            topSequence.TileRChMaker.TileDigitsContainer="TileDigitsFlt"
        ToolSvc.TileBeamInfoProvider.TileDigitsContainer="TileDigitsFlt"
        ToolSvc.TileBeamInfoProvider.TileRawChannelContainer=""

if doTileFit:
    ToolSvc.TileRawChannelBuilderFitFilter.MaxTimeFromPeak = 250.0; # recover behaviour of rel 13.0.30  
    ToolSvc.TileRawChannelBuilderFitFilter.RMSChannelNoise = 3; 
    ToolSvc.TileRawChannelBuilderFitFilter.UseDSPCorrection = not TileBiGainRun

    print ToolSvc.TileRawChannelBuilderFitFilter

if doTileFitCool:
    ToolSvc.TileRawChannelBuilderFitFilterCool.MaxTimeFromPeak = 250.0; # recover behaviour of rel 13.0.30  
    ToolSvc.TileRawChannelBuilderFitFilterCool.UseDSPCorrection = not TileBiGainRun
    
    print ToolSvc.TileRawChannelBuilderFitFilterCool

if doTileOpt2:

    if TileMonoRun or TileRampRun:
        ToolSvc.TileRawChannelBuilderOpt2Filter.MaxIterations = 3 # 3 iterations to match DSP reco
        if TileCompareMode or TileEmulateDSP:
            ToolSvc.TileRawChannelBuilderOpt2Filter.EmulateDSP = True # use dsp emulation
    ToolSvc.TileRawChannelBuilderOpt2Filter.UseDSPCorrection = not TileBiGainRun
    
    print ToolSvc.TileRawChannelBuilderOpt2Filter

if doTileOptATLAS:
    if ReadPool:
        ToolSvc.TileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelFixed"

    if PhaseFromCOOL:
        ToolSvc.TileRawChannelBuilderOptATLAS.TileCondToolTiming = tileInfoConfigurator.TileCondToolTiming
        ToolSvc.TileRawChannelBuilderOptATLAS.correctTime = False; # do not need to correct time with best phase

    ToolSvc.TileRawChannelBuilderOptATLAS.BestPhase   = PhaseFromCOOL; # Phase from COOL or assume phase=0
    if TileCompareMode or TileEmulateDSP:
        ToolSvc.TileRawChannelBuilderOptATLAS.EmulateDSP = True # use dsp emulation
    ToolSvc.TileRawChannelBuilderOptATLAS.UseDSPCorrection = not TileBiGainRun

    print ToolSvc.TileRawChannelBuilderOptATLAS
    
if doTileMF:

    if PhaseFromCOOL:
        ToolSvc.TileRawChannelBuilderMF.TileCondToolTiming = tileInfoConfigurator.TileCondToolTiming
        ToolSvc.TileRawChannelBuilderMF.correctTime = False; # do not need to correct time with best phase

    ToolSvc.TileRawChannelBuilderMF.BestPhase   = PhaseFromCOOL; # Phase from COOL or assume phase=0
    ToolSvc.TileRawChannelBuilderMF.UseDSPCorrection = not TileBiGainRun

    if OfcFromCOOL and not doTileFitCool:
        TilePulseTypes = {0 : 'PHY', 1 : 'PHY', 2 : 'LAS', 4 : 'PHY', 8 : 'CIS'}
        TilePulse = TilePulseTypes[jobproperties.TileRecFlags.TileRunType()]

        tileInfoConfigurator.setupCOOLPULSE(type = TilePulse)
        tileInfoConfigurator.setupCOOLAutoCr()

    print ToolSvc.TileRawChannelBuilderMF 

if doTileOF1:
    ToolSvc.TileRawChannelBuilderOF1.PedestalMode = TileOF1Ped  

    if PhaseFromCOOL:
        ToolSvc.TileRawChannelBuilderOF1.TileCondToolTiming = tileInfoConfigurator.TileCondToolTiming
        ToolSvc.TileRawChannelBuilderOF1.correctTime = False # do not need to correct time with best phase

    ToolSvc.TileRawChannelBuilderOF1.BestPhase   = PhaseFromCOOL # Phase from COOL or assume phase=0
    if TileCompareMode or TileEmulateDSP:
        ToolSvc.TileRawChannelBuilderOF1.EmulateDSP = True # use dsp emulation
    ToolSvc.TileRawChannelBuilderOF1.UseDSPCorrection = not TileBiGainRun

    print ToolSvc.TileRawChannelBuilderOF1    

if (doEventDisplay or doCreatePool):
    # create TileHit from TileRawChannel and store it in TileHitVec
    from TileRecAlgs.TileHitFromRawChGetter import *
    theTileHitFromRawChGetter = TileHitFromRawChGetter()
    theTileHitFromRawChGetter.TileRawChannelToHit().TileRawChannelContainer = "TileRawChannelOpt2"
    theTileHitFromRawChGetter.TileRawChannelToHit().UseSamplFract = False

    print theTileHitFromRawChGetter.TileRawChannelToHit()

if doCaloCell:
   # create TileCell from TileRawChannel and store it in CaloCellContainer
   if TileBiGainRun: 
       include( "TileRec/TileCellMaker_jobOptions_doublegain.py" )
       ToolSvc.TileCellBuilderLG.SkipGain = 1
       ToolSvc.TileCellBuilderHG.SkipGain = 0
       if OldRun: # disable masking on the fly
           ToolSvc.TileCellBuilderLG.TileDSPRawChannelContainer=""
           ToolSvc.TileCellBuilderHG.TileDSPRawChannelContainer=""
   else: 
       include( "TileRec/TileCellMaker_jobOptions.py" )
       if OldRun: # disable masking on the fly
           ToolSvc.TileCellBuilder.TileDSPRawChannelContainer=""
       if doRecoESD:
           topSequence.CaloCellMaker.CaloCellsOutputName = "AllCaloNewReco"
           ToolSvc.TileCellBuilder.MBTSContainer = "MBTSContainerNewReco"
           ToolSvc.TileCellBuilder.E4prContainer = "E4prContainerNewReco"
           ToolSvc.TileCellBuilder.TileDSPRawChannelContainer=""

if doTileTower:
    include( "CaloRec/CaloCombinedTower_jobOptions.py" )
    #NB: ONLY Tile Towers 
    CmbTowerBldr.TowerBuilderTools=[ "TileTowerBuilderTool/TileCmbTwrBldr" ]
    if doRecoESD:
        TileCmbTwrBldr.CellContainerName = "AllCaloNewReco"
        CmbTowerBldr.TowerContainerName="CombinedTowerNewReco"

if doCaloTopoCluster :
    if not 'includeLAr' in dir():
        includeLAr=False
    include ("TileMonitoring/TileMonTopoCluster_jobOptions.py")
    if doRecoESD:
        TileTopoMaker.CellsName = "AllCaloNewReco"

if doTileMuId:
    include ("TileMuId/TileMuId_cosmics_jobOptions.py")
    if doRecoESD:
        theTileLookForMuAlg.CellsNames = "AllCaloNewReco"
        theTileLookForMuAlg.TileMuTagsOutputName = "MuObjNewReco"

if doTileMuonFitter:
    include( "TileCosmicAlgs/TileMuonFitter_jobOptions.py")
    if doLab:
        theTileCosmicMuonGetter.TileMuonFitter().BeamType   = 'collisions'
        doTileMon=True
    if doRecoESD:
        theTileCosmicMuonGetter.TileMuonFitter().CaloCellContainer = "AllCaloNewReco"


if doTileTMDBRawChannel:
    # Set up TileCondToolPulseShape to be used in
    # TileCondToolOfc
    from TileConditions.TileCondToolConf import getTileCondToolMuRcvPulseShape
    ToolSvc += getTileCondToolMuRcvPulseShape('FILE', 'TileCondToolMuRcvPulseShape')
    
    # Set up TileCondToolOfc to be used in TileRawChannelBuilderMF
    ToolSvc += CfgMgr.TileCondToolOfc(name = 'TileCondToolMuRcvOfc'
                                      , OptFilterDeltaCorrelation = True
                                      , TileCondToolPulseShape = ToolSvc.TileCondToolMuRcvPulseShape)
    
    
    # Set up TileRawChannelBuilderOpt2 to be used
    ToolSvc += CfgMgr.TileRawChannelBuilderOpt2Filter(name = 'TileMuRcvRawChannelBuilderOpt2'
                                                      , TileRawChannelContainer = 'TileMuRcvRawChannelOpt2'
                                                      , PedestalMode = 1
                                                      , Minus1Iteration = TRUE
                                                      , calibrateEnergy = False
                                                      , correctTime = False
                                                      , TileCondToolOfc = ToolSvc.TileCondToolMuRcvOfc)
    
    
    topSequence += CfgMgr.TileRawChannelMaker(name = 'TileMuRcvRChMaker'
                                              , TileDigitsContainer = 'MuRcvDigitsCnt'
                                              , TileRawChannelBuilder = [ ToolSvc.TileMuRcvRawChannelBuilderOpt2 ])

if (doTileNtuple or doD3PD):

    theApp.HistogramPersistency = "ROOT"

    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    exec 'svcMgr.THistSvc.Output += [ "AANT DATAFILE=\'%(dir)s/tile_%(RunNum).f_%(Version)s.aan.root\' OPT=\'RECREATE\' " ] ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }
    svcMgr.THistSvc.MaxFileSize = 32768

    from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
    topSequence += AANTupleStream( "AANTupleStream1" )
    AANTupleStream1 = topSequence.AANTupleStream1
    AANTupleStream1.ExtraRefNames = [ "StreamESD","StreamRDO" ]
    exec 'AANTupleStream1.OutputName = \'%(dir)s/tile_%(RunNum).f_%(Version)s.aan.root\' ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }
    AANTupleStream1.ExistDataHeader = False


if doD3PD:

    def _args (level, name, kwin, **kw):
        kw = kw.copy()
        kw['level'] = level
        for (k, v) in kwin.items():
            if k.startswith (name + '_'):
                kw[k[len(name)+1:]] = v
        return kw

    def TileD3PD (file,
                 level = 4,
                 tuplename = 'caloD3PD',
                 allCells = True,
                 seq = topSequence,
                 D3PDSvc = 'D3PD::RootD3PDSvc',
                 streamNameRoot = None,
                 **kw):
        from D3PDMakerCoreComps.MakerAlg import MakerAlg

        alg = MakerAlg(tuplename, seq, file = file ,D3PDSvc =D3PDSvc,streamNameRoot =streamNameRoot)

        if doCaloNtuple:
            from CaloD3PDMaker.TileDetailsD3PDObject import TileDetailsD3PDObject
            from CaloD3PDMaker.CaloInfoD3PDObject import CaloInfoD3PDObject
            from CaloD3PDMaker.MBTSD3PDObject import MBTSD3PDObject

            if doRecoESD and doCaloCell:
                alg += TileDetailsD3PDObject (**_args(1, 'TileDetails',kw, sgkey='AllCaloNewReco', prefix='tile_', \
                                                              Kinematics_WriteEtaPhi = True, TileDetails_SavePositionInfo = TileD3PDSavePosition))
                    
                alg += CaloInfoD3PDObject (**_args(0, 'CaloInfo',kw, sgkey='AllCaloNewReco', prefix='calo_'))
                alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix='mbts_', sgkey='MBTSContainerNewReco'))

#                if RUN2:
#                    alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'e4pr_', sgkey = 'E4prContainerNewReco'))

            else:
                if TileBiGainRun:
                    if allCells:
                        alg += TileDetailsD3PDObject (**_args(1, 'TileDetails', kw, sgkey = 'AllCaloHG', prefix = 'tile_', \
                                                                  Kinematics_WriteEtaPhi = True, TileDetails_SavePositionInfo = TileD3PDSavePosition))
                        alg += CaloInfoD3PDObject (**_args(0, 'CaloInfo', kw, sgkey = 'AllCaloHG', prefix = 'calo_'))

                    alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'mbts_', sgkey = 'MBTSContainerHG'))
                    alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'mbtsLG_', sgkey = 'MBTSContainerLG'))

                    if RUN2:
                        alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'e4pr_', sgkey = 'E4prContainerHG', MBTS_SaveEtaPhiInfo = False))
                        alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'e4prLG_', sgkey = 'E4prContainerLG', MBTS_SaveEtaPhiInfo = False))
                else:
                    if allCells:
                        alg += TileDetailsD3PDObject (**_args(1, 'TileDetails', kw, sgkey = 'AllCalo', prefix = 'tile_', \
                                                              Kinematics_WriteEtaPhi = True, TileDetails_SavePositionInfo = TileD3PDSavePosition))
                        alg += CaloInfoD3PDObject (**_args(0, 'CaloInfo', kw, sgkey = 'AllCalo', prefix = 'calo_'))

                    alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'mbts_', sgkey = 'MBTSContainer'))

                    if RUN2:
                        alg += MBTSD3PDObject (**_args(1, 'MBTS', kw, prefix = 'e4pr_', sgkey = 'E4prContainer', MBTS_SaveEtaPhiInfo = False))

        if doCaloTopoCluster:
            from CaloD3PDMaker.xAODClusterD3PDObject import xAODClusterD3PDObject

            if not includeLAr:
                from CaloD3PDMaker import ClusterSamplingFillerTool
                ClusterSamplingFillerTool.Samplings=[12,13,14,15,16,17,18,19,20]

            alg+= xAODClusterD3PDObject(**_args(3, 'topo_cl',kw, sgkey='TileTopoCluster', prefix='topo_'))

        if doTileMuId:
            from CaloD3PDMaker.TileMuD3PDObject import TileMuD3PDObject
            alg += TileMuD3PDObject(**_args(0,'TileMus',kw,sgkey='TileMuObj',prefix='tilemuid_'))

        if doTileRODMuId:
            from CaloD3PDMaker.TileL2D3PDObject import TileL2D3PDObject
            alg += TileL2D3PDObject(**_args(2,'TileL2s',kw,exclude=['TileL2'],sgkey='TileL2Cnt',prefix='tilemurod_'))

        if doTileMuonFitter:
            from CaloD3PDMaker.TileCosmicMuonD3PDObject import TileCosmicMuonD3PDObject
            if not 'doTMFMethod' in dir():
                doTMFMethod = 'Hough'

            if(doTMFMethod == 'Hough' or doTMFMethod == 'Both'):
                alg += TileCosmicMuonD3PDObject(**_args(2,'TileCosMusHT',kw,sgkey='TileCosmicMuonHT',prefix='TileCosmicsHT_'))

            if(doTMFMethod == 'Minuit' or doTMFMethod == 'Both'):
                alg += TileCosmicMuonD3PDObject(**_args(2,'TileCosMusMF',kw,sgkey='TileCosmicMuonMF',prefix='TileCosmicsMF_'))

        return alg

    tupleName= '%(dir)s/tile_%(RunNum).f_%(Version)s.aan.root' %{'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }
    allC = not ('MBTSonly' in dir() and MBTSonly)
    TheAlg = TileD3PD(file = tupleName , seq = topSequence, allCells = allC)

if doTileNtuple:
    # create TileCal standalone ntuple
    include( "TileRec/TileNtuple_jobOptions.py" )

    if 'ReadAOD' in dir() and ReadAOD:
        TileNtuple.TileDigitsContainerFlt = ""

    if TileCompareMode:
        useRODReco = True
        useDSPUnits = True
        TileNtuple.CalibMode = True
        TileNtuple.CompareMode = True

    if useDSPUnits:
        TileNtuple.UseDspUnits = True
        TileNtuple.CalibrateEnergy = True
        TileNtuple.OfflineUnits = -1
    elif 'TileOfflineUnits' in dir():
        TileNtuple.UseDspUnits = False
        TileNtuple.CalibrateEnergy = True
        TileNtuple.OfflineUnits = TileOfflineUnits
    else:
        TileNtuple.CalibrateEnergy = TileCalibrateEnergy

    if not ReadRch and ReadPool and not doRecoESD:
        TileNtuple.TileRawChannelContainer = ""

    if useRODReco:
        if 'ReadAOD' in dir() and ReadAOD:
            TileNtuple.TileRawChannelContainerDsp = ""
            TileNtuple.TileDigitsContainer = ""
            TileNtuple.TileDigitsContainerFlt = ""
        elif ('ReadESD' in dir() and ReadESD):
            TileNtuple.TileRawChannelContainerDsp = "TileRawChannelFlt"
            TileNtuple.TileDigitsContainer = ""
            TileNtuple.TileDigitsContainerFlt = "TileDigitsFlt"
        else:
            TileNtuple.TileRawChannelContainerDsp = "TileRawChannelCnt"
    elif TilePhysTiming:
        TileNtuple.TileDigitsContainerFlt = "TileDigitsCnt"
        TileNtuple.TileDigitsContainer = "" # do not save various error bits

    if 'doTileNtupleReduced' in dir() and doTileNtupleReduced:
        TileNtuple.TileRawChannelContainerDsp = ""
        TileNtuple.TileRawChannelContainer = ""

    if ReadPool:
        TileNtuple.TileBeamElemContainer = ""
        TileNtuple.BSInput = False

    if TileLasRun:
        TileNtuple.SkipEvents = 4

    TileNtuple.PMTOrder = doTileCable

    TileNtuple.CheckDCS = TileUseDCS


if doTileMon:
    # Monitoring historgrams
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    exec 'svcMgr.THistSvc.Output += [ "Tile DATAFILE=\'%(dir)s/tilemon_%(RunNum).f_%(Version)s.root\' OPT=\'RECREATE\' " ] ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }

    if (TileMonoRun):
        runType = 9
    elif (TileL1CaloRun):
        runType = 9
    elif (TileRampRun):
        runType = 10
    else:
        runType = TileRunType

    from AthenaMonitoring.AthenaMonitoringConf import *
    TileMon = AthenaMonManager( "TileMon" )

    TileMon.ManualRunLBSetup    = True
    TileMon.ManualDataTypeSetup = True
    TileMon.Environment         = "user"
    TileMon.FileKey             = "Tile"
    TileMon.Run                 = RunNumber
    TileMon.LumiBlock           = 1

    from AthenaCommon.AppMgr import ToolSvc
    from TileMonitoring.TileMonitoringConf import *

    doTileMonDigi = ReadDigits
    doTileMonRch = (ReadDigits or ReadRch)
    doTileMonDQ = (TilePhysRun and ReadDigits)

    if not 'doTileMonCell' in dir():
        doTileMonCell = TilePhysRun

    doTileDigiNoiseMon = doTileDigiNoiseMon and ReadDigits
    doTileCellNoiseMon = doTileCellNoiseMon and doCaloCell

    if doTileMonDigi:
        b2d = TilePedRun
        theTileDigitsMon = TileDigitsMonTool ( name            ="TileDigitsMon", 
                                               histoPathBase   = "/Tile/Digits",
                                               book2D          = b2d,
                                               runType         = runType,
                                               FillPedestalDifference = True)
        ToolSvc += theTileDigitsMon
        TileMon.AthenaMonTools += [ theTileDigitsMon ]
        print theTileDigitsMon

    if doTileMonRch:
        b2d = TileCisRun or TileRampRun
        theTileRawChannelMon = TileRawChannelMonTool ( name            ="TileRawChannelMon", 
                                                       histoPathBase   = "/Tile/RawChannel",
                                                       book2D          = b2d,
                                                       PlotDSP         = useRODReco,
                                                       runType         = runType )
        ToolSvc += theTileRawChannelMon
        TileMon.AthenaMonTools += [ theTileRawChannelMon ]

        theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelCnt"; # default for simulation

        if doTileOF1:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelOF1"
            
        if doTileOptATLAS:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelFixed"

        if doTileFit:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelFit"

        if doTileFitCool:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelFitCool"

        if doTileOpt:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelOpt"

        if doTileOpt2:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelOpt2"
            
        if doTileMF:
            theTileRawChannelMon.TileRawChannelContainer = "TileRawChannelMF"

        if useRODReco:
            theTileRawChannelMon.TileRawChannelContainerDSP = "TileRawChannelCnt"

        #theTileRawChannelMon.MinAmpForCorrectedTime = 0.1

        print theTileRawChannelMon

    if doTileMonDQ:
        theTileDQFragMon = TileDQFragMonTool(name               = 'TileDQFragMon',
                                             OutputLevel        = 3,
                                             TileRawChannelContainerDSP    = "TileRawChannelCnt",
                                             TileRawChannelContainerOffl   = jobproperties.TileRecFlags.TileRawChannelContainer(),
                                             TileDigitsContainer           = "TileDigitsCnt",
                                             NegAmpHG           = -200.,
                                             NegAmpLG           = -15.,
                                             SkipMasked         = True,
                                             SkipGapCells       = True,
                                             doOnline           = False,
                                             doPlots            = False,
                                             CheckDCS           = TileUseDCS,
                                             histoPathBase      = "/Tile/DMUErrors");

        theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelCnt"; # default for simulation
            
        if doTileOptATLAS:
            theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelFixed"

        if doTileFit:
            theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelFit"

        if doTileFitCool:
            theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelFitCool"

        if doTileOpt:
            theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelOpt"

        if doTileOpt2:
            theTileDQFragMon.TileRawChannelContainerOffl = "TileRawChannelOpt2"

        if useRODReco:
            theTileDQFragMon.TileRawChannelContainerDSP = "TileRawChannelCnt"

        ToolSvc += theTileDQFragMon;
        TileMon.AthenaMonTools += [ theTileDQFragMon ];
        print theTileDQFragMon

    if doTileMonCell:
        if TileBiGainRun:
            theTileCellMonHG = TileCellMonTool(name               = 'TileCellMonHG',
                                             OutputLevel        = 3,
                                             doOnline           = False,
                                             cellsContainerName = "AllCaloHG",
                                             negEnergyThreshold = -2000,
                                             energyThreshold    = 300,
                                             histoPathBase      = "/Tile/Cell");
            ToolSvc += theTileCellMonHG;
            TileMon.AthenaMonTools += [ theTileCellMonHG ];
            print theTileCellMonHG;

        else:
            theTileCellMon = TileCellMonTool(name               = 'TileCellMon',
                                             OutputLevel        = 3,
                                             doOnline           = False,
                                             negEnergyThreshold = -2000,
                                             energyThreshold    = 300,
                                             cellsContainerName = "AllCalo",
                                             histoPathBase      = "/Tile/Cell");

            #theTileCellMon.energyThreshold = 300.
            #theTileCellMon.energyThresholdForTime = 150.
            #theTileCellMon.FillTimeHistograms = True
            ToolSvc += theTileCellMon;
            TileMon.AthenaMonTools += [ theTileCellMon ];
            print theTileCellMon;

    if doTileDigiNoiseMon:
        TileDigiNoiseMon = TileDigiNoiseMonTool(name               = 'TileDigiNoiseMon',
                                                OutputLevel        = OutputLevel,
                                                TileDigitsContainer = "TileDigitsCnt",
                                                histoPathBase = "/Tile/DigiNoise" );
        
        if not TileBiGainRun: TileDigiNoiseMon.TriggerTypes = [ 0x82 ]
        ToolSvc += TileDigiNoiseMon;
        TileMon.AthenaMonTools += [ TileDigiNoiseMon ];
        print TileDigiNoiseMon;

    if TileBiGainRun and doTileCellNoiseMon:
        TileCellNoiseMonLG = TileCellNoiseMonTool(name               = 'TileCellNoiseMonLG',
                                                  OutputLevel        = OutputLevel,
                                                  doOnline           = athenaCommonFlags.isOnline(),
                                                  cellsContainerName = "AllCaloLG",
                                                  histoPathBase = "/Tile/CellNoise/LG");
        TileCellNoiseMonLG.Xmin          = -2000.;
        TileCellNoiseMonLG.Xmax          =  2000.;
        ToolSvc += TileCellNoiseMonLG;
        TileMon.AthenaMonTools += [ TileCellNoiseMonLG ];
        print TileCellNoiseMonLG;


        TileCellNoiseMonHG = TileCellNoiseMonTool(name               = 'TileCellNoiseMonHG',
                                                  OutputLevel        = OutputLevel,
                                                  doOnline           = athenaCommonFlags.isOnline(),
                                                  cellsContainerName = "AllCaloHG",
                                                  histoPathBase = "/Tile/CellNoise/HG");
        TileCellNoiseMonHG.Xmin          = -300.;
        TileCellNoiseMonHG.Xmax          =  300.;
        ToolSvc += TileCellNoiseMonHG;
        TileMon.AthenaMonTools += [ TileCellNoiseMonHG ];
        print TileCellNoiseMonHG;

    if (not TileBiGainRun) and doTileCellNoiseMon:
        TileCellNoiseMon = TileCellNoiseMonTool(name               = 'TileCellNoiseMon',
                                                OutputLevel        = OutputLevel,
                                                doOnline           = athenaCommonFlags.isOnline(),
                                                cellsContainerName = "AllCalo",
                                                histoPathBase = "/Tile/CellNoise");
        TileCellNoiseMon.Xmin          = -2000.;
        TileCellNoiseMon.Xmax          =  2000.;
        ToolSvc += TileCellNoiseMon;
        TileMon.AthenaMonTools += [ TileCellNoiseMon ];
        print TileCellNoiseMon;


    if doTileRawChannelTimeMonTool:
        TileRawChannelTimeMon = TileRawChannelTimeMonTool ( name            = "TileRawChannelTimeMon",
                                                           histoPathBase    = "/Tile/RawChannelTime",
                                                           runType          = TileRunType,
                                                           LowGainThreshold = 10.0,
                                                           HiGainThreshold  = 40.0,
                                                           doOnline         = athenaCommonFlags.isOnline(),
                                                           TileRawChannelContainer = "TileRawChannelFit")

        ToolSvc += TileRawChannelTimeMon
        TileMon.AthenaMonTools += [ TileRawChannelTimeMon ];
        print TileRawChannelTimeMon




    ############ doTileRawChannelNoiseMonTool #########
    if TileBiGainRun and doTileRawChannelNoiseMon:
        TileRawChannelNoiseMonLG = TileRawChannelNoiseMonTool(name          = 'TileRawChannelNoiseMonLG',
                                                              OutputLevel   = OutputLevel,
                                                              doOnline      = athenaCommonFlags.isOnline(),
                                                              histoPathBase = "/Tile/RawChannelNoise/LG",
                                                              Xmin          = -10.,
                                                              Xmax          =  10.,
                                                              Gain          = "LG",
                                                              do2GFit       = True,
                                                              # doFit         = True,
                                                              SummaryUpdateFrequency = 0 );

        ToolSvc += TileRawChannelNoiseMonLG;
        TileMon.AthenaMonTools += [ TileRawChannelNoiseMonLG ];
        print TileRawChannelNoiseMonLG;


        TileRawChannelNoiseMonHG = TileRawChannelNoiseMonTool(name          = 'TileRawChannelNoiseMonHG',
                                                              OutputLevel   = OutputLevel,
                                                              doOnline      = athenaCommonFlags.isOnline(),
                                                              histoPathBase = "/Tile/RawChannelNoise/HG",
                                                              Xmin          = -10.,
                                                              Xmax          =  10.,
                                                              Gain          = "HG",
                                                              do2GFit       = True,
                                                              # doFit         = True,
                                                              SummaryUpdateFrequency = 0 );


        ToolSvc += TileRawChannelNoiseMonHG;
        TileMon.AthenaMonTools += [ TileRawChannelNoiseMonHG ];
        print TileRawChannelNoiseMonHG;

    if (not TileBiGainRun) and doTileRawChannelNoiseMon:
        TileRawChannelNoiseMon = TileRawChannelNoiseMonTool(name               = 'TileRawChannelNoiseMon',
                                                            OutputLevel        = OutputLevel,
                                                            doOnline           = athenaCommonFlags.isOnline(),
                                                            histoPathBase = "/Tile/RawChannelNoise",
                                                            Xmin          = -10.0,
                                                            Xmax          =  10.0,
                                                            Gain          = "HG",
                                                            do2GFit       = True,
                                                            # doFit         = True,
                                                            SummaryUpdateFrequency = 0);

        # fill raw channel noise mon histograms only for certain trigger types
        # if not defined here, then by default all triggers will be considered
        TileRawChannelNoiseMon.TriggerTypes           = [ 0x82 ];

        ToolSvc += TileRawChannelNoiseMon;
        TileMon.AthenaMonTools += [ TileRawChannelNoiseMon ];
        print TileRawChannelNoiseMon;


    ########### end doTileCellNoiseMon ##########

    if doTileTMDBDigitsMon:
        ToolSvc += CfgMgr.TileTMDBDigitsMonTool(name                  = 'TileTMDBDigitsMon'
                                                , OutputLevel         = INFO
                                                , TileDigitsContainer = "MuRcvDigitsCnt"
                                                , histoPathBase       = "/Tile/TMDBDigits")
        
        TileMon.AthenaMonTools += [ ToolSvc.TileTMDBDigitsMon ]
        print ToolSvc.TileTMDBDigitsMon


    if doTileTMDBRawChannelMon:
        ToolSvc += CfgMgr.TileTMDBRawChannelMonTool(name            = 'TileTMDBRawChannelDspMon'
                                                    , OutputLevel   = INFO
                                                    , NotDSP           = False
                                                    , TileRawChannelContainer = "MuRcvRawChCnt"
                                                    , histoPathBase = "/Tile/TMDBRawChannel/Dsp")


        TileMon.AthenaMonTools += [ToolSvc.TileTMDBRawChannelDspMon ]
        print ToolSvc.TileTMDBRawChannelDspMon
        
        ToolSvc += CfgMgr.TileTMDBRawChannelMonTool(name                      = 'TileTMDBRawChannelMon'
                                                    , OutputLevel             = INFO
                                                    , TileRawChannelContainer = "TileMuRcvRawChannelOpt2"
                                                    , NotDSP                   = True
                                                    , AmplitudeThresholdForTime = 10.0
                                                    , histoPathBase           = "/Tile/TMDBRawChannel")
        
        
        TileMon.AthenaMonTools += [ToolSvc.TileTMDBRawChannelMon ]
        print ToolSvc.TileTMDBRawChannelMon


    if doTileMonDigi or doTileMonRch or doTileMonCell or doTileMonDQ                \
            or doTileDigiNoiseMon or doTileCellNoiseMon or doTileRawChannelNoiseMon \
            or doTileTMDBDigitsMon or doTileTMDBRawChannelMon:

        topSequence += TileMon;


if doTileCalib:
    # new options to be written
    if TilePhysRun or TilePedRun:
        #Add Noise Calib Tool
        from TileCalibAlgs.TileCalibAlgsConf import TileDigiNoiseCalibAlg
        from TileCalibAlgs.TileCalibAlgsConf import TileRawChNoiseCalibAlg

        if Version == "0" or Version == "Ped.0" or Version == "Ped" : # prudce digi noise ntuple only for default version
            theTileDigiNoiseCalibAlg = TileDigiNoiseCalibAlg( "theTileDigiNoiseCalibAlg" )
            theTileDigiNoiseCalibAlg.DoAvgCorr = False # False=> Full AutoCorr matrix calculation
            if TileNoiseFilter > 0:
                exec 'theTileDigiNoiseCalibAlg.FileNamePrefix = \'Digi_NoiseCalib_%(Version)s\' '  %  {'Version': TileNoiseFilter }
            if Version != "0" and Version != "Ped.0" and Version != "Ped" :
                VF = Version+"_tnf"+str(TileNoiseFilter)
                exec 'theTileDigiNoiseCalibAlg.FileNamePrefix = \'Digi_NoiseCalib_%(Version)s\' '  %  {'Version': VF }
            topSequence += theTileDigiNoiseCalibAlg
        theTileRawChNoiseCalibAlg = TileRawChNoiseCalibAlg("theTileRawChNoiseCalibAlg")
        theTileRawChNoiseCalibAlg.UseforCells = 1  # 1= Fixed , 2= Opt2
        if TileNoiseFilter > 0:
            exec 'theTileRawChNoiseCalibAlg.FileNamePrefix = \'RawCh_NoiseCalib_%(Version)s\' ' % {'Version': TileNoiseFilter }
        if Version != "0" and Version != "Ped.0" and Version != "Ped" :
            VF = Version + "_tnf" + str(TileNoiseFilter)
            exec 'theTileRawChNoiseCalibAlg.FileNamePrefix = \'RawCh_NoiseCalib_%(Version)s\' '  %  {'Version': VF }
        topSequence += theTileRawChNoiseCalibAlg

        if TileEmulateDSP:
            theTileRawChNoiseCalibAlg.UseforCells=1 # i.e. from TileRawChannelFixed, which is like DSP results

        if doSim:
            theTileRawChNoiseCalibAlg.doFit   = False
            theTileRawChNoiseCalibAlg.doFixed = False
            theTileRawChNoiseCalibAlg.doOpt   = False
            theTileRawChNoiseCalibAlg.doDsp   = True
            theTileRawChNoiseCalibAlg.UseforCells=3 # i.e. from TileRawChannelCnt (like DSP)

    elif TileCisRun:
        # CIS calibration using top calib alg
        from TileCalibAlgs.TileCalibAlgsConf import TileTopCalibAlg
        from TileCalibAlgs.TileCalibAlgsConf import TileCisDefaultCalibTool

        TileCalibAlg = TileTopCalibAlg()
        TileCalibAlg.RunNumber        = RunNumber
        TileCalibAlg.RunType          = 8
        exec 'TileCalibAlg.FileName = \'%(dir)s/tileCalibCIS_%(RunNum).f_%(Version)s.root\' ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }

        # declare CIS tool(s) and set jobOptions if necessary
        TileCisTool = TileCisDefaultCalibTool()

        if hasattr(ToolSvc, 'TileDigitsMon'):
            TileCisTool.StuckBitsProbsTool = ToolSvc.TileDigitsMon

        TileCisTool.removePed = True
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TileCisTool
        TileCalibAlg.TileCalibTools += [ TileCisTool ]

        topSequence += TileCalibAlg
    elif TileL1CaloRun:
        if RUN2: include("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")
        else: include( "TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py" )

        # Trigger calibration using top calib alg
        from TileCalibAlgs.TileCalibAlgsConf import TileTopCalibAlg
        from TileCalibAlgs.TileCalibAlgsConf import TileTriggerDefaultCalibTool

        TileCalibAlg = TileTopCalibAlg()
        TileCalibAlg.RunNumber        = RunNumber
        TileCalibAlg.RunType          = 8
        exec 'TileCalibAlg.FileName = \'%(dir)s/tileCalibL1Calo_%(RunNum).f_%(Version)s.root\' ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }

        # declare Trigger tool(s) and set jobOptions if necessary
        TileTriggerTool = TileTriggerDefaultCalibTool()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TileTriggerTool
        TileCalibAlg.TileCalibTools += [ TileTriggerTool ]

        topSequence += TileCalibAlg              
    elif TileLasRun:
        # Laser calibration
        from TileCalibAlgs.TileCalibAlgsConf import TileLaserCalibAlg
        from TileCalibAlgs.TileCalibAlgsConf import TileLaserDefaultCalibTool

        TileCalibAlg = TileLaserCalibAlg()
        exec 'TileCalibAlg.FileName = \'%(dir)s/tileCalibLAS_%(RunNum).f_%(Version)s.root\' ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }

        # declare LASER tool(s) and set jobOptions if necessary
        TileLaserTool = TileLaserDefaultCalibTool()
        TileLaserTool.rawChannelContainer    = "TileRawChannelOpt2"
        TileLaserTool.laserObjContainer      = "TileLaserObj"

        if hasattr(ToolSvc, 'TileDigitsMon'):
            TileLaserTool.StuckBitsProbsTool = ToolSvc.TileDigitsMon

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TileLaserTool
        TileCalibAlg.Tools = [ TileLaserTool ]

        topSequence += TileCalibAlg
    else: 
        log.warning( "TileCalib options are not ready yet for this runtype" )

# Provides handle to give a time stamp for calib runs without time in the event header.
if 'ForceTimeStamp' in dir():
    svcMgr.IOVDbSvc.forceTimestamp = ForceTimeStamp


if doCreatePool:
    # Pool Output
    include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    exec 'FileName = \'%(dir)s/tile_%(RunNum).f_%(Version)s.pool.root\' ' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }
    Stream1 = AthenaPoolOutputStream( "Stream1", FileName )

    # list of output objects
    Stream1.ItemList += [ "TileHitVector#*" ]
    Stream1.ItemList += [ "TileDigitsContainer#*" ]
    Stream1.ItemList += [ "TileBeamElemContainer#*" ]
    Stream1.ItemList += [ "TileRawChannelContainer#*" ]
    Stream1.ItemList += [ "TileMuContainer#*" ]
    Stream1.ItemList += [ "TileL2Container#*" ]
    Stream1.ItemList += [ "TileCosmicMuonContainer#*" ]
    Stream1.ItemList += [ "TileCellContainer#*" ]
    Stream1.ItemList += [ "CaloCellContainer#*" ]


if doEventDisplay:
    from VP1Algs.VP1AlgsConf import VP1Alg
    topSequence += VP1Alg()
    topSequence.TimeOut = 0


if doAtlantis:
    include("JiveXML/JiveXML_jobOptionBase.py")
    from AthenaCommon.AppMgr import ToolSvc

    if 'doAtlantisStreamToServer' in dir() and doAtlantisStreamToServer:
        from JiveXML.JiveXMLConf import JiveXML__ONCRPCServerSvc
        svcMgr += JiveXML__ONCRPCServerSvc("ONCRPCServerSvc", OutputLevel = DEBUG)

        from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
        StreamToServerTool = JiveXML__StreamToServerTool(OutputLevel = DEBUG
                                                         , ServerService = svcMgr.ONCRPCServerSvc
                                                         , StreamName = "Test")

        ToolSvc += StreamToServerTool
        theEventData2XML.StreamTools += [ StreamToServerTool ]

    else:
        from JiveXML.JiveXMLConf import JiveXML__StreamToFileTool
        theStreamToFileTool=JiveXML__StreamToFileTool("theStreamToFileTool")
        ToolSvc += theStreamToFileTool
        theEventData2XML.StreamTools = [ theStreamToFileTool ]
        theEventData2XML.WriteToFile = False



    if doCaloCell or doCaloNtuple:
        from CaloJiveXML.CaloJiveXMLConf import JiveXML__CaloTileRetriever
        theCaloTileRetriever = JiveXML__CaloTileRetriever (name = "CaloTileRetriever")
        theCaloTileRetriever.DoTileCellDetails = True
        theCaloTileRetriever.DoTileDigit = ReadDigits or ('ReadESD' in dir() and ReadESD)
        theCaloTileRetriever.CellThreshold = 50.0
        ToolSvc += theCaloTileRetriever
        theEventData2XML.DataTypes += ["JiveXML::CaloTileRetriever/CaloTileRetriever"]

        from CaloJiveXML.CaloJiveXMLConf import JiveXML__CaloMBTSRetriever
        theCaloMBTSRetriever = JiveXML__CaloMBTSRetriever (name = "CaloMBTSRetriever")
        theCaloMBTSRetriever.DoMBTSDigits = ReadDigits or ('ReadESD' in dir() and ReadESD)
        theCaloMBTSRetriever.MBTSThreshold = 0.05
        ToolSvc += theCaloMBTSRetriever
        theEventData2XML.DataTypes += ["JiveXML::CaloMBTSRetriever/CaloMBTSRetriever"]

    if doCaloTopoCluster:
        from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODCaloClusterRetriever
        thexAODCaloClusterRetriever = JiveXML__xAODCaloClusterRetriever (name = "xAODCaloClusterRetriever")
        thexAODCaloClusterRetriever.FavouriteClusterCollection = "TileTopoCluster"
        thexAODCaloClusterRetriever.OtherClusterCollections = [ "" ]
        ToolSvc += thexAODCaloClusterRetriever
        theEventData2XML.DataTypes += ["JiveXML::xAODCaloClusterRetriever/xAODCaloClusterRetriever"]

    print theEventData2XML

#-----------------------
# And some final options
#-----------------------

theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

if not ReadPool:
    svcMgr.EventSelector.MaxBadEvents = 10000
    svcMgr.EventSelector.ProcessBadEvent = True
    if OutputLevel < 2:
        #svcMgr.ByteStreamInputSvc.DumpFlag = True
        ToolSvc.TileROD_Decoder.VerboseOutput = True

print topSequence

svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.EventSelector.SkipEvents = EvtMin
theApp.EvtMax=EvtMax

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if not 'db' in dir():
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)


if hasattr (svcMgr.ToolSvc, 'CaloNoiseToolDefault'):
    ToolSvc.CaloNoiseToolDefault.RescaleForHV = False

# Needed during maintanance campaign
# if doTileMon and doTileMonDigi: ToolSvc.TileDigitsMon.FillPedestalDifference = False

