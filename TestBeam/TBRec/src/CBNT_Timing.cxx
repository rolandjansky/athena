/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CBNT_Timing.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "LArIdentifier/LArOnlineID.h"
#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBPhase.h"
#include "AthenaKernel/Units.h"
#include <vector>


using Athena::Units::GeV;
using Athena::Units::ns;


CBNT_Timing::CBNT_Timing(const std::string & name, ISvcLocator * pSvcLocator)
  :CBNT_TBRecBase(name, pSvcLocator)
   , m_onlineHelper(0)
   , m_emId(0)
   , m_energy_cut(2.*GeV)
   , m_first_event(true)
   , m_caloCellName("AllCalo")
{
  m_sampling_names.resize(0);

  declareProperty("TBPhase",m_tbphase="TBPhase");
  declareProperty("CellContainerName",     m_caloCellName);
  declareProperty("EnergyCut", m_energy_cut);
  declareProperty("IncludedSamplings", m_sampling_names);

  m_energy = 0;
  m_tdc_phase = 0;
  m_time = 0;

  m_energy_cell = 0;
  m_eta_cell = 0;
  m_febId_cell = 0;
  m_febId_feb = 0;
  m_feedthrough_feb = 0;
  m_layer_cell = 0;
  m_layer_layer = 0;
  m_phi_cell = 0;
  m_slot_feb = 0;
  m_time_cell = 0;
  m_time_feb = 0;
  m_time_layer = 0;
}

CBNT_Timing::~CBNT_Timing() 
{//empty
}

StatusCode CBNT_Timing::CBNT_initialize() {

  ATH_MSG_DEBUG  ( "CBNT_Timing in initialize()" );

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

    addBranch ("TDC_TimeCell",m_time_cell);
    addBranch ("TDC_EnergyCell",m_energy_cell);
    addBranch ("TDC_LayerCell",m_layer_cell);
    addBranch ("TDC_EtaCell", m_eta_cell);
    addBranch ("TDC_PhiCell", m_phi_cell);
    addBranch ("TDC_FebIDCell", m_febId_cell);
    addBranch ("TDC_TimeLayer",m_time_layer);
    addBranch ("TDC_LayerIDLayer",m_layer_layer);
    addBranch ("TDC_TimeFeb",m_time_feb);
    addBranch ("TDC_FebIdFeb",m_febId_feb);
    addBranch ("TDC_SlotFeb", m_slot_feb);
    addBranch ("TDC_FeedthroughFeb", m_feedthrough_feb);
    addBranch ("TDC_Phase",m_tdc_phase, "Phase/F");
    addBranch ("TDC_TimeTotal",m_time, "TimeTotal/F");
    addBranch ("TDC_EnergyTotal",m_energy, "EnergyTotal/F");



  
  // setup calorimeter module and sampling lookup tables
  ATH_CHECK( this->setupLookupTables() );

  // get calorimeter samplings ids for the requested samplings
  msg() << MSG::INFO << "Included calorimeter samplings: ";
  for (std::vector<std::string>::const_iterator sampling = m_sampling_names.begin(); sampling != m_sampling_names.end(); sampling++) {
    //    CaloSampling::CaloSample idSamp = CaloSamplingHelper::getSamplingId(*sampling);
    CaloSampling::CaloSample idSamp = m_samplingFromNameLookup[*sampling];
    if (idSamp != CaloSampling::Unknown) {
      m_samplingIndices.push_back(idSamp);
      msg() << MSG::INFO << "\042" << *sampling
             << "\042 ";
    }
  }
  msg() << MSG::INFO << endmsg;

  // get an idCalo keyed map of vectors of idSample for the requested samplings
  for (std::vector<CaloSampling::CaloSample>::iterator sample = m_samplingIndices.begin(); sample != m_samplingIndices.end(); sample++) {
    CaloSampling::CaloSample idSample = *sample;
    // find the idCalo 
    CaloCell_ID::SUBCALO idCalo = m_caloLookup[idSample];
    // build the vector of idSample
    std::vector<CaloSampling::CaloSample> samplingV = m_calosAndSamplings[idCalo];
    samplingV.push_back(idSample);
    m_calosAndSamplings[idCalo] = samplingV;
  }
  // printout
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
  for (; it != m_calosAndSamplings.end(); it++) {
    CaloCell_ID::SUBCALO idCalo = it->first;
    msg() << MSG::INFO
           << "Included calorimeter : \042"
           << m_caloToNameLookup[idCalo]
           << "\042 samplings:";
    std::vector<CaloSampling::CaloSample> samplingV = it->second;
    std::vector<CaloSampling::CaloSample>::iterator sample     = samplingV.begin();
    std::vector<CaloSampling::CaloSample>::iterator lastSample = samplingV.end();
    for (; sample != lastSample; sample++) {
      msg() << MSG::INFO
             << " \042"
             << m_samplingToNameLookup[*sample]
             << "\042";
    }
    msg()  << MSG::INFO << endmsg;
  }

  return StatusCode::SUCCESS; 
  
}


