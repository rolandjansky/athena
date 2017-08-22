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
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"
#include "TrigT1Interfaces/IMuctpiSimTool.h"

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
   m_MuctpiSimTool("LVL1MUCTPI::L1MuctpiTool/LVL1MUCTPI__L1MuctpiTool"),
   m_muonROILocation( LVL1MUCTPI::DEFAULT_MuonRoIBLocation ),
   m_MuonEncoding(0),
   m_MuCTPItoL1TopoLocation ("/Run/L1MuCTPItoL1TopoLocation")
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "ROIBResultLocation", m_roibLocation, "Storegate key for the reading the ROIBResult" );
   declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
   declareProperty( "RecRpcRoiSvc", m_recRPCRoiSvc, "RPC Rec Roi Service");
   declareProperty( "RecTgcRoiSvc", m_recTGCRoiSvc, "TGC Rec Roi Service");
   declareProperty( "MuonROILocation", m_muonROILocation, "Storegate key for the Muon ROIs" );

   declareProperty( "MuonEncoding", m_MuonEncoding = 0, "0=full granularity Mu ROIs, 1=MuCTPiToTopo granularity");
   declareProperty( "locationMuCTPItoL1Topo", m_MuCTPItoL1TopoLocation = "/Run/L1MuCTPItoL1TopoLocation", "Storegate key for MuCTPItoL1Topo ");
   declareProperty( "MuctpiSimTool", m_MuctpiSimTool,"Tool for MUCTPIsimulation");
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
  
   // get MuctpiTool handle
   ATH_MSG_DEBUG("Retrieving MuctpiToolHandle " << m_MuctpiSimTool);
   CHECK( m_MuctpiSimTool.retrieve() );

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

TCS::MuonTOB
MuonInputProvider::createMuonTOB(const MuCTPIL1TopoCandidate & roi) const {


  ATH_MSG_DEBUG("Muon ROI (MuCTPiToTopo): thr ID = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << roi.getphi()  
                << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec );
  ATH_MSG_DEBUG("                            Oct = " << roi.getMioctID() << " etacode=" <<  roi.getetacode() << " phicode= " <<  
                 roi.getphicode()<< ", Sector="<< roi.getSectorName() );

   // The L1 topo hardware works with phi in [0,2pi]. The MuCTPi give muons in [0,2pi].
   // However, L1 topo simulation works with [-pi, pi] and otherwise it crashes. Thus we have to convert here
    int etaTopo = roi.getieta();
    int phiTopo = roi.getiphi();
    if( phiTopo > 31 ) phiTopo -= 64;
    unsigned int pt = topoMuonPtThreshold(roi);
    const unsigned int iso = 0;
    TCS::MuonTOB muon( pt, iso, etaTopo, phiTopo, roi.getRoiID() );
    muon.setEtaDouble( etaTopo );
    muon.setPhiDouble( phiTopo );

   m_hPt->Fill(pt);
   m_hEtaPhi->Fill(muon.eta(),muon.phi());

   return muon;
}

TCS::LateMuonTOB
MuonInputProvider::createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const {

   float phi = roi.getphi();
   if(phi<-M_PI) phi+=2.0*M_PI;
   if(phi> M_PI) phi-=2.0*M_PI;
   unsigned int pt = topoMuonPtThreshold(roi);
   const unsigned int iso = 0;
   ATH_MSG_DEBUG("Late Muon ROI (MuCTPiToTopo):bcid=1 thr pt = " << pt << " eta = " << roi.geteta() << " phi = " << phi << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec);

   TCS::LateMuonTOB muon( pt, iso, int(10*roi.geteta()), int(10*phi), roi.getRoiID() );

   muon.setEtaDouble( roi.geteta() );
   muon.setPhiDouble( phi );

   m_hPt->Fill(pt);
   m_hEtaPhi->Fill(muon.eta(),muon.phi());

   ATH_MSG_DEBUG("LateMuon created");
   return muon;
}

