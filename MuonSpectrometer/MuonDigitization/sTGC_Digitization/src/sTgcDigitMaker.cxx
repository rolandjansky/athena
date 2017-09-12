////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------
// sTgcDigitMaker.cxx
//-----------------------------------------------------
//
// Authors:  Nectarios Benekos  <nectarios.benekos@cern.ch>
//           Jiaming Yu  <jiaming.yu@cern.ch>  
////////////////////////////////////////////////////////////////////////////////


 
#include "sTGC_Digitization/sTgcDigitMaker.h"

#include <iostream>
#include <fstream>

#include "MuonDigitContainer/sTgcDigitCollection.h"
#include "MuonSimEvent/GenericMuonSimHit.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

#include "GaudiKernel/MsgStream.h"
#include "PathResolver/PathResolver.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TF1.h" 
#include "TMath.h"

//---------------------------------------------------
//  Constructor and Destructor
//---------------------------------------------------

//----- Constructor
sTgcDigitMaker::sTgcDigitMaker(sTgcHitIdHelper* hitIdHelper, const MuonGM::MuonDetectorManager* mdManager)
{
  m_digits                  = 0;
  m_engine                  = 0;
  m_hitIdHelper             = hitIdHelper;
  m_mdManager               = mdManager;
  m_idHelper                = 0;
  m_efficiencyOfWireGangs   = 1.000; // 100% efficiency for sTGCSimHit_p1
  m_efficiencyOfStrips      = 1.000; // 100% efficiency for sTGCSimHit_p1
  m_doTimeCorrection        = true;
  m_timeWindowOffsetPad     = 0.;
  m_timeWindowOffsetStrip   = 25.;
  //m_timeWindowPad          = 30.; // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  //m_timeWindowStrip         = 30.; // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  //m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
  //m_timeJitterElectronicsStrip   = 2.; // 2ns jitter of electronics from the threshold to peak
  //m_timeJitterElectronicsPad     = 2.; // 2ns jitter of electronics for passing the threshold
  m_GausMean                = 2.27;  //mm; VMM response from Oct/Nov 2013 test beam
  m_GausSigma               = 0.1885;//mm; VMM response from Oct/Nov 2013 test beam
  m_IntegralTimeOfElectr    = 20.00; // ns
  m_CrossTalk               = 0.03; 
  m_noiseFactor             = 0.;
  m_channelTypes            = 3; // 1 -> strips, 2 -> strips+pad, 3 -> strips/wires/pads
}

//----- Destructor
sTgcDigitMaker::~sTgcDigitMaker()
{}
//------------------------------------------------------
// Initialize
//------------------------------------------------------
StatusCode sTgcDigitMaker::initialize(CLHEP::HepRandomEngine *rndmEngine, const int channelTypes)
{
  // Initialize TgcIdHelper
  if(!m_hitIdHelper) {
    m_hitIdHelper = sTgcHitIdHelper::GetHelper();
  }

  ////set the flag of timeCorrection
  //m_doTimeCorrection = doTimeCorrection;

  //set the flag of channelTypes which will be digitized
  m_channelTypes = channelTypes; 

  // initialize the TGC identifier helper
  m_idHelper = m_mdManager->stgcIdHelper();

  readFileOfTimeJitter();

  // getting our random numbers stream
  m_engine = rndmEngine;

  // Read share/sTGC_Digitization_energyThreshold.dat file and store values in m_energyThreshold.
  readFileOfEnergyThreshold();

  //// Read share/sTGC_Digitization_crossTalk.dat file and store values in m_crossTalk.
  //readFileOfCrossTalk();

  // Read share/sTGC_Digitization_deadChamber.dat file and store values in m_isDeadChamber.
  readFileOfDeadChamber();

  // Read share/sTGC_Digitization_timeWindowOffset.dat file and store values in m_timeWindowOffset.
  readFileOfTimeWindowOffset();

  //// Read share/sTGC_Digitization_alignment.dat file and store values in m_alignmentZ, m_alignmentT, m_alignmentS, m_alignmentTHS
  //readFileOfAlignment();

  return StatusCode::SUCCESS;
}

