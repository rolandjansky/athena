/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_MVFVXCANDIDATE_H
#define VXMULTIVERTEX_MVFVXCANDIDATE_H

#include <vector>
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxMultiVertex/MvfFitInfo.h" 

/**
 *   @class   Trk::MVFVxCandidate.h 
 * 
 *  A vertex candidate representation for the 
 *  multi vertex fit. This class inherits from
 *  the standard VxCandidate, but containes a
 *  Trk::MvfFitInfo (helper data class) in addition.
 *  Also, the Trk::MVFVxTrackAtVertex is used instead of
 *  Trk::VxTrackAtVertex.
 *
 *  begin July 2006
 *
 *  changes: 05/12/06 Kirill.Prokofiev@cern.ch
 *            EDM cleanup. moving to the FitQuality	   
 *   
 *  @authors N. Giacinto Piacquadio (Freiburg University)
 *    giacinto.piacquadio@physik.uni-freiburg.de
 *   
 */

namespace Trk
{
  class MVFVxCandidate: public VxCandidate 
 {
  public:
  
  /**
   * Default constructor and destructor
   */
    MVFVxCandidate();
    virtual ~MVFVxCandidate();
    
  /**
   * Reimplementation of the VxCandidate constructor
   */  
    MVFVxCandidate(const Trk::RecVertex& recVertex,
     		   const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex);
		
  /**
   * Constructors with additional information:
   * constraint vertex, seed vertex, linearization point 
   */		
    MVFVxCandidate(xAOD::Vertex* constraintVertex,
		   Amg::Vector3D* seedVertex,
		   Amg::Vector3D* linearizationVertex,
		   const Trk::RecVertex& recVertex,
		   std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex);

  /**
   * Constructors with additional information:
   * constraint vertex, seed vertex, linearization point 
   */	
    MVFVxCandidate(xAOD::Vertex* constraintVertex,
		   Amg::Vector3D* seedVertex,
		   Amg::Vector3D* linearizationVertex);

 /**
  * Copy-constructor
  */
    MVFVxCandidate(const MVFVxCandidate& rhs);
 /**
  * Assignement operator
  */
    MVFVxCandidate &operator= (const MVFVxCandidate &); 

 /**
  * Fit info const access
  */
    const Trk::MvfFitInfo & vertexFitInfo(void) const;
    
 /**
  * Fit info unconst access
  */        
    Trk::MvfFitInfo & vertexFitInfo(void);
 
 /**
  * Fit info set method
  */     
    void setVertexFitInfo(const Trk::MvfFitInfo & info);

/**
 * Intializaion check
 */ 
    bool isInitialized(void);
    
/**
 * Set intializaion 
 */   
    void setInitialized(bool what);

/**
 * Clone method
 */
    virtual MVFVxCandidate* clone() const;

/** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
/** Output Method for std::ostream, to be overloaded by child classes */
    virtual std::ostream& dump(std::ostream& sl) const;

  private:

    Trk::MvfFitInfo m_fitInfo;
    bool m_initialized;
    
  }; //end of class definitions   

  inline bool MVFVxCandidate::isInitialized(void) {
    return m_initialized;
  }

  inline void MVFVxCandidate::setInitialized(bool what) {
    m_initialized=what;
  }
 
  inline const Trk::MvfFitInfo & MVFVxCandidate::vertexFitInfo(void) const
  {
   return  m_fitInfo;
  }
 
  inline Trk::MvfFitInfo & MVFVxCandidate::vertexFitInfo(void)
  {
    return m_fitInfo;
  }
  
  inline void MVFVxCandidate::setVertexFitInfo(const Trk::MvfFitInfo & info)
  {
   m_fitInfo = info;
  }

  inline Trk::MVFVxCandidate* Trk::MVFVxCandidate::clone() const
  {
    return new Trk::MVFVxCandidate(*this);
  }
    
}//end of namespace definitions
#endif
