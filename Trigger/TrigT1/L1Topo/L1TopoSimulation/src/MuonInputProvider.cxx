/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInputProvider.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfData/L1Menu.h"

using namespace LVL1;

MuonInputProvider::MuonInputProvider( const std::string& type, const std::string& name, 
                                      const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

StatusCode
MuonInputProvider::initialize() {
   ATH_MSG_DEBUG("Retrieving LVL1ConfigSvc " << m_configSvc);
   CHECK( m_configSvc.retrieve() );

   // Get the RPC and TGC RecRoI tool
   ATH_CHECK( m_recRPCRoiTool.retrieve() );
   ATH_CHECK( m_recTGCRoiTool.retrieve() );
   
   ATH_MSG_INFO("UseNewConfig set to " <<  (m_useNewConfig ? "True" : "False"));
   if(! m_useNewConfig) {
      m_MuonThresholds = m_configSvc->ctpConfig()->menu().thresholdConfig().getThresholdVector(TrigConf::L1DataDef::MUON);
   }
   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "MuonInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   // get MuctpiTool handle
   ATH_MSG_DEBUG("Retrieving MuctpiToolHandle " << m_MuctpiSimTool);
   CHECK( m_MuctpiSimTool.retrieve() );


   //This is a bit ugly but I've done it so the job options can be used to determine 
   //use of storegate
   CHECK(m_MuCTPItoL1TopoLocation.initialize(!m_MuCTPItoL1TopoLocation.key().empty()));
   
   if(!m_MuCTPItoL1TopoLocationPlusOne.key().empty())
      m_MuCTPItoL1TopoLocationPlusOne = m_MuCTPItoL1TopoLocation.key()+std::to_string(1);
   
   CHECK(m_MuCTPItoL1TopoLocationPlusOne.initialize(!m_MuCTPItoL1TopoLocationPlusOne.key().empty()));

   CHECK(m_muonROILocation.initialize(!m_muonROILocation.key().empty()));
   CHECK(m_roibLocation.initialize(!m_roibLocation.key().empty()));

   return StatusCode::SUCCESS;
}

void
MuonInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   std::string histPath = "/EXPERT/" + name() + "/";
   std::replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt = std::make_unique<TH1I>("MuonTOBPt", "Muon TOB Pt", 40, 0, 40);
   hPt->SetXTitle("p_{T}");

   auto hEtaPhiTopo = std::make_unique<TH2I>("MuonTOBPhiEtaTopo", "Muon TOB Location", 200, -200, 200, 64, 0, 128);
   hEtaPhiTopo->SetXTitle("#eta");
   hEtaPhiTopo->SetYTitle("#phi");

   auto hEtaPhi = std::make_unique<TH2F>("MuonTOBPhiEta", "Muon TOB Location", 50, -5, 5, 32, -3.2, 3.2);
   hEtaPhi->SetXTitle("#eta");
   hEtaPhi->SetYTitle("#phi");

   auto hBW2or3 = std::make_unique<TH1I>("MuonTOBBW2or3", "Muon TOB BW2or3", 3, -1, 2);
   hBW2or3->SetXTitle("TGC full-station coincidence");

   auto hInnerCoin = std::make_unique<TH1I>("MuonTOBInnerCoin", "Muon TOB InnerCoin", 3, -1, 2);
   hInnerCoin->SetXTitle("TGC inner coincidence");

   auto hGoodMF = std::make_unique<TH1I>("MuonTOBGoodMF", "Muon TOB GoodMF", 3, -1, 2);
   hGoodMF->SetXTitle("good magnetic field");

   auto hCharge = std::make_unique<TH1I>("MuonTOBCharge", "Muon TOB Charge", 3, -1, 2);
   hCharge->SetXTitle("charge");

   auto hIs2cand = std::make_unique<TH1I>("MuonTOBIs2cand", "Muon TOB Is2cand", 3, -1, 2);
   hIs2cand->SetXTitle(">1 cand. in RPC pad");


   if (m_histSvc->regShared( histPath + "TOBPt", std::move(hPt), m_hPt ).isSuccess()){
      ATH_MSG_DEBUG("TOBPt histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBPt histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBPhiEtaTopo", std::move(hEtaPhiTopo), m_hEtaPhiTopo ).isSuccess()){
      ATH_MSG_DEBUG("TOBPhiEtaTopo histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBPhiEtaTopo histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBPhiEta", std::move(hEtaPhi), m_hEtaPhi ).isSuccess()){
      ATH_MSG_DEBUG("TOBPhiEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBPhiEta histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBBW2or3", std::move(hBW2or3), m_hBW2or3 ).isSuccess()){
      ATH_MSG_DEBUG("TOBBW2or3 histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBBW2or3 histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBInnerCoin", std::move(hInnerCoin), m_hInnerCoin ).isSuccess()){
      ATH_MSG_DEBUG("TOBInnerCoin histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBInnerCoin histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBGoodMF", std::move(hGoodMF), m_hGoodMF ).isSuccess()){
      ATH_MSG_DEBUG("TOBGoodMF histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBGoodMF histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBCharge", std::move(hCharge), m_hCharge ).isSuccess()){
      ATH_MSG_DEBUG("TOBCharge histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBCharge histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBIs2cand", std::move(hIs2cand), m_hIs2cand ).isSuccess()){
      ATH_MSG_DEBUG("TOBIs2cand histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBIs2cand histogram for MuonProvider");
   }
}

