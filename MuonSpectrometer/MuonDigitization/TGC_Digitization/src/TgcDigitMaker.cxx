/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcDigitMaker.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "AthenaBaseComps/AthCheckMacros.h"
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
  : AthMessaging (Athena::getMessageSvc(), "TgcDigitMaker")
{
  m_hitIdHelper             = hitIdHelper;
  m_mdManager               = mdManager;
  m_runperiod               = runperiod;
  m_idHelper                = nullptr;
  m_efficiency[kWIRE] = m_efficiency[kSTRIP] = 1.000; // 100% efficiency for TGCSimHit_p1
  m_timeWindowOffsetSensor[kWIRE]  = m_timeWindowOffsetSensor[kSTRIP] = 0.;
  m_gateTimeWindow[kOUTER][kWIRE]  = 29.32; // 29.32ns = 26ns + 4  * 0.83ns(outer station)
  m_gateTimeWindow[kOUTER][kSTRIP] = 40.94; // 40.94ns = 26ns + 18 * 0.83ns(outer station)
  m_gateTimeWindow[kINNER][kWIRE]  = 33.47; // 33.47ns = 26ns + 9  * 0.83ns(inner station)
  m_gateTimeWindow[kINNER][kSTRIP] = 45.09; // 45.09ns = 26ns + 23 * 0.83ns(inner station)
  m_bunchCrossingTime              = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
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
  
  ATH_CHECK(readFileOfTimeJitter());

  // Read share/TGC_Digitization_energyThreshold.dat file and store values in m_energyThreshold. 
  ATH_CHECK(readFileOfEnergyThreshold());

  // Read share/TGC_Digitization_crossTalk.dat file and store values in m_crossTalk.
  ATH_CHECK(readFileOfCrossTalk());

  // Read share/TGC_Digitization_deadChamber.dat file and store values in m_isDeadChamber. 
  ATH_CHECK(readFileOfDeadChamber());

  // Read share/TGC_Digitization_timeWindowOffset.dat file and store values in m_timeWindowOffset. 
  ATH_CHECK(readFileOfTimeWindowOffset());

  // Read share/TGC_Digitization_alignment.dat file and store values in m_alignmentZ, m_alignmentT, m_alignmentS, m_alignmentTHS
  ATH_CHECK(readFileOfAlignment());

  // Read share/TGC_Digitization_ASDpropTimeOffset.dat file and store values in m_ASDpropTimeOffset, m_maxch.
  ATH_CHECK(readFileOfASDpropTimeOffset());

  // Read share/TGC_Digitization_StripPosition.dat file and store values in m_StripPosition.
  ATH_CHECK(readFileOfStripPosition());

  return StatusCode::SUCCESS;
}

