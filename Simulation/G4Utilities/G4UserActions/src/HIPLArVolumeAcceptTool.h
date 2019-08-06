/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H
#define G4USERACTIONS_G4UA__HIPLARVOLUMEACCEPTTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "HIPLArVolumeAccept.h"

namespace G4UA
{

  /// Tool which manages the HIPLArVolumeAccept action
  ///
  class HIPLArVolumeAcceptTool : public UserActionToolBase<HIPLArVolumeAccept>
  {

    public:

      /// Standard constructor
      HIPLArVolumeAcceptTool(const std::string& type, const std::string& name,
                             const IInterface* parent);

      /// Finalize the tool
      virtual StatusCode finalize() override;

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<HIPLArVolumeAccept>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class HIPLArVolumeAcceptTool

} // namespace G4UA

#endif
