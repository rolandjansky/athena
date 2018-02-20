/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FastCaloSimParamAction_H
#define ISF_FastCaloSimParamAction_H

// STL includes
#include <string>
#include <vector>

// athena simulation includes

#include "GaudiKernel/ServiceHandle.h"
#include "LArG4Code/ILArCalculatorSvc.h"
#include "StoreGate/WriteHandle.h"
#include "TileG4Interfaces/ITileCalculator.h"

// CLHEP include for Hep3Vector
#include "CLHEP/Vector/ThreeVector.h"
#include "Identifier/Identifier.h"


// forward declarations in namespaces
namespace ISF_FCS_Parametrization {
  class FCS_StepInfoCollection;
  class FCS_StepInfo;
}
namespace HepMC {
  class GenParticle;
}
// forward declarations in global namespace
class G4VSolid;
class G4AffineTransform;
class LArG4SimpleSD;
class LArIdentifierConverter;
class TileGeoG4SDCalc;
class CaloDetDescrManager;
class LArEM_ID;
class Hep3Vector;
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

#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"


#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace G4UA{


  class FastCaloSimParamAction:
    public G4UserRunAction, public G4UserEventAction, public G4UserSteppingAction
  {

  public:

    struct Config
    {
      bool shift_lar_subhit=true;
      bool shorten_lar_step=false;
      std::string stepInfoCollName="EventSteps";
      // calculators
      ServiceHandle<ILArCalculatorSvc> calculator_EMECIW_pos=ServiceHandle<ILArCalculatorSvc>("EMECPosInnerWheelCalculator", "FastCaloSimParamAction");            //!< handle to EMEC positive inner wheel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_EMECIW_neg=ServiceHandle<ILArCalculatorSvc>("EMECNegInnerWheelCalculator", "FastCaloSimParamAction");            //!< handle to EMEC negative inner wheel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_EMECOW_pos=ServiceHandle<ILArCalculatorSvc>("EMECPosOuterWheelCalculator", "FastCaloSimParamAction");            //!< handle to EMEC positive outer wheel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_EMECOW_neg=ServiceHandle<ILArCalculatorSvc>("EMECNegOuterWheelCalculator", "FastCaloSimParamAction");            //!< handle to EMEC negative outer wheel calculator
      //ServiceHandle<ILArCalculatorSvc> calculator_BIB;                 //!< handle to EMEC Back Inner Barrette wheel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_BOB=ServiceHandle<ILArCalculatorSvc>("EMECBackOuterBarretteCalculator", "FastCaloSimParamAction");                   //!< handle to EMEC Back Outer Barrette wheel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_EMB=ServiceHandle<ILArCalculatorSvc>("EMBCalculator", "FastCaloSimParamAction");                   //!< handle to barrel calculator
      ServiceHandle<ILArCalculatorSvc> calculator_FCAL1=ServiceHandle<ILArCalculatorSvc>("FCAL1Calculator", "FastCaloSimParamAction");
      ServiceHandle<ILArCalculatorSvc> calculator_FCAL2=ServiceHandle<ILArCalculatorSvc>("FCAL2Calculator", "FastCaloSimParamAction");
      ServiceHandle<ILArCalculatorSvc> calculator_FCAL3=ServiceHandle<ILArCalculatorSvc>("FCAL3Calculator", "FastCaloSimParamAction");
      ServiceHandle<ILArCalculatorSvc> calculator_HEC=ServiceHandle<ILArCalculatorSvc>("HECWheelCalculator", "FastCaloSimParamAction");
      ServiceHandle<ILArCalculatorSvc> calculator_EMBPS=ServiceHandle<ILArCalculatorSvc>("EMBPresamplerCalculator", "FastCaloSimParamAction");                 //!< handle to barrel presampler calculator
      ServiceHandle<ILArCalculatorSvc> calculator_EMEPS=ServiceHandle<ILArCalculatorSvc>("EMECPresamplerCalculator", "FastCaloSimParamAction");                 //!< handle to endcap presampler calculator
      //ServiceHandle<ILArCalculatorSvc> calculator_HECLocal;
      ServiceHandle<ITileCalculator> calculator_TILE=ServiceHandle<ITileCalculator>("TileGeoG4SDCalc", "FastCaloSimParamAction");                    //!< pointer to tile calculator

      // Merging properties
      DoubleProperty            m_maxRadius=25.;                //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
      DoubleProperty            m_maxRadiusLAr=25.;             //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
      DoubleProperty            m_maxRadiusHEC=100.;             //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
      DoubleProperty            m_maxRadiusFCAL=100.;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink
      DoubleProperty            m_maxRadiusTile=100.;            //!< property, see @link LArG4GenShowerLib::LArG4GenShowerLib @endlink

      DoubleProperty            m_maxTime=25.;
      DoubleProperty            m_maxTimeLAr=25.;
      DoubleProperty            m_maxTimeHEC=25.;
      DoubleProperty            m_maxTimeFCAL=25.;
      DoubleProperty            m_maxTimeTile=25.;

      // Optimised merging scheme
      DoubleProperty            m_maxEtaPS=1.;
      DoubleProperty            m_maxPhiPS=5.;
      DoubleProperty            m_maxrPS=0.;

      DoubleProperty            m_maxEtaEM1=1.;
      DoubleProperty            m_maxPhiEM1=5.;
      DoubleProperty            m_maxrEM1=15.;

      DoubleProperty            m_maxEtaEM2=1.;
      DoubleProperty            m_maxPhiEM2=5.;
      DoubleProperty            m_maxrEM2=60.;

      DoubleProperty            m_maxEtaEM3=1.;
      DoubleProperty            m_maxPhiEM3=5.;
      DoubleProperty            m_maxrEM3=8.;

    };

    FastCaloSimParamAction(const Config& config);
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void UserSteppingAction(const G4Step*) override;
  private:
    Config m_config;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;


    /* data members */

    ILArCalculatorSvc *m_current_calculator;
    ITileCalculator *m_current_calculator_Tile;

    G4VSolid *m_current_solid;
    G4AffineTransform *m_current_transform;

    // helper
    LArG4SimpleSD *m_lar_helper;
    const LArEM_ID *m_lar_emID;
    const CaloDetDescrManager *m_calo_dd_man;


    void update_map(const CLHEP::Hep3Vector & l_vec, const Identifier & l_cell, double l_energy, double l_time, bool l_valid, int l_detector);
    std::map< Identifier , std::vector< ISF_FCS_Parametrization::FCS_StepInfo* >* > m_hit_map;

    SG::WriteHandle<ISF_FCS_Parametrization::FCS_StepInfoCollection> m_eventSteps;    //!< collection of StepInfo
    std::map<std::string,int> m_detectormap;
    std::set<std::string> m_unuseddetector;
    int m_ndetectors;


  }; // class FastCaloSimParamAction


} // namespace G4UA



#endif
