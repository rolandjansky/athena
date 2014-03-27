/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_TrackToVtxLink_H
#define VXMULTIVERTEX_TrackToVtxLink_H

#include "DataModel/ElementLink.h"

/**
 *   @class Trk::TrackToVtxLink.h  
 *			  
 *   This object is needed by the Multi Vertex Fitter, in order to 
 *   store the information about the association between a single track and all 
 *   the vertices the track has to be fitted with.
 *
 *   begin   : 24-05-2006
 *   @authors Giacinto Piacquadio (Freiburg University) giacinto.piacquadio@physik.uni-freiburg.de
 */


namespace Trk
{

  class VxCandidate;

  class TrackToVtxLink {

  public:    
  
/**
 * Default constructor
 */  
    TrackToVtxLink();
        
/**
 * Destructor
 */  
    ~TrackToVtxLink();

/**
 * Copy constructor
 */     
    TrackToVtxLink(const TrackToVtxLink& rhs);
    
/**
 * Assignement operator
 */    
    TrackToVtxLink & operator= (const TrackToVtxLink& rhs);
    
/**
 * Constructor with also pointer to vertexes
 */
    TrackToVtxLink(std::vector<VxCandidate*>* ptrToVertexes);

 /**
  * Const access to the vertex list
  */ 
    const std::vector<VxCandidate*>* vertexes(void) const;
    
/**   
 * Unconst access to the vertex list
 */    
    std::vector<VxCandidate*>* vertexes(void);
       
  private:

   /**
    * Infos about vertexes which the track can be referred to
    */
    std::vector<VxCandidate*>* m_vertexes;
    
  };
  
  inline const std::vector<VxCandidate*>* TrackToVtxLink::vertexes(void) const {
    return m_vertexes;
  }

  inline std::vector<VxCandidate*>*  TrackToVtxLink::vertexes(void) {
    return m_vertexes;
  }

  

}
#endif

