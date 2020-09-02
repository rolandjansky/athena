/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    ConversionPostSelector.cxx  -  Description
    -------------------
    begin   : 01-01-2008
    authors : Tatjana Lenz, Thomas Koffas
    email   : tatjana.lenz@cern.ch, Thomas.Koffas@cern.ch
    changes :
***************************************************************************/
#include "InDetConversionFinderTools/ConversionPostSelector.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <cmath>

namespace{
  constexpr double twopi{2.*M_PI};
}

namespace InDet {
  
  Trk::ParticleMasses const ConversionPostSelector::s_particleMasses;
  
  static const InterfaceID IID_IConversionPostSelector("InDet::ConversionPostSelector", 1, 0);

  ConversionPostSelector::ConversionPostSelector(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
    : AthAlgTool(type, name, parent)
  {
    m_massK0      = 497.672;
    m_sigmaK0     = 8.5;
    m_massLambda  = 1115.683;
    m_sigmaLambda = 3.5;
    m_nsig        = 5;
    m_minPt       = 0.;
    m_maxdR       = -10000.;
    m_maxPhiVtxTrk= 0.2;
    
    m_maxChi2.push_back(35.);
    m_maxChi2.push_back(25.);
    m_maxChi2.push_back(20.);
    
    m_invMassCut.push_back(10000.);
    m_invMassCut.push_back(10000.);
    m_invMassCut.push_back(10000.);
    
    m_fitMomentum.push_back(0.);
    m_fitMomentum.push_back(0.);
    m_fitMomentum.push_back(0.);
    
    m_minRadius.push_back(-10000.);
    m_minRadius.push_back(-10000.);
    m_minRadius.push_back(-10000.);
    
    declareInterface<ConversionPostSelector>(this);
    declareProperty("MaxChi2Vtx",       m_maxChi2     );
    declareProperty("MaxInvariantMass", m_invMassCut  );
    declareProperty("MinFitMomentum",   m_fitMomentum );
    declareProperty("MinRadius",        m_minRadius   );
    declareProperty("MinPt",            m_minPt       );
    declareProperty("MaxdR",            m_maxdR       );
    declareProperty("MaxPhiVtxTrk",     m_maxPhiVtxTrk);
    declareProperty("NSigma",           m_nsig        );
    declareProperty("DecorateVertices", m_decorateVertices=true);
  }
  
  ConversionPostSelector::~ConversionPostSelector() = default;
  
  const InterfaceID& ConversionPostSelector::interfaceID() {
    return IID_IConversionPostSelector;
  }
  
  StatusCode ConversionPostSelector::initialize() {
    return StatusCode::SUCCESS;
  }
  
  StatusCode ConversionPostSelector::finalize() {
    return StatusCode::SUCCESS;
  }
  
  bool ConversionPostSelector::selectConversionCandidate(xAOD::Vertex * vertex, int flag, 
               std::vector<Amg::Vector3D>& trkL) const{
    bool pass = true;
    
    //Determine the cuts
    double maxChi2     = 1000.;
    double invMassCut  = 1000.;
    double fitMomentum = 0.;
    double radius      = 1000.;
    
    if(flag==0) { // Si+Si
      maxChi2     = m_maxChi2[0]    ; //Vertex fit chi2 cut
      invMassCut  = m_invMassCut[0] ; //Fit invariant mass cut
      fitMomentum = m_fitMomentum[0]; //Photon fitted momentum
      radius      = m_minRadius[0]  ; //Minimum acceptable radius of conversion vertex
    }
    if(flag==1) { // Si+TRT
      maxChi2     = m_maxChi2[1]    ; //Vertex fit chi2 cut
      invMassCut  = m_invMassCut[1] ; //Fit invariant mass cut
      fitMomentum = m_fitMomentum[1]; //Photon fitted momentum
      radius      = m_minRadius[1]  ; //Minimum acceptable radius of conversion vertex
    }
    if(flag==2) { // TRT+TRT
      maxChi2     = m_maxChi2[2]    ; //Vertex fit chi2 cut
      invMassCut  = m_invMassCut[2] ; //Fit invariant mass cut
      fitMomentum = m_fitMomentum[2]; //Photon fitted momentum
      radius      = m_minRadius[2]  ; //Minimum acceptable radius of conversion vertex
    }
    
    //chi2 cut
    if (vertex->nTrackParticles() != 2) {
      ATH_MSG_DEBUG("Incorrect number of tracks used in conversion fit.");
      pass = false;
    } else {
     
      float reducedChi2 = vertex->chiSquared()/vertex->numberDoF();
      if (reducedChi2 > maxChi2) pass =  false;
      if (reducedChi2 > maxChi2) pass =  false;
      
      //Minimum radius cut
      double vtxR = vertex->position().perp();
      if(vtxR < radius) pass = false;

      // Parameters at vertex
      std::vector< Trk::VxTrackAtVertex >& trkAtVx = vertex->vxTrackAtVertex();
      if (trkAtVx.size() != 2 || 
          !trkAtVx[0].perigeeAtVertex() || 
          !trkAtVx[1].perigeeAtVertex())
      {
        ATH_MSG_DEBUG("VxTrackAtVertex or perigeeAtVertex not available");
        return false;
      }         
      const Trk::TrackParameters& perigee1 = *(trkAtVx[0].perigeeAtVertex());
      const Trk::TrackParameters& perigee2 = *(trkAtVx[1].perigeeAtVertex());
      
      //invariant mass
      CLHEP::HepLorentzVector momentum;
      Amg::Vector3D sum_mom = perigee1.momentum() + perigee2.momentum();
      double m2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::electron],2);
      double ee = std::sqrt(m2 +  perigee1.momentum().mag2()) + std::sqrt(m2 +  perigee2.momentum().mag2());
      momentum.setPx(sum_mom.x()); momentum.setPy(sum_mom.y()); momentum.setPz(sum_mom.z()); momentum.setE(ee);
      double inv_mass = momentum.m();
      double photonP = std::sqrt(momentum.x()*momentum.x() + momentum.y()*momentum.y());
      double pt1 = perigee1.pT(); double pt2 = perigee2.pT();
      
      if (pt1<m_minPt || pt2<m_minPt) pass = false;
      if (std::fabs(inv_mass) > invMassCut) pass = false;
      if (photonP < fitMomentum) pass = false;
      
      double fR = 1000.;
      std::vector<Amg::Vector3D>::const_iterator ipb=trkL.begin();
      std::vector<Amg::Vector3D>::const_iterator ipbe=trkL.end();
      for(; ipb!=ipbe;++ipb){
        double tmpfR = (*ipb).perp();
        if(tmpfR<fR) fR = tmpfR;
      }
      if(flag==1 && fR-vtxR<m_maxdR) pass = false;
      
      double PhiVtxTrk = vertex->position().phi() - perigee1.parameters()[Trk::phi0];
      if (PhiVtxTrk < -M_PI) PhiVtxTrk += twopi;
      if (PhiVtxTrk >  M_PI) PhiVtxTrk -= twopi;
      if (std::fabs(PhiVtxTrk)>m_maxPhiVtxTrk) pass = false;
      
      if (pass && m_decorateVertices)
      {
        ATH_MSG_DEBUG("Decorating vertex with values used in post selector");
        decorateVertex(*vertex, inv_mass, pt1, pt2, fR, std::fabs(PhiVtxTrk) );
      }
      
    }
    return pass;
  }

  bool
  ConversionPostSelector::selectSecVtxCandidate(
    xAOD::Vertex* vertex,
    int flag,
    std::vector<Amg::Vector3D>& trkL,
    int& type) const
  {
    bool pass = true;
    bool isK0 = false;
    bool isLambda = false;
    bool isLambdaBar = false;
    int kind = -1;
    
    //Determine the cuts
    double maxChi2     = 1000.;
    double fitMomentum = 0.;
    double radius      = 1000.;
    
    if(flag==0) {
      maxChi2     = m_maxChi2[0]    ; //Vertex fit chi2 cut
      fitMomentum = m_fitMomentum[0]; //Photon fitted momentum
      radius      = m_minRadius[0]  ; //Minimum acceptable radius of conversion vertex
    }
    if(flag==1) {
      maxChi2     = m_maxChi2[1]    ; //Vertex fit chi2 cut
      fitMomentum = m_fitMomentum[1]; //Photon fitted momentum
      radius      = m_minRadius[1]  ; //Minimum acceptable radius of conversion vertex
    }
    if(flag==2) {
      maxChi2     = m_maxChi2[2]    ; //Vertex fit chi2 cut
      fitMomentum = m_fitMomentum[2]; //Photon fitted momentum
      radius      = m_minRadius[2]  ; //Minimum acceptable radius of conversion vertex
    }
    
    //chi2 cut
   
    if (vertex->nTrackParticles() != 2) {
      ATH_MSG_DEBUG("Incorrect number of tracks used in conversion fit.");
      pass = false;
    } else {
      float reducedChi2 = vertex->chiSquared()/vertex->numberDoF();
      if (reducedChi2 > maxChi2) pass =  false;
      
      //Minimum radius cut
      double vtxR = vertex->position().perp();
      if(vtxR < radius) pass = false;
      
      // Parameters at vertex
      std::vector< Trk::VxTrackAtVertex >& trkAtVx = vertex->vxTrackAtVertex();
      if (trkAtVx.size() != 2 || 
          !trkAtVx[0].perigeeAtVertex() || 
          !trkAtVx[1].perigeeAtVertex())
      {
        ATH_MSG_DEBUG("VxTrackAtVertex or perigeeAtVertex not available");
        return false;
      }         
      const Trk::TrackParameters& perigee1 = *(trkAtVx[0].perigeeAtVertex());
      const Trk::TrackParameters& perigee2 = *(trkAtVx[1].perigeeAtVertex());
            
      double pt1 = perigee1.pT(); double pt2 = perigee2.pT();
      if (pt1<m_minPt || pt2<m_minPt) pass = false;
      
      double fR = 1000.;
      std::vector<Amg::Vector3D>::const_iterator ipb=trkL.begin();
      std::vector<Amg::Vector3D>::const_iterator ipbe=trkL.end();
      for(; ipb!=ipbe;++ipb){
        double tmpfR = (*ipb).perp();
        if(tmpfR<fR) fR = tmpfR;
      }
      if(flag==1 && fR-vtxR<m_maxdR) pass = false;
      
      //invariant mass. First assume K0, if failed assume Lambda
      CLHEP::HepLorentzVector momentumK0 = fourP(perigee1,perigee2,m_massK0,false);
      double inv_massK0 = momentumK0.m();
      if (std::fabs(inv_massK0-m_massK0) <= m_nsig*m_sigmaK0) isK0 = true;
      CLHEP::HepLorentzVector momentumL = fourP(perigee1,perigee2,m_massLambda,false);
      double inv_massL = momentumL.m();
      if (std::fabs(inv_massL-m_massLambda) <= m_nsig*m_sigmaLambda) isLambda = true;
      CLHEP::HepLorentzVector momentumLb = fourP(perigee1,perigee2,m_massLambda,true);
      double inv_massLb = momentumLb.m();
      if (std::fabs(inv_massLb-m_massLambda) <= m_nsig*m_sigmaLambda) isLambdaBar = true;
      if (!isLambdaBar && !isLambda && !isK0) pass = false;
      CLHEP::HepLorentzVector momentum;
      if(isK0 && isLambda && !isLambdaBar)  {momentum = momentumK0; kind = 110;}
      if(isK0 && isLambdaBar && !isLambda)  {momentum = momentumK0; kind = 101;}
      if(isK0 && !isLambda && !isLambdaBar) {momentum = momentumK0; kind = 100;}
      if(!isK0 && isLambda && !isLambdaBar) {momentum = momentumL;  kind = 10;}
      if(!isK0 && isLambdaBar && !isLambda) {momentum = momentumLb; kind = 1;}
      if(!isK0 && isLambda && isLambdaBar)  {momentum = momentumL;  kind = 11;}
      double particleP = std::sqrt(momentum.x()*momentum.x() + momentum.y()*momentum.y());
      if (particleP < fitMomentum) pass = false;
    }
    type = kind;
    return pass;
  }

  CLHEP::HepLorentzVector
  ConversionPostSelector::fourP(const Trk::TrackParameters& per1,
                                const Trk::TrackParameters& per2,
                                double mass,
                                bool isBar) const
  {
    CLHEP::HepLorentzVector momentum;
    Amg::Vector3D sum_mom = per1.momentum() + per2.momentum();
    double mp1 = 0.; double mp2 = 0.;
    if(mass==m_massK0) {
      mp1 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
      mp2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
    }else{
      if(!isBar){
        if(per1.charge()>0) {
          mp1 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::proton],2);
          mp2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
        } else {
          mp2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::proton],2);
          mp1 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
        }
      }else{
        if(per1.charge()>0) {
          mp1 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
          mp2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::proton],2);
        } else {
          mp2 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::pion],2);
          mp1 = std::pow(ConversionPostSelector::s_particleMasses.mass[Trk::proton],2);
        }
      }
    }
    double ee = std::sqrt(mp1 +  per1.momentum().mag2()) + std::sqrt(mp2 +  per2.momentum().mag2());
    momentum.setPx(sum_mom.x()); momentum.setPy(sum_mom.y()); momentum.setPz(sum_mom.z()); momentum.setE(ee);
    return momentum;
  }

  void
  ConversionPostSelector::decorateVertex(xAOD::Vertex& vertex,
                                         float inv_mass,
                                         float pt1,
                                         float pt2,
                                         float fR,
                                         float deltaPhiVtxTrk) const
  {
    vertex.auxdata<float>("mass") = inv_mass;
    vertex.auxdata<float>("pt1") = pt1;
    vertex.auxdata<float>("pt2") = pt2;
    vertex.auxdata<float>("minRfirstHit") = fR;
    vertex.auxdata<float>("deltaPhiVtxTrk") = deltaPhiVtxTrk;
  }

} // namespace InDet

