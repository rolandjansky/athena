/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------
// TgcDigitMaker.cxx
//-----------------------------------------------------
// Y. HASEGAWA
//-----------------------------------------------------

#include "TGC_Digitization/TgcDigitMaker.h"

#include <iostream>
#include <fstream>

#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "GaudiKernel/MsgStream.h"
#include "PathResolver/PathResolver.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

//---------------------------------------------------
//  Constructor and Destructor
//---------------------------------------------------

//----- Constructor
TgcDigitMaker::TgcDigitMaker(TgcHitIdHelper*                    hitIdHelper,
			     const MuonGM::MuonDetectorManager* mdManager,
			     unsigned int                       runperiod)
{
  m_digits                  = 0;
  m_hitIdHelper             = hitIdHelper;
  m_mdManager               = mdManager;
  m_runperiod               = runperiod;
  m_idHelper                = 0;
  m_efficiencyOfWireGangs   = 1.000; // 100% efficiency for TGCSimHit_p1
  m_efficiencyOfStrips      = 1.000; // 100% efficiency for TGCSimHit_p1
  m_timeWindowOffsetWire    = 0.;
  m_timeWindowOffsetStrip   = 0.;
  m_timeWindowWire          = 29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  m_timeWindowStrip         = 40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
}

//----- Destructor
TgcDigitMaker::~TgcDigitMaker()
{
}

//------------------------------------------------------
// Initialize
//------------------------------------------------------
StatusCode TgcDigitMaker::initialize()
{
  // Initialize TgcIdHelper
  if(!m_hitIdHelper) {
    m_hitIdHelper = TgcHitIdHelper::GetHelper();
  }

  // initialize the TGC identifier helper
  m_idHelper = m_mdManager->tgcIdHelper();
  
  readFileOfTimeJitter();

  // Read share/TGC_Digitization_energyThreshold.dat file and store values in m_energyThreshold. 
  readFileOfEnergyThreshold();

  // Read share/TGC_Digitization_crossTalk.dat file and store values in m_crossTalk.
  readFileOfCrossTalk();

  // Read share/TGC_Digitization_deadChamber.dat file and store values in m_isDeadChamber. 
  readFileOfDeadChamber();

  // Read share/TGC_Digitization_timeWindowOffset.dat file and store values in m_timeWindowOffset. 
  readFileOfTimeWindowOffset();

  // Read share/TGC_Digitization_alignment.dat file and store values in m_alignmentZ, m_alignmentT, m_alignmentS, m_alignmentTHS
  readFileOfAlignment();

  return StatusCode::SUCCESS;
}

