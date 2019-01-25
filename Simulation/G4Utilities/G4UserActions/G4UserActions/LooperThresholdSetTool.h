/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef G4USERACTIONS_G4UA_LooperThresholdSetTOOL_H
#define G4USERACTIONS_G4UA_LooperThresholdSetTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "G4UserActions/LooperThresholdSet.h"

namespace G4UA
{

/// @class LooperThresholdSetTool
/// @brief Tool which manages the Looper Threshold options.
///
/// Create the LooperThresholdSet for each worker thread
///
/// @author Miha Muskinja
///
class LooperThresholdSetTool : public UserActionToolBase<LooperThresholdSet>
{

public:

/// Standard constructor
LooperThresholdSetTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

virtual StatusCode initialize() override;
virtual StatusCode finalize() override;

protected:

/// Create action for this thread
virtual std::unique_ptr<LooperThresholdSet>
makeAndFillAction(G4AtlasUserActions&) override final;

private:

/// Configuration parameters
LooperThresholdSet::Config m_config;

};   // class LooperThresholdSetTool

} // namespace G4UA

#endif