TCS::MuonTOB
MuonInputProvider::createMuonTOB(uint32_t roiword, const TrigConf::L1Menu * l1menu) const {

   LVL1::RecMuonRoI roi;
   if(m_useNewConfig) {
      roi.construct( roiword, m_recRPCRoiTool.get(), m_recTGCRoiTool.operator->(), l1menu );
   } else {
      roi.construct( roiword, m_recRPCRoiTool.operator->(), m_recTGCRoiTool.operator->(), &m_MuonThresholds );
   }

   ATH_MSG_DEBUG("Muon ROI: thrvalue = " << roi.getThresholdValue() << " eta = " << roi.eta() << " phi = " << roi.phi() << ", w   = " << MSG::hex << std::setw( 8 ) << roi.roiWord() << MSG::dec);
         
   TCS::MuonTOB muon( roi.getThresholdValue(), 0, int(10*roi.eta()), int(10*roi.phi()), roi.roiWord() );
   muon.setEtaDouble( roi.eta() );
   muon.setPhiDouble( roi.phi() );

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());
   
   return muon;
}

TCS::MuonTOB
MuonInputProvider::createMuonTOB(const MuCTPIL1TopoCandidate & roi) const {
   ATH_MSG_DEBUG("Muon ROI (MuCTPiToTopo): thr ID = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << roi.getphi()  
                  << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec );
   ATH_MSG_DEBUG("                            Oct = " << roi.getMioctID() << " etacode=" <<  roi.getetacode() << " phicode= " <<  
                  roi.getphicode()<< ", Sector="<< roi.getSectorName() );

   // roi.geteta() and roi.getphi() return the the exact geometrical coordinates of the trigger chambers
   // L1Topo granularities are 0.025 for eta (=> inverse = 40) and 0.05 for phi (=> inverse = 20)
   // L1Topo simulation uses positive phi (from 0 to 2pi) => transform phiTopo
   float fEta = roi.geteta();
   float fPhi = roi.getphi();
   
   int etaTopo = topoIndex(fEta,40);
   int phiTopo = topoIndex(fPhi,20);

   if (phiTopo < 0){ phiTopo += 128; }
   
   TCS::MuonTOB muon( roi.getptValue()*10, 0, etaTopo, static_cast<unsigned int>(phiTopo), roi.getRoiID() );
   muon.setEtDouble(static_cast<double>(roi.getptValue()));
   muon.setEtaDouble(static_cast<double>(fEta));
   muon.setPhiDouble(static_cast<double>(fPhi));

   // Muon flags
   if ( roi.getSectorName().at(0) != 'B' ) { // TGC ( endcap (E) + forward (F) )
      muon.setBW2or3( topoFlag(roi.getbw2or3()) );
      muon.setInnerCoin( topoFlag(roi.getinnerCoin()) );
      muon.setGoodMF( topoFlag(roi.getgoodMF()) );
      muon.setCharge( topoFlag(roi.getcharge()) );
      muon.setIs2cand( 0 );
   }
   else { // RPC ( barrel (B) )
      muon.setBW2or3( 0 );
      muon.setInnerCoin( 0 );
      muon.setGoodMF( 0 );
      muon.setCharge( 0 );
      muon.setIs2cand( topoFlag(roi.getis2cand()) );
   }

   m_hPt->Fill(muon.EtDouble());
   m_hEtaPhiTopo->Fill(muon.eta(),muon.phi());
   m_hEtaPhi->Fill(muon.EtaDouble(),muon.PhiDouble());

   m_hBW2or3->Fill( muon.bw2or3() );
   m_hInnerCoin->Fill( muon.innerCoin() );
   m_hGoodMF->Fill( muon.goodMF() );
   m_hCharge->Fill( muon.charge() );
   m_hIs2cand->Fill( muon.is2cand() );

   return muon;
}