StatusCode
MuonInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

  if( m_MuonEncoding == 0 ) {

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

	if( !( muonRoI.roIWord() & LVL1::CandidateVetoMask  )  )
        inputEvent.addMuon(MuonInputProvider::createMuonTOB( muonRoI.roIWord() ));
    // overflow implemented only for reduced granularity encoding (see below)
	else
	  ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " <<  muonRoI.roIWord() );
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
       
	if( !( roiword & LVL1::CandidateVetoMask  )  )
        inputEvent.addMuon(MuonInputProvider::createMuonTOB( roiword ));
	else
	  ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " << roiword );
       
      }
     
    }
  } else {  // reduced granularity encoding
    ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs.");

    // first see if L1Muctpi simulation already ran and object is in storegate, if not
    // call tool version of the L1MuctpiSimulation and create it on the fly
    if( evtStore()->contains<LVL1::MuCTPIL1Topo>(m_MuCTPItoL1TopoLocation) ) {
      LVL1::MuCTPIL1Topo* l1topo  {nullptr};
      CHECK( evtStore()->retrieve( l1topo, m_MuCTPItoL1TopoLocation ) );      
      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: retrieve from SG");

      // std::cout << "TW: MuonInputProvider print l1topo candidates from SG" << std::endl; 
      // l1topo->print();

      std::vector<MuCTPIL1TopoCandidate> candList = l1topo->getCandidates();
      for(  std::vector<MuCTPIL1TopoCandidate>::const_iterator iMuCand = candList.begin(); iMuCand != candList.end(); iMuCand++)
	{
	  //MuonInputProvider::createMuonTOB( *iMuCand );
        inputEvent.addMuon(MuonInputProvider::createMuonTOB( *iMuCand ));
      if(iMuCand->moreThan2CandidatesOverflow()){
          inputEvent.setOverflowFromMuonInput(true);
          ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from SG)");
      }
	}
    } else {

      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: calculate from ROIs sent to RoIB");
      LVL1::MuCTPIL1Topo l1topo;
      CHECK(m_MuctpiSimTool->fillMuCTPIL1Topo(l1topo));

      // std::cout << "TW: MuonInputProvider print l1topo candidates from Tool" << std::endl; 
      // l1topo.print();
      
            
      for( const MuCTPIL1TopoCandidate & cand : l1topo.getCandidates() ) {
          inputEvent.addMuon(MuonInputProvider::createMuonTOB( cand ));
          if(cand.moreThan2CandidatesOverflow()){
              inputEvent.setOverflowFromMuonInput(true);
              ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from MuctpiSimTool)");
          }
      }
    }
    
    //BC+1
    // first see if L1Muctpi simulation already ran and object is in storegate, if not
    // call tool version of the L1MuctpiSimulation and create it on the fly
    std::vector<MuCTPIL1TopoCandidate> candList;
    if( evtStore()->contains<LVL1::MuCTPIL1Topo>(m_MuCTPItoL1TopoLocation.toString()+std::to_string(1)) ) {
      LVL1::MuCTPIL1Topo* l1topoBC1  {nullptr};
      CHECK( evtStore()->retrieve( l1topoBC1,m_MuCTPItoL1TopoLocation.toString()+std::to_string(1)));
      ATH_MSG_DEBUG( "Contains L1Topo LateMuons L1Muctpi object from StoreGate!" );
      //      l1topoBC1->print();
      candList = l1topoBC1->getCandidates();
    }else{
      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: calculate from ROIs sent to ROS");
      LVL1::MuCTPIL1Topo l1topoBC1;
      int bcidOffset = 1;
      CHECK(m_MuctpiSimTool->fillMuCTPIL1Topo(l1topoBC1, bcidOffset));
      l1topoBC1.setBcidOffset(bcidOffset);
      //l1topoBC1.print();
      candList = l1topoBC1.getCandidates();
    }  
    
    for(  std::vector<MuCTPIL1TopoCandidate>::const_iterator iMuCand = candList.begin(); iMuCand != candList.end(); iMuCand++)
      {
	//or would it be better to create muon and dynamic_cast?
	ATH_MSG_DEBUG("MuonInputProvider addLateMuon ");
	inputEvent.addLateMuon( MuonInputProvider::createLateMuonTOB( *iMuCand ) );	   
      }
  }
  
  return StatusCode::SUCCESS;
}

unsigned int MuonInputProvider::topoMuonPtThreshold(const MuCTPIL1TopoCandidate &mctpiCand) const
{
    unsigned int threshold = 0;
    for(const TrigConf::TriggerThreshold* thr : m_MuonThresholds) {
        if(static_cast< unsigned int >(thr->thresholdNumber()) == mctpiCand.getptL1TopoCode()) {
            const int ieta = 0;
            const int iphi = 0;
            threshold = thr->triggerThresholdValue(ieta, iphi)->ptcut();
            break;
        }
    }
    return threshold;
}
