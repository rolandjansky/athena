/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// LArROD includes
#include "SuperCellVsCaloCellTestAlg.h"

#include "CaloEvent/CaloCellContainer.h"

#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"

#include "LArRawEvent/LArDigitContainer.h"

//needed for linker ...
//constexpr double SuperCellVsCaloCellTestAlg::eBins[SuperCellVsCaloCellTestAlg::nBinsE+1];
//constexpr double SuperCellVsCaloCellTestAlg::etaBins[SuperCellVsCaloCellTestAlg::nBinsEta+1];

SuperCellVsCaloCellTestAlg::SuperCellVsCaloCellTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_sc2ccMappingTool("CaloSuperCellIDTool"),
  m_ccIdHelper(nullptr),
  m_treeDigits(nullptr),
  m_tree(nullptr)
{

   declareProperty("SuperCellContainer",m_scKey="SCell");
   declareProperty("CaloCellContainer",m_ccKey="AllCalo");
   declareProperty("TruthSuperCellContainer",m_tscKey="SCellTruth");
   declareProperty("DigitContainer",m_digitKey="LArDigitSCL1","Only used for supicious supercells");

   declareProperty("OutputStream",m_stream="SUPERCELLTEST");

}


SuperCellVsCaloCellTestAlg::~SuperCellVsCaloCellTestAlg() {}


