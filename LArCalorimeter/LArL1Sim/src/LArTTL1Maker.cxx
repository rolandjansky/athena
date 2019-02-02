/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Institut ......: ISN Grenoble                                        +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArTTL1Maker.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArRawEvent/LArTTL1.h"

#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArIdentifier/LArIdManager.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
//
// ........ Event Header Files:
//
#include "EventInfo/PileUpTimeEventIndex.h"
//
// ........ Gaudi needed includes
//
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "PathResolver/PathResolver.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"
// trigger time
#include "AthenaKernel/ITriggerTime.h"

//


using CLHEP::RandGaussZiggurat;


namespace {
const double crossingTime = 25 /* nanosecond */;
const double crossingRate = 1. / crossingTime;
}


LArTTL1Maker::LArTTL1Maker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
  , m_atRndmGenSvc("AtRndmGenSvc",name)
  , m_rndmEngineName("LArTTL1Maker")
  , m_rndmEngine(0)
  , m_ttSvc("CaloTriggerTowerService")
  , m_fSamplKey("LArfSampl")
  , m_hitmap(0)
  , m_EmTTL1ContainerName{"LArTTL1EM"}
  , m_HadTTL1ContainerName{"LArTTL1HAD"}
  , m_xxxHitContainerName{{std::string("LArHitEMB"),std::string("LArHitEMEC"), std::string("LArHitHEC"),std::string("LArHitFCAL")}}

// + -------------------------------------------------------------------- +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 09/01/2003                                          +
// + Subject: TTL1 Maker constructor                                     +
// + -------------------------------------------------------------------- +
{
//
// ........ default values of private data
//
  m_chronSvc              = 0;
  m_mergeSvc              = 0;
  m_useTriggerTime        = false;
  //m_triggerTimeToolName   = "CosmicTriggerTimeTool";
  //p_triggerTimeTool       = 0;
  m_useMapfromStore = true;

  m_BeginRunPriority      = 100;

  m_lvl1Helper            = 0;
  m_emHelper              = 0;
  m_hecHelper             = 0;
  m_fcalHelper            = 0;

  m_EmTTL1ContainerName      = "LArTTL1EM";
  m_HadTTL1ContainerName     = "LArTTL1HAD";

  //m_EmBarrelHitContainerName = "LArHitEMB";
  //m_EmEndCapHitContainerName = "LArHitEMEC";
  //m_HecHitContainerName      = "LArHitHEC";
  //m_ForWardHitContainerName  = "LArHitFCAL";

  m_NoiseOnOff               = true;
  m_PileUp                   = false;
  m_noEmCalibMode            = false;
  m_noHadCalibMode           = false;
  m_chronoTest               = false;
  m_debugThresh              = 5000.;

  m_calibCoeffEmb.resize(s_NBETABINS);
  m_calibCoeffEmec.resize(s_NBETABINS);
  m_calibCoeffHec.resize(s_NBETABINS);
  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    m_calibCoeffEmb[ieta]=1.;
    m_calibCoeffEmec[ieta]=1.;
    m_calibCoeffHec[ieta]=1.;
  }
  const int nEta = 4;
  m_calibCoeffFcalEm.resize(nEta);
  m_calibCoeffFcalHad.resize(nEta);
  for(int ieta=0 ; ieta<nEta ; ieta++) {
    m_calibCoeffFcalEm[ieta]=.03;
    m_calibCoeffFcalHad[ieta]=.03;
  }

  //
  // ........ declare the private data as properties
  //

  declareProperty("RndmSvc", m_atRndmGenSvc);

  declareProperty("EmBarrelHitContainerName", m_xxxHitContainerName[0]);
  declareProperty("EmEndCapHitContainerName",m_xxxHitContainerName[1]);
  declareProperty("HecHitContainerName",m_xxxHitContainerName[2]);
  declareProperty("ForWardHitContainerName",m_xxxHitContainerName[3]);

  declareProperty("EmTTL1ContainerName",m_EmTTL1ContainerName);
  declareProperty("HadTTL1ContainerName",m_HadTTL1ContainerName);

  declareProperty("NoiseOnOff",m_NoiseOnOff);

  declareProperty("PileUp",m_PileUp);
  declareProperty("UseTriggerTime",m_useTriggerTime);
  declareProperty("TriggerTimeToolName",m_triggerTimeTool);

  declareProperty("EmBarrelCalibrationCoeffs",m_calibCoeffEmb);
  declareProperty("EmEndCapCalibrationCoeffs",m_calibCoeffEmec);
  declareProperty("HECCalibrationCoeffs",m_calibCoeffHec);
  declareProperty("EmFcalCalibrationCoeffs",m_calibCoeffFcalEm);
  declareProperty("HadFcalCalibrationCoeffs",m_calibCoeffFcalHad);

  declareProperty("NoEmCalibrationMode",m_noEmCalibMode);
  declareProperty("NoHadCalibrationMode",m_noHadCalibMode);
  declareProperty("ChronoTest",m_chronoTest);
  declareProperty("DebugThreshold",m_debugThresh);

  declareProperty("useMapFromStore",m_useMapfromStore,"Use LArHitEMap already filled from detector store");

  declareProperty("TruthHitsContainer",m_truthHitsContainer="","Specify a value to get a pair of LArTTL1 containers with the truth hits in them");
  declareProperty("LArfSamplKey",m_fSamplKey);

//
return;
}


LArTTL1Maker::~LArTTL1Maker()
{  
}


StatusCode LArTTL1Maker::initialize()
{
// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  m_chronSvc = chronoSvc();

  ATH_MSG_INFO 	( "***********************************************" );
  ATH_MSG_INFO 	( "* Steering options for LArTTL1Maker algorithm *"  );
  ATH_MSG_INFO 	( "***********************************************" );
  //
  // ......... print the noise flag
  //
  if ( m_NoiseOnOff )
  {
    ATH_MSG_INFO ( "Electronic noise will be added in each TT for selected sub-detectors." );
  }
  else
  {
    ATH_MSG_INFO ( "No electronic noise added." );
  }

//
// ......... print the pile-up flag
//
  if (m_PileUp)
  {
    ATH_MSG_INFO ( "take events from PileUp service" );
  }
  else
  {
    ATH_MSG_INFO ( "no pile up" );
  }

//
// ......... print the trigger time flag
//
  if (m_useTriggerTime) 
  {
    ATH_MSG_INFO ( "use Trigger Time service " <<  m_triggerTimeTool );
  }
  else
  {
    ATH_MSG_INFO ( "no Trigger Time used" );
  }

//
// ......... print the calibration flags
//
// currently the calib modes are not used anymore -> turning INFO logs into DEBUG
  if (m_noEmCalibMode)
  {
    ATH_MSG_DEBUG
      ( "NO calibration mode chosen for EM towers "
            << " == technical option. Should not be used for physics !!! " );
  }
  else 
  {
    ATH_MSG_DEBUG ( "standard calibration chosen for EM towers" );
  }
    
  if (m_noHadCalibMode)
  {
    ATH_MSG_DEBUG
      ( "NO calibration mode chosen for HEC towers "
        << " == technical option. Should not be used for physics !!! " );
  }
  else {
    ATH_MSG_DEBUG ( "standard calibration mode chosen for HEC towers " );
  }

//
// locate the PileUpMergeSvc and initialize our local ptr
//
  if (m_PileUp) {
    ATH_CHECK( service("PileUpMergeSvc", m_mergeSvc) );
    ATH_MSG_DEBUG ( "PileUpMergeSvc successfully initialized" );
  }     
  
//
// .........retrieve tool computing trigger time if requested
//
  if (m_useTriggerTime && m_PileUp) {
    ATH_MSG_INFO 
      ( " In case of pileup, the trigger time subtraction is done in PileUpSvc " );
    ATH_MSG_INFO ( "  => LArTTL1Maker will not apply Trigger Time " );
    m_useTriggerTime = false;
  }

  if (m_useTriggerTime) {
    ATH_CHECK(m_triggerTimeTool.retrieve());
  }
  else {
    m_triggerTimeTool.disable();
  }
  

  //
  // ..... need LAr and CaloIdManager to retrieve all needed helpers
  //
  const CaloIdManager* caloMgr = nullptr;
  ATH_CHECK( detStore()->retrieve (caloMgr) );
  const LArIdManager*	 larMgr = nullptr;
  ATH_CHECK( detStore()->retrieve (larMgr) );
  
  //
  //..... need of course the LVL1 helper
  //
  m_lvl1Helper = caloMgr->getLVL1_ID();
  if (!m_lvl1Helper) {
    ATH_MSG_ERROR ( "Could not access CaloLVL1_ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Successfully accessed CaloLVL1_ID helper" );
  }
  
  //..... also need the LArEM helper (e.g. to deal with the barrel end part)
  ATH_CHECK( detStore()->retrieve(m_emHelper) );
  ATH_MSG_DEBUG ( "Successfully retrieved LArEM helper from DetectorStore" );
  
  //..... also need the LArHEC helper to avoid adding up energy from 4th compartment
  ATH_CHECK( detStore()->retrieve(m_hecHelper) );
  ATH_MSG_DEBUG ( "Successfully retrieved LArHEC helper from DetectorStore" );

  //..... also need the LArFCAL helper to use hash ids to store all gains
  ATH_CHECK( detStore()->retrieve(m_fcalHelper) );
  ATH_MSG_DEBUG ( "Successfully retrieved LArFCAL helper from DetectorStore" );
  
  ATH_CHECK( m_ttSvc.retrieve());
  
  // Incident Service: 
  IIncidentSvc* incSvc = nullptr;
  ATH_CHECK( service("IncidentSvc", incSvc) );
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun",  m_BeginRunPriority);


  ATH_CHECK( m_atRndmGenSvc.retrieve() );
  m_rndmEngine = m_atRndmGenSvc->GetEngine(m_rndmEngineName);
  if(!m_rndmEngine) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE ;
  }

  ATH_CHECK(m_fSamplKey.initialize());

  //Initialize read-handle keys
  if (!m_PileUp) {
    for (auto& dhk : m_xxxHitContainerName) {
      ATH_CHECK(dhk.initialize());
    }
  }

  
  ATH_CHECK(m_EmTTL1ContainerName.initialize());
  ATH_CHECK(m_HadTTL1ContainerName.initialize());


  ATH_MSG_DEBUG  ( "Initialization completed successfully"  );
  return StatusCode::SUCCESS;
}



