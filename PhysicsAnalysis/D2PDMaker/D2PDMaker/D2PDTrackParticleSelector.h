/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDTRACKPARTICLESELECTOR_H
#define D2PDMAKER_D2PDTRACKPARTICLESELECTOR_H

//============================================================================
// Name: D2PDTrackParticleSelector.h
//
/**
   @class D2PDTrackParticleSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select TrackParticle and write out good ones as ElementLinks

   Class to select good TrackParticle from an input container (which can either
   be an jet container or a container of ElementLinks poining to an
   TrackParticleContainer) and write out the good ones as a new container of
   ElementLinks pointing to the good TrackParticle in the original input container.
   The TrackParticle is the base class to (as far as I know) every offline object.
   That means that every electron, jet, cluster, track, muon,... inherits from this.
*/
//=============================================================================

// Include the header of the base class
#include "D2PDMaker/D2PDSelectorBase.h"

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// TrackParticle includes
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"



class D2PDTrackParticleSelector : public D2PDSelectorBase< Rec::TrackParticle, Rec::TrackParticleContainer >
{
public:

  /** Default constructor */
  D2PDTrackParticleSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDTrackParticleSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the TrackParticles begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED TrackParticle. */
  StatusCode processObject( const Rec::TrackParticle* trackPart,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();



private:

  //------------------------------------------------------
  // Call-back methods
  //------------------------------------------------------

  /// Call-back method to configure the cut properties (min/max): numberBLayerHits
  void setupNumberBLayerHits( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): numberPixelHits
  void setupNumberPixelHits( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): numberSCTHits
  void setupNumberSCTHits( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): numberSiliconHits
  void setupNumberSiliconHits( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): numberTRTHits
  void setupNumberTRTHits( Gaudi::Details::PropertyBase& prop );



private:

  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  /// do NumberBLayerHits
  bool m_doNumberBLayerHits;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNumberBLayerHits;

  /** Minimum number of b-layer hits */
  IntegerProperty m_nBLayerHitsMin;

  /** Maximum number of b-layer hits */
  IntegerProperty m_nBLayerHitsMax;


  /// do NumberPixelHits
  bool m_doNumberPixelHits;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNumberPixelHits;

  /** Minimum number of pixel hits */
  IntegerProperty m_nPixelHitsMin;

  /** Maximum number of pixel hits */
  IntegerProperty m_nPixelHitsMax;


  /// do NumberSCTHits
  bool m_doNumberSCTHits;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNumberSCTHits;

  /** Minimum number of SCT hits */
  IntegerProperty m_nSCTHitsMin;

  /** Maximum number of SCT hits */
  IntegerProperty m_nSCTHitsMax;


  /// do NumberSiliconHits
  bool m_doNumberSiliconHits;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNumberSiliconHits;

  /** Minimum number of silicon hits */
  IntegerProperty m_nSiliconHitsMin;

  /** Maximum number of silicon hits */
  IntegerProperty m_nSiliconHitsMax;


  /// do NumberTRTHits
  bool m_doNumberTRTHits;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxNumberTRTHits;

  /** Minimum number of TRT hits */
  IntegerProperty m_nTRTHitsMin;

  /** Maximum number of TRT hits */
  IntegerProperty m_nTRTHitsMax;


}; // End class D2PDTrackParticleSelector



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void D2PDTrackParticleSelector::setupNumberBLayerHits( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNumberBLayerHits = true;
  return;
}

inline void D2PDTrackParticleSelector::setupNumberPixelHits( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNumberPixelHits = true;
  return;
}

inline void D2PDTrackParticleSelector::setupNumberSCTHits( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNumberSCTHits = true;
  return;
}

inline void D2PDTrackParticleSelector::setupNumberSiliconHits( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNumberSiliconHits = true;
  return;
}

inline void D2PDTrackParticleSelector::setupNumberTRTHits( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doNumberTRTHits = true;
  return;
}


#endif