//---------------------------------------------------
// Execute Digitization
//---------------------------------------------------
sTgcDigitCollection* sTgcDigitMaker::executeDigi(const GenericMuonSimHit* hit, const float /*globalHitTime*/)
{ 

  // check the digitization channel type
  if(m_channelTypes!=1 && m_channelTypes!=2 && m_channelTypes!=3){
    ATH_MSG_ERROR("Invalid ChannelTypes : " << m_channelTypes << " (valid values are : 1 --> strips ; 2 --> strips / wires ; 3 --> strips / wires / pads)"); 
  }

  // SimHits without energy loss are not recorded.
  double energyDeposit = hit->depositEnergy(); // Energy deposit in MeV 
  if(energyDeposit==0.) return 0;

  //////////  convert ID for this digitizer system 
  sTgcSimIdToOfflineId simToOffline(*m_idHelper);  
  int simId = hit->GenericId();
  Identifier layid = simToOffline.convert(simId);
  ATH_MSG_VERBOSE("sTgc hit:  time " << hit->globalTime() << " position " << hit->globalPosition().x() << "  " << hit->globalPosition().y() << "  " << hit->globalPosition().z() << " mclink " << hit->particleLink() << " PDG ID " << hit->particleEncoding() );

  std::string stName = m_idHelper->stationNameString(m_idHelper->stationName(layid));
  int isSmall = stName[2] == 'S';

  ATH_MSG_DEBUG("Retrieving detector element for: isSmall " << isSmall << " eta " << m_idHelper->stationEta(layid) << " phi " << m_idHelper->stationPhi(layid) << " ml " << m_idHelper->multilayer(layid) << " energyDeposit "<<energyDeposit );

  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(layid);
  if( !detEl ){
    ATH_MSG_WARNING("Failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_idHelper->stationEta(layid) << " phi " << m_idHelper->stationPhi(layid) << " ml " << m_idHelper->multilayer(layid) );
    return 0;
  }
 
  // DO THE DIGITIZATTION HERE ////////

  //#################################################################################
  //############### find the detectorElement and check efficiency ###################
  //#################################################################################

  std::string stationName= m_idHelper->stationNameString(m_idHelper->stationName(layid));
  int stationEta = m_idHelper->stationEta(layid);
  int stationPhi  = m_idHelper->stationPhi(layid);
  int multiPlet = m_idHelper->multilayer(layid);
  int gasGap = m_idHelper->gasGap(layid);

  //// Check the chamber is dead or not.
  if(isDeadChamber(stationName, stationEta, stationPhi, multiPlet, gasGap)) return 0;

  //***************************** check effeciency ******************************** 
  // use energyDeposit to implement detector effeciency 
  if(!efficiencyCheck(stationName, stationEta, stationPhi, multiPlet, gasGap, 1, energyDeposit)) return 0; 
  
  IdentifierHash coll_hash;
  // contain (name, eta, phi, multiPlet)
  m_idHelper->get_detectorElement_hash(layid, coll_hash);
  //ATH_MSG_DEBUG(" looking up collection using hash " << (int)coll_hash << " " << m_idHelper->print_to_string(layid) );

  m_digits = new sTgcDigitCollection(layid, coll_hash);

  bool isValid = 0;


  // #################################################################
  // get the GlobalPosition (propergated to the wire surface)
  // #################################################################

  Amg::Vector3D GPOS(hit->globalPosition().x(),hit->globalPosition().y(),hit->globalPosition().z());
  const Amg::Vector3D GLODIRE(hit->globalDirection().x(), hit->globalDirection().y(), hit->globalDirection().z());

  // get strip surface 
  int surfHash_strip =  detEl->surfaceHash(gasGap, 1);
  const Trk::PlaneSurface& SURF_STRIP = detEl->surface(surfHash_strip); // get the strip surface


  // #################################################################
  //***************************** BCTAGGER ******************************** 
  // #################################################################
  //m_timeWindowOffsetPad  = getTimeWindowOffset(stationName, stationEta, 2);
  //m_timeWindowOffsetStrip = getTimeWindowOffset(stationName, stationEta, 1);

  // use energyDeposit to implement detector effeciency 
  // Time of flight correction for each chamber
  // the offset is set to 0 for ASD located at the postion where tof is minimum in each chamber,
  // i.e. the ASD at the smallest R in each chamber
  //float tofCorrection = hit->globalPosition().mag()/CLHEP::c_light; //FIXME use CLHEP::c_light
  //float tofCorrection = globalHitTime; 

  //// bunch time
  //float bunchTime = globalHitTime - tofCorrection;

  Trk::LocalDirection LocDirection;
  SURF_STRIP.globalToLocalDirection(GLODIRE, LocDirection);

  float inAngle_space = fabs( LocDirection.angleXZ() / CLHEP::degree);
  float inAngle_time = fabs( LocDirection.angleYZ() / CLHEP::degree);

  if(inAngle_time > 90)  inAngle_time  = inAngle_time  -90.; 
  if(inAngle_space > 90) inAngle_space = inAngle_space -90.; 

  static const float jitterInitial = 9999.;
  float timeJitterDetector = jitterInitial; // calculated at central strip but also used in all the strips fired by the same hit 
  if(timeJitterDetector > jitterInitial-0.1) {
    timeJitterDetector = timeJitter(inAngle_time);
  }

  //float timeJitterElectronicsStrip = CLHEP::RandGauss::shoot(m_engine, 0, m_timeJitterElectronicsStrip);
  //float timeJitterElectronicsPad = CLHEP::RandGauss::shoot(m_engine, 0, m_timeJitterElectronicsPad);

  //const float stripPropagationTime = 3.3*CLHEP::ns/CLHEP::m * detEl->distanceToReadout(posOnSurf_strip, elemId); // 8.5*ns/m was used until MC10. 
  const float stripPropagationTime = 0.; // 8.5*ns/m was used until MC10. 

  float sDigitTimePad = timeJitterDetector + m_timeWindowOffsetPad;
  float sDigitTimeStrip = timeJitterDetector + m_timeWindowOffsetStrip + stripPropagationTime;


  //if(m_doTimeCorrection) sDigitTime = bunchTime + timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //else {
  //  sDigitTime = timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //  ATH_WARNING("Did not do Time Correction for this hit!");
  //}

  uint16_t bctag = 0;

  //if(sDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetStrip || +m_bunchCrossingTime+m_timeWindowOffsetStrip+ m_timeWindowStrip < sDigitTime) {
  //    ATH_MSG_DEBUG("Strip: Digitized time is outside of time window. " << sDigitTime
  //      	      << " bunchTime: " << bunchTime 
  //                    << " time jitter from detector response : " << timeJitterDetector  
  //                    << " time jitter from electronics response : " << timeJitterElectronics  
  //      	      << " propagation time: " << stripPropagationTime )
  //  return m_digits;
  //}

  //currently do not give any bcId information, just give the acurate time
  //bctag = bcTagging(sDigitTime, channelType);


  //##################################################################################
  //######################################### strip readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::strip response ");
  int channelType = 1;

  Identifier newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1, true);

  Amg::Vector3D hitOnSurface_strip = SURF_STRIP.transform().inverse()*GPOS;
  Amg::Vector2D posOnSurf_strip(hitOnSurface_strip.x(),hitOnSurface_strip.y());
  bool insideBounds = SURF_STRIP.insideBounds(posOnSurf_strip);
  if(!insideBounds) { 
    ATH_MSG_DEBUG("Outside of the strip surface boundary : " <<  m_idHelper->print_to_string(newId) << "; local position " <<posOnSurf_strip ); 
    if(m_digits) {delete m_digits; m_digits = 0;}
    return 0;
  }

  //************************************ find the nearest readout element ************************************** 
       
  int stripNumber = detEl->stripNumber(posOnSurf_strip, newId);
  if( stripNumber == -1 ){
    ATH_MSG_ERROR("Failed to obtain strip number " << m_idHelper->print_to_string(newId) );
    ATH_MSG_ERROR(" pos " << posOnSurf_strip );
    //stripNumber = 1;
  }  
  newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripNumber, true, &isValid);
  if(!isValid && stripNumber != -1) {
    ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    if(m_digits) {delete m_digits; m_digits = 0;}
    return 0;
  }

  int NumberOfStrips = detEl->numberOfStrips(newId); 

  //************************************ spread charge among readout element ************************************** 
  //spread charge to a gaussian distribution
  float charge_width = CLHEP::RandGauss::shoot(m_engine, m_GausMean, m_GausSigma);
  float norm = 1000. * energyDeposit/(charge_width*TMath::Sqrt(2.*TMath::Pi())); //normalization: 1Kev --> Intergral=1
  TF1 *charge_spread = new TF1("fgaus", "gaus(0)", -1000., 1000.); 
  charge_spread->SetParameters(norm, posOnSurf_strip.y(), charge_width);
  
  m_noiseFactor = getNoiseFactor(inAngle_space);

  //ATH_MSG_VERBOSE(" New hit " << m_idHelper->print_to_string(newId) << " from truth " << hitOnSurface_strip << "Edep " << 1000.*energyDeposit << "KeV" );
  //ATH_MSG_VERBOSE(" charge_spread : posOnSurf_strip.x() =  " << posOnSurf_strip.x() );

  int stripnum = -1;
  for(int neighbor=0; neighbor<=3; neighbor++){  // spread the charge to 7 strips for the current algorithm
    stripnum = stripNumber+neighbor;
    if(stripnum>=1&&stripnum<=NumberOfStrips){
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum, true, &isValid);
      if(!isValid) continue;

      Amg::Vector2D locpos(0,0);
      if( !detEl->stripPosition(newId,locpos)){
        ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelper->print_to_string(newId) );
      }
      //ATH_MSG_VERBOSE(" New hit " << m_idHelper->print_to_string(newId) << " locpos " << locpos << " from truth " << hitOnSurface_strip << "Edep " << 1000.*energyDeposit << "KeV" );
      ////??? waiting for Sharka's update
      //const MuonStripDesign design = detEl->getDesign(newId);
      //if( !design ) {
      //        ATH_MSG_WARNING("Failed to obtain strip design for identifier " 
      //        << m_idHelper->print_to_string(newId) );
      //}
      //double stripHalfWidth = design->StripWidth() / 2.;
      float stripHalfWidth = 2.7 / 2.; 
      float xmax = locpos.y() + stripHalfWidth;
      float xmin = locpos.y() - stripHalfWidth;
      float charge = charge_spread->Integral(xmin, xmax);
      charge = CLHEP::RandGauss::shoot(m_engine, charge, m_noiseFactor*charge);

      //ATH_MSG_VERBOSE(" nearby strip " << m_idHelper->print_to_string(newId) << "locpos.x = " << locpos.x() << "  locpos.y = " << locpos.y() );

      addDigit(newId, bctag, sDigitTimeStrip, charge, channelType);

      //************************************** introduce cross talk ************************************************
      for(int crosstalk=1; crosstalk<=3; crosstalk++){ // up to the third nearest neighbors 
        if((stripnum-crosstalk)>=1&&(stripnum-crosstalk)<=NumberOfStrips){
          newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum-crosstalk, true, &isValid);
          if(!isValid) continue;
          addDigit(newId, bctag, sDigitTimeStrip, charge*TMath::Power(m_CrossTalk, crosstalk), channelType);
        } 
        if((stripnum+crosstalk)>=1&&(stripnum+crosstalk)<=NumberOfStrips){
          newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum+crosstalk, true, &isValid);
          if(!isValid) continue;
          addDigit(newId, bctag, sDigitTimeStrip, charge*TMath::Power(m_CrossTalk, crosstalk), channelType);
        } 
      }// end of introduce cross talk
    }// end of when stripnum = stripNumber+neighbor
 
    if(neighbor==0) continue;
    stripnum = stripNumber-neighbor; 
    if(stripnum>=1&&stripnum<=NumberOfStrips){
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum);

      Amg::Vector2D locpos(0,0);
      if( !detEl->stripPosition(newId,locpos)){
        ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelper->print_to_string(newId) );
      }
      ////??? waiting for Sharka's update
      //const MuonStripDesign* design = getDesign(newId);
      //if( !design ) {
      //        ATH_MSG_WARNING("Failed to obtain strip design for identifier " 
      //        << m_idHelper->print_to_string(newId) );
      //}
      //double stripHalfWidth = design->StripWidth() / 2.;
      float stripHalfWidth = 2.7 / 2.; 
      float xmax = locpos.y() + stripHalfWidth;
      float xmin = locpos.y() - stripHalfWidth;
      float charge = charge_spread->Integral(xmin, xmax);
      charge = CLHEP::RandGauss::shoot(m_engine, charge, m_noiseFactor*charge);

      addDigit(newId, bctag, sDigitTimeStrip, charge, channelType);

      //************************************** introduce cross talk ************************************************
      for(int crosstalk=1; crosstalk<=3; crosstalk++){ // up to the third nearest neighbors 
        if((stripnum-crosstalk)>=1&&(stripnum-crosstalk)<=NumberOfStrips){
          newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum-crosstalk);
          addDigit(newId, bctag, sDigitTimeStrip, charge*TMath::Power(m_CrossTalk, crosstalk), channelType);
        } 
        if((stripnum+crosstalk)>=1&&(stripnum+crosstalk)<=NumberOfStrips){
          newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum+crosstalk);
          addDigit(newId, bctag, sDigitTimeStrip, charge*TMath::Power(m_CrossTalk, crosstalk), channelType);
        } 
      }// end of introduce cross talk
    }// end of when stripnum = stripNumber-neighbor

  }//end for spread the charge to 5 strips 

  delete charge_spread;
  // end of strip digitization

  if(m_channelTypes==1) {
    ATH_MSG_WARNING("Only digitize strip response !");
    return m_digits;
  }

  //##################################################################################
  //######################################### pad readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::pad response ");
  channelType = 0;
  
  //************************************ find the nearest readout element ************************************** 
  int  surfHash_pad =  detEl->surfaceHash(gasGap, 0);
  const Trk::PlaneSurface& SURF_PAD = detEl->surface(surfHash_pad); // get the pad surface

  Amg::Vector3D hitOnSurface_pad = SURF_PAD.transform().inverse()*GPOS;
  Amg::Vector2D posOnSurf_pad(hitOnSurface_pad.x(), hitOnSurface_pad.y());

  Identifier PAD_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, 0, 1, true);// find the a pad id

  insideBounds = SURF_PAD.insideBounds(posOnSurf_pad);

  if(insideBounds) { 
    int padNumber = detEl->stripNumber(posOnSurf_pad, PAD_ID);
    if( padNumber == -1 ){
      ATH_MSG_ERROR("Failed to obtain pad number " << m_idHelper->print_to_string(PAD_ID) );
      ATH_MSG_ERROR(" pos " << posOnSurf_pad );
      //padNumber = 1;
    }  
    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, padNumber, true, &isValid);
    if(isValid) {  
      addDigit(newId, bctag, sDigitTimePad, channelType);
    }
    else if(padNumber != -1) {  
      ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    }
  }
  else {
    ATH_MSG_DEBUG("Outside of the pad surface boundary :" << m_idHelper->print_to_string(PAD_ID)<< " local position " <<posOnSurf_pad ); 
  }
  
  if(m_channelTypes==2) {
    ATH_MSG_WARNING("Only digitize strip/pad response !");
    return m_digits;
  }
   
 
  //##################################################################################
  //######################################### wire readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::wire response ");
  channelType = 2;
  if( abs(m_idHelper->stationEta(layid)) >= 3 ) {

    Identifier WIRE_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1, true);// assign a invalid wireNumber first, sTgcReadoutElement::wireNumber will return the correct wireNumber 
        
    //************************************ find the nearest readout element ************************************** 
    int surfHash_wire =  detEl->surfaceHash(gasGap, 2);
    const Trk::PlaneSurface& SURF_WIRE = detEl->surface(surfHash_wire); // get the wire surface

    Amg::Vector3D hitOnSurface_wire = SURF_WIRE.transform().inverse()*GPOS;
    Amg::Vector2D posOnSurf_wire(hitOnSurface_wire.x(), hitOnSurface_wire.y());

    insideBounds = SURF_WIRE.insideBounds(posOnSurf_wire);

    if(insideBounds) {
      int wireNumber = detEl->stripNumber(posOnSurf_wire, WIRE_ID);
      if( wireNumber == -1 ){
        ATH_MSG_ERROR("Failed to obtain wire number " << m_idHelper->print_to_string(WIRE_ID) );
        ATH_MSG_ERROR(" pos " << posOnSurf_wire );
        //wireNumber = 1;
      }  
  
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, wireNumber, true, &isValid);
  
      if(isValid) {  
        int NumberOfWires = detEl->numberOfStrips(newId); // 0 --> pad, 1 --> strip, 2 --> wire 
        if(wireNumber>=1&&wireNumber<=NumberOfWires) addDigit(newId, bctag, sDigitTimePad, channelType);
      } // end of if(isValid)
      else if (wireNumber != -1){  
        ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
      }
    }
    else {
      ATH_MSG_DEBUG("Outside of the wire surface boundary :" << m_idHelper->print_to_string(WIRE_ID)<< " local position " <<posOnSurf_wire ); 
    }
  } // end of wire digitization 

  return m_digits;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfTimeJitter()
{

  const char* const fileName = "sTGC_Digitization_timejitter.dat";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  std::ifstream ifs;
  if (fileWithPath != "") {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not find file " << fileName );
    exit(-1);
  }

  if(ifs.bad()){
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not open file "<< fileName );
    exit(-1);
  }

  int angle = 0;
  int bins = 0;
  int i = 0;
  float prob = 0.;

  while(ifs.good()){
    ifs >> angle >> bins;
    if (ifs.eof()) break;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): Timejitter, angle, Number of bins, prob. dist.: " << angle << " " << bins << " ";
    m_vecAngle_Time.resize(i + 1);
    for (int j = 0; j < 41/*bins*/; j++) {
      ifs >> prob;
      m_vecAngle_Time[i].push_back(prob);
      if (j == 0)
        if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): ";
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << prob << " ";
    }
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endmsg;
    i++;
  }
  ifs.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
