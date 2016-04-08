/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TestActionShowerLib_H
#define TestActionShowerLib_H

// STL includes
#include <string>
#include <vector>

// athena simulation includes

#include "G4AtlasTools/UserActionBase.h"

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
   * @version \$Id: TestActionShowerLib.h 664021 2015-04-30 10:17:27Z disimone $
   *
   */

class TestActionShowerLib final: public UserActionBase {

 public:

  //! default constructor
  TestActionShowerLib(const std::string& type, const std::string& name, const IInterface* parent);

  ~TestActionShowerLib();

  //! run code at begin of event
  void BeginOfEvent(const G4Event*) override;
  //! run code at end of event
  void EndOfEvent(const G4Event*) override;
  //! run code at begin of run
  void BeginOfRun(const G4Run*) override;
  //! run code at end of event
  void EndOfRun(const G4Run*) override;
  //! run code after each step
  void Step(const G4Step*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:

  /* data members */

  LArVCalculator* m_current_calculator;
  G4VSolid* m_current_solid;
  G4AffineTransform* m_current_transform;

  // calculators 
  LArVCalculator* m_calculator_EMECIW;            //!< pointer to EMEC inner wheel calculator
  LArVCalculator* m_calculator_EMECOW;            //!< pointer to EMEC outer wheel calculator
  

  ShowerLib::StepInfoCollection* m_eventSteps;    //!< collection of StepInfo

};

#endif // TestActionShowerLib_H
