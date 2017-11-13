/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//                                                             //
// TRTDigSettings.cxx                                          //
//                                                             //
// Class containing parameters and settings mainly used by TRT //
// digitization.                                               //
//                                                             //
// Author: Thomas Kittelmann <kittel@nbi.dk>                   //
// First version: April 2005. Rewritten November 2005.         //
//                                                             //
//  - Settings can be changed via:                             //
//    share/postInclude.OverrideTRTparameters.py               //
//                                                             //
/////////////////////////////////////////////////////////////////
#include "TRTDigSettings.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h" //For speed of light
#include "GaudiKernel/Algorithm.h"         //For adding properties to an algorithm
#include "GaudiKernel/AlgTool.h"           //For adding properties to an algtool
#include <iostream>
#include <limits>

//Geomodel
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// RDBAccessSvc (Interface to the DD database) has been removed since
// TRT_Digitization-01-00-11 because the fetched values are correctly
// hard-coded already. See TRT_Digitization-01-00-10 for an example of
// how that works.

//_________________________________________________________________________________________________________
TRTDigSettings::TRTDigSettings()
  : m_digversion(-1),
    m_propertyNotSetMagicNumber(-999.0e50),
    m_propertyNotSetMagicNumber_int(-9999),
    m_msg("TRTDigSettings")
{
  defineVariables();
}

