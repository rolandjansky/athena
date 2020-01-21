/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./CTPSimulation.h"

#include "TrigConfData/L1Menu.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "./CTPTriggerThreshold.h"
#include "./CTPTriggerItem.h"
#include "./CTPUtil.h"

#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Interfaces/CTPSLink.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/JetEnergyRoI.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include "TH2.h"

#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace TrigConf;

LVL1CTP::CTPSimulation::CTPSimulation( const std::string& name, ISvcLocator* pSvcLocator ) :
   AthReentrantAlgorithm ( name, pSvcLocator ), 
   m_decoder( new LVL1::CPRoIDecoder() ),
   m_jetDecoder( new LVL1::JEPRoIDecoder() )
{}

LVL1CTP::CTPSimulation::~CTPSimulation()
{}

StatusCode
LVL1CTP::CTPSimulation::initialize() {

   ATH_MSG_DEBUG("initialize");

   // data links
   ATH_CHECK( m_iKeyTopo.initialize() );
   ATH_CHECK( m_iKeyLegacyTopo.initialize() );
   ATH_CHECK( m_iKeyMuctpi.initialize() );
   ATH_CHECK( m_iKeyCtpinEM.initialize() );
   ATH_CHECK( m_iKeyCtpinJet.initialize() );
   ATH_CHECK( m_iKeyCtpinXE.initialize() );
   ATH_CHECK( m_iKeyJFexJets.initialize() );
   ATH_CHECK( m_iKeyJFexLJets.initialize() );
   ATH_CHECK( m_iKeyGFexJets.initialize() );
   ATH_CHECK( m_iKeyGFexMETPufit.initialize() );
   ATH_CHECK( m_iKeyGFexMETRho.initialize() );
   ATH_CHECK( m_iKeyGFexMETJwoJ.initialize() );
   ATH_CHECK( m_iKeyEFexCluster.initialize() );
   ATH_CHECK( m_iKeyEFexTau.initialize() );
   ATH_CHECK( m_oKeyRDO.initialize() );
   ATH_CHECK( m_oKeySLink.initialize() );

   // services
   ATH_CHECK( m_configSvc.retrieve() );
   ATH_CHECK( m_histSvc.retrieve() );
   ATH_CHECK( m_rndmSvc.retrieve() );

   // tools
   ATH_CHECK( m_resultBuilder.retrieve() );

   return StatusCode::SUCCESS;
}





StatusCode
LVL1CTP::CTPSimulation::start() {

   const TrigConf::L1Menu * l1menu = nullptr;
   if( m_useNewConfig ) {
      ATH_CHECK( m_detStore->retrieve(l1menu) ); 
      ATH_MSG_INFO( "Use L1 trigger menu from detector store" );
   } else {
      ATH_MSG_INFO( "Use L1 trigger menu from L1ConfigSvc" );
   }

   // configure the CTP ResultBuilder
   // currently both types of configuration can be given (transition period towards Run 3)
   CLHEP::HepRandomEngine* rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
   CHECK( m_resultBuilder->setConfiguration( m_configSvc->ctpConfig(), l1menu, rndmEngine ) );

   ConfigSource cfgsrc(m_configSvc->ctpConfig(), l1menu);

   CHECK( bookHists(cfgsrc) );

   return StatusCode::SUCCESS;
}


StatusCode

LVL1CTP::CTPSimulation::execute( const EventContext& context ) const {

   fillInputHistograms(context).ignore();

   std::map<std::string, unsigned int> thrMultiMap;

   extractMultiplicities(thrMultiMap, context).ignore();

   simulateItems(thrMultiMap, context).ignore();

   return StatusCode::SUCCESS;
}



StatusCode
LVL1CTP::CTPSimulation::createMultiplicityHist(const ConfigSource & cfgSrc, const std::string & type, TrigConf::L1DataDef::TriggerType tt, unsigned int maxMult ) {

   StatusCode sc;
   const TrigConf::CTPConfig* ctpConfig = cfgSrc.ctpConfig();
   const TrigConf::L1Menu* l1menu = cfgSrc.l1menu();
   if( l1menu ) {
      std::map<std::string,std::vector<std::string>> typeMapping = {
         { "muon", {"MU"} },
         { "jet", {"JET", "jJ", "gJ"} },
         { "xe", {"XE", "gXE", "jXE"} },
         { "te", {"TE"} },
         { "xs", {"XS"} },
         { "em", {"EM", "eEM"} },
         { "tau", {"TAU", "eTAU"} }
      };
      std::vector<TrigConf::L1Threshold> thrV;
      for( const std::string & t : typeMapping[type] ) {
         const std::vector<TrigConf::L1Threshold> & thrV = l1menu->thresholds(t);
         size_t xsize = thrV.empty() ? 1 : thrV.size();
         TH2* hist = new TH2I( Form("%sMult", t.c_str()),
                               Form("%s threshold multiplicity", t.c_str()), xsize, 0, xsize, maxMult, 0, maxMult);
         for(const TrigConf::L1Threshold & thr: thrV) {
            hist->GetXaxis()->SetBinLabel(thr.mapping()+1, thr.name().c_str() );
         }
         sc = hbook( "/multi/" + type, std::unique_ptr<TH2>(hist));
      }
   } else {
      const auto & thrV = ctpConfig->menu().thresholdConfig().getThresholdVector(tt);
      TH2* hist = new TH2I( Form("%sMult", type.c_str()),
                            Form("%s threshold multiplicity", type.c_str()), thrV.size(), 0, thrV.size(), maxMult, 0, maxMult);
      for(const TrigConf::TriggerThreshold * thr: thrV) {
         hist->GetXaxis()->SetBinLabel(thr->mapping()+1, thr->name().c_str() );
      }
      sc = hbook( "/multi/" + type, std::unique_ptr<TH2>(hist));
   }
   return sc;
}


StatusCode
LVL1CTP::CTPSimulation::hbook(const std::string & path, std::unique_ptr<TH1> hist) {
   const std::string & hname(hist->GetName());

   std::string key(path);
   if(key.back()!='/') key += "/";
   key+=hname;

   if(m_hist1D.find(key)!=m_hist1D.end()) {
      ATH_MSG_WARNING("1D-hist " << key << " already exists, won't register it");
      return StatusCode::RECOVERABLE;
   }

   LockedHandle<TH1> lh;
   StatusCode sc = m_histSvc->regShared( m_histPath + name() + path + "/" + hname, std::move(hist), lh );
   if( sc.isSuccess() ) {
      ATH_MSG_DEBUG("1D-hist " << hname << " registered with key - " << key);
      m_hist1D[key] = lh;
   } else {
      ATH_MSG_WARNING("Could not register histogram " << hname);
   }
   return sc;
}

