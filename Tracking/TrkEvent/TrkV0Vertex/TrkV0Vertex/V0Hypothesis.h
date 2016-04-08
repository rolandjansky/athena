/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#ifndef TRKV0VERTEX_V0HYPOTHESIS_H
#define TRKV0VERTEX_V0HYPOTHESIS_H

#include "VxVertex/ExtendedVxCandidate.h"
#include "EventKernel/PdtPdg.h"


/**
 *  @class Trk::V0Hypothesis
 *  An extension of Trk::ExtendedVxCandidate, to be used for
 *  constrained and unconstrained V0 candidates storage.
 *  In addition to the standard  Trk::ExtendedVxCandidate
 *  functionality, this class stores the assumed PDG ID of
 *  the positive and negative tracks, making the V0
 *  and the assumed PDG ID of the V0.
 *
 *   begin   : 20-07-2005
 *  <br> 
 *   changes : 01-06-2006, Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *             This class now inherits from ExtendedVxCandidate
 *             March 2007, E. Bouhova-Thacker, K. Prokofiev
 *             move methods to TrkVertexAnalysisUtils/V0Tools.h
 * 
 *  @authors  Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
 *   e.bouhova@cern.ch, r.henderson@lancaster.ac.uk
 */

class V0HypothesisCnv_p1;

namespace Trk
{

class V0Hypothesis : public ExtendedVxCandidate
{
  public:

/**
 * Default constructor
 */
   V0Hypothesis();

   V0Hypothesis(const Trk::ExtendedVxCandidate& vxCandidate,
                int positiveTrackID = PDG::pi_plus, int negativeTrackID = PDG::pi_minus, int constraintID = PDG::K0);
/**
 * A constructor taking a Trk::RecVertex, a vector of tracks fitted to the vertex, a full covariance matrix of the fit
 * and PDG ID's of particles and constraint mass as arguments.
 */
   V0Hypothesis(const Trk::RecVertex& recVertex,
                const std::vector<Trk::VxTrackAtVertex *>& vxTrackAtVertex,
                const Amg::MatrixX * fullCov = 0,
                int positiveTrackID = PDG::pi_plus, int negativeTrackID = PDG::pi_minus, int constraintID = PDG::K0);

/**
 * Clone method
 */
   virtual V0Hypothesis * clone() const;

/**
 * Assignement operator, clone method and copy constructor
 */ 
   V0Hypothesis& operator= (const V0Hypothesis& rhs);

/**
 * Destructor
 */ 
   virtual ~V0Hypothesis();

/**
 * Copy constructor
 */
   V0Hypothesis(const V0Hypothesis& rhs);

/**
 * Retrieves a positive decay product ID according to
 * EventKernel/PdtPdg.h. 
 */
  int positiveTrackID(void) const;

/**
 * Retrieves a negative decay product according to
 * EventKernel/PdtPdg.h. 
 */  
  int negativeTrackID(void) const;

/**
 * Retrieves a V0 hypothesis ID according to
 * EventKernel/PdtPdg.h. The pid = 0 for no-hypothesis
 * case (unconstrained fit for instance). This coresponds
 * to PDG::null in ATHENA definitions.
 */    
  int hypothesisID(void) const;

  private:
   friend class ::V0HypothesisCnv_p1;

   int    m_positiveTrackID;
   int    m_negativeTrackID;
   int    m_constraintID;

};//end of  class definitions

 inline int V0Hypothesis::positiveTrackID(void) const
 {return m_positiveTrackID;}

 inline int V0Hypothesis::negativeTrackID(void) const
 {return m_negativeTrackID;}

 inline int V0Hypothesis::hypothesisID(void) const
 {return m_constraintID;}

}//end of namespace definitions
#endif