void LArTTL1Maker::handle(const Incident& /* inc*/ )
{
  ATH_MSG_DEBUG ( "LArTTL1Maker handle()" );

  //
  // ...... init hit map 
  //
  if ( this->initHitMap() == StatusCode::FAILURE ) {
    ATH_MSG_ERROR ( " Error from initHitMap() " );
  }

  //
  // ...... Read auxiliary data files
  //
  if ( this->readAuxiliary() == StatusCode::FAILURE ) {
    ATH_MSG_ERROR ( " Error from readAuxiliary() " );
  }

  return;
}

StatusCode LArTTL1Maker::execute()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// + Subject: Make the TTL1s and put them into the container             +
// +                                                                      +
// +======================================================================+

  ATH_MSG_DEBUG ( "Begining of execution"  );

  //
  // ....... fill the LArHitEMap
  //
  if(m_chronoTest) {
    m_chronSvc->chronoStart( "fill LArHitEMap " );
  }
  
  SG::ReadCondHandle<ILArfSampl> fSamplhdl(m_fSamplKey);
  const ILArfSampl* fSampl=*fSamplhdl;


  int totHit=0;
  if ( this->fillEMap(totHit) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  ATH_MSG_DEBUG ( "total number of hits with |E|> 1.e-06 found = " << totHit );
  

  if(m_chronoTest) {
    m_chronSvc->chronoStop ( "fill LArHitEMap " );
    m_chronSvc->chronoPrint( "fill LArHitEMap " );
  }

  if (!m_useMapfromStore && totHit==0) {
    ATH_MSG_WARNING ( " No LAr hit in the event "  );
  }

  //
  // .....get the trigger time if requested
  //
  double trigtime=0;
  if (m_useTriggerTime) {
     trigtime = m_triggerTimeTool->time();
  }
  ATH_MSG_DEBUG ( "Trigger time used : " << trigtime );

  //
  // ....... create the LAr TTL1 Containers
  //

  SG::WriteHandle<LArTTL1Container> ttL1ContainerEm(m_EmTTL1ContainerName);
  ATH_CHECK(ttL1ContainerEm.record(std::make_unique<LArTTL1Container>()));

  SG::WriteHandle<LArTTL1Container> ttL1ContainerHad(m_HadTTL1ContainerName);
  ATH_CHECK(ttL1ContainerHad.record(std::make_unique<LArTTL1Container>()));
  
   LArTTL1Container *truth_ttL1ContainerEm=0;
   LArTTL1Container *truth_ttL1ContainerHad=0;
   if(m_truthHitsContainer.size()>0) {
      truth_ttL1ContainerEm = new LArTTL1Container();
      truth_ttL1ContainerHad = new LArTTL1Container();
   }


  // ... initialise vectors for sums of energy in each TT
  //
  unsigned int nbTT = (unsigned int)m_lvl1Helper->tower_hash_max() ;
  ATH_MSG_DEBUG ( "Max number of LAr Trigger Towers= " << nbTT  );
  std::vector<std::vector<float> > sumEnergy ;   // inner index = time slot (from 0 to visEvecSize-1)
  std::vector<std::vector<float> > sumEnergy2 ;  // to allow barrel/endcap separation in 15th TT + FCAL2/3
  sumEnergy.resize(nbTT);
  sumEnergy2.resize(nbTT);
  std::vector<float> ttSumE;
  int ttSumEvecSize=0;
  int refTime = 0;
  if(!m_PileUp) {
    ttSumEvecSize=2*crossingTime-1;
    refTime = crossingTime-1;
  } else {
    ttSumEvecSize=s_MAXSAMPLES+s_NBSAMPLES-1;
    refTime = s_MAXSAMPLES-1;
  }
  ATH_MSG_DEBUG
    ( "Number of time slots considered= " << ttSumEvecSize
      << " reference time= " << refTime );
  ttSumE.resize(ttSumEvecSize);
  for(unsigned int iTT=0;iTT<nbTT;iTT++){
    sumEnergy[iTT]=ttSumE;
    sumEnergy2[iTT]=ttSumE;
  }

  m_chronSvc->chronoStart( "LArTTL1Mk hit loop " );

  int it = 0;
  int it_end = m_hitmap->GetNbCells();
  ATH_MSG_DEBUG ( "Size of the hit map= " << it_end );
  
  //
  // .... loop on hit lists in the map
  // .... and fill sumEnergy vector according to hit membership to TT
  // .... (one sum per time slot for each TT)
  //
  float outOfTimeE=0.;
  int nOutOfTime=0;
  float inTimeE=0.;
  int nInTime=0;
  float printEthresh=20.;
  int nMissingGain=0;
  for( ; it!=it_end;++it) {
    LArHitList * hitlist = m_hitmap->GetCell(it);
    if (hitlist != 0 ) {
      
      const std::vector<std::pair<float,float> >* timeE = hitlist->getData();
      if (timeE->size() > 0 ) {
	Identifier  cellId = hitlist->getIdentifier();

	int specialCase=0;
	bool skipCell=false;
	//
	// ...  skip cells not summed up in LVL1 (end of barrel PS and 4th compartment of HEC)
	//
	if(!m_ttSvc->is_in_lvl1(cellId)) skipCell=true;

	if(!skipCell) {
	  //
	  // ...determine to which TT this channel belongs
	  //
	  Identifier ttId = m_ttSvc->whichTTID(cellId);

	  if(m_chronoTest) {
	    m_chronSvc->chronoStart( "retrieve RG " );
	  }
	  
	  //
	  // ....... determine the sampling fraction
	  //........ and the relative (layer) gains for this cellId
	  //	
	  const float cellSampFraction = fSampl->FSAMPL(cellId);
          const float inv_cellSampFraction = 1. / cellSampFraction;
	  //	  std::cout << "cellid, SF= " << m_lvl1Helper->show_to_string(cellId) << " " << cellSampFraction << std::endl;
	  float relGain = 0.;
	  float sinTheta = 0.;

 	  int region = m_lvl1Helper->region(ttId);
 	  int eta = m_lvl1Helper->eta(ttId);
 	  int Ieta = decodeInverse(region,eta);
	  
	  // case EM cell
	  if(m_lvl1Helper->is_lar_em(cellId)) {
	    bool barrelEnd = m_lvl1Helper->is_barrel_end(ttId);
	    std::vector<float> vecRG ;
	    if(m_emHelper->is_em_barrel(cellId)) {
	      // Barrel
	      sinTheta = m_sinThetaEmb[Ieta-1];  // Ieta starts at 1
	    } else {
	      // End-Cap
	      if(!barrelEnd) {
		sinTheta = m_sinThetaEmec[Ieta-1];			
	      } else {
		// patching the fact that TT offline ID is ambiguous
                // decodeInverse(eta,region) returns 15 (ok barrel) instead of 1 (ec value).
		sinTheta = m_sinThetaEmec[0];			
		specialCase = 1;
                ATH_MSG_VERBOSE ( " special case " 
                                  << m_emHelper->show_to_string(ttId) );
	      }
	    }
	    relGain = 1.;  // no relative gain for EMB and EMEC
	  }
	  
	  // case HEC cell
	  else if(m_lvl1Helper->is_lar_hec(cellId)) {
	    sinTheta = m_sinThetaHec[Ieta-1];  // Ieta starts at 1
	    relGain = 1.;
	  }
	  
	  // case FCAL cell
	  else if(m_lvl1Helper->is_lar_fcal(cellId)) {
	    IdentifierHash fcalHash = m_fcalHelper->channel_hash(cellId);
	    relGain = m_cellRelGainFcal [fcalHash] ;
	    if(relGain < 0.001)  {
	      nMissingGain++;
	      ATH_MSG_WARNING ( " No relative gain value found for FCAL cell " 
		     << m_emHelper->show_to_string(cellId) << " (index " << fcalHash  
                                << "), setting default value 1. " );
	      relGain = 1. ;
	    }
	    sinTheta = 1.;   // this factor is included in the relative gain
	  }
	  if(m_chronoTest) {
	    m_chronSvc->chronoStop( "retrieve RG " );
	    m_chronSvc->chronoPrint( "retrieve RG " );
	  }
	  
 	  IdentifierHash ttHash = m_lvl1Helper->tower_hash(ttId) ;
	  
	  //
	  // .... loop on hits in hit list
	  //
	  std::vector<std::pair<float,float> >::const_iterator first = timeE->begin();
	  std::vector<std::pair<float,float> >::const_iterator last  = timeE->end();
	  while (first != last) {
	    float hitEnergy = (*first).first;
	    float hitTime   = (*first).second - trigtime;
	    if(hitTime>99000.) {
	      if(hitEnergy > printEthresh) {
		ATH_MSG_WARNING 
                  ( " Found pathological hit time, cellId= " << m_emHelper->show_to_string(cellId) 
                    << "  time= " << hitTime 
                    << "  energy= " << hitEnergy );
	      }
	    //  provisionally fix a bug in PileUpEvent. Normally not needed starting from 10.5.0
	      hitTime-=100000.;
	    }
	    // remove pathological energies (found in some Grid simulated DC2/Rome samples)
	    if (fabs(hitEnergy)>1e+9) {
	      ATH_MSG_WARNING ( " Pathological energy ignored cellId= "
                                << m_emHelper->show_to_string(cellId) << "  energy= " << hitEnergy );
	      hitEnergy = 0.;
	      hitTime = 0.;
	    }
	    //
	    // ....determine time with respect to ref shape 
	    //
	    int iShift = 0;
	    if(!m_PileUp) {
	      // keep 1ns granularity
	      //iShift = (int)(hitTime+0.5);
	      iShift = static_cast<int>(floor(hitTime+0.5));
	    } else {
	      // round to 25ns
	      //iShift = (int)(hitTime/25.+0.5);
	      iShift = static_cast<int>(floor(hitTime*crossingRate+0.5));
	    }
	    //
	    // ....make time positive to allow using it as an index
	    //
	    int iTime = iShift + refTime ;
	    //
	    // .... keep only hits in the timing window
	    //
	    if(iTime>=0 && iTime < ttSumEvecSize ) {
	      
	      if(!specialCase) {
		// standard case
		// ....... make the energy sum
		ttSumE = sumEnergy[ttHash];
		ttSumE[iTime]         += hitEnergy * inv_cellSampFraction * sinTheta * relGain;
		sumEnergy[ttHash] = ttSumE;
	      }
	      else  {
		// ec part of barrel-end or FCAL3
		// ....... make the energy sum
		ttSumE = sumEnergy2[ttHash];
		ttSumE[iTime]          += hitEnergy * inv_cellSampFraction * sinTheta * relGain;
		sumEnergy2[ttHash] = ttSumE;
	      }
	      //	      msglog << MSG::VERBOSE << "applied relative layer gain " << relGain 
	      //		     << " to a hit of cell " << m_emHelper->show_to_string(cellId) << endmsg; 
	      inTimeE+=hitEnergy;
	      nInTime++;
	    } // only hits in timing window
	    else {
	      outOfTimeE+=hitEnergy;
	      nOutOfTime++;
	      if(hitEnergy >  printEthresh) {
		if(!m_PileUp) {
		  ATH_MSG_DEBUG
                    ( "Found a hit out of the timing window, hitTime= " << hitTime
			 << " with more than " << printEthresh << " MeV: hitEnergy= " << hitEnergy << " MeV" );
		}
		else {
                  ATH_MSG_VERBOSE
                    ( "Found a hit out of the timing window, hitTime= " << hitTime
                      << " with more than " << printEthresh << " MeV: hitEnergy= " << hitEnergy << " MeV" );
		}
	      }
	    }
	    ++first;
	  } // end of loop on hits in the list    
	} // skip cell condition
      } // check timeE->size() > 0
    } // check hitlist > 0
  } // end of loop on hit lists
  
  ATH_MSG_DEBUG
    ( "Number of missing relative FCAL gains for this event = "
      <<  nMissingGain );
  if (inTimeE == 0 || nInTime == 0)
    ATH_MSG_VERBOSE ( "No in time energy" );
  else
    ATH_MSG_VERBOSE
      ( "Out of time energy = " << outOfTimeE << " MeV"
        << " represents " <<  100.* outOfTimeE/inTimeE
        << " % of in time energy for " << nOutOfTime << " (" << 100.*nOutOfTime/nInTime << " %) hits" );
  if(outOfTimeE > 0.02*inTimeE) {
    ATH_MSG_WARNING
      ( "Out of time energy = " << outOfTimeE << " MeV"
	   << " larger than 2% of in time energy = " << inTimeE << " MeV; nb of out of time hits = " 
	   << nInTime << " (" 
           <<  (nInTime > 0 ? 100.*nOutOfTime/nInTime : 0)
           << " %)" );
  }

  if(m_chronoTest) {
    m_chronSvc->chronoStop( "LArTTL1Mk hit loop " );
    m_chronSvc->chronoPrint( "LArTTL1Mk hit loop " );
    m_chronSvc->chronoStart( "LArTTL1Mk TT loop " );
  }

  std::vector<std::string> emHadString(2);
  emHadString[0]="ElectroMagnetic";
  emHadString[1]="Hadronic";
	      
  std::vector<Identifier>::const_iterator itTt  = m_lvl1Helper->tower_begin() ;
  std::vector<Identifier>::const_iterator itEnd = m_lvl1Helper->tower_end() ;

  //
  // ....... loop on Trigger Towers
  // ....... and build signals using pulse shape and noise for each TT
  //
  ATH_MSG_DEBUG ( " Starting loop on Trigger Towers " );
  for(; itTt!=itEnd;++itTt){
	    
    Identifier towerId = (*itTt) ;
    
    //
    // ........ skip Tile cal
    //
    if(!m_lvl1Helper->is_tile(towerId)){  

      int emHad = m_lvl1Helper->sampling(towerId); 
      // convert offline id to hash id
      IdentifierHash ttHash = m_lvl1Helper->tower_hash(towerId) ;
      int region = m_lvl1Helper->region(towerId);
      int eta = m_lvl1Helper->eta(towerId);
      int Ieta = decodeInverse(region,eta);
      
      //
      // .... compute the signal for current trigger tower
      //
      if(m_chronoTest) {
	m_chronSvc->chronoStart( "compute signal " );
      }
      std::vector<float> analogSum (s_NBSAMPLES) ;
      std::vector<float> analogSum2(s_NBSAMPLES) ;
      std::vector<float> sumTTE	 = sumEnergy[ttHash];
      std::vector<float> sumTTE2 = sumEnergy2[ttHash];
      int nSpecialCase=0;
	

      bool hasE = false; 
      for ( unsigned int i=0; i<sumTTE.size();++i){
	if (fabs(sumTTE[i]) > 0.){
	  hasE=true;
	  break;
	}
      }

      bool hasE2 = false; 
      for ( unsigned int i=0; i<sumTTE2.size();++i){
	if (fabs(sumTTE2[i]) > 0.){
	  hasE2=true;
	  break;
	}
      }


      //if ( fabs(sumTTE[refTime]) > 0.|| fabs(sumTTE2[refTime]) > 0. ) {
      if ( hasE || hasE2 ) {
	
	//if(fabs(sumTTE[refTime]) > 0.) {
	if(  hasE ) {
	  analogSum = computeSignal(towerId,Ieta,0,sumTTE,refTime) ;
	}
	
	// treat special case of ec part of the "barrel end" and special case of FCAL2/FCAL3
	// fix me !! this way, we double the saturation energy in this tower...(because it is cut into 2 pieces)
	//if(fabs(sumTTE2[refTime]) > 0.) {
	if(  hasE2) {
	  nSpecialCase+=1;
	  if(nSpecialCase>1) {
	    ATH_MSG_WARNING
              ( " more than 1 special case, current Trigger Tower is " << emHadString[emHad] << ": "
		   << m_emHelper->show_to_string(towerId) << " " );
	  }
	  analogSum2 = computeSignal(towerId,Ieta,1,sumTTE2,refTime) ;
	  for( int isamp=0 ; isamp < s_NBSAMPLES ; isamp++) {
	    analogSum[isamp] += analogSum2[isamp] ;
	  }
	}

	if ( sumTTE[refTime] > m_debugThresh) {
          ATH_MSG_DEBUG
            ( " current Trigger Tower is " << emHadString[emHad] << ": "
              << m_emHelper->show_to_string(towerId) << " " );
          ATH_MSG_DEBUG
            ( " transverse E (i.e. sum E / sampling fraction * sin_theta * rel gain)= (at ref. time, before calib)" 
              << sumTTE[refTime] << " + " << sumTTE2[refTime] << " (special cases) " );
	}
	else if ( sumTTE[refTime] > 0.) {
          ATH_MSG_VERBOSE
            ( " current Trigger Tower is "  << emHadString[emHad] << ": "
              << m_emHelper->show_to_string(towerId) << " " );
          ATH_MSG_VERBOSE
            ( " [very low] transverse E (i.e. sum E / sampling fraction * sin_theta * rel gain)= (at ref. time, before calib)" 
              << sumTTE[refTime] << " + " << sumTTE2[refTime] << " (special cases) " );
	}
	
      }
      if(m_chronoTest) {
	m_chronSvc->chronoStop ( "compute signal " );
	m_chronSvc->chronoPrint( "compute signal " );
      }
      
      //
      // ........ add the noise
      //
      if(m_chronoTest) {
	m_chronSvc->chronoStart( "adding noise " );
      }
      std::vector<float> fullSignal(s_NBSAMPLES) ;
      if(m_NoiseOnOff) {
	fullSignal = computeNoise(towerId,Ieta,analogSum);
      } else {
	fullSignal  = analogSum;
      }

      if(m_chronoTest) {
	m_chronSvc->chronoStop ( "adding noise " );
	m_chronSvc->chronoPrint ( "adding noise " );
      }
      
      if(sumTTE[refTime] > m_debugThresh) {
        ATH_MSG_DEBUG
          ( " uncalibrated amplitudes around peak (+-3 time slots): " 
            << sumTTE[refTime-3]  << ", "
            << sumTTE[refTime-2]  << ", "
            << sumTTE[refTime-1]  << ", "
            << sumTTE[refTime]  << ", "
            << sumTTE[refTime+1]  << ", "
            << sumTTE[refTime+2]  << ", "
            << sumTTE[refTime+3] );
        ATH_MSG_DEBUG
          ( " calibrated signal is " 
            << analogSum[0] << ", "
            << analogSum[1] << ", "
            << analogSum[2] << ", "
            << analogSum[3] << ", "
            << analogSum[4] << ", "
            << analogSum[5] << ", "
            << analogSum[6]  );
        ATH_MSG_DEBUG
          ( " shape of calibrated signal is " 
            << analogSum[0]/analogSum[3] << ", "
            << analogSum[1]/analogSum[3] << ", "
            << analogSum[2]/analogSum[3] << ", "
            << analogSum[3]/analogSum[3] << ", "
            << analogSum[4]/analogSum[3] << ", "
            << analogSum[5]/analogSum[3] << ", "
            << analogSum[6]/analogSum[3] );
        ATH_MSG_DEBUG
          ( " after adding noise, full signal is " 
            << fullSignal[0] << ", "
            << fullSignal[1] << ", "
            << fullSignal[2] << ", "
            << fullSignal[3] << ", "
            << fullSignal[4] << ", "
            << fullSignal[5] << ", "
            << fullSignal[6] );
	if (msgLvl(MSG::VERBOSE)) {
	  for (unsigned int iTime=0; iTime<sumTTE.size(); iTime++) {
	    ATH_MSG_VERBOSE
              ( " iTime [range=0-28 or 0-299] = " << iTime
		   << " hit energy = " << sumTTE[iTime] );
	  }
	}
      } else if(sumTTE[refTime] > 0.) {
        ATH_MSG_VERBOSE
          ( " uncalibrated amplitudes around peak (+-3 time slots): " 
            << sumTTE[refTime-3]  << ", "
            << sumTTE[refTime-2]  << ", "
            << sumTTE[refTime-1]  << ", "
            << sumTTE[refTime]  << ", "
            << sumTTE[refTime+1]  << ", "
            << sumTTE[refTime+2]  << ", "
            << sumTTE[refTime+3] );
        ATH_MSG_VERBOSE
          ( " calibrated signal is " 
            << analogSum[0] << ", "
            << analogSum[1] << ", "
            << analogSum[2] << ", "
            << analogSum[3] << ", "
            << analogSum[4] << ", "
            << analogSum[5] << ", "
            << analogSum[6]  );
        ATH_MSG_VERBOSE
          ( " shape of calibrated signal is " 
            << analogSum[0]/analogSum[3] << ", "
            << analogSum[1]/analogSum[3] << ", "
            << analogSum[2]/analogSum[3] << ", "
            << analogSum[3]/analogSum[3] << ", "
            << analogSum[4]/analogSum[3] << ", "
            << analogSum[5]/analogSum[3] << ", "
            << analogSum[6]/analogSum[3] );
        ATH_MSG_VERBOSE
          ( " after adding noise, full signal is " 
            << fullSignal[0] << ", "
            << fullSignal[1] << ", "
            << fullSignal[2] << ", "
            << fullSignal[3] << ", "
            << fullSignal[4] << ", "
            << fullSignal[5] << ", "
            << fullSignal[6] );
      }
      
      if(fabs(fullSignal[s_PEAKPOS]) > 0.) {
	//
	// ...... create the LArTTL1 and push it into the appropriate TTL1 container
	//
	LArTTL1 * ttL1 ;
	HWIdentifier ttChannel;
	ttL1 = new LArTTL1(ttChannel,towerId,fullSignal);
	if (emHad) { ttL1ContainerHad->push_back(ttL1); }
	else       { ttL1ContainerEm->push_back(ttL1); }

         if(m_truthHitsContainer.size()>0) {
            std::vector<float> et(3);et[0] = sumTTE[refTime-1]; et[1] = sumTTE[refTime]; et[2] = sumTTE[refTime+1];
            LArTTL1* truth_ttL1 = new LArTTL1(ttChannel,towerId,et);
            if (emHad) { truth_ttL1ContainerHad->push_back(truth_ttL1); }
	    else       { truth_ttL1ContainerEm->push_back(truth_ttL1); }
         }

      }
      
    }  // end excluding Tile cal
  } // end of for loop on TT
  if(m_chronoTest) {
    m_chronSvc->chronoStop ( "LArTTL1Mk TT loop " );
    m_chronSvc->chronoPrint ( "LArTTL1Mk TT loop " );
  }
  
  ATH_MSG_DEBUG ( "number of created TTL1s (Em, Had) = " 
                  << ttL1ContainerEm->size() 
                  << " , "
                  << ttL1ContainerHad->size() );

  return StatusCode::SUCCESS;
}


