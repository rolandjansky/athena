/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * Name :	NoiseStudy.h
 * PACKAGE :	Trigger/TrigL1Upgrade/TrigL1CaloUpgrade/NoiseStudy
 *
 * AUTHOR :	Denis Oliveira Damazio
 *
 * PURPOSE :	Many checks about SuperCell reconstruction
 *
 * **/

#ifndef TRIGT1CALOANALYSIS_NOISESTUDY
#define TRIGT1CALOANALYSIS_NOISESTUDY

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloConditions/CaloNoise.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
class TFile;
class TH1I;
class TH1F;
class TH2F;
class TNtuple;
class CaloCell_SuperCell_ID;


class NoiseStudy : public AthReentrantAlgorithm
{
public :

	// Usual athena stuff
	NoiseStudy( const std::string& name, ISvcLocator* pSvcLocator );
	virtual ~NoiseStudy();
	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute(const EventContext& context) const;

private :

	/** For cell <-> SCell comparisons */
	ToolHandle<ICaloSuperCellIDTool>     m_scidtool
        { this, "SCIDTool", "CaloSuperCellIDTool" ,
                "Offline / supercell mapping tool."};
	/// Property SG Key for the Expected Noise Sigma diff in diff gains
	SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","electronicNoise","SG Key of CaloNoise data object"};
	
	/// Property SG Key for the CaloLumiBCID
	SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey
	  {this, "BCIDAvgKey", "CaloBCIDAverage", "" };

       /// Property: SG key for the input calorimeter cell container.
       SG::ReadHandleKey<CaloCellContainer> m_sCellContainerInBaseKey
        { this, "SCellContainerBaseIn", "SCell",
                "SG key for the input supercell LAr channel container"};

       /// Property: SG key for the input calorimeter cell container.
       SG::ReadHandleKey<CaloCellContainer> m_sCellContainerInKey
        { this, "SCellContainerIn", "SCellContainer",
                "SG key for the input supercell LAr channel container"};

       /// Property: SG key for the input calorimeter cell container.
       SG::ReadHandleKey<CaloCellContainer> m_cellContainerInKey
        { this, "CellContainerIn", "AllCalo",
                "SG key for the input Calo container"};

       /// Property: SG key for the input Event Info container.
       SG::ReadHandleKey<xAOD::EventInfo> m_evtInKey
        { this, "EventInfoKey", "EventInfo",
                "SG key for the input EventInfo container"};

       /// Property: SG key for the input Truth Info container.
       SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthInKey
        { this, "TruthInfoKey", "TruthParticles",
                "SG key for the input TruthParticles container"};


        const CaloCell_SuperCell_ID*     m_schelper;
	TNtuple* m_ntuple;
	TFile * m_file;
	bool m_compNoise;
	bool m_addBCID;

};

#endif
