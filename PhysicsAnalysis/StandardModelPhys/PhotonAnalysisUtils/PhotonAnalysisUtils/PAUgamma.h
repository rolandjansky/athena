//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// PAUgamma.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This file defines the class PAU::gamma, which is a "wrapper" for photon (and electron) objects
// All PAU code handles photons through PAU::gamma class, so that any additional UserData attributes
// can be stored here
//



#ifndef PHOTONANALYSISUTILS_PAUGAMMA_H
#define PHOTONANALYSISUTILS_PAUGAMMA_H

#include "PhotonAnalysisUtils/PAUutils.h"

#include "egammaEvent/EMShower.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "VxVertex/VxCandidate.h"
#include "CaloEvent/CaloCluster.h"
#include "JetEvent/JetCollection.h"
#include "muonEvent/MuonContainer.h"
#include "MissingETEvent/MissingET.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h" // cannot forward declare because it is a typedef
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/LinkToTrack.h"

#include "Navigation/IAthenaBarCode.h"
#include "FourMom/P4PtEtaPhiM.h"

#include "PhotonAnalysisUtils/PAUnamespace.h"
#include "PhotonAnalysisUtils/PAUgammaAttr.h"
#include "PhotonAnalysisUtils/PAUgammaTruth.h"

#include <iostream>
#include <string>

namespace PAU {

  //-------------------------------------------------------------------------------------
  // class PAU::gamma: a class to treat photons and electrons at the same level
  class gamma : public IAthenaBarCode, public gammaAttr, public gammaTruth   {
  private:
    const Analysis::Photon*    m_ph ;
    const Analysis::Electron*  m_el ;
    const PAU::el*             m_PAUel;
    const Trk::VxCandidate*    m_vx ;
    const Rec::TrackParticle*  m_trk ;
    int                        m_conv_flag;
    P4PtEtaPhiM                m_4mom;
    P4PtEtaPhiM                m_4momclus ;
    double                     m_Eclus, m_etaclus, m_phiclus, m_ptclus ;
    const EMShower*            m_shower;
    //
    void set4mom(); // { 
      if ( m_ph ) {
	m_4mom = (P4PtEtaPhiM) (*m_ph) ;
      } else if ( m_el ) {
	m_4mom = (P4PtEtaPhiM) (*m_el) ;
      }
      bool useTracks = false ;
      m_Eclus = cluster()->e() ;
      //
      float R1, Z1, R2, Z2 ;
      if ( PAUutils::gimeRZ(1,cluster()->etaBE(1),R1,Z1) && PAUutils::gimeRZ(2,cluster()->etaBE(2),R2,Z2) ) {
	m_etaclus = asinh( (Z2-Z1)/(R2-R1) ) ;
      } else {
	m_etaclus = cluster()->eta() ;
      }
      m_phiclus = cluster()->phi() ;
      //
      if ( useTracks ) {
	if ( m_vx ) {
	  // std::cout << ">>> Vertex from photon <<<" << std::endl;
	  const std::vector<Trk::VxTrackAtVertex*>* trklist = m_vx->vxTrackAtVertex() ;
	  Trk::GlobalMomentum pvtx(0,0,0) ;
	  for ( unsigned i=0 ; i<trklist->size() ; ++i ) {
	    Trk::ITrackLink* link = (*trklist)[i]->trackOrParticleLink();
	    const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
	    const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
	    if ( tp->trackSummary()->get(Trk::numberOfPixelHits) + tp->trackSummary()->get(Trk::numberOfSCTHits) > 0 ) {
	      pvtx += (*trklist)[i]->perigeeAtVertex()->momentum() ;
	    }
	  }
	  if ( pvtx != Trk::GlobalMomentum(0,0,0) ) {
	    m_etaclus = pvtx.eta() ;
	    m_phiclus = pvtx.phi() ;
	  }
	} else if ( m_trk && m_trk->trackSummary() &&
		    ( m_trk->trackSummary()->get(Trk::numberOfPixelHits) + m_trk->trackSummary()->get(Trk::numberOfSCTHits) > 0 ) &&
		    m_trk->measuredPerigee() ) {
	  // std::cout << ">>> Track from photon <<<" << std::endl;
	  m_etaclus = m_trk->measuredPerigee()->momentum().eta() ;
	  m_phiclus = m_trk->measuredPerigee()->momentum().phi() ;
	}
      } // end if ( useTracks ) 
      //
      m_ptclus = m_Eclus / cosh(m_etaclus) ;
      m_4momclus = P4PtEtaPhiM( m_ptclus , m_etaclus , m_phiclus , 0 ) ; 
    }
  public:
    