//---------------------------------------------------
// Execute Digitization
//---------------------------------------------------
TgcDigitCollection* TgcDigitMaker::executeDigi(const TGCSimHit* hit,
					       const double globalHitTime,
                                               CLHEP::HepRandomEngine* rndmEngine)
{
  //////////  convert ID for this digitizer system 
  int         Id         = hit->TGCid();
  std::string stationName= m_hitIdHelper->GetStationName(Id);
  int         stationEta = m_hitIdHelper->GetStationEta(Id);
  int         stationPhi = m_hitIdHelper->GetStationPhi(Id);
  int         ilyr       = m_hitIdHelper->GetGasGap(Id);
  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "executeDigi() Got HIT Id." << endmsg;

  // Check the chamber is dead or not. 
  if(isDeadChamber(stationName, stationEta, stationPhi, ilyr)) return 0;

  m_timeWindowOffsetWire  = getTimeWindowOffset(stationName, stationEta, 0);
  m_timeWindowOffsetStrip = getTimeWindowOffset(stationName, stationEta, 1);

  const Identifier elemId = m_idHelper -> elementID(stationName,stationEta,stationPhi);
  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "executeDigi() - element identifier is: " << m_idHelper->show_to_string(elemId) << endmsg;

  const MuonGM::TgcReadoutElement *tgcChamber = m_mdManager->getTgcReadoutElement(elemId);
  if(!tgcChamber) {
    if(msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "executeDigi() - no ReadoutElement found for " << m_idHelper->show_to_string(elemId) << endmsg;
    return 0;
  }

  
  const Amg::Vector3D centreChamber = tgcChamber->globalPosition();
  float height                   = tgcChamber->getRsize();
  float hmin                     = sqrt(pow(centreChamber.x(),2)+pow(centreChamber.y(),2)) - height/2.;
  float wirePitch   = 1.8*CLHEP::mm;
  float frameZwidth = 17. *CLHEP::mm;
  float frameXwidth = 20. *CLHEP::mm;
  


  IdContext tgcContext = m_idHelper->module_context();
  IdentifierHash coll_hash;
  if(m_idHelper->get_hash(elemId, coll_hash, &tgcContext)) {
    if(msgLevel(MSG::WARNING)) msg(MSG::WARNING) << "Unable to get TGC hash id from TGC Digit collection "
						 << "context begin_index = " << tgcContext.begin_index()
						 << " context end_index  = " << tgcContext.end_index()
						 << " the identifier is "
						 << endmsg;
    elemId.show();
  }
  
  m_digits = new TgcDigitCollection(elemId, coll_hash);

  // Direction cosine of incident angle of this track
  Amg::Vector3D direCos = hit->localDireCos();
  
  // local position
  Amg::Vector3D localPos = hit->localPosition();

  /*** Ad hoc implementation of detector position shift */ 
  //adHocPositionShift(stationName, stationEta, stationPhi, direCos, localPos);

  // Local z direction is global r direction.  
  float distanceZ = 1.4*CLHEP::mm/direCos[0]*direCos[2];
  float zLocal = localPos.z() + distanceZ; 
  
  // Local y direction is global phi direction.  
  float distanceY = 1.4*CLHEP::mm/direCos[0]*direCos[1];
  // This ySign depends on the implementation of TGC geometry in G4 simulation
  // left-handed coordinate in A side(+z, stationEta>0)
  float ySign = (stationEta < 0) ? +1. : -1.; 
  float yLocal = ySign * (localPos.y() + distanceY);

  // Time of flight correction for each chamber
  // the offset is set to 0 for ASD located at the postion where tof is minimum in each chamber,
  // i.e. the ASD at the smallest R in each chamber
  double tofCorrection = (sqrt(pow(hmin+frameZwidth,2)+pow(centreChamber.z(),2))/(299792458.*(CLHEP::m/CLHEP::s)));//FIXME use CLHEP::c_light

  // bunch time
  float bunchTime = globalHitTime - tofCorrection;

  static const float jitterInitial = 9999.;
  float jitter = jitterInitial; // calculated at wire group calculation and used also for strip calculation

  /////////////   wire group number calculation   ///////////////
  int isStrip = 0;

  double energyDeposit = hit->energyDeposit(); // Energy deposit in MeV 
  // If TGCSimHit_p1 is used, energyDeposit is the default value of -9999. 
  // If TGCSimHit_p2 is used, energyDeposit is equal to or greater than 0. 
  // Therefore, the energyDeposit value can be used to distinguish TGCSimHit_p1 and TGCSimHit_p2. 
  // For TGCSimHit_p1, old efficiency check with only isStrip variable is used. 
  // For TGCSimHit_p2, new efficiency check with chamber dependent energy threshold is used. 

  if((energyDeposit< -1. && efficiencyCheck(isStrip, rndmEngine)) || // Old efficiencyCheck for TGCSimHit_p1. 
     (energyDeposit>=-1. && efficiencyCheck(stationName, stationEta, stationPhi, ilyr, isStrip, energyDeposit)) // New efficiencyCheck for TGCSimHit_p2
     ) {  

    int iWireGroup[2];
    float posInWireGroup[2] = {0., 0.};
    for(int iPosition=0; iPosition<2; iPosition++) {
      int nWireOffset = (std::abs(stationEta) == 5 || stationName.substr(0,2) == "T4") ? 1 : 0;
      // for chambers in which only the first wire  is not connected                                 : 1
      // for chambers in which the first and last wires are not connected OR all wires are connected : 0

      double zPosInSensArea = zLocal + static_cast<double>(tgcChamber->getTotalWires(ilyr)-nWireOffset)*wirePitch/2.;

      // check a hit in the sensitive area
      if(zPosInSensArea < 0. || zPosInSensArea > tgcChamber->getTotalWires(ilyr)*wirePitch) {
	iWireGroup[iPosition] = 0;
	posInWireGroup[iPosition] = 0.;
	if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "executeDigi(): Wire: Hit position located at outside of a sensitive volume, "
						 << " id: " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << " position: " << zPosInSensArea
						 << " xlocal: " << zLocal 
						 << " dir cosine: " << direCos[0] << "/" << direCos[1] << "/" << direCos[2]
						 << " active region: " << height-frameZwidth*2.
						 << endmsg;
      }
      else {
	int igang      = 1;
	int wire_index = 0;
	while(wirePitch*(static_cast<float>(wire_index)) < zPosInSensArea
	      && igang <= tgcChamber -> getNGangs(ilyr)) {
	  wire_index += tgcChamber->getNWires(ilyr,igang);
	  igang++;
	}
	posInWireGroup[iPosition] = (zPosInSensArea/wirePitch-(static_cast<float>(wire_index)))/(static_cast<float>(tgcChamber->getNWires(ilyr,igang-1)))+1.;

	iWireGroup[iPosition] = ((1==igang) ? 1 : igang-1);
      }
    }
    
    unsigned int jWG[2] = {(iWireGroup[0]<=iWireGroup[1]) ? (unsigned int)(0) : (unsigned int)(1), 
     			   (iWireGroup[0]<=iWireGroup[1]) ? (unsigned int)(1) : (unsigned int)(0)};
    int iWG[2] = {iWireGroup[jWG[0]], iWireGroup[jWG[1]]};
    float posInWG[2] = {posInWireGroup[jWG[0]], posInWireGroup[jWG[1]]};
    if(iWG[0]!=iWG[1]) {
      if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "executeDigi(): Multihits found in WIRE GROUPs:" << iWG[0] << " " << iWG[1] << endmsg;
    }

    for(int iwg=iWG[0]; iwg<=iWG[1]; iwg++) {
      if(1<=iwg && iwg<=tgcChamber->getNGangs(ilyr)) {
	// TGC response time calculation
	float jit = timeJitter(direCos, rndmEngine);
	if(jit < jitter) jitter = jit;
	const float wirePropagationTime = 3.3*CLHEP::ns/CLHEP::m; // 3.7*ns/m was used until MC10.
	float ySignPhi = (stationPhi%2==1) ? -1. : +1.; 
	// stationPhi%2==0 : +1. : ASD attached at the smaller phi side of TGC 
	// stationPhi%2==1 : -1. : ASD attached at the larger phi side of TGC
	float wDigitTime = bunchTime + jit + wirePropagationTime*(ySignPhi*yLocal + tgcChamber->chamberWidth(zLocal)/2.);

	if(wDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetWire || m_bunchCrossingTime+m_timeWindowOffsetWire+m_timeWindowWire < wDigitTime) {
	  if(msgLevel(MSG::DEBUG)) {
	    msg(MSG::DEBUG ) << "WireGroup: digitized time is outside of time window. " << wDigitTime
			     << " bunchTime: " << bunchTime << " time jitter: " << jitter 
			     << " propagation time: " << wirePropagationTime*(ySignPhi*yLocal + height/2. + frameXwidth) << endmsg;
	  }
	}
	else {
	  Identifier newId = m_idHelper -> channelID(stationName,stationEta,stationPhi,
						     ilyr, isStrip, iwg);
	  uint16_t bctag = bcTagging(wDigitTime,isStrip);
	  addDigit(newId,bctag);

	  if(iwg==iWG[0]) {
	    randomCrossTalk(elemId, ilyr, isStrip, iwg, posInWG[0], wDigitTime, rndmEngine);
	  }	 
 
	  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "WireGroup: newid breakdown digitTime x/y/z direcos height_gang bctag: "
						   << newId << " " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr << "/"
						   << isStrip << "/" << iwg << " " 
						   << wDigitTime << " " << localPos.x() << "/"  << localPos.y() << "/" << localPos.z() << " "
						   << direCos.x() << "/"  << direCos.y() << "/" << direCos.z() << " "
						   << height << " " << tgcChamber->getNWires(ilyr,iwg) << " "
						   << bctag << endmsg;
	}
      }
      else {
	if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Wire Gang id is out of range. id, x/y/z, height: " << iwg
						  << " " << localPos.x() << "/"  << localPos.y() << "/" << localPos.z() << " " << height
						  << endmsg;
      }
    }
  } // end of wire group calculation

  //////////////  strip number calculation  //////////////

  isStrip = 1;

  if((ilyr != 2 || (stationName.substr(0,2) != "T1")) && // no stip in middle layers of T1* 
     ((energyDeposit< -1. && efficiencyCheck(isStrip, rndmEngine)) || // Old efficiencyCheck for TGCSimHit_p1.
      (energyDeposit>=-1. && efficiencyCheck(stationName, stationEta, stationPhi, ilyr, isStrip, energyDeposit))) // New efficiencyCheck for TGCSimHit_p2
     ) { 

    int iStation = atoi(stationName.substr(1,1).c_str())-1;

    int iStrip[2];
    float posInStrip[2] = {0., 0.};
    // Take into account of charge spread on cathod plane
    for (int iPosition = 0; iPosition < 2; iPosition++) {

      // check a hit in the sensitive area
      float zPos = zLocal+height/2.-frameZwidth;
      if (zPos < 0.) {
	iStrip[iPosition] = 0;
	posInStrip[iPosition] = 0.;
	if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip: Hit position located at outside of a sensitive volume, id, position, xlocal0/1, dir cosine: "
						 << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << zPos << " " << zLocal << " " << direCos[0] << "/" << direCos[1] << "/" << direCos[2]
						 << endmsg;
      }
      else if (zPos > height-frameZwidth*2.) {
	iStrip[iPosition] =  tgcChamber -> getNStrips(ilyr) + 1;
	posInStrip[iPosition] = 0.;
	if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip: Hit position located at outside of a sensitive volume, id, position, active region: "
						 << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << zPos << " " << height-frameZwidth*2.
						 << endmsg;
      }
      else { // sensitive area

	//
	// for layout P03
	//
	  // number of strips in exclusive phi coverage of a chamber in T[1-3] and T4
	  const float nDivInChamberPhi[4] = {29.5, 29.5, 29.5, 31.5};
	  float dphi;
	  if ("T4E" != stationName) {
	    dphi = 360.*CLHEP::degree/static_cast<float>(tgcChamber->getNPhiChambers())/nDivInChamberPhi[iStation];
	  }
	  else {
	    dphi = 360.*CLHEP::degree/36./nDivInChamberPhi[iStation];
	  }
	  float phiLocal = atan2(yLocal, zLocal+ height/2.+hmin);

	  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "dphi, phiLocal, yLocal, zLocall+ height/2.+hmin: "
						   << dphi << " " << phiLocal << " " << yLocal << " " << zLocal+height/2.+hmin
						   << endmsg;

	  int istr = 0;
	  if ((stationEta > 0 && ilyr == 1) ||
	      (stationEta < 0 && ilyr != 1)   ) {
	    istr = static_cast<int>(floor(phiLocal/dphi + 15.75))                        + 1;
	    posInStrip[iPosition] =       phiLocal/dphi + 15.75 - static_cast<float>(istr- 1);
	    if (istr > 30) { // treatment for two half strips
	      istr = static_cast<int>(floor((phiLocal - dphi*14.25)/(dphi/2.)))                        + 31;
	      posInStrip[iPosition] =       (phiLocal - dphi*14.25)/(dphi/2.) - static_cast<float>(istr- 31);
	    }
	  }
	  else {
	    istr = static_cast<int>(floor(phiLocal/dphi + 16.25))                        + 1;
	    posInStrip[iPosition] =       phiLocal/dphi + 16.25 - static_cast<float>(istr- 1);
	    if (istr < 3) { // treatment for two half strips
	      istr = static_cast<int>(floor((phiLocal + dphi*14.25)/(dphi/2.)))                        + 3;
	      posInStrip[iPosition] =       (phiLocal + dphi*14.25)/(dphi/2.) - static_cast<float>(istr- 3);
	    }
	  }
	  if (istr < 1) {
	    istr = 0;
	    posInStrip[iPosition] = 0.;
	  }
	  else if (32 < istr) {
	    istr = 33;
	    posInStrip[iPosition] = 0.;
	  }
	  iStrip[iPosition] = istr;
      } // sensitive area
    }

    unsigned int jStr[2] = {(iStrip[0]<=iStrip[1]) ? (unsigned int)(0) : (unsigned int)(1), 
     			    (iStrip[0]<=iStrip[1]) ? (unsigned int)(1) : (unsigned int)(0)};
    int iStr[2] = {iStrip[jStr[0]], iStrip[jStr[1]]};
    float posInStr[2] = {posInStrip[jStr[0]], posInStrip[jStr[1]]};
    if(iStr[0]!=iStr[1]) {
      if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Multihits found in STRIPs:" << iStr[0] << " " << iStr[1] << endmsg;
    }

    for(int istr=iStr[0]; istr<=iStr[1]; istr++) {
      if(1<=istr && istr<=32) {
	// TGC response time calculation
	const float stripPropagationTime = 3.3*CLHEP::ns/CLHEP::m; // 8.5*ns/m was used until MC10. 
	// Since MC11 3.3*ns/m (the speed of light) is used and was obtained with Z->mumu data/MC comparison. 
	if(jitter > jitterInitial-0.1) {
	  jitter = timeJitter(direCos, rndmEngine);
	}
	float zSignEta = (abs(stationEta)%2 == 1 && abs(stationEta) != 5) ? -1. : +1.;
	// if(abs(stationEta)%2 == 1 && abs(stationEta) != 5) : -1. : ASD attached at the longer base of TGC
	// else                                               : +1. : ASD attached at the shorter base of TGC   
	float sDigitTime = bunchTime + jitter + stripPropagationTime*(height/2. + frameZwidth + zSignEta*zLocal);

	if(sDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetStrip || +m_bunchCrossingTime+m_timeWindowOffsetStrip+ m_timeWindowStrip < sDigitTime) {
	  if(msgLevel(MSG::DEBUG)) {
	    msg(MSG::DEBUG) << "Strip: Digitized time is outside of time window. " << sDigitTime
			    << " bunchTime: " << bunchTime << " time jitter: " << jitter 
			    << " propagation time: " << stripPropagationTime*(height/2. + frameZwidth + zSignEta*zLocal)
			    << endmsg;
	  }
	}
	else {
	  Identifier newId = m_idHelper -> channelID(stationName,stationEta,stationPhi, ilyr, isStrip, istr);
	  uint16_t bctag = bcTagging(sDigitTime,isStrip);
	  addDigit(newId, bctag);

	  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip: newid breakdown digitTime x/y/z direcos r_center bctag: "
						   << newId << " " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr << "/"
						   << isStrip << "/" << istr << " "
						   << sDigitTime << " " << localPos.x() << "/" << localPos.y() << "/" << localPos.z() << " " 
						   << direCos.x() << "/"  << direCos.y() << "/" << direCos.z() << " "
						   << height/2.+hmin << " "
						   << bctag << endmsg;

	  if(istr==iStr[0]) {
	    randomCrossTalk(elemId, ilyr, isStrip, iStr[0], posInStr[0], sDigitTime, rndmEngine);
	  }
	}
      }
      else {
	if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip id is out of range: gap, id: " << ilyr << " " << istr << endmsg;
      }
    }
  } // end of strip number calculation
  
  return m_digits;
}

MsgStream& TgcDigitMaker::msg(const MSG::Level lvl) const { return m_msg << lvl ; }
bool TgcDigitMaker::msgLevel(const MSG::Level lvl) const { return m_msg.get().level() <= lvl ; }
void TgcDigitMaker::setMessageLevel(const MSG::Level lvl) const { m_msg.get().setLevel(lvl); return; }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TgcDigitMaker::readFileOfTimeJitter()
{

  const char* const fileName = "TGC_Digitization_timejitter.dat";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  std::ifstream ifs;
  if (fileWithPath != "") {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    msg(MSG::FATAL) << "readFileOfTimeJitter(): Could not find file " << fileName << endmsg;
    exit(-1);
  }

  if(ifs.bad()){
    msg(MSG::FATAL) << "readFileOfTimeJitter(): Could not open file "<< fileName << endmsg;
    exit(-1);
  }

  int angle = 0;
  int bins = 0;
  int i = 0;
  float prob = 0.;

  while(ifs.good()){
    ifs >> angle >> bins;
    if (ifs.eof()) break;
    if(msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): Timejitter, angle, Number of bins, prob. dist.: " << angle << " " << bins << " ";
    m_vecAngle_Time.resize(i + 1);
    for (int j = 0; j < 41/*bins*/; j++) {
      ifs >> prob;
      m_vecAngle_Time[i].push_back(prob);
      if (j == 0)
	if(msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): ";
      if(msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << prob << " ";
    }
    if(msgLevel(MSG::VERBOSE)) msg(MSG::VERBOSE) << endmsg;
    i++;
  }
  ifs.close();
}
//+++++++++++++++++++++++++++++++++++++++++++++++
float TgcDigitMaker::timeJitter(const Amg::Vector3D direCosLocal, CLHEP::HepRandomEngine* rndmEngine) const 
{
  float injectionAngle = atan2(fabs(direCosLocal[2]),fabs(direCosLocal[0]))/CLHEP::degree;

  int   ithAngle = static_cast<int>(injectionAngle/5.);
  float wAngle = injectionAngle/5. - static_cast<float>(ithAngle);
  int   jthAngle;
  if (ithAngle > 11) {
    ithAngle = 12;
    jthAngle = 12;
  }
  else {
    jthAngle = ithAngle+1;
  }

  float jitter = 0.;
  float prob = 1.;
  float probRef = 0.;

  while (prob > probRef) {
    prob   = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);
    jitter = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0)*40.; // trial time jitter in nsec
    int ithJitter = static_cast<int>(jitter);
    // probability distribution calculated from weighted sum between neighboring bins of angles
    probRef = (1.-wAngle)*m_vecAngle_Time[ithAngle][ithJitter]
      +    wAngle *m_vecAngle_Time[jthAngle][ithJitter];
  }
  return jitter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool TgcDigitMaker::efficiencyCheck(const int isStrip, CLHEP::HepRandomEngine* rndmEngine) const {
  if(isStrip == 0) { // wire group
    if(CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0) < m_efficiencyOfWireGangs) return true;
  }
  else if(isStrip == 1) { // strip
    if(CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0) < m_efficiencyOfStrips) return true;
  }
  if(msgLevel(MSG::DEBUG)) msg(MSG::DEBUG) << "efficiencyCheck(): Hit removed. isStrip: " << isStrip << endmsg;
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool TgcDigitMaker::efficiencyCheck(const std::string stationName, const int stationEta, const int stationPhi, const int gasGap, const int isStrip, const double energyDeposit) const {
  // If the energy deposit is equal to or greater than the threshold value of the chamber, 
  // return true. 
  return (energyDeposit >= getEnergyThreshold(stationName, stationEta, stationPhi, gasGap, isStrip));
}
//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t TgcDigitMaker::bcTagging(const double digitTime, const int isStrip) const {

  double offset, window;
  if(isStrip == 0) { // Wire gang
    offset = m_timeWindowOffsetWire;
    window = m_timeWindowWire;
  }
  else { // Strip
    offset = m_timeWindowOffsetStrip;
    window = m_timeWindowStrip;
  }

  uint16_t bctag = 0;
  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x1);
  }
  if(                     offset < digitTime && digitTime <                      offset+window) {
    bctag = (bctag | 0x2);
  }
  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
    bctag = (bctag | 0x4);
  }
  return bctag;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
