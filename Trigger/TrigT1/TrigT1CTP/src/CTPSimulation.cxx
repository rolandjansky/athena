/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./CTPSimulation.h"
#include "./CTPTriggerThreshold.h"
#include "./CTPTriggerItem.h"
#include "./CTPUtil.h"

#include "TrigConfData/L1BunchGroupSet.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Interfaces/CTPSLink.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/JetEnergyRoI.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1CaloUtils/CoordToHardware.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "AthenaMonitoringKernel/HistogramDef.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"

#include "TTree.h"

#include <iomanip>
#include <algorithm>
#include <cmath>

namespace {
  static std::mutex s_metadataMutex;
}


using namespace TrigConf;

const std::function< CLHEP::HepRandomEngine*(void) > CTPSimRanluxFactory = [](void)->CLHEP::HepRandomEngine*{
   return new CLHEP::Ranlux64Engine();
};


LVL1CTP::CTPSimulation::CTPSimulation( const std::string& name, ISvcLocator* pSvcLocator ) :
   AthReentrantAlgorithm ( name, pSvcLocator ),
   m_RNGEngines( CTPSimRanluxFactory, SG::getNSlots() ),
   m_decoder( new LVL1::CPRoIDecoder() ),
   m_jetDecoder( new LVL1::JEPRoIDecoder() )
{}

LVL1CTP::CTPSimulation::~CTPSimulation()
{}

