/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4USERACTIONS_G4UA__FASTCALOSIMPARAMACTIONTOOL_H
#define ISF_GEANT4USERACTIONS_G4UA__FASTCALOSIMPARAMACTIONTOOL_H
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"

#include "G4AtlasTools/ActionToolBase.h"

#include "ISF_Geant4UserActions/FastCaloSimParamAction.h"

class ILArCalculatorSvc;
class ITileCalculator;

namespace G4UA{

  /// @class FastCaloSimParamActionTool
  /// @brief Tool which manages the FastCaloSimParamAction
  ///
  /// @author Andrea Di Simone
  ///

  class FastCaloSimParamActionTool:
  public ActionToolBase<FastCaloSimParamAction>,
    public IG4RunActionTool, public IG4EventActionTool, public IG4SteppingActionTool
  {

  public:
    /// Standard constructor
    FastCaloSimParamActionTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Intialize Athena components
    StatusCode initialize() override final;
    /// Retrieve the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// Retrieve the event action
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// Retrieve the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<FastCaloSimParamAction> makeAction() override final;
  private:
    /// Configuration parameters
    FastCaloSimParamAction::Config m_config;

    ServiceHandle<ILArCalculatorSvc> m_emepiwcalc;  //!< EMEC positive inner wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_emeniwcalc;  //!< EMEC negative inner wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_emepowcalc;  //!< EMEC positive outer wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_emenowcalc;  //!< EMEC negative outer wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_emeobarcalc; //!< EMEC Back Outer Barrette wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_embcalc;     //!< Barrel calculator
    ServiceHandle<ILArCalculatorSvc> m_fcal1calc;   //!< FCAL1 calculator
    ServiceHandle<ILArCalculatorSvc> m_fcal2calc;   //!< FCAL2 calculator
    ServiceHandle<ILArCalculatorSvc> m_fcal3calc;   //!< FCAL3 calculator
    ServiceHandle<ILArCalculatorSvc> m_heccalc;     //!< HEC wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_pscalc;      //!< Barrel presampler calculator
    ServiceHandle<ILArCalculatorSvc> m_emepscalc;   //!< Endcap presampler calculator
    ServiceHandle<ITileCalculator> m_tileCalculator; //!< Tile calculator

  }; // class FastCaloSimParamActionTool


} // namespace G4UA
#endif