void TgcDigitMaker::addDigit(const Identifier id, const uint16_t bctag) {
  if((bctag & 0x1) != 0) {
    bool duplicate = false; 
    for(TgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) {
      if(id==(*it)->identify() && TgcDigit::BC_PREVIOUS==(*it)->bcTag()) {
	duplicate = true;
	break; 
      }
    }
    if(!duplicate) { 
      TgcDigit* multihitDigit = new TgcDigit(id,TgcDigit::BC_PREVIOUS);
      m_digits -> push_back(multihitDigit);
    }
  }
  if((bctag & 0x2) != 0) {
    bool duplicate = false;
    for(TgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) { 
      if(id==(*it)->identify() && TgcDigit::BC_CURRENT==(*it)->bcTag()) {
        duplicate = true;
        break;
      }
    }
    if(!duplicate) {
      TgcDigit* multihitDigit = new TgcDigit(id,TgcDigit::BC_CURRENT);
      m_digits -> push_back(multihitDigit);
    }
  }
  if((bctag & 0x4) != 0) {
    bool duplicate = false;
    for(TgcDigitCollection::const_iterator it=m_digits->begin(); it!=m_digits->end(); it++) { 
      if(id==(*it)->identify() && TgcDigit::BC_NEXT==(*it)->bcTag()) {
        duplicate = true;
        break;
      }
    }
    if(!duplicate) {
      TgcDigit* multihitDigit = new TgcDigit(id,TgcDigit::BC_NEXT);
      m_digits -> push_back(multihitDigit);
    }
  }
  return;
}

