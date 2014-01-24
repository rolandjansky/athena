# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import ForwardDetectorsD3PDMaker

AfpSIDSimHitD3PDObject=make_SG_D3PDObject("AtlasHitsVector<AFP_SIDSimHit>","AFP_SIDSimHitCollection","afp_sidsim_", "AfpSIDSimHitD3PDObject" )
AfpSIDSimHitD3PDObject.defineBlock(0, 'AFP_SIDSimHitCollection', ForwardDetectorsD3PDMaker.AfpSIDSimHitCollectionFillerTool)

AfpTDSimHitD3PDObject=make_SG_D3PDObject("AtlasHitsVector<AFP_TDSimHit>","AFP_TDSimHitCollection","afp_tdsim_", "AfpTDSimHitD3PDObject" )
AfpTDSimHitD3PDObject.defineBlock(0, 'AFP_TDSimHitCollection', ForwardDetectorsD3PDMaker.AfpTDSimHitCollectionFillerTool)

AfpSIDDigiD3PDObject=make_SG_D3PDObject("AtlasHitsVector<AFP_SiDigi>","AFP_SiDigiCollection","afp_siddigi_", "AfpSIDDigiD3PDObject" )
AfpSIDDigiD3PDObject.defineBlock(0, 'AFP_SiDigiCollection', ForwardDetectorsD3PDMaker.AfpSIDDigiCollectionFillerTool)

AfpTDDigiD3PDObject=make_SG_D3PDObject("AtlasHitsVector<AFP_TDDigi>","AFP_TDDigiCollection","afp_tddigi_", "AfpTDDigiD3PDObject" )
AfpTDDigiD3PDObject.defineBlock(0, 'AFP_TDDigiCollection', ForwardDetectorsD3PDMaker.AfpTDDigiCollectionFillerTool)

AfpSIDLocRecoEvD3PDObject=make_SG_D3PDObject("AFP_SIDLocRecoEvCollection","AFP_SIDLocRecoEvCollection","afp_sidreco_", "AfpSIDLocRecoEvD3PDObject" )
AfpSIDLocRecoEvD3PDObject.defineBlock(0, 'AFP_SIDLocRecoEvCollection', ForwardDetectorsD3PDMaker.AfpSIDLocRecoEvCollectionFillerTool)

AfpTDLocRecoEvD3PDObject=make_SG_D3PDObject("AFP_TDLocRecoEvCollection","AFP_TDLocRecoEvCollection","afp_tdreco_", "AfpTDLocRecoEvD3PDObject" )
AfpTDLocRecoEvD3PDObject.defineBlock(0, 'AFP_TDLocRecoEvCollection', ForwardDetectorsD3PDMaker.AfpTDLocRecoEvCollectionFillerTool)

AfpTruthInfoD3PDObjectTE = make_SG_D3PDObject( "McEventCollection", "TruthEvent", "afpmc_", "AfpTruthInfoD3PDObject" )
AfpTruthInfoD3PDObjectTE.defineBlock(0, 'AFP_McEventCollection', ForwardDetectorsD3PDMaker.AfpTruthInfoFillerTool )

AfpTruthInfoD3PDObjectAOD = make_SG_D3PDObject( "McEventCollection", "GEN_AOD", "afpmc_", "AfpTruthInfoD3PDObject" )
AfpTruthInfoD3PDObjectAOD.defineBlock(0, 'AFP_McEventCollection', ForwardDetectorsD3PDMaker.AfpTruthInfoFillerTool )
