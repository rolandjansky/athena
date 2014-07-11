///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEvent_p1.h 
// Header file for class GenEvent_p1
// Author: S.Binet<binet@cern.ch>
// Date:   April 2006
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENEVENT_P1_H 
#define GENERATOROBJECTSTPCNV_GENEVENT_P1_H 

// STL includes
#include <vector>
#include <utility> //> for std::pair

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/GenVertex_p1.h"
#include "GeneratorObjectsTPCnv/GenParticle_p1.h"

// forward declarations
class GenEventCnv_p1;

class GenEvent_p1
{ 
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 
 
  // Make the AthenaPoolCnv class our friend.
  friend class GenEventCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public methods 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  GenEvent_p1();
  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Id of the processus being generated
   */
  int m_signalProcessId;

  /** Event number
   */
  int m_eventNbr;

  /** Energy scale. see hep-ph/0109068
   */
  double m_eventScale;

  /** value of the QCD coupling. see hep-ph/0109068
   */
  double m_alphaQCD;

  /** value of the QED coupling. see hep-ph/0109068
   */
  double m_alphaQED;

  /** Barcode of the GenVertex holding the signal process.
   *  0 means that no signal process vertex has been written out.
   *  This may come from upstream limitations (like HEPEVT)
   */
  int m_signalProcessVtx;

  /** Weights for this event.
   *  First weight is used by default for hit and miss.
   */
  std::vector<double> m_weights;

  /** Container of random numbers for the generator states
   */
  std::vector<long int> m_randomStates;

  /** Vector of vertices composing this event.
   *  The vertices are ordered by barcodes.
   */
  std::vector<GenVertex_p1> m_vertices;

  /** Vector of particles composing this event.
   *  The particles are ordered by barcodes.
   */
  std::vector<GenParticle_p1> m_particles;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline GenEvent_p1::GenEvent_p1():
  m_signalProcessId  ( -1 ),
  m_eventNbr         ( -1 ),
  m_eventScale       ( -1 ),
  m_alphaQCD         ( -1 ),
  m_alphaQED         ( -1 ),
  m_signalProcessVtx (  0 ),
  m_weights          (    ),
  m_randomStates     (    ),
  m_vertices         (    ),
  m_particles        (    )   
{}

#endif //> GENERATOROBJECTSTPCNV_GENEVENT_P1_H
