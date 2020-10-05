/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_PHYSICSLISTSVC_H
#define G4ATLASTOOLS_PHYSICSLISTSVC_H

// Base classes
#include "G4AtlasInterfaces/IPhysicsListSvc.h"
#include "AthenaBaseComps/AthService.h"


#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "GaudiKernel/ToolHandle.h"

// Classes that are returned
class G4VModularPhysicsList;
class G4VUserPhysicsList;

// STL library
#include <string>

class PhysicsListSvc : public extends<AthService, IPhysicsListSvc>
{
public:
  // Standard constructor and destructor
  PhysicsListSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PhysicsListSvc() = default;


  virtual StatusCode initialize( ) override;
  // Base class methods from IPhysicsListSvc
  virtual void CreatePhysicsList() override;
  virtual G4VUserPhysicsList* GetPhysicsList() override;
  virtual void SetPhysicsList() override;
  virtual void SetPhysicsOptions() override;

private:

  /// This command prints a message about a G4Command depending on its returnCode
  void CommandLog(int returnCode, const std::string& commandString) const;

  ToolHandleArray<IPhysicsOptionTool> m_phys_option;
  ToolHandleArray<IPhysicsOptionTool> m_phys_decay;
  G4VModularPhysicsList* m_physicsList{}; ///!< Handle on the physics list
  Gaudi::Property<std::string> m_physicsListName{this, "PhysicsList", "FTFP_BERT", "Name for physics list"};     ///!< Name for the physics list (property to be set in the tool)
  Gaudi::Property<double> m_neutronTimeCut{this, "NeutronTimeCut", 0, "Time cut for neutron killer"};           ///!< Time cut for neutrons (in the neutron killer process)
  Gaudi::Property<double> m_neutronEnergyCut{this, "NeutronEnergyCut", 0, "Energy cut for neutron killer"};         ///!< Energy cut for neutrons (in the neutron killer process)
  Gaudi::Property<double> m_generalCut{this, "GeneralCut", 0, "General cut"};               ///!< A general cut - this isn't normally used in our simulation
  Gaudi::Property<double> m_emMaxEnergy{this, "EMMaxEnergy", -1., "Maximum energy for EM tables"};              ///!< Maximum energy of the pre-calculated EM cross-section tables
  Gaudi::Property<double> m_emMinEnergy{this, "EMMinEnergy", -1., "Minimum energy for EM tables"};              ///!< Minimum energy of the pre-calculated EM cross-section tables
  /* --- ATLASSIM-3967 ---
     Old methods SetDEDXBinning and SetLambdaBinning are replaced in G4 10.4 by
     SetNumberOfBinsPerDecade which now controls both settings with the same
     value. Instead of changing the overall number of bins, user should
     specify the number of bins per energy decade instead.
  */
  Gaudi::Property<int> m_emNumberOfBinsPerDecade{this, "EMNumberOfBinsPerDecade", -1, "Number of bins per Energy decade. Used for both DeDx and for the Lambda binning."};     ///!< Number of bins per Energy decade. Used for both DeDx and for the Lambda binning.
  Gaudi::Property<bool> m_applyEMCuts{this, "ApplyEMCuts", true, "Apply cuts EM flag in Geant4"};                ///!< Switch for the G4 "apply cuts" EM physics flag

};

#endif
