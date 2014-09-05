if not 'InputDirectory' in dir():
   # 2006 runs
   #InputDirectory = ["/castor/cern.ch/atlas/testbeam/tilecal/2006/daq"]
   #InputDirectory = ["/castor/cern.ch/atlas/P1commisioning/phaseIII"]
   #For M4 runs
   #InputDirectory="/castor/cern.ch/grid/atlas/t0/perm/DAQ"
   #For M5 runs
   #InputDirectory="/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5"
   #For M7
   #InputDirectory="/castor/cern.ch/grid/atlas/DAQ/2008"    
   #For tests
   InputDirectory="."    
     
if not 'RunNumber' in dir():
   #RunNumber = 14834
   #2006 reference run
   #RunNumber = 61542
   # M4 Good run for Calo
   #RunNumber = 20919
   # pre-M5 Good run for Calo
   #RunNumber = 25341
   # M5 Good run for Calo
   #RunNumber = 29118
   # Pre M6 Good run for Calo
   #RunNumber = 39710
   #RunNumber = 41843
   #RunNumber = 43050
   #RunNumber = 74303
   # 2011 data
   #RunNumber = 186934
   #RunNumber = 190236
   # 2012 data
   #RunNumber = 203195
   RunNumber = 215027

if not 'FileNameVec' in dir():
   if not 'FileName' in dir():
      FileNameVec = []
      for i in xrange(450, 452, 1):
#         FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data12_8TeV/physics_Background/%(R)08d/data12_8TeV.%(R)08d.physics_Background.merge.RAW/data12_8TeV.%(R)08d.physics_Background.merge.RAW._lb%(LB)04d._SFO-ALL._0001.1" % {'LB':i, 'R':RunNumber})
         FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data12_8TeV/express_express/%(R)08d/data12_8TeV.%(R)08d.express_express.merge.RAW/data12_8TeV.%(R)08d.express_express.merge.RAW._lb%(LB)04d._SFO-ALL._0001.1" % {'LB':i, 'R':RunNumber})
#         FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data12_8TeV/physics_JetTauEtmiss/%(R)08d/data12_8TeV.%(R)08d.physics_JetTauEtmiss.merge.RAW/data12_8TeV.%(R)08d.physics_JetTauEtmiss.merge.RAW._lb%(LB)04d._SFO-1._0001.1" % {'LB':i, 'R':RunNumber})
#         FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/physics_JetTauEtmiss/%(R)08d/data11_7TeV.%(R)08d.physics_JetTauEtmiss.merge.RAW/data11_7TeV.%(R)08d.physics_JetTauEtmiss.merge.RAW._lb%(LB)04d._SFO-9._0001.1" % {'LB':i, 'R':RunNumber})
   else:
      if FileName == 'ALL' :
         files=[]
         fullname=[]
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
            temp=files[nn].split('\n')
            fullname.append(InputDirectory+'/'+temp[0])
         FileNameVec = fullname
      else:
         FileNameVec = [ FileName ]

#FileNameVec = ["/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_1beam/express_express//0154193/data10_1beam.00154193.express_express.merge.RAW/data10_1beam.00154193.express_express.merge.RAW._lb0120._0001.1"]
#FileNameVec = []
#for i in xrange(217, 223, 2):
  #FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/express_express/0167963/data10_7TeV.00167963.express_express.merge.RAW/data10_7TeV.00167963.express_express.merge.RAW._lb%04d._SFO-ALL._0001.1" % i)
#for i in xrange(755, 777, 3):
#for i in xrange(1201, 1203, 1):
#  FileNameVec.append("/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_7TeV/express_express/%(R)08d/data11_7TeV.%(R)08d.express_express.merge.RAW/data11_7TeV.%(R)08d.express_express.merge.RAW._lb%(LB)04d._SFO-ALL._0001.1" % {'LB':i, 'R':RunNumber})

print "Input files:"
print FileNameVec

if not 'OutputDirectory' in dir():
   #OutputDirectory = "/tmp/Reco-"+str(RunNumber)+"/"
   OutputDirectory = "."

if OutputDirectory!=".":
   from os import system
   system('mkdir -p %s' %(OutputDirectory))

if "histo_name" not in dir():
    histo_name="tilemon.root"

if not 'RootHistOutputFileName' in dir():
   RootHistOutputFileName=OutputDirectory+"/"+histo_name
elif RootHistOutputFileName.find("/")<0: 
   RootHistOutputFileName=OutputDirectory+"/"+RootHistOutputFileName


if not 'doOnline' in dir():
   doOnline = False 

if not 'doStateless' in dir():
   doStateless = False

if not 'storeHisto' in dir():
   storeHisto = False

#--------------------------------
# TileCal testbeam reconstruction
#--------------------------------
# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder
#if all False the DSPReco will be used

if not 'doTileFlat' in dir():
   doTileFlat= False

if not 'doTileOpt' in dir():
   doTileOpt = False

if not 'doTileOpt2' in dir():
   doTileOpt2 = False

if not 'doTileOptATLAS' in dir():
   doTileOptATLAS = True

if not 'doTileFit' in dir():
   doTileFit = False

#if your are processing CIS run, make sure that you uncommented this
#TileCisRun = True
# skip first event in CIS run which is always junk
#EventSelector = Service( "EventSelector" )
#EventSelector.SkipEvents = 1

if not 'beamType' in dir():
   beamType='singlebeam'

# if run type is set to non-zero value, it overrides event trig type
if not 'TileRunType' in dir():
   TileRunType = 1; # 1 - physics, 2 - laser, 4 - pedestal, 8 - CIS run 

if not 'TileUseCOOL' in dir():
   TileUseCOOL = True
   
if TileUseCOOL:
   if RunNumber>232498:
      tileCOOLtag = "CONDBR2-BLKPA-2014-00"
   else:
      tileCOOLtag = "COMCOND-BLKPA-RUN1-06"

if not 'doTileCells' in dir():
   #CaloCell reconstruction for Tile
   doTileCells = True

if not 'doTowers' in dir():
   #Towers reconstruction for Tile
   doTowers = True

if not 'doClusters' in dir():
   #Clusters reconstruction for Tile
   doClusters = True

if not 'doTileL2Mu' in dir():
   #TileL2 muons
   doTileL2Mu = True

if not 'doTileMuId' in dir():
   #TileMuId
   doTileMuId = True

if not 'doTileMuonFit' in dir():
   #TileMuonFitter
   doTileMuonFit = True

if not 'doMBTS' in dir():
   #MBTS monitoring
   doMBTS = True

if not 'doMonitoring' in dir():
   #Monitoring histograms
   doMonitoring = True
if not 'MonitorOutput' in dir():
   MonitorOutput="Managed"
#   MonitorOutput="SHIFT"
#   MonitorOutput="EXPERT"

if not 'doAtlantis' in dir():
   #Atlantis
   doAtlantis=False
if not 'OnlineAtlantis' in dir():
   OnlineAtlantis=False

#trigger part
if not'doTrigger' in dir():
   doTrigger=True
   
#if doOnline and storeHisto:
#   doAtlantis= False

if not 'Version' in dir():
   Version="0"

#Other jobOptions
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
