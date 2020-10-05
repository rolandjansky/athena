// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef CALOTRKMUIDTOOLS_CALOMUONSCOREONNXRUNTIMESVC_H
#define CALOTRKMUIDTOOLS_CALOMUONSCOREONNXRUNTIMESVC_H

// Local include(s).
#include "ICaloTrkMuIdTools/ICaloMuonScoreONNXRuntimeSvc.h"

// Framework include(s).
#include "AthenaBaseComps/AthService.h"

// ONNX include(s).
#include <core/session/onnxruntime_cxx_api.h>

// System include(s).
#include <memory>

/// Service implementing @c ICaloMuonScoreONNXRuntimeSvc
///
/// This is a very simple implementation, just managing the lifetime
/// of some ONNX Runtime C++ objects.
///
/// Ported from  Control/AthenaExamples/AthExOnnxRuntime (Ricardo Woelker <ricardo.woelker@cern.ch>)
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
class CaloMuonScoreONNXRuntimeSvc : public extends< AthService, ICaloMuonScoreONNXRuntimeSvc > {
  
 public:
  /// Inherit the base class's constructor
  using extends::extends;
  
  /// @name Function(s) inherited from @c Service
  /// @{
  
  /// Function initialising the service
  virtual StatusCode initialize() override;
  /// Function finalising the service
  virtual StatusCode finalize() override;
  
  /// @}
  
  /// @name Function(s) inherited from @c ICaloMuonScoreONNXRuntimeSvc
  /// @{
  
  /// Return the ONNX Runtime environment object
  virtual Ort::Env& env() const override;
  
  /// @}
  
 private:
  /// Global runtime environment for ONNX Runtime
  std::unique_ptr< Ort::Env > m_env;
  
}; // class CaloMuonScoreONNXRuntimeSvc

#endif // CALOTRKMUIDTOOLS_CALOMUONSCOREONNXRUNTIMESVC_H
