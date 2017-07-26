/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasAlg_AthenaTrackingActionTool_H
#define G4AtlasAlg_AthenaTrackingActionTool_H

// Infrastructure includes
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "AthenaTrackingAction.h"

namespace G4UA
{

  /// @class AthenaTrackingActionTool
  /// @brief Tool which manages the AthenaTrackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaTrackingActionTool : public ActionToolBase<AthenaTrackingAction>,
                                   public IPreTrackingActionTool,
                                   public IPostTrackingActionTool
  {

    public:

      /// Standard constructor
      AthenaTrackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool (just for debugging printout)
      virtual StatusCode initialize() override;

      /// Retrieve the tracking action
      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }

      /// Retrieve the tracking action
      virtual IPostTrackingAction* getPostTrackingAction() override final
      { return static_cast<IPostTrackingAction*>( getAction() ); }

    protected:

      /// Create an action for this thread
      virtual std::unique_ptr<AthenaTrackingAction> makeAction() override final;

    private:

      /// The saving level for secondaries.
      int m_secondarySavingLevel;

  }; // class AthenaTrackingActionTool

} // namespace G4UA

#endif
