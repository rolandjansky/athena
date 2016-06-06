/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_TrackToVtxLink_H
#define VXMULTIVERTEX_TrackToVtxLink_H

#include "DataModel/ElementLink.h"

#include "xAODTracking/Vertex.h"

/**
 *   @class Trk::TrackToVtxLink.h  
 *			  
 *   This object is needed by the Multi Vertex Fitter, in order to 
 *   store the information about the association between a single track and all 
 *   the vertices the track has to be fitted with.
 *
 *   begin   : 24-05-2006
 *   @authors Giacinto Piacquadio (Freiburg University) giacinto.piacquadio@physik.uni-freiburg.de
 *
 * -------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *
 * Vertex objects stored using this class are now xAOD::Vertex (from Trk::VxCandidate)
 */


namespace Trk
{

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
    TrackToVtxLink(std::vector<xAOD::Vertex*>* ptrToVertices);

 /**
  * Const access to the vertex list
  */ 
    const std::vector<xAOD::Vertex*>* vertices(void) const;
    
/**   
 * Unconst access to the vertex list
 */    
    std::vector<xAOD::Vertex*>* vertices(void);
       
  private:

   /**
    * Infos about vertexes which the track can be referred to
    */
    std::vector<xAOD::Vertex*>* m_vertices;
    
  };
  
  inline const std::vector<xAOD::Vertex*>* TrackToVtxLink::vertices(void) const {
    return m_vertices;
  }

  inline std::vector<xAOD::Vertex*>*  TrackToVtxLink::vertices(void) {
    return m_vertices;
  }

  

}
#endif

