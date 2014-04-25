/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TestActionShowerLib_H
#define TestActionShowerLib_H

// STL includes
#include <string>
#include <vector>

// athena simulation includes
#include "FadsActions/UserAction.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// forward declarations in namespaces
namespace ShowerLib {
  class StepInfoCollection;
}
namespace HepMC {
  class GenParticle;
}
// forward declarations in global namespace
//class StoreGateSvc;
class LArVCalculator;
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
   * @version \$Id: TestActionShowerLib.h 498105 2012-04-27 11:38:03Z gsedov $
   *
   */

class TestActionShowerLib: public FADS::UserAction {

 public:

  //! default constructor
  TestActionShowerLib(std::string s);

  ~TestActionShowerLib();

  //! run code at begin of event
  void BeginOfEventAction(const G4Event*);
  //! run code at end of event
  void EndOfEventAction(const G4Event*);
  //! run code at begin of run
  void BeginOfRunAction(const G4Run*);
  //! run code at end of event
  void EndOfRunAction(const G4Run*);
  //! run code after each step
  void SteppingAction(const G4Step*);


 private:

  /* data members */

  LArVCalculator* m_current_calculator;
  G4VSolid* m_current_solid;
  G4AffineTransform* m_current_transform;

  // calculators 
  LArVCalculator* m_calculator_EMECIW;            //!< pointer to EMEC inner wheel calculator
  LArVCalculator* m_calculator_EMECOW;            //!< pointer to EMEC outer wheel calculator
  
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_storeGateSvc;                  //!< pointer to StoreGate service
  //! return pointer to StoreGate service
  StoreGateSvc_t storeGateSvc() const;

  ShowerLib::StepInfoCollection* m_eventSteps;    //!< collection of StepInfo

};

#endif // TestActionShowerLib_H