StatusCode LArTTL1Maker::finalize()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//

  ATH_MSG_INFO ( " LArTTL1Maker finalize completed successfully" );

  m_chronSvc->chronoPrint( "LArTTL1Mk hit loop " );
  m_chronSvc->chronoPrint( "LArTTL1Mk TT loop " );
  
  return StatusCode::SUCCESS;

}


std::vector<float> LArTTL1Maker::computeSignal(const Identifier towerId, const int Ieta, const int specialCase,
					       std::vector<float> ttSumEnergy, const int refTime) const

{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                  +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
//

  std::vector<float> bareSignal(s_NBSAMPLES) ;
  std::vector<float> pulseShape(s_MAXSAMPLES) ;
  std::vector<float> pulseShapeDer(s_MAXSAMPLES) ;

  bool emb = m_lvl1Helper->is_emb(towerId);
  bool barrelEnd = m_lvl1Helper->is_barrel_end(towerId);
  bool emec= m_lvl1Helper->is_emec(towerId);

  unsigned int visEvecSize=ttSumEnergy.size();
  ATH_MSG_VERBOSE ( "computeSignal: special case = " << specialCase  );

  //
  // ..... case EM
  //
  if (emb || barrelEnd || emec) {
    //
    // ..... retrieve calib coeffs for the tower
    //
    float calibCoeff=0.;
    if (emb) {
      calibCoeff = m_calibCoeffEmb[Ieta-1];
    }
    else if (emec) {
      calibCoeff = m_calibCoeffEmec[Ieta-1];
    }
    else {  // barrelEnd
      if(!specialCase) {
	calibCoeff = m_calibCoeffEmb[14];
      }
      else {
	calibCoeff = m_calibCoeffEmec[0];
      }
    }
    if(calibCoeff < 0.001) {
      ATH_MSG_WARNING ( " No calibration coefficient value found for tower " 
                        << m_emHelper->show_to_string(towerId) 
                        << " setting default value 6. "  );
      calibCoeff = 6.;
    }

    //
    // ... loop on time samples 
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      if(fabs(ttSumEnergy[iTime]) > 0.) {
	if(!m_noEmCalibMode) {
	  // apply calibration coefficient
	  ttSumEnergy[iTime] *= calibCoeff;
          ATH_MSG_VERBOSE ( " ComputeSignal: applied EM calibration coefficient (iTime) " 
                            << calibCoeff << " (" << iTime << ") " );
	}
	
	// with respect to saturation
	// to compute the amplitude
	float theEnergy = ttSumEnergy[iTime] ;
	if( ttSumEnergy[iTime] > m_refEnergyEm [0]) {
	  theEnergy = m_refEnergyEm [0] ;    
	}
	// to determine the shape
	if( ttSumEnergy[iTime] > m_refEnergyEm [s_NBENERGIES-1]) {
	  // don't know the shape after highest energy point -> stick to 'last' shape
	  ttSumEnergy[iTime] = m_refEnergyEm [s_NBENERGIES-1] ;
	}
	
	//
	// ... determine regime: linear(iene=0) or saturation
	//
	for (int iene = 0 ; iene < s_NBENERGIES ; iene++) {
	  if ( ttSumEnergy[iTime] <= m_refEnergyEm[iene] ) {
	    pulseShape = m_pulseShapeEm[iene] ;  
	    pulseShapeDer = m_pulseShapeDerEm[iene] ;  

	    // make samples
	    for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	      if(!m_PileUp) {
		// go back to signed value of time
		int hitTime = iTime - refTime;
		// go to 25 ns sampling
		//int time = (int)(hitTime/25.+0.5) ;
		int time = static_cast<int>(floor(hitTime*crossingRate+0.5));
		// ....determine fine shift within 25ns 
		float dTime = (float)(hitTime - crossingTime*time);
		int j= iSamp - time ;
		if(j >=0 && j < s_MAXSAMPLES) {
		  bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
		}
	      }
	      else {
		int j= iSamp - iTime + refTime ;
		if(j >=0 && j < s_MAXSAMPLES) {
		  bareSignal[iSamp] += pulseShape[j] * theEnergy ;
		}
	      }
	    } // end loop on samples
	  
	    break ;
	  }
	} // end of loop on reference energies
      } // end condition visE>0
    } // end loop on times
  } // end case EM
  //
  // ..... case HEC
  //
  else if (m_lvl1Helper->is_hec(towerId)) {
    pulseShape = m_pulseShapeHec ;
    pulseShapeDer = m_pulseShapeDerHec ;
    //
    // ... loop on time samples 
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      float theEnergy = ttSumEnergy[iTime];
      if(!m_noHadCalibMode) {
	// apply calibration coefficient
	theEnergy *= m_calibCoeffHec[Ieta-1];
      }

      float satEt = m_satEnergyHec[Ieta-1];  
      for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	if(!m_PileUp) {
	  // go back to signed value of time
	  int hitTime = iTime - refTime;
	  // go to 25 ns sampling
	  //int time = (int)(hitTime/25.+0.5) ;
	  int time = static_cast<int>(floor(hitTime*crossingRate+0.5));
	  // ....determine fine shift within 25ns 
	  float dTime = (float)(hitTime - crossingTime*time);
	  int j= iSamp - time ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
	  }
	}
	else {
	  int j= iSamp - iTime + refTime ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += pulseShape[j] * theEnergy ;
	  }
	}
	// saturation
	if ( bareSignal[iSamp] > satEt) {
	  bareSignal[iSamp] = satEt;
	}
      }
    }
  }
  //
  // ..... case FCAL
  //
  else if (m_lvl1Helper->is_fcal(towerId)) {

    int emOrHad = m_lvl1Helper->sampling(towerId) ;
    int module = emOrHad + 1 ;
    std::vector<float> calibCoeff2= m_calibCoeffFcal[module-1];
    if( emOrHad && (Ieta==3 || Ieta==4) ) {
      module+=1; // FCAL3
    }
    pulseShape = m_pulseShapeFcal[module-1] ;  
    pulseShapeDer = m_pulseShapeDerFcal[module-1] ;  
    
    //
    // ... loop on time samples (only one if no pile-up)
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      float theEnergy = ttSumEnergy[iTime];
      if((!m_noEmCalibMode&&module==1)||(!m_noHadCalibMode&&module>1)) {
	// apply calibration coefficient
	theEnergy *= calibCoeff2[Ieta-1];
      }
      
      // make samples
      for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	if(!m_PileUp) {
	  // go back to signed value of time
	  int hitTime = iTime - refTime;
	  // go to 25 ns sampling
	  //int time = (int)(hitTime/25.+0.5) ;
	  int time = static_cast<int>(floor(hitTime*crossingRate+0.5));
	  // ....determine fine shift within 25ns 
	  float dTime = (float)(hitTime - crossingTime*time);
	  int j= iSamp - time ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
	  }
	}
	else {
	  int j= iSamp - iTime + refTime ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += pulseShape[j] * theEnergy ;
	  }
	}
      }
    }
  }
  else {  
    ATH_MSG_WARNING ( " LArTTL1Maker: computeSignal unknown towerId " 
                      << m_lvl1Helper->show_to_string(towerId) );
    return  bareSignal ;
  }

  return  bareSignal ;

}


