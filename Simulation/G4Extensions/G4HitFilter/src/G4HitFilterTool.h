/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_G4UA__G4HITFILTERTOOL_H
#define G4HITFILTER_G4UA__G4HITFILTERTOOL_H
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4HitFilter.h"

namespace G4UA{

class G4HitFilterTool:
  public ActionToolBaseReport<G4HitFilter>,
    public IG4EventActionTool,
    public IG4RunActionTool
    {

    public:
      G4HitFilterTool(const std::string& type, const std::string& name,const IInterface* parent);

      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }

      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
      virtual StatusCode finalize() override;
    protected:
      virtual std::unique_ptr<G4HitFilter> makeAction() override final;
    private:
      G4HitFilter::Config m_config;
      //G4HitFilter::Report m_report;
    }; // class G4HitFilterTool


} // namespace G4UA
#endif
