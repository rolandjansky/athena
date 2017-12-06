from CaloRec.CaloRecFlags import jobproperties

from AthenaCommon.Resilience import treatException        

from RecExConfig.RecFlags  import rec

from AthenaCommon.GlobalFlags  import globalflags

from AthenaCommon.DetFlags import DetFlags

if globalflags.DataSource()=='data':
    if rec.projectName()=="data09_calophys":
    # for data09_calophys project, force to use DSP output for the cell energy, perform reco like DSP, no dead cell correction
        if rec.doLArg():
            from LArROD.LArRODFlags import larRODFlags
            larRODFlags.readDigits=False
        if rec.doTile():
            from TileRecUtils.TileRecFlags import jobproperties
            jobproperties.TileRecFlags.readDigits=False
        from CaloRec.CaloCellFlags import jobproperties
        jobproperties.CaloCellFlags.doDeadCellCorr=False
        jobproperties.CaloCellFlags.doLArCreateMissingCells=False
        jobproperties.CaloCellFlags.doLArDeadOTXCorr=False 
    else:
        if rec.doLArg() and globalflags.InputFormat() == 'bytestream' and jobproperties.CaloRecFlags.doLArAutoConfiguration():
            # for bytestream reco of real data, autoconfigure based on the run format information
            # use digits only when digits available for all cells (transparent or rawdataresult)
            # use DSP energy + digits when available in case only sparse digits available (results)
            #   (when collisions with timing settled we could use only DSP energy in this case)
            from LArROD.LArRODFlags import larRODFlags
            larRODFlags.readDigits=True
            from LArConditionsCommon.LArCool import larcool
            if (larcool is not None):
                # check format1 & results mode
                if (larcool.format()==1 and larcool.runType()==2) :
                    larRODFlags.keepDSPRaw = True
                if (larcool.format()==0):
                    larRODFlags.keepDSPRaw = False      # raw data transparent mode, no DSP energy

   


## RS Jun 2009 remove CombinedTower, Clusters from input when readESD=doESD=True
#              to allow rebuilding of them

if rec.readESD and rec.doESD:
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore['inputFile'].removeItem(["CaloTowerContainer#CombinedTower"])
    objKeyStore['inputFile'].removeItem(["CaloTopoTowerContainer#TopoTower"])
    
    objKeyStore['inputFile'].removeItem(["CaloClusterContainer#EMTopoCluster430"])
    objKeyStore['inputFile'].removeItem(["CaloShowerContainer#EMTopoCluster430_Data"]) 
    objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#EMTopoCluster430_Link"]) 
    
    objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEM"])
    objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEM7_11Nocorr"])
    objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEMFrwd"])
    objKeyStore['inputFile'].removeItem(["CaloClusterContainer#LArClusterEMSofte"]) 
    
    objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM7_11Nocorr_Data"]) 
    objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEMSofte_Data"]) 
    objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM_Data"]) 
    objKeyStore['inputFile'].removeItem(["CaloShowerContainer#LArClusterEM7_11Nocorr_Data"]) 
    
    objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link"]) 
    objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM7_11Nocorr_Link"]) 
    objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEMSofte_Link"]) 
    objKeyStore['inputFile'].removeItem(["CaloCellLinkContainer#LArClusterEM_Link"]) 

    if jobproperties.CaloRecFlags.doTileMuId() :
        objKeyStore['inputFile'].removeItem(["TileMuContainer:TileMuObj"])
        objKeyStore['inputFile'].removeItem(["TileCosmicMuon:TileCosmicMuonHT"])
        objKeyStore['inputFile'].removeItem(["TileCosmicMuon:TileCosmicMuonMF"])
        objKeyStore['inputFile'].removeItem(["TileCosmicMuonContainer:TileCosmicMuonHT"])

    if jobproperties.CaloRecFlags.doTileCellCorrection() :
        include( "TileRecAlgs/TileCellCorrection_jobOptions.py" )