StatusCode
LVL1CTP::CTPSimulation::hbook(const std::string & path, std::unique_ptr<TH2> hist) {
   const std::string & hname(hist->GetName());

   std::string key(path);
   if(key.back()!='/') key += "/";
   key+=hname;

   if(m_hist2D.find(key)!=m_hist2D.end()) {
      ATH_MSG_WARNING("2D-hist " << key << " already exists, won't register it");
      return StatusCode::RECOVERABLE;
   }

   LockedHandle<TH2> lh;
   StatusCode sc = m_histSvc->regShared( m_histPath + name() + path + "/" + hname, std::move(hist), lh );
   if( sc.isSuccess() ) {
      ATH_MSG_DEBUG("2D-hist " << hname << " registered with key - " << key);
      m_hist2D[key] = lh;
   } else {
      ATH_MSG_WARNING("Could not register histogram " << hname);
   }
   return sc;
}

LockedHandle<TH1> &
LVL1CTP::CTPSimulation::get1DHist(const std::string & histName) const {
   if(m_hist1D.find(histName) == m_hist1D.end()) {
      ATH_MSG_ERROR("1D-hist with registration key " << histName << " does not exist");
   }
   return  m_hist1D[histName];
}

LockedHandle<TH2> &
LVL1CTP::CTPSimulation::get2DHist(const std::string & histName) const {
   if(m_hist2D.find(histName) == m_hist2D.end()) {
      ATH_MSG_ERROR("2D-hist " << histName << " does not exist");
   }
   return  m_hist2D[histName];
}


