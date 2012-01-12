/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUMCPARTICLEINCLUSTER_H
#define TRIGTAUMCPARTICLEINCLUSTER_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauMCParticleInCluster
/// Package : TauTrigPerformance
/// Author  : Soshi Tsuno (KEK)
/// Created : Feb 2009
///
/// DESCRIPTION:
///           contains G4 truth particles at the entrance point of the calorimeter
///           associated with the tau clusters.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauMCParticleInCluster :  public TObject
{
 public :
  TrigTauMCParticleInCluster();
  TrigTauMCParticleInCluster( const TrigTauMCParticleInCluster & other);
  virtual ~TrigTauMCParticleInCluster();

  virtual void reset();

  //==================
  int     m_pdgid;
  int     m_barcode;
  float   m_e;
  float   m_eta;
  float   m_phi;
  float   m_x;
  float   m_y;
  float   m_z;
  float   m_time;

 private:
  ClassDef(TrigTauMCParticleInCluster, 1)  ;
};

#endif
