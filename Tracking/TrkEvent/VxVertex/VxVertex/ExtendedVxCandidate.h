/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXVERTEX_EXTENDEDVXCANDIDATE_H
#define VXVERTEX_EXTENDEDVXCANDIDATE_H 

#include "VxVertex/VxCandidate.h"
#include <vector>
#include "EventPrimitives/EventPrimitives.h"

/**
 *  @class Trk::ExtendedVxCandidate
 *
 *  Extends the standard Trk::VxCandidate to 
 *  store the full covariance matrix of a fitted 
 *  vertex all participating tracks.
 *  <br>
 *  changed: 2-Apr-2007 Giacinto Piacquadio (added clone() method)
 *
 *  @author      Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 */

class ExtendedVxCandidateCnv_p1;

namespace Trk
{

 class RecVertex;
 class VxTrackAtVertex;
 
 class ExtendedVxCandidate : public VxCandidate
 {
 
  public:
  
/**
 * Standard constructor for persistency
 */ 
   ExtendedVxCandidate();

/**
 * Constructor taking a Trk::RecVertex, vector of incident tracks
 * and a full covariance matrix of the fit
 */   
   ExtendedVxCandidate(const Trk::RecVertex& recVertex,
                       const std::vector<Trk::VxTrackAtVertex *>& vxTrackAtVertex,
                       const Amg::MatrixX * fullCov = 0);
  
/**
 * Virtual calss destructor
 */ 
   virtual ~ExtendedVxCandidate();
   
/**
 * Copy constructor 
 */  
   ExtendedVxCandidate(const ExtendedVxCandidate& rhs);

/**
 * Assignement operator
 */  
   ExtendedVxCandidate &operator= (const ExtendedVxCandidate &);  //!< Assignement operator
   
/**
 * Clone method
 */   
   virtual ExtendedVxCandidate* clone() const;                
   
/**
 *Full covariance matrix access method
 */
   const Amg::MatrixX * fullCovariance(void) const;
  
  private:
   friend class ::ExtendedVxCandidateCnv_p1;
 
/**
 * Full covariance matrix of the vertex and all corresponding
 * tracks.  Size (Ntrk*5 +3)X(Ntrk*5+3)
 */
 
   const Amg::MatrixX * m_fullCovariance; //!< full covariance
 
 };//end of class definitions
 
 inline const Amg::MatrixX * ExtendedVxCandidate::fullCovariance(void) const
 {
   return  m_fullCovariance;
 }
 
  inline Trk::ExtendedVxCandidate* Trk::ExtendedVxCandidate::clone() const
  {
    return new Trk::ExtendedVxCandidate(*this);
  }

}//end of namespace definitions

#endif
