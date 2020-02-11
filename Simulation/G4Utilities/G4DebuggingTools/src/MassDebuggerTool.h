/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__MASSDEBUGGERTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__MASSDEBUGGERTOOL_H
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "./MassDebugger.h"

namespace G4UA {

/// @class MassDebuggerTool
/// @brief Tool which manages the MassDebugger action.
///
/// Create the MassDebugger  for each worker thread
///
/// @author Nigel Hessey
///

class MassDebuggerTool:
    public ActionToolBase<MassDebugger>,
    public IG4RunActionTool {
public:
    /// standard tool constructor
    MassDebuggerTool(const std::string &type, const std::string &name, const IInterface *parent);
    /// returns the BoR action
    virtual G4UserRunAction *getRunAction() override final {
        return static_cast<G4UserRunAction *>(getAction());
    }

    virtual StatusCode queryInterface(const InterfaceID &riid, void * *ppvInterface) override;
protected:
    /// builds the action for a thread
    virtual std::unique_ptr<MassDebugger> makeAction() override final;
private:
    /// holds the configuration received from the python side
    MassDebugger::Config m_config;
};

} // namespace G4UA
#endif
