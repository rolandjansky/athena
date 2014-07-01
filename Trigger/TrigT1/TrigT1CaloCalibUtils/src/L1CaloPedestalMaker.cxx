/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloPedestalMaker.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"

#include "TrigT1CaloCalibConditions/L1CaloPedestal.h"
#include "TrigT1CaloCalibConditions/L1CaloPedestalContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

L1CaloPedestalMaker::L1CaloPedestalMaker(const std::string& name, ISvcLocator* pSvcLocator): Algorithm(name, pSvcLocator),
    m_detStore(0),
    m_storeGate(0),
    m_caloMgr(0),
    m_lvl1Helper(0),
    m_ttSvc(0),
    m_l1CondSvc(0),
    m_regSvc(0),
    m_streamer(0),
    m_towerKey(0)
{
	declareProperty("TriggerTowerLocation", m_triggerTowerLocation = "LVL1TriggerTowers");
	declareProperty("PedestalsKey", m_PedestalsKey = "L1CaloPedestals");

	declareProperty("Store2POOL", m_bStore2POOL = false);
	declareProperty("Store2COOL", m_bStore2COOL = false);

	declareProperty("BeginRun", m_iovBeginRun = 0);
	declareProperty("EndRun", m_iovEndRun = 0x7FFFFFFF);
	declareProperty("BeginEvent", m_iovBeginEvent = 0);
	declareProperty("EndEvent", m_iovEndEvent = 0xFFFFFFFF);

	declareProperty("StreamName", m_streamName = "POOLStream");

	declareProperty("DBFolder", m_DBFolder = "");
	declareProperty("DBTag", m_DBTag = "");

	declareProperty("NRefEvents", m_nref = 50);
	declareProperty("NSigmaCut", m_rms_cut = 5.0);
}

L1CaloPedestalMaker::~L1CaloPedestalMaker()
{
	 delete m_towerKey;
}

//-------------------------------------------
// Initialize
//-------------------------------------------
StatusCode L1CaloPedestalMaker::initialize()
{
    MsgStream log( msgSvc(), name() );
    log << MSG::INFO <<"From Initialize..."<<endreq;

    StatusCode sc;

    //get a pointer to DetectorStore services
    sc = service("DetectorStore", m_detStore);
    if (sc.isFailure()) {
        log << MSG::ERROR << "Cannot access DetectorStore" << endreq;
        return StatusCode::FAILURE;
    }

    //get a pointer to Event StoreGate services
    sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
        log << MSG::ERROR << "Cannot access StoreGate" << endreq;
        return StatusCode::FAILURE;
    }

	sc = service("L1CaloCondSvc", m_l1CondSvc);
	if(sc.isFailure()){
		log << MSG::ERROR << "Could not retrieve L1CaloCondSvc" << endreq;
		return StatusCode::FAILURE;
 	}

    // Retrieve CaloIdManager
	sc = m_detStore->retrieve(m_caloMgr);
    if (sc.isFailure()) {
        log << MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
        return StatusCode::FAILURE;
    }

    m_lvl1Helper = m_caloMgr->getLVL1_ID();
	if (!m_lvl1Helper) {
        log << MSG::ERROR << "Could not access CaloLVL1_ID helper" << endreq;
        return StatusCode::FAILURE;
	}

	// Retrieve cabling & tt services
	ISvcLocator* svcLoc = Gaudi::svcLocator( );
	IToolSvc* toolSvc = 0; // Pointer to Tool Service

	sc = svcLoc->service( "ToolSvc",toolSvc  );
	if(sc.isSuccess()) {
	 	sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
		if(sc.isFailure()){
			log << MSG::ERROR << "Could not retrieve CaloTriggerTowerService Tool" << endreq;
			return StatusCode::FAILURE;
	 	}

	 	if (m_bStore2POOL) {
	 		sc = toolSvc->retrieveTool("AthenaPoolOutputStreamTool", m_streamName, m_streamer);
			if (sc.isFailure()) {
			    log << MSG::INFO << "Unable to find AthenaOutputStreamTool" << endreq;
			    return StatusCode::FAILURE;
			}
	 	}
	} else {
	  log << MSG::ERROR << "Could not retrieve ToolSvc" << endreq;
	  return StatusCode::FAILURE;
	}

  // Get the IOVRegistrationSvc when needed
  if (m_bStore2COOL) {
		sc = service("IOVRegistrationSvc", m_regSvc);
		if (sc.isFailure()) {
		    log << MSG::INFO << "Unable to find IOVRegistrationSvc " << endreq;
		    return StatusCode::FAILURE;
		} else {
		    log << MSG::DEBUG << "Found IOVRegistrationSvc "  << endreq;
		}
	  log  << MSG::INFO << "Tag to be used: " << m_DBTag << endreq;
  }

  m_towerKey = new LVL1::TriggerTowerKey();

  return StatusCode::SUCCESS;
}

