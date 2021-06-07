/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// defining this class is just used for testing since class ILArCalculatorSvc is a abstract class which needs to be instatiated in my test code
// actually the class(i.e. IService) that LArCalibCalculatorSvc inherits is also abstract, which means I also need to override the pure virtual functions in class IService
// To make matters more complicated, the classes(i.e. INamedInterface and IStateful) that IService inherits are also abstract, so I need to override the pure virtual functions in classes INamedInterface and IStateful
// In addition, since INamedInterface and IStateful all inherit the class IInterface, which is abstract. Therefore, I also override the pure virtual functions in the class IInterface.

#ifndef LARG4CODE_DERIVEDILARCALCULATORSVCFORTEST_H
#define LARG4CODE_DERIVEDILARCALCULATORSVCFORTEST_H

#include "LArG4Code/ILArCalculatorSvc.h"
#include "GaudiKernel/StateMachine.h"

class DerivedILArCalculatorSvcForTest : public ILArCalculatorSvc {

public:

  DerivedILArCalculatorSvcForTest() {};

  ~DerivedILArCalculatorSvcForTest() {};

// there are no concrete functions for the following member functions, just to override the pure virtual member function in the base classes
  virtual StatusCode sysInitialize()  override
  { 
    StatusCode s1; 
    return s1;
  }

  virtual StatusCode sysStart() override
  {
    StatusCode s2;
    return s2;
  }
  
  virtual StatusCode sysStop() override
  { 
    StatusCode s3; 
    return s3;
  }
  
  virtual StatusCode sysFinalize() override
  { 
    StatusCode s4; 
    return s4;
  }

  virtual StatusCode sysReinitialize() override
  { 
    StatusCode s5;
    return s5;
  }

  virtual StatusCode sysRestart() override
  { 
    StatusCode s6; 
    return s6;
  }

  virtual void setServiceManager( ISvcManager* ) override { return; }

  virtual unsigned long addRef() override { return 1; }

  virtual unsigned long release() override { return 1; }

  virtual 
  StatusCode queryInterface( const InterfaceID& ti, void** pp ) override
  { 
    (void)ti; //to silence the unused-parameter warning, the same below
    (void)pp;
    StatusCode s7; 
    return s7;
  }

  virtual StatusCode configure() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode initialize() override
  {
    StatusCode s7; 
    return s7;
  }

  virtual StatusCode start() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode stop() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode finalize() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode terminate() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode reinitialize() override
  {
    StatusCode s7;
    return s7;
  }

  virtual StatusCode restart() override
  {
    StatusCode s7;
    return s7;
  }

  void initializeForSDCreation() override {};

  virtual Gaudi::StateMachine::State FSMState() const override
  {
    return Gaudi::StateMachine::State(0);
  }

  virtual Gaudi::StateMachine::State targetFSMState() const override
  {
    return Gaudi::StateMachine::State(0);
  }

  virtual const std::string& name() const override
  {
    static const std::string Tes = "test";
    return Tes;
  }

  virtual G4float OOTcut() const override
  {
    return 1.0;
  }

  virtual G4bool isInTime(G4double hitTime) const override
  {
    (void)hitTime;
    return true;
  }

  virtual G4bool Process (const G4Step* a_step, std::vector<LArHitData>& hits) const override
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

