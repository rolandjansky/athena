/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// defining this class is just used for testing since class LArCalibCalculatorSvc is a abstract class which needs to be instatiated in my test code
// actually the class(i.e. IService) that LArCalibCalculatorSvc inherits is also abstract, which means I also need to override the pure virtual functions in class IService
// To make matters more complicated, the classes(i.e. INamedInterface and IStateful) that IService inherits are also abstract, so I need to override the pure virtual functions in classes INamedInterface and IStateful
// In addition, since INamedInterface and IStateful all inherit the class IInterface, which is abstract. Therefore, I also override the pure virtual functions in the class IInterface.

#ifndef LARG4CODE_DERIVEDILARCALIBCALCULATORSVCFORTEST_H
#define LARG4CODE_DERIVEDILARCALIBCALCULATORSVCFORTEST_H

#include "LArG4Code/ILArCalibCalculatorSvc.h"
#include "GaudiKernel/StateMachine.h"

class DerivedILArCalibCalculatorSvcForTest : public ILArCalibCalculatorSvc {

public:

  DerivedILArCalibCalculatorSvcForTest() {};

  ~DerivedILArCalibCalculatorSvcForTest() {};

// there are no concrete functions for the following member functions, 
  StatusCode sysInitialize() 
  { 
    StatusCode s1; 
    return s1;
  }

  StatusCode sysStart() 
  {
    StatusCode s2;
    return s2;
  }
  
  StatusCode sysStop() 
  { 
    StatusCode s3; 
    return s3;
  }
  
  StatusCode sysFinalize() 
  { 
    StatusCode s4; 
    return s4;
  }

  StatusCode sysReinitialize() 
  { 
    StatusCode s5;
    return s5;
  }

  StatusCode sysRestart() 
  { 
    StatusCode s6; 
    return s6;
  }

  void setServiceManager( ISvcManager* ) { return; }

  unsigned long addRef() { return 1; }

  unsigned long release() { return 1; }

  StatusCode queryInterface( const InterfaceID& ti, void** pp ) 
  { 
    (void)ti; //to silence the unused-parameter warning, the same below
    (void)pp;
    StatusCode s7; 
    return s7;
  }

  StatusCode configure() 
  {
    StatusCode s7;
    return s7;
  }

  StatusCode initialize()
  {
    StatusCode s7; 
    return s7;
  }

  StatusCode start()
  {
    StatusCode s7;
    return s7;
  }

  StatusCode stop()
  {
    StatusCode s7;
    return s7;
  }

  StatusCode finalize()
  {
    StatusCode s7;
    return s7;
  }

  StatusCode terminate()
  {
    StatusCode s7;
    return s7;
  }

  StatusCode reinitialize()
  {
    StatusCode s7;
    return s7;
  }

  StatusCode restart()
  {
    StatusCode s7;
    return s7;
  }

  Gaudi::StateMachine::State FSMState() const
  {
    return Gaudi::StateMachine::State(0);
  }

  Gaudi::StateMachine::State targetFSMState() const
  {
    return Gaudi::StateMachine::State(0);
  }

  const std::string& name() const
  {
    static const std::string Tes = "test";
    return Tes;
  }

  G4bool Process (const G4Step* step, LArG4Identifier & identifier,
                            std::vector<G4double> & energies,
                            const LArG4::eCalculatorProcessing process = LArG4::kEnergyAndID) const
  {
    (void)step; //just to silence the unused-parameter warning, the same below
//    (void)energies;
    (void)process;

    identifier.add(4); //add a set of numbers into the LArG4Identifier class, which can provide proper setting for test
    identifier.add(1);
    identifier.add(2);
    identifier.add(3);
    identifier.add(4);
    identifier.add(5);
    identifier.add(6);

    energies = {1., 2., 3., 4.};

    return true; //returning true aims to make "m_calculator->Process( a_step, identifier, vtmp, LArG4::kOnlyID)" return true in the tested code LArG4CalibSD.cc
  }
     
};
#endif
