/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSHOTFINDER_H
#define	TAUREC_TAUSHOTFINDER_H

#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "CaloInterface/IHadronicCalibrationTool.h"

class CaloDetDescrManager;
class CaloCell_ID;

class TauShotFinder : public TauRecToolBase {
public:
    TauShotFinder(const std::string& name);
    ASG_TOOL_CLASS2(TauShotFinder, TauRecToolBase, ITauToolBase);
    virtual ~TauShotFinder();

    virtual StatusCode initialize() override;
    virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& tauShotCaloClusContainer, xAOD::PFOContainer& tauShotPFOContainer) const override;

private:

    /** @brief tool handles */
    ToolHandle<IHadronicCalibrationTool> m_caloWeightTool {this, "CaloWeightTool", "H1WeightToolCSC12Generic"};
    
    /** @brief new shot PFO container and name */
    /** @brief calo cell navigation */
    const CaloCell_ID* m_calo_id = NULL;

    /** @brief Thanks C++ for ruining my day */
    struct ptSort
    { 
         ptSort( const TauShotFinder& info );
         const TauShotFinder& m_info;
         bool operator()( const CaloCell* c1, const CaloCell* c2 );
    };

    /** @brief get neighbour cells */
    std::vector<const CaloCell*> getNeighbours(const CaloCellContainer*,const CaloCell*,int /*maxDepth*/) const;

    void addNeighbours(const CaloCellContainer*,
                       const CaloCell* cell,
                       std::vector<const CaloCell*>& cells,
                       int depth,
                       int maxDepth,
                       bool next) const;

    bool isPhiNeighbour(IdentifierHash cell1Hash, IdentifierHash cell2Hash, bool next) const;

    /** @brief get eta bin */
    float getEtaBin(float /*seedEta*/) const;
  
    /** @brief get NPhotons in shot */
    float getNPhotons(int /*etaBin*/, 
                      float /*seedEnergy*/) const;

    // number of cells in eta
    Gaudi::Property<int> m_nCellsInEta {this, "NCellsInEta"};
    Gaudi::Property<std::vector<float>> m_minPtCut {this, "MinPtCut"};
    Gaudi::Property<std::vector<float>> m_autoDoubleShotCut {this, "AutoDoubleShotCut"};
  
    SG::ReadHandleKey<CaloCellContainer> m_caloCellInputContainer{this,"Key_caloCellInputContainer", "AllCalo", "input vertex container key"};
    SG::WriteHandleKey<xAOD::PFOContainer> m_tauPFOOutputContainer{this,"Key_tauPFOOutputContainer", "TauShotParticleFlowObjects", "tau pfo out key"};
    
};

#endif	/* TAUSHOTFINDER_H */

