/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTriggerTools/MuonTriggerAnalysisTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGate.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "ParticleEvent/ParticleBaseContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "FourMom/P4PtEtaPhiM.h"

#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "muonEvent/MuonContainer.h"

/// general tools for analysis
#include "AnalysisTools/IAnalysisTools.h"

#include <math.h>

using CLHEP::MeV;
using namespace Analysis;
using namespace Trig;
using namespace std;

#define MUONBARRELLIMIT 1.05

static const InterfaceID IID_IMuonTriggerAnalysisTool("MuonTriggerAnalysisTool", 1, 0);

const InterfaceID& MuonTriggerAnalysisTool::interfaceID( ) { 
  return IID_IMuonTriggerAnalysisTool; 
}
 
// default constructor
MuonTriggerAnalysisTool::MuonTriggerAnalysisTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent), 
     m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
     m_lvl1Tool("HLT::Lvl1ResultAccessTool"),
     m_rpcRoiService( "LVL1RPC::RPCRecRoiSvc", name),
     m_tgcRoiService( "LVL1TGC::TGCRecRoiSvc", name)
 {
  declareInterface< MuonTriggerAnalysisTool  >( this );

  declareProperty("IsInOneBC",m_isInOneBC=false);
  declareProperty("IsInThreeBC",m_isInThreeBC=true);

  declareProperty("MuonLvl1DRMatchBarrel",m_muonLvl1DRMatchBarrel=0.5);
  declareProperty("MuonLvl2DRMatchBarrel",m_muonLvl2DRMatchBarrel=0.05);
  declareProperty("MuonEFDRMatchBarrel",  m_muonEFDRMatchBarrel=0.02);

  declareProperty("MuonLvl1DRMatchEndcap",m_muonLvl1DRMatchEndcap=0.5);
  declareProperty("MuonLvl2DRMatchEndcap",m_muonLvl2DRMatchEndcap=0.05);
  declareProperty("MuonEFDRMatchEndcap",  m_muonEFDRMatchEndcap=0.02);
  
}
 
// destructor   
MuonTriggerAnalysisTool::~MuonTriggerAnalysisTool() {

}

StatusCode MuonTriggerAnalysisTool::initialize() {

  ATH_MSG_DEBUG("In initialize");

  // Initialize store gate service
  StatusCode sc = serviceLocator()->service("StoreGateSvc",m_storeGate);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR("Could not get StoreGateSvc ");
    return sc;
  }

  m_all   = m_trigDec->getChainGroup(".*");
  m_allL1 = m_trigDec->getChainGroup("L1.*");
  m_allL2 = m_trigDec->getChainGroup("L2.*");
  m_allEF = m_trigDec->getChainGroup("EF.*"); 
  

  sc = m_rpcRoiService.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't access RPC RecMuonRoISvc" );
  }
  
  sc = m_tgcRoiService.retrieve();
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Couldn't access TGC RecMuonRoISvc" );
  }  

  // load the RoI map
  if ( m_isInThreeBC ){
    cout << " ** Load Roi Map "<<endl;
    ifstream fileRoI;
    fileRoI.open("ROI_Mapping.dump");
    if(!fileRoI) cout << "Error: file could not be opened" << endl;
    
    int SubSysID;
    int sectorID;
    int roINumber;
    float etamin,etamax,phimin,phimax;
    
    // look on file
    while (! fileRoI.eof()) {
      //if (!fileRoI.good()) break;
      fileRoI >> SubSysID >> sectorID >> roINumber >> etamin
	      >> etamax >> phimin >> phimax;
      
      int mySecId=sectorID+32*SubSysID;
      cout << "  sec = "    << mySecId   << endl;
      cout << "  roi = "    << roINumber << endl;
      cout << "  etamin = " << etamin    <<endl;
      cout << " etamax = "  << etamax    <<endl;

      if (mySecId >= 0 && mySecId < 64 &&
          roINumber >= 0 && roINumber < 30)
      {
        muctpiEta_min[mySecId][roINumber]= etamin;
        muctpiEta_max[mySecId][roINumber]= etamax;
        muctpiPhi_min[mySecId][roINumber]= phimin;
        muctpiPhi_max[mySecId][roINumber]= phimax;
      }
    }
    
    fileRoI.close();
  }

  return sc;
}

