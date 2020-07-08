/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2CALOCALIBRATION_EGAMMAGAPCALIBRATION
#define TRIGT2CALOCALIBRATION_EGAMMAGAPCALIBRATION

#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

// Specific for this calibration
#include "CaloUtils/ToolWithConstants.h"
#include "CaloConditions/Array.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODTrigCalo/TrigEMCluster.h"

// Just for the pointer definition
//class TrigEMCluster;

/** General Interface for calibrations at the LVL2 Egamma Calo Fex algo */
class EgammaGapCalibration :
  public extends<CaloUtils::ToolWithConstants<AthAlgTool>,
                 IEgammaCalibration>          
{
 public:

  /// Inherit constructor.
  using base_class::base_class;
  
  /** Initialization of the tool */
  virtual StatusCode initialize() override;
  
  /** Finalization of the tool */
  virtual StatusCode finalize() override;
  
  /** method to perform the correction. The correction
      type is defined by the tool which also uses
      this interface. In some cases, the tool needs
      more than the cluster to perform the calibration.
      This can be passed via the void pointer */
  virtual void makeCorrection(xAOD::TrigEMCluster*, const void* v=nullptr) override;
  
 private:
  MsgStream* m_log;
  // Correction Variables
  Constant<CxxUtils::Array<2> >  m_correction      { this, "correction" };
  Constant<float>                m_eta_start_crack { this, "eta_start_crack" };
  Constant<float>                m_eta_end_crack   { this, "eta_end_crack" };
  Constant<int>                  m_degree          { this, "degree" };
  Constant<bool>                 m_use_raw_eta     { this, "use_raw_eta" };
  BooleanProperty                m_interpolate     { this, "UseInterpolation", true };
};

#endif