//----------------------------------------
// Execute
//----------------------------------------
StatusCode L1CaloPedestalMaker::execute()
{
  MsgStream log( msgSvc(), name() );
  StatusCode sc;

  // retrieve triggertowers container from storegate
  const TriggerTowerCollection* ttCollection = 0;
  sc = m_storeGate->retrieve(ttCollection, m_triggerTowerLocation);
  if (sc.isFailure() || !ttCollection) {
    log << MSG::ERROR << "No Trigger Towers found" << endreq;
    return StatusCode::SUCCESS;
  } else log << MSG::DEBUG <<"TriggerTowerCollection retrieved from StoreGate, size: "<< ttCollection->size() <<endreq;


    //---
    //loop on trigger towers
    TriggerTowerCollection::const_iterator pos = ttCollection->begin();
    TriggerTowerCollection::const_iterator pos_end = ttCollection->end();

    for(;pos!=pos_end;++pos) {
        LVL1::TriggerTower* ptt = (*pos);

        // Compute TowerTrigger Key out of its coordinates
        double eta = ptt->eta();
        double phi = ptt->phi();

        //std::vector<int> emSamples = ptt->emADC();
        //std::vector<int> hadSamples = ptt->hadADC();

        // Loop on EM & HAD calorimeters
        for(int idet=0;idet<2;++idet) {

            //HWIdentifier channelId;
            Identifier Id;

            // temporary, compute id out of the tower position
            int pos_neg_z = (int)(fabs(eta)/eta);
            int region = _lvl1Id_region(eta);
            int ieta   = _lvl1Id_eta(eta);
            int iphi   = _lvl1Id_phi(eta, phi);
            int sampling;

            std::vector<int> samples;

            if(idet==0) {
                sampling = 0; // EM calorimeter
                Id = m_lvl1Helper->tower_id(pos_neg_z, sampling, region, ieta, iphi);
                samples = ptt->emADC();
            }
            if(idet==1) {
                sampling = 1; // Hadronic calorimeter
                Id = m_lvl1Helper->tower_id(pos_neg_z, sampling, region, ieta, iphi);
                samples = ptt->hadADC();
            }

            //log<<MSG::INFO<<samples[0]<<" "<<samples[1]<<" "<<samples[2]<<" "<<samples[3]<<" "<<samples[4]<<endreq;

            m_mapPedestalCumul[Id];

            if(m_mapPedestalCumul[Id].get_max()!=0) {
                // once min and max are defined, check if sample are in window
                std::vector<int>::const_iterator s_it = samples.begin();
                std::vector<int>::const_iterator s_it_end = samples.end();
                const int min = m_mapPedestalCumul[Id].get_min();
                const int max = m_mapPedestalCumul[Id].get_max();
                for(;s_it!=s_it_end && (*s_it)>=min && (*s_it)<=max;++s_it)
                  ;
                if(s_it==s_it_end) {
                    m_mapPedestalCumul[Id].add(samples);
                }
            } else {
                m_mapPedestalCumul[Id].add(samples);
                // if the number of reference events is reached, the min and max values
                // are computed then the pedestal is cleared
                if(m_mapPedestalCumul[Id].get_nentries()==m_nref && m_nref>0) {
                    const double mean = m_mapPedestalCumul[Id].get_mean();
                    const double rms  = m_mapPedestalCumul[Id].get_rms();
                    const int min = (int) floor(mean - m_rms_cut*rms);
                    const int max = (int) ceil(mean + m_rms_cut*rms);
                    m_mapPedestalCumul[Id].set_min(min);
                    m_mapPedestalCumul[Id].set_max(max);
                    m_mapPedestalCumul[Id].clear();
                }
            }

        } // -- End Loop on EM & HAD calorimeters --

    } // -- End of trigger tower loop --

  return StatusCode::SUCCESS;
}

