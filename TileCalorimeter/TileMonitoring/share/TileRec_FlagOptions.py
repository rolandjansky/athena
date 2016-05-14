if not 'InputDirectory' in dir():
    InputDirectory = "."    
     
if not 'RunNumber' in dir():
    RunNumber = 215027

if not 'FileNameVec' in dir():
    if not 'FileName' in dir():
        FileNameVec = []
        for i in xrange(450, 452, 1):
            FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data12_8TeV/express_express/%(R)08d/data12_8TeV.%(R)08d.express_express.merge.RAW/data12_8TeV.%(R)08d.express_express.merge.RAW._lb%(LB)04d._SFO-ALL._0001.1" % {'LB':i, 'R':RunNumber})
    else:
        if FileName == 'ALL' :
            files = []
            fullname = []
            if InputDirectory.startswith("/castor") :
                for f in popen('nsls %(path)s | grep %(run)s' % {'path': InputDirectory, 'run':RunNumber }):
                    files.append(f)
            elif InputDirectory.startswith("/eos") :
                for f in popen('eos ls %(path)s | grep %(run)s' % {'path': InputDirectory, 'run':RunNumber }):
                    files.append(f)
            else:
                for f in popen('ls %(path)s | grep %(run)s' % {'path': InputDirectory, 'run':RunNumber }):
                    files.append(f)
            for nn in range(len(files)):
               temp = files[nn].split('\n')
               fullname.append(InputDirectory + '/' + temp[0])
            FileNameVec = fullname
        else:
            FileNameVec = [ FileName ]


print "Input files:"
print FileNameVec

if not 'doOnline' in dir():
    doOnline = False 

if not 'doStateless' in dir():
    doStateless = False

if doStateless:
    doOnline = True

import os.path

if not 'OutputDirectory' in dir():
    # OutputDirectory = "/tmp/Reco-"+str(RunNumber)+"/"
    if doOnline and os.path.exists("/det/tile/efmon/tilemon"):
        OutputDirectory = "/det/tile/efmon/tilemon"
    else:
        OutputDirectory = "."

if OutputDirectory != ".":
    from os import system
    system('mkdir -p %s' % (OutputDirectory))

if "histo_name" not in dir():
    histo_name = "tilemon.root"

if not 'RootHistOutputFileName' in dir():
    RootHistOutputFileName = OutputDirectory + "/" + histo_name
elif RootHistOutputFileName.find("/") < 0: 
    RootHistOutputFileName = OutputDirectory + "/" + RootHistOutputFileName


if not 'storeHisto' in dir():
    storeHisto = False

#--------------------------------
# TileCal testbeam reconstruction
#--------------------------------
# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder
# if all False the DSPReco will be used

if not 'doTileFlat' in dir():
    doTileFlat = False

if not 'doTileOpt' in dir():
    doTileOpt = False

if not 'doTileOpt2' in dir():
    doTileOpt2 = True

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = True

if not 'doTileFit' in dir():
    doTileFit = False

# if your are processing CIS run, make sure that you uncommented this
# TileCisRun = True
# skip first event in CIS run which is always junk
# EventSelector = Service( "EventSelector" )
# EventSelector.SkipEvents = 1

if not 'beamType' in dir():
    beamType = 'singlebeam'

# if run type is set to non-zero value, it overrides event trig type
if not 'TileRunType' in dir():
    TileRunType = 1;  # 1 - physics, 2 - laser, 4 - pedestal, 8 - CIS run 

if not 'TileUseCOOL' in dir():
    TileUseCOOL = True
   
if TileUseCOOL and not 'tileCOOLtag' in dir():
    if doOnline:
        tileCOOLtag = "CONDBR2-HLTP-2016-01"
    else:
        if RunNumber > 232498:
            tileCOOLtag = "CONDBR2-BLKPA-2016-11"
        else:
            tileCOOLtag = "COMCOND-BLKPA-RUN1-06"

if not 'doTileCells' in dir():
   # CaloCell reconstruction for Tile
    doTileCells = True

if not 'doTowers' in dir():
   # Towers reconstruction for Tile
    doTowers = True

if not 'doClusters' in dir():
   # Clusters reconstruction for Tile
    doClusters = True

if not 'doTileL2Mu' in dir():
   # TileL2 muons
    doTileL2Mu = True

if not 'doTileMuId' in dir():
   # TileMuId
    doTileMuId = True

if not 'doTileMuonFit' in dir():
   # TileMuonFitter
    doTileMuonFit = True

if not 'doMBTS' in dir():
    # MBTS monitoring
    doMBTS = True


if not 'doTileTMDBRawChannel' in dir():
    doTileTMDBRawChannel = True

if not 'doMonitoring' in dir():
   # Monitoring histograms
    doMonitoring = True
if not 'MonitorOutput' in dir():
    MonitorOutput = "Tile"
#   MonitorOutput="SHIFT"
#   MonitorOutput="EXPERT"

if not 'doAtlantis' in dir():
   # Atlantis
    doAtlantis = False
if not 'OnlineAtlantis' in dir():
    OnlineAtlantis = False

# trigger part
if not'doTrigger' in dir():
    doTrigger = True
   
# if doOnline and storeHisto:
#   doAtlantis= False

if not 'Version' in dir():
    Version = "0"

# Other jobOptions
if not 'MaxBadEvents' in dir():
    MaxBadEvents = 10000

if not 'MsgLinesLimit' in dir():
    MsgLinesLimit = 1000000

if not 'useColors' in dir():
    useColors = False

if not 'OutputLevel' in dir():
    OutputLevel = 3

if not 'SG_Dump' in dir():
    SG_Dump = False

if not 'doPerfMon' in dir():
    doPerfMon = True

if not 'EvtMax' in dir():
    EvtMax = -1
#   EvtMax = 100

if not 'EvtMin' in dir():
    EvtMin = 0

if not 'CheckDCS' in dir():
    CheckDCS = True

if not 'doTileNoiseMon' in dir():
    doTileNoiseMon = False    

if not 'doTileTMDBDigitsMon' in dir():
    doTileTMDBDigitsMon = True
    
if not 'doTileTMDBRawChannelMon' in dir():
    doTileTMDBRawChannelMon = True

if doTileNoiseMon:
    doTileCells = False
    doTrigger = False

if doAtlantis:
    doMonitoring = False
    doTrigger = False
    doTileMuId = False
    doTileMuonFit = False

if doOnline and not 'TileCablingType' in dir():
    TileCablingType = 4

if not 'TileNoiseFilter' in dir():
    TileNoiseFilter = 1
