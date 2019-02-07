/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_PHYSICSLISTTOOLBASE_H
#define G4ATLASTOOLS_PHYSICSLISTTOOLBASE_H

// Base classes
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "AthenaBaseComps/AthAlgTool.h"


#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "GaudiKernel/ToolHandle.h"

// Classes that are returned
class G4VModularPhysicsList;
class G4VUserPhysicsList;

// STL library
#include <string>

class PhysicsListToolBase : public extends<AthAlgTool, IPhysicsListTool>
{
public:
  // Standard constructor and destructor
  PhysicsListToolBase(const std::string& type, const std::string& name, const IInterface *parent);
  ~PhysicsListToolBase() {}


  virtual StatusCode initialize( ) override;
  // Base class methods from IPhysicsListTool
  virtual void CreatePhysicsList() override;
  virtual G4VUserPhysicsList* GetPhysicsList() override;
  virtual void SetPhysicsList() override;
  virtual void SetPhysicsOptions() override;

private:

  /// This command prints a message about a G4Command depending on its returnCode
  void CommandLog(int returnCode, const std::string& commandString) const;

  ToolHandleArray<IPhysicsOptionTool> m_phys_option;
  ToolHandleArray<IPhysicsOptionTool> m_phys_decay;
  G4VModularPhysicsList* m_physicsList; ///!< Handle on the physics list
  std::string m_physicsListName;     ///!< Name for the physics list (property to be set in the tool)
  double m_neutronTimeCut;           ///!< Time cut for neutrons (in the neutron killer process)
  double m_neutronEnergyCut;         ///!< Energy cut for neutrons (in the neutron killer process)
  double m_generalCut;               ///!< A general cut - this isn't normally used in our simulation
  double m_emMaxEnergy;              ///!< Maximum energy of the pre-calculated EM cross-section tables
  double m_emMinEnergy;              ///!< Minimum energy of the pre-calculated EM cross-section tables
  int m_emNumberOfBinsPerDecade;     ///!< Number of bins per Energy decade. Used for both DeDx and for the Lambda binning.
  bool m_applyEMCuts;                ///!< Switch for the G4 "apply cuts" EM physics flag

};

#endif