if rec.readESD and not rec.doESD:

    # this is needed by CaloTowerContainerCnv
    from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
    svcMgr.ToolSvc += LArTowerBuilderTool("LArTowerEMHEC",
                                          IncludedCalos = ["LAREM","LARHEC"])
    ToolSvc.LArTowerEMHEC.IncludedCalos = ["LAREM","LARHEC"]

# possible hack : when reading ESD modify in put calo cell
## if rec.readESD() and DetFlags.detdescr.Calo_on():
##     from CaloRec.CaloRecConf import CaloCellMaker
##     theCaloCellMaker=CaloCellMaker(CaloCellsOutputName="AllCalo",CaloCellHack=True)    
##     # calo modifying tool to be inserted there
##     from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
##     theCaloNoiseTool = CaloNoiseToolDefault()
##     ToolSvc+=theCaloNoiseTool
    
##     from CaloCellCorrection.CaloCellCorrectionConf import CaloCellRandomizer     
##     theCaloCellRandomizer=CaloCellRandomizer(noiseTool=theCaloNoiseTool,DoGaussRandomization=True)
##     ToolSvc += theCaloCellRandomizer

##     from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
##     from CaloIdentifier import SUBCALO
##     theCaloCellContainerCorrectorTool = CaloCellContainerCorrectorTool(
##                                                        # CaloNums = [ SUBCALO.LAREM,SUBCALO.LARHEC,SUBCALO.LARFCAL],
##                                                        #CaloNums = [ SUBCALO.TILE],#SUBCALO.TILE
##                                                        CaloNums = [ SUBCALO.NSUBCALO],
##                                                        CellCorrectionToolNames = [theCaloCellRandomizer] )
##     ToolSvc += theCaloCellContainerCorrectorTool
##     theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCorrectorTool ]



##     from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool     
##     theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
##     ToolSvc += theCaloCellContainerFinalizerTool
##     theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool ]

##     from CaloRec.CaloRecConf import CaloCellContainerCheckerTool   
##     theCaloCellContainerCheckerTool=CaloCellContainerCheckerTool()     
##     # FIXME
##     theCaloCellContainerCheckerTool.OutputLevel=DEBUG
##     ToolSvc += theCaloCellContainerCheckerTool
##     theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool] 

##     topSequence+=theCaloCellMaker;