float sTgcDigitMaker::getNoiseFactor(float inAngle_space) const
{
  float noiseFactor = 0.0;
  if     (inAngle_space<10) noiseFactor = 0.003*inAngle_space + 0.04;
  else if(inAngle_space<20) noiseFactor = 0.002*inAngle_space + 0.05;
  else                      noiseFactor = 0.09;
 
  return noiseFactor;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
float sTgcDigitMaker::timeJitter(float inAngle_time) const
{

  int   ithAngle = static_cast<int>(inAngle_time/5.);
  float wAngle = inAngle_time/5. - static_cast<float>(ithAngle);
  int   jthAngle;
  if (ithAngle > 11) {
    ithAngle = 12;
    jthAngle = 12;
  }
  else {
    jthAngle = ithAngle+1;
  }

  float jitter;
  float prob = 1.;
  float probRef = 0.;

  while (prob > probRef) {
    prob   = CLHEP::RandFlat::shoot(m_engine, 0.0, 1.0);
    jitter = CLHEP::RandFlat::shoot(m_engine, 0.0, 1.0)*40.; // trial time jitter in nsec
    int ithJitter = static_cast<int>(jitter);
    // probability distribution calculated from weighted sum between neighboring bins of angles
    probRef = (1.-wAngle)*m_vecAngle_Time[ithAngle][ithJitter]
      +    wAngle *m_vecAngle_Time[jthAngle][ithJitter];
  }

  ATH_MSG_VERBOSE("sTgcDigitMaker::timeJitter : angle = " << inAngle_time 
                  << ";  timeJitterDetector = " << jitter );

  return jitter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::efficiencyCheck(const int channelType) const {
  if(channelType == 0) { // wire group
    if(CLHEP::RandFlat::shoot(m_engine,0.0,1.0) < m_efficiencyOfWireGangs) return true;
  }
  else if(channelType == 1) { // strip
    if(CLHEP::RandFlat::shoot(m_engine,0.0,1.0) < m_efficiencyOfStrips) return true;
  }
  ATH_MSG_DEBUG("efficiencyCheck(): Hit removed. channelType: " << channelType );
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::efficiencyCheck(const std::string stationName, const int stationEta, const int stationPhi,const int multiPlet, const int gasGap, const int channelType, const double energyDeposit) const {
  // If the energy deposit is equal to or greater than the threshold value of the chamber,
  // return true.
  return (energyDeposit >= getEnergyThreshold(stationName, stationEta, stationPhi, multiPlet, gasGap, channelType));
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//uint16_t sTgcDigitMaker::bcTagging(const float digitTime, const int channelType) const {
//
//  uint16_t bctag = 0;
//
//  double offset, window;
//  if(channelType == 1) { //strips 
//    offset = m_timeWindowOffsetStrip;
//    window = m_timeWindowStrip;
//  }
//  else { // wire gangs or pads
//    offset = m_timeWindowOffsetPad;
//    window = m_timeWindowPad;
//  }
//
//  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x1);
//  }
//  if(                     offset < digitTime && digitTime <                      offset+window) {
//    bctag = (bctag | 0x2);
//  }
//  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x4);
//  }
//
//  return bctag;
//}
//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::addDigit(const Identifier id, const uint16_t bctag, const float digittime, int channelType) {

  if(channelType!=0&&channelType!=2) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  //sTgcDigit* multihitDigit = 0;
  //ATH_MSG_DEBUG( "sTgcDigitMaker::addDigit"
  //      	    << " id = " << id
  //      	    << " bctag  = " << bctag
  //      	    << " channelType  = " << channelType );
   
  //if((bctag & 0x1) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_PREVIOUS==(*it)->bcTag()) {
  //      duplicate = true;
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_PREVIOUS);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_PREVIOUS));
  //  }
  //}
  //if((bctag & 0x2) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_CURRENT==(*it)->bcTag()) {
  //      duplicate = true;
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_CURRENT);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_CURRENT));
  //  }
  //}
  //if((bctag & 0x4) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_NEXT==(*it)->bcTag()) {
  //      duplicate = true;
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_NEXT);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_NEXT));
  //  }
  //}

  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    m_digits->push_back(new sTgcDigit(id, bctag, digittime, -1, 0, 0));
  }

  return;
}

