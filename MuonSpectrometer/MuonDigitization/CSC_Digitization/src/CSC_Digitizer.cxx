/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, February 19 2004 : move to MuonGeoModel
// Digitization routine for the CSC hits
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "HepMC/GenParticle.h"
#include "GeneratorObjects/HepMcParticleLink.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CSC_Digitization/CSC_Digitizer.h"
//#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Random/RandPoisson.h"

using namespace MuonGM;

// Constructors
CSC_Digitizer::CSC_Digitizer(CscHitIdHelper * cscHitHelper,
			     const MuonDetectorManager* muonMgr,
                             ICscCalibTool* pcalib) :
  m_cscIdHelper(NULL),
  m_Polia(0.0),
  m_timeWindowLowerOffset(0.0),
  m_timeWindowUpperOffset(0.0),
  m_bunchTime(0.0),
  m_amplification(0.0)
{
  m_cscHitHelper        = cscHitHelper;
  m_muonMgr             = muonMgr;
  m_sprob               = 0;
  std::string  gVersion = m_muonMgr->geometryVersion();
  std::cout << "CSC_Digitiser::initialize() : geometeryVersion = " << gVersion << std::endl;
  m_pointingPhiStrips   = (gVersion == "P03");
  m_pcalib              = pcalib;
  m_debug =0;

  m_driftVelocity = 60; // / (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns // 0.06
  m_NInterFromEnergyLoss = true;

  m_electronEnergy = 66;  // unit is eV 
  
  m_maxElectron = 21; // max electron per interaction :not configurable 
}

// Public methods
StatusCode CSC_Digitizer::initialize() {
  
  // initialize random number generators
  // then initialize the CSC identifier helper
  // This method must be called before looping over the hits
  // to digitize them using the method digitize_hit below
  m_bunchTime = 0.0;

  // initialize random number generators
  //  double average_int = 30;  // average interactions per cm
  //  m_FlatDist = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);
  //  m_GaussDist = CLHEP::RandGauss::shoot(rndmEngine,0.0,1.0);  
  //  m_GammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0+m_Polia), 1.0); 
  //  m_PoissonDist = CLHEP::RandPoisson::shoot(rndmEngine, average_int); 
  
  // initialize the CSC identifier helper
  m_cscIdHelper = m_muonMgr->cscIdHelper();
  //if (!m_cscIdHelper) return status;
  
  // calculate the probability densities
  //  m_maxElectron = 21; // max number of electrons per interaction
  double prob[] = {79.4, 12.,   3.4,   1.6,   0.95,
                   0.6,  0.44,  0.34,  0.27,  0.21,
                   0.17, 0.13,  0.1,   0.08,  0.06,
                   0.05, 0.042, 0.037, 0.033, 0.029,
                   0.};
  
  //  std::cout << "CSC_Digitizer::intialize() : DriftVelocity" << m_driftVelocity << std::endl;
  //  std::cout << "CSC_Digitizer::intialize() : MaxElectron  " << m_maxElectron << std::endl;

  m_sprob = new double[m_maxElectron];
  if (m_sprob == 0) {
    std::cout << "CSC_Digitizer::initialize() : cannot request memory from the heap"  << std::endl;
    return StatusCode::FAILURE;
  }
  double sump = 0;
  for (int i=0; i<m_maxElectron; i++) {
    sump += prob[i];
    *(m_sprob+i) = sump;
  }

  //intialize hash offsets
  if (m_pointingPhiStrips) {
    m_hashOffset[0][0] = 0;
    m_hashOffset[0][1] = 27392;
    m_hashOffset[1][0] = m_hashOffset[0][1]+3584;
    m_hashOffset[1][1] = m_hashOffset[0][1]+m_hashOffset[1][0];    
  } else {
    std::string  gVersion = m_muonMgr->geometryVersion();
    m_hashOffset[0][0] = 0;
    m_hashOffset[0][1] = 24576;
    m_hashOffset[1][0] = m_hashOffset[0][1]+6144;
    m_hashOffset[1][1] = m_hashOffset[0][1]+m_hashOffset[1][0];
  } 
  return StatusCode::SUCCESS;
}


