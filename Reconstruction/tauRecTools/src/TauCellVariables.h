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
    virtual StatusCode execute(xAOD::TauJet& pTau);


    virtual void print() const { }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

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