void sTgcDigitMaker::addDigit(const Identifier id, const uint16_t bctag, const float digittime, float charge, int channelType) {

  if(channelType!=1) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  //sTgcDigit* multihitDigit = 0;
  //ATH_MSG_DEBUG( "sTgcDigitMaker::addDigit"
  //      	    << " id = " << id
  //      	    << " bctag  = " << bctag
  //      	    << " charge = " << charge
  //      	    << " channelType  = " << channelType );
   
  //if((bctag & 0x1) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_PREVIOUS==(*it)->bcTag()) {
  //      duplicate = true;
  //      (*it)->set_charge(charge+(*it)->charge());  
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_PREVIOUS, charge);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_PREVIOUS, charge));
  //  }
  //}
  //if((bctag & 0x2) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_CURRENT==(*it)->bcTag()) {
  //      (*it)->set_charge(charge+(*it)->charge());  
  //      duplicate = true;
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_CURRENT, charge);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_CURRENT, charge));
  //  }
  //}
  //if((bctag & 0x4) != 0) {
  //  for(sTgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
  //    if(id==(*it)->identify() && sTgcDigit::BC_NEXT==(*it)->bcTag()) {
  //      (*it)->set_charge(charge+(*it)->charge());  
  //      duplicate = true;
  //      break;
  //    }
  //  }
  //  if(!duplicate) {
  //    //multihitDigit = new sTgcDigit(id,sTgcDigit::BC_NEXT, charge);
  //    //m_digits->push_back(multihitDigit);
  //    m_digits->push_back(new sTgcDigit(id,sTgcDigit::BC_NEXT, charge));
  //  }
  //}

  for(sTgcDigitCollection::iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      (*it)->set_charge(charge+(*it)->charge());  
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    m_digits->push_back(new sTgcDigit(id, bctag, digittime, charge, 0, 0));
  }

  return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfEnergyThreshold() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
              m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = 0.00052; //MeV
            }
          }
        }
      }
    }
  }

  // Find path to the sTGC_Digitization_energyThreshold.dat file
  const std::string fileName = "sTGC_Digitization_energyThreshold.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_energyThreshold.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not open file " << fileName.c_str() );
    return;
  }

  double energyThreshold;
  // Read the sTGC_Digitization_energyThreshold.dat file
  while(ifs.good()&& !ifs.eof() ) {
    ifs >> iStationName >> stationEta
        >> stationPhi   >> multiPlet
        >> gasGap >> channelType >> energyThreshold;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfEnergyThreshold"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= " << multiPlet
                    << " gasGap= " << gasGap
                    << " channelType= " << channelType
                    << " energyThreshold(MeV)= " << energyThreshold );

    // Subtract offsets to use indices of energyThreshold array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;
    if(channelType <0 || channelType >=N_CHANNELTYPE    ) continue;

    m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = energyThreshold;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_energyThreshold.dat file
  ifs.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfDeadChamber() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = false;
          }
        } 
      }
    }
  }

  // Find path to the sTGC_Digitization_deadChamber.dat file
  const std::string fileName = "sTGC_Digitization_deadChamber.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_deadChamber.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not open file " << fileName.c_str() );
    return;
  }

  // Read the sTGC_Digitization_deadChamber.dat file
  unsigned int nDeadChambers = 0;
  std::string comment;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> multiPlet >> gasGap;
    bool valid = getline(ifs, comment).good();
    if(!valid) break;

    ATH_MSG_DEBUG( "sTgcDigitMaker::readFileOfDeadChamber"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= "  << multiPlet 
                    << " gasGap= " << gasGap
                    << " comment= " << comment );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;

    m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = true;
    nDeadChambers++;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_deadChamber.dat file
  ifs.close();

  ATH_MSG_INFO("sTgcDigitMaker::readFileOfDeadChamber: the number of dead chambers = " << nDeadChambers );
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfTimeWindowOffset() {
  // Indices to be used
  int iStationName, stationEta, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
        m_timeWindowOffset[iStationName][stationEta][channelType] = 0.;
      }
    }
  }

  // Find path to the sTGC_Digitization_timeWindowOffset.dat file
  const std::string fileName = "sTGC_Digitization_timeWindowOffset.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_timeWindowOffset.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not open file " << fileName.c_str() );
    return;
  }

  // Read the sTGC_Digitization_timeWindowOffset.dat file
  double timeWindowOffset;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> channelType >> timeWindowOffset;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfTimeWindowOffset"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " channelType= " << channelType
                    << " timeWindowOffset= " << timeWindowOffset );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(channelType     <0 || channelType     >=N_CHANNELTYPE    ) continue;

    m_timeWindowOffset[iStationName][stationEta][channelType] = timeWindowOffset;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_timeWindowOffset.dat file
  ifs.close();
}