StatusCode
LVL1CTP::CTPSimulation::initialize() {

   ATH_MSG_DEBUG("initialize");
   if(m_forceBunchGroupPattern) {
      ATH_MSG_INFO("Will use bunchgroup pattern 0x" << std::hex << m_bunchGroupPattern);
   } else {
      ATH_MSG_INFO("Will use bunchgroup definition from bunchgroup set");
   }

   if( m_isData ) {
      CHECK( m_oKeyRDO.assign(LVL1CTP::DEFAULT_RDOOutputLocation_Rerun) );
      CHECK( m_oKeySLink.assign(LVL1CTP::DEFAULT_CTPSLinkLocation_Rerun) );
   }

   // data links
   ATH_CHECK( m_iKeyTopo.initialize( !m_iKeyTopo.empty() ) );
   ATH_CHECK( m_iKeyMuctpi.initialize( ! m_iKeyMuctpi.empty() ) );

   // Legacy L1Calo 
   ATH_CHECK( m_iKeyLegacyTopo.initialize( !m_iKeyLegacyTopo.empty() &&  m_doL1CaloLegacy ) );
   ATH_CHECK( m_iKeyCtpinEM.initialize( m_doL1CaloLegacy ) );
   ATH_CHECK( m_iKeyCtpinJet.initialize( m_doL1CaloLegacy ) );
   ATH_CHECK( m_iKeyCtpinXE.initialize( m_doL1CaloLegacy ) );

   // new L1Calo
   ATH_CHECK( m_iKeyJFexJets.initialize( ! m_iKeyJFexJets.empty() ) );
   ATH_CHECK( m_iKeyJFexLJets.initialize( ! m_iKeyJFexLJets.empty() ) );
   ATH_CHECK( m_iKeyGFexJets.initialize( ! m_iKeyGFexJets.empty() ) );
   ATH_CHECK( m_iKeyGFexMETNC.initialize( ! m_iKeyGFexMETNC.empty() ) );
   ATH_CHECK( m_iKeyGFexMETRho.initialize( ! m_iKeyGFexMETRho.empty() ) );
   ATH_CHECK( m_iKeyGFexMETJwoJ.initialize( ! m_iKeyGFexMETJwoJ.empty() ) );
   ATH_CHECK( m_iKeyEFexCluster.initialize( ! m_iKeyEFexCluster.empty() ) );
   ATH_CHECK( m_iKeyEFexTau.initialize( ! m_iKeyEFexTau.empty() ) );
   ATH_CHECK( m_oKeyRDO.initialize( ! m_oKeyRDO.empty() ) );
   ATH_CHECK( m_oKeySLink.initialize( ! m_oKeySLink.empty() ) );

   // services
   ATH_CHECK( m_histSvc.retrieve() );

   // tools
   ATH_CHECK( m_resultBuilder.retrieve() );

   ATH_MSG_DEBUG("Registering histograms under " << getBaseHistPath() << " in " << m_histSvc);

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::start() {

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) );

   bookHists().ignore();

   // configure the CTP ResultBuilder
   ATH_CHECK( m_resultBuilder->setConfiguration( *l1menu ) );
   setHistLabels(*l1menu).ignore();

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::execute( const EventContext& context ) const {

   ATH_MSG_DEBUG("execute");

   fillInputHistograms(context).ignore();

   std::map<std::string, unsigned int> thrMultiMap;

   extractMultiplicities(thrMultiMap, context).ignore();

   simulateItems(thrMultiMap, context).ignore();

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::createMultiplicityHist(const std::string & type, unsigned int maxMult ) const {

   StatusCode sc;
   std::map<std::string,std::vector<std::string>> typeMapping = {
      { "muon", {"MU"} },
      { "jet", {"JET", "jJ", "jLJ", "gJ", "gLJ"} },
      { "xe", {"XE", "gXE", "jXE"} },
      { "te", {"TE", "jTE", "gTE"} },
      { "xs", {"XS"} },
      { "em", {"EM", "eEM", "jEM"} },
      { "tau", {"TAU", "eTAU", "jTAU", "cTAU"} }
   };
   std::vector<TrigConf::L1Threshold> thrV;
   for( const std::string & t : typeMapping[type] ) {
      size_t xsize = 1;
      TH2* hist = new TH2I( Form("%sMult", t.c_str()),
                            Form("%s threshold multiplicity", t.c_str()), xsize, 0, xsize, maxMult, 0, maxMult);
      sc = hbook( "/multi/" + type, std::unique_ptr<TH2>(hist));
   }

   return sc;
}

StatusCode
LVL1CTP::CTPSimulation::setMultiplicityHistLabels(const TrigConf::L1Menu& l1menu, const std::string & type) const {
   StatusCode sc;
   std::map<std::string,std::vector<std::string>> typeMapping = {
      { "muon", {"MU"} },
      { "jet", {"JET", "jJ", "jLJ", "gJ", "gLJ"} },
      { "xe", {"XE", "gXE", "jXE"} },
      { "te", {"TE", "jTE", "gTE"} },
      { "xs", {"XS"} },
      { "em", {"EM", "eEM", "jEM"} },
      { "tau", {"TAU", "eTAU", "jTAU", "cTAU"} }
   };


   std::vector<TrigConf::L1Threshold> thrV;
   for( const std::string & t : typeMapping[type] ) {
     try {
       auto hist = get2DHist( "/multi/" + type + "/" + t + "Mult" );
       auto & thrV = l1menu.thresholds(t);
       while( hist->GetNbinsX() < (int)thrV.size() ) {
	 hist->LabelsInflate("xaxis");
       }
       for(auto thr : thrV) {
	 hist->GetXaxis()->SetBinLabel(thr->mapping()+1, thr->name().c_str() );
       }
     } catch (std::exception & ex) {
       ATH_MSG_DEBUG("Caught exception when setting new JSON MultiplicityHistLabel " << t << " : " << ex.what());
     }
   }
   return sc;
}

std::string
LVL1CTP::CTPSimulation::getBaseHistPath() const {
   std::string baseHistPath( m_histPath );
   if(baseHistPath.back()!='/') baseHistPath += "/";
   baseHistPath += name();
   return baseHistPath;
}

StatusCode
LVL1CTP::CTPSimulation::hbook(const std::string & path, std::unique_ptr<TH1> hist) const {
   const std::string & hname(hist->GetName());

   std::string key(path);
   if(key.back()!='/') key += "/";
   key+=hname;

   if(m_hist1D.find(key)!=m_hist1D.end()) {
      ATH_MSG_WARNING("1D-hist " << key << " already exists, won't register it");
      return StatusCode::RECOVERABLE;
   }

   LockedHandle<TH1> lh;
   StatusCode sc = m_histSvc->regShared( getBaseHistPath() + key, std::move(hist), lh );
   if( sc.isSuccess() ) {
      ATH_MSG_DEBUG("1D-hist " << hname << " registered with key - " << key);
      m_hist1D[key] = lh;
   } else {
      ATH_MSG_WARNING("Could not register histogram " << hname);
   }
   return sc;
}

StatusCode
LVL1CTP::CTPSimulation::hbook(const std::string & path, std::unique_ptr<TH2> hist) const {
   const std::string & hname(hist->GetName());

   std::string key(path);
   if(key.back()!='/') key += "/";
   key+=hname;

   if(m_hist2D.find(key)!=m_hist2D.end()) {
      ATH_MSG_WARNING("2D-hist " << key << " already exists, won't register it");
      return StatusCode::RECOVERABLE;
   }

   LockedHandle<TH2> lh;
   StatusCode sc = m_histSvc->regShared( getBaseHistPath() + key, std::move(hist), lh );
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
LVL1CTP::CTPSimulation::setHistLabels(const TrigConf::L1Menu& l1menu) const {

   ATH_MSG_DEBUG("setHistLabels(). L1 menu " << l1menu.size() << " items" );
   // threshold multiplicities
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "muon" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "jet" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "xe" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "te" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "xs" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "em" ) );
   ATH_CHECK ( setMultiplicityHistLabels( l1menu, "tau" ) );

   // Topo
   std::vector<std::string> connNames = l1menu.connectorNames();
   for( const std::string connName : {"LegacyTopo0", "LegacyTopo1", "Topo1El", "Topo2El", "Topo3El", "Topo1Opt0", "Topo1Opt1", "Topo1Opt2", "Topo1Opt3"}) {
      if( find(connNames.begin(), connNames.end(), connName) == connNames.end() ) {
         continue;
      }
      auto hTopo = *get1DHist("/input/topo/" + connName);
      for(uint fpga : {0,1}) {
         for(uint clock : {0,1}) {
            for(auto & tl : l1menu.connector(connName).triggerLines(fpga,clock)) {
               uint flatIndex = tl.flatindex(); 
               hTopo->GetXaxis()->SetBinLabel(flatIndex+1,tl.name().c_str());
            }
         }
      }
   }

   std::vector<std::string> orderedItemnames;
   orderedItemnames.reserve( l1menu.size() );
   for( const auto & item : l1menu ) {
      orderedItemnames.emplace_back(item.name());
   }
   std::sort(orderedItemnames.begin(), orderedItemnames.end());

   // item decision
   auto tbpById = *get1DHist( "/output/tbpById" );
   auto tapById = *get1DHist( "/output/tapById" );
   auto tavById = *get1DHist( "/output/tavById" );
   auto tbpByName = *get1DHist( "/output/tbpByName" );
   auto tapByName = *get1DHist( "/output/tapByName" );
   auto tavByName = *get1DHist( "/output/tavByName" );
   unsigned int bin = 1;
   for ( const std::string & itemname : orderedItemnames ) {
      unsigned int ctpId(0);
      TrigConf::L1Item item = l1menu.item( itemname );
      ctpId = item.ctpId();
      tbpById->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tapById->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tavById->GetXaxis()->SetBinLabel( ctpId+1, itemname.c_str() );
      tbpByName->GetXaxis()->SetBinLabel( bin, itemname.c_str() );
      tapByName->GetXaxis()->SetBinLabel( bin, itemname.c_str() );
      tavByName->GetXaxis()->SetBinLabel( bin++, itemname.c_str() );
   }

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::bookHists() const {

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
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gLJetPt","Jet p_{T} - gLJ", 40, 0, 80) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gLJetEta","Jet #eta - gLJ", 64, -3.2, 3.2) ));
   ATH_CHECK ( hbook( "/input/jets/", std::make_unique<TH1I>("gLJetPhi","Jet #phi - gLJ", 64, -3.2, 3.2) ));

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
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("gLJets","Number of jets (gLJ)", 40, 0, 40) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("muons","Number of muons", 10, 0, 10) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("emcluster","Number of EM clusters", 20, 0, 20) ));
   ATH_CHECK ( hbook( "/input/counts/", std::make_unique<TH1I>("taus","Number of TAU candidates", 20, 0, 20) ));

   // threshold multiplicities
   ATH_CHECK ( createMultiplicityHist( "muon" ) );
   ATH_CHECK ( createMultiplicityHist( "jet" ) );
   ATH_CHECK ( createMultiplicityHist( "xe", 2) );
   ATH_CHECK ( createMultiplicityHist( "te", 2) );
   ATH_CHECK ( createMultiplicityHist( "xs", 2) );
   ATH_CHECK ( createMultiplicityHist( "em" ) );
   ATH_CHECK ( createMultiplicityHist( "tau" ) );

   ATH_CHECK ( hbook( "/multi/all", (std::unique_ptr<TH2>)std::make_unique<TH2I>("LegacyMult", "Legacy thresholds multiplicity", 1, 0, 1, 10, 0, 10) ));
   ATH_CHECK ( hbook( "/multi/all", (std::unique_ptr<TH2>)std::make_unique<TH2I>("R3Mult", "New thresholds multiplicity", 1, 0, 1, 10, 0, 10) ));

   // Topo
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("LegacyTopo0","L1Topo Decision (Legacy 0)", 64, 0, 64) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("LegacyTopo1","L1Topo Decision (Legacy 1)", 64, 0, 64) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo1El","L1Topo Decision (Topo 1 electrical)", 64, 0, 64) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo2El","L1Topo Decision (Topo 2 electrical)", 64, 0, 64) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo3El","L1Topo Decision (Topo 3 electrical)", 64, 0, 64) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo1Opt0","L1Topo Decision (Topo 1 optical 0)", 128, 0, 128) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo1Opt1","L1Topo Decision (Topo 1 optical 1)", 128, 0, 128) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo1Opt2","L1Topo Decision (Topo 1 optical 2)", 128, 0, 128) ));
   ATH_CHECK( hbook( "/input/topo/", std::make_unique<TH1I>("Topo1Opt3","L1Topo Decision (Topo 1 optical 3)", 128, 0, 128) ));

   // item decision
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tbpById", "Items decision (tbp)", 512, 0, 512) ));
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tapById", "Items decision (tap)", 512, 0, 512) ));
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tavById", "Items decision (tav)", 512, 0, 512) ));
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tbpByName", "Items decision (tbp)", 512, 0, 512) ));
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tapByName", "Items decision (tap)", 512, 0, 512) ));
   ATH_CHECK ( hbook( "/output/", std::make_unique<TH1I>("tavByName", "Items decision (tav)", 512, 0, 512) ));

   ATH_CHECK ( hbook( "/", std::make_unique<TH1I>("bcid", "Bunch crossing ID", 3564, 0, 3564)) );

   ATH_CHECK(storeMetadata());

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::fillInputHistograms(const EventContext& context) const {

   ATH_MSG_DEBUG( "fillInputHistograms" );

   // jFEX jets
   if( not m_iKeyJFexJets.empty() ) {
      auto jFexJets  = SG::makeHandle( m_iKeyJFexJets, context );
      if(jFexJets.isValid()) {
         get1DHist("/input/counts/jJets")->Fill(jFexJets->size());
         auto h0 = *get1DHist("/input/jets/jJetPt"); // calling operator* to get the Guard outside the filling loop
         auto h1 = *get1DHist("/input/jets/jJetEta");
         auto h2 = *get1DHist("/input/jets/jJetPhi");
         for( const auto jet : *jFexJets ) {
            h0->Fill(fabs(jet->et8x8()/1000.));
            h1->Fill(jet->eta());
            h2->Fill(jet->phi());
         }
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyJFexJets);
      }
   }

   // jFEX large-R jets
   if( not m_iKeyJFexLJets.empty() ) {
      auto jFexLJets = SG::makeHandle( m_iKeyJFexLJets, context );
      if(jFexLJets.isValid()) {
         get1DHist("/input/counts/jLets")->Fill(jFexLJets->size());
         auto h0 = *get1DHist("/input/jets/jLJetPt");
         auto h1 = *get1DHist("/input/jets/jLJetEta");
         auto h2 = *get1DHist("/input/jets/jLJetPhi");
         for( const auto jet : *jFexLJets ) {
            h0->Fill(fabs(jet->et8x8()/1000.));
            h1->Fill(jet->eta());
            h2->Fill(jet->phi());
         }
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyJFexLJets);
      }
   } else {
      ATH_MSG_DEBUG("No collection " << m_iKeyJFexLJets);
   }

   // gFEX jets
   if( not m_iKeyGFexJets.empty() ) {
      auto gFexJets  = SG::makeHandle( m_iKeyGFexJets, context );
      if(gFexJets.isValid()) {
         get1DHist("/input/counts/gJets")->Fill(gFexJets->size());
         auto h0 = *get1DHist("/input/jets/gJetPt");
         auto h1 = *get1DHist("/input/jets/gJetEta");
         auto h2 = *get1DHist("/input/jets/gJetPhi");
         for( const auto jet : *gFexJets ) {
            h0->Fill(fabs(jet->et8x8()/1000.));
            h1->Fill(jet->eta());
            h2->Fill(jet->phi());
         }
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyGFexJets);
      }
   }

   // MET
   if( not m_iKeyGFexMETNC.empty() ) {
      auto gFexMETPufit  = SG::makeHandle( m_iKeyGFexMETNC, context );
      if( gFexMETPufit.isValid() ) {
         get1DHist("/input/met/Pufit")->Fill(gFexMETPufit->energyT()/1000.);
         get1DHist("/input/met/PufitPhi")->Fill(atan2(gFexMETPufit->energyX(), gFexMETPufit->energyY()));
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETNC);
      }
   }

   if( not m_iKeyGFexMETRho.empty() ) {
      auto gFexMETRho  = SG::makeHandle( m_iKeyGFexMETRho, context );
      if( gFexMETRho.isValid() ) {
         get1DHist("/input/met/Rho")->Fill(gFexMETRho->energyT()/1000.);
         get1DHist("/input/met/RhoPhi")->Fill(atan2(gFexMETRho->energyX(), gFexMETRho->energyY()));
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETRho);
      }
   }

   if( not m_iKeyGFexMETJwoJ.empty() ) {
      auto gFexMETJwoJ  = SG::makeHandle( m_iKeyGFexMETJwoJ, context );
      if( gFexMETJwoJ.isValid() ) {
         get1DHist("/input/met/JwoJ")->Fill(gFexMETJwoJ->energyT()/1000.);
         get1DHist("/input/met/JwoJPhi")->Fill(atan2(gFexMETJwoJ->energyX(), gFexMETJwoJ->energyY()));
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyGFexMETJwoJ);
      }
   }

   // EM cluster
   if( not m_iKeyEFexCluster.empty() ) {
      auto eFexCluster  = SG::makeHandle( m_iKeyEFexCluster, context );
      if( eFexCluster.isValid() ) {
         get1DHist( "/input/counts/emcluster")->Fill(eFexCluster->size());
         auto h0 = *get1DHist("/input/em/et");
         auto h1 = *get1DHist("/input/em/eta");
         auto h2 = *get1DHist("/input/em/phi");
         for( const auto cl : *eFexCluster ) {
            h0->Fill(cl->et());
            h1->Fill(cl->eta());
            h2->Fill(cl->phi());
         }
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyEFexCluster);
      }
   }

   // eFEX Tau
   if( not m_iKeyEFexTau.empty() ) {
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
         for( const auto tau : *eFexTau ) {
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
   }

   // topo
   if( not m_iKeyLegacyTopo.empty() &&  m_doL1CaloLegacy) {
      auto legacyTopoInput = SG::makeHandle( m_iKeyLegacyTopo, context );
      if(legacyTopoInput.isValid()) {
         ATH_MSG_DEBUG("Retrieved input from L1Topo from StoreGate with key " << m_iKeyTopo);
         ATH_MSG_DEBUG("L1TopoLegacy0 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << legacyTopoInput->cableWord1(0));
         ATH_MSG_DEBUG("L1TopoLegacy0 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << legacyTopoInput->cableWord1(1));
         ATH_MSG_DEBUG("L1TopoLegacy1 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << legacyTopoInput->cableWord2(0));
         ATH_MSG_DEBUG("L1TopoLegacy1 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << legacyTopoInput->cableWord2(1));
         auto h0 = *get1DHist("/input/topo/LegacyTopo0");
         auto h1 = *get1DHist("/input/topo/LegacyTopo1");
         for(unsigned int i=0; i<32; ++i) {
            uint32_t mask = 0x1; mask <<= i;
            if( (legacyTopoInput->cableWord1(0) & mask) != 0 ) h0->Fill(i); // cable 0, clock 0
            if( (legacyTopoInput->cableWord1(1) & mask) != 0 ) h0->Fill(32 + i); // cable 0, clock 1
            if( (legacyTopoInput->cableWord2(0) & mask) != 0 ) h1->Fill(i); // cable 1, clock 0
            if( (legacyTopoInput->cableWord2(1) & mask) != 0 ) h1->Fill(32 + i); // cable 1, clock 1
         }
      }      
   }

   if( not m_iKeyTopo.empty() ) {
      auto topoInput = SG::makeHandle( m_iKeyTopo, context );
      if(topoInput.isValid()) {
         ATH_MSG_DEBUG("Retrieved input from L1Topo from StoreGate with key " << m_iKeyTopo);
         ATH_MSG_DEBUG("L1Topo0 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord1(0));
         ATH_MSG_DEBUG("L1Topo0 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord1(1));
         ATH_MSG_DEBUG("L1Topo1 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord2(0));
         ATH_MSG_DEBUG("L1Topo1 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << topoInput->cableWord2(1));
         auto h0 = *get1DHist("/input/topo/Topo2El");
         auto h1 = *get1DHist("/input/topo/Topo3El");
         for(unsigned int i=0; i<32; ++i) {
            uint32_t mask = 0x1; mask <<= i;
            if( (topoInput->cableWord1(0) & mask) != 0 ) h0->Fill(i); // cable 0, clock 0
            if( (topoInput->cableWord1(1) & mask) != 0 ) h0->Fill(32 + i); // cable 0, clock 1
            if( (topoInput->cableWord2(0) & mask) != 0 ) h1->Fill(i); // cable 1, clock 0
            if( (topoInput->cableWord2(1) & mask) != 0 ) h1->Fill(32 + i); // cable 1, clock 1
         }
         auto h2 = *get1DHist("/input/topo/Topo1Opt0");
         auto h3 = *get1DHist("/input/topo/Topo1Opt1");
         auto h4 = *get1DHist("/input/topo/Topo1Opt2");
         auto h5 = *get1DHist("/input/topo/Topo1Opt3");
         for(unsigned int i=0; i<128; i += 3) {
           std::bitset<128> mask = 0x11; mask <<= i;
           if( (topoInput->optcableWord("Topo1Opt0") & mask) != 0 ) h2->Fill(i); //
           if( (topoInput->optcableWord("Topo1Opt1") & mask) != 0 ) h3->Fill(i); //
           if( (topoInput->optcableWord("Topo1Opt2") & mask) != 0 ) h4->Fill(i); //
           if( (topoInput->optcableWord("Topo1Opt3") & mask) != 0 ) h5->Fill(i); //
         }
      } else {
         ATH_MSG_DEBUG("No collection " << m_iKeyTopo);
      }
   }

   // bcid
   auto bcid = context.eventID().bunch_crossing_id();
   get1DHist( "/bcid")->Fill(bcid);

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::extractMultiplicities(std::map<std::string, unsigned int> & thrMultiMap, const EventContext& context) const {

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) );

   thrMultiMap.clear();

   std::vector<std::string> connNames = l1menu->connectorNames();
   for( const std::string connName : {"LegacyTopo0", "LegacyTopo1", "Topo1El", "Topo2El", "Topo3El", "Topo1Opt0", "Topo1Opt1", "Topo1Opt2", "Topo1Opt3"}) {
      if( find(connNames.begin(), connNames.end(), connName) == connNames.end() ) {
         continue;
      }
      uint64_t cable {0};
      if (connName.find("Legacy")==0) { // legacy topo
         if (m_iKeyLegacyTopo.empty() || !m_doL1CaloLegacy )
         {
            continue;
         }
         auto topoInput = SG::makeHandle( m_iKeyLegacyTopo, context );
         if (not topoInput.isValid()) {
            continue;
         }
         if(connName == "LegacyTopo0") {
            cable = ( (uint64_t)topoInput->cableWord1( 1 ) << 32) + topoInput->cableWord1( 0 );
         } else if (connName == "LegacyTopo1") {
            cable = ( (uint64_t)topoInput->cableWord2( 1 ) << 32) + topoInput->cableWord2( 0 );
         }
      } else { // new topo
         if (m_iKeyTopo.empty())
         {
            continue;
         }
         auto topoInput = SG::makeHandle( m_iKeyTopo, context );
         if (not topoInput.isValid()) {
            continue;
         }
         if(connName == "Topo1El") {
            cable = ( (uint64_t)topoInput->cableWord0( 1 ) << 32) + topoInput->cableWord0( 0 );
         } else if(connName == "Topo2El") {
            cable = ( (uint64_t)topoInput->cableWord1( 1 ) << 32) + topoInput->cableWord1( 0 );
         } else if (connName == "Topo3El") {
            cable = ( (uint64_t)topoInput->cableWord2( 1 ) << 32) + topoInput->cableWord2( 0 );
         } else if(connName == "Topo1Opt0") {
            ATH_MSG_DEBUG("BIT word Topo1Opt0: " << topoInput->optcableWord( connName ));
         } else if(connName == "Topo1Opt1") {
            ATH_MSG_DEBUG("BIT word Topo1Opt1: " << topoInput->optcableWord( connName ));
         } else if(connName == "Topo1Opt2") {
            ATH_MSG_DEBUG("BIT word Topo1Opt2: " << topoInput->optcableWord( connName ));
         } else if(connName == "Topo1Opt3") {
            ATH_MSG_DEBUG("BIT word Topo1Opt3: " << topoInput->optcableWord( connName ));
         }
      }
      auto & conn = l1menu->connector(connName);
      for(uint fpga : {0,1}) {
         for(uint clock : {0,1}) {
            for(auto & tl : conn.triggerLines(fpga,clock)) {
               uint flatIndex = tl.flatindex();
               uint pass = (cable & (uint64_t(0x1) << flatIndex)) == 0 ? 0 : 1;
               if(size_t pos = tl.name().find('['); pos == std::string::npos) {
                  thrMultiMap[tl.name()] = pass;
                  ATH_MSG_DEBUG(tl.name() << " MULT calculated mult for topo " << pass);
               } else {
                  auto thrName = tl.name().substr(0,pos);
                  int bit = std::stoi(tl.name().substr(pos+1));
                  thrMultiMap.try_emplace(thrName, 0);
                  thrMultiMap[thrName] += (pass << bit);
                  ATH_MSG_DEBUG(thrName << " MULT updated mult for topo " << pass);
               }
            }
         }
      }
   }
   for ( auto & thr : l1menu->thresholds() ) {
      if(thr->type() == "TOPO" or thr->type()== "R2TOPO" or thr->type() == "MULTTOPO" or thr->type() == "MUTOPO") {
         continue;
      }
      // get the multiplicity for each threshold
      unsigned int multiplicity = calculateMultiplicity( *thr, l1menu, context );
      // and record in threshold--> multiplicity map (to be used for item decision)
      thrMultiMap[thr->name()] = multiplicity;
      ATH_MSG_DEBUG( thr->name()  << " MULT calculated mult for topo " << multiplicity);
   }

   // internal triggers
   auto bcid = context.eventID().bunch_crossing_id();
   if( m_forceBunchGroupPattern ) {
      // force bunch group pattern from job options
      for ( size_t bg = 0; bg < 16; ++bg ) {
         std::string bgName("BGRP");
         bgName += std::to_string(bg);
         thrMultiMap[bgName] = ( m_bunchGroupPattern & (0x1<<bg) ) ? 1 : 0;
      }
   } else {
      // use bunchgroup definition from configuration and pick according to the BCID
      const TrigConf::L1BunchGroupSet *l1bgs = nullptr;
      detStore()->retrieve(l1bgs).ignore();
      if (l1bgs)
      {
         for (size_t i = 0; i < l1bgs->maxNBunchGroups(); ++i)
         {
            std::shared_ptr<TrigConf::L1BunchGroup> bg = l1bgs->getBunchGroup(i);
            thrMultiMap[std::string("BGRP") + std::to_string(i)] = bg->contains(bcid) ? 1 : 0;
         }
      }
      else
      {
         ATH_MSG_ERROR("Did not find L1BunchGroupSet in DetectorStore");
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
      if(m_doL1CaloLegacy) {
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
      }
   }
   get2DHist( "/multi/jet/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("JET MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateEMMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity (0);
   if ( confThr.name()[0]=='e' ) {
      // new EM threshold from eFEX
      if(!m_iKeyEFexCluster.empty()) {
         float scale = l1menu->getObject("thresholds.legacyCalo.EM.emscale").getValue<float>();
         auto eFexCluster = SG::makeHandle( m_iKeyEFexCluster, context );
         for ( const auto cl : *eFexCluster ) {
            float eta = cl->eta();
            int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
            unsigned int thrV = confThr.thrValue( ieta );
            bool clusterPasses = ( ((unsigned int) cl->et()) > (thrV * scale) ); // need to add cut on isolation and other variables, once available
            multiplicity +=  clusterPasses ? 1 : 0;
         }
      }
   } else {
      // old EM threshold from data
      if(m_doL1CaloLegacy) {
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
   }
   get2DHist( "/multi/em/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("EM MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
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
         for ( const auto tau : *eFexTaus ) {
            unsigned int eT = (unsigned int) (accR3ClET(*tau)/1000.); // tau eT is in MeV while the cut is in GeV - this is only temporary and needs to be made consistent for all L1Calo
            //float iso = accR3ClIso(*tau);
            unsigned int etCut = confThr.data().get_child("et").get_value<unsigned int>();
            bool tauPasses = ( eT >= etCut ); // need to add cut on isolation and other variables, once available
            multiplicity +=  tauPasses ? 1 : 0;
         }
      }
   } else {
      // old TAU threshold
      if(m_doL1CaloLegacy) {
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
   }
   get2DHist( "/multi/tau/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("TAU MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateMETMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   if ( confThr.type() == "XE" or confThr.type() == "TE" or confThr.type() == "XS" ) {
      // old XE, TE, XS
      if(m_doL1CaloLegacy) {
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
      }
   } else {
      // new XE
      const SG::ReadHandleKey< xAOD::EnergySumRoI > * rhk { nullptr };
      if ( confThr.name().find("gXENC")==0 ) {
         rhk = & m_iKeyGFexMETNC;
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
      if(!rhk->empty()) {
         auto met = SG::makeHandle( *rhk, context );
         multiplicity = ( met->energyT()/1000. < confThr.getAttribute<unsigned int>("xe") ) ? 0 : 1; // energyT value is in MeV, cut in GeV
      }
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
LVL1CTP::CTPSimulation::calculateMuonMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   if(m_iKeyMuctpi.empty()) {
      return 0;
   }
   unsigned int multiplicity = 0;
   auto ctpinMuon  = SG::makeHandle( m_iKeyMuctpi, context );
   if ( ctpinMuon.isValid() ) {
      auto & triggerline = l1menu->connector("MuCTPiOpt0").triggerLine(confThr.name());
      multiplicity = CTPUtil::getMuonMult( ctpinMuon->muCTPIWord(), triggerline.startbit() + (m_muonRun2Format ? 1 : 0), triggerline.endbit()+ (m_muonRun2Format ? 1 : 0) );
   }
   get2DHist( "/multi/muon/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
   ATH_MSG_DEBUG("MU MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity);
   return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTopoOptMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
  if(m_iKeyTopo.empty()) {
    return 0;
  }
  unsigned int multiplicity = 0;
  auto topoInput = SG::makeHandle( m_iKeyTopo, context );
  std::string connector = "";
  if (topoInput.isValid()) {
    connector = l1menu->connectorNameFromThreshold(confThr.name());
    auto & triggerline = l1menu->connector(connector).triggerLine(confThr.name());
    std::bitset<128> bits = topoInput->optcableWord(connector);
    multiplicity = CTPUtil::getOptMult( bits, triggerline.startbit(), triggerline.endbit() );
  }
  std::string subfolder = "";
  if (confThr.type().find("XE") != std::string::npos) {
    subfolder = "xe";
  } else if (confThr.type().find("TE") != std::string::npos) {
    subfolder = "te";
  } else if (confThr.type().find("TAU") != std::string::npos) {
    subfolder = "tau";
  } else if (confThr.type().find("EM") != std::string::npos) {
    subfolder = "em";
  } else if (confThr.type().find("jJ") != std::string::npos) {
    subfolder = "jet";
  } else if (confThr.type().find("jLJ") != std::string::npos) {
    subfolder = "jet";
  } else if (confThr.type().find("gJ") != std::string::npos) {
    subfolder = "jet";
  } else if (confThr.type().find("gLJ") != std::string::npos) {
    subfolder = "jet";
  }
  get2DHist( "/multi/" + subfolder + "/" + confThr.type() + "Mult" )->Fill(confThr.mapping(), multiplicity);
  ATH_MSG_DEBUG("TOPO OPT input MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity << " received via connector: " << connector);
  //ATH_MSG_INFO("TOPO OPT input MULT calculated mult for threshold " << confThr.name() << " : " << multiplicity << " received via connector: " << connector);
  return multiplicity;
}


unsigned int
LVL1CTP::CTPSimulation::calculateTopoMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   if (m_iKeyTopo.empty())
   {
      return 0;
   }
   unsigned int multiplicity = 0;
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
LVL1CTP::CTPSimulation::calculateMultiplicity( const TrigConf::L1Threshold & confThr, const TrigConf::L1Menu * l1menu, const EventContext& context ) const {
   unsigned int multiplicity = 0;
   try {
     //ATH_MSG_INFO("confThr.type() " << confThr.type() << " ");
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
      } else if ( confThr.type()[0] == 'e' || confThr.type()[0] == 'c' || confThr.type()[0] == 'j' || confThr.type()[0] == 'g' ){
      	 multiplicity = calculateTopoOptMultiplicity( confThr, l1menu, context );
	 //ATH_MSG_INFO("confThr.type() " << confThr.type() << " " << "mult:" << multiplicity);  
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
   CLHEP::HepRandomEngine* rndmEngine = m_RNGEngines.getEngine( context );
   CHECK( m_resultBuilder->buildItemDecision(thrMultiMap, itemDecisionMap, rndmEngine) );

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

   // fill histograms with item simulation results
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
   ATH_CHECK( detStore()->retrieve(l1menu) );

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
      for( auto & item : *l1menu ) {
         unsigned int ctpId = item.ctpId();
         htbp->Fill( item.name().c_str(), tbp[ctpId]);
         htap->Fill( item.name().c_str(), tap[ctpId]);
         htav->Fill( item.name().c_str(), tav[ctpId]);
      }
      htbp->Sumw2(0);
      htap->Sumw2(0);
      htav->Sumw2(0);
      htbp->LabelsDeflate();
      htap->LabelsDeflate();
      htav->LabelsDeflate();

   }

   // fill the threshold summary hists
   {
      // run 2 thresholds (legacy + muon)
      std::vector<std::string> thrHists{ "em/EM", "muon/MU", "tau/TAU", "jet/JET", "xe/XE", "te/TE", "xs/XS" };
      auto hist = * get2DHist( "/multi/all/LegacyMult" );
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
   }
   {
      // run 3 thresholds
      auto hist = * get2DHist( "/multi/all/R3Mult" );
      std::vector<std::string> thrHists = { "em/eEM", "em/jEM", "muon/MU", "tau/eTAU", "tau/jTAU", "tau/cTAU", "jet/jJ", "jet/jLJ", "jet/gJ", "jet/gLJ", "xe/gXE", "xe/jXE", "te/jTE", "te/gTE" };
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
      hist->Sumw2(0);
      hist->LabelsDeflate();
   }

   if ( msgLvl(MSG::DEBUG) ) {
      for( auto & item : *l1menu ) {
         ATH_MSG_DEBUG( "REGTEST CTPSim " << item.name() << " " << item.ctpId() <<
                        " TBP " << tbp[item.ctpId()] <<
                        " TAP " << tap[item.ctpId()] <<
                        " TAV " << tav[item.ctpId()]);
      }
   }
   return StatusCode::SUCCESS;
}

/*
  stores metadata for all histograms to provide merging information
  adapted from @c HistogramFiller/OfflineHistogramProvider.h
*/
StatusCode
LVL1CTP::CTPSimulation::storeMetadata() const {
   std::vector<std::string> storedPaths;
   for( auto & entry : m_hist1D ) {
      storedPaths.push_back( getBaseHistPath() + entry.first);
   }
   for( auto & entry : m_hist2D ) {
      storedPaths.push_back( getBaseHistPath() + entry.first);
   }
   std::scoped_lock<std::mutex> metadataLock(s_metadataMutex);
   for (const auto & path : storedPaths) {
      size_t pos = path.find_last_of('/');
      auto splitPath = std::make_pair(path.substr(0, pos), path.substr(pos + 1));
      std::string treePath = splitPath.first + "/metadata";
      std::string interval("run");
      char triggerData[] = "<none>";
      const std::string mergeDataStr = "<default>";
      std::vector<char> mergeData{mergeDataStr.begin(), mergeDataStr.end()};
      mergeData.push_back('\0');
      interval = "run";
      if (!m_histSvc->existsTree(treePath)) {
         auto tree = std::make_unique<TTree>("metadata", "Monitoring Metadata");
         tree->SetDirectory(nullptr);
         tree->Branch("Name", &(splitPath.second[0]), "Name/C");
         tree->Branch("Interval", &(interval[0]), "Interval/C");
         tree->Branch("TriggerChain", triggerData, "TriggerChain/C");
         tree->Branch("MergeMethod", mergeData.data(), "MergeMethod/C");
         tree->Fill();
         if (!m_histSvc->regTree(treePath, std::move(tree))) {
            MsgStream log(Athena::getMessageSvc(), "OfflineHistogramProvider");
            log << MSG::ERROR
                << "Failed to register DQ metadata TTree " << treePath << endmsg;
         }
      } else {
         TTree *tree{nullptr};
         if (m_histSvc->getTree(treePath, tree).isSuccess()) {
            tree->SetBranchAddress("Name", &(splitPath.second[0]));
            tree->SetBranchAddress("Interval", &(interval[0]));
            tree->SetBranchAddress("TriggerChain", triggerData);
            tree->SetBranchAddress("MergeMethod", mergeData.data());
            tree->Fill();
         } else {
            MsgStream log(Athena::getMessageSvc(), "OfflineHistogramProvider");
            log << MSG::ERROR
                << "Failed to retrieve DQ metadata TTree " << treePath << " which is reported to exist" << endmsg;
         }
      }
   }
   return StatusCode::SUCCESS;
}