StatusCode
LVL1CTP::CTPSimulation::bookHists(const ConfigSource & cfgSrc) {

   const TrigConf::CTPConfig* ctpConfig = cfgSrc.ctpConfig();
   const TrigConf::L1Menu* l1menu = cfgSrc.l1menu();

   // jets
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jJetPt","Jet p_{T} - jJ", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jJetEta","Jet #eta - jJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jJetPhi","Jet #phi - jJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jLJetPt","Jet p_{T} - jLJ", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jLJetEta","Jet #eta - jLJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("jLJetPhi","Jet #phi - jLJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gJetPt","Jet p_{T} - gJ", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gJetEta","Jet #eta - gJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gJetPhi","Jet #phi - gJ", 64, -3.2, 3.2) ));

   // MET
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("Pufit","Missing ET from algorithm pufit", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("PufitPhi","Missing ET PUfit phi", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("Rho","Missing ET from algorithm rhosub", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("RhoPhi","Missing ET rhosub phi", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("JwoJ","Missing ET from algorithm jet without jets", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/met/", std::make_unique<TH1I>("JwoJPhi","Missing ET jet without jet phi", 64, -3.2, 3.2) ));

   // cluster
   ATH_CHECK ( hbook( "/input/em/", std::make_unique<TH1I>("et","Cluster et", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/em/", std::make_unique<TH1I>("eta","Cluster eta ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/em/", std::make_unique<TH1I>("phi","Cluster phi", 64, -3.2, 3.2) ));

   // tau
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("et","Tau et", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("eta","Tau eta ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("phi","Tau phi", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("emIso","Tau em isolation", 40, 0, 1) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("hadIso","Tau hadronic isolation", 40, 0, 1) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("R3ET","Tau eT", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/tau/", std::make_unique<TH1I>("R3Iso","Tau isolation", 40, 0, 1) ));

   // input counts
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("jJets","Number of jets (jJ)", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("jLJets","Number of jets (jLJ)", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("gJets","Number of jets (gJ)", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("muons","Number of muons", 10, 0, 10) ));  
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("emcluster","Number of EM clusters", 20, 0, 20) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("taus","Number of TAU candidates", 20, 0, 20) ));
 
   // threshold multiplicities
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "muon", L1DataDef::MUON, 5) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "jet", L1DataDef::JET) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "xe", L1DataDef::XE, 2) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "te", L1DataDef::TE, 2) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "xs", L1DataDef::XS, 2) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "em", L1DataDef::EM) );
   ATH_CHECK ( createMultiplicityHist( cfgSrc, "tau", L1DataDef::TAU) );

   // Topo
   TH1I* hTopo0 = new TH1I("l1topo0","L1Topo Decision Cable 0", 64, 0, 64);
   TH1I* hTopo1 = new TH1I("l1topo1","L1Topo Decision Cable 1", 64, 0, 64);
   if ( l1menu ) {
      // to be implemented
   } else {
      for(const TIP * tip : ctpConfig->menu().tipVector() ) {
         if ( tip->tipNumber() < 384 )
            continue;
         unsigned int tipNumber = (unsigned int) ( tip->tipNumber() - 384 );
         switch(tipNumber / 64) {
         case 0:
            hTopo0->GetXaxis()->SetBinLabel(1+ tipNumber % 64, tip->thresholdName().c_str() );
            break;
         case 1:
            hTopo1->GetXaxis()->SetBinLabel(1+ tipNumber % 64, tip->thresholdName().c_str() );
            break;
         default:
            break;
         }
      }
   }
   ATH_CHECK( hbook( "/input/topo/", std::unique_ptr<TH1I>(hTopo0) ));
   ATH_CHECK( hbook( "/input/topo/", std::unique_ptr<TH1I>(hTopo1) ));


   // item decision
   TH1I * tbpByID = new TH1I("tbpById", "Items decision (tbp)", 512, 0, 512);
   TH1I * tapByID = new TH1I("tapById", "Items decision (tap)", 512, 0, 512);
   TH1I * tavByID = new TH1I("tavById", "Items decision (tav)", 512, 0, 512);

   TH1I * tbpByName = new TH1I("tbpByName", "Items decision (tbp)", 512, 0, 512);
   TH1I * tapByName = new TH1I("tapByName", "Items decision (tap)", 512, 0, 512);
   TH1I * tavByName = new TH1I("tavByName", "Items decision (tav)", 512, 0, 512);

   std::vector<std::string> orderedItemnames;
   if( l1menu ) {
      orderedItemnames.reserve( l1menu->size() );
      for( const auto & item : *l1menu ) {
         orderedItemnames.emplace_back(item.name());
      }
   } else {
      orderedItemnames.reserve(ctpConfig->menu().items().size());
      for( const auto & item : ctpConfig->menu().items() ) {
         orderedItemnames.emplace_back(item->name());
      }
   }
   std::sort(orderedItemnames.begin(), orderedItemnames.end());
   
   unsigned int bin = 1;
   for ( const std::string & itemname : orderedItemnames ) {
      unsigned int ctpId(0);
      if (l1menu) {
         TrigConf::L1Item item = l1menu->item( itemname );
         ctpId = item.ctpId();
      } else {
         const TrigConf::TriggerItem * item = ctpConfig->menu().item( itemname );
         ctpId = item->ctpId();
      }
      tbpByID->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tapByID->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tavByID->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tbpByName->GetXaxis()->SetBinLabel( bin, itemname.c_str() );
      tapByName->GetXaxis()->SetBinLabel( bin, itemname.c_str() );
      tavByName->GetXaxis()->SetBinLabel( bin++, itemname.c_str() );
   }

   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tbpByID) ));
   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tapByID) ));
   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tavByID) ));
   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tbpByName) ));
   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tapByName) ));
   ATH_CHECK ( hbook( "/output/", std::unique_ptr<TH1I>(tavByName) ));

   ATH_CHECK ( hbook( "/", std::unique_ptr<TH1I>(new TH1I("bcid", "Bunch crossing ID", 3564, 0, 3564)) ));

   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::fillInputHistograms(const EventContext& context) const {

   ATH_MSG_DEBUG( "fillInputHistograms" );

   // jFEX jets
   auto jFexJets  = SG::makeHandle( m_iKeyJFexJets, context );
   if(jFexJets.isValid()) {
      get1DHist("/input/counts/jJets")->Fill(jFexJets->size());
      auto h0 = *get1DHist("/input/jets/jJetPt"); // calling operator* to get the Guard outside the filling loop
      auto h1 = *get1DHist("/input/jets/jJetEta");
      auto h2 = *get1DHist("/input/jets/jJetPhi");
      for( const auto & jet : *jFexJets ) {
         h0->Fill(fabs(jet->et8x8()/1000.));
         h1->Fill(jet->eta());
         h2->Fill(jet->phi());
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyJFexJets);
   }

   // jFEX large-R jets
   auto jFexLJets = SG::makeHandle( m_iKeyJFexLJets, context );
   if(jFexLJets.isValid()) {
      get1DHist("/input/counts/jLets")->Fill(jFexLJets->size());
      auto h0 = *get1DHist("/input/jets/jLJetPt");
      auto h1 = *get1DHist("/input/jets/jLJetEta");
      auto h2 = *get1DHist("/input/jets/jLJetPhi");
      for( const auto & jet : *jFexLJets ) {
         h0->Fill(fabs(jet->et8x8()/1000.));
         h1->Fill(jet->eta());
         h2->Fill(jet->phi());
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyJFexLJets);
   }

   // gFEX jets
   auto gFexJets  = SG::makeHandle( m_iKeyGFexJets, context );
   if(gFexJets.isValid()) {
      get1DHist("/input/counts/gJets")->Fill(gFexJets->size());
      auto h0 = *get1DHist("/input/jets/gJetPt");
      auto h1 = *get1DHist("/input/jets/gJetEta");
      auto h2 = *get1DHist("/input/jets/gJetPhi");
      for( const auto & jet : *gFexJets ) {
         h0->Fill(fabs(jet->et8x8()/1000.));
         h1->Fill(jet->eta());
         h2->Fill(jet->phi());
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyGFexJets);
   }

   // MET
   auto gFexMETPufit  = SG::makeHandle( m_iKeyGFexMETPufit, context );
   if( gFexMETPufit.isValid() ) {
      get1DHist("/input/met/Pufit")->Fill(gFexMETPufit->energyT()/1000.);
      get1DHist("/input/met/PufitPhi")->Fill(atan2(gFexMETPufit->energyX(), gFexMETPufit->energyY()));
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETPufit);
   }
   auto gFexMETRho  = SG::makeHandle( m_iKeyGFexMETRho, context );
   if( gFexMETRho.isValid() ) {
      get1DHist("/input/met/Rho")->Fill(gFexMETRho->energyT()/1000.);
      get1DHist("/input/met/RhoPhi")->Fill(atan2(gFexMETRho->energyX(), gFexMETRho->energyY()));
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETRho);
   }
   auto gFexMETJwoJ  = SG::makeHandle( m_iKeyGFexMETJwoJ, context );
   if( gFexMETJwoJ.isValid() ) {
      get1DHist("/input/met/JwoJ")->Fill(gFexMETJwoJ->energyT()/1000.);
      get1DHist("/input/met/JwoJPhi")->Fill(atan2(gFexMETJwoJ->energyX(), gFexMETJwoJ->energyY()));
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETJwoJ);
   }

   // EM cluster
   auto eFexCluster  = SG::makeHandle( m_iKeyEFexCluster, context );
   if( eFexCluster.isValid() ) {
      get1DHist( "/input/counts/emcluster")->Fill(eFexCluster->size());
      auto h0 = *get1DHist("/input/em/et");
      auto h1 = *get1DHist("/input/em/eta");
      auto h2 = *get1DHist("/input/em/phi");
      for( const auto & cl : *eFexCluster ) {
       h0->Fill(cl->et());
       h1->Fill(cl->eta());
       h2->Fill(cl->phi());
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyEFexCluster);
   }

   // eFEX Tau
   auto eFexTau  = SG::makeHandle( m_iKeyEFexTau, context );
   if( eFexTau.isValid() ) {
      get1DHist( "/input/counts/taus")->Fill(eFexTau->size());
      auto h0 = *get1DHist("/input/tau/et");
      auto h1 = *get1DHist("/input/tau/eta");
      auto h2 = *get1DHist("/input/tau/phi");
      auto h3 = *get1DHist("/input/tau/emIso");
      auto h4 = *get1DHist("/input/tau/hadIso");
      auto h5 = *get1DHist("/input/tau/R3ClusterET");
      auto h6 = *get1DHist("/input/tau/R3ClusterIso");
      const static SG::AuxElement::ConstAccessor<float> accR3ClET ("R3ClusterET");
      const static SG::AuxElement::ConstAccessor<float> accR3ClIso ("R3ClusterIso");
      for( const auto & tau : *eFexTau ) {
         h0->Fill(tau->eT());
         h1->Fill(tau->eta());
         h2->Fill(tau->phi());
         h3->Fill(tau->emIsol());
         h4->Fill(tau->hadIsol());
         h5->Fill(accR3ClET(*tau)/1000.);
         h6->Fill(accR3ClIso(*tau));
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyEFexTau);
   }

   // topo
   auto topoInput = SG::makeHandle( m_iKeyTopo, context );
   if(topoInput.isValid()) {
      ATH_MSG_DEBUG("Retrieved input from L1Topo from StoreGate with key " << m_iKeyTopo);
      ATH_MSG_DEBUG("L1Topo0 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord1(0));
      ATH_MSG_DEBUG("L1Topo0 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord1(1));
      ATH_MSG_DEBUG("L1Topo1 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord2(0));
      ATH_MSG_DEBUG("L1Topo1 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord2(1));
      auto h0 = *get1DHist("l1Topo0");
      auto h1 = *get1DHist("l1Topo1");
      for(unsigned int i=0; i<32; ++i) {
         uint32_t mask = 0x1; mask <<= i;
         if( (topoInput->cableWord1(0) & mask) != 0 ) h0->Fill(i); // cable 0, clock 0
         if( (topoInput->cableWord1(1) & mask) != 0 ) h0->Fill(32 + i); // cable 0, clock 1
         if( (topoInput->cableWord2(0) & mask) != 0 ) h1->Fill(i); // cable 1, clock 0
         if( (topoInput->cableWord2(1) & mask) != 0 ) h1->Fill(32 + i); // cable 1, clock 1
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyTopo);
   }

   // bcid
   auto bcid = context.eventID().bunch_crossing_id();
   get1DHist( "/bcid")->Fill(bcid);

   return StatusCode::SUCCESS;
}

namespace {
   bool containsBunch(unsigned int bcid, const TrigConf::BunchGroup & bg) {
      return std::find(bg.bunches().begin(), bg.bunches().end(), bcid) != bg.bunches().end();
   }
}

StatusCode
LVL1CTP::CTPSimulation::extractMultiplicities(std::map<std::string, unsigned int> & thrMultiMap, const EventContext& context) const {

   const TrigConf::L1Menu * l1menu = nullptr;
   if( m_useNewConfig ) {
      ATH_CHECK( m_detStore->retrieve(l1menu) );
   }

   thrMultiMap.clear();

   if( l1menu ) {
      const auto & thrV = l1menu->thresholds();
      for ( const TrigConf::L1Threshold & thr : thrV ) {
         // get the multiplicity for each threshold
         unsigned int multiplicity = calculateMultiplicity( thr, l1menu, context );
         // and record in threshold--> multiplicity map (to be used for item decision)
         thrMultiMap[thr.name()] = multiplicity;
      }
   } else {
      for ( const TrigConf::TriggerThreshold * thr : m_configSvc->ctpConfig()->menu().thresholdVector() ) {
         // get the multiplicity for each threshold
         unsigned int multiplicity = calculateMultiplicity( thr, context );
         // and record in threshold--> multiplicity map (to be used for item decision)
         thrMultiMap[thr->name()] = multiplicity;
      }
   }


   // internal triggers
   auto bcid = context.eventID().bunch_crossing_id();
   get1DHist( "/bcid")->Fill(bcid);
   if( m_forceBunchGroupPattern ) {
      // force bunch group pattern from job options
      for ( size_t bg = 0; bg < 16; ++bg ) {
         std::string bgName("BGRP");
         bgName += std::to_string(bg);
         thrMultiMap[bgName] = ( m_bunchGroupPattern & (0x1<<bg) ) ? 1 : 0;
      }
   } else {
      // use bunchgroup definition from configuration and pick according to the BCID
      if( ! l1menu ) {
         for( const TrigConf::BunchGroup & bg : m_configSvc->ctpConfig()->bunchGroupSet().bunchGroups() ) {
            std::string bgName("BGRP");
            bgName += std::to_string(bg.internalNumber());
            thrMultiMap[bgName] = containsBunch(bcid,bg) ? 1 : 0;
         }
      }
   }

   // all RNDM triggers run with 40MHz, so they are always in
   thrMultiMap["RNDM0"] = 1;
   thrMultiMap["RNDM1"] = 1;
   thrMultiMap["RNDM2"] = 1;
   thrMultiMap["RNDM3"] = 1;


   return StatusCode::SUCCESS;
}



unsigned int
LVL1CTP::CTPSimulation::calculateJetMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if( confThr.type() == "JET" ) {
      auto ctpinJet  = SG::makeHandle( m_iKeyCtpinJet, context );
      if ( ctpinJet.isValid() ) {
         if( l1menu->connector("JET1").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("JET1").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinJet->cableWord0(), triggerline.startbit(), triggerline.endbit() );
         } else if( l1menu->connector("JET2").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("JET2").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinJet->cableWord1(), triggerline.startbit(), triggerline.endbit() );
         }
      }
   } else {
      // Run-3 threshold
      const SG::ReadHandleKey< xAOD::JetRoIContainer > * rhk { nullptr };
      if( confThr.type() == ("gJ") ) {
	 rhk = & m_iKeyGFexJets;
      } else if( confThr.name().find("jL") == 0 ) {
	 rhk = & m_iKeyJFexLJets;
      } else if( confThr.name().find("j") == 0 ) {
	 rhk = & m_iKeyJFexJets;
      } else {
	 ATH_MSG_ERROR( "Unexpected threshold name " << confThr.name() << ". Should start with j, jL, g, or J.");
      }
      auto jets = SG::makeHandle( *rhk, context );
      if ( jets.isValid() ) {
         auto pt = confThr.getAttribute<unsigned int>("pt");
         auto ranges = confThr.getList("ranges");
	 for ( const auto & jet : *jets ) {
            if( (unsigned int) (jet->et8x8()/1000.) < pt ) continue;
            // calculate eta index from eta
            float eta = jet->eta();
            LVL1::Coordinate coord(/*phi=*/0, eta);
            LVL1::CoordToHardware converter;
            unsigned int jepCoord = converter.jepCoordinateWord(coord);
            uint32_t roiword = jepCoord << 19;
            auto coordRange = m_jetDecoder->coordinate(roiword);
            int ieta =
               int((coordRange.eta() + ((coordRange.eta() > 0.01) ? 0.025 : -0.025)) / 0.1) - 1;
            // Adjustment due to irregular geometries
            if (ieta > 24)
               ieta += 2;
            // copied from
	    // https://acode-browser.usatlas.bnl.gov/lxr/source/athena/Trigger/TrigT1/TrigT1CaloUtils/src/JetAlgorithm.cxx#0337
	    //int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
	    //int iphi = 0; // int((m_refPhi-0.005)*32/M_PI); iphi = 16*(iphi/16) + 8;
            bool inRange = false;
            for( auto r : ranges ) {
               if( ieta >= r.getAttribute<int>("etamin") &&
                   ieta <= r.getAttribute<int>("etamax") ) {
                  inRange = true; break;
               }
            }
            if( ! inRange )
               continue;
            ++multiplicity;
         }
      }
   }
   get2DHist( "/multi/jet/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("JET MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}



unsigned int
LVL1CTP::CTPSimulation::calculateJetMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if( confThr->name().find("J") == 0 ) {
      auto ctpinJet  = SG::makeHandle( m_iKeyCtpinJet, context );
      if ( ctpinJet.isValid() ) {
         if ( confThr->cableName() == "JEP1" || confThr->cableName() == "JET1" ) {
            multiplicity = CTPUtil::getMult( ctpinJet->cableWord0(), confThr->cableStart(), confThr->cableEnd() );
         } else if ( confThr->cableName() == "JEP2" || confThr->cableName() == "JET2" ) {
            multiplicity = CTPUtil::getMult( ctpinJet->cableWord1(), confThr->cableStart(), confThr->cableEnd() );
         }
      } 
   } else {
      // Run-3 threshold
      const SG::ReadHandleKey< xAOD::JetRoIContainer > * rhk { nullptr };
      if( confThr->name().find("g") == 0 ) {
	 rhk = & m_iKeyGFexJets;
      } else if( confThr->name().find("jL") == 0 ) {
	 rhk = & m_iKeyJFexLJets;
      } else if( confThr->name().find("j") == 0 ) {
	 rhk = & m_iKeyJFexJets;
      } else {
	 ATH_MSG_ERROR( "Unexpected threshold name " << confThr->name() << ". Should start with j, jL, g, or J.");
      }
      auto jets = SG::makeHandle( *rhk, context );
      if ( jets.isValid() ) {
	 for ( const auto & jet : *jets ) {
            float eta = jet->eta();
            float phi = jet->phi();
            if ( phi < 0 ) phi += 2*M_PI;
            if ( phi >= 2*M_PI ) phi -= 2*M_PI;
            LVL1::Coordinate coord(phi, eta);
            LVL1::CoordToHardware converter;
            unsigned int jepCoord = converter.jepCoordinateWord(coord);
            uint32_t roiword = jepCoord << 19;
            auto coordRange = m_jetDecoder->coordinate(roiword);
            int ieta =
               int((coordRange.eta() + ((coordRange.eta() > 0.01) ? 0.025 : -0.025)) / 0.1) - 1;
            // Adjustment due to irregular geometries
            if (ieta > 24)
               ieta += 2;
            int iphi = int((coordRange.phiRange().min() + 0.025) * 32 / M_PI);
            // copied from
	    // https://acode-browser.usatlas.bnl.gov/lxr/source/athena/Trigger/TrigT1/TrigT1CaloUtils/src/JetAlgorithm.cxx#0337
	    //int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
	    //int iphi = 0; // int((m_refPhi-0.005)*32/M_PI); iphi = 16*(iphi/16) + 8;
            bool pass = ((unsigned int) (jet->et8x8()/1000.)) > confThr->triggerThresholdValue( ieta, iphi )->ptcut();
            multiplicity += pass ? 1 : 0;
         }
      }
   }
   get2DHist( "/multi/jet/jetMult" )->Fill(confThr->mapping(), multiplicity);
   ATH_MSG_DEBUG("JET MULT calculated mult for threshold " << confThr->name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateEMMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity (0);
   if ( confThr.name()[0]=='e' ) {
      // new EM threshold from eFEX
      float scale = l1menu->getObject("thresholds.legacyCalo.EM.emscale").getValue<float>();
      auto eFexCluster = SG::makeHandle( m_iKeyEFexCluster, context );
      for ( const auto & cl : *eFexCluster ) {
         float eta = cl->eta();
         int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
         TrigConf::DataStructure thrV = confThr.thresholdValue( ieta );
         bool clusterPasses = ( ((unsigned int) cl->et()) > thrV.getAttribute<unsigned int>("et")*scale ); // need to add cut on isolation and other variables, once available
         multiplicity +=  clusterPasses ? 1 : 0;
      }
   } else {
      // old EM threshold from data
      auto ctpinEM  = SG::makeHandle( m_iKeyCtpinEM, context );
      if ( ctpinEM.isValid() ) {
         if( l1menu->connector("EM1").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("EM1").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord0(), triggerline.startbit(), triggerline.endbit() );
         } else if( l1menu->connector("EM2").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("EM2").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord1(), triggerline.startbit(), triggerline.endbit() );
         }
      }
   }
   get2DHist( "/multi/em/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("EM MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}



