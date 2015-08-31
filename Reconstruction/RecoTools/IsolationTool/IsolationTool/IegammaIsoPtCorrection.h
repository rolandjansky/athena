/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaIsoPtCorrection.h, (c) ATLAS Detector software 2010
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMAISOPTCORRECTION_H
#define EGAMMAINTERFACES_IEGAMMAISOPTCORRECTION_H

/// @class IegammaIsoPtCorrection 
/// @brief Interface for the Reconstruction/egamma/egammaCaloTools/egammaIsoPtCorrection
///
/// @author T. Cuhadar Donszelmann
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
#include "xAODEgamma/EgammaFwd.h"

static const InterfaceID IID_IegammaIsoPtCorrection("IEgammaIsoPtCorrection", 1, 0);
        
class IegammaIsoPtCorrection : virtual public IAlgTool {
     
 public:
  /** @brief Virtual destructor */
  virtual ~IegammaIsoPtCorrection(){};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  /** @brief pt correction for etcone */
  virtual double CalcPtCorrection(const xAOD::Egamma* eg, double dR, std::string s="") = 0;

};

inline const InterfaceID& IegammaIsoPtCorrection::interfaceID()
{
  return IID_IegammaIsoPtCorrection;
}

#endif // EGAMMAINTERFACES_IEGAMMAISOPTCORRECTION_H
