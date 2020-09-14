/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCELLVARIABLES_H
#define	TAUREC_TAUCELLVARIABLES_H

#include "tauRecTools/TauRecToolBase.h"

/**
 @brief Calculate tau calorimeter variables from cell information.
 @warning Cells are selected relative to the tau intermediate axis. There are no checks. 
 @authors  Srini Rajagopalan, Anna Kaczmarska, Felix Friedrich
 */

class TauCellVariables : public TauRecToolBase {

  public:
    
    ASG_TOOL_CLASS2(TauCellVariables, TauRecToolBase, ITauToolBase);
   
    /** @brief Constructor */ 
    TauCellVariables(const std::string& name);
    
    /** @brief Destructor */
    ~TauCellVariables();
    
    /** @brief Perform the calculation of cell variables for each tau candidate */
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;

  private:
    
    /** @brief Check whether the CaloSample is a EM layer */ 
    bool isEMLayer(const CaloSampling::CaloSample& calo) const;

    /** @brief Check whether the CaloSample is a Strip layer */
    bool isStripLayer(const CaloSampling::CaloSample& calo) const;

    Gaudi::Property<double> m_cellEthr {this, "CellEthreshold", 0.2 * Gaudi::Units::GeV, "energy threshould of EM cell"};
    Gaudi::Property<double> m_stripEthr {this, "StripEthreshold", 0.2 * Gaudi::Units::GeV, "energy threshould for strip cell"};
    Gaudi::Property<double> m_cellCone {this, "CellCone", 0.2, "outer cone for cells used in calculation"};
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of vertex correction"};
    Gaudi::Property<bool> m_incShowerSubtr {this, "IncShowerSubtr", true, "use shower subtracted clusters in calo calculations"};
};

//______________________________________________________________________________
inline bool TauCellVariables::isEMLayer(const CaloSampling::CaloSample& calo) const {
  if ((calo == CaloSampling::PreSamplerB) ||
      (calo == CaloSampling::PreSamplerE) ||
      (calo == CaloSampling::EMB1) ||
      (calo == CaloSampling::EME1) ||
      (calo == CaloSampling::EMB2) ||
      (calo == CaloSampling::EME2)) {
    return true;
  }
  else {
    return false;
  }
}

//______________________________________________________________________________
inline bool TauCellVariables::isStripLayer(const CaloSampling::CaloSample& calo) const {
  if ((calo == CaloSampling::EMB1) ||
      (calo == CaloSampling::EME1)) {
    return true;
  }
  else {
    return false;
  }
}

#endif	/* TAUREC_TAUCELLVARIABLES_H */