void TgcDigitMaker::readFileOfEnergyThreshold() {
  // Indices to be used 
  int iStationName, stationEta, stationPhi, gasGap, isStrip;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
          for(isStrip=0; isStrip<N_ISSTRIP; isStrip++) {
            m_energyThreshold[iStationName][stationEta][stationPhi][gasGap][isStrip] = -999999.;
          }
        }
      }
    }
  }

  // Find path to the TGC_Digitization_energyThreshold.dat file 
  const std::string fileName = "TGC_Digitization_energyThreshold.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    msg(MSG::FATAL) << "readFileOfEnergyThreshold(): Could not find file " << fileName.c_str() << endmsg;
    return;
  }

  // Open the TGC_Digitization_energyThreshold.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    msg(MSG::FATAL) << "readFileOfEnergyThreshold(): Could not open file " << fileName.c_str() << endmsg;
    return;
  }
    
  double energyThreshold; 
  // Read the TGC_Digitization_energyThreshold.dat file
  while(ifs.good()) {
    ifs >> iStationName >> stationEta 
      	>> stationPhi 
	>> gasGap >> isStrip >> energyThreshold;
    if(msgLevel(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "TgcDigitMaker::readFileOfEnergyThreshold" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " isStrip= " << isStrip
		      << " energyThreshold(MeV)= " << energyThreshold
		      << endmsg;
    }

    // Subtract offsets to use indices of energyThreshold array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    gasGap       -= OFFSET_GASGAP;
    isStrip      -= OFFSET_ISSTRIP;

    // Check the indices are valid 
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;
    if(isStrip     <0 || isStrip     >=N_ISSTRIP    ) continue;

    m_energyThreshold[iStationName][stationEta][stationPhi][gasGap][isStrip] = energyThreshold;

    // If it is the end of the file, get out from while loop.   
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_energyThreshold.dat file 
  ifs.close();
}

void TgcDigitMaker::readFileOfCrossTalk() {
  // Indices to be used 
  int iStationName = -1;
  int stationEta = -1;
  int stationPhi = -1;
  int gasGap = -1;
  int isStrip = -1;
  int iProb = -1;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
	  for(isStrip=0; isStrip<N_ISSTRIP; isStrip++) {
	    for(iProb=0; iProb<N_CROSSTALK_PARAMETER; iProb++) {
	      m_crossTalk[iStationName][stationEta][stationPhi][gasGap][isStrip][iProb] = 0.;
	    }
          }
        }
      }
    }
  }

  // Find path to the TGC_Digitization_crossTalk.dat file 
  const std::string fileName = "TGC_Digitization_crossTalk.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    msg(MSG::FATAL) << "readFileOfCrossTalk(): Could not find file " << fileName.c_str() << endmsg;
    return;
  }

  // Open the TGC_Digitization_crossTalk.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    msg(MSG::FATAL) << "readFileOfCrossTalk(): Could not open file " << fileName.c_str() << endmsg;
    return;
  }
    
  double crossTalk_10 = 0.; 
  double crossTalk_11 = 0.; 
  double crossTalk_20 = 0.; 
  double crossTalk_21 = 0.; 
  // Read the TGC_Digitization_crossTalk.dat file
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> /*stationPhi >>*/ gasGap >> isStrip >> crossTalk_10 >> crossTalk_11 >> crossTalk_20 >> crossTalk_21;
    if(msgLevel(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "TgcDigitMaker::readFileOfCrossTalk" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
	//		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " isStrip= " << isStrip
		      << " prob(10) " << crossTalk_10
		      << " prob(11) " << crossTalk_11
		      << " prob(20) " << crossTalk_20
		      << " prob(21) " << crossTalk_21
		      << endmsg;
    }

    // Subtract offsets to use indices of crossTalk array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    //    stationPhi   -= OFFSET_STATIONPHI;
    gasGap       -= OFFSET_GASGAP;
    isStrip      -= OFFSET_ISSTRIP;

    // Check the indices are valid 
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    //    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;
    if(isStrip     <0 || isStrip     >=N_ISSTRIP    ) continue; 

    // stationPhi dependence is now omitted. 
    // The same energy threshold value is used. 
    for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
      m_crossTalk[iStationName][stationEta][stationPhi][gasGap][isStrip][0] = crossTalk_10;
      m_crossTalk[iStationName][stationEta][stationPhi][gasGap][isStrip][1] = crossTalk_11;
      m_crossTalk[iStationName][stationEta][stationPhi][gasGap][isStrip][2] = crossTalk_20;
      m_crossTalk[iStationName][stationEta][stationPhi][gasGap][isStrip][3] = crossTalk_21;
    }

    // If it is the end of the file, get out from while loop.   
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_crossTalk.dat file 
  ifs.close();
}

