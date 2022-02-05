/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// NAME:     LArSuperCellMonAlg.cxx
//           based on LArCellMonAlg:
//                L. Morvaj, P.Strizenec, D. Oliveira Damazio - develop for Digital Trigger monitoring (2021)
// ********************************************************************
#include "LArSuperCellMonAlg.h"


#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloGain.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

//#include "AthenaMonitoring/DQBadLBFilterTool.h"
//#include "AthenaMonitoring/DQAtlasReadyFilterTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "AthenaKernel/Units.h"

#include <cassert>
#include <algorithm>

////////////////////////////////////////////
LArSuperCellMonAlg::LArSuperCellMonAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  :AthMonitorAlgorithm(name, pSvcLocator),
   //   m_badChannelMask("BadLArRawChannelMask",this),
   m_calo_id(nullptr)
{    
}



LArSuperCellMonAlg::~LArSuperCellMonAlg() {
}

////////////////////////////////////////////
StatusCode LArSuperCellMonAlg::initialize() {

  ATH_MSG_DEBUG("LArSuperCellMonAlg::initialize() start");


  //Identfier-helpers 
  ATH_CHECK( detStore()->retrieve(m_calo_id) );

  ATH_CHECK( m_superCellContainerKey.initialize() );
  ATH_CHECK( m_superCellContainerRefKey.initialize() );
  ATH_CHECK( m_noiseCDOKey.initialize() );
  ATH_CHECK( m_bcDataKey.initialize() );

  ATH_MSG_DEBUG("LArSuperCellMonAlg::initialize() is done!");

  return AthMonitorAlgorithm::initialize();
}



