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
    changes: new!

   (c) Atlas Collaboration 2007

   For more details look into the header file.


 ***************************************************************************/

#include "VxJetVertex/VxClusteringTable.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"

namespace Trk {

  VxClusteringTable::VxClusteringTable() {}

  VxClusteringTable::~VxClusteringTable() {}

  VxClusteringTable & VxClusteringTable::operator= (const VxClusteringTable & rhs) {
    if (this!=&rhs) {
      m_compatibilityPairOfVertices=rhs.m_compatibilityPairOfVertices;
    }
    return *this;
  }
  
  VxClusteringTable::VxClusteringTable(const VxClusteringTable& rhs) {
    m_compatibilityPairOfVertices=rhs.m_compatibilityPairOfVertices;
  }
  
  MsgStream& VxClusteringTable::dump(MsgStream& sl) const {
    sl << "Trk::VxClusteringTable:" << endmsg;
    if (m_compatibilityPairOfVertices.empty()) {
      sl << "No couple of vertices contained in the table " << endmsg;
    } else {
      sl << "Numbers of compatibilities store: " << m_compatibilityPairOfVertices.size() << endmsg;
      sl << "Highest probability: " << m_compatibilityPairOfVertices.rbegin()->first << endmsg;
      
      std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompBegin=m_compatibilityPairOfVertices.begin();
      std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompEnd=m_compatibilityPairOfVertices.end();
      
      for (std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompIter=CompBegin;
	   CompIter!=CompEnd;++CompIter) {
	
	sl << " Compatibility between track n " << (*CompIter).second.first->getNumVertex() << 
	  " and " << (*CompIter).second.second->getNumVertex() << " is " << (*CompIter).first << 
	  endmsg;
	
      }   
      
    }
    return sl;
  }

  MsgStream& operator << ( MsgStream& sl, const VxClusteringTable& sf) 
  { return sf.dump(sl); }

  std::ostream& operator << ( std::ostream& sl, const VxClusteringTable& sf) 
  { return sf.dump(sl); }

  std::ostream& VxClusteringTable::dump(std::ostream& sl) const {
    sl << "Trk::VxClusteringTable:" << std::endl;
    if (m_compatibilityPairOfVertices.empty()) {
      sl << "No couple of vertices contained in the table " << std::endl;
    } else {
      sl << "Numbers of compatibilities store: " << m_compatibilityPairOfVertices.size() << std::endl;
      sl << "Highest probability: " << m_compatibilityPairOfVertices.rbegin()->first << std::endl;

      std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompBegin=m_compatibilityPairOfVertices.begin();
      std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompEnd=m_compatibilityPairOfVertices.end();

      for (std::map<float,PairOfVxVertexOnJetAxis>::const_iterator CompIter=CompBegin;
	   CompIter!=CompEnd;++CompIter) {

	sl << " Compatibility between track n " << (*CompIter).second.first->getNumVertex() << 
	  " and " << (*CompIter).second.second->getNumVertex() << " is " << (*CompIter).first << 
	  std::endl;


      }
      
    }
    return sl;
  }

  const std::map<float,PairOfVxVertexOnJetAxis> & VxClusteringTable::getCompatibilityPairOfVertices(void) const {
    return m_compatibilityPairOfVertices;
  }


  void VxClusteringTable::setCompatibilityOfTo(const PairOfVxVertexOnJetAxis& pairOfVertices,float compatibility) {
    if (compatibility!=0) {
      m_compatibilityPairOfVertices[compatibility]=pairOfVertices;
    }
    // else {
    //  std::cout << "Warning in VxClusteringTable: assigning to a couple of track 0 probability. Should not happen (waste of time!) " << std::endl;
    //}
    //GP 02-04-2007 Too verbose... happens too often...
  }


  PairOfVxVertexOnJetAxis VxClusteringTable::getMostCompatibleVertices(float & probability) const {
    
    if (m_compatibilityPairOfVertices.empty()) {
      //      std::cout << "Warning in VxClusteringTable: requested highest compatibility to an empty probability table" << std::endl;
      //GP 02-04-2007 Too verbose... happens too often...
      probability=0;
      return PairOfVxVertexOnJetAxis();
    }
    
    std::map<float,PairOfVxVertexOnJetAxis>::const_reverse_iterator pairHighest=
      m_compatibilityPairOfVertices.rbegin();
    
    probability=(*pairHighest).first;
    return (*pairHighest).second;
    
  }

  PairOfVxVertexOnJetAxis VxClusteringTable::getMostCompatibleVerticesExcludingPrimary(float & probability) const 
  {
    
    std::map<float,PairOfVxVertexOnJetAxis>::const_reverse_iterator revIteratorEnd=m_compatibilityPairOfVertices.rbegin();
    std::map<float,PairOfVxVertexOnJetAxis>::const_reverse_iterator revIterator=revIteratorEnd;
    std::map<float,PairOfVxVertexOnJetAxis>::const_reverse_iterator pairHighest=revIteratorEnd;
    
    bool found(false);
    for ( ; revIterator!=m_compatibilityPairOfVertices.rend() ; revIterator++)
    {
      
      const PairOfVxVertexOnJetAxis & myVxVertex=(*revIterator).second;
      VxVertexOnJetAxis* firstVertex=myVxVertex.first;
      VxVertexOnJetAxis* secondVertex=myVxVertex.second;
      
      if (firstVertex->getNumVertex()==-10 || secondVertex->getNumVertex()==-10)
      {
        continue;
      }
      
      found=true;
      pairHighest=revIterator;
      break;
    }
    
 
    if (!found)
    {
      probability=0;
      return PairOfVxVertexOnJetAxis();
    }
 
    probability=(*pairHighest).first;
    return (*pairHighest).second;
  }
 
}//end namespace