//_________________________________________________________________________________________________________
void TRTDigSettings::initialize(const InDetDD::TRT_DetectorManager* detmgr) {

  if (msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing" << endmsg;

  //1) Fill defaults based on digversion
  fillDefaults(detmgr);

  //2) Process overrides.
  processOverrides();

}

//_________________________________________________________________________________________________________
void TRTDigSettings::defineVariables() {

  //The ranges of allowed values are pretty wide. If parameters are outside we will assume something is wrong.

  //doubles:
  defineNewVariable("ionisationPotential",&m_ionisationPotential,"Ionisation potential","eV",CLHEP::eV,1.0,50.0);
  defineNewVariable("ionisationPotentialArgon",&m_ionisationPotentialArgon,"Ionisation potential Argon","eV",CLHEP::eV,1.0,50.0);
  defineNewVariable("ionisationPotentialKrypton",&m_ionisationPotentialKrypton,"Ionisation potential Krypton","eV",CLHEP::eV,1.0,50.0);
  defineNewVariable("smearingFactor",&m_smearingFactor,"Cluster energy smearing factor","",1,0.1,1.0);
  defineNewVariable("smearingFactorArgon",&m_smearingFactorArgon,"Cluster energy smearing factor Argon","",1,0.1,1.0);
  defineNewVariable("smearingFactorKrypton",&m_smearingFactorKrypton,"Cluster energy smearing factor Krypton","",1,0.1,1.0);
  defineNewVariable("timeInterval",&m_timeInterval,"Time interval covered by each digit","ns",CLHEP::ns,1,200);
  defineNewVariable("minDiscriminatorWidth",&m_minDiscriminatorWidth,"Minimum discriminator time over threshold","ns",CLHEP::ns,0.5,20.0);
  defineNewVariable("discriminatorSettlingTime",&m_discriminatorSettlingTime,"Discriminator settling time","ns",CLHEP::ns,0.5,20.0);
  defineNewVariable("discriminatorDeadTime",&m_discriminatorDeadTime,"Discriminator dead time","ns",CLHEP::ns,0.5,20.0);
  defineNewVariable("TrtRangeCutProperty",&m_trtRangeCutProperty,"Electrons range cut in TRT xenon simulation","mm",CLHEP::mm,0.05,30.00);

  // LT Bar
  defineNewVariable("lowThresholdBar",       &m_lowThresholdBar,       "Low Threshold Barrel",         "eV",CLHEP::eV,50.0,500.0);
  defineNewVariable("lowThresholdBarArgon",  &m_lowThresholdBarArgon,  "Low Threshold Barrel Argon",   "eV",CLHEP::eV,50.0,500.0);
  defineNewVariable("lowThresholdBarKrypton",&m_lowThresholdBarKrypton,"Low Threshold Barrel Krypton", "eV",CLHEP::eV,50.0,500.0);
  // LT EC
  defineNewVariable("lowThresholdEC",        &m_lowThresholdEC,        "Low Threshold end-cap",        "eV",CLHEP::eV,50.0,500.0);
  defineNewVariable("lowThresholdECArgon",   &m_lowThresholdECArgon,   "Low Threshold end-cap Argon",  "eV",CLHEP::eV,50.0,500.0);
  defineNewVariable("lowThresholdECKrypton", &m_lowThresholdECKrypton, "Low Threshold end-cap Krypton","eV",CLHEP::eV,50.0,500.0);
  // HT Short
  defineNewVariable("highThresholdBarShort",       &m_highThresholdBarShort,       "High Threshold short barrel straws",        "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdBarShortArgon",  &m_highThresholdBarShortArgon,  "High Threshold short barrel straws Argon",  "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdBarShortKrypton",&m_highThresholdBarShortKrypton,"High Threshold short barrel straws Krypton","keV",CLHEP::keV,1.0,10.0);
  // HT Long
  defineNewVariable("highThresholdBarLong",       &m_highThresholdBarLong,       "High Threshold long barrel straws",        "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdBarLongArgon",  &m_highThresholdBarLongArgon,  "High Threshold long barrel straws Argon",  "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdBarLongKrypton",&m_highThresholdBarLongKrypton,"High Threshold long barrel straws Krypton","keV",CLHEP::keV,1.0,10.0);
  // HT EC-A
  defineNewVariable("highThresholdECAwheels",       &m_highThresholdECAwheels,       "High Threshold A type wheels",        "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdECAwheelsArgon",  &m_highThresholdECAwheelsArgon,  "High Threshold A type wheels Argon",  "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdECAwheelsKrypton",&m_highThresholdECAwheelsKrypton,"High Threshold A type wheels Krypton","keV",CLHEP::keV,1.0,10.0);
  // HT EC-B
  defineNewVariable("highThresholdECBwheels",       &m_highThresholdECBwheels,       "High Threshold B type wheels",        "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdECBwheelsArgon",  &m_highThresholdECBwheelsArgon,  "High Threshold B type wheels Argon",  "keV",CLHEP::keV,1.0,10.0);
  defineNewVariable("highThresholdECBwheelsKrypton",&m_highThresholdECBwheelsKrypton,"High Threshold B type wheels Krypton","keV",CLHEP::keV,1.0,10.0);

  defineNewVariable("strawLengthBarrel",&m_strawLengthBarrel,"Long barrel straw length","mm",CLHEP::mm,1400.0,1450.0); // 1425.5
  defineNewVariable("innerRadiusEndcap",&m_innerRadiusEndcap,"Inner radius of the endcap straws","mm",CLHEP::mm,600.0,640.0); // 621.18
  defineNewVariable("outerRadiusEndcap",&m_outerRadiusEndcap,"Outer radius of the endcap straws","mm",CLHEP::mm,1060.0,1070.0); // 1067

  defineNewVariable("innerRadiusOfStraw",&m_innerRadiusOfStraw,"Inner radius of straw","mm",CLHEP::mm,1.0,3.0); // 2.0 mm
  defineNewVariable("outerRadiusOfWire",&m_outerRadiusOfWire,"Outer radius of wire","micrometer",CLHEP::micrometer,5.0,40.0); // 0.0155 mm
  defineNewVariable("lengthOfDeadRegion",&m_lengthOfDeadRegion,"Length of dead region at straw ends","mm",CLHEP::mm,1.0,3.0);
  defineNewVariable("signalPropagationSpeed",&m_signalPropagationSpeed,"Speed of signal propagation along wire","c",CLHEP::c_light,0.1,1.0);
  defineNewVariable("overallT0Shift",&m_overallT0Shift,"Overall shift of all electronics T0's to get correct effects of pileup, noise, etc.","ns",CLHEP::ns,-5000.0,5000.0);
  defineNewVariable("overallT0ShiftShortBarrel",&m_overallT0ShiftShortBarrel,"Overall shift of electronics T0's in the short barrel straws.","ns",CLHEP::ns,-5000.0,5000.0);
  defineNewVariable("distanceToTimeFactor",&m_distanceToTimeFactor,"Fudge factor changing assumed particle propagation speed in time corr. calculations","",1.0,0.1,10.0);
  defineNewVariable("maxVertexDisplacement",&m_maxVertexDisplacement,"Maximum vertex displacement","cm",CLHEP::cm,0.0,50.0);
  defineNewVariable("timeOffsetCalcVertexX",&m_timeOffsetCalcVertexX,"X coord. of point where particles are assumed to originate from for time-shift","m",CLHEP::m,-150.0,150.0);
  defineNewVariable("timeOffsetCalcVertexY",&m_timeOffsetCalcVertexY,"Y coord. of point where particles are assumed to originate from for time-shift","m",CLHEP::m,-150.0,150.0);
  defineNewVariable("timeOffsetCalcVertexZ",&m_timeOffsetCalcVertexZ,"Z coord. of point where particles are assumed to originate from for time-shift","m",CLHEP::m,-150.0,150.0);
  defineNewVariable("pileUpSDOsMinEkin",&m_pileUpSDOsMinEkin,"Minimum kinetic energy for pile-up MC-truth (0.0=all, 999TeV=none)","GeV",CLHEP::GeV,0.0,999.0*CLHEP::TeV);
  defineNewVariable("trEfficiencyBarrel",&m_trEfficiencyBarrel,"Transition radiation efficiency barrel","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapA",&m_trEfficiencyEndCapA,"Transition radiation efficiency endcap A","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapB",&m_trEfficiencyEndCapB,"Transition radiation efficiency endcap B","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyBarrelArgon",&m_trEfficiencyBarrelArgon,"Transition radiation efficiency barrel Argon","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapAArgon",&m_trEfficiencyEndCapAArgon,"Transition radiation efficiency endcap A Argon","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapBArgon",&m_trEfficiencyEndCapBArgon,"Transition radiation efficiency endcap B Argon","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyBarrelKrypton",&m_trEfficiencyBarrelKrypton,"Transition radiation efficiency barrel Krypton","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapAKrypton",&m_trEfficiencyEndCapAKrypton,"Transition radiation efficiency endcap A Krypton","%",0.01,0.0,100.0);
  defineNewVariable("trEfficiencyEndCapBKrypton",&m_trEfficiencyEndCapBKrypton,"Transition radiation efficiency endcap B Krypton","%",0.01,0.0,100.0);
  defineNewVariable("fastElectronicsNoisePulseDistance",&m_fastElectronicsNoisePulseDistance,"Fast electronics noise-pulse distance","ns",CLHEP::ns,0.01,20.0);
  defineNewVariable("slowPeriodicNoisePulseDistance",&m_slowPeriodicNoisePulseDistance,"Slow periodic electronics noise-pulse distance","ns",CLHEP::ns,1.0,500.0);
  defineNewVariable("slowPeriodicNoisePulseFraction",&m_slowPeriodicNoisePulseFraction,"Fraction of slow periodic pulses","%",0.01,0.0,1.0);
  defineNewVariable("averageNoiseLevel",&m_averageNoiseLevel,"Average noise level used for fake cond. map","%",0.01,0.0,10.0);
  defineNewVariable("crossTalkNoiseLevel",&m_crosstalkNoiseLevel,"Average crosstalk noise level used for fake cond. map","%",0.01,0.0,100.0);
  defineNewVariable("crossTalkNoiseLevelOtherEnd",&m_crosstalkNoiseLevelOtherEnd,"Average crosstalk noise level on the other end as straws with pad hits","%",0.01,0.0,100.0);
  defineNewVariable("relativeLowThresholdFluctuation",&m_relativeLowThresholdFluctuation,"Relative LT fluct. (evt to evt & straw to straw)","%",0.01,0.0,35.0);
  defineNewVariable("relativeHighThresholdFluctuation",&m_relativeHighThresholdFluctuation,"Relative HT fluct. (evt to evt & straw to straw)","%",0.01,0.0,35.0);
  defineNewVariable("solenoidFieldStrength",&m_solenoidFieldStrength,"Solenoid Field Strength (assume perfect uniform field)","T",CLHEP::tesla,0.0,3.0);
  defineNewVariable("jitterTimeOffset",&m_jitterTimeOffset,"Jitter time offset (simulate lack of cosmic bunchcrossing time","ns",CLHEP::ns,0.0,25.0);
  defineNewVariable("attenuationLength",&m_attenuationLength,"Signal attenuation length in the wire","cm",CLHEP::cm,100.0,700.0);

  //unsigned ints:
  defineNewUIntVariable("numberOfBins",&m_numberOfBins,"Number of internal bins in time interval covered by each digit (must be whole multiple of numberOfBinsInEncodingBin)",24,480);
  defineNewUIntVariable("numberOfBinsInEncodingBin",&m_numberOfBinsInEncodingBin,"Number of bins in each encoding bin in digit",1,20);
  defineNewUIntVariable("numberOfCrossingsBeforeMain",&m_numberOfCrossingsBeforeMain,"Number of crossings before main event",0,50);
  defineNewUIntVariable("cosmicFlag",&m_cosmicFlag,"Flag for masking out certain barrel modules for the cosmic runs",0,3);
  defineNewUIntVariable("storeSDO",&m_storeSDO,"Define the time range in which to store the SDOs",0,2);

  //bools:
  defineNewBoolVariable("timeCorrection",&m_timeCorrection,"Corrections due to time of flight and wire-propagation times (false for beamType='cosmics')");
  defineNewBoolVariable("noiseInUnhitStraws",&m_noiseInUnhitStraws,"Noise in straws not passed by sim. particles");
  defineNewBoolVariable("noiseInSimhits",&m_noiseInSimhits,"Noise in straws passed by sim. particles");
  defineNewBoolVariable("electronicsAreAtFarEnd",&m_electronicsAreAtFarEnd,"Electronics assumed to be at the straw ends furthest from primary sim. vertex");
  defineNewBoolVariable("timeshiftsSymmetricForPhiSectors",&m_timeshiftsSymmetricForPhiSectors,"Electronics Time offsets are symmetric from phi-sector to phi-sector");
  defineNewBoolVariable("isCTB",&m_isCTB,"Flag set for CTB digitization");
  defineNewBoolVariable("killEndCap",&m_killEndCap,"Kill all EndCap straws");
  defineNewBoolVariable("killBarrel",&m_killBarrel,"Kill all Barrel straws");
  defineNewBoolVariable("doCosmicTimingPit",&m_doCosmicTimingPit,"Do cosmic timing corresponding to pit setup");
  defineNewBoolVariable("doCrosstalk",&m_doCrosstalk,"Do crosstalk noise simulation");
  defineNewBoolVariable("useMagneticFieldMap",&m_useMagneticFieldMap,"Use magnetic field map in drifttime calculation");
  defineNewBoolVariable("useAttenuation",&m_useAttenuation,"Simulate attenuation of signal strength depending on propagation length in wire");
  defineNewBoolVariable("getT0FromData",&m_getT0FromData,"Shift the individual straw t0 according to data (conditions database)");
  defineNewBoolVariable("isOverlay",&m_isOverlay,"Flag set for overlay jobs");

  //ints:
  defineNewIntVariable("htT0shiftBarShort", &m_htT0shiftBarShort, "HT T0 delta shift in 0.78125 ns steps, short barrel straws",-32,32);
  defineNewIntVariable("htT0shiftBarLong",  &m_htT0shiftBarLong,  "HT T0 delta shift in 0.78125 ns steps, long barrel straws", -32,32);
  defineNewIntVariable("htT0shiftECAwheels",&m_htT0shiftECAwheels,"HT T0 delta shift in 0.78125 ns steps, A type wheels",      -32,32);
  defineNewIntVariable("htT0shiftECBwheels",&m_htT0shiftECBwheels,"HT T0 delta shift in 0.78125 ns steps, B type wheels",      -32,32);

}

//check that all have a default set and that it is inside range!!
//(check also that unsetmagicnumber is outside range)
//In init: setDefaultsBasedOnVersionNumber(digVers);
//_________________________________________________________________________________________________________
void TRTDigSettings::print(const std::string& front) const {

  std::cout << front << "======= Printing TRT Digitization Settings ======="<<std::endl;
  //std::cout << front <<std::endl;

  std::map <std::string,doubleparameter>::const_iterator itd = m_doubleparMap.begin();
  std::map <std::string,doubleparameter>::const_iterator itdE = m_doubleparMap.end();

  for (;itd!=itdE;++itd) {
    //std::cout << front << std::endl;
    std::cout << front << itd->second.description<<": "<<(*(itd->second.directvaraddress))/itd->second.unit<<" "<<itd->second.unitname <<std::endl;
  };

  std::map <std::string,intboolparameter>::const_iterator itib = m_intboolparMap.begin();
  std::map <std::string,intboolparameter>::const_iterator itibE = m_intboolparMap.end();

  for (;itib!=itibE;++itib) {
    //std::cout << front << std::endl;
    if (itib->second.directvaraddress_int) {
      //int
      std::cout << front << itib->second.description<<": "<<*(itib->second.directvaraddress_int)<<std::endl;
    } else if (itib->second.directvaraddress_uint) {
      //unsigned int
      std::cout << front << itib->second.description<<": "<<*(itib->second.directvaraddress_uint)<<std::endl;
    } else {
      //bool
      assert(itib->second.directvaraddress_bool);
      std::cout << front << itib->second.description<<": "<<(*(itib->second.directvaraddress_bool)?1:0)<<std::endl;
    }
  }

  //std::cout << front <<std::endl;
  std::cout << front << "==============================================="<<std::endl;

  return;

}

//---------------------------------------------------------------------

StatusCode TRTDigSettings::DigSettingsFromCondDB(int dig_vers_from_condDB) {

  /////////////////////////////////////////////////////////////////////////////////////
  // This function is called during TRTDigitizationTool::lateInitialize(). It can be //
  // used to reset parameters according to the value of TRT_Dig_Vers in the condDB   //
  // /TRT/Cond/DigVers.                                                              //
  // At the moment DigSettingsFromCondDB() has no effect.                            //
  // At the time of writing (October 2013) dig_vers_from_condDB==12                //
  // which is now the same settings (2012/2013 re-tune) that are applied as default. //
  /////////////////////////////////////////////////////////////////////////////////////

  // std::cout << "digversion fron condDB: " << dig_vers_from_condDB << std::endl;
  if (dig_vers_from_condDB==12) {
    // the settings are default now
  } else {
    if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Error in settings / condDB" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::addPropertiesForOverrideableParameters( Algorithm * alg ) {

  std::map <std::string,doubleparameter>::iterator itd = m_doubleparMap.begin();
  std::map <std::string,doubleparameter>::iterator itdE = m_doubleparMap.end();

  for (;itd!=itdE;++itd) {
    alg->declareProperty(std::string("Override_")+itd->first, itd->second.valueSetByUser );
  }

  std::map <std::string,intboolparameter>::iterator itib = m_intboolparMap.begin();
  std::map <std::string,intboolparameter>::iterator itibE = m_intboolparMap.end();

  for (;itib!=itibE;++itib) {
    alg->declareProperty(std::string("Override_")+itib->first, itib->second.valueSetByUser );
  }
}

//_________________________________________________________________________________________________________
void TRTDigSettings::addPropertiesForOverrideableParameters( AlgTool * algt ) {

  std::map <std::string,doubleparameter>::iterator itd = m_doubleparMap.begin();
  std::map <std::string,doubleparameter>::iterator itdE = m_doubleparMap.end();

  for (;itd!=itdE;++itd) {
    algt->declareProperty(std::string("Override_")+itd->first, itd->second.valueSetByUser );
  }

  std::map <std::string,intboolparameter>::iterator itib = m_intboolparMap.begin();
  std::map <std::string,intboolparameter>::iterator itibE = m_intboolparMap.end();

  for (;itib!=itibE;++itib) {
    algt->declareProperty(std::string("Override_")+itib->first, itib->second.valueSetByUser );
  }

}

//_________________________________________________________________________________________________________
void TRTDigSettings::printFlagsForOverrideableParameters(const std::string& front) const {

  std::cout << front << "======= Printing TRT Digitization Settings Overrideable parameters ======="<<std::endl;

  std::map <std::string,doubleparameter>::const_iterator itd = m_doubleparMap.begin();
  std::map <std::string,doubleparameter>::const_iterator itdE = m_doubleparMap.end();

  for (;itd!=itdE;++itd) {
    std::cout << front << std::endl;
    std::cout << front << "Alg. Property: Override_"<<itd->first<<std::endl;
    std::cout << front << "  Type: Double"<<std::endl;
    std::cout << front << "  Range: ["<<itd->second.okrange_low/itd->second.unit<<" "
	      << itd->second.unitname<<", "
	      << itd->second.okrange_high/itd->second.unit<<" "<<itd->second.unitname<<"]"<<std::endl;
    std::cout << front << "  Description: "<<itd->second.description<<std::endl;
  }

  std::map <std::string,intboolparameter>::const_iterator itib = m_intboolparMap.begin();
  std::map <std::string,intboolparameter>::const_iterator itibE = m_intboolparMap.end();

  for (;itib!=itibE;++itib) {
    std::cout << front << std::endl;
    std::cout << front << "Alg. Property: Override_"<<itib->first<<std::endl;
    std::cout << front << "  Type: Int"<<std::endl;
    std::cout << front << "  Range: ["<<itib->second.okrange_low<<", "
	      << itib->second.okrange_high<<"]"<<std::endl;
    std::cout << front << "  Description: "<<itib->second.description<<std::endl;
  }

  std::cout << front <<std::endl;
  std::cout << front << "==============================================="<<std::endl;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::defineNewVariable(std::string name, double * datamember,std::string description,
					std::string unitname,double unitval,double lowrange,double highrange) {

  //sanity checks:
  if (m_intboolparMap.find(name)!=m_intboolparMap.end() || m_doubleparMap.find(name)!=m_doubleparMap.end()) {
    std::cout << "TRTDigSettings: Multiple definitions of "<<name<<std::endl; exit(1);
  }
  if (lowrange > highrange || lowrange*unitval<=m_propertyNotSetMagicNumber ) {
    std::cout << "TRTDigSettings: Problem in range of par "<<name<<std::endl; exit(1);
  }

  doubleparameter p;
  p.description = description;
  p.directvaraddress = datamember;
  p.unitname = unitname;
  p.unit = unitval;
  p.okrange_low = lowrange*unitval;
  p.okrange_high = highrange*unitval;
  p.valueSetByUser = m_propertyNotSetMagicNumber;

  m_doubleparMap[name] = p;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::defineNewUIntVariable(std::string name,unsigned int * datamember,std::string description,
			   unsigned int lowrange, unsigned int highrange) {
  //sanity checks:
  if (m_intboolparMap.find(name)!=m_intboolparMap.end() || m_doubleparMap.find(name)!=m_doubleparMap.end()) {
    std::cout << "TRTDigSettings: Multiple definitions of "<<name<<std::endl; exit(1);
  }
  if (lowrange > highrange ) {
    std::cout << "TRTDigSettings: Problem in range of par "<<name<<std::endl; exit(1);
  }

  intboolparameter p;

  p.description = description;
  p.directvaraddress_int = 0;
  p.directvaraddress_uint = datamember;
  p.directvaraddress_bool = 0;
  p.okrange_low = static_cast<int>(lowrange);
  p.okrange_high = static_cast<int>(highrange);
  p.valueSetByUser = m_propertyNotSetMagicNumber_int;

  m_intboolparMap[name] = p;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::defineNewIntVariable(std::string name,int * datamember,std::string description,
			  int lowrange, int highrange) {
  //sanity checks:
  if (m_intboolparMap.find(name)!=m_intboolparMap.end() || m_doubleparMap.find(name)!=m_doubleparMap.end()) {
    std::cout << "TRTDigSettings: Multiple definitions of "<<name<<std::endl; exit(1);
  }
  if (lowrange > highrange || lowrange<=m_propertyNotSetMagicNumber_int ) {
    std::cout << "TRTDigSettings: Problem in range of par "<<name<<std::endl; exit(1);
  }

  intboolparameter p;

  p.description = description;
  p.directvaraddress_int = datamember;
  p.directvaraddress_uint = 0;
  p.directvaraddress_bool = 0;
  p.okrange_low = lowrange;
  p.okrange_high = highrange;
  p.valueSetByUser = m_propertyNotSetMagicNumber_int;

  m_intboolparMap[name] = p;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::defineNewBoolVariable(std::string name,bool * datamember,std::string description) {
  //sanity checks:
  if (m_intboolparMap.find(name)!=m_intboolparMap.end() || m_doubleparMap.find(name)!=m_doubleparMap.end()) {
    std::cout << "TRTDigSettings: Multiple definitions of "<<name<<std::endl; exit(1);
  }

  intboolparameter p;

  p.description = description;
  p.directvaraddress_int = 0;
  p.directvaraddress_uint = 0;
  p.directvaraddress_bool = datamember;
  p.okrange_low = 0;
  p.okrange_high = 1;
  p.valueSetByUser = m_propertyNotSetMagicNumber_int;

  m_intboolparMap[name] = p;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::fillDefaults(const InDetDD::TRT_DetectorManager* detmgr) {

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Note that at the time of writing (October 2013), the defaults below correspond to:                   //
  //  1) detmgr m_digversion=11, digversionname=DC3version3 (collision data)                              //
  //  2) Late initialization with DigSettingsFromCondDB() fetching m_digversion=12 from the coniditons DB //
  // There is response to digver's except for a "throw" for values <11 as such MC is no longer supported. //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////

  InDetDD::TRT_DetectorManager::ActiveGasType activegastype = detmgr->gasType();
  m_digversion                                              = detmgr->digitizationVersion();
  std::string digversionname                                = detmgr->digitizationVersionName();

  std::cout << "TRTDigSettings::fillDefaults TRT digitization version: digversion = "
            << m_digversion << " " << digversionname << std::endl;

  // After TRT_Digitization-00-10-74 (end of Run1) we will no longer support m_digversion<11
  if (m_digversion<11) {
      if (msgLevel(MSG::FATAL)) msg(MSG::FATAL) << "digversion < 11 (" << m_digversion << ") is no longer supported. The job will die now :(" <<endmsg;
      throw;
  }

  bool gasok = false;
  if ( activegastype == InDetDD::TRT_DetectorManager::newgas ) gasok = true;

  if (!gasok) {
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Active gas setting seems incompatible with dig. version number."<<endmsg;
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "If not deliberate, it might indicate a configuration or DB problem."<<endmsg;
  }

  // miscellaneous
  m_solenoidFieldStrength = 2.0*CLHEP::tesla;

  // falses (unlikely to change)
  m_doCrosstalk = false;  //Crosstalk noise switched off by default (so far). Switch on by doCrosstalk() flag in j.o.
  m_crosstalkNoiseLevel = 0.01; // In cosmics: tuned to 0.01
  m_crosstalkNoiseLevelOtherEnd = 0.01;// In cosmics: tuned to 0.01
  m_doCosmicTimingPit = false;
  m_getT0FromData=false;
  m_killEndCap = false;
  m_killBarrel = false;
  m_cosmicFlag=0;
  m_isCTB = false;
  m_isOverlay=false;
  m_noiseInUnhitStraws = false;

  // trues
  m_timeCorrection = true;
  m_noiseInSimhits = true;
  m_electronicsAreAtFarEnd = true;
  m_timeshiftsSymmetricForPhiSectors = true;
  m_useAttenuation = true;
  m_useMagneticFieldMap = true;

  // output
  m_timeInterval = 75*CLHEP::ns;
  m_numberOfBins = 96;
  m_numberOfBinsInEncodingBin = 4;
  m_storeSDO = 2;
  m_numberOfCrossingsBeforeMain = 2; // allows migration of signal in readout window
  m_pileUpSDOsMinEkin = 0.0*CLHEP::MeV; //lowered for lowPt tracking

  // speed
  m_signalPropagationSpeed = 225.0*CLHEP::mm/CLHEP::ns;
  m_distanceToTimeFactor = 1.0;

  // time
  // Fred: It would seem to me that the timing base for both low and high hits could
  //       be slightly different for the A & C sides and it would be wise to allow
  //       for the possibility in the code [FIXME].
  // We need to tune the T0shift separately the endcap and the barrel [FIXME].
  m_overallT0ShiftShortBarrel = 0.0*CLHEP::ns;
  m_overallT0Shift            = 1.0*CLHEP::ns;
  m_minDiscriminatorWidth     = 1.1*CLHEP::ns;
  m_discriminatorSettlingTime = 1.1*CLHEP::ns;
  m_discriminatorDeadTime     = 6.0*CLHEP::ns;
  m_jitterTimeOffset          = 0.0*CLHEP::ns;

  // HT middle-bit fraction tune; KyungEon.Choi@cern.ch
  // https://indico.cern.ch/event/389682/contribution/5/material/slides/0.pdf
  m_htT0shiftBarShort  = -6; // This is a delta shift w.r.t m_overallT0Shift (steps of 0.78125 ns).
  m_htT0shiftBarLong   = -6; // It affects only HL threshold timing. The purpose is to
  m_htT0shiftECAwheels = -6; // tune the middle HT bit fraction so that HT probability
  m_htT0shiftECBwheels = -6; // can be based on the middle bit only at high occupancy.

  // length
  m_strawLengthBarrel  = 1425.5*CLHEP::mm;
  m_innerRadiusEndcap  = 621.18*CLHEP::mm;
  m_outerRadiusEndcap  = 1067.0*CLHEP::mm;
  m_innerRadiusOfStraw =    2.0*CLHEP::mm;
  m_outerRadiusOfWire  = 0.0155*CLHEP::mm;
  m_lengthOfDeadRegion =    3.0*CLHEP::mm;
  m_attenuationLength  =   400.*CLHEP::cm;
  m_maxVertexDisplacement = 0.0;           // units!?
  m_timeOffsetCalcVertexX = 0.0*CLHEP::cm; // units!?
  m_timeOffsetCalcVertexY = 0.0*CLHEP::cm; // units!?
  m_timeOffsetCalcVertexZ = 0.0*CLHEP::cm; // units!?

  // (Xenon)
  // HT fine-tune on 2011 data tagged as 00-11-07
  // HT middle-bit fraction tune - wider shaping function; 01-00-24
  // Delta-ray suppression tune tagged as 01-01-03
  // Delta-ray suppression tune with backward compatibility with non suppressed delta-ray simulation tagged as 01-01-07
  // Delta-ray suppression HT middle-bit fraction tune - 2015 data; 01-01-16
  m_trtRangeCutProperty = m_doubleparMap["TrtRangeCutProperty"].valueSetByUser;//To avoid overwritting warning message and to use python configured value
  if(fabs(m_trtRangeCutProperty-0.05) >= std::numeric_limits<double>::epsilon()){
    m_lowThresholdBar        = 0.260*CLHEP::keV;
    m_lowThresholdEC         = 0.275*CLHEP::keV;
    m_highThresholdBarShort  = 5.195*CLHEP::keV;
    m_highThresholdBarLong   = 4.751*CLHEP::keV;
    m_highThresholdECAwheels = 5.513*CLHEP::keV;
    m_highThresholdECBwheels = 5.326*CLHEP::keV;
    m_trEfficiencyBarrel = 0.774;
    m_trEfficiencyEndCapA = 0.909;
    m_trEfficiencyEndCapB = 0.809;
  }
  else {
    m_lowThresholdBar        = 0.260*CLHEP::keV;
    m_lowThresholdEC         = 0.275*CLHEP::keV;
    m_highThresholdBarShort  = 6.576*CLHEP::keV;
    m_highThresholdBarLong   = 6.016*CLHEP::keV;
    m_highThresholdECAwheels = 6.390*CLHEP::keV;
    m_highThresholdECBwheels = 6.074*CLHEP::keV;
    m_trEfficiencyBarrel = 0.95;
    m_trEfficiencyEndCapA = 1.00;
    m_trEfficiencyEndCapB = 1.00;
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Setting up non suppressed double counted delta-ray xenon tune"<<endmsg;
  }

  // (Argon) Initial tuning by Artem July 2014. See log file. Requires fine tuning.
  // HT middle-bit fraction tune - wider shaping function; 01-00-24
  // HT middle-bit fraction tune - 2015 data; 01-01-16
  // Argon LT tune to 2015 data; 01-02-06
  m_lowThresholdBarArgon        = 0.150*CLHEP::keV;
  m_lowThresholdECArgon         = 0.150*CLHEP::keV;
  m_highThresholdBarShortArgon  = 2.607*CLHEP::keV;
  m_highThresholdBarLongArgon   = 2.540*CLHEP::keV;
  m_highThresholdECAwheelsArgon = 2.414*CLHEP::keV;
  m_highThresholdECBwheelsArgon = 2.295*CLHEP::keV;
  m_trEfficiencyBarrelArgon = 0.61;
  m_trEfficiencyEndCapAArgon = 0.80;
  m_trEfficiencyEndCapBArgon = 0.80;

  // (Krypton)
  // Initial implementation in May 2015 - guess; 01-01-00
  // Tuning from 2015 data by Kevin in April 2016, no LT tune; 01-02-01
  m_lowThresholdBarKrypton = 0.140*CLHEP::keV;
  m_lowThresholdECKrypton  = 0.150*CLHEP::keV;
  m_highThresholdBarShortKrypton  = 3.07*CLHEP::keV;
  m_highThresholdBarLongKrypton   = 2.90*CLHEP::keV;
  m_highThresholdECAwheelsKrypton = 3.15*CLHEP::keV;
  m_highThresholdECBwheelsKrypton = 3.02*CLHEP::keV;
  m_trEfficiencyBarrelKrypton = 0.49;
  m_trEfficiencyEndCapAKrypton = 0.68;
  m_trEfficiencyEndCapBKrypton = 0.68;

  // Noise
  m_fastElectronicsNoisePulseDistance = 1.0*CLHEP::ns;
  m_slowPeriodicNoisePulseDistance   = 25.0*CLHEP::ns;
  m_slowPeriodicNoisePulseFraction   = 0.6;
  m_averageNoiseLevel = 0.02;
  m_relativeLowThresholdFluctuation = 0.0;
  m_relativeHighThresholdFluctuation = 0.05;

  // Clusters
  // Note: smearingFactor is a possible candidate for tuning drift time accuracy
  m_smearingFactor        = 0.4;
  m_smearingFactorArgon   = 0.4;
  m_smearingFactorKrypton = 0.4;
  m_ionisationPotential        = 26.0*CLHEP::eV;
  m_ionisationPotentialArgon   = 28.3*CLHEP::eV; // according to thesis of Peter Cwetanski (TABLE 4-I)
  m_ionisationPotentialKrypton = 28.3*CLHEP::eV;

}

//_________________________________________________________________________________________________________
void TRTDigSettings::processOverrides() {

  bool anyoverrides = false;
  std::map <std::string,doubleparameter>::iterator itd = m_doubleparMap.begin();
  std::map <std::string,doubleparameter>::iterator itdE = m_doubleparMap.end();

  for (;itd!=itdE;++itd) {
    if (itd->second.valueSetByUser != m_propertyNotSetMagicNumber) {
      if (itd->second.valueSetByUser < itd->second.okrange_low || itd->second.valueSetByUser > itd->second.okrange_high) {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can not override value of "<<itd->first<<" : New value outside allowed range" << endmsg;
      } else {
        if ( static_cast<float>(*(itd->second.directvaraddress)) != static_cast<float>(itd->second.valueSetByUser) ) {
          if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Overriding "<<itd->first<<" flag ("
                                                        << (*(itd->second.directvaraddress))/itd->second.unit<<" "<<itd->second.unitname<<" -> "
                                                        << itd->second.valueSetByUser/itd->second.unit<<" "<<itd->second.unitname<<")"<<endmsg;
          *(itd->second.directvaraddress) = itd->second.valueSetByUser;
          anyoverrides = true;
        }
      }
    }
  }

  std::map <std::string,intboolparameter>::iterator itib = m_intboolparMap.begin();
  std::map <std::string,intboolparameter>::iterator itibE = m_intboolparMap.end();

  for (;itib!=itibE;++itib) {
    if (itib->second.valueSetByUser != m_propertyNotSetMagicNumber_int) {
      if (itib->second.valueSetByUser < itib->second.okrange_low || itib->second.valueSetByUser > itib->second.okrange_high) {
	if (msgLevel(MSG::ERROR)) msg(MSG::ERROR) << "Can not override value of "<<itib->first<<" : New value outside allowed range" << endmsg;
      } else {
	if (itib->second.directvaraddress_int) {
	  //int
          if ( (*(itib->second.directvaraddress_int)) != itib->second.valueSetByUser ) {
	    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Overriding "<<itib->first<<" flag ("
	  						  << *(itib->second.directvaraddress_int)<<" -> "<< itib->second.valueSetByUser<<")"<<endmsg;
	    *(itib->second.directvaraddress_int) = itib->second.valueSetByUser;
            anyoverrides = true;
          }
	} else if (itib->second.directvaraddress_uint) {
	  //unsigned int
         if ( (*(itib->second.directvaraddress_uint)) != static_cast<unsigned int>(itib->second.valueSetByUser) ) {
	    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Overriding "<<itib->first<<" flag ("
						 	  << *(itib->second.directvaraddress_uint)<<" -> "<< itib->second.valueSetByUser<<")"<<endmsg;
	    *(itib->second.directvaraddress_uint) = itib->second.valueSetByUser;
            anyoverrides = true;
          }
	} else {
	  //bool
	  assert(itib->second.directvaraddress_bool);
          if ( (*(itib->second.directvaraddress_bool)) != itib->second.valueSetByUser ) {
	    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Overriding "<<itib->first<<" flag ("
							  << (*(itib->second.directvaraddress_bool)?1:0)<<" -> "
							  << (itib->second.valueSetByUser?1:0)<<")"<<endmsg;
	    *(itib->second.directvaraddress_bool) = itib->second.valueSetByUser == 1;
            anyoverrides = true;
	  }
	}
      }
    }
  }

  if (anyoverrides)
    if (msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Settings overridden from joboptions => possible deviation from version defaults." << endmsg;

}
