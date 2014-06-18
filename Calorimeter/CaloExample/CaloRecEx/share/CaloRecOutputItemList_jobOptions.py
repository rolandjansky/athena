#**************   ESD list  ************************************************

from AthenaCommon.JobProperties import jobproperties

CaloESDList = []

CaloESDList += [ "CaloCellContainer#AllCalo" ]

# compactify calo cell
from CaloTools.CaloToolsConf import CaloCompactCellTool
svcMgr.ToolSvc += CaloCompactCellTool()

# add explicitly MBTS cells and trigger output to ESD
CaloESDList += [ "TileCellContainer#MBTSContainer" ]
CaloESDList += [ "TileTTL1Container#TileTTL1MBTS" ]

# explicitly list of CaloCluster related keys
CaloClusterItemList=[]
CaloClusterKeys=[]

CaloClusterKeys+=["CaloCalTopoCluster"]
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
    CaloClusterItemList+=["xAOD::CaloClusterContainer_v1#"+theKey]
    CaloClusterItemList+=["xAOD::CaloClusterAuxContainer_v1#"+theKey+"Aux."]
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

# Cosmic muons reconstructed with Tile muon fitter
if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam' :
    CaloESDList +=["TileCosmicMuonContainer#TileCosmicMuonHT","TileCosmicMuonContainer#TileCosmicMuonMF"]


#**************   AOD list  ************************************************

#List of AOD moments: (copied from CaloClusterTopoGetter)

AODMoments=["LATERAL"
            ,"LONGITUDINAL"
            ,"SECOND_R" 
            ,"SECOND_LAMBDA"
            ,"CENTER_MAG"
            ,"CENTER_LAMBDA"
            ,"FIRST_ENG_DENS"
            ,"ENG_FRAC_MAX" 
            ,"ISOLATION"
            ,"ENG_BAD_CELLS"
            ,"N_BAD_CELLS"
            ,"BADLARQ_FRAC"
            ,"ENG_POS"
            ,"SIGNIFICANCE"
            ,"CELL_SIGNIFICANCE"
            ,"CELL_SIG_SAMPLING"
            ,"AVG_LAR_Q"
            ,"AVG_TILE_Q"
            ,"EM_PROBABILITY"
            ,"BadChannelList"
            ]


CaloAODList = []

# cells 
CaloAODList+=["CaloCellContainer#AODCellContainer"]

# CaloClusters 
CaloClusterItemList=[]
CaloClusterKeys=[]


CaloClusterKeys+=["CaloCalTopoCluster"]
CaloClusterKeys+=["CombinedCluster"]
#CaloClusterKeys+=["EMTopoCluster430"]
CaloClusterKeys+=["EMTopoSW35"]

#CaloClusterKeys+=["egClusterCollection"] Moved to eg output item list
#CaloClusterKeys+=["LArClusterEMSofte"]



for theKey in CaloClusterKeys: #Fixme .. Apply this only to TopoClusters?
     CaloClusterItemList+=["xAOD::CaloClusterContainer_v1#"+theKey]
     AuxListItem="xAOD::CaloClusterAuxContainer_v1#"+theKey+"Aux"
     for moment in AODMoments:
         AuxListItem+="."+moment
         pass
     if len(AODMoments)==0: AuxListItem+="." 
     CaloClusterItemList+=[AuxListItem]

# write the link only for egClusterColl
#CaloClusterItemList+=["CaloClusterCellLinkContainer#egClusterCollection_links"]


CaloAODList+=CaloClusterItemList

CaloAODList+=["CaloClusterContainer#Tau1P3PPi0ClusterContainer"]

# MBTS cells
CaloAODList+=["TileCellContainer#MBTSContainer"]

# muons in Tile
CaloAODList+=["TileMuContainer#TileMuObj"]

# LAr noisy Feb/PA summary
CaloAODList +=  ["LArNoisyROSummary#LArNoisyROSummary"]

