/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4UA__G4COSMICORFILTERTOOL_H
#define G4COSMICFILTER_G4UA__G4COSMICORFILTERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "G4CosmicFilter/G4CosmicOrFilter.h"

namespace G4UA
{

  class G4CosmicOrFilterTool : public UserActionToolBase<G4CosmicOrFilter>
  {

    public:

      /// Standard constructor
      G4CosmicOrFilterTool(const std::string& type, const std::string& name,
                           const IInterface* parent);

      virtual StatusCode finalize() override;

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<G4CosmicOrFilter>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      G4CosmicOrFilter::Config m_config;

  }; // class G4CosmicOrFilterTool

} // namespace G4UA

#endif
