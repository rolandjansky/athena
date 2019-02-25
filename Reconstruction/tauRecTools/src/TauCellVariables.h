/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCELLVARIABLES_H
#define	TAUREC_TAUCELLVARIABLES_H

#include "tauRecTools/TauRecToolBase.h"

class LArEM_ID;
class TileID;

/**
 * @brief Calculate tau calorimeter variables from cell information.
 * 
 * @authors  Srini Rajagopalan, Anna Kaczmarska, Felix Friedrich
 */

class TauCellVariables : public TauRecToolBase {

public:
    TauCellVariables(const std::string& name);
    ASG_TOOL_CLASS2(TauCellVariables, TauRecToolBase, ITauToolBase);
    ~TauCellVariables();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize();
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual void print() const { }


private:
    double m_cellEthr;  //!< EM cell E threshold
    double m_stripEthr; //!< cell E threshold for strips
    double m_EMSumThr;  //!< threshold for 4-vector EM sum
    double m_EMSumR;    //!< radius for 4-vector EM sum
    double m_cellCone;  //!< outer cone for cells used in calculations

    const LArEM_ID* m_emid;
    const TileID* m_tileid;

    /** 
     * enable cell origin correction 
     * eta and phi of the cells are corrected wrt to the origin of the tau vertex
     */
    bool m_doCellCorrection;
};

#endif	/* TAUREC_TAUCELLVARIABLES_H */