////////////////////////////////////////////
/// New way as of 10/2010
//////////////////
StatusCode CSC_Digitizer::digitize_hit (const CSCSimHit * cscHit, 
					std::vector<IdentifierHash>& hashVec,
					std::map<IdentifierHash,std::vector<float> >& data_SampleMap,
					std::map<IdentifierHash,std::vector<float> >& data_SampleMapOddPhase,
					CLHEP::HepRandomEngine* rndmEngine) {
  // method to digitize a single hit. Must be called in a loop over hits
  // after a call to the method "StatusCode CSC_Digitize::initialize()" 
  // get the step vector from the begining and end vectors of the G4 step

  Amg::Vector3D startHit = cscHit->getHitStart();
  Amg::Vector3D endHit   = cscHit->getHitEnd();

  // decode the hit ID
  const int hitId          = cscHit->CSCid();
  std::string stationName  = m_cscHitHelper->GetStationName(hitId);
  int eta                  = m_cscHitHelper->GetZSector(hitId);
  int phi                  = m_cscHitHelper->GetPhiSector(hitId);
  int chamberLayer         = m_cscHitHelper->GetChamberLayer(hitId);
  int wireLayer            = m_cscHitHelper->GetWireLayer(hitId);

  int chamberType = -1;
  if (stationName == "CSS") {
    chamberType = 0;
  }
  else if (stationName == "CSL") {
    chamberType = 1; 
  }
  else {
    std::cout <<"CSC_Digitizer::ERROR : Invalid station name = " 
	      << stationName << std::endl;
    return StatusCode::FAILURE;
  }

  int istation = (chamberType) ? 2 : 1;
  int sector = eta*(2*phi - istation +1);
  
  int etaIndex = -1;
  if (eta == -1) 
    etaIndex = 1;
  else if (eta == +1) 
    etaIndex = 2;
  else {
    std::cout << "CSC_Digitizer::ERROR : Invalid station eta = " 
	      << eta << std::endl;
    return StatusCode::FAILURE;
  }

  // find the detector descriptor
  const CscReadoutElement * descriptor = m_muonMgr->getCscReadoutElement(chamberType,etaIndex-1,
									 phi-1,chamberLayer-1);
  if (!descriptor) {
    std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " 
	      << std::endl;
    return StatusCode::FAILURE;
  }

  // get some chamber external/internal paramters from CSC descriptor
  // the dead zone width = 2*roxacell width. In AMDB-Q, it will be 
  // 2*(wire bar position + Full Gas Gap Bar Width)
  // but in any case for the short and the long widths, 
  // one has to correct for the angle: 1 mm (see the drawing) --- KAA

  // float shortLength      = descriptor->lengthUpToMaxWidth(); // not used anywhere
  // float shortWidth       = descriptor->shortWidth(); // not used anywhere
  // float longWidth        = descriptor->longWidth(); // not used anywhere
  float length           = descriptor->length();
  float roxacellWidth    = descriptor->roxacellWidth();

  /*
    std::cout << "CSC_Digitizer::digitize_hit : Chamber parameters " 
    << "shortLength = " << shortLength
    << " shortWidth = " << shortWidth
    << " longWidth  = " << longWidth
    << " fullLength = " << length
    << " excent     = " << excent
    << " chamberType = " << chamberType
    << std::endl;
  */

  // float beta = atan ( (descriptor->longWidth()-descriptor->shortWidth())
  //                      /(2.*descriptor->lengthUpToMaxWidth()) ); // not used anywhere
  // shortWidth = descriptor->shortWidth() - 2*roxacellWidth * (1-sin(beta))/cos(beta); // not used anywhere
  // longWidth =  descriptor->longWidth() - 2*roxacellWidth * (1+sin(beta))/cos(beta); // not used anywhere
  length = descriptor->length() - 2*roxacellWidth;

  // if (chamberType == 0) {
  //   shortLength = descriptor->lengthUpToMaxWidth()-2*roxacellWidth; // not used anywhere
  // } else {
  //   double alpha = atan ( (descriptor->excent()-descriptor->length())
  // 			  /(descriptor->longWidth()/2.) );
  //   double shortLongWidth = descriptor->longWidth() *
  //                           (descriptor->excent()-descriptor->length())
  //                           /(descriptor->excent()-descriptor->lengthUpToMaxWidth());
  //   double gslWidth = shortLongWidth - 2*roxacellWidth*(1-cos(alpha))/sin(alpha);
  //   shortLength  = length - (longWidth-gslWidth)*tan(alpha)/2.; // not used anywhere
  // }

  // particle ID
  int ipart = cscHit->particleID();

  //if (fabs(endHit.x()) > anodeCathodeDist) endHit.x() = anodeCathodeDist;
   
  Amg::Vector3D stepHit  = Amg::Vector3D( (endHit.x()-startHit.x()),
					  (endHit.y()-startHit.y()),
					  (endHit.z()-startHit.z()) ); 
  double step = sqrt( stepHit.x()*stepHit.x() + 
                      stepHit.y()*stepHit.y() +
                      stepHit.z()*stepHit.z() );  // path length in a layer                                            
  
  if (step == 0) return StatusCode::SUCCESS;
  
  // get average number of interaction::one interaction for photons
  //  std::cout << "The particle ID = " << ipart << std::endl;
  
  int nInter=0;
  if (ipart == 1)
    nInter = 1;
  else {
    if (m_NInterFromEnergyLoss) {
      double energyLoss =cscHit->energyDeposit(); // MeV peaking at 0.001MeV for on time track (25ns from IP) 
      double elecEnergy =m_electronEnergy*0.000001; // each ionized electron has 66eV (66 is from 0.001MeV/15 (average interaction per layer 0.5cm) 
      nInter = energyLoss/elecEnergy;
    } else {
      double average_int = 30;  // average interactions per cm
      double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
      nInter = int(step*pois/10.0 + 0.5); //  number of interaction according to Poisson
      if (m_debug) std::cout << "[CSC_Digitizer::digitize_hit(NEW)] nInter info from random number pois:step:nInter  "
                             << pois << " " << step << " " << nInter << std::endl;
    }
  }
  
  if (nInter <= 0) return StatusCode::SUCCESS;
  
  double xc;
  double yc;
  double zc;
  double wireCharge;
  int nElectrons=0;


  // loop over number of interactions and do digitization
  for (int i=0; i<nInter; i++) {
    double t = 0.0;
    if (ipart == 1 || ipart == 999) t = step/2.0; // one interaction for photons & geantinos
    else t = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);                 // for other particles
    xc = startHit.x() + t*stepHit.x();
    yc = startHit.y() + t*stepHit.y();
    zc = startHit.z() + t*stepHit.z();

    // pileup - check if within time window
    double preDriftTime = getDriftTime(descriptor,Amg::Vector3D(xc,yc,zc)); // before geant hit time addition.
    double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime; 

    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " " << cscHit->meanTime() << " "
                << preDriftTime << " " << m_bunchTime << std::endl;
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] start hit coordinates (xc,yc,zc) / step hit and t / the hit are "
                << startHit.x() << " " << startHit.y() << " " << startHit.z() 
                << "    "
                << stepHit.x() << " " << stepHit.y() << " " << stepHit.z() << "   " << t 
                << "    "
                << xc << " " << yc << " " << zc << std::endl;
    }
    
    // Prompt muon case::
    // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
    // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

    // it should be decided by bunchtime. -400 to +200
    if (outsideWindow(m_bunchTime)) continue; 
    
    // number of electrons in this interaction
    double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);
    double p = *(m_sprob+m_maxElectron-1) * flat;
    for (int k=0; k < m_maxElectron; k++) {
      if (p <= *(m_sprob+k) ) {
	nElectrons = k+1;
	break;
      }
    }
    

    // find the charge on the wire in electron-equivalent charge

    m_Polia = 0.38;           // parmeter for charge loss:: random gamma function	 
    double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0+m_Polia), 1.0);
    wireCharge = qWire(nElectrons,gammaDist);
 
    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] flat:p:maxEle:nEle:gammaDist:wireCharge "
                << flat << " " << p << " " << m_maxElectron << " "
                << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
    }
    
    //now digitize hits for R- and phi-strips
    for (int measuresPhi=0; measuresPhi<=1; measuresPhi++) {

      float stripWidth   = descriptor->cathodeReadoutPitch(chamberLayer,measuresPhi);
      const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi,stripWidth);

      if (m_debug) {
        std::cout << "[CSC_Digitizer::digitize_hit(NEW)] CSC_Digitizer::digitize_hit: Chamber Parameters" 
                  << " chamberType= "  << chamberType << " wireLayer= "    << wireLayer << " measuresPhi= "  << measuresPhi
                  << " stripWidth= " << stripWidth  << " maxStrip= "   << maxStrip  << std::endl;
      }
      
      // now digitize hits for R-strips/phi-strips
  
      if (measuresPhi == 0) {  // R-strips :: find the charge+noise
	double zz = zc+maxStrip*stripWidth/2.0;   // assuming zero offset
	int strip = int(zz/stripWidth)+1;
	// find the charges induced on the phi strips
        for (int j=strip-4; j<=strip+4; j++) {
	  double zpos = stripWidth*j - stripWidth/2.0;
	  if (j > 0 && j <= maxStrip) {
	    //double stripCharge = wireCharge * qStripR(zpos-zz) / 2.0;
	    double stripCharge = wireCharge * qStripR((zpos-zz)/stripWidth,stationName) / 2.0;
            //	    if (stripCharge < 0) stripCharge = 0.0;

            if (m_debug) 
              std::cout << "[CSC_Digitizer::digitize_hit(NEW)] zpos = " << zpos << " zz = " << zz << " diff = " << fabs(zz-zpos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl;

            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMap);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMapOddPhase, 1);
            //            IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];
            //	    fillMaps (hashId, stripCharge, hashVec, data_map);
            if (m_debug)
              std::cout << "[CSC_Digitizer::digitize_hit(NEW)] DriftTimeVSstripCharge " << driftTime << " " << stripCharge << std::endl;

          }
	}
      } else {                // Phi-strips :: find the charge+noise 
	double yy   = 0.0;
	int strip = 0;

	if (m_pointingPhiStrips) { // pointing phi-strips or not
	  //double z0 = shortLength*shortWidth/(longWidth-shortWidth);
	  double z0 = descriptor->z0();
	  yy = (z0+length/2)*yc/(z0+length/2+zc) // center of phi-strip assuming zero offset
	    + maxStrip*stripWidth/2; //trasnsform for strip counting to start at 1
	  //+ shortWidth*(z0+shortLength/2)/(2*z0); //trasnsform for strip counting to start at 1
	  strip = int (yy/stripWidth)+1;
	} else {
	  yy = yc+maxStrip*stripWidth/2;               // non-pointing phi-strips :: assuming zero offset
	  if (eta > 0) yy = -yc+maxStrip*stripWidth/2; // non-pointing phi-strips :: assuming zero offset
	  strip = int (yy/stripWidth)+1;
	}

	// find the charges induced on the phi strips
	for (int j=strip-1; j<=strip+1; j++) {
	  if (j > 0 && j <= maxStrip) {  
	    double ypos = stripWidth*(j-0.5);
	    double stripCharge = wireCharge * qStripPhi(ypos-yy,stationName) / 2.0; 
            if (m_debug)
              std::cout << "[CSC_Digitizer::digitize_hit(NEW)] ypos = " << ypos << " yy = " << yy << " diff = " << fabs(yy-ypos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl; 

            //            unsigned int corrStripId = j-1; // 0-47
            //            if (etaIndex ==2) {
            //              corrStripId = 47-corrStripId;
            //            }
            
            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMap);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMapOddPhase, 1);

            //	    IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];
            //	    if (stripCharge < 0) stripCharge = 0.0;
            //	    fillMaps(hashId, stripCharge, hashVec, data_map);

          }
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}






