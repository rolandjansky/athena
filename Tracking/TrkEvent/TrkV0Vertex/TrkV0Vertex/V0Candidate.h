/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class Trk::V0Candidate.h 
 *   std::vector<Trk::V0Hypothesis *> : contains the Unconstrained V0 hypothesis
 *  plus one or more mass constrained hypotheses.
 *  <br>
 *   begin   : 20-07-2005
 *  <br>
 *   changes : 20-03-2007 
 *             Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 *             Changing private datamembers to pointers, adding clone method, equality operator etc.   
 *	      <br>
 *   changes : 01-04-2007	      
 *	      Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *	      Adding unconst access to the vector of hypothesys.
 *	      Required by finder.
 *	      	      
 *  @authors Evelina Bouhova-Thacker (Lancaster University), Rob Henderson (Lancater University)
 *  e.bouhova@cern.ch, r.henderson@lancaster.ac.uk	      
 */

#ifndef TRKV0VERTEX_V0CANDIDATE_H
#define TRKV0VERTEX_V0CANDIDATE_H

#include "TrkV0Vertex/V0Hypothesis.h"

namespace Trk
{
class V0Candidate
{
  public:


/**
 * Default constructor
 */
   V0Candidate(); 

/**
 * Constructor, using a vector of Trk::V0Hypothesis
 */
   V0Candidate(const std::vector<Trk::V0Hypothesis *>& v0Hyp);

   V0Candidate(std::vector<Trk::V0Hypothesis *>&& v0Hyp);
	      
	      
/**
  * Copy constructor
  */	      
   V0Candidate(const V0Candidate& rhs); 
 /**
   * Assignement constructor
   */	  
  V0Candidate &operator= (const V0Candidate & rhs);  
  V0Candidate &operator= (V0Candidate && rhs);  
  /**
     * Clone method
      */ 
   virtual V0Candidate* clone() const;         
 
/**
 * Destructor.
 */ 
   virtual ~V0Candidate();

/**
 * Unconst access to the vector of V0Hypothesis
 * Required by the finder to set the element links properly.
 */
  std::vector<Trk::V0Hypothesis *>* v0Hypothesis(void);

/**
 * Const access to the vector of V0Hypothesis
 */ 
   const std::vector<Trk::V0Hypothesis *>* v0Hypothesis(void) const; 

  private:

   std::vector<Trk::V0Hypothesis * > m_v0Hyp;

}; //end of class definitions

  inline const std::vector<Trk::V0Hypothesis *>* V0Candidate::v0Hypothesis(void) const
  {
   return &m_v0Hyp;
  }
  
  inline std::vector<Trk::V0Hypothesis *>* V0Candidate::v0Hypothesis(void)
  {
   return &m_v0Hyp;
  }

  inline V0Candidate* V0Candidate::clone() const
  {
   return new V0Candidate(*this);
  }

} //end of namespace definitions
#endif


