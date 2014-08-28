/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// PAUtruthMatchingTool.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Marcello Fanti, marcello.fanti@mi.infn.it
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////

#include "PhotonAnalysisUtils/PAUtruthMatchingTool.h"
#include "PhotonAnalysisUtils/PAUutils.h"
#include "PhotonAnalysisUtils/PAUnamespace.h"

#include "McParticleEvent/TruthParticleContainer.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "TrackToCalo/ImpactInCalo.h"

#include "TrkParameters/TrackParameters.h"

#include <math.h>
#include <iostream>

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"


using HepGeom::Point3D;


class Best2WorstMatch {
public:
  bool operator() (const PAUtruthMatchingTool::match_t & m1,const PAUtruthMatchingTool::match_t & m2) { return m1.score > m2.score ; } 
};


template <typename T,typename T1,typename T2> inline T constrain(T x, T1 min, T2 max ) { 
  return (x>max) ? max : ( (x<min) ? min : x ) ;
}

// constructor
PAUtruthMatchingTool::PAUtruthMatchingTool(const std::string& t, const std::string& n, const IInterface* p) 
  : AthAlgTool(t,n,p),
    m_Truth             ( "PAUtruthTool"         ) ,
    m_PAUhggFitter         ( "PAUhggFitter" )
{
  declareInterface<IPAUtruthMatchingTool>(this);
  declareProperty("ExtrapolateToCaloToolName"        ,m_extrapolateToCaloToolName         = "ExtrapolateToCaloTool");
  declareProperty("ExtrapolateElectronToCaloToolName",m_extrapolateElectronToCaloToolName = "ExtrapolTrackToCaloTool/extrapolElectronTrackToCaloTool");
  declareProperty("ExtrapolateGammaToCaloToolName"   ,m_extrapolateGammaToCaloToolName    = "ExtrapolTrackToCaloTool/extrapolGammaTrackToCaloTool");
}

// destructor
PAUtruthMatchingTool::~PAUtruthMatchingTool()
{}

// Athena standard methods

