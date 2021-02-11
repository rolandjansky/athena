/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUCELLVARIABLES_H
#define TAURECTOOLS_TAUCELLVARIABLES_H

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
    virtual ~TauCellVariables() = default;
    
    /** @brief Perform the calculation of cell variables for each tau candidate */
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;

  private:
    
    /** @brief Check whether the CaloSample is a EM layer */ 
    bool isEMLayer(const CaloSampling::CaloSample& calo) const;

    /** @brief Check whether the CaloSample is a Strip layer */
    bool isStripLayer(const CaloSampling::CaloSample& calo) const;

    Gaudi::Property<double> m_stripEthr {this, "StripEthreshold", 0.2 * Gaudi::Units::GeV, "energy threshould for strip cell"};
    Gaudi::Property<double> m_cellCone {this, "CellCone", 0.2, "outer cone for cells used in calculation"};
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of vertex correction"};
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

#endif // TAURECTOOLS_TAUCELLVARIABLES_H