StatusCode MuonTriggerAnalysisTool::finalize() {

  return StatusCode::SUCCESS;

}


bool MuonTriggerAnalysisTool::checkMuon( const TruthParticle* truthParticle, 
					 const std::string chain)
{
  double deltaR = 0.0;
  return checkMuon(truthParticle, chain, deltaR);
}

bool MuonTriggerAnalysisTool::checkMuon( const TruthParticle* truthParticle, 
					 const std::string chain,
					 double& deltaR)
{

  ATH_MSG_DEBUG("in checkMuon()");

  std::string trigLevel = chain.substr(0,3);

  P4PtEtaPhiM mom(truthParticle->pt(), truthParticle->eta(), truthParticle->phi(), truthParticle->m() );  

  if (trigLevel=="L1_") {
    map<string, double> output = map<string, double>() ;
    return muonLvl1Match (&mom,  chain, deltaR, output);
  }
  else if (trigLevel=="L2_") {
    return muonLvl2Match (&mom,  chain, deltaR);
  }
  else if (trigLevel=="EF_") {
    return muonEFMatch (&mom,  chain, deltaR);
  }

  ATH_MSG_ERROR("Trigger level not identified for chain: " << chain);
  return false; 

}

bool MuonTriggerAnalysisTool::checkMuon( const Trk::Track* track, 
					 const std::string chain, double& deltaR)
{

  ATH_MSG_DEBUG("in checkMuon()");

  std::string trigLevel = chain.substr(0,3);

  const Trk::Perigee* perigee = track->perigeeParameters();
  if (!perigee) {
    ATH_MSG_DEBUG("No perigee associated to the track ");
    return false;
  }

  const auto & parameters = perigee->parameters();

  P4IPtCotThPhiM mom (fabs(parameters[Trk::qOverP]), 1./tan(parameters[Trk::theta]), parameters[Trk::phi], 105.6/MeV );

  if (trigLevel=="L1_") {
    map<string, double> output = map<string, double>();
    return muonLvl1Match (&mom,  chain, deltaR, output);
  }
  else if (trigLevel=="L2_") {
    return muonLvl2Match (&mom,  chain, deltaR);
  }
  else if (trigLevel=="EF_") {
    return muonEFMatch (&mom,  chain, deltaR);
  }

  ATH_MSG_ERROR("Trigger level not identified for chain: " << chain);
  return false;

}

bool MuonTriggerAnalysisTool::checkMuon( const Trk::Track* track, 
					 const std::string chain )
{
  double deltaR = 0.0;
  return checkMuon(track, chain, deltaR);
}

bool MuonTriggerAnalysisTool::checkMuon( const Rec::TrackParticle* particle, 
					 const std::string chain )
{
  double deltaR = 0.0;
  map<string, double> output = map<string, double>();
  return checkMuon(particle, chain, deltaR, output);
}

bool MuonTriggerAnalysisTool::checkMuon( const Rec::TrackParticle* particle, 
					 const std::string chain, double& deltaR )
{
  map<string, double> output = map<string, double>();
  return checkMuon(particle, chain, deltaR, output);
}

bool MuonTriggerAnalysisTool::checkMuon( const Rec::TrackParticle* particle, 
					 const std::string chain, double& deltaR,
					 map<string, double>& output)
{
  ATH_MSG_DEBUG("in checkMuon()");

  std::string trigLevel = chain.substr(0,3);

  P4PtEtaPhiM mom(particle->pt(), particle->eta(), particle->phi(), particle->m() );  

  if (trigLevel=="L1_") {
    return muonLvl1Match (&mom,  chain, deltaR, output);
  }
  else if (trigLevel=="L2_") {
    return muonLvl2Match (&mom,  chain, deltaR);
  }
  else if (trigLevel=="EF_") {
    return muonEFMatch (&mom,  chain, deltaR);
  }

  ATH_MSG_ERROR("Trigger level not identified for chain: " << chain);

  return false;
}