std::vector<float> LArTTL1Maker::computeNoise(const Identifier towerId, const int Ieta,  
					      std::vector<float>& inputV) 
{

// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+

  std::vector<float> outputV (s_NBSAMPLES) ;

  //
  // .... first retrieve auto-correlation matrix and noise rms
  //

  float sigmaNoise  = 0; 
  std::vector<float>* autoC = 0; 
  std::vector<float> noiseRms(4) ;

  bool emb = m_lvl1Helper->is_emb(towerId);
  bool barrelEnd = m_lvl1Helper->is_barrel_end(towerId);
  bool emec= m_lvl1Helper->is_emec(towerId);
  //
  // ..... case EM
  //
  if (emb || barrelEnd || emec) {
    //
    // ..... retrieve noise info for the tower
    //
    autoC = &(m_autoCorrEm);
    if (emb) {
      sigmaNoise = m_noiseRmsEmb[Ieta-1];
    }
    else if (emec) {
      sigmaNoise = m_noiseRmsEmec[Ieta-1];
    }
    else {  // barrelEnd
      sigmaNoise = sqrt(m_noiseRmsEmb[14]*m_noiseRmsEmb[14] + m_noiseRmsEmec[0]*m_noiseRmsEmec[0]);
      ATH_MSG_VERBOSE ( " LArTTL1Maker: barrelEnd noise rms" << sigmaNoise );
    }
  } // end case EM
  //
  // ..... case HEC
  //
  else if (m_lvl1Helper->is_hec(towerId)) {

    autoC = &(m_autoCorrHec[Ieta-1]);
    sigmaNoise = m_noiseRmsHec[Ieta-1];
  }
  //
  // ..... case FCAL
  //
  else if (m_lvl1Helper->is_fcal(towerId)) {

    int emOrHad = m_lvl1Helper->sampling(towerId) ;
    int module = emOrHad + 1 ;
    if( emOrHad && (Ieta==3 || Ieta==4) ) {
      module+=1; // FCAL3
    }
    autoC = &m_autoCorrFcal;
    //    sigmaNoise = m_noiseRmsFcal[module-1];
    noiseRms = m_noiseRmsFcal[module-1];
    sigmaNoise = noiseRms[Ieta-1];
    ATH_MSG_VERBOSE ( " LArTTL1Maker: noise FCAL= " << sigmaNoise
                      << " module= " << module << "Ieta= " << Ieta );
  }
  else {  
    ATH_MSG_WARNING ( " LArTTL1Maker: computeNoise unknown towerId " 
                      << m_lvl1Helper->show_to_string(towerId) );
    return  inputV ;
  }

  if(fabs((*autoC)[0]) < 0.001) {
    ATH_MSG_WARNING ( " No autocorrelation matrix found for tower " 
                      << m_emHelper->show_to_string(towerId) << " " 
                      << "setting default values 1.00   0.10  -0.30  -0.20  -0.05  -0.01  -0.01 ");
    (*autoC)[0] = 1.00;
    (*autoC)[1] = 0.10;
    (*autoC)[2] =-0.30;
    (*autoC)[3] =-0.20;
    (*autoC)[4] =-0.05;
    (*autoC)[5] =-0.01;
    (*autoC)[6] =-0.01;
  }
  if(sigmaNoise < 0.001) {
    ATH_MSG_WARNING ( " No noise rms value found for tower " 
                      << m_emHelper->show_to_string(towerId) << " " 
                      << "setting default value 300 MeV " );
    sigmaNoise = 300.;
  }

  //
  // .... now compute the noise 'signal'
  //

  const float c11 =  sigmaNoise*(*autoC)[0];
  const float c21 =  sigmaNoise*(*autoC)[1];
  const float c31 =  sigmaNoise*(*autoC)[2];
  const float c41 =  sigmaNoise*(*autoC)[3];
  const float c51 =  sigmaNoise*(*autoC)[4];
  const float c61 =  sigmaNoise*(*autoC)[5];
  const float c71 =  sigmaNoise*(*autoC)[6];
//
  const float c22 = sqrt(c11*c11-c21*c21);
  const float inv_c22 = 1. / c22;
  const float c32 = (c21*c11-c21*c31)*inv_c22;
  const float c33 = sqrt(c11*c11-c31*c31-c32*c32);
  const float inv_c33 = 1. / c33;
  const float c42 = (c31*c11-c21*c41)*inv_c22;
  const float c43 = (c21*c11-c31*c41-c32*c42) * inv_c33;
  const float c44 = sqrt(c11*c11-c41*c41-c42*c42-c43*c43);
  const float inv_c44 = 1. / c44;
  const float c52 = (c41*c11-c21*c51) * inv_c22;
  const float c53 = (c31*c11-c31*c51-c32*c52) * inv_c33;
  const float c54 = (c21*c11-c41*c51-c42*c52-c43*c53) * inv_c44;
  const float c55 = sqrt(c11*c11-c51*c51-c52*c52-c53*c53-c54*c54);
  const float inv_c55 = 1. / c55;
  const float c62 = (c51*c11-c21*c61) * inv_c22;
  const float c63 = (c41*c11-c31*c61-c32*c62) * inv_c33;
  const float c64 = (c31*c11-c41*c61-c42*c62-c43*c63) * inv_c44;
  const float c65 = (c21*c11-c51*c61-c52*c62-c53*c63-c54*c64) * inv_c55;
  const float c66 = sqrt(c11*c11-c61*c61-c62*c62-c63*c63-c64*c64-c65*c65);
  const float c72 = (c61*c11-c21*c71) * inv_c22;
  const float c73 = (c51*c11-c31*c71-c32*c72) * inv_c33;
  const float c74 = (c41*c11-c41*c71-c42*c72-c43*c73) * inv_c44;
  const float c75 = (c31*c11-c51*c71-c52*c72-c53*c73-c54*c74) * inv_c55;
  const float c76 = (c21*c11-c61*c71-c62*c72-c63*c73-c64*c74-c65*c75)/c66;
  const float c77 = sqrt(c11*c11-c71*c71-c72*c72-c73*c73-c74*c74-c75*c75-c76*c76);
  
  double rndm[s_NBSAMPLES];
  RandGaussZiggurat::shootArray(m_rndmEngine,(int)s_NBSAMPLES,rndm,0.,1.);
  outputV[0] = inputV[0] + c11*rndm[0];
  outputV[1] = inputV[1] + c21*rndm[0] + c22*rndm[1];
  outputV[2] = inputV[2] + c31*rndm[0] + c32*rndm[1] + c33*rndm[2];
  outputV[3] = inputV[3] + c41*rndm[0] + c42*rndm[1] + c43*rndm[2] + c44*rndm[3];
  outputV[4] = inputV[4] + c51*rndm[0] + c52*rndm[1] + c53*rndm[2] + c54*rndm[3] + c55*rndm[4];
  outputV[5] = inputV[5] + c61*rndm[0] + c62*rndm[1] + c63*rndm[2] + c64*rndm[3] + c65*rndm[4] + c66*rndm[5];
  outputV[6] = inputV[6] + c71*rndm[0] + c72*rndm[1] + c73*rndm[2] + c74*rndm[3] + c75*rndm[4] + c76*rndm[5] + c77*rndm[6];

  return outputV ;

}