//-----------------------------------------
// Finalize
//-----------------------------------------
StatusCode L1CaloPedestalMaker::finalize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO <<"From finalize..."<<endreq;

  StatusCode sc;

  L1CaloPedestalContainer* pedestalContainer = new L1CaloPedestalContainer;

  log<<MSG::DEBUG<<"Map size: "<<m_mapPedestalCumul.size() <<endreq;

  std::map< Identifier, L1CaloPedestalCumul>::const_iterator it = m_mapPedestalCumul.begin();
  std::map< Identifier, L1CaloPedestalCumul>::const_iterator it_end = m_mapPedestalCumul.end();

  for(;it!=it_end;++it) {

      //convert offline Id into an Online one
      HWIdentifier channelId;
      try {
        channelId = m_ttSvc->createTTChannelID(it->first);
      } catch(CaloID_Exception& except) {
        log << MSG::ERROR << "CaloID_Exception " << (std::string) except << endreq ;
      }

      // Convert Athena Online Id into a TDAQ online one
      L1CaloCoolChannelId coolChannelId;
      try {
        coolChannelId = m_ttSvc->createL1CoolChannelId(channelId);
      } catch(CaloID_Exception& except) {
        log << MSG::ERROR << "CaloID_Exception " << (std::string) except << endreq ;
      }

      L1CaloPedestal* pedestal = new L1CaloPedestal(coolChannelId.id(), it->second);
      pedestalContainer->push_back(pedestal);
  }

  sc = m_detStore->record(pedestalContainer, m_PedestalsKey);
  if (sc.isSuccess()) log << MSG::DEBUG << "Stored Pedestals in DetStore at "<< m_PedestalsKey<< " with size: "<< pedestalContainer->size() << endreq;
  else {
    log << MSG::ERROR << "failed to write Pedestals to DetStore at " << m_PedestalsKey << endreq;
    return StatusCode::FAILURE;
  }

  //---
  // Write transient PedestalContainer to POOL
  if (m_bStore2POOL) {
  	log << MSG::INFO << "Writing L1CaloPedestalContainer to POOL file"  << endreq;
    sc = m_streamer->connectOutput();
    if (sc.isFailure()) {
			log <<MSG::ERROR <<"Could not connect stream to output" <<endreq;
			return( StatusCode::FAILURE);
    }

    int npairs = 1;
    int index = 0;
    IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
		IAthenaOutputStreamTool::TypeKeyPair   pedestalsContPair("L1CaloPedestalContainer", "");
		typeKeys[index] = pedestalsContPair;
		++index;

	  log << MSG::DEBUG <<"Stream out for pairs:" <<endreq;
	  for (unsigned int i = 0; i < typeKeys.size(); ++i) {
	      log << MSG::DEBUG << typeKeys[i].first << " " << typeKeys[i].second << " " << endreq;
	  }

	  log << MSG::DEBUG <<"StreamObjects" <<endreq;
	  sc = m_streamer->streamObjects(typeKeys);
	  if (sc.isFailure()) {
			log <<MSG::ERROR <<"Could not stream out L1CaloPedestalContainer" <<endreq;
			return( StatusCode::FAILURE);
	  }
	  log << MSG::DEBUG <<"commitOutput" <<endreq;
	  sc = m_streamer->commitOutput();
	  if (sc.isFailure()) {
			log <<MSG::ERROR <<"Could not commit output stream" <<endreq;
			return( StatusCode::FAILURE);
		}
  }

  //---
  // register condition objetcs in IOVDB
  if (m_bStore2COOL) {
    log << MSG::INFO << "Registering L1CaloPedestalContainer to IOVDB"  << endreq;
    StatusCode sc;
    const L1CaloPedestalContainer* constPedestalContainer = pedestalContainer;
    std::string tag = "no tag";
    if (m_DBTag!="") {
    	tag = m_DBFolder;
    	tag.erase(std::remove( tag.begin(), tag.end(), '/' ),tag.end()); //remove all '/'
    	tag = tag + m_DBTag;
    	sc = m_l1CondSvc->store(constPedestalContainer, m_PedestalsKey, m_DBFolder, tag);
    } else {
    	sc = m_l1CondSvc->store(constPedestalContainer, m_PedestalsKey, m_DBFolder, "");
    }

    if (sc.isFailure()) {
			log <<MSG::ERROR <<"Could not register in IOV DB for L1CaloPedestalContainer" <<endreq;
			return( StatusCode::FAILURE);
    }
    log << MSG::DEBUG << "Registered L1CaloPedestalContainer in folder "<< m_DBFolder <<" with tag " << tag << endreq;
  }

	log << MSG::INFO <<"End of finalize..."<<endreq;
  return StatusCode::SUCCESS;
}

// return the offline region index from the trigger tower eta position
int L1CaloPedestalMaker::_lvl1Id_region(double eta) {
	int region = -1;
	double fabseta = fabs(eta);
	if(fabseta<2.5)      region = 0;  //gran 0.1
	else if(fabseta<3.1) region = 1;  //gran 0.2
	else if(fabseta<3.2) region = 2;  //gran 0.1
	else if(fabseta<4.9) region = 3;  //gran 0.425
	return region;
}

// return the offline eta index from the trigger tower eta position
int L1CaloPedestalMaker::_lvl1Id_eta(double eta) {
	double gran[4] = {0.1, 0.2, 0.1, 0.425};
  double offset[4] = {0., 2.5, 3.1, 3.2};

	int region = _lvl1Id_region(eta);
	if(region>=0 && region <4) {
		double etatemp = ((fabs(eta)-offset[region])/gran[region] - 0.5);
		//return (int)((fabs(eta)-offset[region])/gran[region] - 0.5);
		if((etatemp-floor(etatemp))>0.5) return (int)ceil(etatemp); // to handle bad rounding, not very elegant...
		else return (int)floor(etatemp);
	}
	else return -1;
}

// return the offline phi index from the trigger tower eta and phi positions
int L1CaloPedestalMaker::_lvl1Id_phi(double eta, double phi) {
	int phimax[4] = { 63, 31, 31, 15 };
	int region = _lvl1Id_region(eta);
	if(region>=0 && region <4) {
		double phitemp = (phi*(phimax[region]+1)/(2*M_PI)-0.5);
		//if(region>=0 && region <4) return (int)(phi*(phimax[region]+1)/(2*M_PI)-0.5);
		if((phitemp-floor(phitemp))>0.5) return (int)ceil(phitemp); // to handle bad rounding, not very elegant...
		else return (int)floor(phitemp);
	}	else return -1;
}
