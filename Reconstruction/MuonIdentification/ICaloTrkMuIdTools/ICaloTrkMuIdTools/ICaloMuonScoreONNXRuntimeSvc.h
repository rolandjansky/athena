#ifndef CALOTRKMUIDTOOLS_ICALOMUONSCOREONNXRUNTIMESVC_H
#define CALOTRKMUIDTOOLS_ICALOMUONSCOREONNXRUNTIMESVC_H

// Gaudi include(s).
#include "GaudiKernel/IService.h"

// ONNX include(s).
#include <core/session/onnxruntime_cxx_api.h>

/// Namespace holding all of the ONNX Runtime example code
namespace AthONNX {

  /// Service used for managing global objects used by ONNX Runtime
  ///
  /// In order to allow multiple clients to use ONNX Runtime at the same
  /// time, this service is used to manage the objects that must only
  /// be created once in the Athena process.
  ///
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  ///
  class ICaloMuonScoreONNXRuntimeSvc : public virtual IService {

  public:
    /// Virtual destructor, to make vtable happy
    virtual ~ICaloMuonScoreONNXRuntimeSvc() = default;

    /// Declare an ID for this interface
    DeclareInterfaceID( AthONNX::ICaloMuonScoreONNXRuntimeSvc, 1, 0 );

    /// Return the ONNX Runtime environment object
    virtual Ort::Env& env() const = 0;

  }; // class ICaloMuonScoreONNXRuntimeSvc

} // namespace AthONNX

#endif // CALOTRKMUIDTOOLS_ICALOMUONSCOREONNXRUNTIMESVC_H
