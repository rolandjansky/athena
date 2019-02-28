/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSHOTFINDER_H
#define	TAUREC_TAUSHOTFINDER_H

//#include <string>
//#include <vector>
//#include <map>

#include "GaudiKernel/ToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFOAuxContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

//#include "CaloIdentifier/CaloCell_ID.h"

class CaloDetDescrManager;
class CaloCell_ID;
class IHadronicCalibrationTool;

/*
namespace TMVA{
    class Reader;
}
*/

/**
 * @brief Find photon shots in the EM1 strip layer.
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch>
 * @author Stephanie Yuen <stephanie.yuen@cern.ch> 
 */

class TauShotFinder : public TauRecToolBase {
public:
    TauShotFinder(const std::string& name);
    ASG_TOOL_CLASS2(TauShotFinder, TauRecToolBase, ITauToolBase);
    virtual ~TauShotFinder();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize();
    virtual StatusCode executeShotFinder(xAOD::TauJet& pTau, xAOD::CaloClusterContainer& tauShotCaloClusContainer, xAOD::PFOContainer& tauShotPFOContainer);
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode execute(xAOD::TauJet&){ return StatusCode::SUCCESS; };
    virtual StatusCode eventFinalize();
    virtual StatusCode finalize();

    virtual void print() const { }


private:

    /** @brief tool handles */
    ToolHandle<IHadronicCalibrationTool> m_caloWeightTool;
    
    /** @brief new shot PFO container and name */
    /** @brief calo cell navigation */
    const CaloDetDescrManager* m_calo_dd_man;
    const CaloCell_ID* m_calo_id;

    /** @brief readers */
    /*
    std::string m_readerOption;

    TMVA::Reader *m_tmvaReader_barrel;
    std::string m_weightfile_barrel;

    TMVA::Reader *m_tmvaReader_endcap1;
    std::string m_weightfile_endcap1;

    TMVA::Reader *m_tmvaReader_endcap2;
    std::string m_weightfile_endcap2;
    */

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
  
    /** @brief get merged BDTscore */
    // float getMergedBDTScore(int /*etaBin*/);

    /** @brief get NPhotons in shot */
    float getNPhotons(int /*etaBin*/, 
                      float /*mergedBDTScore*/, 
                      float /*seedEnergy*/);

    
    /** @brief Book TMVA methods. */
    /*
    StatusCode bookMethod(TMVA::Reader *reader_barrel, 
                          TMVA::Reader *reader_endcap1, 
                          TMVA::Reader *reader_endcap2, 
                          const std::string &methodName) const;
    */

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

    /*
    // FIXME: same variable names as in Stephanie's private code atm
    float G_PTFRAC;
    float G_STDPT_5;
    float G_STDETA_5;
    float G_DELTAPT_MIN;
    */

    SG::ReadHandleKey<CaloCellContainer> m_caloCellInputContainer{this,"Key_caloCellInputContainer", "AllCalo", "input vertex container key"};
    SG::WriteHandleKey<xAOD::PFOContainer> m_tauPFOOutputContainer{this,"Key_tauPFOOutputContainer", "TauShotParticleFlowObjects", "tau pfo out key"};
    
};

#endif	/* TAUSHOTFINDER_H */

