/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// defining this class is just used for testing since class ILArCalculatorSvc is a abstract class which needs to be instatiated in my test code
// actually the class(i.e. IService) that LArCalibCalculatorSvc inherits is also abstract, which means I also need to override the pure virtual functions in class IService
// To make matters more complicated, the classes(i.e. INamedInterface and IStateful) that IService inherits are also abstract, so I need to override the pure virtual functions in classes INamedInterface and IStateful
// In addition, since INamedInterface and IStateful all inherit the class IInterface, which is abstract. Therefore, I also override the pure virtual functions in the class IInterface.

#ifndef __DERIVEDILARCALCULATORSVC_H__
#define __DERIVEDILARCALCULATORSVC_H__

#include "LArG4Code/ILArCalculatorSvc.h"
#include "GaudiKernel/StateMachine.h"

class DerivedILArCalculatorSvcForTest : public ILArCalculatorSvc {

public:

  DerivedILArCalculatorSvcForTest() {};

  ~DerivedILArCalculatorSvcForTest() {};

// there are no concrete functions for the following member functions, just to override the pure virtual member function in the base classes
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

  std::string& name() const
  {
    std::string test1 = "test";
    return test1;
  }

  G4float OOTcut() const
  {
    return 1.0;
  }

  G4bool isInTime(G4double hitTime) const
  {
    (void)hitTime;
    return true;
  }

  G4bool Process (const G4Step* a_step, std::vector<LArHitData>& hits) const
  {
//firstly set the G4Step object a_step to make it complete, you can set it here as much as you want. Since there is no need to set too much for it, I don't set sth for it
    (void)a_step;

//insert LArHitData object into hits
    LArG4Identifier a_ident;//define a LArG4Identifier object called a_ident
    a_ident.add(4);
    a_ident.add(2);
    a_ident.add(2);
    a_ident.add(3);
    a_ident.add(4);
    a_ident.add(5);
    a_ident.add(6);
    LArHitData hit = {};//define LArHitData object hit
    hit.id = a_ident;//set hit, the same below
    hit.time = 1.0;
    hit.energy = 1.0;
    hits.push_back(hit);//insert hit into the container hits

    return true;
  }

};
#endif

