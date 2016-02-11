/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_G4UA__G4HITFILTERTOOL_H
#define G4HITFILTER_G4UA__G4HITFILTERTOOL_H
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4HitFilter.h"

namespace G4UA{

class G4HitFilterTool:
  public ActionToolBaseReport<G4HitFilter>,
    public IEndEventActionTool,
    public IBeginRunActionTool
    {

    public:
      G4HitFilterTool(const std::string& type, const std::string& name,const IInterface* parent);

      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

      virtual IBeginRunAction* getBeginRunAction() override final
      { return static_cast<IBeginRunAction*>( getAction() ); }

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
