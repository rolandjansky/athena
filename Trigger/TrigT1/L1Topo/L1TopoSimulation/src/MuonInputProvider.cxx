/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInputProvider.h"

#include <math.h>
#include "TH1.h"
#include "TH2.h"

#include "GaudiKernel/ITHistSvc.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/MuCTPIToRoIBSLink.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"

using namespace std;
using namespace LVL1;

MuonInputProvider::MuonInputProvider( const std::string& type, const std::string& name, 
                                      const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
   m_recRPCRoiSvc( LVL1::ID_RecRpcRoiSvc, name ),
   m_recTGCRoiSvc( LVL1::ID_RecTgcRoiSvc, name ),
   m_muonROILocation( LVL1MUCTPI::DEFAULT_MuonRoIBLocation )
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "ROIBResultLocation", m_roibLocation, "Storegate key for the reading the ROIBResult" );
   declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
   declareProperty( "RecRpcRoiSvc", m_recRPCRoiSvc, "RPC Rec Roi Service");
   declareProperty( "RecTgcRoiSvc", m_recTGCRoiSvc, "TGC Rec Roi Service");
   declareProperty( "MuonROILocation", m_muonROILocation, "Storegate key for the Muon ROIs" );

}

MuonInputProvider::~MuonInputProvider()
{}


StatusCode
MuonInputProvider::initialize() {

   ATH_MSG_DEBUG("Retrieving LVL1ConfigSvc " << m_configSvc);
   CHECK( m_configSvc.retrieve() );

   ATH_MSG_DEBUG("Retrieving RPC RoI Service " << m_recRPCRoiSvc);
   CHECK( m_recRPCRoiSvc.retrieve() );

   ATH_MSG_DEBUG("Retrieving TGC RoI Service " << m_recTGCRoiSvc);
   CHECK( m_recTGCRoiSvc.retrieve() );

   m_MuonThresholds = m_configSvc->ctpConfig()->menu().thresholdConfig().getThresholdVector(TrigConf::L1DataDef::MUON);

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "MuonInputProvider");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();
  
   return StatusCode::SUCCESS;
}


void
MuonInputProvider::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   m_hPt = new TH1I( "MuonTOBPt", "Muon TOB Pt", 40, 0, 200);
   m_hPt->SetXTitle("p_{T}");

   m_hEtaPhi = new TH2I( "MuonTOBPhiEta", "Muon TOB Location", 25, -50, 50, 32, -32, 32);
   m_hEtaPhi->SetXTitle("#eta");
   m_hEtaPhi->SetYTitle("#phi");

   m_histSvc->regHist( histPath + "TOBPt", m_hPt ).ignore();
   m_histSvc->regHist( histPath + "TOBPhiEta", m_hEtaPhi ).ignore();
}


TCS::MuonTOB
MuonInputProvider::createMuonTOB(uint32_t roiword) const {
   LVL1::RecMuonRoI roi( roiword, m_recRPCRoiSvc.operator->(), m_recTGCRoiSvc.operator->(), &m_MuonThresholds );

   ATH_MSG_DEBUG("Muon ROI: thrvalue = " << roi.getThresholdValue() << " eta = " << roi.eta() << " phi = " << roi.phi() << ", w   = " << MSG::hex << std::setw( 8 ) << roi.roiWord() << MSG::dec);
         
   TCS::MuonTOB muon( roi.getThresholdValue(), 0, int(10*roi.eta()), int(10*roi.phi()), roi.roiWord() );
   muon.setEtaDouble( roi.eta() );
   muon.setPhiDouble( roi.phi() );

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());
   
   return muon;
}

StatusCode
MuonInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   ATH_MSG_DEBUG("Filling the muon input from MuCTPIToRoIBSLink produced by L1Muctpi.cxx.");

   const ROIB::RoIBResult* roibResult {nullptr};

   const L1MUINT::MuCTPIToRoIBSLink* muctpi_slink {nullptr};

   if( evtStore()->contains<L1MUINT::MuCTPIToRoIBSLink>("/Run/L1MuCTPItoRoIBLocation") ) {
      CHECK( evtStore()->retrieve( muctpi_slink, "/Run/L1MuCTPItoRoIBLocation" ) );
   } else if( evtStore()->contains<ROIB::RoIBResult>(m_roibLocation) ) {
      CHECK( evtStore()->retrieve(roibResult, m_roibLocation) );
   } else {
      ATH_MSG_WARNING("Neither a MuCTPIToRoIBSLink with SG key '/Run/L1MuCTPItoRoIBLocation' nor a an RoIBResult were found in the event. No muon input for the L1Topo simulation.");
      return StatusCode::RECOVERABLE;
   }

   if( roibResult ) {

      const std::vector< ROIB::MuCTPIRoI >& rois = roibResult->muCTPIResult().roIVec();

      ATH_MSG_DEBUG("Filling the input event from RoIBResult. Number of Muon ROIs: " << rois.size() );

      for( const ROIB::MuCTPIRoI & muonRoI : rois ) {

         inputEvent.addMuon( MuonInputProvider::createMuonTOB( muonRoI.roIWord() ) );

      }

   } else if( muctpi_slink ) {

      ATH_MSG_DEBUG("Filling the input event. Number of Muon ROIs: " << muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Header::wordsPerHeader - ROIB::Trailer::wordsPerTrailer - 1);

      unsigned int icnt = 0;
      for ( unsigned int roiword : muctpi_slink->getMuCTPIToRoIBWords() ) {

         ++icnt;
         // skip header
         if ( icnt <= ROIB::Header::wordsPerHeader + 1 )
            continue;

         // skip trailer
         if ( icnt > ( muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Trailer::wordsPerTrailer ) )
            continue;

         inputEvent.addMuon( MuonInputProvider::createMuonTOB( roiword ) );

      }

   }





   return StatusCode::SUCCESS;
}