bool MuonTriggerAnalysisTool::muonLvl1Match( const I4Momentum* particle,
					     const std::string chain, 
					     double& deltaR,
					     std::map<std::string, double>& output)
{

  ATH_MSG_DEBUG("in muonLvl1Match()");

  bool matchFound = false;
  double dRMin = 999.;

  double etaTrue = particle->eta();
  double phiTrue = particle->phi();


  if ( m_isInThreeBC ){
    const MuCTPI_RDO* muCTPI_RDO;
    std::string  muCPTILocation ="MUCTPI_RDO";
    StatusCode sc = m_storeGate->retrieve( muCTPI_RDO, muCPTILocation );
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not find MUCTPI_RDO at " << muCPTILocation);
      return false;
    }
    
    if (muCTPI_RDO) {
      
      std::vector< uint32_t > dataWord = muCTPI_RDO->dataWord();
      std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;
      //m_nMuctpi = dataWord.size();
      output["ROIsize"] = dataWord.size();
      output["ROIeta"] = -999.9;
      output["ROIphi"] = -999.9;
      output["ROIpt"] = -999.9;
      output["ROInumber"] = -999.9;
      output["ROIsectorID"] = -999.9;
      output["ROIsectorLocation"] = -999.9;
      output["ROIsectorAddress"] = -999.9;
      output["ROIhemisphere"] = -999.9;
      output["ROIbcid"] = -999.9;
      output["ROIcandidateIsHighestPt"] = -999.9;
      output["ROIsent"] = -999.9;
      output["ROIsectorOverflow"] = -999.9;
      output["ROIoverflow"] = -999.9;
      output["ROIoverlapBits"] = -999.9;
      for ( std::vector< uint32_t >::const_iterator it = dataWord.begin(); it != dataWord.end(); ++it ) {
	uint32_t     daqWord = *it;
	unsigned int data = ( daqWord & 0x3fff ) | ( ( daqWord >> 3 ) & 0x7fc000 );
	LVL1::RecMuonRoI recRoi( data, &( *m_rpcRoiService ), &( *m_tgcRoiService ), &dummy_thresholds );

	MuCTPI_DataWord_Decoder daqWordDecod = MuCTPI_DataWord_Decoder( daqWord );
	//uint16_t   roiBCIDWord = daqWordDecod.getBCID();
	//unsigned int roiBCID   = ( roiBCIDWord & 0x7 );
	int pt = daqWordDecod.getPt();
	ATH_MSG_DEBUG(" Treshold pt: " << pt );
	if ( (chain=="L1_MU0" && pt > 0) || 
	     (chain=="L1_MU6" && pt > 1) || 
	     (chain=="L1_MU10" && pt > 2) ) {

	  // get roi sector location (BARREL = 0, ENDCAP = 1, FORWARD = 2)
	  MuCTPI_RDO::SectorLocation sectorLocation = daqWordDecod.getSectorLocation();

	  // get roi sector id and number
	  uint16_t sectorID = daqWordDecod.getSectorID() ;
	  uint16_t number = daqWordDecod.getRoiNumber() ; 

	  // get roi eta and phi
	  double eta = recRoi.eta();
	  double phi = recRoi.phi();

	  // get additional info
	  uint16_t sectorAddress = daqWordDecod.getSectorAddress() ;
	  uint16_t hemisphere = daqWordDecod.getHemisphere();
	  uint16_t bcid = daqWordDecod.getBCID();
	  uint16_t candidateIsHighestPt = daqWordDecod.getCandidateIsHighestPt();
	  uint16_t sent = daqWordDecod.getSentRoi();
	  uint16_t sectorOverflow = daqWordDecod.getSectorOverflow();
	  uint16_t overflow = daqWordDecod.getRoiOverflow();
	  uint16_t overlapBits = daqWordDecod.getOverlapBits();

	  if ( sectorLocation == 0 ) {
	    // barrel -> remapping due to cabling problem
	    eta = ( muctpiEta_max[sectorID][number] + muctpiEta_min[sectorID][number] )/2;
	    phi = ( muctpiPhi_max[sectorID][number] + muctpiPhi_min[sectorID][number] )/2;
	    ATH_MSG_DEBUG( " MuCTPI entry (corrected): eta=" << eta << " phi=" << phi );
	  }
	  else {
	    // ec -> no remapping
	    ATH_MSG_DEBUG( " MuCTPI entry: eta=" << eta << " phi=" << phi );
	  }

	  // check matching
	  double deta = eta-etaTrue;
	  double dphi = phi-phiTrue;
	  if (fabs(dphi)>M_PI) dphi = 2.*M_PI-fabs(dphi);

	  double dR = sqrt(deta*deta+dphi*dphi);
	  ATH_MSG_DEBUG(" DeltaR: " <<  dR );
	  if (dR<dRMin) {
	    dRMin = dR;
	    output["ROIeta"] = eta;
	    output["ROIphi"] = phi;
	    output["ROIpt"] = pt;
	    output["ROInumber"] = sectorID;
	    output["ROIsectorID"] = number;
	    output["ROIsectorLocation"] = sectorLocation;
	    output["ROIsectorAddress"] = sectorAddress;
	    output["ROIhemisphere"] = hemisphere;
	    output["ROIbcid"] = bcid;
	    output["ROIcandidateIsHighestPt"] = candidateIsHighestPt;
	    output["ROIsent"] = sent;
	    output["ROIsectorOverflow"] = sectorOverflow;
	    output["ROIoverflow"] = overflow;
	    output["ROIoverlapBits"] = overlapBits;
	  }

	}
      }
      
    }
  } else if ( m_isInOneBC ){
    // retrieve lvl1 ROIs
    // retrieve the lvl1 result container
    const LVL1_ROI* lvl1Roi;
    StatusCode sc = m_storeGate->retrieve(lvl1Roi,"LVL1_ROI");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Can't retrieve the LVL1_ROIs");
      return false;
    }
    
    
    // loop on the rois to find a matching one
    // get the electron ROIs
    if (lvl1Roi) {
      LVL1_ROI::muons_type::const_iterator it = (lvl1Roi->getMuonROIs()).begin();
      LVL1_ROI::muons_type::const_iterator it_end = (lvl1Roi->getMuonROIs()).end();
      

      std::string threshold = chain.substr(3,8);
      
      ATH_MSG_DEBUG("Checking muon L1 ROIs for chain, threshold: " << chain << " " << threshold);
      
      for ( ; it != it_end ; ++it ) {
	
	ATH_MSG_DEBUG("RoI threshold: " << it->getThrName());
	
	if ( 
	    
	    (chain=="L1_MU20" && (it->getThrName() == threshold )) ||
	    (chain=="L1_MU15" && (it->getThrName() == threshold || it->getThrName()=="MU20" )) ||
	    (chain=="L1_MU0_COMM" && (it->getThrName() == threshold || it->getThrName()=="MU15" || it->getThrName()=="MU20")) ||
	    (chain=="L1_MU10" && (it->getThrName() == threshold || it->getThrName()=="MU0_COMM" || it->getThrName()=="MU15" || it->getThrName()=="MU20")) ||
	    (chain=="L1_MU6"  && (it->getThrName() == threshold || it->getThrName()=="MU10" || it->getThrName()=="MU0_COMM" || it->getThrName()=="MU15" || it->getThrName()=="MU20")) || 
	    (chain=="L1_MU0"  && (it->getThrName() == threshold || it->getThrName()=="MU6"  || it->getThrName()=="MU10" || it->getThrName()=="MU0_COMM" || it->getThrName()=="MU15" || it->getThrName()=="MU20"))
	    


//	    (chain=="L1_MU40" && (it->getThrName() == threshold)) ||
//	    (chain=="L1_MU20" && (it->getThrName() == threshold || it->getThrName()=="MU40")) ||
//	    (chain=="L1_MU15" && (it->getThrName() == threshold || it->getThrName()=="MU20" || it->getThrName()=="MU40")) ||
//	    (chain=="L1_MU11" && (it->getThrName() == threshold || it->getThrName()=="MU15" || it->getThrName()=="MU20" || it->getThrName()=="MU40")) ||
//	    (chain=="L1_MU10" && (it->getThrName() == threshold || it->getThrName()=="MU11" || it->getThrName()=="MU15" || it->getThrName()=="MU20" || it->getThrName()=="MU40")) ||
//	    (chain=="L1_MU6"  && (it->getThrName() == threshold || it->getThrName()=="MU10" || it->getThrName()=="MU11" || it->getThrName()=="MU15" || it->getThrName()=="MU20" || it->getThrName()=="MU40")) || 
//	    (chain=="L1_MU4"  && (it->getThrName() == threshold || it->getThrName()=="MU6"  || it->getThrName()=="MU10" || it->getThrName()=="MU11" || it->getThrName()=="MU15" || it->getThrName()=="MU20" || it->getThrName()=="MU40")) ||
//	    (chain=="L1_MU0"  && (it->getThrName() == threshold || it->getThrName()=="MU4" || it->getThrName()=="MU6"  || it->getThrName()=="MU10" || it->getThrName()=="MU11" || it->getThrName()=="MU15" || it->getThrName()=="MU20" || it->getThrName()=="MU40"))
//	    

	    ) 
	  {
	    
	    double deta = it->eta()-etaTrue;
	    double dphi = it->phi()-phiTrue;
	    if (fabs(dphi)>M_PI) dphi = 2.*M_PI-fabs(dphi);
	    
	    double dR = sqrt(deta*deta+dphi*dphi);
	    
	    ATH_MSG_DEBUG("RoI eta, phi: " << it->eta() << " " << it->phi() << "  Muon eta, phi: " << etaTrue 
			  << " " << phiTrue);
	    
	    if (dR<dRMin) {
	      dRMin = dR;
	    }
	    
	  }
	
      }
    }
    
   
  }
  
  // fill histos for muons satifying the chain
  if ( (fabs(etaTrue)<= MUONBARRELLIMIT && dRMin<m_muonLvl1DRMatchBarrel) ||
       (fabs(etaTrue)> MUONBARRELLIMIT && dRMin<m_muonLvl1DRMatchEndcap) ) {

    ATH_MSG_DEBUG("Found a matching feature for chain: " << chain);
    matchFound = true;

  }
  
  deltaR = dRMin;

  return matchFound;
}