StatusCode SuperCellVsCaloCellTestAlg::initialize() {

   //get the mapping tool
   CHECK( m_sc2ccMappingTool.retrieve() );
   //and the id helper (used for making id hashes)
   CHECK( detStore()->retrieve (m_ccIdHelper, "CaloCell_ID") );

   double etBins[11] = {0.1,0.25,0.5,0.75,1.,1.5,2.,3.,4.,5.,10.}; //in GeV
   for(uint i=0;i<CaloSampling::getNumberOfSamplings();i++) {
      if(!m_ccKey.empty()) {
         m_etReso.push_back(new TProfile(TString::Format("%s_calocell",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s;CaloCell E_{T} [GeV];SuperCell E_{T} / CaloCell E_{T}",CaloSampling::getSamplingName(i).c_str()),10,etBins,"s"/* standard deviation for error*/));
         m_etReso.back()->SetDirectory(0);
	 m_Reso_et.push_back(new TH1F(TString::Format("%s_SuperCellResolution",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s;(SuperCell E_{T} - CaloCell E_{T}) / CaloCell E_{T} (%%)",CaloSampling::getSamplingName(i).c_str()),80,-40,40) );
	 m_Reso_et_vs_et.push_back(new TH2F(TString::Format("%s_SuperCellResolution_versus_et",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s; E_{T}[GeV] ; (SuperCell E_{T} - CaloCell E_{T}) / CaloCell E_{T} (%%)",CaloSampling::getSamplingName(i).c_str()),60,-10,50,80,-40,40) );
	 m_Reso_et_vs_eta.push_back(new TH2F(TString::Format("%s_SuperCellResolution_versus_eta",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s; #eta ; (SuperCell E_{T} - CaloCell E_{T}) / CaloCell E_{T} (%%)",CaloSampling::getSamplingName(i).c_str()),50,-2.5,2.5,80,-40,40) );
	 m_Linear_SCet_vs_et.push_back(new TH2F(TString::Format("%s_Linearity",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s; CaloCell Sum E_{T}[GeV] ; SuperCell E_{T}",CaloSampling::getSamplingName(i).c_str()),60,-10,50,60,-10,50) );
      }
      if(!m_tscKey.empty()) {
         m_etResoTruth.push_back(new TProfile(TString::Format("%s_truth",CaloSampling::getSamplingName(i).c_str()),TString::Format("%s;Truth SuperCell E_{T} [GeV];SuperCell E_{T} / Truth SuperCell E_{T}",CaloSampling::getSamplingName(i).c_str()),10,etBins,"s"/* standard deviation for error*/));
         m_etResoTruth.back()->SetDirectory(0);
      }
   }

   m_tree = new TTree("debug","debug");

   m_treeDigits= new std::vector<short int>;

   m_tree->Branch("EventNumber",&m_eventNumber);
   m_tree->Branch("Channel",&m_treeChannel);
   m_tree->Branch("Sampling",&m_treeSampling);
   m_tree->Branch("eta",&m_treeEta);
   m_tree->Branch("scET",&m_treeSCET);
   m_tree->Branch("truthET",&m_treeTruthET);
   m_tree->Branch("digits",&m_treeDigits);

   ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
   CHECK( histSvc->regTree(TString::Format("/%s/debug",m_stream.c_str()).Data(),m_tree) );

   ATH_CHECK( m_cablingKey.initialize() );


   return StatusCode::SUCCESS;
}

StatusCode SuperCellVsCaloCellTestAlg::finalize() {

   ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

   for(uint i=0;i<CaloSampling::getNumberOfSamplings();i++) { //don't bother writing empty hists
      if(m_etReso[i]->GetEntries()) CHECK( histSvc->regHist(TString::Format("/%s/SuperCellValidation/%s",m_stream.c_str(),m_etReso[i]->GetName()).Data(),m_etReso[i]) );
      if(m_Reso_et[i]->GetEntries()) CHECK( histSvc->regHist(TString::Format("/%s/SuperCellValidation/%s",m_stream.c_str(),m_Reso_et[i]->GetName()).Data(), m_Reso_et[i]) );
      if(m_Reso_et_vs_eta[i]->GetEntries()) CHECK( histSvc->regHist(TString::Format("/%s/SuperCellValidation/%s",m_stream.c_str(),m_Reso_et_vs_eta[i]->GetName()).Data(), m_Reso_et_vs_eta[i]) );
      if(m_Reso_et_vs_et[i]->GetEntries()) CHECK( histSvc->regHist(TString::Format("/%s/SuperCellValidation/%s",m_stream.c_str(),m_Reso_et_vs_et[i]->GetName()).Data(), m_Reso_et_vs_et[i]) );
      if(m_Linear_SCet_vs_et[i]->GetEntries()) CHECK( histSvc->regHist(TString::Format("/%s/SuperCellValidation/%s",m_stream.c_str(),m_Linear_SCet_vs_et[i]->GetName()).Data(), m_Linear_SCet_vs_et[i]) );


    if(!m_tscKey.empty()) {
      if(m_etResoTruth[i]->GetEntries())CHECK( histSvc->regHist(TString::Format("/%s/%s",m_stream.c_str(),m_etResoTruth[i]->GetName()).Data(),m_etResoTruth[i]) );
    }
   }

   //also write any graphs
   for(auto graphPointsX : m_graphsX) {
      TGraph* g = new TGraph(graphPointsX.second.size(),&graphPointsX.second[0],&m_graphsY[graphPointsX.first][0]);
      g->SetTitle(graphPointsX.first);
      g->SetMarkerStyle(6);
      CHECK( histSvc->regGraph(TString::Format("/%s/%s",m_stream.c_str(),graphPointsX.first.Data()).Data(),g) );
   }


  return StatusCode::SUCCESS;
}

StatusCode SuperCellVsCaloCellTestAlg::execute() {  
   //get the supercells, calocells
   const CaloCellContainer* scells=0;CHECK( evtStore()->retrieve(scells, m_scKey) );

   const CaloCellContainer* ccells=0;if(!m_ccKey.empty()) CHECK( evtStore()->retrieve(ccells, m_ccKey) );

   const CaloCellContainer* tscells=0;if(!m_tscKey.empty()) CHECK( evtStore()->retrieve(tscells,m_tscKey) );

   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey}; 
   const LArOnOffIdMapping* cabling{*cablingHdl};
   if(!cabling) {
      ATH_MSG_ERROR("Do not have SC mapping object " << m_cablingKey.key() );
      return StatusCode::FAILURE;
   }

   //iterate over supercells, and build up a histogram of the resolution
   for(auto scell : *scells) {
      //bool passPeakFinder( (scell->provenance()&0x40) );
      //if(!passPeakFinder) continue; //skip non maxima?

      int samplingEnum = m_ccIdHelper->calo_sample(scell->ID());

      double scellEt = scell->e()*scell->sinTh()*1e-3;

      if(ccells) {
         std::vector<Identifier> ccellIds = m_sc2ccMappingTool->superCellToOfflineID( scell->ID() );
         double cellEt(0.);
         //use findCell function of CaloCellContainer, which takes an identifier hash 
         for(auto& ccellId : ccellIds) {
            const CaloCell* ccell = ccells->findCell(m_ccIdHelper->calo_cell_hash(ccellId));
            if(!ccell) { ATH_MSG_WARNING("Could not find cell"); continue; }
            if(ccell->e()>0) cellEt += ccell->e()*ccell->sinTh();
         }
         if(cellEt>0) m_etReso[samplingEnum]->Fill(cellEt*1e-3,scellEt*1000./cellEt);
	 float resolution = 0.0;
	 if ( TMath::Abs(cellEt)>1 ) resolution = 100*(scellEt - cellEt*1e-3 ) / (cellEt*1e-3);
	 if ( TMath::Abs(cellEt)>150 ) {
	 	m_Reso_et[samplingEnum]->Fill( resolution );
	 	m_Reso_et_vs_et[samplingEnum]->Fill( cellEt*1e-3, resolution );
		m_Reso_et_vs_eta[samplingEnum]->Fill( scell->eta(), resolution );
		m_Linear_SCet_vs_et[samplingEnum]->Fill( cellEt*1e-3 , scellEt );
	 }
      }
            
      if(tscells) {
         const CaloCell* tscell = tscells->findCell(scell->caloDDE()->calo_hash());
         double tscellEt = tscell->e()*tscell->sinTh()*1e-3;
         if(tscellEt>0) m_etResoTruth[samplingEnum]->Fill(tscellEt,scellEt/tscellEt);

         //detect suspicious supercells .. where truth energy is greater than 1GeV and we measure less than 25% of it, or super cell ET is greater than 1GeV and truth ET < 25% of that
         if( (tscellEt>1. && scellEt/tscellEt<0.25) || (scellEt>1. && tscellEt/scellEt<0.25) ) {
	    m_eventNumber = getContext().eventID().event_number();

            HWIdentifier hwid = cabling->createSignalChannelID(scell->ID());
            m_treeChannel = hwid.get_identifier32().get_compact();
            m_treeSampling = samplingEnum;
            m_treeEta = scell->caloDDE()->eta();
            m_treeSCET = scellEt;
            m_treeTruthET = tscellEt;
            if(!m_digitKey.empty()) {
               const LArDigitContainer* digits=0;CHECK( evtStore()->retrieve(digits, m_digitKey) );
               for(auto digit : *digits) { if(digit->hardwareID()==hwid) *m_treeDigits = digit->samples(); }
            }
            m_tree->Fill();
         } 

      }




   }


  return StatusCode::SUCCESS;
}


