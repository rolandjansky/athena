/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_PHYSICSLISTTOOLBASE_H 
#define G4ATLASTOOLS_PHYSICSLISTTOOLBASE_H

// Base classes
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Classes that are returned
class G4VUserPhysicsList;

// STL library
#include <string>

class PhysicsListToolBase : virtual public IPhysicsListTool , public AthAlgTool {
 public:
  // Standard constructor and destructor
  PhysicsListToolBase(const std::string& type, const std::string& name, const IInterface *parent);
  ~PhysicsListToolBase() {}

  // Base class methods from IPhysicsListTool
  virtual void CreatePhysicsList() override;
  virtual G4VUserPhysicsList* GetPhysicsList() override {if (!m_physicsList) CreatePhysicsList(); return m_physicsList;}
  virtual void SetPhysicsList() override;

 private:
  G4VUserPhysicsList* m_physicsList; ///!< Handle on the physics list
  std::string m_physicsListName;     ///!< Name for the physics list (property to be set in the tool)
  double m_neutronTimeCut;           ///!< Time cut for neutrons (in the neutron killer process)
  double m_neutronEnergyCut;         ///!< Energy cut for neutrons (in the neutron killer process)
  double m_generalCut;               ///!< A general cut - this isn't normally used in our simulation
  double m_emMaxEnergy;              ///!< Maximum energy of the pre-calculated EM cross-section tables
  double m_emMinEnergy;              ///!< Minimum energy of the pre-calculated EM cross-section tables
  int m_emDEDXBinning;               ///!< Binning for the EM dE/dX tables
  int m_emLambdaBinning;             ///!< Binning for the EM lambda tables
  bool m_applyEMCuts;                ///!< Switch for the G4 "apply cuts" EM physics flag

 protected:
  void SetCuts();                    ///!< Method for setting all the cuts in the list
  void ConstructProcess();           ///!< Method for constructing the physics processes

};

#endif
