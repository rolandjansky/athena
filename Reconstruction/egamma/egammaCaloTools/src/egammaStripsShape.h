/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H
#define EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H

/// @class egammaStripsShape
/// @brief   EM cluster shower shape calculations in 1st ECAL sampling
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///

class CaloCellContainer;
class CaloDetDescrManager;
class LArEM_ID;

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "xAODCaloEvent/CaloCluster.h"

class egammaStripsShape
  : public AthAlgTool
  , virtual public IegammaStripsShape
{

public:
  /** @brief Default constructor*/
  egammaStripsShape(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  /** @brief Destructor*/
  ~egammaStripsShape() = default;

  /** @brief AlgTool initialize method.*/
  StatusCode initialize() override;
  /** @brief AlgTool finalize method */
  StatusCode finalize() override;

  /** @brief AlgTool main method */
  virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                             const CaloDetDescrManager& cmgr,
                             Info& info) const override final;

private:
  /** @brief boolean to calculate all variables*/
  Gaudi::Property<bool> m_ExecAllVariables{ this,
                                            "ExecAllVariables",
                                            true,
                                            "flag used by trigger" };
};

#endif
