/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FastCaloSimParamAction_H
#define ISF_FastCaloSimParamAction_H

// STL includes
#include <string>
#include <vector>

// athena simulation includes
#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ServiceHandle.h"

// forward declarations in namespaces
namespace ISF_FCS_Parametrization {
  class FCS_StepInfoCollection;
}
namespace HepMC {
  class GenParticle;
}
// forward declarations in global namespace
class LArVCalculator;
class G4VSolid;
class G4AffineTransform;
class LArG4SimpleSD;
class LArIdentifierConverter;
class TileGeoG4SDCalc;
class CaloDetDescrManager;
class LArEM_ID;
  /**
   *
   *   @short Class for collecting G4 hit information
   *
   *          Collect and store Geant4 hit information, i.e.
   *          position, deposited energy and time, from hits
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *  @author Zdenek Hubacek, CERN
   *  @author Geoffrey Gilles, Clermont-Ferrand, France
   *
   *
   */

class FastCaloSimParamAction: public UserActionBase {

 public:

  //! default constructor
  FastCaloSimParamAction(const std::string& type, const std::string& name, const IInterface* parent);

  ~FastCaloSimParamAction();

  //! run code at begin of event
  virtual void BeginOfEvent(const G4Event*) override;
  //! run code at end of event
  virtual void EndOfEvent(const G4Event*) override;
  //! run code at begin of run
  virtual void BeginOfRun(const G4Run*) override;
  //! run code at end of event
  virtual void EndOfRun(const G4Run*) override;
  //! run code after each step
  virtual void Step(const G4Step*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

 private:

  /* data members */

  LArVCalculator* m_current_calculator;
  TileGeoG4SDCalc* m_current_calculator_Tile;

  G4VSolid* m_current_solid;
  G4AffineTransform* m_current_transform;

  // calculators
  LArVCalculator* m_calculator_EMECIW_pos;            //!< pointer to EMEC inner wheel calculator
  LArVCalculator* m_calculator_EMECIW_neg;            //!< pointer to EMEC inner wheel calculator

  LArVCalculator* m_calculator_EMECOW_pos;            //!< pointer to EMEC outer wheel calculator
  LArVCalculator* m_calculator_EMECOW_neg;

  //LArVCalculator* m_calculator_BIB; //!< BackInnerBarrette
  LArVCalculator* m_calculator_BOB; //!< BackOuterBarrette

  LArVCalculator* m_calculator_EMB;
  LArVCalculator* m_calculator_FCAL1;
  LArVCalculator* m_calculator_FCAL2;
  LArVCalculator* m_calculator_FCAL3;
  LArVCalculator* m_calculator_HEC;
  LArVCalculator* m_calculator_EMBPS;             //!< calculators for presampler
  LArVCalculator* m_calculator_EMEPS;

  //LArVCalculator* m_calculator_HECLocal;
  TileGeoG4SDCalc* m_calculator_Tile;

  // helper
  LArG4SimpleSD* m_lar_helper;
  const LArEM_ID *m_lar_emID;
  const CaloDetDescrManager* m_calo_dd_man;

  ISF_FCS_Parametrization::FCS_StepInfoCollection* m_eventSteps;    //!< collection of StepInfo
  std::map<std::string,int> m_detectormap;
  std::set<std::string> m_unuseddetector;
  int m_ndetectors;
  bool m_shift_lar_subhit;
  bool m_shorten_lar_step;
};

#endif