StatusCode LArSuperCellMonAlg::initThresh() {

  return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////////////////////
StatusCode LArSuperCellMonAlg::fillHistograms(const EventContext& ctx) const{  

  ATH_MSG_DEBUG("LArSuperCellMonAlg::fillHistograms() starts");

  SG::ReadHandle<CaloCellContainer> superCellHdl{m_superCellContainerKey, ctx};
  const CaloCellContainer* superCellCont = superCellHdl.cptr();
  
  SG::ReadHandle<CaloCellContainer> superCellRefHdl{m_superCellContainerRefKey, ctx};
  const CaloCellContainer* superCellRefCont = superCellRefHdl.cptr();
  
  SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey, ctx};
  const CaloNoise *noisep = *noiseHdl;

  SG::ReadCondHandle<BunchCrossingCondData> bccd (m_bcDataKey,ctx);

  if (ctx.evt()==0) {
    ATH_CHECK(createPerJobHistograms(superCellCont, noisep));
  }

  //get LB
  auto lumiBlock = Monitored::Scalar<unsigned int>("lumiBlock",0);
  lumiBlock = GetEventInfo(ctx)->lumiBlock();
  auto bcid = Monitored::Scalar<unsigned int>("bcid",0);
  bcid = GetEventInfo(ctx)->bcid();
  int bcidFFB = bccd->distanceFromFront(bcid,BunchCrossingCondData::BunchCrossings);



    //////////////// loop over SUPER cells -------------

  CaloCellContainer::const_iterator SCit = superCellCont->begin(); 
  CaloCellContainer::const_iterator SCit_e = superCellCont->end(); 

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;
  
  for ( ; SCit!=SCit_e;++SCit) {

    const CaloCell* superCell = *SCit; 
    variables.clear();
    
    float SCet = superCell->et();
    const CaloDetDescrElement* SCcaloDDE = superCell->caloDDE();
    float SCeta,SCphi;
    unsigned iLyr, iLyrNS;
    float SCt = superCell->time();
    
    getHistoCoordinates(SCcaloDDE, SCeta, SCphi, iLyr, iLyrNS);
    
 
    int SCprov = superCell->provenance()&0xFFF;
    bool SCpassTime = SCprov & 0x200;
    bool SCpassPF =   SCprov & 0x40;

    const CaloCell* superCellRef = superCellRefCont->findCell( SCcaloDDE->identifyHash() );
    float SCetRef = superCellRef->et();
    float resolution = -100;
    float resolutionPass = -100;
    float resolutionHET = -100;
    if ( SCetRef > 100 ) resolution = 100.0*(SCet-SCetRef)/SCetRef;
    if ( SCpassTime || SCpassPF ) resolutionPass = resolution;
    if ( SCet > 4e3 ) resolutionHET=resolution;

    // real monitoring business
    auto MSCet = Monitored::Scalar<float>("superCellEt",SCet);
    auto MSCt = Monitored::Scalar<float>("superCelltime",SCt);
    auto MSCprov = Monitored::Scalar<int>("superCellprovenance",SCprov);
    auto MSCeta = Monitored::Scalar<float>("superCellEta",SCeta);
    auto MSCres = Monitored::Scalar<float>("resolution",resolution);
    auto MSCresPass = Monitored::Scalar<float>("resolutionPass",resolutionPass);
    auto MSCresHET = Monitored::Scalar<float>("resolutionHET",resolutionHET);
    auto MSCetRef = Monitored::Scalar<float>("superCellEtRef",SCetRef);
    auto MSCtRef = Monitored::Scalar<float>("superCelltimeRef",superCellRef->time());
    auto MSCprovRef = Monitored::Scalar<int>("superCellprovenanceRef",(superCellRef->provenance()&0xFFF));
    variables.push_back(MSCet);
    variables.push_back(MSCt);
    variables.push_back(MSCprov);
    variables.push_back(MSCeta);
    if ( SCetRef > 100 ) variables.push_back(MSCres);
    if ( (SCetRef > 100 ) && (SCpassTime || SCpassPF ) ) variables.push_back(MSCresPass);
    if ( (SCetRef > 100 ) && (SCet > 4e3 ) ) variables.push_back(MSCresHET);
    variables.push_back(MSCetRef);
    // let us put conditional to force building the linearity plot
    // only when the new signal passes BCID
    if ( SCpassTime || SCpassPF ) variables.push_back(MSCtRef);
    variables.push_back(MSCprovRef);

    // per layer
    auto layerName=m_layerNames[iLyr];
    auto LMSCet = Monitored::Scalar<float>("superCellEt_"+layerName,SCet);
    auto LMSCt = Monitored::Scalar<float>("superCelltime_"+layerName,SCt);
    auto LMSCprov = Monitored::Scalar<int>("superCellprovenance_"+layerName,SCprov);
    auto LMSCeta = Monitored::Scalar<float>("superCellEta_"+layerName,SCeta);
    auto LMSCres = Monitored::Scalar<float>("resolution_"+layerName,resolution);
    auto LMSCresPass = Monitored::Scalar<float>("resolutionPass_"+layerName,resolutionPass);
    auto LMSCresHET = Monitored::Scalar<float>("resolutionHET_"+layerName,resolutionHET);
    auto LMSCetRef = Monitored::Scalar<float>("superCellEtRef_"+layerName,SCetRef);
    auto LMSCtRef = Monitored::Scalar<float>("superCelltimeRef_"+layerName,superCellRef->time());
    auto LMSCprovRef = Monitored::Scalar<int>("superCellprovenanceRef_"+layerName,(superCellRef->provenance()&0xFFF));

    auto MBCIDFFB = Monitored::Scalar<int>("BCID",bcidFFB);
    variables.push_back(LMSCet);
    variables.push_back(LMSCt);
    variables.push_back(LMSCprov);
    variables.push_back(LMSCeta);
    if ( SCetRef > 100 ) variables.push_back(LMSCres);
    if ( (SCetRef > 100 ) && (SCpassTime || SCpassPF ) ) variables.push_back(LMSCresPass);
    if ( (SCetRef > 100 ) && (SCet > 4e3 ) ) variables.push_back(LMSCresHET);
    variables.push_back(LMSCetRef);
    if ( SCpassTime || SCpassPF ) variables.push_back(LMSCtRef);
    variables.push_back(LMSCprovRef);
    variables.push_back(MBCIDFFB);

    fill(m_MonGroupName,variables);

  }	// end loop over SC
  



  ATH_MSG_DEBUG("LArSuperCellMonAlg::fillLarHists() is done");
  return StatusCode::SUCCESS;
}


