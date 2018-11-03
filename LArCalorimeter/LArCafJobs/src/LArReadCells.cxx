/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadCells.h"

#include "xAODEventInfo/EventInfo.h"

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArElecCalib/ILArPedestal.h"



LArReadCells::LArReadCells( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  m_etcut=-10000.;
  declareProperty("etCut",m_etcut);

}


LArReadCells::~LArReadCells() {}


StatusCode LArReadCells::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  m_tree = new TTree("myTree","myTree");
  CHECK( histSvc->regTree("/SPLASH/myTree",m_tree) );
  m_tree->Branch("RunNumber",&m_runNumber,"RunNumber/I");
  m_tree->Branch("LBNumber",&m_lbNumber,"LBNumber/I");
  m_tree->Branch("EventNumber",&m_eventNumber,"EventNumber/I");
  m_tree->Branch("BCID",&m_bcid,"BCID/I");
  m_tree->Branch("LArError",&m_error,"LArError/I");
  m_tree->Branch("ncells",&m_ncells,"ncells/I");
  m_tree->Branch("ECell",m_ECell,"eCell[ncells]/F");
  m_tree->Branch("TCell",m_TCell,"tCell[ncells]/F");
  m_tree->Branch("EtaCell",m_EtaCell,"etaCell[ncells]/F");
  m_tree->Branch("PhiCell",m_PhiCell,"phiCell[ncells]/F");
  m_tree->Branch("LayerCell",m_LayerCell,"layerCell[ncells]/I");
  m_tree->Branch("ProvCell", m_ProvCell,"provCell[ncells]/I");
  m_tree->Branch("QuaCell", m_QuaCell,"quaCell[ncells]/I");
  m_tree->Branch("GainCell", m_GainCell,"gainCell[ncells]/I");
  m_tree->Branch("HwidCell", m_HwidCell,"hwidCell[ncells]/I");
  m_tree->Branch("ADC",m_ADC,"ADC[ncells][32]/I");

  ATH_CHECK(detStore()->retrieve(m_caloIdMgr));
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();
  ATH_CHECK(m_larCablingSvc.retrieve());

  ATH_CHECK(detStore()->retrieve(m_lar_online_id, "LArOnlineID"));



  return StatusCode::SUCCESS;
}

StatusCode LArReadCells::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode LArReadCells::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  std::cout << " Et cut is " << m_etcut << std::endl;

  const ILArPedestal* larPedestal = nullptr;
  if( detStore()->retrieve(larPedestal).isFailure() ){
   std::cout << " cannot retrieve pedestal " << std::endl;
  }

  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve( eventInfo) );
  std::cout << " run number " << eventInfo->runNumber() << std::endl;
  
  m_runNumber   = eventInfo->runNumber();
  m_eventNumber = eventInfo->eventNumber();
  m_lbNumber    = eventInfo->lumiBlock();
  m_bcid        = eventInfo->bcid();
  m_error       = 0;
  if (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) m_error=1;

  std::vector<LArDigit*> m_IndexDigit;
  int nCell = m_calo_id->calo_cell_hash_max();
  m_IndexDigit.resize(nCell,NULL);

  const LArDigitContainer* digit_container;
  if (evtStore()->contains<LArDigitContainer>("FREE")) {
  std::cout << " retrieving digit container " << std::endl;
  CHECK(evtStore()->retrieve(digit_container,"FREE"));
  LArDigitContainer::const_iterator first_digit = digit_container->begin();
  LArDigitContainer::const_iterator end_digit   = digit_container->end();
  for (; first_digit != end_digit; ++first_digit)
  {
    HWIdentifier hwid = (*first_digit)->hardwareID();
    Identifier   id = m_larCablingSvc->cnvToIdentifier(hwid);
    int index = (int) (m_calo_id->calo_cell_hash(id));
    if (index>=0 && index<nCell) m_IndexDigit[index]=(*first_digit);
  }
  }


 std::cout << " retrieving cell container " << std::endl;
 const CaloCellContainer* cell_container;
 CHECK( evtStore()->retrieve(cell_container,"AllCalo"));

 CaloCellContainer::const_iterator first_cell = cell_container->begin();
 CaloCellContainer::const_iterator end_cell   = cell_container->end();
 m_ncells=0;
 for (; first_cell != end_cell; ++first_cell)
 {
     Identifier cellID = (*first_cell)->ID();
     double et    =  (*first_cell)->et();

     if (et > m_etcut && !(m_calo_id->is_tile(cellID)) ){

  // select only layer 1 cells 2.2-2.4 in eta
 //       if (m_calo_id->calo_sample(cellID)!=4) continue;
 //       if ((*first_cell)->eta()<2.2) continue;
 //       if ((*first_cell)->eta()>2.4) continue;


        m_ECell[m_ncells]= (*first_cell)->energy();
        m_TCell[m_ncells]= (*first_cell)->time();
        m_EtaCell[m_ncells]= (*first_cell)->eta();
        m_PhiCell[m_ncells]= (*first_cell)->phi();
        m_LayerCell[m_ncells]= m_calo_id->calo_sample(cellID);
        m_ProvCell[m_ncells]=(*first_cell)->provenance();
        m_QuaCell[m_ncells]=(*first_cell)->quality();
        m_GainCell[m_ncells]=(*first_cell)->gain();

        HWIdentifier hwid =  m_larCablingSvc->createSignalChannelID(cellID);
        int barrel_ec = m_lar_online_id->barrel_ec(hwid);
        int pos_neg   = m_lar_online_id->pos_neg(hwid);
        int FT        = m_lar_online_id->feedthrough(hwid);
        int slot      = m_lar_online_id->slot(hwid);
        int channel   = m_lar_online_id->channel(hwid);
        int myid=0;
        if (barrel_ec<2 && pos_neg<2 && FT<32 && slot<16 && channel<128) 
         myid = (channel) | (slot << 7) | (FT<<11) | (pos_neg << 16) | (barrel_ec << 17);

        float pedestal=0.;
        if (larPedestal) {
         pedestal =  larPedestal->pedestal(cellID,(*first_cell)->gain());
        }
   
        m_HwidCell[m_ncells]=myid;

        int index = (int) (m_calo_id->calo_cell_hash(cellID));
        //std::cout << " index " << index << std::endl;
        if (m_IndexDigit[index]) {
            const std::vector<short>& vSamples=(m_IndexDigit[index])->samples();
            int nsamples = vSamples.size();
            for (int i=0;i<std::min(32,nsamples);i++) {
              m_ADC[m_ncells][i]=vSamples[i]-pedestal;
            }
        }
        m_ncells++;

     }

    if (m_ncells>=250000) {
       std::cout << " --- too many cells " << m_ncells << std::endl;
       break;
    }
 }

  std::cout << "  Number of cells read " << m_ncells << std::endl;

  m_tree->Fill();

  return StatusCode::SUCCESS;
}


