if not 'myreadESD' in dir(): myreadESD = False
if not 'isMC' in dir(): isMC = False

from RecExConfig.RecFlags import rec

## Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool() 
ToolSvc.GoodRunsListSelectorTool.GoodRunsListVec     = 	[ 	
							  #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/lartile.xml',
							  #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/lartile_collisions.xml',
							  #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/lartilejet.xml',
							  #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/lartilemet.xml',
                                                          #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/lartilejetmet.xml',
							  #'/afs/cern.ch/user/m/mbaak/muwork/workarea/15.6.1/WorkArea/run/xml/noreq_lartile.xml',
							]

## configure the collision time filter
if rec.readESD() or myreadESD:
  #
  include ("LArCellRec/LArCollisionTime_jobOptions.py")
  #
  from PrimaryDPDMaker.PrimaryDPDMakerConf import MBTSTimeFilterTool
  ToolSvc += MBTSTimeFilterTool()
  MBTSTimeFilterTool.ChargeThreshold   = 60.0/222.0
  MBTSTimeFilterTool.MinHitsPerSide    = 2
  MBTSTimeFilterTool.MaxTimeDifference = 10.0
  #
  ### Get the needed CaloTimeFilterTool
  #from PrimaryDPDMaker.PrimaryDPDMakerConf import CaloTimeFilterTool
  #ToolSvc += CaloTimeFilterTool() 
  #CaloTimeFilterTool.timeDiffCut               = 5
  #CaloTimeFilterTool.MinCellsPerSide           = 2
  #if rec.doTruth() or isMC:
  #  CaloTimeFilterTool.isMC = True

## next: add the list of the algorithms to be executed at run time:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from MissingETGoodness.MissingETGoodnessConf import METCollisionFilter

## DQ filter
from AthenaCommon.GlobalFlags import globalflags
if False: #globalflags.DataSource()=='data':
  topSequence += METCollisionFilter('DQLarTile')
  topSequence.DQLarTile.Prefix = 'DQLarTile_'
  #topSequence.DQLarTile.GoodRunsListArray = ['lartile']
  ##
  topSequence += METCollisionFilter('DQLarTileJet')
  topSequence.DQLarTileJet.Prefix = 'DQLarTileJet_'
  #topSequence.DQLarTileJet.GoodRunsListArray = ['lartilejet']
  ##
  topSequence += METCollisionFilter('DQLarTileMet')
  topSequence.DQLarTileMet.Prefix = 'DQLarTileMet_'
  #topSequence.DQLarTileMet.GoodRunsListArray = ['lartilemet']
  ##
  topSequence += METCollisionFilter('DQLarTileJetMet')
  topSequence.DQLarTileJetMet.Prefix = 'DQLarTileJetMet_'
  #topSequence.DQLarTileJetMet.GoodRunsListArray = ['lartilejetmet']
  ##
  topSequence += METCollisionFilter('DQNoLarTile')
  topSequence.DQNoLarTile.Prefix = 'DQNoLarTile_'
  #topSequence.DQNoLarTile.GoodRunsListArray = ['noreq_lartile']
  ##
  topSequence += METCollisionFilter('DQLarTileColls')
  topSequence.DQLarTileColls.Prefix = 'DQLarTileColls_'
  #topSequence.DQLarTileColls.GoodRunsListArray = ['lartile_collisions']

## collision time filtering
topSequence += METCollisionFilter()
topSequence.METCollisionFilter.DoCaloTimeFilter = (rec.readESD() or myreadESD)
topSequence.METCollisionFilter.DoMBTSTimeFilter = (rec.readESD() or myreadESD)

