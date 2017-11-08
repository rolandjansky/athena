/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCDigitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonDigitContainer/sTgcDigit.h"

#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "MuonDigitContainer/sTgcDigitCollection.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode sTGCDigitVariables::fillVariables()
{
  ATH_MSG_INFO(" do fillNSWsTGCDigitVariables()");
  MSG::Level msgLevel = MSG::VERBOSE;
  m_msg.get().setLevel(msgLevel);

  CHECK( this->clearVariables() );

  // get truth information container of digitization
  const MuonSimDataCollection* nsw_sTgcSdoContainer = nullptr;
//  CHECK( m_evtStore->retrieve(nsw_sTgcSdoContainer, "sTGC_SDO") );

  const sTgcDigitContainer* nsw_sTgcDigitContainer = nullptr;
  CHECK( m_evtStore->retrieve(nsw_sTgcDigitContainer, "sTGC_DIGITS") );
  
  sTgcDigitContainer::const_iterator it   = nsw_sTgcDigitContainer->begin();
  sTgcDigitContainer::const_iterator it_e = nsw_sTgcDigitContainer->end();
  ATH_MSG_DEBUG("retrieved sTGC Digit Container with size "<<nsw_sTgcDigitContainer->digit_size());

  if(nsw_sTgcDigitContainer->size()==0) ATH_MSG_WARNING(" sTGC Digit Continer empty ");
  for(; it!=it_e; ++it) {
      const sTgcDigitCollection* coll = *it;
      ATH_MSG_DEBUG( "processing collection with size " << coll->size() );
  for (unsigned int digitNum=0; digitNum<coll->size(); digitNum++) {
  	const sTgcDigit* digit = coll->at(digitNum);
  	Identifier Id = digit->identify();

    std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
    int stationEta       = m_sTgcIdHelper->stationEta(Id);
    int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
    int multiplet        = m_sTgcIdHelper->multilayer(Id);
    int gas_gap          = m_sTgcIdHelper->gasGap(Id);
    int channel          = m_sTgcIdHelper->channel(Id);
    int stationEtaMin    = m_sTgcIdHelper->stationEtaMin(Id);
    int stationEtaMax    = m_sTgcIdHelper->stationEtaMax(Id);
    int stationPhiMin    = m_sTgcIdHelper->stationPhiMin(Id);
    int stationPhiMax    = m_sTgcIdHelper->stationPhiMax(Id);
    int gas_gapMin       = m_sTgcIdHelper->gasGapMin(Id);
    int gas_gapMax       = m_sTgcIdHelper->gasGapMax(Id);
    int padEta           = m_sTgcIdHelper->padEta(Id);
    int padPhi           = m_sTgcIdHelper->padPhi(Id);
    int NofMultilayers   = m_sTgcIdHelper->numberOfMultilayers(Id);
    int multilayerMin    = m_sTgcIdHelper->multilayerMin(Id);
    int multilayerMax    = m_sTgcIdHelper->multilayerMax(Id);
    int channelTypeMin   = m_sTgcIdHelper->channelTypeMin(Id);
    int channelTypeMax   = m_sTgcIdHelper->channelTypeMax(Id);
    int channelMin       = m_sTgcIdHelper->channelMin(Id);
    int channelMax       = m_sTgcIdHelper->channelMax(Id);
    int channelType      = m_sTgcIdHelper->channelType(Id);

    ATH_MSG_DEBUG(     "sTGC Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]"
                      << " Station EtaMin ["  << stationEtaMin      << "]"
                      << " Station EtaMax ["  << stationEtaMax      << "]"
                      << " Station PhiMin ["  << stationPhiMin      << "]"
                      << " Station PhiMax ["  << stationPhiMax      << "]");

    int isSmall = stName[2] == 'S';
		const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet );

		int channelNumber = 0;
		const Identifier phiId, etaId;
		Amg::Vector3D gpos(0.,0.,0.);
		Amg::Vector2D lpos(0.,0.);

		rdoEl->stripPosition(Id,lpos);
		rdoEl->surface(Id).localToGlobal(lpos, gpos,gpos);


		m_NSWsTGC_dig_globalPosX->push_back( gpos.x() );
		m_NSWsTGC_dig_globalPosY->push_back( gpos.y() );
		m_NSWsTGC_dig_globalPosZ->push_back( gpos.z() );
		m_NSWsTGC_dig_localPosX->push_back( lpos.x() );
		m_NSWsTGC_dig_localPosY->push_back( lpos.y() );
		channelNumber  = rdoEl->stripNumber(lpos,Id);
		m_NSWsTGC_dig_channelPosX->push_back( lpos.x() );
		m_NSWsTGC_dig_channelPosY->push_back( lpos.y() );

		m_NSWsTGC_dig_channel_type->push_back(channelType);
		m_NSWsTGC_dig_stationName->push_back(stName);
		m_NSWsTGC_dig_stationEta->push_back(stationEta);
		m_NSWsTGC_dig_stationPhi->push_back(stationPhi);
		m_NSWsTGC_dig_multiplet->push_back(multiplet);
		m_NSWsTGC_dig_gas_gap->push_back(gas_gap);
		m_NSWsTGC_dig_channel->push_back(channel);
		m_NSWsTGC_dig_stationEtaMin->push_back(stationEtaMin);
		m_NSWsTGC_dig_stationEtaMax->push_back(stationEtaMax);
		m_NSWsTGC_dig_stationPhiMin->push_back(stationPhiMin);
		m_NSWsTGC_dig_stationPhiMax->push_back(stationPhiMax);
		m_NSWsTGC_dig_gas_gapMin->push_back(gas_gapMin);
		m_NSWsTGC_dig_gas_gapMax->push_back(gas_gapMax);
		m_NSWsTGC_dig_padEta->push_back(padEta);
		m_NSWsTGC_dig_padPhi->push_back(padPhi);
		m_NSWsTGC_dig_numberOfMultilayers->push_back(NofMultilayers);
		m_NSWsTGC_dig_multilayerMin->push_back(multilayerMin);
		m_NSWsTGC_dig_multilayerMax->push_back(multilayerMax);
		m_NSWsTGC_dig_channelTypeMin->push_back(channelTypeMin);
		m_NSWsTGC_dig_channelTypeMax->push_back(channelTypeMax);
		m_NSWsTGC_dig_channelMin->push_back(channelMin);
		m_NSWsTGC_dig_channelMax->push_back(channelMax);
		m_NSWsTGC_dig_channelNumber->push_back(channelNumber);
        
        m_NSWsTGC_dig_bctag->push_back(digit->bcTag());
		m_NSWsTGC_dig_time->push_back(digit->time());
		m_NSWsTGC_dig_charge->push_back(digit->charge());
		m_NSWsTGC_dig_isDead->push_back(digit->isDead());
		m_NSWsTGC_dig_isPileup->push_back(digit->isPileup());


		// retrieve the MC truth associated with the digit (means the Geant4 hit information)
		if (nsw_sTgcSdoContainer) {

			const MuonSimData sTgc_sdo = (nsw_sTgcSdoContainer->find(Id))->second;
			std::vector<MuonSimData::Deposit> deposits;
			sTgc_sdo.deposits(deposits);

			int    truth_barcode   = deposits[0].first.barcode();
			double truth_localPosX = deposits[0].second.firstEntry();
			double truth_localPosY = deposits[0].second.secondEntry();
			float  truth_angle     = sTgc_sdo.word()/1000.;
			Amg::Vector2D hit_on_surface(truth_localPosX, truth_localPosY);
			Amg::Vector3D hit_gpos(0., 0., 0.);
			rdoEl->surface(Id).localToGlobal(hit_on_surface, Amg::Vector3D(0., 0., 0.), hit_gpos);
			ATH_MSG_DEBUG("        sTGC Digit, truth barcode=" << truth_barcode);
			ATH_MSG_DEBUG("      sTGC Digit, truth localPosX=" << std::setw(9) << std::setprecision(2) << truth_localPosX
															<< ", truth localPosY=" << std::setw(9) << std::setprecision(2) << truth_localPosY
															<< ", truth XZ angle=" << std::setw(8) << std::setprecision(5) << truth_angle);


			m_NSWsTGC_dig_truth_barcode->push_back( truth_barcode );
			m_NSWsTGC_dig_truth_localPosX->push_back( truth_localPosX );
			m_NSWsTGC_dig_truth_localPosY->push_back( truth_localPosY );
			m_NSWsTGC_dig_truth_XZ_angle->push_back( truth_angle );
			m_NSWsTGC_dig_truth_globalPosX->push_back( hit_gpos[0] );
			m_NSWsTGC_dig_truth_globalPosY->push_back( hit_gpos[1] );
			m_NSWsTGC_dig_truth_globalPosZ->push_back( hit_gpos[2] );
		}
        if(channelType == 0) m_NSWsTGC_nPadDigits++;
		m_NSWsTGC_nDigits++;
	}
}
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode sTGCDigitVariables::clearVariables()
{

  m_NSWsTGC_nDigits = 0;
  m_NSWsTGC_nPadDigits = 0;
  m_NSWsTGC_dig_time->clear();
  m_NSWsTGC_dig_bctag->clear();
  m_NSWsTGC_dig_charge->clear();
  m_NSWsTGC_dig_isDead->clear();
  m_NSWsTGC_dig_isPileup->clear();
  m_NSWsTGC_dig_stationName->clear();
  m_NSWsTGC_dig_stationEta->clear();
  m_NSWsTGC_dig_stationPhi->clear();
  m_NSWsTGC_dig_multiplet->clear();
  m_NSWsTGC_dig_gas_gap->clear();
  m_NSWsTGC_dig_channel_type->clear();
  m_NSWsTGC_dig_channel->clear();
  m_NSWsTGC_dig_stationEtaMin->clear();
  m_NSWsTGC_dig_stationEtaMax->clear();
  m_NSWsTGC_dig_stationPhiMin->clear();
  m_NSWsTGC_dig_stationPhiMax->clear();
  m_NSWsTGC_dig_gas_gapMin->clear();
  m_NSWsTGC_dig_gas_gapMax->clear();
  m_NSWsTGC_dig_padEta->clear();
  m_NSWsTGC_dig_padPhi->clear();
  m_NSWsTGC_dig_numberOfMultilayers->clear();
  m_NSWsTGC_dig_multilayerMin->clear();
  m_NSWsTGC_dig_multilayerMax->clear();
  m_NSWsTGC_dig_channelTypeMin->clear();
  m_NSWsTGC_dig_channelTypeMax->clear();
  m_NSWsTGC_dig_channelMin->clear();
  m_NSWsTGC_dig_channelMax->clear();
  m_NSWsTGC_dig_channelNumber->clear();
  m_NSWsTGC_dig_channelPosX->clear();
  m_NSWsTGC_dig_channelPosY->clear();
  m_NSWsTGC_dig_localPosX->clear();
  m_NSWsTGC_dig_localPosY->clear();
  m_NSWsTGC_dig_globalPosX->clear();
  m_NSWsTGC_dig_globalPosY->clear();
  m_NSWsTGC_dig_globalPosZ->clear();

  m_NSWsTGC_dig_truth_barcode->clear();
  m_NSWsTGC_dig_truth_localPosX->clear();
  m_NSWsTGC_dig_truth_localPosY->clear();
  m_NSWsTGC_dig_truth_globalPosX->clear();
  m_NSWsTGC_dig_truth_globalPosY->clear();
  m_NSWsTGC_dig_truth_globalPosZ->clear();
  m_NSWsTGC_dig_truth_XZ_angle->clear();

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode sTGCDigitVariables::initializeVariables()
{

  m_NSWsTGC_nDigits    				= 0;
  m_NSWsTGC_nPadDigits    				= 0;
  m_NSWsTGC_dig_time     			= new std::vector<double>();
  m_NSWsTGC_dig_bctag     			= new std::vector<int>();
  m_NSWsTGC_dig_charge   			= new std::vector<double>();
  m_NSWsTGC_dig_isDead   			= new std::vector<bool>();
  m_NSWsTGC_dig_isPileup 			= new std::vector<bool>();
  m_NSWsTGC_dig_stationName   = new std::vector<std::string>();
  m_NSWsTGC_dig_stationEta    = new std::vector<int>();
  m_NSWsTGC_dig_stationPhi    = new std::vector<int>();
  m_NSWsTGC_dig_multiplet     = new std::vector<int>();
  m_NSWsTGC_dig_gas_gap       = new std::vector<int>();
  m_NSWsTGC_dig_channel_type  = new std::vector<int>();
  m_NSWsTGC_dig_channel       = new std::vector<int>();

  m_NSWsTGC_dig_stationEtaMin = new std::vector<int>();
  m_NSWsTGC_dig_stationEtaMax = new std::vector<int>();
  m_NSWsTGC_dig_stationPhiMin = new std::vector<int>();
  m_NSWsTGC_dig_stationPhiMax = new std::vector<int>();
  m_NSWsTGC_dig_gas_gapMin    = new std::vector<int>();
  m_NSWsTGC_dig_gas_gapMax    = new std::vector<int>();
  m_NSWsTGC_dig_padEta        = new std::vector<int>();
  m_NSWsTGC_dig_padPhi        = new std::vector<int>();

  m_NSWsTGC_dig_numberOfMultilayers = new std::vector<int>();
  m_NSWsTGC_dig_multilayerMin       = new std::vector<int>();
  m_NSWsTGC_dig_multilayerMax       = new std::vector<int>();
  m_NSWsTGC_dig_channelTypeMin      = new std::vector<int>();
  m_NSWsTGC_dig_channelTypeMax      = new std::vector<int>();
  m_NSWsTGC_dig_channelMin          = new std::vector<int>();
  m_NSWsTGC_dig_channelMax          = new std::vector<int>();
  m_NSWsTGC_dig_channelNumber       = new std::vector<int>();

  m_NSWsTGC_dig_channelPosX = new std::vector<double>();
  m_NSWsTGC_dig_channelPosY = new std::vector<double>();
  m_NSWsTGC_dig_localPosX = new std::vector<double>();
  m_NSWsTGC_dig_localPosY = new std::vector<double>();
  m_NSWsTGC_dig_globalPosX = new std::vector<double>();
  m_NSWsTGC_dig_globalPosY = new std::vector<double>();
  m_NSWsTGC_dig_globalPosZ = new std::vector<double>();

  m_NSWsTGC_dig_truth_barcode    = new std::vector<int>();
  m_NSWsTGC_dig_truth_localPosX  = new std::vector<double>();
  m_NSWsTGC_dig_truth_localPosY  = new std::vector<double>();
  m_NSWsTGC_dig_truth_globalPosX = new std::vector<double>();
  m_NSWsTGC_dig_truth_globalPosY = new std::vector<double>();
  m_NSWsTGC_dig_truth_globalPosZ = new std::vector<double>();
  m_NSWsTGC_dig_truth_XZ_angle   = new std::vector<float>();

  if(m_tree) {
    m_tree->Branch("Digit_sTGC", 						 &m_NSWsTGC_nDigits, "Digits_sTGC_n/i");
    m_tree->Branch("Digit_sTGC_Pad_Digits", 						 &m_NSWsTGC_nPadDigits, "Digit_sTGC_Pad_Digits_n/i");
    m_tree->Branch("Digit_sTGC_time",       "std::vector< double >", &m_NSWsTGC_dig_time);
    m_tree->Branch("Digit_sTGC_bctag",       "std::vector< int >", &m_NSWsTGC_dig_bctag);
    m_tree->Branch("Digit_sTGC_charge",     "std::vector< double >", &m_NSWsTGC_dig_charge);
    m_tree->Branch("Digit_sTGC_isDead",     "std::vector< bool >",   &m_NSWsTGC_dig_isDead);
    m_tree->Branch("Digit_sTGC_isPileup",   "std::vector< bool >",  &m_NSWsTGC_dig_isPileup);
    m_tree->Branch("Digit_sTGC_stationName", &m_NSWsTGC_dig_stationName);
    m_tree->Branch("Digit_sTGC_stationEta",  &m_NSWsTGC_dig_stationEta);
    m_tree->Branch("Digit_sTGC_stationPhi",  &m_NSWsTGC_dig_stationPhi);
    m_tree->Branch("Digit_sTGC_multiplet",   &m_NSWsTGC_dig_multiplet);
    m_tree->Branch("Digit_sTGC_gas_gap",     &m_NSWsTGC_dig_gas_gap);
    m_tree->Branch("Digit_sTGC_channel_type",&m_NSWsTGC_dig_channel_type);
    m_tree->Branch("Digit_sTGC_channel",     &m_NSWsTGC_dig_channel);
    m_tree->Branch("Digit_sTGC_stationEtaMin",  &m_NSWsTGC_dig_stationEtaMin);
    m_tree->Branch("Digit_sTGC_stationEtaMax",  &m_NSWsTGC_dig_stationEtaMax);
    m_tree->Branch("Digit_sTGC_stationPhiMin",  &m_NSWsTGC_dig_stationPhiMin);
    m_tree->Branch("Digit_sTGC_stationPhiMax",  &m_NSWsTGC_dig_stationPhiMax);
    m_tree->Branch("Digit_sTGC_gas_gapMin",  &m_NSWsTGC_dig_gas_gapMin);
    m_tree->Branch("Digit_sTGC_gas_gapMax",  &m_NSWsTGC_dig_gas_gapMax);
    m_tree->Branch("Digit_sTGC_padEta",  &m_NSWsTGC_dig_padEta);
    m_tree->Branch("Digit_sTGC_padPhi",  &m_NSWsTGC_dig_padPhi);
    m_tree->Branch("Digit_sTGC_numberOfMultilayers",  &m_NSWsTGC_dig_numberOfMultilayers);
    m_tree->Branch("Digit_sTGC_multilayerMin",  &m_NSWsTGC_dig_multilayerMin);
    m_tree->Branch("Digit_sTGC_multilayerMax",  &m_NSWsTGC_dig_multilayerMax);
    m_tree->Branch("Digit_sTGC_channelTypeMin",  &m_NSWsTGC_dig_channelTypeMin);
    m_tree->Branch("Digit_sTGC_channelTypeMax",  &m_NSWsTGC_dig_channelTypeMax);
    m_tree->Branch("Digit_sTGC_channelMin",  &m_NSWsTGC_dig_channelMin);
    m_tree->Branch("Digit_sTGC_channelMax",  &m_NSWsTGC_dig_channelMax);
    m_tree->Branch("Digit_sTGC_channelNumber",  &m_NSWsTGC_dig_channelNumber);

    m_tree->Branch("Digits_sTGC_channelPosX", "std::vector< double >", &m_NSWsTGC_dig_channelPosX);
    m_tree->Branch("Digits_sTGC_channelPosY", "std::vector< double >", &m_NSWsTGC_dig_channelPosY);
    m_tree->Branch("Digits_sTGC_localPosX", "std::vector< double >", &m_NSWsTGC_dig_localPosX);
    m_tree->Branch("Digits_sTGC_localPosY", "std::vector< double >", &m_NSWsTGC_dig_localPosY);
    m_tree->Branch("Digits_sTGC_globalPosX", "std::vector< double >", &m_NSWsTGC_dig_globalPosX);
    m_tree->Branch("Digits_sTGC_globalPosY", "std::vector< double >", &m_NSWsTGC_dig_globalPosY);
    m_tree->Branch("Digits_sTGC_globalPosZ", "std::vector< double >", &m_NSWsTGC_dig_globalPosZ);

    m_tree->Branch("Digits_sTGC_truth_barcode",    "std::vector< int >",    &m_NSWsTGC_dig_truth_barcode);
    m_tree->Branch("Digits_sTGC_truth_localPosX",  "std::vector< double >", &m_NSWsTGC_dig_truth_localPosX);
    m_tree->Branch("Digits_sTGC_truth_localPosY",  "std::vector< double >", &m_NSWsTGC_dig_truth_localPosY);
    m_tree->Branch("Digits_sTGC_truth_globalPosX", "std::vector< double >", &m_NSWsTGC_dig_truth_globalPosX);
    m_tree->Branch("Digits_sTGC_truth_globalPosY", "std::vector< double >", &m_NSWsTGC_dig_truth_globalPosY);
    m_tree->Branch("Digits_sTGC_truth_globalPosZ", "std::vector< double >", &m_NSWsTGC_dig_truth_globalPosZ);
    m_tree->Branch("Digits_sTGC_truth_XZ_angle",   "std::vector< float >",  &m_NSWsTGC_dig_truth_XZ_angle);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called on finalize level of main alg */
StatusCode sTGCDigitVariables::deleteVariables()
{

  delete m_NSWsTGC_dig_stationName;
  delete m_NSWsTGC_dig_stationEta;
  delete m_NSWsTGC_dig_stationPhi;
  delete m_NSWsTGC_dig_multiplet;
  delete m_NSWsTGC_dig_gas_gap;
  delete m_NSWsTGC_dig_channel_type;
  delete m_NSWsTGC_dig_channel;
  delete m_NSWsTGC_dig_stationEtaMin;
  delete m_NSWsTGC_dig_stationEtaMax;
  delete m_NSWsTGC_dig_stationPhiMin;
  delete m_NSWsTGC_dig_stationPhiMax;
  delete m_NSWsTGC_dig_gas_gapMin;
  delete m_NSWsTGC_dig_gas_gapMax;
  delete m_NSWsTGC_dig_padEta;
  delete m_NSWsTGC_dig_padPhi;
  delete m_NSWsTGC_dig_numberOfMultilayers;
  delete m_NSWsTGC_dig_multilayerMin;
  delete m_NSWsTGC_dig_multilayerMax;
  delete m_NSWsTGC_dig_channelTypeMin;
  delete m_NSWsTGC_dig_channelTypeMax;
  delete m_NSWsTGC_dig_channelMin;
  delete m_NSWsTGC_dig_channelMax;
  delete m_NSWsTGC_dig_channelNumber;
  delete m_NSWsTGC_dig_channelPosX;
  delete m_NSWsTGC_dig_channelPosY;
  delete m_NSWsTGC_dig_localPosX;
  delete m_NSWsTGC_dig_localPosY;
  delete m_NSWsTGC_dig_globalPosX;
  delete m_NSWsTGC_dig_globalPosY;
  delete m_NSWsTGC_dig_globalPosZ;

  delete m_NSWsTGC_dig_truth_barcode;
  delete m_NSWsTGC_dig_truth_localPosX;
  delete m_NSWsTGC_dig_truth_localPosY;
  delete m_NSWsTGC_dig_truth_XZ_angle;
  delete m_NSWsTGC_dig_truth_globalPosX;
  delete m_NSWsTGC_dig_truth_globalPosY;
  delete m_NSWsTGC_dig_truth_globalPosZ;

  delete m_NSWsTGC_dig_time;
  delete m_NSWsTGC_dig_bctag;
  delete m_NSWsTGC_dig_charge;
  delete m_NSWsTGC_dig_isDead;
  delete m_NSWsTGC_dig_isPileup;


  m_NSWsTGC_nDigits      = 0;
  m_NSWsTGC_nPadDigits      = 0;
  m_NSWsTGC_dig_time     = nullptr;
  m_NSWsTGC_dig_bctag    = nullptr;
  m_NSWsTGC_dig_charge   = nullptr;
  m_NSWsTGC_dig_isDead   = nullptr;
  m_NSWsTGC_dig_isPileup = nullptr;
  m_NSWsTGC_dig_stationName   = nullptr;
  m_NSWsTGC_dig_stationEta    = nullptr;
  m_NSWsTGC_dig_stationPhi    = nullptr;
  m_NSWsTGC_dig_multiplet     = nullptr;
  m_NSWsTGC_dig_gas_gap       = nullptr;
  m_NSWsTGC_dig_channel_type  = nullptr;
  m_NSWsTGC_dig_channel       = nullptr;
  m_NSWsTGC_dig_stationEtaMin = nullptr;
  m_NSWsTGC_dig_stationEtaMax = nullptr;
  m_NSWsTGC_dig_stationPhiMin = nullptr;
  m_NSWsTGC_dig_stationPhiMax = nullptr;
  m_NSWsTGC_dig_gas_gapMin    = nullptr;
  m_NSWsTGC_dig_gas_gapMax    = nullptr;
  m_NSWsTGC_dig_padEta        = nullptr;
  m_NSWsTGC_dig_padPhi        = nullptr;
  m_NSWsTGC_dig_numberOfMultilayers = nullptr;
  m_NSWsTGC_dig_multilayerMin       = nullptr;
  m_NSWsTGC_dig_multilayerMax       = nullptr;
  m_NSWsTGC_dig_channelTypeMin      = nullptr;
  m_NSWsTGC_dig_channelTypeMax      = nullptr;
  m_NSWsTGC_dig_channelMin          = nullptr;
  m_NSWsTGC_dig_channelMax          = nullptr;
  m_NSWsTGC_dig_channelNumber       = nullptr;
  m_NSWsTGC_dig_channelPosX = nullptr;
  m_NSWsTGC_dig_channelPosY = nullptr;
  m_NSWsTGC_dig_localPosX   = nullptr;
  m_NSWsTGC_dig_localPosY   = nullptr;
  m_NSWsTGC_dig_globalPosX  = nullptr;
  m_NSWsTGC_dig_globalPosY  = nullptr;
  m_NSWsTGC_dig_globalPosZ  = nullptr;

  m_NSWsTGC_dig_truth_barcode    = nullptr;
  m_NSWsTGC_dig_truth_localPosX  = nullptr;
  m_NSWsTGC_dig_truth_localPosY  = nullptr;
  m_NSWsTGC_dig_truth_globalPosX = nullptr;
  m_NSWsTGC_dig_truth_globalPosY = nullptr;
  m_NSWsTGC_dig_truth_globalPosZ = nullptr;
  m_NSWsTGC_dig_truth_XZ_angle   = nullptr;

  return StatusCode::SUCCESS;
}
