/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONE_H
#define CONE_H

#include "egammaEvent/Electron.h"
#include "muonEvent/Muon.h"
#include "McParticleEvent/TruthParticle.h"


/** @class Cone defines a geometrical cone in eta-phi */
class Cone
{
 public:
  /** default constructor*/
  Cone();

  /** destructor */
  ~Cone();

  /** main constructor */
  Cone( double eta, double phi, float radius );
  Cone( double eta, double phi, float radius, const Analysis::Electron& el );
  Cone( double eta, double phi, float radius, const Analysis::Muon& mu );
  Cone( double eta, double phi, float radius, const TruthParticle& trp );

  /** set the eta coordinate of the cone center */
  void SetEta ( double eta   ) { m_eta   = eta;   };

  /** set the phi coordinate of the cone center */
  void SetPhi ( double phi   ) { m_phi   = phi;   };

  /** set the radius of the cone */
  void SetRadius ( double radius) { m_radius= radius;};

  /** set the electron 
      If a muon or another electron is already associated to the cone
      doen nothing
   */
  void SetElectron(const Analysis::Electron& el)
  { 
    if( !m_hasMu && !m_hasEl){
      m_el = el;
      m_hasEl = true;
    }else{
      std::cout << "!!!WARNING: Another lepton already associated to the cone. Do nothing." << std::endl;
    }
  };

  /* set the muon */
  void SetMuon(const Analysis::Muon& mu)
  {
    if( !m_hasMu && !m_hasEl){
      m_mu = mu;
      m_hasMu = true;
    }else{
      std::cout << "!!!WARNING: Another lepton already associated to the cone. Do nothing." << std::endl;
    }
  };

  /* set truth lepton */
  void SetTruthLep( const TruthParticle& trp )
  {
    if( !m_hasMu && !m_hasEl && !m_hasTruth ){
      m_trp = trp;
      m_hasTruth = true;
    }else{
      std::cout << "!!!WARNING: Another lepton already associated to the cone. Do nothing." << std::endl;
    }
  };
  /* returns the eta coordinate of the cone center */
  double Eta() const { return m_eta;   };

  /* returns the phi coordinate of the cone center */
  double Phi() const { return m_phi;   };

  /* returns the radius of the cone */
  double Radius() const { return m_radius; };

  /* returns the electron */
  Analysis::Electron GetElectron() const {return m_el;};

  /* returns the muon */
  Analysis::Muon GetMuon() const {return m_mu;};
  
  /* returns the truth lepton */
  TruthParticle  GetTruthLep()  const {return m_trp;};
  
  bool HasEl() const {return m_hasEl;};
  bool HasMu() const {return m_hasMu;};
  bool HasTruth() const {return m_hasTruth;};
  
 private:
  /** eta of the center of the cone */
  double m_eta;

  /** phi of the center of the cone */
  double m_phi;

  /** radius of the cone*/
  double m_radius;

  /** electron associated to the cone */
  Analysis::Electron m_el;

  /** muon associated to the cone */
  Analysis::Muon     m_mu;

  /** truth lepton associated to the cone */
  TruthParticle      m_trp;

  /** true if the cone is built around one electron */
  bool m_hasEl;

  /** true if the cone is built around one muon */
  bool m_hasMu;

  /** true if the cone is built around one truth lepton */
  bool m_hasTruth;
  
};
#endif
