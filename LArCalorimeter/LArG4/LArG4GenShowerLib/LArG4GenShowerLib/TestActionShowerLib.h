/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestActionShowerLib_H
#define TestActionShowerLib_H

// STL includes
#include <string>
#include <vector>


// forward declarations in namespaces
namespace ShowerLib {
  class StepInfoCollection;
}
namespace HepMC {
  class GenParticle;
}
// forward declarations in global namespace
//class StoreGateSvc;
class EnergyCalculator;
class G4VSolid;
class G4AffineTransform;

  /**
   *
   *   @short Class for collecting G4 hit information
   *
   *          Collect and store Geant4 hit information, i.e.
   *          position, deposited energy and time, from hits
   *          for the creation of a shower library
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version \$Id: TestActionShowerLib.h 780759 2016-10-27 13:48:04Z pavol $
   *
   */


#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"

#include "LArG4Code/ILArCalculatorSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
namespace G4UA{
  
  
  class TestActionShowerLib:
  public G4UserEventAction, public G4UserRunAction, public G4UserSteppingAction
  {
    
  public:
    TestActionShowerLib();
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;
    
    /* data members */
    
    ServiceHandle<ILArCalculatorSvc> m_current_calculator;
    G4VSolid* m_current_solid;
    G4AffineTransform* m_current_transform;
    
    // calculators 
    ServiceHandle<ILArCalculatorSvc> m_calculator_EMECIW;            //!< pointer to EMEC inner wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_calculator_EMECOW;            //!< pointer to EMEC outer wheel calculator
    ServiceHandle<ILArCalculatorSvc> m_calculator_FCAL1;
    ServiceHandle<ILArCalculatorSvc> m_calculator_FCAL2;
    ServiceHandle<ILArCalculatorSvc> m_calculator_FCAL3;
    ServiceHandle<ILArCalculatorSvc> m_calculator_EMB;
    
    ShowerLib::StepInfoCollection* m_eventSteps;    //!< collection of StepInfo

}; // class TestActionShowerLib


} // namespace G4UA 


#endif // TestActionShowerLib_H
