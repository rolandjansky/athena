/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for sim. hit. handle classes                  //
//                                                            //
//  Description: Wrapper for simhits and track records        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//          update:  Feb 2014 - Ric Bianchi (rbianchi@cern.ch)//
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLEBASE_H
#define SIMHITHANDLEBASE_H

#include "VP1TrackSystems/SimBarCode.h"

// Eigen migration
//#include "TrkEventPrimitives/GlobalMomentum.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "TrkParameters/TrackParameters.h"
#include <QString>
// typedef std::pair<int,HepMcParticleLink::index_type> ExtBarCode;

class SimHitHandleBase {
public:
  SimHitHandleBase() : m_trackPars(nullptr),m_pdgId(SimBarCode::unknownPDG), m_fakemom(-2), m_charge(-99999.0) {}
  virtual ~SimHitHandleBase(){}
  virtual QString type() const = 0;

  //Fixme: Evaluate which of the following we really need:
  virtual Amg::Vector3D momentumDirection() const = 0;
  virtual double actualMomentum() const { return -1; }
  double momentum() const { return m_fakemom == -2 ? actualMomentum() : m_fakemom; }
  virtual Amg::Vector3D posStart() const = 0;
  virtual Amg::Vector3D posEnd() const = 0;
  virtual double hitTime() const = 0;

  virtual const HepMcParticleLink& particleLink() const = 0;
  int pdg() const;
  SimBarCode simBarCode() const { return SimBarCode(particleLink(),pdg()); }

  bool hasCharge() const { return m_charge!=-99999.0; }
  double charge() const { return m_charge; }
//   static ExtBarCode extBarCode(const HepMcParticleLink& l) { return ExtBarCode( l.barcode(),l.eventIndex()); }
//   inline ExtBarCode extBarCode() const { return extBarCode( particleLink() ); }

  virtual Trk::TrackParameters * createTrackParameters() const = 0;

  //FIXME: for TrackCollHandle_TruthTracks
  void setFakeMomentum(const double& m ) { m_fakemom = m; }
  void setCharge(const double& c ) { m_charge = c; }
  void cacheMomentum() { if (m_fakemom==-2) m_fakemom = actualMomentum(); }

protected:
  virtual int actualPDGCodeFromSimHit() const { return SimBarCode::unknownPDG; };

private:
  // It is illegal to copy/assign a SimHitHandleBase:
  SimHitHandleBase( const SimHitHandleBase & );
  SimHitHandleBase & operator= ( const SimHitHandleBase & );
  mutable Trk::TrackParameters * m_trackPars;
  int m_pdgId;
  void setPDG(int);//For TrackCollHandle_TruthTracks in case actualPDGCodeFromSimHit() returns SimBarCode::unknownPDG;
  double m_fakemom;
  double m_charge;
  friend class TrackCollHandle_TruthTracks;
  friend class TrackCollHandle_SimulationTracks;
};

typedef std::vector<std::pair<double,SimHitHandleBase*> > SimHitList;//hitTime() to SimHitHandle's

///////////////
//  Inlines  //
///////////////

inline int SimHitHandleBase::pdg() const
{
  return m_pdgId;
}

inline void SimHitHandleBase::setPDG(int pdg)
{
  m_pdgId = pdg;
}

#endif