StatusCode LArTTL1Maker::fillEMap(int& totHit) 

{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/14                                            +
// +                                                                      +
// +======================================================================+

//
// ........ reset the map Energies
//
  if (m_useMapfromStore) {
    ATH_CHECK( detStore()->retrieve(m_hitmap,"LArHitEMap") );
  }

  else {

    m_hitmap->EnergyReset();
    ATH_MSG_DEBUG ( "Execute: energy reset done" );


    Identifier cellId;
    float hitEnergy;
    float hitTime;
    int skipHit=0;

    //
    // ............ loop over the wanted hit containers (one per sub-detector)
    //

    // std::vector<const LArHitContainer*> hitContainers;
    // SG::ReadHandle<LArHitContainer>  emBarrelHitsHdl(m_EmBarrelHitContainerName);
    // hitContainers.push_back(emBarrelHitsHdl);
    // SG::ReadHandle<LArHitContainer>  emEndCapHitsHdl(m_EmEndCapHitContainerName);
    // hitContainers.push_back(emEndCapHitsHdl);
    // SG::ReadHandle<LArHitContainer>  hecHitsHdl( m_HecHitContainerName);
    // hitContainers.push_back(hecCapHitsHdl);
    // SG::ReadHandle<LArHitContainer>  fcalHitsHdl( m_ForWardHitContainerName);
    // hitContainers.push_back(fcalHitsHdl);

    for (auto& dhk : m_xxxHitContainerName) 
    {      
      if (!m_PileUp) {
	//
	// ....... loop over hits and get informations
	//
	
	SG::ReadHandle<LArHitContainer> hit_container(dhk);
	LArHitContainer::const_iterator hititer;
	for(hititer=hit_container->begin();hititer != hit_container->end();hititer++) {
	  cellId = (*hititer)->cellID();
	  hitEnergy = (float)(*hititer)->energy();
	  hitTime = (float)(*hititer)->time();
	    
	  //
	  // ....... fill the Map ; don't keep hits with ridiculously small energy
	  //
	  if (fabs(hitEnergy) > 1.e-06) {
	    if ( !m_hitmap->AddEnergy(cellId,hitEnergy,hitTime) ) {
	      ATH_MSG_FATAL ( " Cell " << cellId.getString()
                              << " could not add the energy= " << hitEnergy  << " (MeV)" );
	      return(StatusCode::FAILURE);
	    }
	    ++totHit;
	  }
	  else {
	    ++skipHit;
	  }
	  
	} // .... end of loop over hits
      } // ... end of NO pile-up condition
      else {
	
	typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
	TimedHitContList hitContList;
	//
	// ...retrieve list of pairs (time,container) from PileUp service
	//
	
	if (!(m_mergeSvc->retrieveSubEvtsData(dhk.key(),hitContList).isSuccess()) && hitContList.size()==0) {
	  ATH_MSG_ERROR ( "Could not fill TimedHitContList" );
	  return StatusCode::FAILURE;
	}
	
        ATH_MSG_DEBUG ( "number of containers in the list: " << hitContList.size() );
	
	//
	// ...loop over this list
	//
	TimedHitContList::iterator iFirstCont(hitContList.begin());
	TimedHitContList::iterator iEndCont(hitContList.end());
	double SubEvtTimOffset;

	while (iFirstCont != iEndCont) {
	  // get time for this subevent
	  // new coding of time information (January 05)
	  const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
	  SubEvtTimOffset = time_evt->time();
	  //	  SubEvtTimOffset = iFirstCont->first;

	  // get LArHitContainer for this subevent
	  const LArHitContainer& firstCont = *(iFirstCont->second);
          ATH_MSG_DEBUG 
            ( "number of hits in container: " << firstCont.size() 
              << ", first five are:" );
	
	  int numHit=0;
	  // Loop over cells in this LArHitContainer
	  LArHitContainer::const_iterator f_cell=firstCont.begin();
	  LArHitContainer::const_iterator l_cell=firstCont.end();
	  while (f_cell != l_cell) {
	    hitEnergy = (float) (*f_cell)->energy();
	    hitTime = (float) SubEvtTimOffset;
	    cellId = (*f_cell)->cellID();
            ++f_cell;

	    if(numHit<5) {
	      ATH_MSG_DEBUG
                ( "cellId " << m_lvl1Helper->show_to_string(cellId) 
                  << ", energy= " << hitEnergy
                  << ", time= " << hitTime );
	    }
	    ++numHit;
	    ++totHit;
	    
	    //
	    // ....... fill the Map
	    //
	    if ( !m_hitmap->AddEnergy(cellId,hitEnergy,hitTime) )
	      {
		ATH_MSG_FATAL ( " Cell " << cellId.getString()
                                << " could not add the energy= " << hitEnergy  << " (?eV)" );
		return(StatusCode::FAILURE);
	      }
	    
	  } // ... end of while loop on hits
	  
	  ++iFirstCont;
	} // ... end of while loop on containers
      
      } // ... end of pile-up condition
    } // .... end of loop over containers
  
    ATH_MSG_DEBUG
      ( "skipped " << skipHit  << " hits with abs(energy) less than 1.e-06 " );
  }

  return StatusCode::SUCCESS;
}


