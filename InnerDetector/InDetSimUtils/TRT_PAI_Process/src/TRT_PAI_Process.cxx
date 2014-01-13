/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_PAI_Process/TRT_PAI_Process.h"
#include "TRT_PAI_Process/TRT_PAI_gasdata.h"
#include "TRT_PAI_Process/TRT_PAI_element.h"
#include "TRT_PAI_Process/TRT_PAI_gasComponent.h"
#include "TRT_PAI_Process/TRT_PAI_gasMixture.h"
#include "TRT_PAI_Process/TRT_PAI_effectiveGas.h"
#include "TRT_PAI_Process/TRT_PAI_physicsConstants.h"
#include "TRT_PAI_Process/TRT_PAI_utils.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Units/SystemOfUnits.h"

// For Athena-based random numbers
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// To get TRT dig. settings:
#include "StoreGate/StoreGateSvc.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <vector>
#include <map>
#include <cmath>
#include <iostream>

//__________________________________________________________________________

TRT_PAI_Process::TRT_PAI_Process( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AthAlgTool( type, name, parent ),
    m_nTabulatedGammaValues( 56 ),
    m_gamExpMin( -2. ),
    m_gamExpMax(  5. ),
    m_deltaGamExp( (m_gamExpMax-m_gamExpMin)/m_nTabulatedGammaValues ),
    m_pAtRndmGenSvc ("AtRndmGenSvc", name)
{
  // declare special interface
  declareInterface<ITRT_PAITool>(this);

  //Properties:
  declareProperty( "GasType", m_gasType = "Auto", "Gas Type" );
  declareProperty( "RndServ", m_pAtRndmGenSvc, "Random Number Service for TRT_PAI_Process" );

}
//__________________________________________________________________________
StatusCode TRT_PAI_Process::initialize() {

  using namespace TRT_PAI_gasdata;
  using namespace TRT_PAI_physicsConstants;

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Initializing TRT_PAI_Process." << endreq;

  // Get the Rndm number service
  if ( !m_pAtRndmGenSvc.retrieve().isSuccess() ) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Problems retrieving random number service" << endreq;
    return StatusCode::FAILURE;
  }
  m_pHRengine = m_pAtRndmGenSvc->GetEngine("TRT_PAI");

  // Sasha:
  // currently supported gasType's:
  // 1) XenonNew - Xe/CO2/O2    70/27/3
  // 2) XenonOld - Xe/CO2/CF4   70/10/20
  // 3) Argon    - Ar/CO2/O2    70/27/3
  //
  // Figure out which type of gas we are having:
  std::string gasType = "XenonNew";

  /// Sasha: looks like 'Auto' work only for Xenon based mixtures
  if ( m_gasType == "Auto" ) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Autodetecting gas type via TRT_DetectorManager" << endreq;

    // Get the TRT Detector Manager
    const InDetDD::TRT_DetectorManager * manager;
    if ( StatusCode::SUCCESS != detStore()->retrieve(manager,"TRT") ) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Can't get TRT_DetectorManager." << endreq;
      return StatusCode::FAILURE;
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved TRT_DetectorManager with version "
                                              << manager->getVersion().majorNum() << endreq;
    }

    if ( manager->gasType() == InDetDD::TRT_DetectorManager::newgas )      { gasType = "XenonNew"; }
    else if ( manager->gasType() == InDetDD::TRT_DetectorManager::oldgas ) { gasType = "XenonOld"; }
    else if ( manager->gasType() == InDetDD::TRT_DetectorManager::unknown ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "GasType info not set in TRT_DetectorManager. Using new gas." << endreq;
      gasType = "XenonNew";
    } else {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unrecognized GasType info set in TRT_DetectorManager. Using new gas." << endreq;
      gasType = "XenonNew";
    }

  } else if ( m_gasType == "XenonNew" || m_gasType == "XenonOld" || m_gasType == "Argon" ) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Gastype is overriden from joboptions to be " << m_gasType << endreq;
    gasType = m_gasType;
  }
  else {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "GasType property set to '" << m_gasType
         << "'. Must be one of 'Auto', 'XenonNew', 'XenonOld' or 'Argon'." << endreq;
    return StatusCode::FAILURE;
  };

  // Done with "trivial" initialization.
  // Finally we can start to construct gas

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Constructing gas mixture." << endreq;

  // Need the gas temperature

  const double tempK = 289.;  // At 289. degrees, we get same densities as Nevski had

  // Define elements
  typedef std::map<std::string, TRT_PAI_element, std::less<std::string> > element_type;

  element_type elements;
  elements["Xe"] = TRT_PAI_element( "Xe", EXe , SXe , NXe , ZXe,  AXe);
  elements["C"]  = TRT_PAI_element( "C" , EC  , SC  , NC  , ZC ,  AC );
  elements["F"]  = TRT_PAI_element( "F" , EF  , SF  , NF  , ZF ,  AF );
  elements["O"]  = TRT_PAI_element( "O" , EO  , SO  , NO  , ZO ,  AO );
  elements["Ar"] = TRT_PAI_element( "Ar", EAr , SAr , NAr , ZAr,  AAr);

  // Print out elements
  {
    for (element_type::iterator ei=elements.begin(); ei!=elements.end(); ei++) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	<< ". Element " << (*ei).second.getName()
	<< ", A= "    << (*ei).second.getAtomicA()
	<< ", Z= "    << (*ei).second.getAtomicZ()
	<< ", rho="   << (*ei).second.getDensity(tempK)
	<< " (" << tempK << " Kelvin)"
	<< endreq;
    }
  }

  // Define gas components

  typedef std::map<std::string, TRT_PAI_gasComponent, std::less<std::string> > component_type;

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

  // Print out gas components

  {
    std::vector<std::string> cnam(5);
    cnam[0] = "Xe"; cnam[1] = "CO2"; cnam[2] = "CF4"; cnam[3] = "CO2"; cnam[4] = "Ar";

    int n;
    for ( int ic=0; ic<5; ++ic ) {
      n = components[cnam[ic]].getNElementTypes();
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	<< ". Gas component " << components[cnam[ic]].getName() << " contains"<< endreq;
      for ( int ie=0; ie<n; ++ie ) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
	  << "   - "     <<  components[cnam[ic]].getElementMultiplicity(ie)
	  << " atoms "   << (components[cnam[ic]].getElement(ie))->getName()
	  << " with Z= " << (components[cnam[ic]].getElement(ie))->getAtomicZ()
	  << " and A= "  << (components[cnam[ic]].getElement(ie))->getAtomicA()
	  << endreq;
      }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "   > density: " << components[cnam[ic]].getDensity(tempK) << " (" << tempK << " Kelvin)" << endreq;
    }
  }

  // Construct TRT gas mixture

  m_trtgas = new TRT_PAI_gasMixture("TRT Gas Mixture");

  if ( gasType == "XenonNew" ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using new Xenon gas mixture (Xe/CO2/O2 - 70/27/3)." << endreq;
    m_trtgas->addComponent( &components["Xe"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.27);
    m_trtgas->addComponent( &components["O2"] , 0.03);
  }

  if (( gasType == "XenonOld" )){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using old Xenon gas mixture (Xe/CO2/CF4 - 70/10/20)." << endreq;
    m_trtgas->addComponent( &components["Xe"] , 0.70);
    m_trtgas->addComponent( &components["CO2"], 0.10);
    m_trtgas->addComponent( &components["CF4"], 0.20);
  }

  if ( gasType == "Argon" ) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using Argon gas mixture (Ar/CO2/O2 - 70/27/3)." << endreq;
    m_trtgas->addComponent( &components["Ar"] , 0.70);
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

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Making tables for various gamma factors." << endreq;

  std::vector<float> gamvec(m_nTabulatedGammaValues);
  std::vector<float> lnE;

  double gamvar = m_gamExpMin - 0.5*m_deltaGamExp;
  for ( unsigned int ig = 0; ig < m_nTabulatedGammaValues; ++ig ) {
    gamvar    += m_deltaGamExp;
    gamvec[ig] = 1. + pow(10.,gamvar);
  }

  // Here is the real action!

  effectiveGas.GasTab(gamvec, m_en_array, m_fn_array, m_dndx );

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialization completed." << endreq;

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
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
    << "Mean free path for scaledKineticEnergy = " << scaledKineticEnergy
    << " is " << freepath/CLHEP::mm << " mm " << endreq;
#endif

  // All internal calculations are performed without initialization with
  //   CLHEP units, so we put it here instead

  return freepath * CLHEP::cm;
}

//__________________________________________________________________________
double TRT_PAI_Process::GetEnergyTransfer(double scaledKineticEnergy) const {

  double gv = ScaledEkin2GamVarTab( scaledKineticEnergy );
  unsigned int tabIndx = 0;

  if ( gv>0 ) tabIndx = static_cast<unsigned int>( gv );

  tabIndx = std::min( tabIndx, m_nTabulatedGammaValues-1 );

  // Generate a random number uniformly in [0,1].
  //  CLHEP::HepRandomEngine* pHRengine = m_pAtRndmGenSvc->GetEngine("TRT_PAI");
  double random = CLHEP::RandFlat::shoot(m_pHRengine, 0., 1.);

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
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
    << "Energy transfer for scaledKineticEnergy = "
    << scaledKineticEnergy << " is " << Etransf << " eV"
    << endreq;
#endif

  return Etransf * CLHEP::eV;
}