TCS::LateMuonTOB
MuonInputProvider::createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const {

   float phi = roi.getphi();
   if(phi<-M_PI) phi+=2.0*M_PI;
   if(phi> M_PI) phi-=2.0*M_PI;

   ATH_MSG_DEBUG("Late Muon ROI (MuCTPiToTopo):bcid=1 thr pt = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << phi << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec);

   TCS::LateMuonTOB muon( roi.getptValue(), 0, int(10*roi.geteta()), int(10*phi), roi.getRoiID() );

   muon.setEtaDouble( roi.geteta() );
   muon.setPhiDouble( phi );

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());

   ATH_MSG_DEBUG("LateMuon created");
   return muon;
}

int
MuonInputProvider::topoIndex(float x, int g) const {
  float tmp = x*g;
  float index;
  if ( (abs(tmp)-0.5)/2. == std::round((abs(tmp)-0.5)/2.) ) {
    if ( tmp>0 ) { index = std::floor(tmp); }               
    else { index = std::ceil(tmp); }                      
  }
  else { index = std::round(tmp); }
  return static_cast<int>(index);
}

int
MuonInputProvider::topoFlag(bool flag) const {
  if ( flag == true ) { return 1; }
  else { return -1; }
}

StatusCode
MuonInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   if( m_MuonEncoding == 0 ) {

      ATH_MSG_DEBUG("Filling the muon input from MuCTPIToRoIBSLink produced by L1Muctpi.cxx.");

      const ROIB::RoIBResult* roibResult {nullptr};

      const L1MUINT::MuCTPIToRoIBSLink* muctpi_slink {nullptr};


      if(m_muonROILocation.key().empty()==false){
         SG::ReadHandle<L1MUINT::MuCTPIToRoIBSLink> MuCTPIHandle(m_muonROILocation);//LVL1MUCTPI::DEFAULT_MuonRoIBLocation)
         if( MuCTPIHandle.isValid() ){
            muctpi_slink = MuCTPIHandle.cptr();
         }
      }

      if(muctpi_slink == nullptr && not m_roibLocation.key().empty()){
         SG::ReadHandle<ROIB::RoIBResult> roib (m_roibLocation);
         if( roib.isValid() ){
            roibResult = roib.cptr(); 
         }
      }

      if(!muctpi_slink && !roibResult) {
         ATH_MSG_WARNING("Neither a MuCTPIToRoIBSLink with SG key " << m_muonROILocation.key() << " nor an RoIBResult were found in the event. No muon input for the L1Topo simulation.");
         return StatusCode::RECOVERABLE;
      }


      if( roibResult ) {

         const TrigConf::L1Menu * l1menu = nullptr;
         if( m_useNewConfig ) {
            ATH_CHECK( detStore()->retrieve(l1menu) );
         }

         const std::vector< ROIB::MuCTPIRoI >& rois = roibResult->muCTPIResult().roIVec();

         ATH_MSG_DEBUG("Filling the input event from RoIBResult. Number of Muon ROIs: " << rois.size() );

         for( const ROIB::MuCTPIRoI & muonRoI : rois ) {

            if( !( muonRoI.roIWord() & LVL1::CandidateVetoMask  ) ) {
               inputEvent.addMuon( MuonInputProvider::createMuonTOB( muonRoI.roIWord(), l1menu ) );
            } else {
            // overflow implemented only for reduced granularity encoding (see below)
               ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " <<  muonRoI.roIWord() );
            }
         }

      } else if( muctpi_slink ) {

         const TrigConf::L1Menu * l1menu = nullptr;
         if( m_useNewConfig ) {
            ATH_CHECK( detStore()->retrieve(l1menu) );
         }

         ATH_MSG_DEBUG("Filling the input event. Number of Muon ROIs: " << muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Header::wordsPerHeader - ROIB::Trailer::wordsPerTrailer - 1);
      
         unsigned int icnt = 0;
         for ( unsigned int roiword : muctpi_slink->getMuCTPIToRoIBWords() ) {
               
            ++icnt;
            // skip header
            if ( icnt <= ROIB::Header::wordsPerHeader + 1 ) {
               continue;
            }
            // skip trailer
            if ( icnt > ( muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Trailer::wordsPerTrailer ) ) {
               continue;
            }
            if( !(roiword & LVL1::CandidateVetoMask) ) {
               inputEvent.addMuon( MuonInputProvider::createMuonTOB( roiword, l1menu ) );
            } else {
               ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " << roiword );
            }
         }
      
      }
   } else {  // reduced granularity encoding
      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs.");

      // first see if L1Muctpi simulation already ran and object is in storegate, if not
      // call tool version of the L1MuctpiSimulation and create it on the fly

      const LVL1::MuCTPIL1Topo* l1topo  {nullptr};

      if(m_MuCTPItoL1TopoLocation.key().empty()==false){
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoh(m_MuCTPItoL1TopoLocation);
         if( l1topoh.isValid() ) l1topo = l1topoh.cptr();
      }

      if( l1topo ) {
         ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: retrieve from SG");

         const std::vector<MuCTPIL1TopoCandidate> & candList = l1topo->getCandidates();
         for( const MuCTPIL1TopoCandidate & muCand : candList) {
            inputEvent.addMuon( MuonInputProvider::createMuonTOB( muCand ) );
            if(muCand.moreThan2CandidatesOverflow()){
               inputEvent.setOverflowFromMuonInput(true);
               ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from SG)");
            }
         }
      } else {
         ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: calculate from ROIs sent to RoIB");
         LVL1::MuCTPIL1Topo l1topo;
         CHECK(m_MuctpiSimTool->fillMuCTPIL1Topo(l1topo));
         for( const MuCTPIL1TopoCandidate & cand : l1topo.getCandidates() ) {
            inputEvent.addMuon( MuonInputProvider::createMuonTOB( cand ) );
            if(cand.moreThan2CandidatesOverflow()){
               inputEvent.setOverflowFromMuonInput(true);
               ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from MuctpiSimTool)");
            }
         }
      }

      //BC+1 ... this can only come from simulation, in data taking this is collected by the L1Topo at its input
      // so no need to do anything else here
      if(m_MuCTPItoL1TopoLocationPlusOne.key().empty()==false) {
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoBC1(m_MuCTPItoL1TopoLocationPlusOne);
         if( l1topoBC1.isValid() ) {
            ATH_MSG_DEBUG( "Contains L1Topo LateMuons L1Muctpi object from StoreGate!" );
            const std::vector<MuCTPIL1TopoCandidate> & candList = l1topoBC1->getCandidates();
            for( const MuCTPIL1TopoCandidate& muCand : candList)
            {
               ATH_MSG_DEBUG("MuonInputProvider addLateMuon ");
               inputEvent.addLateMuon( MuonInputProvider::createLateMuonTOB( muCand ) );	   
            }
         }
      }
   }
   return StatusCode::SUCCESS;
}
