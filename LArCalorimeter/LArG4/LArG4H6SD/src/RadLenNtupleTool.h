/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_RADLENNTUPLETOOL_H
#define LARG4H6SD_RADLENNTUPLETOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "RadLenNtuple.h"

namespace G4UA
{
  /// @class RadLenNtupleTool
  /// @brief Tool which manages the RadLenNtuple action.
  ///
  /// Create the RadLenNtuple for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class RadLenNtupleTool : public ActionToolBase<RadLenNtuple>,
                           public IBeginEventActionTool,
                           public IEndEventActionTool,
                           public ISteppingActionTool
  {

  public:

    /// Standard constructor
    RadLenNtupleTool(const std::string& type, const std::string& name,
                                   const IInterface* parent);

    /// Framework methods
    StatusCode initialize() override final;

    /// Retrieve the begin-event action interface
    virtual IBeginEventAction* getBeginEventAction() override final
    { return static_cast<IBeginEventAction*>( getAction() ); }

    /// Retrieve the end-event action interface
    virtual IEndEventAction* getEndEventAction() override final
    { return static_cast<IEndEventAction*>( getAction() ); }

    /// Retrieve the stepping action interface
    virtual ISteppingAction* getSteppingAction() override final
    { return static_cast<ISteppingAction*>( getAction() ); }

    /// Query interface for gaudi
    virtual StatusCode
    queryInterface(const InterfaceID& riid, void** ppvInterface) override;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<RadLenNtuple> makeAction() override final;

  private:
    /// Configuration parameters
    RadLenNtuple::Config m_config;

  }; // class RadLenNtupleTool

} // namespace G4UA

#endif