void TgcDigitMaker::readFileOfDeadChamber() {
  // Indices to be used 
  int iStationName, stationEta, stationPhi, gasGap;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
	  m_isDeadChamber[iStationName][stationEta][stationPhi][gasGap] = false;
        }
      }
    }
  }

  // Find path to the TGC_Digitization_deadChamber.dat file
  std::string fileName;
  if(m_runperiod == 1) fileName = "TGC_Digitization_deadChamber.dat";
  else if(m_runperiod == 2) fileName = "TGC_Digitization_2016deadChamber.dat";
  else if(m_runperiod == 3) fileName = "TGC_Digitization_NOdeadChamber.dat";
  else {
    msg(MSG::ERROR) << "Run Period " << m_runperiod << " is unexpected in TgcDigitMaker - using NOdeadChamber configuration." << endmsg;
    fileName = "TGC_Digitization_NOdeadChamber.dat";
    return;
  }
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    msg(MSG::FATAL) << "readFileOfDeadChamber(): Could not find file " << fileName.c_str() << endmsg;
    return;
  }

  // Open the TGC_Digitization_deadChamber.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    msg(MSG::FATAL) << "readFileOfDeadChamber(): Could not open file " << fileName.c_str() << endmsg;
    return;
  }
    
  // Read the TGC_Digitization_deadChamber.dat file
  unsigned int nDeadChambers = 0;
  std::string comment;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> gasGap;
    bool valid = getline(ifs, comment).good(); 
    if(!valid) break; 

    if(msgLevel(MSG::DEBUG)) { 
      msg(MSG::DEBUG) << "TgcDigitMaker::readFileOfDeadChamber" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " comment= " << comment
		      << endmsg;
    }

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    gasGap       -= OFFSET_GASGAP;

    // Check the indices are valid 
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;

    m_isDeadChamber[iStationName][stationEta][stationPhi][gasGap] = true;
    nDeadChambers++;

    // If it is the end of the file, get out from while loop.   
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_deadChamber.dat file 
  ifs.close();

  if(msgLevel(MSG::INFO)) msg(MSG::INFO) << "TgcDigitMaker::readFileOfDeadChamber: the number of dead chambers = " << nDeadChambers << endmsg;
}