# create LArFebErrorSummary for BS input
if rec.doLArg() and globalflags.DataSource()=='data' and globalflags.InputFormat() == 'bytestream':
    from LArROD.LArRODFlags import larRODFlags
    if larRODFlags.doLArFebErrorSummary() :
        try:
            include("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
        except Exception:
            treatException("Problem with LArFebErrorSummaryMaker_jobOptions.py switch larRODFlags.doLArFebErrorSummary ")
            larRODFlags.doLArFebErrorSummary=False



#
# functionality: Calorimeter cells
#
if DetFlags.makeRIO.Calo_on() and not rec.doWriteBS() : 
    from AthenaCommon.Include import excludeTracePattern
    excludeTracePattern.append("*/CaloClusterCorrection/common.py")

    try:
        include ("TileRec/TileDefaults_jobOptions.py")
    except Exception:
        treatException("Could not set up Tile default options.")

# CaloCellGetter
#
    try:
        from CaloRec.CaloCellGetter import CaloCellGetter
        CaloCellGetter()

        # for data09_calophys project, force to use the same quality cut as in BS converter
        if globalflags.DataSource()=='data':
            if rec.projectName()=="data09_calophys":
                if rec.doTile():
                    from AthenaCommon.AppMgr import ToolSvc
                    ToolSvc.TileCellBuilder.QualityCut = 15
    except Exception:
        treatException("Problem with CaloCellGetter. Switched off.")
        DetFlags.makeRIO.Calo_setOff()

# CaloCellGetter_DigiHSTruth
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.doDigiTruth():
      try:
        from CaloRec.CaloCellGetter_DigiHSTruth import CaloCellGetter_DigiHSTruth
        CaloCellGetter_DigiHSTruth()

      except Exception:
        treatException("Problem with CaloCellGetter_DigiHSTruth. Switched off.")



#
#
# functionality : Calorimeter combined clustering
#
if jobproperties.CaloRecFlags.doCaloCluster() and DetFlags.haveRIO.Calo_on() :    
    try:
        from CaloRec.CaloClusterSWCmbGetter import CaloClusterSWCmbGetter
        CaloClusterSWCmbGetter()
    except Exception:    
        treatException("Problem with CaloSWCmbCluster. Switched off.")
        jobproperties.CaloRecFlags.doCaloCluster=False        
else:
        jobproperties.CaloRecFlags.doCaloCluster=False        
#
# functionality : LAr Calorimeter clustering
#
if jobproperties.CaloRecFlags.doEmCluster() and DetFlags.haveRIO.LAr_on() :

    try:
        include( "LArClusterRec/LArCluster_jobOptions.py" )
        
        # introduce multisize possibility
        # include( "LArClusterRec/LArCluster_MultiSize_jobOptions.py" )
    except Exception:        
        treatException("Problem with LArCluster. Switched off.")
        jobproperties.CaloRecFlags.doEmCluster=False    

    # write digits of EM clusters
    if jobproperties.CaloRecFlags.doEMDigits() and globalflags.DataSource()=='data' and globalflags.InputFormat() == 'bytestream': 
        try:
            include ("LArClusterRec/LArDigits_fromEMCluster_jobptions.py")
        except Exception:
            treatException("Problem with LArDigitsFromEMClust. Switched off.")
            jobproperties.CaloRecFlags.doEMDigits=False


else:
    jobproperties.CaloRecFlags.doEmCluster=False    

#
# functionality : Topological clustering   (combined and EM)
#
if jobproperties.CaloRecFlags.doCaloTopoCluster() and DetFlags.haveRIO.Calo_on() :
    try:
        include( "CaloRec/CaloTopoCluster_jobOptions.py" )
    except Exception:
        treatException("Problem with CaloTopoCluster. Switched off.")
        jobproperties.CaloRecFlags.doCaloTopoCluster=False        
else:
    jobproperties.CaloRecFlags.doCaloTopoCluster=False

if jobproperties.CaloRecFlags.doCaloEMTopoCluster() and  DetFlags.haveRIO.Calo_on() :
    try:
        include( "CaloRec/EMTopoCluster_jobOptions.py" )    
    except Exception:
        treatException("Problem with EMTopoCluster. Switched off")
        jobproperties.CaloRecFlags.doCaloTopoCluster=False
else:
    jobproperties.CaloRecFlags.doCaloEMTopoCluster=False

#
# functionality : Noise suppressed tower
#
if jobproperties.CaloRecFlags.doCaloTopoTower() and DetFlags.haveRIO.Calo_on():
   try:
       include ("CaloRec/CaloTopoTower_jobOptions.py")
   except Exception:
       treatException("Problem with CaloTopoTower. Switched off.")
       jobproperties.CaloRecFlags.doCaloTopoTower=False
else:
   jobproperties.CaloRecFlags.doCaloTopoTower=False

#
# functionality : muon candidates in Tile
#
if DetFlags.haveRDO.Tile_on() and DetFlags.detdescr.Tile_on() :
    # this part is needed if we are reading Geant4 RDO created before 14.5.0
    # but it's also safe to include this jobOptions even for new Geant4 RDO
    if globalflags.DataSource()=='geant4' and rec.doTrigger() :
        try:
            include( "TileL2Algs/TileL2Algs_jobOptions.py" )
        except Exception:
            treatException("Could not set up TileMuId at ROD")

if jobproperties.CaloRecFlags.doTileMuId() :

    if DetFlags.haveRIO.Tile_on() :
        from AthenaCommon.BeamFlags import jobproperties
        try:
            # will merge these 2 jobOptions eventually
            if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam' :
                include ("TileMuId/TileMuId_cosmics_jobOptions.py")
            else:
                include( "TileMuId/TileMuId_jobOptions.py" )
        except Exception:
            treatException("Could not set up TileMuId")

        # Tile Muon fitter to find cosmic muon tracks
        if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam' :
            try:
                include( "TileCosmicAlgs/TileMuonFitter_jobOptions.py")
            except Exception:
                treatException("Could not set up TileMuonFitter")

    
#
# information on Calo AffectedRegion, write metadata
#
if DetFlags.dcs.LAr_on():
    if jobproperties.CaloRecFlags.doLArAffectedRegion() and rec.doESD and (not rec.readESD):
        try:
            include ("LArAffectedRegion/LArAffectedRegion_write.py")
        except Exception: 
            treatException("Could not setup LArAffectedRegion")

#
# find noise LAr regions/ events
if jobproperties.CaloRecFlags.doLArNoisyRO() and rec.doESD:
    try:
        from LArCellRec.LArNoisyROSummaryGetter import LArNoisyROSummaryGetter
        LArNoisyROSummaryGetter()
    except Exception:
        treatException("Problem with LArNoisyROSummaryGetter. Switched off.")

#Fill the  background word from MBTS
if rec.doTile() and rec.doESD() and jobproperties.CaloRecFlags.doFillMBTSBackgroundBit():
    try:
        include ("TileRecAlgs/MBTSTimeDiffEventInfoAlg_jobOptions.py")
    except Exception:
        treatException("Problem with MBTSTimeDiffEventInfoAlg.")

# Fill error bit in Event Info to veto noise LAr burst (done in raw->esd stage)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if  globalflags.DataSource()=='data' and jobproperties.CaloRecFlags.doLArNoiseBurstVeto() and rec.doESD() and rec.doLArg() and (not athenaCommonFlags.isOnline()):
    try:
        include("LArCellRec/LArTimeVetoAlg_jobOptions.py")
    except Exception:
        treatException("Problem with LArTimeVetoAlg")


if jobproperties.CaloRecFlags.doCaloTowerFromCells() and rec.doESD  and rec.doCalo():
    from CaloRec.CaloRecConf import CaloTowerxAODFromCells
    topSequence+=CaloTowerxAODFromCells(CaloTowerContainer="CmbTowers",
#                                        doCrossChecks=True
                                        )

if jobproperties.CaloRecFlags.doCaloTowerFromCluster() and rec.doESD  and rec.doCalo():
    from CaloRec.CaloRecConf import CaloTowerxAODFromClusters
    topSequence+=CaloTowerxAODFromClusters(CaloTowerContainer="CmbTopoTowers",
#                                           doCrossChecks=True
                                           )
    



#Convert clusters to xAOD::CaloCluster (temporary solution)
if rec.doWritexAOD():
    from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
    topSequence+=ClusterCreator("CaloCluster2xAOD")

    #CaloClusterKeys=("CaloCalTopoCluster", "CaloTopoCluster", "EMTopoCluster430", "LArClusterEM", "LArClusterEM7_11Nocorr",
    #                 "CombinedCluster","EMTopoSW35","egClusterCollection","LArClusterEMSofte")
    #for k in CaloClusterKeys:
    #    itemname="CaloClusterContainer#"+k
    #    if objKeyStore["transient"].has_item(itemname):#  "CaloClusterContainer",k):
    #        print "Scheduling xAOD converter for CaloClusterContainer with key",k
    #        topSequence+=ClusterCreator("ClusterxAOD_"+k,AODContainerName=k,xAODContainerName=k,CellContainerName="AllCalo")
    #    else:
    #        print "CaloClusterContainer with key",k,"not found, no xAOD converter scheduled"
    #        pass
    #    pass
    
    
#L1Calo Trigger tower decoration
if globalflags.DataSource()=='data' and rec.doESD() and rec.doCalo() and rec.doTrigger():
    include("TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py")