StatusCode LArTTL1Maker::readAuxiliary()
{
  //
  // ...... Read auxiliary data file for EM (barrel and EC)
  //

  ATH_MSG_DEBUG ( "executing readAuxiliary()" );

  float refEnergy;
  std::vector<float> pulseShape(s_MAXSAMPLES) ;
  std::vector<float> pulseShapeDer(s_MAXSAMPLES) ;
  std::string barrel_endcap ;
  int Ieta ;
  float sinTheta=0. ;
  std::vector<float> layerRelGain(s_NBDEPTHS) ;
  std::vector<float> noiseRms(3) ;
  std::vector<float> noiseRms4(4) ;
  std::vector<float> autoCorr(s_NBSAMPLES) ;


  std::string pulsedataname=PathResolver::find_file ("LArEmLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR ( "Could not locate LArEmLvl1.data file" );
    return StatusCode::FAILURE;
  }
  const char * pulsedatafile= pulsedataname.c_str() ;
  std::ifstream infile (pulsedatafile) ;

  if(!infile) {
    ATH_MSG_ERROR ( " cannot open EM file " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( " EM file opened " );
  }

  // first read the pulse shape for all energies 
  // valid for both barrel and endcap (from Xavier de la Broise)
  for(int iene=0 ; iene<s_NBENERGIES ; iene++) {
    infile >> refEnergy 
	   >> pulseShape[0] 
	   >> pulseShape[1] 
	   >> pulseShape[2] 
	   >> pulseShape[3] 
	   >> pulseShape[4] 
	   >> pulseShape[5] 
	   >> pulseShape[6] 
	   >> pulseShape[7] 
	   >> pulseShape[8] 
	   >> pulseShape[9] 
	   >> pulseShape[10] 
	   >> pulseShape[11] 
	   >> pulseShape[12] 
	   >> pulseShape[13] 
	   >> pulseShape[14] 
	   >> pulseShape[15] 
	   >> pulseShape[16] 
	   >> pulseShape[17] 
	   >> pulseShape[18] 
	   >> pulseShape[19] 
	   >> pulseShape[20] 
	   >> pulseShape[21] 
	   >> pulseShape[22]
	   >> pulseShape[23];
    m_refEnergyEm.push_back(refEnergy);
    m_pulseShapeEm.push_back(pulseShape);
    infile >> pulseShapeDer[0] 
	   >> pulseShapeDer[1] 
	   >> pulseShapeDer[2] 
	   >> pulseShapeDer[3] 
	   >> pulseShapeDer[4] 
	   >> pulseShapeDer[5] 
	   >> pulseShapeDer[6] 
	   >> pulseShapeDer[7] 
	   >> pulseShapeDer[8] 
	   >> pulseShapeDer[9] 
	   >> pulseShapeDer[10] 
	   >> pulseShapeDer[11] 
	   >> pulseShapeDer[12] 
	   >> pulseShapeDer[13] 
	   >> pulseShapeDer[14] 
	   >> pulseShapeDer[15] 
	   >> pulseShapeDer[16] 
	   >> pulseShapeDer[17] 
	   >> pulseShapeDer[18] 
	   >> pulseShapeDer[19] 
	   >> pulseShapeDer[20] 
	   >> pulseShapeDer[21] 
	   >> pulseShapeDer[22]
	   >> pulseShapeDer[23];
    m_pulseShapeDerEm.push_back(pulseShapeDer);
  }

  // then read autocorrelation function valid for barrel and endcap
  // ("default" auto-correlation function)
  m_autoCorrEm.resize(s_NBSAMPLES);
  infile >> m_autoCorrEm[0]
         >> m_autoCorrEm[1]
         >> m_autoCorrEm[2]
         >> m_autoCorrEm[3]
         >> m_autoCorrEm[4]
         >> m_autoCorrEm[5]
         >> m_autoCorrEm[6] ;

  // then read the separator for barrel
  infile >> barrel_endcap;

  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    // then read the calibration factor (from MC) 
    // and the noise rms (from Bill Cleland) for each eta bin, in barrel
    infile >> Ieta
	   >> sinTheta;
    m_sinThetaEmb.push_back(sinTheta);
    infile >> noiseRms[0] 
	   >> noiseRms[1] 
	   >> noiseRms[2] ;
    m_noiseRmsEmb.push_back(noiseRms[2]);
    // if later we want to disentangle between pre-sum and summing electronic noise...
    //    m_noiseRmsEmb.push_back(noiseRms);
  }

  // now read the separator for endcap 
  infile >> barrel_endcap;

  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    // then read the calibration coefficient (from MC) 
    // and the noise rms (from Bill Cleland) for each eta bin, in barrel
    infile >> Ieta
	   >> sinTheta;
    m_sinThetaEmec.push_back(sinTheta);
    infile >> noiseRms[0] 
	   >> noiseRms[1] 
	   >> noiseRms[2] ;
    m_noiseRmsEmec.push_back(noiseRms[2]);
    // if later we want to disentangle between pre-sum and summing electronic noise...
    //    m_noiseRmsEmec.push_back(noiseRms);

  }
  
  infile.close() ;
  ATH_MSG_DEBUG ( " EM file closed " );

  ATH_MSG_INFO ( " 1 pulse shape per energy for EMB+EMEC : " );
  for(int iene=0;iene<s_NBENERGIES;iene++) {
    ATH_MSG_INFO
      ( m_refEnergyEm[iene] << " MeV: "
	   << (m_pulseShapeEm[iene])[0] << ", "
	   << (m_pulseShapeEm[iene])[1] << ", "
	   << (m_pulseShapeEm[iene])[2] << ", "
	   << (m_pulseShapeEm[iene])[3] << ", "
	   << (m_pulseShapeEm[iene])[4] << ", "
	   << (m_pulseShapeEm[iene])[5] << ", "
	   << (m_pulseShapeEm[iene])[6] << ", "
	   << (m_pulseShapeEm[iene])[7] << ", "
	   << (m_pulseShapeEm[iene])[8] << ", "
	   << (m_pulseShapeEm[iene])[9] << ", "
	   << (m_pulseShapeEm[iene])[10] << ", "
	   << (m_pulseShapeEm[iene])[11] << ", "
	   << (m_pulseShapeEm[iene])[12] << ", "
	   << (m_pulseShapeEm[iene])[13] << ", "
	   << (m_pulseShapeEm[iene])[14] << ", "
	   << (m_pulseShapeEm[iene])[15] << ", "
	   << (m_pulseShapeEm[iene])[16] << ", "
	   << (m_pulseShapeEm[iene])[17] << ", "
	   << (m_pulseShapeEm[iene])[18] << ", "
	   << (m_pulseShapeEm[iene])[19] << ", "
	   << (m_pulseShapeEm[iene])[20] << ", "
	   << (m_pulseShapeEm[iene])[21] << ", "
	   << (m_pulseShapeEm[iene])[22] << ", "
	   << (m_pulseShapeEm[iene])[23] << ", "
        );
  }
  ATH_MSG_INFO
    ( " 1 auto-corr matrix for EMB+EMEC : "
	 << m_autoCorrEm[0] << ", "
	 << m_autoCorrEm[1] << ", "
	 << m_autoCorrEm[2] << ", "
	 << m_autoCorrEm[3] << ", "
	 << m_autoCorrEm[4] << ", "
	 << m_autoCorrEm[5] << ", "
	 << m_autoCorrEm[6] 
      );

  ATH_MSG_DEBUG
    ( " Finished reading 1 calib coeff + 1 noise value per eta bin for EM: " );
  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
