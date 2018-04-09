/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4UA__G4COSMICANDFILTERTOOL_H
#define G4COSMICFILTER_G4UA__G4COSMICANDFILTERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "G4CosmicFilter/G4CosmicAndFilter.h"

namespace G4UA
{

  class G4CosmicAndFilterTool : public UserActionToolBase<G4CosmicAndFilter>
  {

    public:

      /// Standard constructor
      G4CosmicAndFilterTool(const std::string& type, const std::string& name,
                            const IInterface* parent);

      virtual StatusCode finalize() override;

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<G4CosmicAndFilter>
      makeAndFillAction(G4AtlasUserActions&) override final;

    private:

      G4CosmicAndFilter::Config m_config;

  }; // class G4CosmicAndFilterTool

} // namespace G4UA

#endif
