/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4UA__G4COSMICFILTERTOOL_H
#define G4COSMICFILTER_G4UA__G4COSMICFILTERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "G4CosmicFilter/G4CosmicFilter.h"

namespace G4UA
{

  class G4CosmicFilterTool : public UserActionToolBase<G4CosmicFilter>
  {

    public:

      /// Standard constructor
      G4CosmicFilterTool(const std::string& type, const std::string& name,
                         const IInterface* parent);

      virtual StatusCode finalize() override;

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<G4CosmicFilter>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      G4CosmicFilter::Config m_config;

  }; // class G4CosmicFilterTool

} // namespace G4UA

#endif
