/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSHOTFINDER_H
#define	TAUREC_TAUSHOTFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

class CaloDetDescrManager;
class CaloCell_ID;
class IHadronicCalibrationTool;

class TauShotFinder : public TauRecToolBase {
public:
    TauShotFinder(const std::string& name);
    ASG_TOOL_CLASS2(TauShotFinder, TauRecToolBase, ITauToolBase);
    virtual ~TauShotFinder();

    virtual StatusCode initialize();
    virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& tauShotCaloClusContainer, xAOD::PFOContainer& tauShotPFOContainer);
    virtual StatusCode finalize();

private:

    /** @brief tool handles */
    ToolHandle<IHadronicCalibrationTool> m_caloWeightTool;
    
    /** @brief new shot PFO container and name */
    /** @brief calo cell navigation */
    const CaloCell_ID* m_calo_id;

    /** @brief Thanks C++ for ruining my day */
    struct ptSort
    { 
         ptSort( const TauShotFinder& info );
         const TauShotFinder& m_info;
         bool operator()( const CaloCell* c1, const CaloCell* c2 );
    };

    /** @brief get neighbour cells */
    std::vector<const CaloCell*> getNeighbours(const CaloCellContainer*,const CaloCell*,int /*maxDepth*/);

    void addNeighbours(const CaloCellContainer*,
                       const CaloCell* cell,
                       std::vector<const CaloCell*>& cells,
                       int depth,
                       int maxDepth,
                       bool next);

    bool isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash, bool next);

    /** @brief get eta bin */
    float getEtaBin(float /*seedEta*/);
  
    /** @brief get NPhotons in shot */
    float getNPhotons(int /*etaBin*/, 
                      float /*seedEnergy*/);

    // number of cells in eta
    int m_nCellsInEta;

    // cut values
    std::vector<float> m_minPtCut;
    std::vector<float> m_autoDoubleShotCut;
    std::vector<float> m_mergedBDTScoreCut;
  
    // BDT input variables
    float m_pt1;
    float m_pt3;
    float m_pt5;
    /*
    float m_ws5;
    float m_sdevEta5_WRTmean;
    float m_sdevEta5_WRTmode;
    float m_sdevPt5;
    float m_deltaPt12_min;
    float m_Fside_3not1;
    float m_Fside_5not1;
    float m_Fside_5not3;
    float m_fracSide_3not1;
    float m_fracSide_5not1;
    float m_fracSide_5not3;
    float m_pt1OverPt3;
    float m_pt3OverPt5;
    */

    SG::ReadHandleKey<CaloCellContainer> m_caloCellInputContainer{this,"Key_caloCellInputContainer", "AllCalo", "input vertex container key"};
    SG::WriteHandleKey<xAOD::PFOContainer> m_tauPFOOutputContainer{this,"Key_tauPFOOutputContainer", "TauShotParticleFlowObjects", "tau pfo out key"};
    
};

#endif	/* TAUSHOTFINDER_H */