StatusCode CBNT_Timing::CBNT_execute()
{
  ATH_MSG_DEBUG ( "in execute()" );
  
  /////////////////
  // Data Access //
  /////////////////
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // CaloCells
  const CaloCellContainer* cellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(cellContainer, m_caloCellName ) );

  // TBPhase 
  TBPhase * phase = nullptr;
  ATH_CHECK( evtStore()->retrieve(phase,m_tbphase) );
  
  m_tdc_phase = phase->getPhase();

  ////////////////////////
  // First Event Action //
  ////////////////////////

  if (m_first_event) {

    // find all febID's related to the requested samplings
    // loop over desired calorimeter modules first, more efficient this way
    std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
    for (; it != m_calosAndSamplings.end(); it++) {
      CaloCell_ID::SUBCALO idCalo = it->first;
      std::vector<CaloSampling::CaloSample> samplingV = it->second;

      // loop over the corresponding CaloCell's
      for (CaloCellContainer::const_iterator cell = cellContainer->beginConstCalo(idCalo);
           cell != cellContainer->endConstCalo(idCalo); cell++) {

        // get the corresponding sample
        CaloSampling::CaloSample idSample;
        const CaloDetDescrElement * theCaloDDE= (*cell)->caloDDE();
        if (theCaloDDE) {
              idSample = (CaloSampling::CaloSample) theCaloDDE->getSampling();
        }else{
              idSample =  CaloSampling::Unknown;
        } 

        // only consider CaloCell's in the requested sampling
        if (find(samplingV.begin(), samplingV.end(), idSample) != samplingV.end()) {
          
          // here you have the CaloCell with idCalo, idSample, **cell
          // find the hardware ID and the corresponding febID

          HWIdentifier id = cabling->createSignalChannelID((*cell)->ID());
          HWIdentifier febID = m_onlineHelper->feb_Id(id);

          // store it if you don't have it already
          if (find(m_febIDs.begin(), m_febIDs.end(), febID) == m_febIDs.end()) m_febIDs.push_back(febID);

        }
      }
    }
    // print out
    msg() << MSG::INFO << "FEB IDs: ";
    for (std::vector<HWIdentifier>::iterator it_febID = m_febIDs.begin(); it_febID != m_febIDs.end(); it_febID++) {
      std::ostringstream os; 
      os << std::hex << *it_febID;
      msg() << MSG::INFO << " \042" << os.str() << "\042";
    }
    msg() << MSG::INFO << endmsg;

    m_first_event = false;
  }


  ///////////////////////////////////
  // sampling timing distributions //
  ///////////////////////////////////

  // energy weighted time stores
  std::map<CaloSampling::CaloSample, double> sumEPerSampling, sumETimePerSampling;  // sampling id keyed
  std::map<HWIdentifier, double>             sumEPerFeb,      sumETimePerFeb;       // febID keyed
  double sumETotal = 0, sumETimeTotal = 0;
  bool eSet = false;

  // fill energy weighted time stores
  // loop over desired calorimeter modules first, more efficient this way
  std::map< CaloCell_ID::SUBCALO, std::vector<CaloSampling::CaloSample> >::iterator it = m_calosAndSamplings.begin();
  m_energy = 0;

  for (; it != m_calosAndSamplings.end(); it++) {
    CaloCell_ID::SUBCALO idCalo = it->first;
    ATH_MSG_DEBUG
      ( "Looping over CaloCells of calorimeter : \042"
        << m_caloToNameLookup[idCalo]
        << "\042" );
    std::vector<CaloSampling::CaloSample> samplingV = it->second;

    // loop over the corresponding CaloCell's
    for (CaloCellContainer::const_iterator cell = cellContainer->beginConstCalo(idCalo);
         cell != cellContainer->endConstCalo(idCalo); cell++) {

      // get the corresponding sample
      //CaloSampling::CaloSample idSample = CaloSampling::getSampling(**cell);
      CaloSampling::CaloSample idSample;
      const CaloDetDescrElement * theCaloDDE= (*cell)->caloDDE();
      if (theCaloDDE) {
            idSample = (CaloSampling::CaloSample) theCaloDDE->getSampling();
      }else{
            idSample =  CaloSampling::Unknown;
      } 

      // only consider CaloCell's in the requested sampling
      if (find(samplingV.begin(), samplingV.end(), idSample) != samplingV.end()) {

        // here you have the CaloCell with idCalo, idSample, **cell
        // find the hardware ID and the corresponding febID

        HWIdentifier id = cabling->createSignalChannelID((*cell)->ID());
        HWIdentifier febID = m_onlineHelper->feb_Id(id);
       
        // gather sums for energy weighted cubic peaking time
        // select cells for which the cubic interpolation was successfully applied
        // *** for now, this is done by requiring that time(ns) is not too close to 25ns * n
        // *** but it should be done using the CaloCell quality, when available...
        // select cells above an energy threshold

        double energy = (*cell)->e();
        double time = (*cell)->time();
        if (fabs(time/ns - float(int(time/(25*ns))*25.)) > 0.001 && energy > m_energy_cut) {
	  
	  m_time_cell->push_back(time/ns);
	  m_energy_cell->push_back(energy/GeV);	  
	  m_energy += energy/GeV;

          sumEPerSampling[idSample] += energy;
          sumETimePerSampling[idSample] += energy * time;

          sumEPerFeb[febID] += energy; 
          sumETimePerFeb[febID] += energy * time;

	  sumETotal += energy;
	  sumETimeTotal += energy * time;
	  eSet = true;

	  ATH_MSG_DEBUG
            ( "cell time = " << time/ns << " ns"
              << "; energy = " << energy/GeV << " GeV" );

	} else {
	  // below energy cut
	  m_time_cell->push_back(NOTIME);
	  m_energy_cell->push_back(NOENERGY);
	}
	
	m_febId_cell->push_back(febID.get_identifier32().get_compact());
	//try {
	//const Identifier ident = cabling->cnvToIdentifier(id);
	if ( m_emId->is_lar_em((*cell)->ID()) ) { 
	  m_eta_cell->push_back(m_emId->eta((*cell)->ID()));
	  m_phi_cell->push_back(m_emId->phi((*cell)->ID()));
	  m_layer_cell->push_back(m_emId->sampling((*cell)->ID()));
	} else {
	  m_eta_cell->push_back(0);
	  m_phi_cell->push_back(0);
	  m_layer_cell->push_back(0);
	}
	//}
	/*catch (LArID_Exception & except) {
	  m_eta_cell->push_back(-999);
	  m_phi_cell->push_back(-999);
	  m_layer_cell->push_back(-999);
	  }*/

      }
    }
  }

  // fill energy weighted cubic peaking time for each requested sampling
  for (std::vector<CaloSampling::CaloSample>::iterator sample = m_samplingIndices.begin(); sample != m_samplingIndices.end(); sample++) {
    CaloSampling::CaloSample idSample = *sample;
    // go on only if there is data stored for this sample:
    //if (sumEPerSampling.find(idSample) != sumEPerSampling.end()) {
      double peakTime = (sumEPerSampling[idSample] > 0.) ? sumETimePerSampling[idSample]/sumEPerSampling[idSample] : NOTIME;
      m_time_layer->push_back(peakTime/ns);
      m_layer_layer->push_back(idSample);
      //}
  }

  // fill energy weighted cubic peaking time for each requested FEB
  for (std::vector<HWIdentifier>::iterator it_febID = m_febIDs.begin(); it_febID != m_febIDs.end(); it_febID++) {
      HWIdentifier febID = *it_febID;
    // go on only if there is data stored for this febID
    //if (sumEPerFeb.find(febID) != sumEPerFeb.end()) {
      double peakTime = (sumEPerFeb[febID] > 0.) ? sumETimePerFeb[febID]/sumEPerFeb[febID] : NOTIME;
      m_time_feb->push_back(peakTime/ns);
      m_febId_feb->push_back(febID.get_identifier32().get_compact());
      m_slot_feb->push_back(m_onlineHelper->slot(febID));
      m_feedthrough_feb->push_back(m_onlineHelper->feedthrough(febID));
      //}
  }

  // fill energy weighted cubic peaking time for everything
  if (eSet) {
    m_time = (sumETotal > 0.) ? sumETimeTotal/sumETotal : 0.;
  }

  //if (m_Ncells > 0) std::cout << "#cells: " << m_Ncells << std::endl;
  return StatusCode::SUCCESS;
}