unsigned int
LVL1CTP::CTPSimulation::calculateEMMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity (0);
   if ( confThr->name()[0]=='e' ) { 
      // new EM threshold from eFEX
      auto eFexCluster = SG::makeHandle( m_iKeyEFexCluster, context );
      for ( const auto & cl : *eFexCluster ) {
         float eta = cl->eta();
         int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
         int iphi = 0;
         const TrigConf::TriggerThresholdValue * thrV = confThr->triggerThresholdValue( ieta, iphi );
         const ClusterThresholdValue *ctv = dynamic_cast<const ClusterThresholdValue *>(thrV);
         float scale = ctv->caloInfo().globalEmScale();
         bool clusterPasses = ( ((unsigned int) cl->et()) > thrV->ptcut()*scale ); // need to add cut on isolation and other variables, once available
         multiplicity +=  clusterPasses ? 1 : 0;
      }
   } else {
      // old EM threshold from data
      auto ctpinEM  = SG::makeHandle( m_iKeyCtpinEM, context );
      if ( ctpinEM.isValid() ) {
         if ( confThr->cableName() == "CP1" || confThr->cableName() == "EM1" ) {
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord0(), confThr->cableStart(), confThr->cableEnd() );
         } else if ( confThr->cableName() == "CP2" || confThr->cableName() == "EM2" ) {
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord1(), confThr->cableStart(), confThr->cableEnd() );
         }
      }
   }
   get2DHist( "/multi/em/emMult" )->Fill(confThr->mapping(), multiplicity);
   ATH_MSG_DEBUG("EM MULT calculated mult for threshold " << confThr->name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTauMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if ( confThr.name()[0]=='e' ) {
      // new TAU threshold from eFEX
      auto eFexTaus  = SG::makeHandle( m_iKeyEFexTau, context );
      const static SG::AuxElement::ConstAccessor<float> accR3ClET ("R3ClusterET");
      const static SG::AuxElement::ConstAccessor<float> accR3ClIso ("R3ClusterIso");
      if( eFexTaus.isValid() ) {
         for ( const auto & tau : *eFexTaus ) {
            unsigned int eT = (unsigned int) (accR3ClET(*tau)/1000.); // tau eT is in MeV while the cut is in GeV - this is only temporary and needs to be made consistent for all L1Calo
            //float iso = accR3ClIso(*tau);
            unsigned int etCut = confThr.data().get_child("et").get_value<unsigned int>();
            bool tauPasses = ( eT >= etCut ); // need to add cut on isolation and other variables, once available
            multiplicity +=  tauPasses ? 1 : 0;
         }
      }
   } else {
      // old TAU threshold
      auto ctpinEM  = SG::makeHandle( m_iKeyCtpinEM, context );
      if ( ctpinEM.isValid() ) {
         if( l1menu->connector("TAU1").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("TAU1").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord2(), triggerline.startbit(), triggerline.endbit() );
         } else if( l1menu->connector("TAU2").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("TAU2").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord3(), triggerline.startbit(), triggerline.endbit() );
         }
      }
   }
   get2DHist( "/multi/tau/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("TAU MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTauMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if ( confThr->name()[0]=='e' ) { 
      // new TAU threshold from eFEX
      auto eFexTaus  = SG::makeHandle( m_iKeyEFexTau, context );
      const static SG::AuxElement::ConstAccessor<float> accR3ClET ("R3ClusterET");
      const static SG::AuxElement::ConstAccessor<float> accR3ClIso ("R3ClusterIso");
      if( eFexTaus.isValid() ) {
         for ( const auto & tau : *eFexTaus ) {
            unsigned int eT = (unsigned int) (accR3ClET(*tau)/1000.); // tau eT is in MeV while the cut is in GeV - this is only temporary and needs to be made consistent for all L1Calo
            //float iso = accR3ClIso(*tau);
            float eta = tau->eta();
            int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
            int iphi = 0;
            const TrigConf::TriggerThresholdValue * thrV = confThr->triggerThresholdValue( ieta, iphi );
            bool tauPasses = ( eT >= thrV->ptcut() ); // need to add cut on isolation and other variables, once available
            multiplicity +=  tauPasses ? 1 : 0;
         }
      }
   } else {
      // old TAU threshold
      auto ctpinEM  = SG::makeHandle( m_iKeyCtpinEM, context );
      if ( ctpinEM.isValid() ) {
         if ( confThr->cableName() == "TAU1" ) {
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord2(), confThr->cableStart(), confThr->cableEnd() );
         } else if ( confThr->cableName() == "TAU2" ) {
            multiplicity = CTPUtil::getMult( ctpinEM->cableWord3(), confThr->cableStart(), confThr->cableEnd() );
         }
      }
   }
   get2DHist( "/multi/tau/tauMult" )->Fill(confThr->mapping(), multiplicity);
   ATH_MSG_DEBUG("TAU MULT calculated mult for threshold " << confThr->name() << " : " << multiplicity);
   return multiplicity;
}