bool MuonTriggerAnalysisTool::muonLvl2Match( const I4Momentum* particle, 
					     const std::string chain, 
					     double& deltaR )
{

  ATH_MSG_DEBUG("in muonLvl2Match()");

  bool matchFound = false;
  double dRMin = 999.;

  //double ptTrue =  particle->pt();
  double etaTrue = particle->eta();
  double phiTrue = particle->phi();

  const Trig::FeatureContainer f = m_trigDec->features(chain);

  ATH_MSG_DEBUG("chain: " << chain);
  ATH_MSG_DEBUG("checking RoIs " << f.getCombinations().size());


  // get the flat vector of features
  std::vector<Feature<CombinedMuonFeature> > featureVector = f.get<CombinedMuonFeature>();
  std::vector<Feature<CombinedMuonFeature> >::const_iterator fIt = featureVector.begin();

  // loop on the trigger elements, retrieve features
  for ( ; fIt != featureVector.end() ; ++fIt) {
    
    // retrieve the features
    const CombinedMuonFeature* cbMuon = (*fIt);
    
    if (cbMuon) {
      
      double dEta = cbMuon->eta()-etaTrue;
      double dPhi = cbMuon->phi()-phiTrue;
      if (fabs(dPhi)>M_PI) dPhi = 2.*M_PI-fabs(dPhi);
      
      double dR = sqrt(dEta*dEta+dPhi*dPhi);
      
      if (dR<dRMin) {
	dRMin=dR;
      } 
      
    }
  }

  // fill histos for muons satifying the chain
  if ( (fabs(etaTrue)<= MUONBARRELLIMIT && dRMin<m_muonLvl2DRMatchBarrel) ||
       (fabs(etaTrue)> MUONBARRELLIMIT && dRMin<m_muonLvl2DRMatchEndcap) ) {

    ATH_MSG_DEBUG("Found a matching feature for chain, index : " << chain);
    matchFound = true;
  }  

  deltaR = dRMin;
  return matchFound;
}