//---------------------------------------------------
// Execute Digitization
//---------------------------------------------------
TgcDigitCollection* TgcDigitMaker::executeDigi(const TGCSimHit* hit,
                                               const double globalHitTime,
                                               const TgcDigitASDposData* ASDpos,
                                               CLHEP::HepRandomEngine* rndmEngine)
{
  //////////  convert ID for this digitizer system 
  int         Id         = hit->TGCid();
  std::string stationName= m_hitIdHelper->GetStationName(Id);
  int         stationEta = m_hitIdHelper->GetStationEta(Id);
  int         stationPhi = m_hitIdHelper->GetStationPhi(Id);
  int         ilyr       = m_hitIdHelper->GetGasGap(Id);
  ATH_MSG_DEBUG("executeDigi() Got HIT Id.");

  // Check the chamber is dead or not. 
  if(isDeadChamber(stationName, stationEta, stationPhi, ilyr)) return nullptr;

  const Identifier elemId = m_idHelper -> elementID(stationName,stationEta,stationPhi);
  ATH_MSG_DEBUG("executeDigi() - element identifier is: " << m_idHelper->show_to_string(elemId));

  const MuonGM::TgcReadoutElement *tgcChamber = m_mdManager->getTgcReadoutElement(elemId);
  if(!tgcChamber) {
    ATH_MSG_WARNING("executeDigi() - no ReadoutElement found for " << m_idHelper->show_to_string(elemId));
    return nullptr;
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
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC Digit collection "
						 << "context begin_index = " << tgcContext.begin_index()
						 << " context end_index  = " << tgcContext.end_index()
						 << " the identifier is " << elemId);
    elemId.show();
  }
  
  std::unique_ptr<TgcDigitCollection> digits = std::make_unique<TgcDigitCollection>(elemId, coll_hash);

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

  int iStationName = getIStationName(stationName);

  /////////////   wire group number calculation   ///////////////
  TgcSensor sensor = kWIRE;
  m_timeWindowOffsetSensor[sensor] = getTimeWindowOffset(stationName, stationEta, sensor);

  double energyDeposit = hit->energyDeposit(); // Energy deposit in MeV 
  // If TGCSimHit_p1 is used, energyDeposit is the default value of -9999. 
  // If TGCSimHit_p2 is used, energyDeposit is equal to or greater than 0. 
  // Therefore, the energyDeposit value can be used to distinguish TGCSimHit_p1 and TGCSimHit_p2. 
  // For TGCSimHit_p1, old efficiency check with only isStrip variable is used. 
  // For TGCSimHit_p2, new efficiency check with chamber dependent energy threshold is used. 

  if((energyDeposit< -1. && efficiencyCheck(sensor, rndmEngine)) || // Old efficiencyCheck for TGCSimHit_p1. 
     (energyDeposit>=-1. && efficiencyCheck(stationName, stationEta, stationPhi, ilyr, sensor, energyDeposit)) // New efficiencyCheck for TGCSimHit_p2
     ) {  

    int iWireGroup[2];
    float posInWireGroup[2] = {0., 0.};
    for(int iPosition=0; iPosition<2; iPosition++) {
      int nWireOffset = (std::abs(stationEta) == 5 || stationName.compare(0,2,"T4") == 0) ? 1 : 0;
      // for chambers in which only the first wire  is not connected                                 : 1
      // for chambers in which the first and last wires are not connected OR all wires are connected : 0

      double zPosInSensArea = zLocal + static_cast<double>(tgcChamber->getTotalWires(ilyr)-nWireOffset)*wirePitch/2.;

      // check a hit in the sensitive area
      if(zPosInSensArea < 0. || zPosInSensArea > tgcChamber->getTotalWires(ilyr)*wirePitch) {
        iWireGroup[iPosition] = 0;
        posInWireGroup[iPosition] = 0.;
	      ATH_MSG_DEBUG("executeDigi(): Wire: Hit position located at outside of a sensitive volume, "
						 << " id: " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << " position: " << zPosInSensArea
						 << " xlocal: " << zLocal 
						 << " dir cosine: " << direCos[0] << "/" << direCos[1] << "/" << direCos[2]
						 << " active region: " << height-frameZwidth*2.);
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
      ATH_MSG_DEBUG("executeDigi(): Multihits found in WIRE GROUPs:" << iWG[0] << " " << iWG[1]);
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
  float wTimeDiffByRadiusOfInner = this->timeDiffByCableRadiusOfInner(iStationName, stationPhi, iwg);
	float wDigitTime = bunchTime + jit + wirePropagationTime*(ySignPhi*yLocal + tgcChamber->chamberWidth(zLocal)/2.) + wTimeDiffByRadiusOfInner;
  float wOffset = m_timeWindowOffsetSensor[sensor];
  float wASDDis{-1000};
  float wSigPropTimeDelay{-1000};
  if (ASDpos != nullptr) {
    wASDDis = this->getDistanceToAsdFromSensor(ASDpos, iStationName, abs(stationEta), stationPhi, sensor, iwg, zLocal);
    float wCableDis = wASDDis + (ySignPhi*yLocal + tgcChamber->chamberWidth(zLocal)/2.)/1000.;
    wSigPropTimeDelay = this->getSigPropTimeDelay(wCableDis);
    wDigitTime += wSigPropTimeDelay + wASDDis * 5;
    wOffset += getASDpropTimeOffset(elemId, (int)sensor, iwg);
  }

	TgcStation station = (m_idHelper->stationName(elemId) > 46) ? kINNER : kOUTER;
        uint16_t bctag = bcTagging(wDigitTime,m_gateTimeWindow[station][sensor],wOffset);

    if(bctag == 0x0) {
	    ATH_MSG_DEBUG("WireGroup: digitized time is outside of time window. " << wDigitTime
			     << " bunchTime: " << bunchTime << " time jitter: " << jitter 
			     << " propagation time: " << wirePropagationTime*(ySignPhi*yLocal + height/2. + frameXwidth)
			     << " signal delay time: " << wSigPropTimeDelay
			     << " time difference by cable radius of inner station: " << wTimeDiffByRadiusOfInner
			     << " propagation time to the ASD from the sensor: " << wASDDis * 5);
	}
	else {
	  Identifier newId = m_idHelper -> channelID(stationName,stationEta,stationPhi,
						     ilyr, (int)sensor, iwg);
	  addDigit(newId,bctag, digits.get());

	  if(iwg==iWG[0]) {
	    randomCrossTalk(elemId, ilyr, sensor, iwg, posInWG[0], wDigitTime, rndmEngine, digits.get());
	  }	 
 
	  ATH_MSG_DEBUG("WireGroup: newid breakdown digitTime x/y/z direcos height_gang bctag: "
						   << newId << " " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr << "/"
						   << sensor << "/" << iwg << " " 
						   << wDigitTime << " " << localPos.x() << "/"  << localPos.y() << "/" << localPos.z() << " "
						   << direCos.x() << "/"  << direCos.y() << "/" << direCos.z() << " "
						   << height << " " << tgcChamber->getNWires(ilyr,iwg) << " "
						   << bctag);
  }
      }
      else {
	      ATH_MSG_DEBUG("Wire Gang id is out of range. id, x/y/z, height: " << iwg
						  << " " << localPos.x() << "/"  << localPos.y() << "/" << localPos.z() << " " << height);
      }
    }
  } // end of wire group calculation

  //////////////  strip number calculation  //////////////
  sensor = kSTRIP;
  m_timeWindowOffsetSensor[sensor] = getTimeWindowOffset(stationName, stationEta, sensor);

  if((ilyr != 2 || (stationName.compare(0,2,"T1") != 0)) && // no stip in middle layers of T1* 
     ((energyDeposit< -1. && efficiencyCheck(sensor, rndmEngine)) || // Old efficiencyCheck for TGCSimHit_p1.
      (energyDeposit>=-1. && efficiencyCheck(stationName, stationEta, stationPhi, ilyr, sensor, energyDeposit))) // New efficiencyCheck for TGCSimHit_p2
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
        ATH_MSG_DEBUG("Strip: Hit position located at outside of a sensitive volume, id, position, xlocal0/1, dir cosine: "
						 << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << zPos << " " << zLocal << " " << direCos[0] << "/" << direCos[1] << "/" << direCos[2]);
      }
      else if (zPos > height-frameZwidth*2.) {
        iStrip[iPosition] =  tgcChamber -> getNStrips(ilyr) + 1;
        posInStrip[iPosition] = 0.;
        ATH_MSG_DEBUG("Strip: Hit position located at outside of a sensitive volume, id, position, active region: "
						 << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr
						 << zPos << " " << height-frameZwidth*2.);
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

	  ATH_MSG_DEBUG("dphi, phiLocal, yLocal, zLocall+ height/2.+hmin: "
						   << dphi << " " << phiLocal << " " << yLocal << " " << zLocal+height/2.+hmin);

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
      ATH_MSG_DEBUG("Multihits found in STRIPs:" << iStr[0] << " " << iStr[1]);
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
  float sTimeDiffByRadiusOfInner = this->timeDiffByCableRadiusOfInner(iStationName, stationPhi, istr);
	float sDigitTime = bunchTime + jitter + stripPropagationTime*(height/2. + frameZwidth + zSignEta*zLocal) + sTimeDiffByRadiusOfInner;
  float sOffset = m_timeWindowOffsetSensor[sensor];
  float sASDDis{-1000};
  float sSigPropTimeDelay{-1000};
  if (ASDpos != nullptr) {
    sASDDis = this->getDistanceToAsdFromSensor(ASDpos, iStationName, abs(stationEta), stationPhi, sensor, istr, getStripPosition(stationName, abs(stationEta), istr));
    float sCableDis = sASDDis + (height/2. + frameZwidth + zSignEta*zLocal)/1000.;
    sSigPropTimeDelay = this->getSigPropTimeDelay(sCableDis);
    sDigitTime += sSigPropTimeDelay + sASDDis * 5;
    sOffset += getASDpropTimeOffset(elemId, (int)sensor, istr);
  }

	TgcStation station = (m_idHelper->stationName(elemId) > 46) ? kINNER : kOUTER;
	uint16_t bctag = bcTagging(sDigitTime,m_gateTimeWindow[station][sensor],sOffset);

	if(bctag == 0x0) {
	  ATH_MSG_DEBUG("Strip: Digitized time is outside of time window. " << sDigitTime
			    << " bunchTime: " << bunchTime << " time jitter: " << jitter 
			    << " propagation time: " << stripPropagationTime*(height/2. + frameZwidth + zSignEta*zLocal)
			    << " signal delay time: " << sSigPropTimeDelay
			    << " time difference by cable radius of inner station: " << sTimeDiffByRadiusOfInner
			    << " propagation time to the ASD from the sensor: " << sASDDis * 5);
	}
	else {
	  Identifier newId = m_idHelper -> channelID(stationName,stationEta,stationPhi,
						     ilyr, (int)sensor, istr);
	  addDigit(newId, bctag, digits.get());

	  if(istr==iStr[0]) {
	    randomCrossTalk(elemId, ilyr, sensor, iStr[0], posInStr[0], sDigitTime, rndmEngine, digits.get());
	  }

	  ATH_MSG_DEBUG("Strip: newid breakdown digitTime x/y/z direcos r_center bctag: "
						   << newId << " " << stationName << "/" << stationEta << "/" << stationPhi << "/" << ilyr << "/"
						   << sensor << "/" << istr << " "
						   << sDigitTime << " " << localPos.x() << "/" << localPos.y() << "/" << localPos.z() << " " 
						   << direCos.x() << "/"  << direCos.y() << "/" << direCos.z() << " "
						   << height/2.+hmin << " "
						   << bctag);
	}
      }
      else {
	      ATH_MSG_DEBUG("Strip id is out of range: gap, id: " << ilyr << " " << istr);
      }
    }
  } // end of strip number calculation
  
  return digits.release();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