unsigned int
LVL1CTP::CTPSimulation::calculateMETMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if ( confThr.type() == "XE" or confThr.type() == "TE" or confThr.type() == "XS" ) {
      // old XE, TE, XS
      auto ctpinEnergy = SG::makeHandle( m_iKeyCtpinXE, context );
      if ( ctpinEnergy.isValid() ) {
         if( l1menu->connector("EN1").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("EN1").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord0(), triggerline.startbit(), triggerline.endbit() );
         } else if( l1menu->connector("EN2").hasLine(confThr.name()) ) {
            auto & triggerline = l1menu->connector("EN2").triggerLine(confThr.name());
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord1(), triggerline.startbit(), triggerline.endbit() );
         }
      }
   } else {
      // new XE
      const SG::ReadHandleKey< xAOD::EnergySumRoI > * rhk { nullptr };
      if ( confThr.name().find("gXEPUFIT")==0 ) {
         rhk = & m_iKeyGFexMETPufit;
         ATH_MSG_DEBUG("Using Pufit input for threshold " << confThr.name() );
      } else if ( confThr.name().find("gXERHO")==0 ) {
         rhk = & m_iKeyGFexMETRho;
         ATH_MSG_DEBUG("Using Rho input for threshold " << confThr.name() );
      } else if ( confThr.name().find("gXEJWOJ")==0 ) {
         rhk = & m_iKeyGFexMETJwoJ;
         ATH_MSG_DEBUG("Using JwoJ input for threshold " << confThr.name() );
      } else {
         rhk = & m_iKeyGFexMETJwoJ;
         ATH_MSG_DEBUG("Using default input JwoJ for threshold " << confThr.name() );
      }
      auto met = SG::makeHandle( *rhk, context );
      multiplicity = ( met->energyT()/1000. < confThr.getAttribute<unsigned int>("xe") ) ? 0 : 1; // energyT value is in MeV, cut in GeV
   }
   if(confThr.type() == "TE") {
      get2DHist( "/multi/te/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
      ATH_MSG_DEBUG("TE MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   } else  if(confThr.type() == "XS") {
      get2DHist( "/multi/xs/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
      ATH_MSG_DEBUG("XS MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   } else {
      get2DHist( "/multi/xe/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
      ATH_MSG_DEBUG("XE MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   }
   return multiplicity;
}



unsigned int
LVL1CTP::CTPSimulation::calculateMETMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if ( confThr->name().find("XE")==0 ) {
      // old XE
      auto ctpinEnergy = SG::makeHandle( m_iKeyCtpinXE, context );
      if ( ctpinEnergy.isValid() ) {
         if ( confThr->cableName() == "JEP3" || confThr->cableName() == "EN1") {
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord0(), confThr->cableStart(), confThr->cableEnd() );
         } else if ( confThr->cableName() == "EN2") {
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord1(), confThr->cableStart(), confThr->cableEnd() );
         }
      }
   } else if ( confThr->name().find("TE")==0 ) {
      // old TE 
      auto ctpinEnergy = SG::makeHandle( m_iKeyCtpinXE, context );
      if ( ctpinEnergy.isValid() ) {
         if ( confThr->cableName() == "JEP3" || confThr->cableName() == "EN1") {
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord0(), confThr->cableStart(), confThr->cableEnd() );
         } else if ( confThr->cableName() == "EN2") {
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord1(), confThr->cableStart(), confThr->cableEnd() );
         }
      }
   } else if ( confThr->name().find("XS")==0 ) {
      // old XS
      auto ctpinEnergy = SG::makeHandle( m_iKeyCtpinXE, context );
      if ( ctpinEnergy.isValid() ) {
         if ( confThr->cableName() == "EN1" ) {
            multiplicity = CTPUtil::getMult( ctpinEnergy->cableWord0(), confThr->cableStart(), confThr->cableEnd() );
         }
      }
   } else {
      // new XE
      const SG::ReadHandleKey< xAOD::EnergySumRoI > * rhk { nullptr };
      if ( confThr->name().find("gXEPUFIT")==0 ) {
         rhk = & m_iKeyGFexMETPufit;
         ATH_MSG_DEBUG("Using Pufit input for threshold " << confThr->name() );
      } else if ( confThr->name().find("gXERHO")==0 ) {
         rhk = & m_iKeyGFexMETRho;
         ATH_MSG_DEBUG("Using Rho input for threshold " << confThr->name() );
      } else if ( confThr->name().find("gXEJWOJ")==0 ) {
         rhk = & m_iKeyGFexMETJwoJ;
         ATH_MSG_DEBUG("Using JwoJ input for threshold " << confThr->name() );
      } else {
         rhk = & m_iKeyGFexMETJwoJ;
         ATH_MSG_DEBUG("Using default input JwoJ for threshold " << confThr->name() );
      }
      auto met = SG::makeHandle( *rhk, context );
      multiplicity = ( met->energyT()/1000. < confThr->thresholdValueVector()[0]->ptcut() ) ? 0 : 1; // energyT value is in MeV, cut in GeV
   }
   if ( confThr->name().find("TE")==0 ) {
      get2DHist( "/multi/te/teMult" )->Fill(confThr->mapping(), multiplicity);
   } else if ( confThr->name().find("XE")==0 ) {
      get2DHist( "/multi/xe/xeMult" )->Fill(confThr->mapping(), multiplicity);
   } else {
      get2DHist( "/multi/xs/xsMult" )->Fill(confThr->mapping(), multiplicity);
   }
   ATH_MSG_DEBUG("XE/TE/XS MULT calculated mult for threshold " << confThr->name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateMuonMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   auto ctpinMuon  = SG::makeHandle( m_iKeyMuctpi, context );
   if ( ctpinMuon.isValid() ) {
      auto & triggerline = l1menu->connector("MuCTPiOpt0").triggerLine(confThr.name());
      multiplicity = CTPUtil::getMult( ctpinMuon->muCTPIWord(), triggerline.startbit(), triggerline.endbit() );
   }
   get2DHist( "/multi/muon/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("MU MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateMuonMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   auto ctpinMuon  = SG::makeHandle( m_iKeyMuctpi, context );
   if ( ctpinMuon.isValid() ) {
      multiplicity = CTPUtil::getMult( ctpinMuon->muCTPIWord(), confThr->cableStart(), confThr->cableEnd() );
   }
   get2DHist( "/multi/muon/muonMult" )->Fill(confThr->mapping(), multiplicity);
   ATH_MSG_DEBUG("MU MULT calculated mult for threshold " << confThr->name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTopoMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   // topo
   auto topoInput = SG::makeHandle( m_iKeyTopo, context );
   if(topoInput.isValid()) {
      uint64_t cable = 0;
      std::string conn("");
      if( l1menu->connector("LegacyTopo0").hasLine(confThr.name()) ) {
         conn = "LegacyTopo0";
         cable = ( (uint64_t)topoInput->cableWord1( 1 ) << 32) + topoInput->cableWord1( 0 );
      } else if( l1menu->connector("LegacyTopo1").hasLine(confThr.name()) ) {
         conn = "LegacyTopo1";
         cable = ( (uint64_t)topoInput->cableWord2( 1 ) << 32) + topoInput->cableWord2( 0 );
      }
      if(conn != "") {
         auto & triggerline = l1menu->connector(conn).triggerLine(confThr.name());
         ATH_MSG_DEBUG( " ---> Topo input " << confThr.name() << " on module " << conn
                        << ", cable start " << triggerline.startbit() << " and end " << triggerline.endbit()
                        << " double word 0x" << std::setw(16) << std::setfill('0') << std::hex << cable << std::dec << std::setfill(' ') );
         multiplicity = CTPUtil::getMultTopo( cable, triggerline.startbit(), triggerline.endbit(), triggerline.clock() );
      }
   }
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTopoMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   // topo
   auto topoInput = SG::makeHandle( m_iKeyTopo, context );
   if(topoInput.isValid()) {
      uint64_t cable = 0;
      if ( confThr->cableName() == "TOPO1" ) {
         cable = ( (uint64_t)topoInput->cableWord1( 1 ) << 32) + topoInput->cableWord1( 0 );
      } else {
         cable = ( (uint64_t)topoInput->cableWord2( 1 ) << 32) + topoInput->cableWord2( 0 );
      }
      ATH_MSG_DEBUG( " ---> Topo input " << confThr->name() << " on module " << confThr->cableName() << " with clock " << confThr->clock()
                     << ", cable start " << confThr->cableStart() << " and end " << confThr->cableEnd()
                     << " double word 0x" << std::setw(16) << std::setfill('0') << std::hex << cable << std::dec << std::setfill(' ') );
      multiplicity = CTPUtil::getMultTopo( cable, confThr->cableStart(), confThr->cableEnd(), confThr->clock() );
   }
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateMultiplicity( const TrigConf::TriggerThreshold * confThr, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if( confThr->cableName() == "CTPCAL" || 
       confThr->cableName() == "ALFA" || 
       confThr->cableName() == "NIM1" || 
       confThr->cableName() == "NIM2" || 
       confThr->type() == "NIM") {
      return 0;
   }
   // here we need to write specific code for the different types of L1 FEX objects
   if ( confThr->ttype() == TrigConf::L1DataDef::EM ) {
      multiplicity = calculateEMMultiplicity( confThr, context );
   } else if ( confThr->ttype() == TrigConf::L1DataDef::TAU ) {
      multiplicity = calculateTauMultiplicity( confThr, context );
   } else if ( confThr->ttype() == TrigConf::L1DataDef::XE ||
               confThr->ttype() == TrigConf::L1DataDef::TE ||
               confThr->ttype() == TrigConf::L1DataDef::XS ) {
      multiplicity = calculateMETMultiplicity( confThr, context );
   } else if ( confThr->ttype() == TrigConf::L1DataDef::JET ) {
      multiplicity = calculateJetMultiplicity( confThr, context );
   } else if ( confThr->ttype() == TrigConf::L1DataDef::MUON ) {
      multiplicity = calculateMuonMultiplicity( confThr, context );
   } else if ( confThr->ttype() == TrigConf::L1DataDef::TOPO ) {
      multiplicity = calculateTopoMultiplicity( confThr, context );
   }
   return multiplicity;
}

unsigned int
LVL1CTP::CTPSimulation::calculateMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   try {
      if ( confThr.type() == "EM" ) {
         multiplicity = calculateEMMultiplicity( confThr, l1menu, context );
      } else if ( confThr.type() == "TAU" ) {
         multiplicity = calculateTauMultiplicity( confThr, l1menu, context );
      } else if ( confThr.type() == "XE" || confThr.type() == "TE" || confThr.type() == "XS" ) {
         multiplicity = calculateMETMultiplicity( confThr, l1menu, context );
      } else if ( confThr.type() == "JET" ) {
         multiplicity = calculateJetMultiplicity( confThr, l1menu, context );
      } else if ( confThr.type() == "MU" ) {
         multiplicity = calculateMuonMultiplicity( confThr, l1menu, context );
      } else if ( confThr.type() == "TOPO" ) {
         multiplicity = calculateTopoMultiplicity( confThr, l1menu, context );
      }
   }
   catch(std::exception & ex) {
      ATH_MSG_FATAL("Caught exception when calculating multiplicity for threshold " << confThr.name() << ": " << ex.what());
      throw;
   }
   // all other thresholds are not simulated
   return multiplicity;
}

StatusCode
LVL1CTP::CTPSimulation::simulateItems(const std::map<std::string, unsigned int> & thrMultiMap,
                                     const EventContext& context) const
{

   std::vector<uint32_t> tip;
   CHECK( m_resultBuilder->constructTIPVector( thrMultiMap, tip ) );

   std::map<std::string, unsigned int> itemDecisionMap;
   CHECK( m_resultBuilder->buildItemDecision(thrMultiMap, itemDecisionMap) );

   std::vector<uint32_t> tbp;
   std::vector<uint32_t> tap;
   std::vector<uint32_t> tav;
   unsigned char triggerType(0);
   CHECK( m_resultBuilder->constructResultVectors( itemDecisionMap, tbp, tap, tav, triggerType ) );

   auto eventID = context.eventID();
   std::unique_ptr<CTP_RDO> rdo = m_resultBuilder->constructRDOResult( eventID, tbp, tap, tav, tip );
   std::unique_ptr<CTPSLink> roi = m_resultBuilder->constructRoIResult( eventID, tbp, tap, tav, tip, triggerType );

   // create CTP output format and store in the event
   auto rdoWriteHandle = SG::makeHandle( m_oKeyRDO, context );
   auto sLinkWriteHandle = SG::makeHandle( m_oKeySLink, context );
   ATH_CHECK( rdoWriteHandle.record( std::move(rdo) ));
   ATH_CHECK( sLinkWriteHandle.record( std::move(roi)  ));

   {
      auto tbpById = *get1DHist( "/output/tbpById" );
      auto tapById = *get1DHist( "/output/tapById" );
      auto tavById = *get1DHist( "/output/tavById" );
      for( unsigned int ctpId= 0; ctpId < 512; ++ctpId ) {
         unsigned int wordNr = ctpId / 32;
         unsigned int posWithinWord = ctpId % 32;
         auto mask = 1L << posWithinWord;
         if( 0 != (tbp[wordNr] & mask) ) {
            tbpById->Fill( ctpId );
         }
         if( 0 != (tap[wordNr] & mask) ) {
            tapById->Fill( ctpId );
         }
         if( 0 != (tav[wordNr] & mask) ) {
            tavById->Fill( ctpId );
         }
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::finalize() {

   const TrigConf::L1Menu * l1menu = nullptr;
   if( m_useNewConfig ) {
      ATH_CHECK( m_detStore->retrieve(l1menu) );
   }


   constexpr unsigned int sizeOfCTPOutput = 512;

   unsigned int tbp[sizeOfCTPOutput];
   unsigned int tap[sizeOfCTPOutput];
   unsigned int tav[sizeOfCTPOutput];
   {
      auto h = *get1DHist( "/output/tbpById" );
      for( unsigned int id = 0; id < sizeOfCTPOutput; ++id ) tbp[id] = h->GetBinContent(id+1);
   }
   {
      auto h = *get1DHist( "/output/tapById" );
      for( unsigned int id = 0; id < sizeOfCTPOutput; ++id ) tap[id] = h->GetBinContent(id+1);
   }
   {
      auto h = *get1DHist( "/output/tavById" );
      for( unsigned int id = 0; id < sizeOfCTPOutput; ++id ) tav[id] = h->GetBinContent(id+1);
   }


   // fill the byName TAV histograms from the byID ones
   {
      auto htbp = *get1DHist( "/output/tbpByName" );
      auto htap = *get1DHist( "/output/tapByName" );
      auto htav = *get1DHist( "/output/tavByName" );
      if(l1menu) {
         for( auto & item : *l1menu ) {
            unsigned int ctpId = item.ctpId();
            htbp->Fill( item.name().c_str(), tbp[ctpId]);
            htap->Fill( item.name().c_str(), tap[ctpId]);
            htav->Fill( item.name().c_str(), tav[ctpId]);
         }
      } else {
         for(const TriggerItem * item : m_configSvc->ctpConfig()->menu().items()) {
            unsigned int ctpId = item->ctpId();
            htbp->Fill( item->name().c_str(), tbp[ctpId]);
            htap->Fill( item->name().c_str(), tap[ctpId]);
            htav->Fill( item->name().c_str(), tav[ctpId]);
         }
      }
      htbp->Sumw2(0);
      htap->Sumw2(0);
      htav->Sumw2(0);
      htbp->LabelsDeflate();
      htap->LabelsDeflate();
      htav->LabelsDeflate();

   }

   StatusCode sc(StatusCode::SUCCESS);

   // fill the threshold summary hists
   {
      // run 2 thresholds (legacy + muon)
      std::vector<std::string> thrHists;
      if( l1menu ) {
         thrHists = { "em/EM", "muon/MU", "tau/TAU", "jet/JET", "xe/XE", "te/TE", "xs/XS" };
      } else {
         thrHists = { "em/em", "muon/muon", "tau/tau", "jet/jet", "xe/xe", "te/te", "xs/xs" };
      }
      TH2* hist = new TH2I( "LegacyMult", "Legacy thresholds multiplicity", 1, 0, 1, 10, 0, 10);
      for(const std::string & histpath : thrHists) {
         auto h = * get2DHist( "/multi/" + histpath + "Mult" );
         auto xaxis = h->GetXaxis();
         size_t xsize = xaxis->GetNbins();
         size_t ysize = h->GetNbinsY();
         for(size_t x = 1; x<xsize; x++) {
            std::string s("");
            for(size_t y = 1; y<=ysize; y++) {
               size_t binContent = h->GetBinContent(x,y);
               hist->Fill(xaxis->GetBinLabel(x),y-1,binContent);
               s += std::to_string(binContent) + " ";
            }
            ATH_MSG_DEBUG( "REGTEST CTPSim " << xaxis->GetBinLabel(x) << " MULT " << s);
         }
      }
      hist->Sumw2(0);
      hist->LabelsDeflate();
      sc = hbook( "/multi/all", std::unique_ptr<TH2>(hist));
   }
   {
      // run 3 thresholds
      TH2* hist = new TH2I( "R3Mult", "New thresholds multiplicity", 1, 0, 1, 10, 0, 10);
      if( l1menu ) {
         std::vector<std::string> thrHists = { "em/eEM", "muon/MU", "tau/eTAU", "jet/jJ", "jet/gJ", "xe/gXE", "xe/jXE" };
         for(const std::string & histpath : thrHists) {
            auto h = * get2DHist( "/multi/" + histpath + "Mult" );
            auto xaxis = h->GetXaxis();
            size_t xsize = xaxis->GetNbins();
            size_t ysize = h->GetNbinsY();
            for(size_t x = 1; x<=xsize; x++) {
               std::string s("");
               for(size_t y = 1; y<=ysize; y++) {
                  size_t binContent = h->GetBinContent(x,y);
                  hist->Fill(xaxis->GetBinLabel(x) ,y-1, binContent);
                  s += std::to_string(binContent) + " ";
               }
               ATH_MSG_DEBUG( "REGTEST CTPSim " << xaxis->GetBinLabel(x) << " MULT " << s);
            }
         }
      }
      hist->Sumw2(0);
      hist->LabelsDeflate();
      sc = hbook( "/multi/all", std::unique_ptr<TH2>(hist));
   }

   if(l1menu) {
      for( auto & item : *l1menu ) {
         ATH_MSG_DEBUG( "REGTEST CTPSim " << item.name() << " " << item.ctpId() <<
                        " TBP " << tbp[item.ctpId()] <<
                        " TAP " << tap[item.ctpId()] <<
                        " TAV " << tav[item.ctpId()]);
      }
   } else {
      for(const TriggerItem * item : m_configSvc->ctpConfig()->menu().items()) {
         ATH_MSG_DEBUG( "REGTEST CTPSim " << item->name() << " " << item->ctpId() <<
                        " TBP " << tbp[item->ctpId()] <<
                        " TAP " << tap[item->ctpId()] <<
                        " TAV " << tav[item->ctpId()]);
      }
   }
   return sc;
}
