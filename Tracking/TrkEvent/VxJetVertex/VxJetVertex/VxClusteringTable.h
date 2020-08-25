/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxClusteringTable.h  -  Description
                             -------------------
    begin   : Februar 2007
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch

   (c) Atlas Collaboration 2007

   (1) November 6, 2016
       adding support for differentiating between pair of probabilities with PV or without      
       Giacinto Piacquadio (Stony Brook University)
       giacinto.piacquadio@stonybrook.edu

   This class stores the information about the compatiblity of the vertices fitted 
   on the Jet Axis among themselves and with the Primary Vertex.

   This class essentially contains a map between the pairs of VxVertexOnJetAxis and 
   the probability (the key of the map) to form a common vertex under the constraint of having this common vertex 
   on the JetAxis. This helps to reject light-quark jet random lifetime tracks when applying the 
   JetFitter algorithm to B-Tagging. (refer to the new JetFitter algorithm for more details)


 ***************************************************************************/

#ifndef VXJETVERTEX_VxClusteringTable_H
#define VXJETVERTEX_VxClusteringTable_H

#include "GaudiKernel/MsgStream.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include <map>
 

namespace Trk {

  class VxVertexOnJetAxis;

  class VxClusteringTable 
  {
   public:
   
  /**
   * Default constructor, if called initializes a VxClusteringTable
   * with all data members set to 0.
   */
    VxClusteringTable();

    ~VxClusteringTable();
  
    VxClusteringTable(const VxClusteringTable& ); 
    VxClusteringTable &operator= (const VxClusteringTable &);  

  /**
   * Set compatibility of a new pair of tracks
   */

    void setCompatibilityOfTo(const PairOfVxVertexOnJetAxis&,float);

  /**
   * Get pair of tracks with highest compatibility
   */

    PairOfVxVertexOnJetAxis getMostCompatibleVertices(float & probability) const;

  /**
   * Get back full compatibility info (only for reading)
   */

    const std::map<float,PairOfVxVertexOnJetAxis> & 
      getCompatibilityPairOfVertices(void) const;
    
  /**
   * Get pair of vertices with highest compatibility, removing cases with primary
   */
 
    PairOfVxVertexOnJetAxis getMostCompatibleVerticesExcludingPrimary(float & probability) const;
    
 

  /** 
   * Output Method for MsgStream, to be overloaded by child classes 
   */
    MsgStream& dump(MsgStream& sl) const;
    
  /** 
   * Output Method for std::ostream, to be overloaded by child classes 
   */

    std::ostream& dump(std::ostream& sl) const;

  private:
    std::map<float,PairOfVxVertexOnJetAxis> m_compatibilityPairOfVertices;
    
    
  };


  MsgStream& operator << ( MsgStream& sl, const VxClusteringTable& sf); 
  std::ostream& operator << ( std::ostream& sl, const VxClusteringTable& sf) ;



} // end of namespace
#endif

