/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/LArReadCells.h"

#include "xAODEventInfo/EventInfo.h"

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloUtils/CaloCellList.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArElecCalib/ILArPedestal.h"



LArReadCells::LArReadCells( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){
   m_ECell.reserve(250000);
   m_TCell.reserve(250000);
   m_EtaCell.reserve(250000);
   m_PhiCell.reserve(250000);
   m_LayerCell.reserve(250000);
   m_ProvCell.reserve(250000);
   m_QuaCell.reserve(250000);
   m_GainCell.reserve(250000);
   m_ChidCell.reserve(250000);
   m_HwidCell.reserve(250000);
   m_ADC.reserve(250000);
}


LArReadCells::~LArReadCells() {}


StatusCode LArReadCells::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  CHECK( histSvc.retrieve() );
  m_tree = new TTree("myTree","myTree");
  std::string out("/"+m_outStream+"/myTree");
  CHECK( histSvc->regTree(out.c_str(),m_tree) );
  ATH_MSG_INFO("Registered tree: " << out);
  m_tree->Branch("RunNumber",&m_runNumber,"RunNumber/I");
  m_tree->Branch("LBNumber",&m_lbNumber,"LBNumber/I");
  m_tree->Branch("EventNumber",&m_eventNumber,"EventNumber/I");
  m_tree->Branch("BCID",&m_bcid,"BCID/I");
  m_tree->Branch("LArError",&m_error,"LArError/I");
  m_tree->Branch("ncells",&m_ncells,"ncells/I");
  m_tree->Branch("ECell",m_ECell.data(),"eCell[ncells]/F");
  m_tree->Branch("TCell",m_TCell.data(),"tCell[ncells]/F");
  m_tree->Branch("EtaCell",m_EtaCell.data(),"etaCell[ncells]/F");
  m_tree->Branch("PhiCell",m_PhiCell.data(),"phiCell[ncells]/F");
  m_tree->Branch("LayerCell",m_LayerCell.data(),"layerCell[ncells]/I");
  m_tree->Branch("ProvCell", m_ProvCell.data(),"provCell[ncells]/I");
  m_tree->Branch("QuaCell", m_QuaCell.data(),"quaCell[ncells]/I");
  m_tree->Branch("GainCell", m_GainCell.data(),"gainCell[ncells]/I");
  m_tree->Branch("ChidCell", m_ChidCell.data(),"chidCell[ncells]/I");
  m_tree->Branch("HwidCell", m_HwidCell.data(),"hwidCell[ncells]/I");
  m_tree->Branch("ADC",m_ADC.data(),"ADC[ncells][32]/F");

  ATH_CHECK(detStore()->retrieve(m_caloIdMgr));
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();
  ATH_CHECK( detStore()->retrieve(m_lar_online_id, "LArOnlineID") );

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK(m_pedestalKey.initialize());
  ATH_CHECK(m_caloMgrKey.initialize());
  ATH_MSG_INFO("Energy cut for time and quality computation: " << m_etcut);

  return StatusCode::SUCCESS;
}