    // default constructor
    gamma() : IAthenaBarCode(), m_ph(0)  , m_el(0)  , m_vx(0)  , m_trk(0), m_conv_flag(0), m_4mom(), m_4momclus(), m_shower(0) {} ;
    
    // constructors from Analysis::Photon
    
    gamma(const Analysis::Photon* ph, int flag=0)
      : IAthenaBarCode(), gammaAttr(), gammaTruth(), 
	m_ph(ph) , m_el(0) , m_vx(ph->conversion()) , m_trk(ph->trackParticle()) , m_conv_flag(flag) , m_shower(0)
    {set4mom();} ;
    gamma(const Analysis::Photon* ph,const Trk::VxCandidate* vx, int flag)
      : IAthenaBarCode(), gammaAttr(), gammaTruth(), 
	m_ph(ph) , m_el(0) , m_vx(vx) , m_trk(ph->trackParticle()) , m_conv_flag(flag) , m_shower(0)
    {set4mom();} ;
    
    // constructors from Analysis::Electron
    
    gamma(const Analysis::Electron* el, int flag=0)
      : IAthenaBarCode(), gammaAttr(), gammaTruth(), 
	m_ph(0) , m_el(el) , m_vx(el->conversion()) , m_trk(el->trackParticle()) , m_conv_flag(flag) , m_shower(0)
    {set4mom(); std::cout<<"DEPRECATED! create PAU::gamma from Electron"<<std::endl;} ;
    gamma(const Analysis::Electron* el,const Trk::VxCandidate* vx, int flag)
      : IAthenaBarCode(), gammaAttr(), gammaTruth(), 
	m_ph(0) , m_el(el) , m_vx(vx) , m_trk(el->trackParticle()) , m_conv_flag(flag) , m_shower(0)
    {set4mom(); std::cout<<"DEPRECATED! create PAU::gamma from Electron"<<std::endl;} ;
    
    gamma(const Analysis::Electron* el, int flag=-1, bool set_four_mom=true)
      : IAthenaBarCode(), gammaAttr(), gammaTruth(), 
	m_ph(0) , m_el(el) , m_vx(0), m_trk(el->trackParticle()) , m_conv_flag(flag) , m_shower(0)
    {if(set_four_mom) set4mom();} ;
    
    // copy constructor
    gamma(const gamma& g) : IAthenaBarCode(), gammaAttr(g), gammaTruth(g), 
      m_ph(g.m_ph) , m_el(g.m_el) , m_vx(g.m_vx) , m_trk(g.m_trk) , m_conv_flag(g.m_conv_flag) , m_shower(g.m_shower) {} ;

    //
    // access methods 
    //

    inline unsigned int author() const {
      if ( m_ph ) return m_ph->author() ;
      if ( m_el ) return m_el->author() ;
      return -1 ;
    } ;

    inline void setShower(const EMShower* the_shower) { m_shower = the_shower ; }

    inline const EMShower* shower(const std::string& egDetailContainerName="") const {
      if ( egDetailContainerName == "" ) {
	return m_shower;
      } else {
	if ( m_ph    )  return m_ph->detail<EMShower>(egDetailContainerName);
	if ( m_PAUel )  return m_PAUel->shower();
	if ( m_el    )  return m_el->detail<EMShower>(egDetailContainerName);
      }
      return 0;
    } ;
    