// initialize
StatusCode PAUtruthMatchingTool::initialize() {
  MsgStream log(msgSvc(), name());
  StatusCode sc ;
  
  log << MSG::DEBUG << "... seeking PAUtruthTool" << endreq ;
  sc = m_Truth.retrieve();
  if ( StatusCode::SUCCESS != sc || m_Truth == 0 ) {
    log << MSG::ERROR << "Can't get handle on PAUtruthTool tool" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "... seeking PAU fitter" << endreq ;
  sc = m_PAUhggFitter.retrieve();
  if ( StatusCode::SUCCESS != sc || m_PAUhggFitter == 0 ) {
    log << MSG::ERROR << "Can't get handle on PAUhggFitter tool" << endreq;
    return StatusCode::FAILURE;
  }

  IToolSvc* p_toolSvc = 0;
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << " Tool Service not found " << endreq;
    return StatusCode::FAILURE;
  }
  IAlgTool* algtool; 

  ListItem ExtrapolateTool1(m_extrapolateElectronToCaloToolName);
  sc = p_toolSvc->retrieveTool(ExtrapolateTool1.type(), ExtrapolateTool1.name(), algtool, this);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve extrapolElectronToCaloTool " << endreq;
    return StatusCode::FAILURE;
  } else {
    m_eToCalo = dynamic_cast<IExtrapolateToCaloTool*>(algtool);
  }

  ListItem ExtrapolateTool2(m_extrapolateGammaToCaloToolName);
  sc = p_toolSvc->retrieveTool(ExtrapolateTool2.type(), ExtrapolateTool2.name(), algtool, this);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve extrapolGammaTrackToCaloTool " << endreq;
    return StatusCode::FAILURE;
  } else {
    m_gToCalo = dynamic_cast<IExtrapolateToCaloTool*>(algtool);
  }

  ListItem ExtrapolateTool3(m_extrapolateToCaloToolName);
  sc = p_toolSvc->retrieveTool(ExtrapolateTool3.type(), ExtrapolateTool3.name(), algtool, this);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Cannot retrieve ExtrapolateToCaloTool " << endreq;
    return StatusCode::FAILURE;
  } else {
    m_extrapolateToCalo = dynamic_cast<IExtrapolateToCaloTool*>(algtool);
  }

  m_calodepth = m_eToCalo->getCaloDepth();
  if (!m_calodepth) {
    log << MSG::ERROR << "Cannot get CaloDepthTool" << endreq;       
    return StatusCode::FAILURE;
  }
  

  log <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUtruthMatchingTool::finalize() {
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<" finalize() successful" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode PAUtruthMatchingTool::execute() {
  MsgStream log(msgSvc(), name());

  static int lastRunNumber = -99999 , 
    lastEventNumber = -99999 ;

  static StatusCode lastSC = StatusCode::FAILURE ;

  if ( doMatch() ) {
    lastSC = StatusCode::SUCCESS ;
  } else {
    lastSC = StatusCode::FAILURE ;
    log << MSG::ERROR << "truth/rec matching failed for run=" << lastRunNumber << " event=" << lastEventNumber << endreq ;
  }

  return lastSC ;

}

const PAU::egamma* PAUtruthMatchingTool::getRecoMatch(const TruthParticle* tp) const {
  /*
  for ( unsigned u=0 ; u<m_myMatches.size() ; ++u ) {
    if ( m_myMatches[u].tp == tp ) return m_myMatches[u].rec ;
  }
  return 0 ;
  */
  int found = -1;
  for ( unsigned u=0 ; u<m_myMatches.size() ; ++u ) {
    if ( m_myMatches[u].tp == tp ) {
      if ( found < 0 ) {
	found = u ;
      } else {
	std::cout << "Ambiguity in Reco->Truth" << std::endl ;
      }
    }
  }
  if ( found >= 0 ) return m_myMatches[found].rec ;
  return 0 ;
}

const TruthParticle* PAUtruthMatchingTool::getTruthMatch(const PAU::egamma* g) const {
  std::vector<const TruthParticle*> myTPs    = m_Truth->getFinalStateMC() ;
  std::vector<match_t> tmpMatches ;
  for ( unsigned uTP=0 ; uTP<myTPs.size() ; ++uTP ) {
    double etaCalo, phiCalo ;
    if ( m_Truth->extrapolToCalo(myTPs[uTP],etaCalo,phiCalo) ) {
	match_t thisMatch ;
	thisMatch.rec = g;
	thisMatch.tp  = myTPs[uTP] ;
	thisMatch.etaCaloTruth = etaCalo ;
	thisMatch.phiCaloTruth = phiCalo ;
	thisMatch.score = score(thisMatch) ;
	if ( thisMatch.score > 0 ) {
	  tmpMatches.push_back( thisMatch ) ;
	}
    }
  }

  // sort matches best to worst
  std::sort( tmpMatches.begin() , tmpMatches.end() , Best2WorstMatch() ) ;
  // remove conflicts
  for ( unsigned i=0 ; i<tmpMatches.size() ; ++i ) {
    for ( unsigned k=i+1 ; k<tmpMatches.size() ; ++k ) {
      if ( conflict(tmpMatches[i],
		    tmpMatches[k]))  tmpMatches[k].score = -1 ;
    }
  }

  if(tmpMatches.size() > 0) return tmpMatches[0].tp;

  return 0 ;
}

bool PAUtruthMatchingTool::doMatch() {
  // MH: 2013-11-12
  // removed all of this.

  return true ;
}


int PAUtruthMatchingTool::score(const match_t & m) const {
  static const float dR2 = 0.1 ;
  static const float dR1 = 0.025 ; // one EMC Middle cell
  //
  const PAU::egamma*      rec = m.rec ;
  const TruthParticle*   tp  = m.tp ;

  float etaClus    = rec->cluster()->etaBE(2) ;
  float phiClus    = rec->cluster()->phiBE(2) ;

  float dR = PAUutils::deltaR(etaClus,phiClus,m.etaCaloTruth,m.phiCaloTruth) ;

  if ( dR > dR2 ) {
    // invalid match!
    return -99999 ;
  } 
  int myscore = 0 ;

  // score by angular distance:
  float drscore = 100 * log10(dR2/dR) ;    // dR2->0 , dR1->60
  drscore = constrain(drscore,0,200) ;     // don't go closer than 0.001 (a strip has eta width ~0.003)
  myscore += (int) drscore ;               // granularity: 10^(1/100) ~ 2%

  // inside narrow cone, score by truth pt
  if ( dR < dR1 ) { 
    float ptscore = 1000 + 100 * log10( tp->pt() / 1000 ) ; // 1GeV->1000 , 10GeV->1100 , 100GeV->1200, 1TeV->1300
    ptscore = constrain(ptscore,1000,1400) ;   // don't go below 0.1 GeV and above 10 TeV (just in case...)
    myscore += (int) ptscore ;                 // granularity: dp/p = 10^(1/100) ~ 2%
    // favour true photon
    if ( tp->pdgId() == 22 ) {
      myscore += 10000 ;
    }
  } 

  //
  return myscore ;
}

bool PAUtruthMatchingTool::simpleExtrapolator(const TruthParticle* tp, double& etaCalo, double& phiCalo) const {
  if ( tp == 0 ) return false ;
  if ( tp->genParticle() == 0 ) return false ;
  if ( tp->genParticle()->production_vertex() == 0 ) return false ;
  //
  const double B       = 2 ; //Tesla
  //const double Rbarrel = 1700 ; // [mm] approx radius of 2nd sampling in barrel
  //const double Zendcap = 3880 ; // [mm] Z-coordinate of 2nd sampling in endcap
  //const double Rbarrel = 1443 ; // [mm] radius of presampler in barrel
  //const double Rbarrel = 1200 ; // [mm] inner radius of coil in barrel
  //const double Zendcap = 3625 ; // [mm] Z-coordinate of presampler in endcap
  const double Rbarrel = 1500 ; // [mm]
  const double Zendcap = 3600 ; // [mm]
  //
  double q      = tp->charge() ;
  double pt     = tp->pt() ;         // [MeV/c] 
  double eta    = tp->eta() ;
  double phi    = tp->phi() ;
  double sgneta = (eta>=0) ? +1 : -1 ;
  double z0     = tp->genParticle()->production_vertex()->point3d().z() ;
  //
  if ( fabs(z0) > Zendcap ) return false ;
  // 
  if ( q == 0 ) {
    phiCalo = phi ;
    double z1 = z0 + Rbarrel * sinh(eta) ;
    if ( fabs(z1) < Zendcap ) {
      etaCalo = asinh( z1/Rbarrel ) ;
    } else {
      double z1 = Zendcap*sgneta ;
      double r1 = ( z1 - z0 ) / sinh(eta) ;
      etaCalo = asinh( z1/r1 ) ;
    }
  } else {
    double rho = pt / ( 0.3 * fabs(q) * B ) ; // [mm] curvature
    //
    // parametrize helix as:
    // x' = rho*(1-cos(alpha))
    // y' = rho*sin(alpha)
    // z  = z0 + rho*sinh(eta)*alpha
    // where (x',y') are a rotation of (x,y) such that th track is tangent to y' at perigee
    // therefore:
    // r' = 2*rho*sin(alpha/2)
    //
    double alphaB = +9999999 ;
    if ( Rbarrel < 2*rho ) {
      alphaB = 2 * asin( Rbarrel/(2*rho) ) ;
    }
    double alphaE = +9999999 ;
    if ( eta > 0 ) {
      alphaE = ( Zendcap - z0 ) / ( rho * sinh(eta) ) ;
    } else if ( eta < 0 ) {
      alphaE = ( -Zendcap - z0 ) / ( rho * sinh(eta) ) ;
    }
    double alpha = (alphaB<alphaE) ? alphaB : alphaE ;
    if ( alpha >= +9999990 ) return false ;
    //
    phiCalo = phi - q/fabs(q) * alpha/2 ;
    double r1 = 2*rho*sin(alpha/2) ;
    double z1 = z0 + rho*sinh(eta)*alpha ;
    etaCalo = asinh( z1/r1 ) ;
  }
  return true ;
}

bool PAUtruthMatchingTool::betterExtrapolator1(const TruthParticle* tp, double& etaCalo, double& phiCalo) const {
  if ( tp == 0 ) return false ;
  const HepMC::GenParticle* gp = tp->genParticle() ;
  if ( gp == 0 ) return false ;
  HepMC::GenVertex* pvtx = gp->production_vertex();
  if ( pvtx == 0 ) return false ;
  // define particle perigee:
  std::cout << "defining GlobalPosition ..." << std:: endl ;
  Amg::Vector3D pos( pvtx->position().x() , pvtx->position().y() , pvtx->position().z() ) ;
  std::cout << "defining GlobalMomentum ..." << std:: endl ;
  Amg::Vector3D mom( gp->momentum().x() , gp->momentum().y() , gp->momentum().z() ) ;
  std::cout << "defining Perigee ..." << std:: endl ;
  Trk::Perigee perigee(pos,mom,tp->charge(),pos);
  // define particle hypothesis:
  std::cout << "defining ParticleHypothesis ..." << std:: endl ;
  Trk::ParticleHypothesis particle = Trk::undefined ;
  switch ( abs(tp->pdgId()) ) {
  case   11 :   particle = Trk::electron   ; break ;
  case   22 :   particle = Trk::photon     ; break ;
  case   13 :   particle = Trk::muon       ; break ;
  case  211 :   particle = Trk::pion       ; break ;
  case  321 :   particle = Trk::kaon       ; break ;
  case  130 :   particle = Trk::kaon       ; break ;
  case 2212 :   particle = Trk::proton     ; break ;
  case 2112 :   particle = Trk::proton     ; break ;
  }
  const Trk::TrackParameters* result = 0 ;
  const double offset = 0;
  // test whether impct is in barrel:
  std::cout << "calling extrapolator to barrel ..." << std::endl ;
  result = m_extrapolateToCalo->extrapolate(perigee,CaloCell_ID::EMB2,offset,particle);
  if ( result==0 || fabs(result->position().z())>3130 ) {
    // try with endcap
    std::cout << "failed for barrel, calling extrapolator to endcap ..." << std::endl ;
    result = m_extrapolateToCalo->extrapolate(perigee,CaloCell_ID::EME2,offset,particle);
    if ( result==0 || fabs(result->position().perp())>2800 ) {
      // failed!
      std::cout << "failed for endcap ..." << std::endl ;
      result = 0 ;
    }
  } 
  if ( result == 0 ) return false ;
  //
  std::cout << "setting etaCalo, phiCalo ..." << std::endl ;
  etaCalo = result->position().eta() ;
  phiCalo = result->position().phi() ;
  std::cout << "done!" << std::endl ;
  return true ;
}

bool PAUtruthMatchingTool::betterExtrapolator2(const TruthParticle* tp, double& etaCalo, double& phiCalo) const {
  if ( tp == 0 ) return false ;
  const HepMC::GenParticle* gp = tp->genParticle() ;
  if ( gp == 0 ) return false ;
  HepMC::GenVertex* pvtx = gp->production_vertex();
  if ( pvtx == 0 ) return false ;
  // define particle perigee:
  //std::cout << "defining GlobalPosition ..." << std:: endl ;
  Amg::Vector3D pos( pvtx->position().x() , pvtx->position().y() , pvtx->position().z() ) ;
  //std::cout << "defining GlobalMomentum ..." << std:: endl ;
  Amg::Vector3D mom( gp->momentum().x() , gp->momentum().y() , gp->momentum().z() ) ;
  //std::cout << "defining Perigee ..." << std:: endl ;

  // get extrapolation       
  ImpactInCalo* imp(0);
  if ( tp->charge() == 0 ) {
    //Trk::NeutralPerigee perigee(pos,gp->momentum().phi(),gp->momentum().theta(),0);
    Trk::NeutralPerigee perigee(pos,mom,tp->charge(),pos);
    imp = m_gToCalo->getImpactInCalo (perigee,
				      Trk::nonInteracting);
  } else { 
    Trk::Perigee perigee(pos,mom,tp->charge(),pos);
    imp = m_eToCalo->getImpactInCalo (perigee,
				      Trk::nonInteracting);
  }
  if(!imp) return false;
  etaCalo = imp->etaCaloLocal_2();
  phiCalo = imp->phiCaloLocal_2();
  return true ;
}

bool PAUtruthMatchingTool::collectEetaphi(const TruthParticle* tp, double& sumE, double& sumEeta, double& sumEphi, int& ncalls) const {
  MsgStream log(msgSvc(), name());
  --ncalls ;
  if ( ncalls <= 0 ) {
    log << MSG::WARNING << "collectEetaphi : no more recursive calls allowed!" << endreq ;
  }
  if ( tp == 0 ) return false ;
  if ( tp->genParticle() == 0 ) return false ;
  const double Zmax = 2800 ;
  const double Rmax =  800 ;
  HepMC::GenVertex* evtx = tp->genParticle()->end_vertex();
  if ( ncalls<=0 || tp->nDecay()==0 || evtx==0 
       || fabs(evtx->point3d().z())>Zmax 
       || sqrt(pow(evtx->point3d().x(),2)+pow(evtx->point3d().y(),2))>Rmax ) {
    double E = tp->e() ;
    double etaCalo, phiCalo ;
    if ( ! betterExtrapolator2(tp,etaCalo,phiCalo) ) return false ;
    log << MSG::VERBOSE << "\tcollectEetaphi: collect: part=" << tp->pdgId() << "\tE=" << tp->e() 
	<< "\tphi=" << tp->phi() << "\tphiCalo=" << phiCalo << endreq ;
    sumE    += E ;
    sumEeta += E*etaCalo ;
    sumEphi += E*phiCalo ;
  } else {
    log << MSG::VERBOSE << "\tcollectEetaphi: recursive: part=" << tp->pdgId() << "\tE=" << tp->e() << "\tphi=" << tp->phi() << endreq ;
    for ( unsigned u=0 ; u<tp->nDecay() ; ++u ) {
      log << MSG::VERBOSE << "\t\tchild=" << tp->child(u)->pdgId() << "\tEchild=" << tp->child(u)->e() 
	  << "\tphi=" << tp->child(u)->phi() << "\tEchild/E=" << tp->child(u)->e()/tp->e() << endreq;
    }
    for ( unsigned u=0 ; u<tp->nDecay() ; ++u ) {
      if ( ! collectEetaphi( tp->child(u), sumE, sumEeta, sumEphi, ncalls ) ) return false ;
    }
  }
  return true ;
}

bool PAUtruthMatchingTool::clusterToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) const {
  const unsigned NmaxCalls = 1000 ;
  MsgStream log(msgSvc(), name());
  double sumE=0 , sumEeta=0 , sumEphi=0 ;
  int ncalls = NmaxCalls ;
  if ( ! collectEetaphi(tp, sumE, sumEeta, sumEphi, ncalls) )  return false ;
  if ( sumE <= 0 )  return false ;
  etaCalo = sumEeta/sumE ;
  phiCalo = sumEphi/sumE ;
  log << MSG::VERBOSE << "Total n.r calls = " << NmaxCalls - ncalls << endreq ;
  return true ;
}

bool PAUtruthMatchingTool::extrapolToCalo(const TruthParticle* tp, double& etaCalo, double& phiCalo) const {
  //  return betterExtrapolator2(tp,etaCalo,phiCalo) ;
  return m_Truth->extrapolToCalo(tp,etaCalo,phiCalo) ;
}

