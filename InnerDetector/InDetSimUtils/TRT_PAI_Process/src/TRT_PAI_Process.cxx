/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_Process.h"
#include "TRT_PAI_gasdata.h"
#include "TRT_PAI_element.h"
#include "TRT_PAI_gasComponent.h"
#include "TRT_PAI_gasMixture.h"
#include "TRT_PAI_effectiveGas.h"
#include "TRT_PAI_physicsConstants.h"
#include "TRT_PAI_utils.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Units/SystemOfUnits.h"

// For Athena-based random numbers
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

// To get TRT dig. settings:
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include <vector>
#include <map>
#include <cmath>
#include <iostream>

//__________________________________________________________________________

TRT_PAI_Process::TRT_PAI_Process( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : base_class( type, name, parent )
  , m_deltaGamExp( (m_gamExpMax-m_gamExpMin)/m_nTabulatedGammaValues )
{
  //Properties:
  declareProperty( "GasType", m_gasType, "Gas Type" );
}
//__________________________________________________________________________
StatusCode TRT_PAI_Process::initialize() {

  using namespace TRT_PAI_gasdata;
  using namespace TRT_PAI_physicsConstants;

  ATH_MSG_VERBOSE ( "TRT_PAI_Process::initialize()" );

  // Supported gasType's are 70%/27%/03%:
  // 1) "Xenon"  - Xe/CO2/O2
  // 2) "Argon"  - Ar/CO2/O2
  // 3) "Kryton" - Kr/CO2/O2
  //
  // Figure out which type of gas we are having:

  ATH_MSG_DEBUG ( "Gastype read as " << m_gasType );

  std::string gasType = "NotSet";
  // Here m_gasType="Auto" was previously used to prompt a
  // check of InDetDD::TRT_DetectorManager for whether we want
  // the old Xenon mix: Xe/CO2/CF4 or the new one: Xe/CO2/O2
  // Nowadays, it is always the new one, so we don't check this anymore.

  if ( m_gasType == "Auto" )
    {
      gasType = "Xenon";
    }
  else if ( m_gasType == "Xenon" || m_gasType == "Argon" || m_gasType == "Krypton" )
    {
      ATH_MSG_DEBUG ( "Gastype is overriden from joboptions to be " << m_gasType );
      gasType = m_gasType;
    }
  else
    {
      ATH_MSG_FATAL ( "GasType property set to '" << m_gasType
                     << "'. Must be one of 'Auto', 'Xenon', 'Argon' or 'Krypton'." );
      return StatusCode::FAILURE;
    };

  ATH_MSG_INFO ( name() << " will use gas type = " << gasType );

  // Done with "trivial" initialization.
  // Finally we can start to construct gas

  ATH_MSG_VERBOSE ( "Constructing gas mixture." );

  // Need the gas temperature
  const double tempK = 289.;  // At 289. degrees, we get same densities as Nevski had

  // Define elements
  using element_type = std::map<std::string, TRT_PAI_element, std::less<std::string>>;

  element_type elements;
  elements["Xe"] = TRT_PAI_element( "Xe", EXe , SXe , NXe , ZXe,  AXe);
  elements["C"]  = TRT_PAI_element( "C" , EC  , SC  , NC  , ZC ,  AC );
  elements["F"]  = TRT_PAI_element( "F" , EF  , SF  , NF  , ZF ,  AF );
  elements["O"]  = TRT_PAI_element( "O" , EO  , SO  , NO  , ZO ,  AO );
  elements["Ar"] = TRT_PAI_element( "Ar", EAr , SAr , NAr , ZAr,  AAr);
  elements["Kr"] = TRT_PAI_element( "Kr", EKr , SKr , NKr , ZKr,  AKr);

  // Print out elements
  {
    for (element_type::iterator ei=elements.begin(); ei!=elements.end(); ++ei) {
      ATH_MSG_DEBUG ( ". Element "  << (*ei).second.getName()
                      << ", A= "    << (*ei).second.getAtomicA()
                      << ", Z= "    << (*ei).second.getAtomicZ()
                      << ", rho="   << (*ei).second.getDensity(tempK)
                      << " (" << tempK << " Kelvin)"
                      );
    }
  }

  // Define gas components

  using component_type = std::map<std::string, TRT_PAI_gasComponent, std::less<std::string>>;

  component_type components;

  components["Xe"]  = TRT_PAI_gasComponent("Xe");
  components["Xe"].addElement(&elements["Xe"],1);

  components["CO2"] = TRT_PAI_gasComponent("CO2");
  components["CO2"].addElement(&elements["C"],1);
  components["CO2"].addElement(&elements["O"],2);

  components["CF4"] = TRT_PAI_gasComponent("CF4");
  components["CF4"].addElement(&elements["C"],1);
  components["CF4"].addElement(&elements["F"],4);

  components["O2"]  = TRT_PAI_gasComponent("O2");
  components["O2"].addElement(&elements["O"],2);

  components["Ar"]  = TRT_PAI_gasComponent("Ar");
  components["Ar"].addElement(&elements["Ar"],1);

  components["Kr"]  = TRT_PAI_gasComponent("Kr");
  components["Kr"].addElement(&elements["Kr"],1);

  // Print out gas components

  {
    std::vector<std::string> cnam(6);
    cnam[0] = "Xe"; cnam[1] = "CO2"; cnam[2] = "CF4"; cnam[3] = "CO2"; cnam[4] = "Ar"; cnam[5] = "Kr";

    int n;
    for ( int ic=0; ic<6; ++ic ) {
      n = components[cnam[ic]].getNElementTypes();
      ATH_MSG_DEBUG ( ". Gas component " << components[cnam[ic]].getName() << " contains");
      for ( int ie=0; ie<n; ++ie ) {
        ATH_MSG_DEBUG ( "   - "        <<  components[cnam[ic]].getElementMultiplicity(ie)
                        << " atoms "   << (components[cnam[ic]].getElement(ie))->getName()
                        << " with Z= " << (components[cnam[ic]].getElement(ie))->getAtomicZ()
                        << " and A= "  << (components[cnam[ic]].getElement(ie))->getAtomicA()
                        );
      }
      ATH_MSG_DEBUG ( "   > density: " << components[cnam[ic]].getDensity(tempK) << " (" << tempK << " Kelvin)" );
    }
  }

  // Construct TRT gas mixture
  std::string mixtureName="TRT  Gas Mixture";
  mixtureName.insert(4, gasType);
  m_trtgas = new TRT_PAI_gasMixture(mixtureName);

  if ( gasType == "Xenon" ) {
    ATH_MSG_DEBUG ( "Using new Xenon gas mixture (Xe/CO2/O2 - 70/27/3)." );
    m_trtgas->addComponent( &components["Xe"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.27);
    m_trtgas->addComponent( &components["O2"] , 0.03);
  }

  if (( gasType == "XenonOld" )){
    ATH_MSG_DEBUG ( "Using old Xenon gas mixture (Xe/CO2/CF4 - 70/10/20)." );
    m_trtgas->addComponent( &components["Xe"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.10);
    m_trtgas->addComponent( &components["CF4"], 0.20);
  }

  if ( gasType == "Argon" ) {
    ATH_MSG_DEBUG ( "Using Argon gas mixture (Ar/CO2/O2 - 70/27/3)." );
    m_trtgas->addComponent( &components["Ar"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.27);
    m_trtgas->addComponent( &components["O2"] , 0.03);
  }

  if ( gasType == "Krypton" ) {
    ATH_MSG_DEBUG ( "Using Krypton gas mixture (Kr/CO2/O2 - 70/27/3)." );
    m_trtgas->addComponent( &components["Kr"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.27);
    m_trtgas->addComponent( &components["O2"] , 0.03);
  }

  m_trtgas->freezeGas();

  m_trtgas->showStructure();

  // Create and initialize effective gas
  // Sasha: to check: does Emin depends from Energy levels of active gas?
  // for Xenon lowest energetic level is 12.08 eV
  // for Argon - 15.83 eV. Does Emin should be different for Argon???

  const double Emin = 12.0;        // [eV]
  const double Emax = 1e+07;       // [eV]
                                   // Don't change Emax from 10 MeV without
                                   // talking to Pavel Nevski!
  const double eps   = 0.01;       // Epsilon for numeric integration

  TRT_PAI_effectiveGas effectiveGas(m_trtgas, Emin, Emax, tempK, eps);

  // Now tabulate...

  ATH_MSG_DEBUG ( "Making tables for various gamma factors." );

  std::vector<float> gamvec(m_nTabulatedGammaValues);
  std::vector<float> lnE;

  double gamvar = m_gamExpMin - 0.5*m_deltaGamExp;
  for ( unsigned int ig = 0; ig < m_nTabulatedGammaValues; ++ig ) {
    gamvar    += m_deltaGamExp;
    gamvec[ig] = 1. + pow(10.,gamvar);
  }

  // Here is the real action!

  effectiveGas.GasTab(gamvec, m_en_array, m_fn_array, m_dndx );

  ATH_MSG_DEBUG ( "Initialization completed." );

  return StatusCode::SUCCESS;
}

//__________________________________________________________________________
StatusCode TRT_PAI_Process::finalize() {
  delete m_trtgas;
  return StatusCode::SUCCESS;
}

//__________________________________________________________________________
inline double TRT_PAI_Process::ScaledEkin2GamVarTab(double scaledKineticEnergy)
  const {
  using namespace TRT_PAI_physicsConstants;
  return (log(scaledKineticEnergy/MProtonMeV)*invlog10 - m_gamExpMin)/m_deltaGamExp;
}

//__________________________________________________________________________
double TRT_PAI_Process::GetMeanFreePath(double scaledKineticEnergy,
                                        double squaredCharge) const {

  if ( squaredCharge == 0. ) return 99999.0 * CLHEP::km;

  double gv = ScaledEkin2GamVarTab( scaledKineticEnergy );
  unsigned int index = 0;
  if ( gv > 0 ) index = static_cast<unsigned int >(gv);

  double d;
  if ( gv < 0 ) d = m_dndx[0];
  else if ( index >= m_nTabulatedGammaValues-1 ) d = m_dndx[m_nTabulatedGammaValues-1];
  else {
    double remain = gv-index;
    d = (1.-remain)*m_dndx[index] + remain*m_dndx[index+1];
  }

  double freepath = 1./(d*squaredCharge);

#ifndef NDEBUG
  ATH_MSG_VERBOSE ( "Mean free path for scaledKineticEnergy = " << scaledKineticEnergy
                    << " is " << freepath/CLHEP::mm << " mm " );
#endif

  // All internal calculations are performed without initialization with
  //   CLHEP units, so we put it here instead

  return freepath * CLHEP::cm;
}

//__________________________________________________________________________
double TRT_PAI_Process::GetEnergyTransfer(double scaledKineticEnergy, CLHEP::HepRandomEngine* rndmEngine) const {

  double gv = ScaledEkin2GamVarTab( scaledKineticEnergy );
  unsigned int tabIndx = 0;

  if ( gv>0 ) tabIndx = static_cast<unsigned int>( gv );

  tabIndx = std::min( tabIndx, m_nTabulatedGammaValues-1 );

  // Generate a random number uniformly in [0,1].
  //  CLHEP::HepRandomEngine* pHRengine = m_pAtRndmGenSvc->GetEngine("TRT_PAI");
  double random = CLHEP::RandFlat::shoot(rndmEngine, 0., 1.);

  // What we are doing next is actually to select a value of E from
  // dN^2/dXdE considered as a function of E using the standard Monte
  // Carlo method. The total area under the function is equal to
  // dN/dX, and since we can not calculate the inverse antiderivative,
  // we have tabulated the antiderivative in (m_en_array, m_fn_array)
  // and taking the inverse is as simple as switching the roles of the
  // two arrays.

  double Etransf = TRT_PAI_utils::Interpolate (random * m_dndx[tabIndx], m_fn_array[tabIndx], m_en_array ) ;

  Etransf = exp(Etransf);

#ifndef NDEBUG
  ATH_MSG_VERBOSE ( "Energy transfer for scaledKineticEnergy = "
                    << scaledKineticEnergy << " is " << Etransf << " eV"
                    );
#endif

  return Etransf * CLHEP::eV;
}
