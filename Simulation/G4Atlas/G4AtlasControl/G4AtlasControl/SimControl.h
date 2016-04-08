/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimControl_H
#define SimControl_H

#include <string>
#include "G4AtlasControl/PhysicsMenu.h"
#include "G4AtlasControl/MCTruthMenu.h"
#include "G4AtlasControl/FieldMenu.h"
#include "G4AtlasControl/FieldIntegrationMenu.h"

class DataCardSvc;

class SimControl {
public:
  SimControl();
  virtual ~SimControl();
  void G4Command(const std::string&) const;
  void UIsession() const;
  void load(const std::string&) const;
  void ReadXML(const std::string&) const;
  const PhysicsMenu& physMenu() const ;
  const MCTruthMenu& mcMenu() const ;
  const FieldMenu& fieldMenu() const;
  const FieldIntegrationMenu& fieldIntegrationMenu() const;
  void initializeG4() const;
  void initializeGraphics() const;

  void DefineCard(const std::string name, const std::string type, int defValue);
  void DefineCard(const std::string name, const std::string type, double defValue);
  void DefineCard(const std::string name, const std::string type, const std::string defValue);

  void SetCard(const std::string name, int Value);
  void SetCard(const std::string name, double Value);
  void SetCard(const std::string name, const std::string Value);

  PhysicsMenu          physicsMenu;
  MCTruthMenu          mctruthMenu;
  FieldMenu            fldMenu;
  FieldIntegrationMenu fldIntMenu;

private:
  DataCardSvc          *dCard;
};

#endif
