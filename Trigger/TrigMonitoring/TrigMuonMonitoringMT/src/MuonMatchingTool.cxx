/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMatchingTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

static const InterfaceID IID_MuonMatchingTool("IID_MuonMatchingTool", 1, 0);

const InterfaceID& MuonMatchingTool::interfaceID() { return IID_MuonMatchingTool; }


MuonMatchingTool :: MuonMatchingTool(const std::string& type, const std::string& name, const IInterface*  parent)
  : AthAlgTool(type, name, parent)
{}


StatusCode MuonMatchingTool :: initialize(){

  ATH_CHECK( m_trigDec.retrieve() );
  if(m_use_extrapolator){
    ATH_CHECK( m_extrapolator.retrieve() );
  }

  return StatusCode::SUCCESS;
}


template<>
std::tuple<double,double> MuonMatchingTool :: trigPosForMatch<xAOD::L2StandAloneMuon>(const xAOD::L2StandAloneMuon *trig) const {
  return std::forward_as_tuple(trig->roiEta(), trig->roiPhi()); 
}

template<>
std::tuple<double,double> MuonMatchingTool :: offlinePosForMatch<xAOD::L2StandAloneMuonContainer>(const xAOD::Muon *mu) const {
  double eta = mu->eta();
  double phi = mu->phi();
  if(m_use_extrapolator){
    const xAOD::TrackParticle* track = mu->primaryTrackParticle();
    const Trk::TrackParameters* extPar = extTrackToPivot(track);
    if(extPar){
      ATH_MSG_DEBUG("exrapolation successful");
      eta = extPar->position().eta();
      phi = extPar->position().phi();
      delete extPar; extPar = nullptr;
    }
  }
  return std::forward_as_tuple(eta, phi);
}


StatusCode MuonMatchingTool :: matchEF(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchEF()");
  return match<xAOD::MuonContainer>( mu, trig, 0.03, pass);
}



StatusCode MuonMatchingTool :: matchSA(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchSA()");
  double reqdR = 0.25;
  if(m_use_extrapolator) reqdR = reqdRL1byPt(mu->pt());
  return match<xAOD::L2StandAloneMuonContainer>( mu, trig, reqdR, pass);
}


StatusCode MuonMatchingTool :: matchCB(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchCB()");
  return match<xAOD::L2CombinedMuonContainer>( mu, trig, 0.03, pass);
}


StatusCode MuonMatchingTool :: matchL1(  const xAOD::Muon *mu, SG::ReadHandle<xAOD::MuonRoIContainer> &murois, std::string trig, bool &pass) const {

  double refEta = mu->eta();
  double refPhi = mu->phi();
  double reqdR = 0.25;
  if(m_use_extrapolator){
    const xAOD::TrackParticle* track = mu->primaryTrackParticle();
    const Trk::TrackParameters* extPar = extTrackToPivot(track);
    if(extPar){
      ATH_MSG_DEBUG("exrapolation successful");
      refEta = extPar->position().eta();
      refPhi = extPar->position().phi();
      reqdR = reqdRL1byPt(mu->pt());
      delete extPar; extPar = nullptr;
    }
  }

  pass = false;
  for(const auto &roi : *murois){
    double roiEta = roi->eta();
    double roiPhi = roi->phi();
    int roiThr = roi->getThrNumber();
    
    double deta = refEta - roiEta;
    double dphi = xAOD::P4Helpers::deltaPhi(refPhi, roiPhi);
    double dR = sqrt(deta*deta + dphi*dphi);
    ATH_MSG_VERBOSE("L1 muon candidate eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR);
    if( dR<reqdR && roiThr>=L1ItemSTI(trig)){
      reqdR = dR;
      pass = true;
      ATH_MSG_DEBUG("* L1 muon eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR <<  " isPassed=true" ); 
    }
  }

  return StatusCode::SUCCESS;
}



double MuonMatchingTool :: FermiFunction(double x, double x0, double w) const {
  return 1/(1+TMath::Exp(-10*(x-x0)/w));
}


  
const Trk::TrackParameters* MuonMatchingTool :: extTrackToPivot(const xAOD::TrackParticle* track) const {

  const Trk::TrackParameters *extRPC = nullptr;
  const Trk::TrackParameters *extTGC = nullptr;

  if(!track) return extTGC;
  double trkEta = track->eta();
  double extEta =0.;
  bool isBarrel = true;

  if( fabs(trkEta)<1.05){
    extRPC = extTrackToRPC(track);
    if(!extRPC){
      isBarrel = false;
      extTGC = extTrackToTGC(track);
    }
    else{
      isBarrel = true;
      extEta = extRPC->position().eta();
      if(fabs(extEta)>=1.05){
	extTGC = extTrackToTGC(track);
	isBarrel = (extTGC)? false : true; 
      }
    }
  }
  else if( fabs(trkEta)>=1.05 ){
    extTGC = extTrackToTGC(track);
    if(!extTGC){
      isBarrel = true;
      extRPC = extTrackToRPC(track);
    }
    else{
      isBarrel = false;
      extEta = extTGC->position().eta();
      if(fabs(extEta)<1.05){
	extRPC = extTrackToRPC(track);
	isBarrel = (extRPC) ? true : false;
      }
    }
  }

  ATH_MSG_DEBUG("extTGC=" << extTGC << " extRPC=" << extRPC << " isBarrel=" << isBarrel);
  if(   isBarrel) delete extTGC;
  else if(!isBarrel) delete extRPC;
  return (isBarrel) ? extRPC : extTGC;
}

  

const Trk::TrackParameters* MuonMatchingTool :: extTrackToTGC( const xAOD::TrackParticle* trk ) const {
  ATH_MSG_DEBUG("extTrackToTGC");
  if(!trk) return nullptr;
  double TGC_Z = ( trk->eta()>0 )? 15153.0:-15153.0;
  std::unique_ptr<Amg::Transform3D> matrix(new Amg::Transform3D( Amg::Vector3D( 0.,0.,TGC_Z ) ));
  //object pointed by matrix will be deleted in destructer of DiscSurface, therefore release it
  std::unique_ptr<Trk::DiscSurface> disc(new Trk::DiscSurface( matrix.release(), 0., 15000.));
  const bool boundaryCheck = true;

  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*trk,
                                                                  *disc,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);

  ATH_MSG_DEBUG("param=" << param 
		<< " eta=" << ((param) ? param->position().eta() : 0) 
		<< " phi=" << ((param) ? param->position().phi() : 0));;
  return param;
}



const Trk::TrackParameters* MuonMatchingTool :: extTrackToRPC( const xAOD::TrackParticle* trk ) const {
  ATH_MSG_DEBUG("extTrackToRPC");
  if(!trk) return nullptr;
  std::unique_ptr<Trk::CylinderSurface> barrel(new Trk::CylinderSurface(  7478., 15000. ));
  const bool boundaryCheck = true;

  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*trk,
                                                                  *barrel,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  return param;
}



double MuonMatchingTool :: reqdRL1byPt( double mupt){
  double dR = 0.08;
  if( mupt < 10000. ) {
    dR = -0.00001*mupt + 0.18;
  } 
  return dR;
}