//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getEnergyThreshold(const std::string stationName, int stationEta, int stationPhi, int multiPlet, int gasGap, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;
  channelType  -= OFFSET_CHANNELTYPE;

  double energyThreshold = +999999.;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET ) &&
     (gasGap      >=0 && gasGap      <N_GASGAP     ) &&
     (channelType >=0 && channelType <N_CHANNELTYPE    )) {
    energyThreshold = m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE( "sTgcDigitMaker::getEnergyThreshold"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet= " << multiPlet+OFFSET_MULTIPLET
                    << " gasGap= "      << gasGap+OFFSET_GASGAP
                    << " channelType= " << channelType+OFFSET_CHANNELTYPE
                    << " energyThreshold(MeV)= " << energyThreshold );

  return energyThreshold;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::isDeadChamber(const std::string stationName, int stationEta, int stationPhi, int multiPlet, int gasGap) {
  bool v_isDeadChamber = true;

  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET  ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     )) {
    v_isDeadChamber = m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE(  "sTgcDigitMaker::isDeadChamber"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet = " << multiPlet +OFFSET_MULTIPLET
                    << " gasGap= " << gasGap+OFFSET_GASGAP
                    << " isDeadChamber= " << v_isDeadChamber );

  return v_isDeadChamber;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getTimeWindowOffset(const std::string stationName, int stationEta, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  channelType  -= OFFSET_CHANNELTYPE;

  // Check the indices are valid
  if(iStationName<0 || iStationName >=N_STATIONNAME) return 0.;
  if(stationEta  <0 || stationEta   >=N_STATIONETA ) return 0.;
  if(channelType <0 || channelType  >=N_CHANNELTYPE    ) return 0.;

  // Values were determined to reproduce the fraction of Previous BC hit fraction in Z->mumu data during Periods B,D,E in 2011.
  return m_timeWindowOffset[iStationName][stationEta][channelType];
}

int sTgcDigitMaker::getIStationName(const std::string stationName) const {
  int iStationName = 0;
  if(     stationName=="STS") iStationName = 0;
  else if(stationName=="STL") iStationName = 1;

  return iStationName;
}

////+++++++++++++++++++++++++++++++++++++++++++++++
//void sTgcDigitMaker::adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
//                                       const Amg::Vector3D direCos, Amg::Vector3D &localPos) const {
//  int iStationName = getIStationName(stationName);
//  iStationName -= OFFSET_STATIONNAME;
//  stationEta   -= OFFSET_STATIONETA;
//  stationPhi   -= OFFSET_STATIONPHI;
//  // Check the indices are valid
//  if(iStationName<0 || iStationName>=N_STATIONNAME) return;
//  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return;
//  if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) return;
//
//  // Local +x (-x) direction is global +z direction on A-side (C-side).
//  double localDisplacementX = m_alignmentT[iStationName][stationEta][stationPhi];
//
//  // Local +z direction is global +r direction.
//  double localDisplacementZ = m_alignmentZ[iStationName][stationEta][stationPhi];
//
//  // Local +s direction is global +phi direction.
//  double localDisplacementY = m_alignmentS[iStationName][stationEta][stationPhi];
//
//  // Rotation around the s-axis is not implemented yet (2011/11/29).
//  // m_alignmentTHS[tmpStationName][tmpStationEta][tmpStationPhi];
//
//  // Convert local x translation to local y and z translations
//  double localDisplacementYByX = 0.;
//  double localDisplacementZByX = 0.;
//  if(fabs(localDisplacementX)>1.0E-12) { // local y and z translations are non-zero only if local x translation is non-zero.
//    if(fabs(direCos[0])<1.0E-12) return; // To avoid zero-division
//    localDisplacementYByX = direCos[1]/direCos[0]*localDisplacementX;
//    localDisplacementZByX = direCos[2]/direCos[0]*localDisplacementX;
//  }
//  localPos.y() = localPos.y()+localDisplacementYByX+localDisplacementY;
//  localPos.z() = localPos.z()+localDisplacementZByX-localDisplacementZ;
//}


