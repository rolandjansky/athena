/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXVERTEX_VXCANDIDATE_H
#define VXVERTEX_VXCANDIDATE_H

#include <vector>
#include "VxVertex/RecVertex.h"
#include "TrkEventPrimitives/VertexType.h"

/**
 * @class Trk::VxCandidate.h 
 *                         
 *    @authors andreas.wildauer@cern.ch, Kirill.Prokofiev@cern.ch, Nicola.Giacinto.Piacquadio@cern.ch 
 */

class MsgStream;

namespace Trk
{

class VxTrackAtVertex;
  
class VxCandidate
{
  public:
  
/**
 * Default constructor for persistency
 */
    VxCandidate();
     
/**
 * Constructor using vertex position, fit quality and covariance matrix
 * and a vector of fitted VxTrackAtVertex
 */   
    VxCandidate(const Trk::RecVertex& recVertex,
                const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex);

    VxCandidate(Trk::RecVertex&& recVertex,
                std::vector<Trk::VxTrackAtVertex*>&& vxTrackAtVertex);
				
/**
 * Virtual destructor, copy-constructor and assignement operator
 */   	
    virtual ~VxCandidate();
    VxCandidate(const VxCandidate& rhs);
    VxCandidate &operator= (const VxCandidate &); 
    VxCandidate &operator= (VxCandidate &&); 
    virtual VxCandidate* clone() const;                

/** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
/** Output Method for std::ostream, to be overloaded by child classes */
    virtual std::ostream& dump(std::ostream& sl) const;

/**
 * Returns a reference to reconstructed vertex
 */       
    const Trk::RecVertex& recVertex(void) const; 
    
/**
 * Returns unconst reference to a reconstructed vertex
 * Required by some of the vertex fitters
 */               
    Trk::RecVertex& recVertex(void);
    
/**
 * RecVertex set method
 */                  
    void setRecVertex(Trk::RecVertex& recVertex);

/**
 *  Unconst pointer to the vector of tracks
 *  Required by some of the vertex fitters
 */                  
    std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex(void); 
 
/**
 * Const access to the vector of tracks fitted to the vertex 
 */ 
    const std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex(void) const; 
    
/**
 * return the type of the vertex
 */ 
    void setVertexType(VertexType vertexType);
    
/**
 * return the type of the vertex
 */ 
    VertexType vertexType() const;

/**return number of objects currently created*/
    static unsigned int numberOfInstantiations() ;
    
  protected:
// the type of the vertex (see TrkEventPrimitives/VertexType.h for details) 
    Trk::VertexType m_vertexType;
    
// the fitted vertex (position plus error matrix)
    Trk::RecVertex m_recVertex; 

// information on used tracks
    std::vector<Trk::VxTrackAtVertex*> m_vxTrackAtVertex; 
   
  private:
    static unsigned int s_numberOfInstantiations;

};//end of class definitions

/**Overload of << operator for both, MsgStream and std::ostream for debug 
output; only needed in base class?*/ 
MsgStream& operator << ( MsgStream& sl, const VxCandidate& sf);
std::ostream& operator << ( std::ostream& sl, const VxCandidate& sf); 

  inline void VxCandidate::setVertexType(VertexType vertexType)
  {
    m_vertexType = vertexType;
  }
  
  inline VertexType VxCandidate::vertexType() const {
    return m_vertexType;
  }

 inline void VxCandidate::setRecVertex(Trk::RecVertex& recVertex)
  {
    m_recVertex=recVertex;
  }

  inline const Trk::RecVertex& VxCandidate::recVertex(void) const
  {
    return m_recVertex;
  }

  inline Trk::RecVertex& VxCandidate::recVertex(void)
  {
    return m_recVertex;
  }

  inline std::vector<Trk::VxTrackAtVertex*>* VxCandidate::vxTrackAtVertex(void)
  {
    return &m_vxTrackAtVertex;
  }

  inline const std::vector<Trk::VxTrackAtVertex*>* VxCandidate::vxTrackAtVertex(void) const
  {
    return &m_vxTrackAtVertex;
  }

  inline Trk::VxCandidate* Trk::VxCandidate::clone() const
  {
    return new Trk::VxCandidate(*this);
  }

}//end of namespace definitions
#endif