StatusCode CBNT_Timing::CBNT_finalize()
{
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}


StatusCode CBNT_Timing::setupLookupTables() {

  // fill slot and febName lookup table for EM barrel online
  m_slotToFebNameLookup[1]="Presampler";
  m_slotToFebNameLookup[2]="Front 0";
  m_slotToFebNameLookup[3]="Front 1";
  m_slotToFebNameLookup[4]="Front 2";
  m_slotToFebNameLookup[5]="Front 3";
  m_slotToFebNameLookup[6]="Front 4";
  m_slotToFebNameLookup[7]="Front 5";
  m_slotToFebNameLookup[8]="Front 6";
  m_slotToFebNameLookup[9]="Back 0";
  m_slotToFebNameLookup[10]="Back 1";
  m_slotToFebNameLookup[11]="Middle 0";
  m_slotToFebNameLookup[12]="Middle 1";
  m_slotToFebNameLookup[13]="Middle 2";
  m_slotToFebNameLookup[14]="Middle 3";

  // fill calo names lookup tables
  m_caloToNameLookup[CaloCell_ID::LAREM]   = "LAREM";
  m_caloToNameLookup[CaloCell_ID::LARHEC]  = "LARHEC";
  m_caloToNameLookup[CaloCell_ID::LARFCAL] = "LARFCAL";
  m_caloToNameLookup[CaloCell_ID::TILE]    = "TILE";

  m_caloFromNameLookup["LAREM"]   = CaloCell_ID::LAREM;
  m_caloFromNameLookup["LARHEC"]  = CaloCell_ID::LARHEC;
  m_caloFromNameLookup["LARFCAL"] = CaloCell_ID::LARFCAL;
  m_caloFromNameLookup["TILE"]    = CaloCell_ID::TILE;

  // fill sampling names lookup table
  m_samplingToNameLookup[CaloSampling::PreSamplerB] = "PreSamplerB";  // electromagnetic barrel
  m_samplingToNameLookup[CaloSampling::EMB1]        = "EMB1";        
  m_samplingToNameLookup[CaloSampling::EMB2]        = "EMB2";        
  m_samplingToNameLookup[CaloSampling::EMB3]        = "EMB3";        
  m_samplingToNameLookup[CaloSampling::PreSamplerE] = "PreSamplerE";  // electromagnetic endcap
  m_samplingToNameLookup[CaloSampling::EME1]        = "EME1";        
  m_samplingToNameLookup[CaloSampling::EME2]        = "EME2";        
  m_samplingToNameLookup[CaloSampling::EME3]        = "EME3";        
  m_samplingToNameLookup[CaloSampling::HEC0]        = "HEC0";         // hadronic endcap
  m_samplingToNameLookup[CaloSampling::HEC1]        = "HEC1";        
  m_samplingToNameLookup[CaloSampling::HEC2]        = "HEC2";        
  m_samplingToNameLookup[CaloSampling::HEC3]        = "HEC3";        
  m_samplingToNameLookup[CaloSampling::TileBar0]    = "TileBar0";     // tile barrel
  m_samplingToNameLookup[CaloSampling::TileBar1]    = "TileBar1";    
  m_samplingToNameLookup[CaloSampling::TileBar2]    = "TileBar2";    
  m_samplingToNameLookup[CaloSampling::TileGap1]    = "TileGap1";     // tile gap scintillators
  m_samplingToNameLookup[CaloSampling::TileGap2]    = "TileGap2";    
  m_samplingToNameLookup[CaloSampling::TileGap3]    = "TileGap3";    
  m_samplingToNameLookup[CaloSampling::TileExt0]    = "TileExt0";     // tile extended barrel
  m_samplingToNameLookup[CaloSampling::TileExt1]    = "TileExt1";    
  m_samplingToNameLookup[CaloSampling::TileExt2]    = "TileExt2";    
  m_samplingToNameLookup[CaloSampling::FCAL0]       = "FCal1";        // fcal
  m_samplingToNameLookup[CaloSampling::FCAL1]       = "FCal2";       
  m_samplingToNameLookup[CaloSampling::FCAL2]       = "FCal3";       
  m_samplingToNameLookup[CaloSampling::Unknown]     = "unknown";

  m_samplingFromNameLookup["PreSamplerB"] = CaloSampling::PreSamplerB; // electromagnetic barrel
  m_samplingFromNameLookup["EMB1"]        = CaloSampling::EMB1;       
  m_samplingFromNameLookup["EMB2"]        = CaloSampling::EMB2;       
  m_samplingFromNameLookup["EMB3"]        = CaloSampling::EMB3;       
  m_samplingFromNameLookup["PreSamplerE"] = CaloSampling::PreSamplerE; // electromagnetic endcap
  m_samplingFromNameLookup["EME1"]        = CaloSampling::EME1;       
  m_samplingFromNameLookup["EME2"]        = CaloSampling::EME2;       
  m_samplingFromNameLookup["EME3"]        = CaloSampling::EME3;       
  m_samplingFromNameLookup["HEC0"]        = CaloSampling::HEC0;        // hadronic endcap
  m_samplingFromNameLookup["HEC1"]        = CaloSampling::HEC1;       
  m_samplingFromNameLookup["HEC2"]        = CaloSampling::HEC2;       
  m_samplingFromNameLookup["HEC3"]        = CaloSampling::HEC3;       
  m_samplingFromNameLookup["TileBar0"]    = CaloSampling::TileBar0;    // tile barrel
  m_samplingFromNameLookup["TileBar1"]    = CaloSampling::TileBar1;   
  m_samplingFromNameLookup["TileBar2"]    = CaloSampling::TileBar2;   
  m_samplingFromNameLookup["TileGap1"]    = CaloSampling::TileGap1;    // tile gap scintillators
  m_samplingFromNameLookup["TileGap2"]    = CaloSampling::TileGap2;   
  m_samplingFromNameLookup["TileGap3"]    = CaloSampling::TileGap3;   
  m_samplingFromNameLookup["TileExt0"]    = CaloSampling::TileExt0;    // tile extended barrel
  m_samplingFromNameLookup["TileExt1"]    = CaloSampling::TileExt1;   
  m_samplingFromNameLookup["TileExt2"]    = CaloSampling::TileExt2;   
  m_samplingFromNameLookup["FCal1"]       = CaloSampling::FCAL0;       // fcal
  m_samplingFromNameLookup["FCal2"]       = CaloSampling::FCAL1;      
  m_samplingFromNameLookup["FCal3"]       = CaloSampling::FCAL2;      
  m_samplingFromNameLookup["unknown"]     = CaloSampling::Unknown;     

  // fill calo lookup table
  m_caloLookup[CaloSampling::PreSamplerB] = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB1]        = CaloCell_ID::LAREM;
  m_caloLookup[CaloSampling::EMB2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EMB3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::PreSamplerE] = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME1]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME2]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::EME3]        = CaloCell_ID::LAREM; 
  m_caloLookup[CaloSampling::HEC0]        = CaloCell_ID::LARHEC; 
  m_caloLookup[CaloSampling::HEC1]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC2]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::HEC3]        = CaloCell_ID::LARHEC;  
  m_caloLookup[CaloSampling::TileBar0]    = CaloCell_ID::TILE;  
  m_caloLookup[CaloSampling::TileBar1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileBar2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap1]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap2]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileGap3]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt0]    = CaloCell_ID::TILE;   
  m_caloLookup[CaloSampling::TileExt1]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::TileExt2]    = CaloCell_ID::TILE;    
  m_caloLookup[CaloSampling::FCAL0]       = CaloCell_ID::LARFCAL; 
  m_caloLookup[CaloSampling::FCAL1]       = CaloCell_ID::LARFCAL;  
  m_caloLookup[CaloSampling::FCAL2]       = CaloCell_ID::LARFCAL;  

  return StatusCode::SUCCESS;
}

StatusCode CBNT_Timing::CBNT_clear() {
   
   if(m_layer_cell) m_layer_cell->clear();
   if(m_eta_cell) m_eta_cell->clear();
   if(m_phi_cell) m_phi_cell->clear();
   if(m_febId_cell) m_febId_cell->clear();
   if(m_energy_cell) m_energy_cell->clear();
   if(m_time_cell) m_time_cell->clear();
   if(m_time_layer) m_time_layer->clear();
   if(m_layer_layer) m_layer_layer->clear();
   if(m_febId_feb) m_febId_feb->clear();
   if(m_time_feb) m_time_feb->clear();
   if(m_slot_feb) m_slot_feb->clear();
   if(m_feedthrough_feb) m_feedthrough_feb->clear();   
   return StatusCode::SUCCESS;
}
