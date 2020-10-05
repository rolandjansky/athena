/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWGAP_G3_H
#define CALOCLUSTERCORRECTION_CALOSWGAP_G3_H

/********************************************************************

NAME:     CaloSwGap_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  L. Carminati
CREATED:  Feb 2003

PURPOSE:  Correction for the barrel-endcap cracks
          Tuned using 100 GeV photons
          Correction tuned on G3 samples.

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/

// INCLUDE HEADER FILES:

#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include "StoreGate/ReadHandleKey.h"

class CaloSwGap_g3 : public CaloClusterCorrection,
                     public ISetCaloCellContainerName
{
public:
  /// Inherit constructor
  using CaloClusterCorrection::CaloClusterCorrection;
 
  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;

  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;


  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) override;


 private:
  CaloSwGap_g3() = delete;
 
  Constant<float>             m_etamin_crack   { this, "etamin_crack", "" };
  Constant<float>             m_etamax_crack   { this, "etamax_crack", "" };
  Constant<CxxUtils::Array<1> > m_scint_weight { this, "scint_weight", "" };
  Constant<CxxUtils::Array<1> > m_correction   { this, "correction",   "" };

  /// Property: The name of the container in which to look to find tile cells.
  SG::ReadHandleKey<CaloCellContainer> m_cells_name
  { this, "cells_name", "AllCalo", "" };
};
#endif