StatusCode LArSuperCellMonAlg::createPerJobHistograms(const CaloCellContainer* cellCont, const CaloNoise *noisep ) const {

  ATH_MSG_INFO("Creating the once-per-job histograms");
  //The following histograms can be considered constants for one job
  //(in fact, they are constant for an entire run or even run-periode)
  //ActiveCells in eta/phi (to normalize 1D occupancy plots)
  //BadChannel word
  //Database noise
   
  auto doDatabaseNoisePlot = Monitored::Scalar<bool>("doDatabaseNoisePlot",m_doDatabaseNoiseVsEtaPhi);

  //  if(!doDatabaseNoisePlot && !doCellsActiveEtaPlot && !doCellsActivePhiPlot) {
  if(!doDatabaseNoisePlot) {
    ATH_MSG_INFO("No once-per-job histogram requested");
    return StatusCode::SUCCESS;
  }

  
  //filling:

  CaloCellContainer::const_iterator it = cellCont->begin(); 
  CaloCellContainer::const_iterator it_e = cellCont->end(); 
  for ( ; it!=it_e;++it) { 
    const CaloCell* cell = *it; 
    Identifier id = cell->ID();
    bool is_lar=m_calo_id->is_lar(id);
    if(!is_lar) continue;
    const CaloDetDescrElement* caloDDEl=cell->caloDDE();
    float celleta, cellphi;
    unsigned iLyr, iLyrNS;


    getHistoCoordinates(caloDDEl, celleta, cellphi, iLyr, iLyrNS);
    
    auto mon_eta = Monitored::Scalar<float>("celleta_"+m_layerNames[iLyr],celleta);
    auto mon_phi = Monitored::Scalar<float>("cellphi_"+m_layerNames[iLyr],cellphi);   
    auto cellnoisedb = Monitored::Scalar<float>("cellnoisedb_"+m_layerNames[iLyr],noisep->getNoise(id,cell->gain()));

    fill(m_MonGroupName,cellnoisedb,mon_eta,mon_phi);

    
  }//end loop over cells

  return StatusCode::SUCCESS;
}



std::string  LArSuperCellMonAlg::strToLower(const std::string& input) const {
  std::string output;
  for (const auto& c : input) {
    output.push_back(std::tolower(c));
  }
  return output;
}



void LArSuperCellMonAlg::getHistoCoordinates(const CaloDetDescrElement* dde, float& celleta, float& cellphi, unsigned& iLyr, unsigned& iLyrNS) const {
  
  celleta=dde->eta_raw();
  cellphi=dde->phi_raw();
  
  int calosample=dde->getSampling();
  if (dde->is_lar_em_endcap_inner()) calosample-=1; //Here, we consider the two layers of the EMEC IW as EME1 and EME2 instad of layer 2 and 3
  iLyrNS=m_caloSamplingToLyrNS.at(calosample); //will throw if out of bounds
  if ((iLyrNS==EMB1NS || iLyrNS==EMB2NS) && m_calo_id->region(dde->identify())==1) {
    //We are in the awkward region 1 of the EM Barrel
    //Looking at the image at http://atlas.web.cern.ch/Atlas/GROUPS/LIQARGEXT/TDR/figures6/figure6-17.eps
    //may be useful to understand what's going on here. 

    //In brief: Region 1, layer 1 is closer related ot the middle compartment (aka layer 2)
    //          and region 1, layer 2 closer related to the back compartment (aka layer 3)
    iLyrNS+=1;

    //Layer 2: 0<eta<1.4 + 1.4<eta<1.475, deta = 0.025. 3 rows of cells from region 1
    //Layer 3: 0<eta<1.35 (deta=0.050) +  1.4<eta<1.475 (deta = 0.075).  1 row of cell from region 1 with different dEta
  }
  
  const unsigned side=(celleta>0) ? 0 : 1; //Value >0 means A-side
  iLyr=iLyrNS*2+side;  //Getting LayerEnum value. This logic works because of the way the enums LayerEnum and LayerEnumNoSides are set up. 
  return;
}


