if ( "streamAOD" in dir() ):
  print "Enabling JGTowerMaker"


  #svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"
  svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=gJTowerMap.db;dbname=OFLP200"
  svcMgr += CfgMgr.THistSvc()
  svcMgr.THistSvc.Output += ["OUTPUT DATAFILE='myOutputFile.root' OPT='RECREATE'"]




  folderlist=("/LAR/Identifier/GTowerSCMap",
              "/LAR/Identifier/JTowerSCMap"
              )

  folderspec=[]
  tagspec=[]
  for f in folderlist:
      folderspec.append("GTowerSCMap#GTowerSCMapAtlas#"+f)
      tagspec.append("".join(f.split("/"))+"-boohoo")
      pass

  from TrigT1CaloFexSim  import *
  theJGTowerMaker = JGTowerMaker()
  digitizationFlags.rndmSeedList.addSeed("JGTowerMaker",1234 , 5678 )

  theJGTowerMaker=JGTowerMaker()
  theJGTowerMaker.RndmSvc = digitizationFlags.rndmSvc.get_Value()


  StreamAOD.ItemList+=["CaloCellContainer#AllCalo"]
  StreamAOD.ItemList+=["CaloCellContainer#SCell"]
  StreamAOD.ItemList+=["xAOD::TriggerTowerContainer#xAODTriggerTowers","xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux."];
  StreamAOD.ItemList+=["JGTowerContainer#JTower"] # This might need to be controlled later to avoid increase of AOD size
  StreamAOD.ItemList+=["JGTowerAuxContainer#JTowerAux.*"]         # This might need to be controlled later to avoid increase of AOD size
  StreamAOD.ItemList+=["JGTowerContainer#GTower"] # This might need to be controlled later to avoid increase of AOD size
  StreamAOD.ItemList+=["JGTowerAuxContainer#GTowerAux.*"]         # This might need to be controlled later to avoid increase of AOD size

#  StreamAOD.TakeItemsFromInput = True
  #topSequence.remove(streamAOD)
  topSequence += StreamAOD


  algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
  algseq += CfgMgr.JGTowerMaker(
    useSCQuality = True,
    useAllCalo = False
)