StatusCode CSC_Digitizer::digitize_hit (const CSCSimHit * cscHit, 
					std::vector<IdentifierHash>& hashVec,
					std::map<IdentifierHash,std::vector<float> >& data_SampleMap,
					CLHEP::HepRandomEngine* rndmEngine) {
  // method to digitize a single hit. Must be called in a loop over hits
  // after a call to the method "StatusCode CSC_Digitize::initialize()" 
  // get the step vector from the begining and end vectors of the G4 step
  Amg::Vector3D startHit = cscHit->getHitStart();
  Amg::Vector3D endHit   = cscHit->getHitEnd();

  // decode the hit ID
  const int hitId          = cscHit->CSCid();
  std::string stationName  = m_cscHitHelper->GetStationName(hitId);
  int eta                  = m_cscHitHelper->GetZSector(hitId);
  int phi                  = m_cscHitHelper->GetPhiSector(hitId);
  int chamberLayer         = m_cscHitHelper->GetChamberLayer(hitId);
  int wireLayer            = m_cscHitHelper->GetWireLayer(hitId);

  int chamberType = -1;
  if (stationName == "CSS") {
    chamberType = 0;
  }
  else if (stationName == "CSL") {
    chamberType = 1; 
  }
  else {
    std::cout <<"CSC_Digitizer::ERROR : Invalid station name = " 
	      << stationName << std::endl;
    return StatusCode::FAILURE;
  }

  int istation = (chamberType) ? 2 : 1;
  int sector = eta*(2*phi - istation +1);
  
  int etaIndex = -1;
  if (eta == -1) 
    etaIndex = 1;
  else if (eta == +1) 
    etaIndex = 2;
  else {
    std::cout << "CSC_Digitizer::ERROR : Invalid station eta = " 
	      << eta << std::endl;
    return StatusCode::FAILURE;
  }

  // find the detector descriptor
  const CscReadoutElement * descriptor = m_muonMgr->getCscReadoutElement(chamberType,etaIndex-1,
									 phi-1,chamberLayer-1);
  if (!descriptor) {
    std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " 
	      << std::endl;
    return StatusCode::FAILURE;
  }

  // get some chamber external/internal paramters from CSC descriptor
  // the dead zone width = 2*roxacell width. In AMDB-Q, it will be 
  // 2*(wire bar position + Full Gas Gap Bar Width)
  // but in any case for the short and the long widths, 
  // one has to correct for the angle: 1 mm (see the drawing) --- KAA

  // float shortLength      = descriptor->lengthUpToMaxWidth();    // not used anywhere
  // float shortWidth       = descriptor->shortWidth(); // not used anywhere
  // float longWidth        = descriptor->longWidth();  // not used anywhere
  float length           = descriptor->length();
  float roxacellWidth    = descriptor->roxacellWidth();

  /*
    std::cout << "CSC_Digitizer::digitize_hit : Chamber parameters " 
    << "shortLength = " << shortLength
    << " shortWidth = " << shortWidth
    << " longWidth  = " << longWidth
    << " fullLength = " << length
    << " excent     = " << excent
    << " chamberType = " << chamberType
    << std::endl;
  */

  // float beta = atan ( (descriptor->longWidth()-descriptor->shortWidth())
  //                      /(2.*descriptor->lengthUpToMaxWidth()) ); // not used anywhere
  // shortWidth = descriptor->shortWidth() - 2*roxacellWidth * (1-sin(beta))/cos(beta); // not used anywhere
  // longWidth =  descriptor->longWidth() - 2*roxacellWidth * (1+sin(beta))/cos(beta); // not used anywhere
  length = descriptor->length() - 2*roxacellWidth;

  // if (chamberType == 0) {
  //   shortLength = descriptor->lengthUpToMaxWidth()-2*roxacellWidth;// not used anywhere
  // } else {
  //   double alpha = atan ( (descriptor->excent()-descriptor->length())
  // 			  /(descriptor->longWidth()/2.) );
  //   double shortLongWidth = descriptor->longWidth() *
  //                           (descriptor->excent()-descriptor->length())
  //                           /(descriptor->excent()-descriptor->lengthUpToMaxWidth());
  //   double gslWidth = shortLongWidth - 2*roxacellWidth*(1-cos(alpha))/sin(alpha);
  //   shortLength  = length - (longWidth-gslWidth)*tan(alpha)/2.;// not used anywhere
  // }

  // particle ID
  int ipart = cscHit->particleID();

  //if (fabs(endHit.x()) > anodeCathodeDist) endHit.x() = anodeCathodeDist;
   
  Amg::Vector3D stepHit  = Amg::Vector3D( (endHit.x()-startHit.x()),
					  (endHit.y()-startHit.y()),
					  (endHit.z()-startHit.z()) ); 
  double step = sqrt( stepHit.x()*stepHit.x() + 
                      stepHit.y()*stepHit.y() +
                      stepHit.z()*stepHit.z() );                                              

  //std::cout << "step size = " << step << std::endl;

  /*  
      std::cout << "Gas Layer=" << wireLayer << " Start of Hit: " << " x=" << startHit.x() << " y="
      << startHit.y() << " z=" << startHit.z() << std::endl;

      std::cout << "Gas Layer=" << wireLayer << " End of Hit: " << " x=" << endHit.x() << " y="
      << endHit.y() << " z=" << endHit.z() << " stepLength=" << step << std::endl;

      std::cout << " " << std::endl;
  */

  if (step == 0) return StatusCode::SUCCESS;

  // get average number of interaction::one interaction for photons
  //  std::cout << "The particle ID = " << ipart << std::endl;

  int nInter=0;
  if (ipart == 1)
    nInter = 1;
  else {
    double average_int = 30;  // average interactions per cm
    double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
    nInter = int(step*pois/10.0 + 0.5); //  number of interaction according to Poisson
    if (m_debug) std::cout << "[CSC_Digitizer::digitize_hit(NEW)] nInter info from random number pois:step:nInter  "
			   << pois << " " << step << " " << nInter << std::endl;
  }


  if (nInter <= 0) return StatusCode::SUCCESS;

  //std::cout << "The number of interactions is " << nInter << std::endl;

  double xc;
  double yc;
  double zc;
  double wireCharge;
  int nElectrons=0;

  // loop over number of interactions and do digitization
  for (int i=0; i<nInter; i++) {
    double t = 0.0;
    if (ipart == 1 || ipart == 999) t = step/2.0; // one interaction for photons & geantinos
    else t = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);                 // for other particles
    xc = startHit.x() + t*stepHit.x();
    yc = startHit.y() + t*stepHit.y();
    zc = startHit.z() + t*stepHit.z();

    // pileup - check if within time window
    double preDriftTime = getDriftTime(descriptor,Amg::Vector3D(xc,yc,zc)); // before geant hit time addition.
    double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime; 

    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " " << cscHit->meanTime() << " "
                << preDriftTime << " " << m_bunchTime << std::endl;
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] start hit coordinates (xc,yc,zc) / step hit and t / the hit are "
                << startHit.x() << " " << startHit.y() << " " << startHit.z() 
                << "    "
                << stepHit.x() << " " << stepHit.y() << " " << stepHit.z() << "   " << t 
                << "    "
                << xc << " " << yc << " " << zc << std::endl;
    }
    
    // Prompt muon case::
    // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
    // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

    // it should be decided by bunchtime. -400 to +200
    if (outsideWindow(m_bunchTime)) continue; 
    
    // number of electrons in this interaction
    double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);
    double p = *(m_sprob+m_maxElectron-1) * flat;
    for (int k=0; k < m_maxElectron; k++) {
      if (p <= *(m_sprob+k) ) {
	nElectrons = k+1;
	break;
      }
    }
    

    // find the charge on the wire in electron-equivalent charge

    m_Polia = 0.38;           // parmeter for charge loss:: random gamma function	 
    double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0+m_Polia), 1.0);
    wireCharge = qWire(nElectrons,gammaDist);
 
    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit(NEW)] flat:p:maxEle:nEle:gammaDist:wireCharge "
                << flat << " " << p << " " << m_maxElectron << " "
                << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
    }
    
    //now digitize hits for R- and phi-strips
    for (int measuresPhi=0; measuresPhi<=1; measuresPhi++) {

      float stripWidth   = descriptor->cathodeReadoutPitch(chamberLayer,measuresPhi);
      const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi,stripWidth);

      if (m_debug) {
        std::cout << "[CSC_Digitizer::digitize_hit(NEW)] CSC_Digitizer::digitize_hit: Chamber Parameters" 
                  << " chamberType= "  << chamberType << " wireLayer= "    << wireLayer << " measuresPhi= "  << measuresPhi
                  << " stripWidth= " << stripWidth  << " maxStrip= "   << maxStrip  << std::endl;
      }
      
      // now digitize hits for R-strips/phi-strips
  
      if (measuresPhi == 0) {  // R-strips :: find the charge+noise
	double zz = zc+maxStrip*stripWidth/2.0;   // assuming zero offset
	int strip = int(zz/stripWidth)+1;
	// find the charges induced on the phi strips
        for (int j=strip-4; j<=strip+4; j++) {
	  double zpos = stripWidth*j - stripWidth/2.0;
	  if (j > 0 && j <= maxStrip) {
	    //double stripCharge = wireCharge * qStripR(zpos-zz) / 2.0;
	    double stripCharge = wireCharge * qStripR((zpos-zz)/stripWidth,stationName) / 2.0;
	   
            //	    if (stripCharge < 0) stripCharge = 0.0;

            if (m_debug) 
              std::cout << "[CSC_Digitizer::digitize_hit(NEW)] zpos = " << zpos << " zz = " << zz << " diff = " << fabs(zz-zpos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl;

            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMap);
            //            IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];
            //	    fillMaps (hashId, stripCharge, hashVec, data_map);

            if (m_debug) std::cout << "[CSC_Digitizer::digitize_hit(NEW)] DriftTimeVSstripCharge " << driftTime << " " << stripCharge << std::endl;

          }
	}
      } else {                // Phi-strips :: find the charge+noise 
	double yy   = 0.0;
	int strip = 0;

	if (m_pointingPhiStrips) { // pointing phi-strips or not
	  //double z0 = shortLength*shortWidth/(longWidth-shortWidth);
	  double z0 = descriptor->z0();
	  yy = (z0+length/2)*yc/(z0+length/2+zc) // center of phi-strip assuming zero offset
	    + maxStrip*stripWidth/2; //trasnsform for strip counting to start at 1
	  //+ shortWidth*(z0+shortLength/2)/(2*z0); //trasnsform for strip counting to start at 1
	  strip = int (yy/stripWidth)+1;
	} else {
	  yy = yc+maxStrip*stripWidth/2;               // non-pointing phi-strips :: assuming zero offset
	  if (eta > 0) yy = -yc+maxStrip*stripWidth/2; // non-pointing phi-strips :: assuming zero offset
	  strip = int (yy/stripWidth)+1;
	}

	// find the charges induced on the phi strips
	for (int j=strip-1; j<=strip+1; j++) {
	  if (j > 0 && j <= maxStrip) {  
	    double ypos = stripWidth*(j-0.5);
	    double stripCharge = wireCharge * qStripPhi(ypos-yy,stationName) / 2.0; 
            if (m_debug)
              std::cout << "[CSC_Digitizer::digitize_hit(NEW)] ypos = " << ypos << " yy = " << yy << " diff = " << fabs(yy-ypos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl; 

            //            unsigned int corrStripId = j-1; // 0-47
            //            if (etaIndex ==2) {
            //              corrStripId = 47-corrStripId;
            //            }
            
            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);
	    fillSampleMaps (hashId, driftTime, stripCharge, hashVec, data_SampleMap);

            //	    IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];
            //	    if (stripCharge < 0) stripCharge = 0.0;
            //	    fillMaps(hashId, stripCharge, hashVec, data_map);

          }
	}
      }
    }
  }
  
  return StatusCode::SUCCESS;
}