void TgcDigitMaker::readFileOfTimeWindowOffset() {
  // Indices to be used 
  int iStationName, stationEta, isStrip;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(isStrip=0; isStrip<N_ISSTRIP; isStrip++) {
	m_timeWindowOffset[iStationName][stationEta][isStrip] = 0.;
      }
    }
  }

  // Find path to the TGC_Digitization_timeWindowOffset.dat file 
  const std::string fileName = "TGC_Digitization_timeWindowOffset.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    msg(MSG::FATAL) << "readFileOfTimeWindowOffset(): Could not find file " << fileName.c_str() << endmsg;
    return;
  }

  // Open the TGC_Digitization_timeWindowOffset.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    msg(MSG::FATAL) << "readFileOfTimeWindowOffset(): Could not open file " << fileName.c_str() << endmsg;
    return;
  }
    
  // Read the TGC_Digitization_timeWindowOffset.dat file
  double timeWindowOffset;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> isStrip >> timeWindowOffset;
    if(msgLevel(MSG::DEBUG)) { 
      msg(MSG::DEBUG) << "TgcDigitMaker::readFileOfTimeWindowOffset" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " isStrip= " << isStrip
		      << " timeWindowOffset= " << timeWindowOffset
		      << endmsg;
    }

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    isStrip      -= OFFSET_ISSTRIP;

    // Check the indices are valid 
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(isStrip     <0 || isStrip     >=N_ISSTRIP    ) continue;

    m_timeWindowOffset[iStationName][stationEta][isStrip] = timeWindowOffset;

    // If it is the end of the file, get out from while loop.   
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_timeWindowOffset.dat file 
  ifs.close();
}