StatusCode TgcDigitMaker::readFileOfTimeJitter()
{

  const char* const fileName = "TGC_Digitization_timejitter.dat";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  std::ifstream ifs;
  if (!fileWithPath.empty()) {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  if(ifs.bad()){
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not open file "<< fileName);
    return StatusCode::FAILURE;
  }

  int angle = 0;
  int bins = 0;
  int i = 0;
  float prob = 0.;
  bool verbose = msgLvl(MSG::VERBOSE);

  while(ifs.good()){
    ifs >> angle >> bins;
    if (ifs.eof()) break;
    ATH_MSG_VERBOSE("readFileOfTimeJitter(): Timejitter, angle, Number of bins, prob. dist.: " << angle << " " << bins << " ");
    m_vecAngle_Time.resize(i + 1);
    for (int j = 0; j < 41/*bins*/; j++) {
      ifs >> prob;
      m_vecAngle_Time[i].push_back(prob);
      if (j == 0 && verbose) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): ";
      if(verbose) msg(MSG::VERBOSE) << prob << " ";
    }
    if(verbose) msg(MSG::VERBOSE) << endmsg;
    i++;
  }
  ifs.close();
  return StatusCode::SUCCESS;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
float TgcDigitMaker::timeJitter(const Amg::Vector3D& direCosLocal, CLHEP::HepRandomEngine* rndmEngine) const 
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
bool TgcDigitMaker::efficiencyCheck(const TgcSensor sensor, CLHEP::HepRandomEngine* rndmEngine) const {
    if(CLHEP::RandFlat::shoot(rndmEngine,0.0,1.0) < m_efficiency[sensor]) return true;
    ATH_MSG_DEBUG("efficiencyCheck(): Hit removed for sensor= " << sensor << "(0=WIRE,1=STRIP)");
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool TgcDigitMaker::efficiencyCheck(const std::string& stationName, const int stationEta, const int stationPhi, const int gasGap, const TgcSensor sensor, const double energyDeposit) const {
  // If the energy deposit is equal to or greater than the threshold value of the chamber, 
  // return true. 
  return (energyDeposit >= getEnergyThreshold(stationName, stationEta, stationPhi, gasGap, sensor));
}
//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t TgcDigitMaker::bcTagging(const double digitTime, const double window, const double offset) const {

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
void TgcDigitMaker::addDigit(const Identifier id, const uint16_t bctag, TgcDigitCollection* digits) const {
  if ((bctag & 0x1) != 0) {
    bool duplicate = false;
    for (const auto digit : *digits) {
      if (id==digit->identify() && TgcDigit::BC_PREVIOUS==digit->bcTag()) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      std::unique_ptr<TgcDigit> multihitDigit = std::make_unique<TgcDigit>(id,TgcDigit::BC_PREVIOUS);
      digits -> push_back(multihitDigit.release());
    }
  }
  if ((bctag & 0x2) != 0) {
    bool duplicate = false;
    for (const auto digit : *digits) {
      if (id==digit->identify() && TgcDigit::BC_CURRENT==digit->bcTag()) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      std::unique_ptr<TgcDigit> multihitDigit = std::make_unique<TgcDigit>(id,TgcDigit::BC_CURRENT);
      digits -> push_back(multihitDigit.release());
    }
  }
  if ((bctag & 0x4) != 0) {
    bool duplicate = false;
    for (const auto digit : *digits) {
      if (id==digit->identify() && TgcDigit::BC_NEXT==digit->bcTag()) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      std::unique_ptr<TgcDigit> multihitDigit = std::make_unique<TgcDigit>(id,TgcDigit::BC_NEXT);
      digits -> push_back(multihitDigit.release());
    }
  }
  return;
}

StatusCode TgcDigitMaker::readFileOfEnergyThreshold() {
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
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_energyThreshold.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }
    
  double energyThreshold; 
  // Read the TGC_Digitization_energyThreshold.dat file
  while(ifs.good()) {
    ifs >> iStationName >> stationEta 
      	>> stationPhi 
        >> gasGap >> isStrip >> energyThreshold;
    ATH_MSG_DEBUG("readFileOfEnergyThreshold" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " isStrip= " << isStrip
		      << " energyThreshold(MeV)= " << energyThreshold);

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

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfCrossTalk() {
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
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfCrossTalk(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_crossTalk.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfCrossTalk(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }
    
  double crossTalk_10 = 0.; 
  double crossTalk_11 = 0.; 
  double crossTalk_20 = 0.; 
  double crossTalk_21 = 0.; 
  // Read the TGC_Digitization_crossTalk.dat file
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> /*stationPhi >>*/ gasGap >> isStrip >> crossTalk_10 >> crossTalk_11 >> crossTalk_20 >> crossTalk_21;
    ATH_MSG_DEBUG("TgcDigitMaker::readFileOfCrossTalk" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
	//		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " isStrip= " << isStrip
		      << " prob(10) " << crossTalk_10
		      << " prob(11) " << crossTalk_11
		      << " prob(20) " << crossTalk_20
		      << " prob(21) " << crossTalk_21);

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

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfDeadChamber() {
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
    ATH_MSG_ERROR("Run Period " << m_runperiod << " is unexpected in TgcDigitMaker - using NOdeadChamber configuration.");
    return StatusCode::FAILURE;
  }
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_deadChamber.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }
    
  // Read the TGC_Digitization_deadChamber.dat file
  unsigned int nDeadChambers = 0;
  std::string comment;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> gasGap;
    bool valid = getline(ifs, comment).good(); 
    if(!valid) break; 

    ATH_MSG_DEBUG("TgcDigitMaker::readFileOfDeadChamber" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi
		      << " gasGap= " << gasGap 
		      << " comment= " << comment);

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

  ATH_MSG_INFO("readFileOfDeadChamber: the number of dead chambers = " << nDeadChambers);

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfTimeWindowOffset() {
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
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_timeWindowOffset.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }
    
  // Read the TGC_Digitization_timeWindowOffset.dat file
  double timeWindowOffset;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> isStrip >> timeWindowOffset;
    ATH_MSG_DEBUG("TgcDigitMaker::readFileOfTimeWindowOffset" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " isStrip= " << isStrip
		      << " timeWindowOffset= " << timeWindowOffset);

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

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfAlignment() {
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
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfAlignment(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_alignment.dat file 
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfAlignment(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }
    
  // Read the TGC_Digitization_alignment.dat file
  double tmpZ;
  double tmpT;
  double tmpS;
  double tmpTHS;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> tmpZ >> tmpT >> tmpS >> tmpTHS;
    ATH_MSG_DEBUG("readFileOfAlignment" 
		      << " stationName= " << iStationName  
		      << " stationEta= " << stationEta 
		      << " stationPhi= " << stationPhi 
		      << " z[mm]= " << tmpZ  
		      << " t[mm]= " << tmpT  
		      << " s[mm]= " << tmpS
		      << " ths[rad]= " << tmpTHS);

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

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfASDpropTimeOffset() {
  // Indices to be used  
  int iStationName = -1;
  int stationEta = -1;
  int isStrip = -1;
  int asdnum = -1;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(isStrip=0; isStrip<N_ISSTRIP; isStrip++) {
        for(asdnum=0; asdnum<N_ASDNUM; asdnum++) {
          m_maxch[iStationName][stationEta][isStrip][asdnum] = 0.;
          m_ASDpropTimeOffset[iStationName][stationEta][isStrip][asdnum] = 0.;
        }
      }
    }
  }

  // Find path to the TGC_Digitization_ASDpropTimeOffset.dat file                                                                           
  const std::string fileName = "TGC_Digitization_ASDpropTimeOffset.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    ATH_MSG_FATAL("readFileOfASDpropTimeOffset(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_ASDpropTimeOffset.dat file    
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfASDpropTimeOffset(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }

  double max_ch = 0.;
  double asd_propTimeOffset = 0.;
  // Read the TGC_Digitization_ASDpropTimeOffset.dat file   
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> isStrip >> asdnum >> max_ch >> asd_propTimeOffset;
    ATH_MSG_DEBUG("readFileOfASDpropTimeOffset"
                      << " stationName= " << iStationName
                      << " stationEta= " << stationEta
                      << " isStrip= " << isStrip
                      << " asdnum= " << asdnum
                      << " max_ch= " << max_ch
                      << " asd_propTimeOffset= " << asd_propTimeOffset);

    // Subtract offsets to use indices of ASDpropTimeOffset array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    isStrip      -= OFFSET_ISSTRIP;
    asdnum       -= OFFSET_ASDNUM;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(isStrip     <0 || isStrip     >=N_ISSTRIP    ) continue;
    if(asdnum      <0 || asdnum      >=N_ASDNUM     ) continue;

    m_maxch[iStationName][stationEta][isStrip][asdnum] = max_ch;
    m_ASDpropTimeOffset[iStationName][stationEta][isStrip][asdnum] = asd_propTimeOffset;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the TGC_Digitization_ASDpropTimeOffset.dat file 
  ifs.close();

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitMaker::readFileOfStripPosition() {
  //Indices to be used
  int iStationName, stationEta, channel;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_ABSSTATIONETA; stationEta++) {
      for(channel=0; channel<N_STRIPCHANNEL; channel++) {
      m_StripPos[iStationName][stationEta][channel] = 0.;
      }
    }
  }

  // Find path to the TGC_Digitization_StripPosition.dat file                                                                         
  const std::string fileName = "TGC_Digitization_StripPosition.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath == "") {
    ATH_MSG_FATAL("readFileOfStripPosition(): Could not find file " << fileName);
    return StatusCode::FAILURE;
  }

  // Open the TGC_Digitization_StripPosition.dat file                                
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfStripPosition(): Could not open file " << fileName);
    return StatusCode::FAILURE;
  }

  // Read the TGC_Digitization_StripPosition.dat file                                 
  double strippos;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> channel >> strippos;
    ATH_MSG_DEBUG("readFileOfStripPosition"
                      << " stationName= " << iStationName
                      << " stationEta= " << stationEta
                      << " channel= " << channel
                      << " StripPosition= " << strippos);

    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_ABSSTATIONETA;
    channel      -= OFFSET_STRIPCHANNEL;

    // Check the indices are valid                                                                                                                     
    if(iStationName<0 || iStationName>=N_STATIONNAME  ) continue;
    if(stationEta  <0 || stationEta  >=N_ABSSTATIONETA) continue;
    if(channel     <0 || channel     >=N_STRIPCHANNEL ) continue;

    m_StripPos[iStationName][stationEta][channel] = strippos;
    // If it is the end of the file, get out from while loop.                                                                           
    if(ifs.eof()) break;
  }
  // Close the TGC_Digitization_StripPosition.dat file                                                                                                            
  ifs.close();

  return StatusCode::SUCCESS;
}

double TgcDigitMaker::getEnergyThreshold(const std::string& stationName, int stationEta, int stationPhi, int gasGap, const TgcSensor sensor) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array 
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  gasGap       -= OFFSET_GASGAP; 

  double energyThreshold = +999999.;

  // If the indices are valid, the energyThreshold array is fetched.  
  if((iStationName>=0 && iStationName<N_STATIONNAME) && 
     (stationEta  >=0 && stationEta  <N_STATIONETA ) && 
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     )) { 
    energyThreshold = m_energyThreshold[iStationName][stationEta][stationPhi][gasGap][sensor];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE("getEnergyThreshold" 
		      << " stationName= " << iStationName+OFFSET_STATIONNAME
		      << " stationEta= " << stationEta+OFFSET_STATIONETA 
		      << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
		      << " gasGap= " << gasGap+OFFSET_GASGAP 
		      << " sensor= " << sensor
		      << " energyThreshold(MeV)= " << energyThreshold);

  return energyThreshold;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void TgcDigitMaker::randomCrossTalk(const Identifier elemId,
                                    const int gasGap,
                                    const TgcSensor sensor,
                                    const int channel,
                                    const float posInChan,
                                    const double digitTime,
                                    CLHEP::HepRandomEngine* rndmEngine,
                                    TgcDigitCollection* digits) const
{
  int stationName = m_idHelper->stationName(elemId) - OFFSET_STATIONNAME;
  int stationEta  = m_idHelper->stationEta(elemId)  - OFFSET_STATIONETA;
  int stationPhi  = m_idHelper->stationPhi(elemId)  - OFFSET_STATIONPHI;
  int iGasGap     = gasGap                          - OFFSET_GASGAP; 

  double prob1CrossTalk  = 0.;
  double prob11CrossTalk = 0.;
  double prob20CrossTalk = 0.;
  double prob21CrossTalk = 0.;

  if((stationName>=0 && stationName<N_STATIONNAME) &&
     (stationEta >=0 && stationEta <N_STATIONETA ) &&
     (stationPhi >=0 && stationPhi <N_STATIONPHI ) &&
     (iGasGap    >=0 && iGasGap    <N_GASGAP     )) {
    prob1CrossTalk  = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][sensor][0];
    prob11CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][sensor][1];
    prob20CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][sensor][2];
    prob21CrossTalk = m_crossTalk[stationName][stationEta][stationPhi][iGasGap][sensor][3];
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
  TgcStation station = (stationName > 46) ? kINNER : kOUTER;
  uint16_t bctag = bcTagging(dt, m_gateTimeWindow[station][sensor], m_timeWindowOffsetSensor[sensor]);
  // obtain max channel number
  Identifier thisId = m_idHelper->channelID(elemId, gasGap, (int)sensor, channel);
  int maxChannelNumber = m_idHelper->channelMax(thisId);

  for(int jChan=channel-nCrossTalks_neg; jChan<=channel+nCrossTalks_pos; jChan++) {
    if(jChan == channel || jChan < 1 || jChan > maxChannelNumber) continue;

    Identifier newId = m_idHelper->channelID(elemId, gasGap, (int)sensor, jChan);
    addDigit(newId, bctag, digits); // TgcDigit can be duplicated. 
  }
}

