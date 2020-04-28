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
    
    Gaudi::Property<double> m_cellEthr {this, "CellEthreshold", 0.2 * Gaudi::Units::GeV, "energy threshould of EM cell"};
    Gaudi::Property<double> m_stripEthr {this, "StripEthreshold", 0.2 * Gaudi::Units::GeV, "energy threshould for strip cell"};
    Gaudi::Property<double> m_cellCone {this, "CellCone", 0.2, "outer cone for cells used in calculation"};
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of vertex correction"};
};

#endif	/* TAUREC_TAUCELLVARIABLES_H */