// currently the calib coeffs are all set to 1 -> turning INFO logs into DEBUG
    ATH_MSG_DEBUG
      ( "Ieta= " << ieta+1
	   << ", calib coeff EMB:  " << m_calibCoeffEmb[ieta]
	   << ", calib coeff EMEC: " << m_calibCoeffEmec[ieta] );
    ATH_MSG_INFO
      ( "Ieta= " << ieta+1
	   << ", noise rms   EMB:  " << m_noiseRmsEmb[ieta] << " MeV"
	   << ", noise rms   EMEC: " << m_noiseRmsEmec[ieta] << " MeV" );
  }

  //
  // ...... Read auxiliary data file for HEC
  //

  pulsedataname=PathResolver::find_file ("LArHecLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR ( "Could not locate LArHecLvl1.data file" );
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    ATH_MSG_ERROR ( " cannot open HEC file " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( " HEC file opened " );
  }

  // first read the pulse shape for each eta bin (from Leonid Kurchaninov)
  m_pulseShapeHec.resize(s_MAXSAMPLES);
  infile >> m_pulseShapeHec [0]
         >> m_pulseShapeHec [1]
	 >> m_pulseShapeHec [2]
	 >> m_pulseShapeHec [3]
	 >> m_pulseShapeHec [4]
	 >> m_pulseShapeHec [5]
	 >> m_pulseShapeHec [6]
	 >> m_pulseShapeHec [7]
	 >> m_pulseShapeHec [8]
	 >> m_pulseShapeHec [9]
	 >> m_pulseShapeHec [10]
	 >> m_pulseShapeHec [11]
	 >> m_pulseShapeHec [12]
	 >> m_pulseShapeHec [13]
	 >> m_pulseShapeHec [14]
	 >> m_pulseShapeHec [15]
	 >> m_pulseShapeHec [16]
	 >> m_pulseShapeHec [17]
	 >> m_pulseShapeHec [18]
	 >> m_pulseShapeHec [19]
	 >> m_pulseShapeHec [20]
	 >> m_pulseShapeHec [21]
	 >> m_pulseShapeHec [22]
	 >> m_pulseShapeHec [23] ;
  
  m_pulseShapeDerHec.resize(s_MAXSAMPLES);
  infile >> m_pulseShapeDerHec [0]
         >> m_pulseShapeDerHec [1]
	 >> m_pulseShapeDerHec [2]
	 >> m_pulseShapeDerHec [3]
	 >> m_pulseShapeDerHec [4]
	 >> m_pulseShapeDerHec [5]
	 >> m_pulseShapeDerHec [6]
	 >> m_pulseShapeDerHec [7]
	 >> m_pulseShapeDerHec [8]
	 >> m_pulseShapeDerHec [9]
	 >> m_pulseShapeDerHec [10]
	 >> m_pulseShapeDerHec [11]
	 >> m_pulseShapeDerHec [12]
	 >> m_pulseShapeDerHec [13]
	 >> m_pulseShapeDerHec [14]
	 >> m_pulseShapeDerHec [15]
	 >> m_pulseShapeDerHec [16]
	 >> m_pulseShapeDerHec [17]
	 >> m_pulseShapeDerHec [18]
	 >> m_pulseShapeDerHec [19]
	 >> m_pulseShapeDerHec [20]
	 >> m_pulseShapeDerHec [21]
	 >> m_pulseShapeDerHec [22]
	 >> m_pulseShapeDerHec [23] ;
  
  m_satEnergyHec.resize(s_NBETABINS);
  m_sinThetaHec.resize(s_NBETABINS);
  m_noiseRmsHec.resize(s_NBETABINS);
  m_autoCorrHec.push_back(autoCorr);
  for(int ieta=1 ; ieta<s_NBETABINS ; ieta++) {
    // now read  for each eta bin:
    // - the calibration factor (determined from MC)
    // - the transverse saturating energies (same in all eta bins but Ieta=15)
    // - the value of sin_theta 
    // - the noise rms
    infile >> Ieta
           >> m_satEnergyHec[ieta]
	   >> m_sinThetaHec[ieta]
           >> m_noiseRmsHec[ieta];

  // and the autocorrelation function for each eta bin (from Leonid Kurchaninov)
    infile >> autoCorr[0]
	   >> autoCorr[1]
	   >> autoCorr[2]
	   >> autoCorr[3]
	   >> autoCorr[4]
	   >> autoCorr[5]
	   >> autoCorr[6] ;
    m_autoCorrHec.push_back(autoCorr);
  }
  infile.close() ;
  ATH_MSG_DEBUG ( " HEC file closed " );
  ATH_MSG_INFO
    ( " 1 pulse shape for HEC : "
	 << m_pulseShapeHec[0] << ", "
	 << m_pulseShapeHec[1] << ", "
	 << m_pulseShapeHec[2] << ", "
	 << m_pulseShapeHec[3] << ", "
	 << m_pulseShapeHec[4] << ", "
	 << m_pulseShapeHec[5] << ", "
	 << m_pulseShapeHec[5] << ", "
	 << m_pulseShapeHec[6] << ", "
	 << m_pulseShapeHec[7] << ", "
	 << m_pulseShapeHec[8] << ", "
	 << m_pulseShapeHec[9] << ", "
	 << m_pulseShapeHec[10] << ", "
	 << m_pulseShapeHec[11] << ", "
	 << m_pulseShapeHec[12] << ", "
	 << m_pulseShapeHec[13] << ", "
	 << m_pulseShapeHec[14] << ", "
	 << m_pulseShapeHec[15] << ", "
	 << m_pulseShapeHec[16] << ", "
	 << m_pulseShapeHec[17] << ", "
	 << m_pulseShapeHec[18] << ", "
	 << m_pulseShapeHec[19] << ", "
	 << m_pulseShapeHec[20] << ", "
	 << m_pulseShapeHec[21] << ", "
	 << m_pulseShapeHec[22] << ", "
	 << m_pulseShapeHec[23] 
      );
  ATH_MSG_DEBUG
    ( "Finished reading calib coeff, noise rms, sat ene, auto corr for each eta bin for HEC: "  );
    for(int ieta=1 ; ieta<s_NBETABINS ; ieta++) {
// currently the calib coeffs are all set to 1 -> turning INFO logs into DEBUG
      ATH_MSG_DEBUG
        ( " Ieta= " << ieta+1
	     << ", calib coeff HEC: " << m_calibCoeffHec[ieta]
          );
      ATH_MSG_INFO
        ( " Ieta= " << ieta+1
	     << ", noise rms HEC: " << m_noiseRmsHec[ieta] << " MeV"
	     << ", sat ene HEC: " << m_satEnergyHec[ieta] << " MeV"
          );
      ATH_MSG_INFO
        ( " Ieta= " << ieta+1
	     << ", auto corr HEC: "
	     << (m_autoCorrHec[ieta])[0] << ", "
	     << (m_autoCorrHec[ieta])[1] << ", "
	     << (m_autoCorrHec[ieta])[2] << ", "
	     << (m_autoCorrHec[ieta])[3] << ", "
	     << (m_autoCorrHec[ieta])[4] << ", "
	     << (m_autoCorrHec[ieta])[5] << ", "
	     << (m_autoCorrHec[ieta])[6] << ", "
          );
    }

  //
  // ...... Read auxiliary data files for FCAL
  //

  pulsedataname=PathResolver::find_file ("LArFcalLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR ( "Could not locate LArFcalLvl1.data file" );
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    ATH_MSG_ERROR ( " cannot open FCAL file " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( " FCAL file opened " );
  }

  const int nMod = 3;
  //  m_noiseRmsFcal.resize(nMod);
  int imod=0;

  for(int iMod=0; iMod < nMod; iMod++) {

    // first read the module number + noise rms (from J. Rutherfoord)
    //    infile >> imod >> m_noiseRmsFcal[iMod] ;
    infile >> imod 
	   >> noiseRms4[0]
	   >> noiseRms4[1]
	   >> noiseRms4[2]
	   >> noiseRms4[3] ;
    m_noiseRmsFcal.push_back(noiseRms4);

    // read the pulse shape for this module (from John Rutherfoord)
    infile >> pulseShape [0]
	   >> pulseShape [1]
	   >> pulseShape [2]
	   >> pulseShape [3]
	   >> pulseShape [4]
	   >> pulseShape [5]
	   >> pulseShape [6]
	   >> pulseShape [7]
	   >> pulseShape [8]
	   >> pulseShape [9]
	   >> pulseShape [10]
	   >> pulseShape [11]
	   >> pulseShape [12]
	   >> pulseShape [13]
	   >> pulseShape [14]
	   >> pulseShape [15]
	   >> pulseShape [16]
	   >> pulseShape [17]
	   >> pulseShape [18]
	   >> pulseShape [19]
	   >> pulseShape [20]
	   >> pulseShape [21]
	   >> pulseShape [22]
	   >> pulseShape [23] ;
    m_pulseShapeFcal.push_back(pulseShape);

    // read the pulse shape derivative
    infile >> pulseShapeDer [0]
	   >> pulseShapeDer [1]
	   >> pulseShapeDer [2]
	   >> pulseShapeDer [3]
	   >> pulseShapeDer [4]
	   >> pulseShapeDer [5]
	   >> pulseShapeDer [6]
	   >> pulseShapeDer [7]
	   >> pulseShapeDer [8]
	   >> pulseShapeDer [9]
	   >> pulseShapeDer [10]
	   >> pulseShapeDer [11]
	   >> pulseShapeDer [12]
	   >> pulseShapeDer [13]
	   >> pulseShapeDer [14]
	   >> pulseShapeDer [15]
	   >> pulseShapeDer [16]
	   >> pulseShapeDer [17]
	   >> pulseShapeDer [18]
	   >> pulseShapeDer [19]
	   >> pulseShapeDer [20]
	   >> pulseShapeDer [21]
	   >> pulseShapeDer [22]
	   >> pulseShapeDer [23] ;
    m_pulseShapeDerFcal.push_back(pulseShapeDer);
  }

  m_autoCorrFcal.resize(s_NBSAMPLES);
  // finally read standard autocorrelation matrix
  infile >> m_autoCorrFcal[0]
	 >> m_autoCorrFcal[1]
	 >> m_autoCorrFcal[2]
	 >> m_autoCorrFcal[3]
	 >> m_autoCorrFcal[4]
	 >> m_autoCorrFcal[5]
	 >> m_autoCorrFcal[6] ;
  
  infile.close() ;
  ATH_MSG_DEBUG ( " FCAL file closed " );

  std::vector<float> auxV(3);
  m_calibCoeffFcal.push_back(m_calibCoeffFcalEm);
  m_calibCoeffFcal.push_back(m_calibCoeffFcalHad);

  ATH_MSG_DEBUG
    ( "Finished reading noise, calib coeff and pulse shape for each module for FCAL: " );
  for(int iMod=0; iMod < nMod; iMod++) {

    ATH_MSG_INFO
      ( " iMod= " << iMod
	   << ", noise rms FCAL (eta bin 1,2,3,4): " 
	   << m_noiseRmsFcal[iMod] << " (transverse) MeV "
        );
    if(iMod < 2){
      ATH_MSG_INFO
        ( " iMod= " << iMod
	     << ", calib coeff FCAL (eta bin 1,2,3,4): " 
	     << (m_calibCoeffFcal[iMod])[0] << ", "
	     << (m_calibCoeffFcal[iMod])[1] << ", "
	     << (m_calibCoeffFcal[iMod])[2] << ", "
	     << (m_calibCoeffFcal[iMod])[3]
          );
    }
    ATH_MSG_INFO
      ( " iMod= " << iMod
	   << ", pulse shape FCAL: " 
	   << (m_pulseShapeFcal[iMod])[0] << ", "
	   << (m_pulseShapeFcal[iMod])[1] << ", "
	   << (m_pulseShapeFcal[iMod])[2] << ", "
	   << (m_pulseShapeFcal[iMod])[3] << ", "
	   << (m_pulseShapeFcal[iMod])[4] << ", "
	   << (m_pulseShapeFcal[iMod])[5] << ", "
	   << (m_pulseShapeFcal[iMod])[6] << ", "
	   << (m_pulseShapeFcal[iMod])[7] << ", "
	   << (m_pulseShapeFcal[iMod])[8] << ", "
	   << (m_pulseShapeFcal[iMod])[9] << ", "
	   << (m_pulseShapeFcal[iMod])[10] << ", "
	   << (m_pulseShapeFcal[iMod])[11] << ", "
	   << (m_pulseShapeFcal[iMod])[12] << ", "
	   << (m_pulseShapeFcal[iMod])[13] << ", "
	   << (m_pulseShapeFcal[iMod])[14] << ", "
	   << (m_pulseShapeFcal[iMod])[15] << ", "
	   << (m_pulseShapeFcal[iMod])[16] << ", "
	   << (m_pulseShapeFcal[iMod])[17] << ", "
	   << (m_pulseShapeFcal[iMod])[18] << ", "
	   << (m_pulseShapeFcal[iMod])[19] << ", "
	   << (m_pulseShapeFcal[iMod])[20] << ", "
	   << (m_pulseShapeFcal[iMod])[21] << ", "
	   << (m_pulseShapeFcal[iMod])[22] << ", "
	   << (m_pulseShapeFcal[iMod])[23]
        );
  }
  ATH_MSG_INFO
    ( "auto corr FCAL: " 
	 << m_autoCorrFcal[0] << ", "
	 << m_autoCorrFcal[1] << ", "
	 << m_autoCorrFcal[2] << ", "
	 << m_autoCorrFcal[3] << ", "
	 << m_autoCorrFcal[4] << ", "
	 << m_autoCorrFcal[5] << ", "
	 << m_autoCorrFcal[6]
      );

  // now the relative gains
  pulsedataname=PathResolver::find_file ("Fcal_ptweights_table7.data", "DATAPATH");
  if (pulsedataname == "") {
    ATH_MSG_ERROR ( "Could not locate Fcal_ptweights_table7.data file" );
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    ATH_MSG_ERROR ( " cannot open FCAL gains file " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( " FCAL gains file opened " );
  }

  // first read the gain file for all cells (from John Rutherfoord)
  
  m_cellRelGainFcal.resize(m_fcalHelper->channel_hash_max());
  // file structure:
  const unsigned int colNum=14;
  // number of cells per 'group' (sharing the same gain)
  const unsigned int maxCell = 4;
  std::vector<std::string> TTlabel;
  TTlabel.resize(colNum);
  std::vector<float> gain;
  gain.resize(colNum);
  int iline=0;
  int ngain=0;

  while ( infile >> TTlabel [0] >> gain [0]
	  >> TTlabel [1] >> gain [1]
	  >> TTlabel [2] >> gain [2]
	  >> TTlabel [3] >> gain [3]
	  >> TTlabel [4] >> gain [4]
	  >> TTlabel [5] >> gain [5]
	  >> TTlabel [6] >> gain [6]
	  >> TTlabel [7] >> gain [7]
	  >> TTlabel [8] >> gain [8]
	  >> TTlabel [9] >> gain [9]
	  >> TTlabel [10] >> gain [10]
	  >> TTlabel [11] >> gain [11]
	  >> TTlabel [12] >> gain [12]
	  >> TTlabel [13] >> gain [13]) {

    ATH_MSG_DEBUG ( " TTlabel[0], gain[0]= " << TTlabel[0] << ", " << gain[0] );
    ATH_MSG_DEBUG ( " [...] ");
    ATH_MSG_DEBUG
      ( " TTlabel[13], gain[13]= " << TTlabel[13] << ", "  << gain[13] );

    //int barrel_ec_fcal = 2;
    //int pos_neg = -2; // C side (neg z)
    int detZside = -1;
    if(iline < 32) {detZside = 1;} // A side (pos z)
    //if(iline < 32) {pos_neg = 2;} // A side (pos z)
    for(unsigned int icol=0; icol<colNum; icol++){

      int em_had = 0; // FCAL1
      int layer=0;
      if(icol > 7) {
	em_had = 1;  // FCAL2+FCAL3
	if(icol > 11) {layer=1;}
      }
      int region = 3 ; // FCAL

      std::string TTlab = TTlabel[icol];
      //int Ieta = int(TTlab[0])-48;
      //int Lphi = int(TTlab[1])-64;
      int  eta = int(TTlab[0])-49;
      int  phi = int(TTlab[1])-65;
      int nPhi = 16;
      if(detZside<0) {
	phi = ( phi<nPhi/2 ? nPhi/2 -1 -phi : 3*nPhi/2 -1 -phi );
      }
 
      int group = 1;
      if(TTlab.size() > 3) {group = int(TTlab[3])-48;}

      // added Nov 2009 following introduction of TTCell map "FcalFix" 
      // modified again Jun 2010 following introduction of TTCell map "FcalFix2" (the C side is not symmetric wrt the A side)
      // modified again Jan 2011 in an attempt to correctly load the gains to the correct cells (not all cells were being assigned gains)
      if(em_had) {  // FCAL-had only
	if( (layer==1 && detZside>0) || (layer==0 && detZside<0) ) {
	  if(eta==0||eta==2) {
	    eta+=1;
	    //group+=1; //- OLD CODE (pre Jan2011 change)
		
	  } else {
		if(layer==1) {group+=1;} else { group+=2; }
	  }
	}
	else {
	  if(eta==1||eta==3) {
	    eta-=1;
	    //group+=2; //- OLD CODE (pre Jan2011 change)
	    if(layer==1) {group+=1;} else { group+=2; }
	  }
	}
      }
      //
      // ... create an offline TT+layer identifier from the labels read in.
      //
      Identifier ttId = m_lvl1Helper->layer_id(detZside, em_had, region, eta, phi, layer) ;
      ATH_MSG_DEBUG
        ( "ttId= " << m_lvl1Helper->show_to_string(ttId) );
      // 
      //... fill a vector with offline identifier of cells belonging to this tower (with layer info)
      //
      std::vector<Identifier> cellIdVec = m_ttSvc->createCellIDvecLayer(ttId);
      //
      // .... loop on all LAr offline channels belonging to the trigger tower (with layer info) 
      //
      std::vector<unsigned int> hashVec;
      // number of connected cells in the current tower
      unsigned int nCell=0;
      for (unsigned int ichan=0;ichan<cellIdVec.size();ichan++){	
	Identifier cellId=cellIdVec[ichan];
	
	int cellPhi = m_fcalHelper->phi(cellId);
	if(cellPhi >= 0) {  // protection skipping unconnected channels (they have eta=phi=-999) - normally, they should not be in the list
	  nCell++;
	  //
	  // .... convert to FCAL hash
	  //
	  IdentifierHash fcalHash = m_fcalHelper->channel_hash(cellId);
	  //
	  //... save hash indices
	  //
	  hashVec.push_back(fcalHash);
	} // end condition cellPhi>=0
	
      } // end of loop on channels in the tower
      ATH_MSG_DEBUG ( "nCell= " << nCell );
      
      //
      //... loop on the 4 cells of the current group and put their gain in the gain vector (indexed by hashes)
      //
      for(unsigned int iCell=0; iCell<maxCell; iCell++) {
	unsigned int index0 = (group-1)*maxCell + iCell ;
	if(index0 < nCell) {  // unconnected channels have highest hash ids (because highest eta values)
	  unsigned int index = hashVec[index0] ;
	  m_cellRelGainFcal [index] = gain[icol] ;
	  ngain++;
	  ATH_MSG_DEBUG ( " index, gain= " << index << ", " << gain[icol] );
	}
      }
      
    } // end of loop on columns
    iline++;
  } // end of loop on lines

  
  ATH_MSG_DEBUG ( " number of lines found= " << iline  );
  infile.close() ;
  ATH_MSG_INFO ( " FCAL gains file closed, extracted " << ngain << " gains" );

  return StatusCode::SUCCESS;

}
//===========================================================================
int LArTTL1Maker::decodeInverse(int region, int eta)
{
//===========================================================================
// Maps [ region , eta ]  to [ Ieta] 
// ==========================================================================
// Problem: this is NOT a bijection, because of the "barrel end" zone. 
//         Convention: only the barrel part of the identifying fields is returned 
//===========================================================================
 
 int Ieta=0;
 if(region == 0){  // Barrel + EC-OW
   if(eta <= 14) {
     Ieta=eta+1;         
   }
   else {
     Ieta=eta-13;
   }
 }
 else if( region == 1 || region == 2) { // EC-IW
   if(region == 1) {   
     Ieta=eta+12;
   }
   else {   
     Ieta=15;
   }
 }
 else if(region == 3) {  // FCAL 
   Ieta=eta+1;
 }
 return Ieta ; 
}


StatusCode LArTTL1Maker::initHitMap()
{
  if (m_useMapfromStore) {
    ATH_MSG_INFO ( " Use LArHitEMap from detector store. should be filled by LArDigitMaker (digitmaker1)" );
    return StatusCode::SUCCESS;
  }

  m_hitmap = new LArHitEMap();

//
// ......... make the Sub-detector flag vector
//
  static std::vector<bool> SubDetFlag;
  for (int i=0; i < LArHitEMap::NUMDET ; i++)
  {
   SubDetFlag.push_back(true);
  }

//
//  put all cell energies to 0.
//

  if ( ! m_hitmap->Initialize(SubDetFlag) )
    {
      ATH_MSG_FATAL ( "Making of the cell table failed"  );
      return StatusCode::FAILURE;
    }
  
  ATH_MSG_INFO ( "Number of created cells in LArHitEMap " 
                 << m_hitmap->GetNbCells() );

  return StatusCode::SUCCESS;
}