bool TgcDigitMaker::isDeadChamber(const std::string& stationName, int stationEta, int stationPhi, int gasGap) {
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
  ATH_MSG_VERBOSE("TgcDigitMaker::getEnergyThreshold" 
		      << " stationName= " << iStationName+OFFSET_STATIONNAME
		      << " stationEta= " << stationEta+OFFSET_STATIONETA 
		      << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
		      << " gasGap= " << gasGap+OFFSET_GASGAP 
		      << " isDeadChamber= " << v_isDeadChamber);

  return v_isDeadChamber; 
}

double TgcDigitMaker::getTimeWindowOffset(const std::string& stationName, int stationEta, const TgcSensor sensor) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;

  // Check the indices are valid
  if(iStationName<0 || iStationName>=N_STATIONNAME) return 0.;
  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return 0.;
  
  // Values were determined to reproduce the fraction of Previous BC hit fraction in Z->mumu data during Periods B,D,E in 2011.
  return m_timeWindowOffset[iStationName][stationEta][sensor];
}

int TgcDigitMaker::getIStationName(const std::string& stationName) const {
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

void TgcDigitMaker::adHocPositionShift(const std::string& stationName, int stationEta, int stationPhi,
				       const Amg::Vector3D& direCos, Amg::Vector3D &localPos) const {
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

float TgcDigitMaker::getASDpropTimeOffset(const Identifier elemId,
					   const int isStrip,
					   const int channel) const
{
  int StationName = m_idHelper->stationName(elemId) - OFFSET_STATIONNAME;
  int stationEta  = m_idHelper->stationEta(elemId)  - OFFSET_STATIONETA;
  int stationPhi  = m_idHelper->stationPhi(elemId)  - OFFSET_STATIONPHI;
  int iIsStrip    = isStrip                         - OFFSET_ISSTRIP;

  std::array<double, N_ASDNUM> maxch{};
  double asdpropTimeOffset = 0.;

  if((StationName>=0 && StationName<N_STATIONNAME) &&
     (stationEta >=0 && stationEta <N_STATIONETA ) &&
     (stationPhi >=0 && stationPhi <N_STATIONPHI )) {
    for(int asdnum=0; asdnum<N_ASDNUM; asdnum++){
      maxch[asdnum] = m_maxch[StationName][stationEta][iIsStrip][asdnum];
    }
    for(int asdnum=0; asdnum<N_ASDNUM; asdnum++){
      if(maxch[asdnum] == 0) continue;
      if(asdnum==0 && 1 <= channel && channel <= maxch[0]) asdpropTimeOffset = m_ASDpropTimeOffset[StationName][stationEta][iIsStrip][asdnum];
      if(asdnum!=0 && maxch[asdnum-1] < channel && channel <= maxch[asdnum]) asdpropTimeOffset = m_ASDpropTimeOffset[StationName][stationEta][iIsStrip][asdnum];
    }
  }
  return asdpropTimeOffset;
}

float TgcDigitMaker::getStripPosition(const std::string& stationName, int stationEta, int channel) const {
  // Convert std::string stationName to int iStationName from 41 to 48                              
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array                      
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_ABSSTATIONETA;
  channel      -= OFFSET_STRIPCHANNEL;

  // Check the indices are valid                                                                  
  if(iStationName<0 || iStationName>=N_STATIONNAME  ) return 0.;
  if(stationEta  <0 || stationEta  >=N_ABSSTATIONETA) return 0.;
  if(channel     <0 || channel     >=N_STRIPCHANNEL ) return 0.;

  return m_StripPos[iStationName][stationEta][channel];
}

float TgcDigitMaker::timeDiffByCableRadiusOfInner(const int iStationName,
						  const int stationPhi,
						  const int channel) const {

  if(iStationName != 47 && iStationName != 48) return 0.0; // only EIFI station
  if(channel < 12 || (channel > 16 && channel < 27)) {
    int cablenum = (stationPhi >= 13) ? 25 - stationPhi : stationPhi;
    return 2.3 - 0.06 * cablenum;
  }
  return 0.0;
}

float TgcDigitMaker::getSigPropTimeDelay(const float cableDistance) const {

  return 0.0049 * std::pow(cableDistance, 2) + 0.0002 * cableDistance;
}

float TgcDigitMaker::getDistanceToAsdFromSensor(const TgcDigitASDposData* readCdo,
						const int iStationName,
						const int stationEta,
						const int stationPhi,
						const TgcSensor sensor,
						const int channel,
						const float position) const {

  int phiId = (iStationName >= 47) ? stationPhi : -99;
  int dbNum = -99;

  for(unsigned int i_dbNum=0;i_dbNum<readCdo->stationNum.size();i_dbNum++) {
    if(iStationName != readCdo->stationNum.at(i_dbNum)) continue;
    if(stationEta   != readCdo->stationEta.at(i_dbNum)) continue;
    if(phiId        != readCdo->stationPhi.at(i_dbNum)) continue;
    dbNum = i_dbNum;
    break;
  }

  unsigned int asdNum[TgcSensor::N_SENSOR];
  asdNum[TgcSensor::kSTRIP] = channel / TgcDigitASDposData::N_CHANNELINPUT_TOASD;
  asdNum[TgcSensor::kWIRE]  = channel / TgcDigitASDposData::N_CHANNELINPUT_TOASD + TgcDigitASDposData::N_STRIPASDPOS;

  float disToAsd = fabs( position*CLHEP::mm/CLHEP::m - readCdo->asdPos[ asdNum[sensor] ][dbNum] );

  return disToAsd;
}
