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
class ILArCalculatorSvc;
class ITileCalculator;
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

#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/IEndRunAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"


#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"


namespace G4UA{


  class FastCaloSimParamAction:
    public IBeginRunAction,  public IEndRunAction,  public IBeginEventAction,  public IEndEventAction,  public ISteppingAction
  {

  public:

    struct Config
    {
      bool shift_lar_subhit=true;
      bool shorten_lar_step=false;
      // calculators
      ILArCalculatorSvc* calculator_EMECIW_pos=nullptr;            //!< pointer to EMEC positive inner wheel calculator
      ILArCalculatorSvc* calculator_EMECIW_neg=nullptr;            //!< pointer to EMEC negative inner wheel calculator
      ILArCalculatorSvc* calculator_EMECOW_pos=nullptr;            //!< pointer to EMEC positive outer wheel calculator
      ILArCalculatorSvc* calculator_EMECOW_neg=nullptr;            //!< pointer to EMEC negative outer wheel calculator
      //ILArCalculatorSvc* calculator_BIB=nullptr;                 //!< pointer to EMEC Back Inner Barrette wheel calculator
      ILArCalculatorSvc* calculator_BOB=nullptr;                   //!< pointer to EMEC Back Outer Barrette wheel calculator
      ILArCalculatorSvc* calculator_EMB=nullptr;                   //!< pointer to barrel calculator
      ILArCalculatorSvc* calculator_FCAL1=nullptr;
      ILArCalculatorSvc* calculator_FCAL2=nullptr;
      ILArCalculatorSvc* calculator_FCAL3=nullptr;
      ILArCalculatorSvc* calculator_HEC=nullptr;
      ILArCalculatorSvc* calculator_EMBPS=nullptr;                 //!< pointer to barrel presampler calculator
      ILArCalculatorSvc* calculator_EMEPS=nullptr;                 //!< pointer to endcap presampler calculator
      //ILArCalculatorSvc* calculator_HECLocal=nullptr;
      ITileCalculator* calculator_TILE=nullptr;                    //!< pointer to tile calculator

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

    };

    FastCaloSimParamAction(const Config& config);
    virtual void beginOfRun(const G4Run*) override;
    virtual void endOfRun(const G4Run*) override;
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void processStep(const G4Step*) override;
  private:
    Config m_config;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
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

    ISF_FCS_Parametrization::FCS_StepInfoCollection *m_eventSteps;    //!< collection of StepInfo
    std::map<std::string,int> m_detectormap;
    std::set<std::string> m_unuseddetector;
    int m_ndetectors;


  }; // class FastCaloSimParamAction


} // namespace G4UA



#endif