void TgcDigitMaker::readFileOfAlignment() {
  // Indices to be used 
  int iStationName, stationEta, stationPhi;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
	m_alignmentZ[iStationName][stationEta][stationPhi] = 0.;
	m_alignmentT[iStationName][stationEta][stationPhi] = 0.;
	m_alignmentS[iStationName][stationEta][stationPhi] = 0.;
	m_alignmentTHS[iStationName][stationEta][stationPhi] = 0.;
      }
    }
  }

  // Find path to the TGC_Digitization_alignment.dat file 
  const std::string fileName = "TGC_Digitization_alignment.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    msg(MSG::FATAL) << "readFileOfAlignment(): Could not find file " << fileName.c_str() << endmsg;
    return;
  }

  // Open the TGC_Digitization_alignment.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    msg(MSG::FATAL) << "readFileOfAlignment(): Could not open file " << fileName.c_str() << endmsg;
    return;
  }
    
  // Read the TGC_Digitization_alignment.dat file
  double tmpZ;
  double tmpT;
  double tmpS;
  double tmpTHS;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> tmpZ >> tmpT >> tmpS >> tmpTHS;
    if(msgLevel(MSG::DEBUG)) { 
      msg(MSG::DEBUG) << "TgcDigitMaker::readFileOfAlignment" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi 
		      << " z[mm]= " << tmpZ  
		      << " t[mm]= " << tmpT  
		      << " s[mm]= " << tmpS
		      << " ths[rad]= " << tmpTHS  
		      << endmsg;
    }

    // Subtract offsets to use indices of m_alignmentZ, m_alignmentT, m_alignmentTHS arrays
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;

    // Check the indices are valid 
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;

    m_alignmentZ[iStationName][stationEta][stationPhi]   = tmpZ;
    m_alignmentT[iStationName][stationEta][stationPhi]   = tmpT;
    m_alignmentS[iStationName][stationEta][stationPhi]   = tmpS;
    m_alignmentTHS[iStationName][stationEta][stationPhi] = tmpTHS;

    // If it is the end of the file, get out from while loop.   
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_timeWindowOffset.dat file 
  ifs.close();
}

double TgcDigitMaker::getEnergyThreshold(const std::string stationName, int stationEta, int stationPhi, int gasGap, int isStrip) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array 
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  gasGap       -= OFFSET_GASGAP; 
  isStrip      -= OFFSET_ISSTRIP;

  double energyThreshold = +999999.;

  // If the indices are valid, the energyThreshold array is fetched.  
  if((iStationName>=0 && iStationName<N_STATIONNAME) && 
     (stationEta  >=0 && stationEta  <N_STATIONETA ) && 
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     ) && 
     (isStrip     >=0 && isStrip     <N_ISSTRIP    )) {  
    energyThreshold = m_energyThreshold[iStationName][stationEta][stationPhi][gasGap][isStrip];
  }

  // Show the energy threshold value
  if(msgLevel(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "TgcDigitMaker::getEnergyThreshold" 
		      << " stationName= " << iStationName+OFFSET_STATIONNAME
		      << " stationEta= " << stationEta+OFFSET_STATIONETA 
		      << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
		      << " gasGap= " << gasGap+OFFSET_GASGAP 
		      << " isStrip= " << isStrip+OFFSET_ISSTRIP
		      << " energyThreshold(MeV)= " << energyThreshold
		      << endmsg;
  }

  return energyThreshold;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TgcDigitMaker::randomCrossTalk(const Identifier elemId,
                                    const int gasGap,
                                    const int isStrip,
                                    const int channel,
                                    const float posInChan,
                                    const double digitTime,
                                    CLHEP::HepRandomEngine* rndmEngine) 
{
  int stationName = m_idHelper->stationName(elemId) - OFFSET_STATIONNAME;
  int stationEta  = m_idHelper->stationEta(elemId)  - OFFSET_STATIONETA;
  int stationPhi  = m_idHelper->stationPhi(elemId)  - OFFSET_STATIONPHI;
  int iGasGap     = gasGap                          - OFFSET_GASGAP; 
  int iIsStrip    = isStrip                         - OFFSET_ISSTRIP; 

  double prob1CrossTalk  = 0.;
  double prob11CrossTalk = 0.;
  double prob20CrossTalk = 0.;
  double prob21CrossTalk = 0.;

  if((stationName>=0 && stationName<N_STATIONNAME) &&
     (stationEta >=0 && stationEta <N_STATIONETA ) &&
     (stationPhi >=0 && stationPhi <N_STATIONPHI ) &&
     (iGasGap    >=0 && iGasGap    <N_GASGAP     )) {
    prob1CrossTalk  = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][iIsStrip][0];
    prob11CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][iIsStrip][1];
    prob20CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][iIsStrip][2];
    prob21CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][iIsStrip][3];
  }

  int nCrossTalks_neg = 0; 
  int nCrossTalks_pos = 0; 

  if(posInChan < prob1CrossTalk) {
    nCrossTalks_neg = 1;  // 1-0
  } else if(posInChan > 1.-prob1CrossTalk) {
    nCrossTalks_pos = 1;  // 0-1
  } else {
    double prob = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);
    if(prob < prob11CrossTalk/(1.-2.*prob1CrossTalk)) {
      nCrossTalks_neg = 1; nCrossTalks_pos = 1;  // 1-1
    } else if(prob < (prob20CrossTalk + prob11CrossTalk) / (1.-2.*prob1CrossTalk)) { 
      if(posInChan<0.5) { nCrossTalks_neg = 2; } // 2-0
      else              { nCrossTalks_pos = 2; } // 0-2
    } else {
      if(prob < (prob20CrossTalk + prob11CrossTalk + 2.*prob21CrossTalk)/(1.-2.*prob1CrossTalk)) {
	if(posInChan<0.5) { nCrossTalks_neg = 2; nCrossTalks_pos = 1; } // 2-1
	else              { nCrossTalks_neg = 1; nCrossTalks_pos = 2; } // 1-2
      }
    }
  } 

  if(nCrossTalks_neg==0 && nCrossTalks_pos==0) return; // No cross-talk case 

  // No time structure is implemented yet. 
  double dt = digitTime; 
  uint16_t bctag = bcTagging(dt, isStrip); 

  // obtain max channel number
  Identifier thisId = m_idHelper->channelID(elemId, gasGap, isStrip, channel);
  int maxChannelNumber = m_idHelper->channelMax(thisId);

  for(int jChan=channel-nCrossTalks_neg; jChan<=channel+nCrossTalks_pos; jChan++) {
    if(jChan == channel || jChan < 1 || jChan > maxChannelNumber) continue;

    Identifier newId = m_idHelper->channelID(elemId, gasGap, isStrip, jChan);
    addDigit(newId, bctag); // TgcDigit can be duplicated. 
  }
}

