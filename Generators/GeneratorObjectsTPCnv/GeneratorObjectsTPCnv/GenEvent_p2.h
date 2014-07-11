///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEvent_p2.h 
// Header file for class GenEvent_p2
// Author: S.Binet<binet@cern.ch>
// Date:   October 2006
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_GENEVENT_P2_H 
#define GENERATOROBJECTSTPCNV_GENEVENT_P2_H 

// STL includes
#include <vector>
#include <utility> //> for std::pair

// forward declarations
class McEventCollectionCnv_p2;

class GenEvent_p2
{ 
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class McEventCollectionCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor
   */
  GenEvent_p2();
  
  /** Constructor with parameters
   */
  GenEvent_p2( int signalProcessId, 
	       int eventNbr,
	       double eventScale,
	       double alphaQCD,
	       double alphaQED,
	       int signalProcessVtx,
	       const std::vector<double>& weights,
	       const std::vector<long int>& randomStates,
	       unsigned int verticesBegin,
	       unsigned int verticesEnd,
	       unsigned int particlesBegin,
	       unsigned int particlesEnd );

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

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

  /** Begin position in the vector of vertices composing this event.
   */
  unsigned int m_verticesBegin;

  /** End position in the vector of vertices composing this event.
   */
  unsigned int m_verticesEnd;

  /** Begin position in the vector of particles composing this event.
   */
  unsigned int m_particlesBegin;

  /** End position in the vector of particles composing this event.
   */
  unsigned int m_particlesEnd;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline GenEvent_p2::GenEvent_p2():
  m_signalProcessId  ( -1 ),
  m_eventNbr         ( -1 ),
  m_eventScale       ( -1 ),
  m_alphaQCD         ( -1 ),
  m_alphaQED         ( -1 ),
  m_signalProcessVtx (  0 ),
  m_weights          (    ),
  m_randomStates     (    ),
  m_verticesBegin    (  0 ),
  m_verticesEnd      (  0 ),
  m_particlesBegin   (  0 ),
  m_particlesEnd     (  0 )
{}

inline GenEvent_p2::GenEvent_p2( int signalProcessId, 
				 int eventNbr,
				 double eventScale,
				 double alphaQCD,
				 double alphaQED,
				 int signalProcessVtx,
				 const std::vector<double>& weights,
				 const std::vector<long int>& randomStates,
				 unsigned int verticesBegin,
				 unsigned int verticesEnd,
				 unsigned int particlesBegin,
				 unsigned int particlesEnd ) :
  m_signalProcessId  ( signalProcessId ),
  m_eventNbr         ( eventNbr ),
  m_eventScale       ( eventScale ),
  m_alphaQCD         ( alphaQCD ),
  m_alphaQED         ( alphaQED ),
  m_signalProcessVtx ( signalProcessVtx ),
  m_weights          ( weights ),
  m_randomStates     ( randomStates ),
  m_verticesBegin    ( verticesBegin ),
  m_verticesEnd      ( verticesEnd ),
  m_particlesBegin   ( particlesBegin ),
  m_particlesEnd     ( particlesEnd )
{}



#endif //> GENERATOROBJECTSTPCNV_GENEVENT_P2_H
