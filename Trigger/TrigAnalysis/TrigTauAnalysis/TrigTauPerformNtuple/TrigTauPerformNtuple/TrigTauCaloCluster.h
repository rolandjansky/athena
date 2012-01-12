/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUCALOCLUSTER_H
#define TRIGTAUCALOCLUSTER_H
///////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauCaloCluster
/// Package : TauTrigPerformance
/// Author  : Mogens Dam
/// Created : Feb 2009
///
/// DESCRIPTION:
///           contains CaloClusters associated with tau object
///
///////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TRefArray.h"
#include "TrigTauPerformNtuple/TrigTauCell.h"
#include "TrigTauPerformNtuple/TrigTauMCParticleInCluster.h"

#include <vector>

class TrigTauCaloCluster :  public TObject
{
 public :
  TrigTauCaloCluster();
  TrigTauCaloCluster( const TrigTauCaloCluster & other);
  virtual ~TrigTauCaloCluster();

  virtual void reset();

  //==================
  float   m_e;
  float   m_eta;
  float   m_phi;
  float   m_m;

  float   m_radius;
  bool    m_containsBadCells;

  //------- cell information
  unsigned int         nCells() const {return r_cells.GetEntriesFast();} ;
  const  TrigTauCell&  cell(unsigned int i ) const {return *((TrigTauCell*)r_cells.At(i));};
  TrigTauCell&         cell(unsigned int i )       {return *((TrigTauCell*)r_cells.At(i));};
  void                 resetCells() {r_cells.Clear();} ; 

  TRefArray r_cells;

  //------- g4 truth information
  unsigned int         nG4ParticleAtCalo() const {return g4Particles.size();} ;
  const  TrigTauMCParticleInCluster&  g4ParticleAtCalo(unsigned int i ) const {return *g4Particles[i];};

  std::vector<TrigTauMCParticleInCluster*> g4Particles;

 private:
  ClassDef(TrigTauCaloCluster, 3)  ;
};

#endif