StatusCode LArReadCells::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode LArReadCells::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling=*cablingHdl;
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey}; 
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

  //Get Conditions input
  SG::ReadCondHandle<ILArPedestal> pedHdl{m_pedestalKey};
  const ILArPedestal* larPedestal=*pedHdl;


  const xAOD::EventInfo* eventInfo = 0;
  ATH_CHECK( evtStore()->retrieve( eventInfo) );
  ATH_MSG_INFO( " run number " << eventInfo->runNumber() );
  
  m_runNumber   = eventInfo->runNumber();
  m_eventNumber = eventInfo->eventNumber();
  m_lbNumber    = eventInfo->lumiBlock();
  m_bcid        = eventInfo->bcid();
  m_error       = 0;
  if (eventInfo->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) m_error=1;

  std::vector<const LArDigit*> IndexDigit;
  int nCell = m_calo_id->calo_cell_hash_max();
  IndexDigit.resize(nCell,NULL);

  const LArDigitContainer* digit_container;
  if (evtStore()->contains<LArDigitContainer>("FREE")) {
  CHECK(evtStore()->retrieve(digit_container,"FREE"));
  LArDigitContainer::const_iterator first_digit = digit_container->begin();
  LArDigitContainer::const_iterator end_digit   = digit_container->end();
  for (; first_digit != end_digit; ++first_digit)
  {
    HWIdentifier hwid = (*first_digit)->hardwareID();
    Identifier   id = cabling->cnvToIdentifier(hwid);
    int index = (int) (m_calo_id->calo_cell_hash(id));
    if (index>=0 && index<nCell) IndexDigit[index]=(*first_digit);
  }
  }


 const CaloCellContainer* cell_container;
 CHECK( evtStore()->retrieve(cell_container,"AllCalo"));

 std::vector<int> iflag_cell;
 iflag_cell.resize(nCell,0);

 CaloCellContainer::const_iterator first_cell = cell_container->begin();
 CaloCellContainer::const_iterator end_cell   = cell_container->end();
 m_ncells=0;
 for (; first_cell != end_cell; ++first_cell)
 {
     Identifier cellID = (*first_cell)->ID();
     int index = (int) (m_calo_id->calo_cell_hash(cellID));
     double et    =  (*first_cell)->et();

     if (et > m_etcut && !(m_calo_id->is_tile(cellID)) ){
        iflag_cell[index]=1;

        if ((m_calo_id->calo_sample(cellID)==CaloSampling::CaloSample::EMB2 || m_calo_id->calo_sample(cellID)==CaloSampling::CaloSample::EME2) 
              && et>m_etcut2) {

           CaloCellList myList(caloDDMgr, cell_container);
           myList.select((*first_cell)->eta(),(*first_cell)->phi(),0.10);
           for (const CaloCell* cell : myList) {
             Identifier cellID2 =cell->ID();
             int index2 = (int)(m_calo_id->calo_cell_hash(cellID2));
             iflag_cell[index2]=1;
           }
       }// second layer
     }// above first etcut
 }// over cells

 first_cell = cell_container->begin();
 for  (; first_cell != end_cell; ++first_cell) {

     Identifier cellID = (*first_cell)->ID();
     int index = (int) (m_calo_id->calo_cell_hash(cellID));

     if (iflag_cell[index]==1 ){
        m_ECell[m_ncells]= (*first_cell)->energy();
        m_TCell[m_ncells]= (*first_cell)->time();
        m_EtaCell[m_ncells]= (*first_cell)->eta();
        m_PhiCell[m_ncells]= (*first_cell)->phi();
        m_LayerCell[m_ncells]= m_calo_id->calo_sample(cellID);
        m_ProvCell[m_ncells]=(*first_cell)->provenance();
        m_QuaCell[m_ncells]=(*first_cell)->quality();
        m_GainCell[m_ncells]=(*first_cell)->gain();

        for (int i=0;i<32;i++)  m_ADC[m_ncells][i]=0.;
        float pedestal=0.;
        int myid=0;
        if (!(m_calo_id->is_tile(cellID))) {
           HWIdentifier hwid =  cabling->createSignalChannelID(cellID);
           int barrel_ec = m_lar_online_id->barrel_ec(hwid);
           int pos_neg   = m_lar_online_id->pos_neg(hwid);
           int FT        = m_lar_online_id->feedthrough(hwid);
           int slot      = m_lar_online_id->slot(hwid);
           int channel   = m_lar_online_id->channel(hwid);
           if (barrel_ec<2 && pos_neg<2 && FT<32 && slot<16 && channel<128) 
            myid = (channel) | (slot << 7) | (FT<<11) | (pos_neg << 16) | (barrel_ec << 17);
           if (myid < 128) {
             ATH_MSG_WARNING("Why we have such cell: "<<barrel_ec<<" "<<pos_neg<<" "<<FT<<" "<<slot<<" "<<channel<<" | "<<myid);
             ATH_MSG_WARNING("from: "<<hwid.get_identifier32().get_compact());
           }
 
           if (larPedestal) {
            pedestal =  larPedestal->pedestal(hwid,(*first_cell)->gain());
           }
      
           m_HwidCell[m_ncells]=myid;
           m_ChidCell[m_ncells]=hwid.get_identifier32().get_compact();
           int index = (int) (m_calo_id->calo_cell_hash(cellID));
           if (IndexDigit[index]) {
               const std::vector<short>& vSamples=(IndexDigit[index])->samples();
               int nsamples = vSamples.size();
               for (int i=0;i<std::min(32,nsamples);i++) {
                 m_ADC[m_ncells][i]=(float)(vSamples[i])-pedestal;
               }
           }
        } // not Tile
        m_ncells++;
     }// iflag
 } // over cells

 ATH_MSG_INFO("Number of cells read " << m_ncells );

 m_tree->Fill();

 return StatusCode::SUCCESS;
}