    inline const CaloCluster* cluster() const {
      if ( m_ph ) return m_ph->cluster() ;
      if ( m_el ) return m_el->cluster() ;
      return 0 ;
    } ;

    inline int isem(int bitmask=0xffffffff) const {
      if ( m_ph ) return m_ph->isem(bitmask) ;
      if ( m_el ) return m_el->isem(bitmask) ;
      return bitmask ;
    }

    inline double e()   const { return m_4mom.e() ; }
    inline double eta() const { return m_4mom.eta() ; }
    inline double phi() const { return m_4mom.phi() ; }
    inline double pt()  const { return m_4mom.pt() ; }
    inline double pz()  const { return m_4mom.pz() ; }
    inline double px()  const { return m_4mom.px() ; }
    inline double py()  const { return m_4mom.py() ; }
    inline double m()   const { return 0 ; }
    inline const I4Momentum * as_4mom() const { return & m_4mom ;} 
    // What follows is to get 4momentum from cluster alone
    inline double eclus()   const { return cluster()->e() ; }
    inline double etaclus() const { return cluster()->eta() ; }
    inline double phiclus() const { return cluster()->phi() ; }
    inline double ptclus()  const { return cluster()->pt() ; }
    inline double pxclus()  const { return cluster()->px() ; }
    inline double pyclus()  const { return cluster()->py() ; }
    inline double pzclus()  const { return cluster()->pz() ; }
    // What follows is to get 4momentum from cluster pointing
    inline double ecalopoint()   const { return m_Eclus ; }
    inline double etacalopoint() const { return m_etaclus ; }
    inline double phicalopoint() const { return m_phiclus ; }
    inline double ptcalopoint()  const { return m_ptclus ; }
    inline double pzcalopoint()  const { return ecalopoint() * tanh( etacalopoint() ) ; }
    inline double pxcalopoint()  const { return ptcalopoint() * cos( phicalopoint() ) ; }
    inline double pycalopoint()  const { return ptcalopoint() * sin( phicalopoint() ) ; }
    inline const I4Momentum * as_4momcalopoint() const { return & m_4momclus ;} 

    inline bool barcode()   const   { return IAthenaBarCode } ;
    inline bool isPhoton()   const   { return (m_ph!=0) ; } ;
    inline bool isElectron() const   { return (m_el!=0) ; } ;
    inline bool isConversion() const { return (m_vx!=0) ; } ; 
    inline const Analysis::Photon* photon() const { return m_ph ; } ; 
    inline const Analysis::Electron* electron() const { return m_el ; } ; 
    inline const PAU::el* PAUel() const { return m_PAUel; };
    inline void set_PAUel(const PAU::el* el) { m_PAUel = el; };
    inline const Trk::VxCandidate* convVx() const { return m_vx ; } ; 
    inline const Rec::TrackParticle* trackParticle() const { return m_trk ; } ;
    inline int convFlag() const {
      return m_conv_flag;
    }
    inline float Rconv() const {
      if (m_vx) return sqrt( pow(m_vx->recVertex().position()[0],2) + pow(m_vx->recVertex().position()[1],2) ) ;
      else      return UNKNOWN;
    } ;
    inline float Zconv() const {
      if (m_vx) return m_vx->recVertex().position()[2] ;
      else      return UNKNOWN;
    } ;
    //DONT USE inline void setElectron(const Analysis::Electron * el){m_el=el;m_ph=0;set4mom();}
    //DONT USE inline void setPhoton(const Analysis::Photon * ph){m_el=0;m_ph=ph;set4mom();}
    //DONT USE inline void setTrack(const Rec::TrackParticle *trk){ m_trk = trk;}
    //DONT USE inline void setVtx(const Trk::VxCandidate* vtx) {m_vx = vtx;}
  } ;

  
}

#endif // PHOTONANALYSISUTILS_PAUGAMMA_H
