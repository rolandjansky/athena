/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    
    ASG_TOOL_CLASS2(TauCellVariables, TauRecToolBase, ITauToolBase);
    
    TauCellVariables(const std::string& name);
    ~TauCellVariables();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) override;



private:
    double m_cellEthr;  //!< EM cell E threshold
    double m_stripEthr; //!< cell E threshold for strips
    double m_cellCone;  //!< outer cone for cells used in calculations

    /** 
     * enable cell origin correction 
     * eta and phi of the cells are corrected wrt to the origin of the tau vertex
     */
    bool m_doVertexCorrection;
};

#endif	/* TAUREC_TAUCELLVARIABLES_H */

