/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT2CALOCALIBRATION_EGAMMAGAPCALIBRATION
#define TRIGT2CALOCALIBRATION_EGAMMAGAPCALIBRATION

#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

// Specific for this calibration
#include "CaloRec/ToolWithConstantsMixin.h"
#include "CaloConditions/Array.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODTrigCalo/TrigEMCluster.h"

// Just for the pointer definition
//class TrigEMCluster;

/** General Interface for calibrations at the LVL2 Egamma Calo Fex algo */
class EgammaGapCalibration : virtual public IEgammaCalibration,
			     virtual public AthAlgTool, 
			     public CaloRec::ToolWithConstantsMixin 
{
 public:
  
  /** Constructor */
  EgammaGapCalibration(const std::string & type, 
		       const std::string & name,
		       const IInterface* parent) 
    : AthAlgTool(type,name,parent), CaloRec::ToolWithConstantsMixin() {
    declareInterface<IEgammaCalibration>(this);
    declareConstant("correction",      m_correction);
    declareConstant("eta_start_crack", m_eta_start_crack);
    declareConstant("eta_end_crack",   m_eta_end_crack);
    declareConstant("degree",          m_degree);
    declareConstant("use_raw_eta",     m_use_raw_eta);
    declareProperty("UseInterpolation",m_interpolate = true);
    finish_ctor();
  }
  
  /** Destructor */
  virtual ~EgammaGapCalibration() { }
  
  /** Initialization of the tool */
  StatusCode initialize();
  
  /** Finalization of the tool */
  StatusCode finalize();
  
  /** Set Property necessary */
  using AthAlgTool::setProperty;
  StatusCode setProperty(const std::string&, const std::string&);
  StatusCode setProperty(const Property&);
  
  /** method to perform the correction. The correction
      type is defined by the tool which also uses
      this interface. In some cases, the tool needs
      more than the cluster to perform the calibration.
      This can be passed via the void pointer */
  virtual void makeCorrection(xAOD::TrigEMCluster*, const void* v=NULL);
  
 private:
  MsgStream* m_log;
  // Correction Variables
  CaloRec::Array<2>     m_correction;
  float m_eta_start_crack;
  float m_eta_end_crack;
  int m_degree;
  bool m_use_raw_eta;
  bool m_interpolate;  
};

#endif
