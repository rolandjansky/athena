/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4UA__G4COSMICFILTERTOOL_H
#define G4COSMICFILTER_G4UA__G4COSMICFILTERTOOL_H

#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4CosmicFilter/G4CosmicFilter.h"

namespace G4UA
{

  class G4CosmicFilterTool: public ActionToolBaseReport<G4CosmicFilter>,
                            public IG4EventActionTool
  {

    public:

      G4CosmicFilterTool(const std::string& type, const std::string& name,const IInterface* parent);

      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
      virtual StatusCode finalize() override;

    protected:

      virtual std::unique_ptr<G4CosmicFilter> makeAction() override final;

    private:

      G4CosmicFilter::Config m_config;

  }; // class G4CosmicFilterTool

} // namespace G4UA

#endif
