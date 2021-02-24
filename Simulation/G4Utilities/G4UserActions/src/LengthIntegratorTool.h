/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H
#define G4USERACTIONS_G4UA_LENGTHINTEGRATOR_H

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "LengthIntegrator.h"

namespace G4UA
{

  /// @class LengthIntegratorTool
  /// @brief Tool which manages the LengthIntegrator action.
  ///
  /// Creates the LengthIntegrator for each worker thread.
  ///
  class LengthIntegratorTool : public UserActionToolBase<LengthIntegrator>
  {

    public:

      /// Standard constructor
      LengthIntegratorTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override;

    protected:

      /// Create aciton for this thread
      virtual std::unique_ptr<LengthIntegrator>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      /// Handle to the histogram service
      ServiceHandle<ITHistSvc> m_hSvc;

      //do we fill histograms as well as TTree?
      //(very large filesize...)
      bool m_doHistos;

  }; // class LengthIntegratorTool

} // namespace G4UA

#endif