bool TgcDigitMaker::isDeadChamber(const std::string stationName, int stationEta, int stationPhi, int gasGap) {
  bool v_isDeadChamber = true;

  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array 
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  gasGap       -= OFFSET_GASGAP; 

  // If the indices are valid, the energyThreshold array is fetched.  
  if((iStationName>=0 && iStationName<N_STATIONNAME) && 
     (stationEta  >=0 && stationEta  <N_STATIONETA ) && 
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     )) {  
    v_isDeadChamber = m_isDeadChamber[iStationName][stationEta][stationPhi][gasGap];
  }

  // Show the energy threshold value
  if(msgLevel(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "TgcDigitMaker::getEnergyThreshold" 
		      << " stationName= " << iStationName+OFFSET_STATIONNAME
		      << " stationEta= " << stationEta+OFFSET_STATIONETA 
		      << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
		      << " gasGap= " << gasGap+OFFSET_GASGAP 
		      << " isDeadChamber= " << v_isDeadChamber
		      << endmsg;
  }

  return v_isDeadChamber; 
}

double TgcDigitMaker::getTimeWindowOffset(const std::string stationName, int stationEta, int isStrip) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  isStrip      -= OFFSET_ISSTRIP;

  // Check the indices are valid
  if(iStationName<0 || iStationName>=N_STATIONNAME) return 0.;
  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return 0.;
  if(isStrip     <0 || isStrip     >=N_ISSTRIP    ) return 0.;
  
  // Values were determined to reproduce the fraction of Previous BC hit fraction in Z->mumu data during Periods B,D,E in 2011.
  return m_timeWindowOffset[iStationName][stationEta][isStrip];
}

int TgcDigitMaker::getIStationName(const std::string stationName) const {
  int iStationName = 0;
  if(     stationName=="T1F") iStationName = 41;
  else if(stationName=="T1E") iStationName = 42;
  else if(stationName=="T2F") iStationName = 43;
  else if(stationName=="T2E") iStationName = 44;
  else if(stationName=="T3F") iStationName = 45;
  else if(stationName=="T3E") iStationName = 46;
  else if(stationName=="T4F") iStationName = 47;
  else if(stationName=="T4E") iStationName = 48;
  
  return iStationName;
}

void TgcDigitMaker::adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
				       const Amg::Vector3D direCos, Amg::Vector3D &localPos) const {
  int iStationName = getIStationName(stationName);
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  // Check the indices are valid
  if(iStationName<0 || iStationName>=N_STATIONNAME) return;
  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return;
  if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) return;
  
  // Local +x (-x) direction is global +z direction on A-side (C-side). 
  double localDisplacementX = m_alignmentT[iStationName][stationEta][stationPhi]; 

  // Local +z direction is global +r direction.  
  double localDisplacementZ = m_alignmentZ[iStationName][stationEta][stationPhi]; 

  // Local +s direction is global +phi direction.  
  double localDisplacementY = m_alignmentS[iStationName][stationEta][stationPhi]; 

  // Rotation around the s-axis is not implemented yet (2011/11/29).
  // m_alignmentTHS[tmpStationName][tmpStationEta][tmpStationPhi];

  // Convert local x translation to local y and z translations 
  double localDisplacementYByX = 0.;
  double localDisplacementZByX = 0.;
  if(fabs(localDisplacementX)>1.0E-12) { // local y and z translations are non-zero only if local x translation is non-zero. 
    if(fabs(direCos[0])<1.0E-12) return; // To avoid zero-division 
    localDisplacementYByX = direCos[1]/direCos[0]*localDisplacementX;  
    localDisplacementZByX = direCos[2]/direCos[0]*localDisplacementX;  
  }
  localPos.y() = localPos.y()+localDisplacementYByX+localDisplacementY; 
  localPos.z() = localPos.z()+localDisplacementZByX-localDisplacementZ; 
}