bool MuonTriggerAnalysisTool::muonEFMatch( const I4Momentum* particle, 
					   const std::string chain, 
					   double& deltaR )
{

  ATH_MSG_DEBUG("in muonEFMatch()");

  bool matchFound = false;
  double dRMin = 999.;

  //double pt =  particle->pt();
  double eta = particle->eta();
  double phi = particle->phi();

  //retrieve featuresContainer
  const Trig::FeatureContainer f = m_trigDec->features(chain);

  Trig::FeatureContainer::combination_const_iterator cIt;

  ATH_MSG_DEBUG("chain: " << chain);
  ATH_MSG_DEBUG(" checking RoIs, size of the combination vector " 
      << f.getCombinations().size());

  //  const std::string label = "MuonEF";
  //const std::vector<Trig::Feature<TrigMuonEFInfoContainer> > muContainerVector = f.get<TrigMuonEFInfoContainer>();
	 
  //loop on the feature container
  for ( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
  //
    ATH_MSG_DEBUG(" checking specific RoI ");
    
    // combination can be printed
    ATH_MSG_DEBUG(*cIt);
       
    // get the flat vector of features
    std::vector<Trig::Feature<TrigMuonEFInfoContainer> > muContainerVector = cIt->get<TrigMuonEFInfoContainer>();
    //std::vector<Trig::Feature<TrigMuonEFContainer> >::const_iterator fIt = muContainerVector.begin();

    if(muContainerVector.size()==0){
      ATH_MSG_DEBUG("Vector of TrigMuonEFContainer is empty!");
      return false;
    }
    
    const TrigMuonEFInfoContainer* muContainer = muContainerVector[0].cptr();
    
    //loop on feature container vector
    if (muContainer) {
      ATH_MSG_DEBUG("loop on muContainer of size: " << muContainer->size());
      // loop on the muon objects
      for (const TrigMuonEFInfo* muinfo : *muContainer) {

        const TrigMuonEFInfoTrackContainer* tracks = muinfo->TrackContainer();
        if (!tracks || tracks->empty()) continue;
        const TrigMuonEFInfoTrack* t = tracks->front();
	const TrigMuonEFCbTrack* trigMuonEF = t->CombinedTrack();
 
	double dEta = trigMuonEF->eta()-eta;
	double dPhi = trigMuonEF->phi()-phi;

	if (fabs(dPhi)>M_PI) dPhi = 2.*M_PI-fabs(dPhi);
	double dR = sqrt(dEta*dEta+dPhi*dPhi);
	
	ATH_MSG_DEBUG("deltaR: " << dR);
	if (dR<dRMin) {
	  dRMin = dR;
	}	
	
      }
      
      // fill histos for muons satifying the chain
      if ( (fabs(eta)<= MUONBARRELLIMIT && dRMin<m_muonEFDRMatchBarrel) ||
	   (fabs(eta)> MUONBARRELLIMIT && dRMin<m_muonEFDRMatchEndcap) ) {
	
	ATH_MSG_DEBUG("Found a matching feature for chain, index : " << chain);
	matchFound = true;
      }    
      
    }
  }
  deltaR=dRMin;
  
  ATH_MSG_DEBUG("match found: " << matchFound);
  return matchFound;
}


bool MuonTriggerAnalysisTool::dumpLvl1()
{

  // retrieve lvl1 ROIs
  // retrieve the lvl1 result container
  const LVL1_ROI* lvl1Roi;
  StatusCode sc = m_storeGate->retrieve(lvl1Roi,"LVL1_ROI");
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Can't retrieve the LVL1_ROIs");
    return false;
  }

  // loop on the rois to find a matching one
  // get the electron ROIs
  if (lvl1Roi) {
 
    LVL1_ROI::muons_type::const_iterator it = (lvl1Roi->getMuonROIs()).begin();
    LVL1_ROI::muons_type::const_iterator it_end = (lvl1Roi->getMuonROIs()).end();
    
    ATH_MSG_INFO("======================================================" );
    ATH_MSG_INFO("Dumping Muon LVL1 ROIs: " );
    
    for ( ; it != it_end ; ++it ) {
      
      ATH_MSG_INFO("RoI threshold, eta, phi: " << it->getThrName() << "  " 
		   << it->eta() << "  " << it->phi() );                 
      
      
    } // loop on the RoIs
    
  }



  return true;
}