////////////////////////////////////////////
/// Old way before 10/2010
//////////////////
StatusCode CSC_Digitizer::digitize_hit (const CSCSimHit * cscHit, 
					std::vector<IdentifierHash>& hashVec,
					std::map<IdentifierHash,std::pair<double,double> >& data_map,
					CLHEP::HepRandomEngine* rndmEngine) {
  // method to digitize a single hit. Must be called in a loop over hits
  // after a call to the method "StatusCode CSC_Digitize::initialize()" 
  // get the step vector from the begining and end vectors of the G4 step
  Amg::Vector3D startHit = cscHit->getHitStart();
  Amg::Vector3D endHit   = cscHit->getHitEnd();

  // decode the hit ID
  const int hitId          = cscHit->CSCid();
  std::string stationName  = m_cscHitHelper->GetStationName(hitId);
  int eta                  = m_cscHitHelper->GetZSector(hitId);
  int phi                  = m_cscHitHelper->GetPhiSector(hitId);
  int chamberLayer         = m_cscHitHelper->GetChamberLayer(hitId);
  int wireLayer            = m_cscHitHelper->GetWireLayer(hitId);

  
  int chamberType = -1;
  if (stationName == "CSS") {
    chamberType = 0;
  }
  else if (stationName == "CSL") {
    chamberType = 1; 
  }
  else {
    std::cout <<"CSC_Digitizer::ERROR : Invalid station name = " 
	      << stationName << std::endl;
    return StatusCode::FAILURE;
  }

  int istation = (chamberType) ? 2 : 1;
  int sector = eta*(2*phi - istation +1);

  
  int etaIndex = -1;
  if (eta == -1) 
    etaIndex = 1;
  else if (eta == +1) 
    etaIndex = 2;
  else {
    std::cout << "CSC_Digitizer::ERROR : Invalid station eta = " 
	      << eta << std::endl;
    return StatusCode::FAILURE;
  }

  // find the detector descriptor
  const CscReadoutElement * descriptor = m_muonMgr->getCscReadoutElement(chamberType,etaIndex-1,
									 phi-1,chamberLayer-1);
  if (!descriptor) {
    std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " 
	      << std::endl;
    return StatusCode::FAILURE;
  }

  // get some chamber external/internal paramters from CSC descriptor
  // the dead zone width = 2*roxacell width. In AMDB-Q, it will be 
  // 2*(wire bar position + Full Gas Gap Bar Width)
  // but in any case for the short and the long widths, 
  // one has to correct for the angle: 1 mm (see the drawing) --- KAA

  // float shortLength      = descriptor->lengthUpToMaxWidth();    // not used anywhere
  // float shortWidth       = descriptor->shortWidth(); // not used anywhere
  // float longWidth        = descriptor->longWidth();  // not used anywhere
  float length           = descriptor->length();
  float roxacellWidth    = descriptor->roxacellWidth();

  /*
    std::cout << "CSC_Digitizer::digitize_hit : Chamber parameters " 
    << "shortLength = " << shortLength
    << " shortWidth = " << shortWidth
    << " longWidth  = " << longWidth
    << " fullLength = " << length
    << " excent     = " << excent
    << " chamberType = " << chamberType
    << std::endl;
  */

  // float beta = atan ( (descriptor->longWidth()-descriptor->shortWidth())
  //                      /(2.*descriptor->lengthUpToMaxWidth()) ); // not used anywhere
  // shortWidth = descriptor->shortWidth() - 2*roxacellWidth * (1-sin(beta))/cos(beta); // not used anywhere
  // longWidth =  descriptor->longWidth() - 2*roxacellWidth * (1+sin(beta))/cos(beta); // not used anywhere
  length = descriptor->length() - 2*roxacellWidth;

  // if (chamberType == 0) {
  //   shortLength = descriptor->lengthUpToMaxWidth()-2*roxacellWidth;// not used anywhere
  // } else {
  //   double alpha = atan ( (descriptor->excent()-descriptor->length())
  // 			  /(descriptor->longWidth()/2.) );
  //   double shortLongWidth = descriptor->longWidth() *
  //                           (descriptor->excent()-descriptor->length())
  //                           /(descriptor->excent()-descriptor->lengthUpToMaxWidth());
  //   double gslWidth = shortLongWidth - 2*roxacellWidth*(1-cos(alpha))/sin(alpha);
  //   shortLength  = length - (longWidth-gslWidth)*tan(alpha)/2.;// not used anywhere
  // }

  // particle ID
  int ipart = cscHit->particleID();

  //if (fabs(endHit.x()) > anodeCathodeDist) endHit.x() = anodeCathodeDist;
   
  Amg::Vector3D stepHit  = Amg::Vector3D( (endHit.x()-startHit.x()),
					  (endHit.y()-startHit.y()),
					  (endHit.z()-startHit.z()) ); 
  double step = sqrt( stepHit.x()*stepHit.x() + 
                      stepHit.y()*stepHit.y() +
                      stepHit.z()*stepHit.z() );                                              
  
  //std::cout << "step size = " << step << std::endl;
  
  /*  
      std::cout << "Gas Layer=" << wireLayer << " Start of Hit: " << " x=" << startHit.x() << " y="
      << startHit.y() << " z=" << startHit.z() << std::endl;

      std::cout << "Gas Layer=" << wireLayer << " End of Hit: " << " x=" << endHit.x() << " y="
      << endHit.y() << " z=" << endHit.z() << " stepLength=" << step << std::endl;

      std::cout << " " << std::endl;
  */

  if (step == 0) return StatusCode::SUCCESS;

  // get average number of interaction::one interaction for photons
  //  std::cout << "The particle ID = " << ipart << std::endl;

  int nInter=0;
  if (ipart == 1)
    nInter = 1;
  else {
    double average_int = 30;  // average interactions per cm
    double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
    nInter = int(step*pois/10.0 + 0.5); //  number of interaction according to Poisson
    if (m_debug)
      std::cout << "[CSC_Digitizer::digitize_hit] nInter info from random number pois:step:nInter  "
                << pois << " " << step << " " << nInter << std::endl;
  }


  if (nInter <= 0) return StatusCode::SUCCESS;

  //std::cout << "The number of interactions is " << nInter << std::endl;

  double xc;
  double yc;
  double zc;
  double wireCharge;
  int nElectrons=0;

  // loop over number of interactions and do digitization
  for (int i=0; i<nInter; i++) {
    double t = 0.0;
    if (ipart == 1 || ipart == 999) t = step/2.0; // one interaction for photons & geantinos
    else t = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);                 // for other particles
    xc = startHit.x() + t*stepHit.x();
    yc = startHit.y() + t*stepHit.y();
    zc = startHit.z() + t*stepHit.z();

    // pileup - check if within time window
    double preDriftTime = getDriftTime(descriptor,Amg::Vector3D(xc,yc,zc)); // before geant hit time addition.
    double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime; 

    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " " << cscHit->meanTime() << " "
                << preDriftTime << " " << m_bunchTime << std::endl;
      std::cout << "[CSC_Digitizer::digitize_hit] start hit coordinates (xc,yc,zc) / step hit and t / the hit are "
                << startHit.x() << " " << startHit.y() << " " << startHit.z() 
                << "    "
                << stepHit.x() << " " << stepHit.y() << " " << stepHit.z() << "   " << t 
                << "    "
                << xc << " " << yc << " " << zc << std::endl;
    }
    
    // Prompt muon case::
    // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
    // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

    // it should be decided by bunchtime. -400 to +200
    if (outsideWindow(m_bunchTime)) continue; 
    
    // number of electrons in this interaction
    double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0,1.0);
    double p = *(m_sprob+m_maxElectron-1) * flat;
    for (int k=0; k < m_maxElectron; k++) {
      if (p <= *(m_sprob+k) ) {
	nElectrons = k+1;
	break;
      }
    }
    

    // find the charge on the wire in electron-equivalent charge
    m_Polia = 0.38;           // parmeter for charge loss:: random gamma function	 
    double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0+m_Polia), 1.0);
    wireCharge = qWire(nElectrons,gammaDist);
 
    if (m_debug) {
      std::cout << "[CSC_Digitizer::digitize_hit] flat:p:maxEle:nEle:gammaDist:wireCharge "
                << flat << " " << p << " " << m_maxElectron << " "
                << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
    }
    
    //now digitize hits for R- and phi-strips
    for (int measuresPhi=0; measuresPhi<=1; measuresPhi++) {

      float stripWidth   = descriptor->cathodeReadoutPitch(chamberLayer,measuresPhi);
      const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi,stripWidth);

      if (m_debug) {
        std::cout << "[CSC_Digitizer::digitize_hit] CSC_Digitizer::digitize_hit: Chamber Parameters" 
                  << " chamberType= "  << chamberType
                  << " wireLayer= "    << wireLayer 
                  << " measuresPhi= "  << measuresPhi
                  << " stripWidth= " << stripWidth
                  << " maxStrip= "   << maxStrip
                  << std::endl;
      }
      
      // now digitize hits for R-strips/phi-strips
  
      if (measuresPhi == 0) {  // R-strips :: find the charge+noise
	double zz = zc+maxStrip*stripWidth/2.0;   // assuming zero offset
	int strip = int(zz/stripWidth)+1;
	// find the charges induced on the phi strips
        
        for (int j=strip-4; j<=strip+4; j++) {
	  double zpos = stripWidth*j - stripWidth/2.0;
	  if (j > 0 && j <= maxStrip) {
	    //double stripCharge = wireCharge * qStripR(zpos-zz) / 2.0;
	    double stripCharge = wireCharge * qStripR((zpos-zz)/stripWidth,stationName) / 2.0;
            //	    if (stripCharge < 0) stripCharge = 0.0;

            if (m_debug) 
              std::cout << "[CSC_Digitizer::digitize_hit] zpos = " << zpos << " zz = " << zz << " diff = " << fabs(zz-zpos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl;

            //            IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];
            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);

            //	    fillMaps (hashId, stripCharge, hashVec, data_map);
	    fillMaps (hashId, driftTime, stripCharge, hashVec, data_map);

            if (m_debug)
              std::cout << "[CSC_Digitizer::digitize_hit] DriftTimeVSstripCharge " << driftTime << " " << stripCharge << std::endl;

          }
	}
      } else {                // Phi-strips :: find the charge+noise 
	double yy   = 0.0;
	int strip = 0;

	if (m_pointingPhiStrips) { // pointing phi-strips or not
	  //double z0 = shortLength*shortWidth/(longWidth-shortWidth);
	  double z0 = descriptor->z0();
	  yy = (z0+length/2)*yc/(z0+length/2+zc) // center of phi-strip assuming zero offset
	    + maxStrip*stripWidth/2; //trasnsform for strip counting to start at 1
	  //+ shortWidth*(z0+shortLength/2)/(2*z0); //trasnsform for strip counting to start at 1
	  strip = int (yy/stripWidth)+1;
	} else {
	  yy = yc+maxStrip*stripWidth/2;               // non-pointing phi-strips :: assuming zero offset
	  if (eta > 0) yy = -yc+maxStrip*stripWidth/2; // non-pointing phi-strips :: assuming zero offset
	  strip = int (yy/stripWidth)+1;
	}

	// find the charges induced on the phi strips
	for (int j=strip-1; j<=strip+1; j++) {
	  if (j > 0 && j <= maxStrip) {  
	    double ypos = stripWidth*(j-0.5);
	    double stripCharge = wireCharge * qStripPhi(ypos-yy,stationName) / 2.0; 
            if (m_debug)
              std::cout << "[CSC_Digitizer::digitize_hit] ypos = " << ypos << " yy = " << yy << " diff = " << fabs(yy-ypos)
                        << " sector= "  << sector
                        << " wireLayer= "    << wireLayer
                        << " strip = " << j << " charge = " << stripCharge << std::endl; 

            //            unsigned int corrStripId = j-1; // 0-47
            //            if (etaIndex ==2) {
            //              corrStripId = 47-corrStripId;
            //            }
            
            IdentifierHash hashId = getHashId(eta, phi, chamberLayer, chamberType, wireLayer, j, maxStrip, measuresPhi);

            //	    IdentifierHash hashId = (j-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
            //              +8*maxStrip*(phi-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];

            fillMaps(hashId, driftTime, stripCharge, hashVec, data_map);
            //	    if (stripCharge < 0) stripCharge = 0.0;
            //	    fillMaps(hashId, stripCharge, hashVec, data_map);


            //            std::cout <<  i << "th int. " << j << "th str. " << driftTime << " " << stripCharge <<  std::endl;


          }
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}


