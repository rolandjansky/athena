/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxVertexOnJetAxis.h  -  Description
                             -------------------
    begin   : Februar 2007
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

   (c) Atlas Collaboration 2007

   This class represents one vertex on the Jet axis to be fitted by the 
   "JetFitter" algorithm and all vertices are stored in a vector inside 
   the VxJetCandidate class.
   
   It contains a vector of pointers to the tracks which belongs to the vertex:
   this tracks "belong" directly to the VxJetCandidate.

   Properties of the single VxVertexOnJetAxis class are:
   FitQuality (chi2 and ndf)
   Num_vertex (gives the vertex numbering scheme, used internally by JetFitter, 
               should not be needed by the user)
   CompatibilityToPrimaryVtx (gives the probability of that vertex to be compatible 
                              with the primary vertex)
   

 ***************************************************************************/

#ifndef VXJETVERTEX_VxVertexOnJetAxis_H
#define VXJETVERTEX_VxVertexOnJetAxis_H

#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/FitQuality.h"
 
namespace Trk {

  class VxTrackAtVertex;

  /** 
   *  VxVertexOnJetAxis inherits from Vertex.
   *  In addition to the vertex position, defined in Vertex class,
   *  it contains its error matrix, which can be retrieved as covariance
   *  or  weight matrix, and well as fit quality (in for of chi2 and ndf).
   */
   
  class VxVertexOnJetAxis 
  {
   public:
   
  /**
   * Default constructor, if called initializes a VxVertexOnJetAxis
   * with all data members set to 0.
   */
    VxVertexOnJetAxis();
  
     
  /**
   * Constructs a VxVertexOnJetAxis with its tracks pointers 
   */                               
    VxVertexOnJetAxis(const std::vector<VxTrackAtVertex*> & tracksAtVertex);              

  /**
   * Constructs a VxVertexOnJetAxis with its tracks pointers + number of Vertex (only internally 
   used by the fitter)
   */                               
    VxVertexOnJetAxis(const std::vector<VxTrackAtVertex*> & tracksAtVertex,
		      int numVertex);
  
  /**
   * Copy constructor and assignement operator
   */  
    VxVertexOnJetAxis(const VxVertexOnJetAxis& ); 
    VxVertexOnJetAxis &operator= (const VxVertexOnJetAxis &);  

  /** 
   * Output Method for MsgStream, to be overloaded by child classes 
   */
    virtual MsgStream& dump(MsgStream& sl) const;
    
  /** 
   * Output Method for std::ostream, to be overloaded by child classes 
   */
    virtual std::ostream& dump(std::ostream& sl) const;

  /**
   * Set Method for NumVertex
   */
    void setNumVertex(int numVertex);

  /**
   * Get Method for NumVertex
   */
    int getNumVertex(void) const;

  /**
   * Destructor 
   */
    virtual ~VxVertexOnJetAxis(); 


  /**
   * Fit quality access method
   */
    const Trk::FitQuality& fitQuality() const;

   /**
    * Fit quality set method
    */
     void setFitQuality(const Trk::FitQuality& fitQuality);


  /**
   * get Tracks At Vertex Method
   */
    const std::vector<VxTrackAtVertex*> & getTracksAtVertex(void) const;


  /**
   * set Tracks At Vertex Method
   */
    void setTracksAtVertex(const std::vector<VxTrackAtVertex*> & tracksAtVertex);

  /**
   * get compatibility to the primary vertex
   */
    float getCompatibilityToPrimaryVtx(void) const;

  /**
   * set compatibility to the primary vertex
   */
    void setCompatibilityToPrimaryVtx(float);

  private:
    std::vector<VxTrackAtVertex*> m_tracksAtVertex;
    int m_numVertex;//starts from 0, is used internally by the fitter, should not be needed by the user
    Trk::FitQuality m_fitQuality; //chi2 and number of degrees of freedom of the fit
    float m_compatibilityToPrimaryVtx;
  };




} // end of namespace
#endif

