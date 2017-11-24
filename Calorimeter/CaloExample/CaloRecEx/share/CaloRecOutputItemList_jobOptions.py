#**************   ESD list  ************************************************

from AthenaCommon.JobProperties import jobproperties

CaloESDList = []

CaloESDList += [ "CaloCellContainer#AllCalo" ]

# compactify calo cell
from CaloTools.CaloToolsConf import CaloCompactCellTool
svcMgr.ToolSvc += CaloCompactCellTool()

# add explicitly E4', MBTS cells and trigger output to ESD
CaloESDList += [ "TileCellContainer#E4prContainer" ]
CaloESDList += [ "TileCellContainer#MBTSContainer" ]
CaloESDList += [ "TileTTL1Container#TileTTL1MBTS" ]

# explicitly list of CaloCluster related keys
CaloClusterItemList=[]
CaloClusterKeys=[]

CaloClusterKeys+=["CaloCalTopoClusters"]
CaloClusterKeys+=["CombinedCluster"]
#CaloClusterKeys+=["EMTopoCluster430"]
CaloClusterKeys+=["EMTopoSW35"]

# reshuffl em 
CaloClusterKeys+=["LArClusterEM"]

CaloClusterKeys+=["LArClusterEM7_11Nocorr"]
#CaloClusterKeys+=["egClusterCollection"] Move to eg output list
#CaloClusterKeys+=["LArClusterEMSofte"]

if rec.Commissioning():
    # CaloClusterKeys += ["LArMuClusterCandidates","CaloTopoCluster","EMTopoCluster420","TileTopoCluster","MuonClusterCollection"]
    CaloClusterKeys += ["MuonClusterCollection"]    

for theKey in CaloClusterKeys:
    CaloClusterItemList+=["xAOD::CaloClusterContainer#"+theKey]
    CaloClusterItemList+=["xAOD::CaloClusterAuxContainer#"+theKey+"Aux."]
    CaloClusterItemList+=["CaloClusterCellLinkContainer#"+theKey+"_links"]

CaloESDList+=CaloClusterItemList
logRecoOutputItemList_jobOptions.info('CaloClusterItemList = ')
print CaloClusterItemList

#CaloTowerContainer needed for jet->cell
#FIXME
CaloESDList += [ "CaloTowerContainer#CombinedTower" ]

# LAr thinned digits
CaloESDList += ["LArDigitContainer#LArDigitContainer_Thinned"]

# LAr FEB error summary data 
CaloESDList += ["LArFebErrorSummary#LArFebErrorSummary"]

# LAr thinned digits for LArMuId
if jobproperties.Beam.beamType() == "cosmics" :
    CaloESDList += ["LArDigitContainer#LArDigitContainer_IIC"]
# LAr thinned digits from EM clusters
CaloESDList += ["LArDigitContainer#LArDigitContainer_EMClust"]

# LAr noisy Feb/PA summary
CaloESDList += ["LArNoisyROSummary#LArNoisyROSummary"]

# Tile filtered digits, muons reconstructed offline or inside ROD
CaloESDList += ["TileDigitsContainer#TileDigitsFlt"]
CaloESDList += ["TileRawChannelContainer#TileRawChannelFlt"]
CaloESDList += ["TileMuContainer#TileMuObj"]
CaloESDList += ["TileL2Container#TileL2Cnt"]

# Tile TMDB output
CaloESDList += ["TileDigitsContainer#MuRcvDigitsCnt"]
CaloESDList += ["TileRawChannelContainer#MuRcvRawChCnt"]
CaloESDList += ["TileMuonReceiverContainer#TileMuRcvCnt"]

# Cosmic muons reconstructed with Tile muon fitter
if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam' :
    CaloESDList +=["TileCosmicMuonContainer#TileCosmicMuonHT","TileCosmicMuonContainer#TileCosmicMuonMF"]


#**************   AOD list  ************************************************

#List of AOD moments: (copied from CaloClusterTopoGetter)

AODMoments=[#"LATERAL"
            #,"LONGITUDINAL"
            "SECOND_R" 
            ,"SECOND_LAMBDA"
            ,"CENTER_MAG"
            ,"CENTER_LAMBDA"
            ,"FIRST_ENG_DENS"
            ,"ENG_FRAC_MAX" 
            ,"ISOLATION"
            ,"ENG_BAD_CELLS"
            ,"N_BAD_CELLS"
            ,"BADLARQ_FRAC"
            #,"ENG_BAD_HV_CELLS"
            #,"N_BAD_HV_CELLS"
            ,"ENG_POS"
            #,"SIGNIFICANCE"
            #,"CELL_SIGNIFICANCE"
            #,"CELL_SIG_SAMPLING"
            ,"AVG_LAR_Q"
            ,"AVG_TILE_Q"
            ,"EM_PROBABILITY"
            #,"PTD"
            ,"BadChannelList"
            "SECOND_R_Truth"
            ,"SECOND_LAMBDA_Truth"
            ,"CENTER_MAG_Truth"
            ,"CENTER_LAMBDA_Truth"
            ,"FIRST_ENG_DENS_Truth"
            ,"ENG_FRAC_MAX_Truth"
            ,"ISOLATION_Truth"
            ,"ENG_BAD_CELLS_Truth"
            ,"N_BAD_CELLS_Truth"
            ,"BADLARQ_FRAC_Truth"
            #,"ENG_BAD_HV_CELLS_Truth"
            #,"N_BAD_HV_CELLS_Truth"
            ,"ENG_POS_Truth"
            #,"SIGNIFICANCE_Truth"
            #,"CELL_SIGNIFICANCE_Truth"
            #,"CELL_SIG_SAMPLING_Truth"
            ,"AVG_LAR_Q_Truth"
            ,"AVG_TILE_Q_Truth"
            ,"EM_PROBABILITY_Truth"
            #,"PTD_Truth"
            ,"ENERGY_Truth"
            ,"ETA_Truth"
            ,"PHI_Truth"
            ]


CaloAODList = []

# cells 
CaloAODList+=["CaloCellContainer#AODCellContainer"]

# CaloClusters 
CaloClusterItemList=[]
CaloClusterKeys=[]


CaloClusterKeys+=["CaloCalTopoClusters"]
CaloClusterKeys+=["CombinedCluster"]
#CaloClusterKeys+=["EMTopoCluster430"]
CaloClusterKeys+=["EMTopoSW35"]

#CaloClusterKeys+=["egClusterCollection"] Moved to eg output item list
#CaloClusterKeys+=["LArClusterEMSofte"]



for theKey in CaloClusterKeys: #Fixme .. Apply this only to TopoClusters?
     CaloClusterItemList+=["xAOD::CaloClusterContainer#"+theKey]
     AuxListItem="xAOD::CaloClusterAuxContainer#"+theKey+"Aux"
     for moment in AODMoments:
         AuxListItem+="."+moment
         pass
     if len(AODMoments)==0: AuxListItem+="." 
     CaloClusterItemList+=[AuxListItem]

# write the link only for egClusterColl
#CaloClusterItemList+=["CaloClusterCellLinkContainer#egClusterCollection_links"]


CaloAODList+=CaloClusterItemList

CaloAODList+=["CaloClusterContainer#Tau1P3PPi0ClusterContainer"]

# E4' cells
CaloAODList+=["TileCellContainer#E4prContainer"]

# MBTS cells
CaloAODList+=["TileCellContainer#MBTSContainer"]

# muons in Tile
CaloAODList+=["TileMuContainer#TileMuObj"]

# LAr noisy Feb/PA summary
CaloAODList +=  ["LArNoisyROSummary#LArNoisyROSummary"]

