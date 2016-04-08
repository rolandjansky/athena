/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERVALIDATIONUTILS_TRKPRIVXPURITY_H
#define TRKVERTEXFITTERVALIDATIONUTILS_TRKPRIVXPURITY_H

#include <vector>
#include "GaudiKernel/MsgStream.h"

/**
 *   @class Trk::TrkPriVxPurity
 *
 *  A prototype data class for primary vertex purity.
 *  Provides a truth information summary about the fitted 
 *  primary vertex:
 *
 *  -- Number tracks fitted to the vertex
 *
 *  -- Number of tracks w/o original info stored
 *
 *  -- Number of tracks w/o truth associated:
 *      While the PU navigation is not available, this
 *      number includes both fake reconstructed and pileup tracks
 *
 *  -- Vector of weights for tracks fitted to the vertex, but
 *     originating elswhere:
 *       So far these are only signal event outliers, products of decay of
 *       long-living particles, conversions and do on... Once the PU truth
 *       navigation is available, these will also be tracks coming from PU.       
 *
 *  -- Vector of weights of  tracks w.r.t. fitted vertex
 *
 *   @author Kirill Prokofiev,   October 2006.
 */
 
namespace Trk
{
 
 class TrkPriVxPurity
 {
 
  public:

/**
 * Constructor, requiring a number of fitted tracks, wectors of weghts
 * for inlyers, outlyers, pileup tracks and tracks with no truth
 * association. Number of tracks with broken links to the truth
 * (possible software bugs) is also stored.
 */   
   TrkPriVxPurity(int ntrk, const std::vector<double> puWeights, 
                  const std::vector<double> noTruth, 
                  int brLinks, const std::vector<double> inWeights, 
		  const std::vector<double> outWeights);
 
 /**
 * Destructor
 */   
   ~TrkPriVxPurity(){}
     
/**
 * Number of fitted tracks  
 */
   unsigned int fittedTracks() const;
 
/**
 * Weights of tracks from pileUp events   
 */
  const std::vector<double> pileUpWeights() const;
     
/**
 * Weights of not associated tracks     
 */ 
  const std::vector<double> noTruthFound() const;
   
/**
 * Number of lost pointers: these must software bugs if any
 */
  unsigned int brokenLinks() const;   
   
/** 
 * List of track weights for inliers (only tracks from signal event are listed) 
 */
   const std::vector<double> inlierWeights()const;
    
/**
 * List of track weights for outliers (only tracks from the signal event are listed)   
 */
   const std::vector<double> outlierWeights()const;
   
/**
 * Dump info method
 */   
   MsgStream& dump(MsgStream& sl) const;

/**
 * Dump info method
 */   
   std::ostream& dump(std::ostream& sl) const;
   
  private:
 
   unsigned int m_fittedTracks;
   std::vector<double> m_puWeights;
   std::vector<double> m_noTruth;
   unsigned int m_brokenLinks;
   
   std::vector<double> m_inWeights;
   std::vector<double> m_outWeights;
 }; 

  inline unsigned int TrkPriVxPurity::fittedTracks() const
  { return m_fittedTracks; } 
  
  inline const std::vector<double> TrkPriVxPurity::noTruthFound() const
  { return m_noTruth; }
  
  inline const std::vector<double> TrkPriVxPurity::outlierWeights()const
  { return m_outWeights; }
  
  inline const std::vector<double> TrkPriVxPurity::inlierWeights()const
  { return m_inWeights; }
  
  inline  unsigned int TrkPriVxPurity::brokenLinks() const
  {return m_brokenLinks; }
  
  inline const std::vector<double> TrkPriVxPurity::pileUpWeights() const
  {return m_puWeights; }
 
}// end of namespace definitions

#endif
