/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
 
  // constructor 
  CaloSwGap_g3(const std::string& type,
               const std::string& name,
               const IInterface* parent);
  // destructor 
  virtual ~CaloSwGap_g3() override;

  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;

  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster* cluster) const override;


  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name);


 private:

  CaloSwGap_g3() = delete;
 
  float             m_etamin_crack;
  float             m_etamax_crack;
  CaloRec::Array<1> m_scint_weight;
  CaloRec::Array<1> m_correction;

  /// Property: The name of the container in which to look to find tile cells.
  SG::ReadHandleKey<CaloCellContainer> m_cells_name;
};
#endif
