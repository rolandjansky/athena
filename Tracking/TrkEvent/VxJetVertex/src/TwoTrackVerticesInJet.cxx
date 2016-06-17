/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           TwoTrackVerticesInJet.cxx  -  Description
                             -------------------
    begin   : Jan 2008
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

    (C) Atlas Collaboration 2008

   More information contained in the header file 

 ***************************************************************************/

#include "VxJetVertex/TwoTrackVerticesInJet.h"

#include "VxVertex/VxCandidate.h"

#include "TrkParticleBase/TrackParticleBase.h"

namespace Trk 
{
  
  TwoTrackVerticesInJet::TwoTrackVerticesInJet() {}

  TwoTrackVerticesInJet::TwoTrackVerticesInJet(const std::vector<const xAOD::Vertex*> & twoTrackVertices,
                                               const std::vector<const TrackParticleBase*> & neutralTrackOfVertex)
          :
      m_twoTrackVertices(twoTrackVertices),
      m_neutralTrackOfVertex(neutralTrackOfVertex)
  {}
  

  
  TwoTrackVerticesInJet::~TwoTrackVerticesInJet() 
  {
    deleteAll(m_twoTrackVertices);
    deleteAll(m_neutralTrackOfVertex);
  }
  
  TwoTrackVerticesInJet::TwoTrackVerticesInJet(const TwoTrackVerticesInJet& rhs)         
  {

    std::vector<const xAOD::Vertex*>::const_iterator vxBegin=rhs.m_twoTrackVertices.begin();
    std::vector<const xAOD::Vertex*>::const_iterator vxEnd=rhs.m_twoTrackVertices.end();

    std::vector<const TrackParticleBase*>::const_iterator neuBegin=rhs.m_neutralTrackOfVertex.begin();
    std::vector<const TrackParticleBase*>::const_iterator neuEnd=rhs.m_neutralTrackOfVertex.end();


    for (std::vector<const xAOD::Vertex*>::const_iterator vxIter=vxBegin;
         vxIter!=vxEnd;++vxIter) 
    {
      if (*vxIter!=0)
      {
        const xAOD::Vertex* thisPtr=*vxIter;
        //const xAOD::Vertex* newVertex=thisPtr->clone();
        const xAOD::Vertex* newVertex = new xAOD::Vertex(*thisPtr);
        m_twoTrackVertices.push_back(newVertex);
      }
    }

    for (std::vector<const TrackParticleBase*>::const_iterator neuIter=neuBegin;
         neuIter!=neuEnd;++neuIter)
    {
      if (*neuIter!=0)
      {
        m_neutralTrackOfVertex.push_back(new TrackParticleBase(**neuIter));
      }
    }
  }
  
  TwoTrackVerticesInJet & TwoTrackVerticesInJet::operator= (const TwoTrackVerticesInJet & rhs) 
  {
    if (this!=&rhs)
    {
      deleteAll(m_twoTrackVertices);
      deleteAll(m_neutralTrackOfVertex);
      
      m_twoTrackVertices.clear();
      m_neutralTrackOfVertex.clear();
      
      std::vector<const xAOD::Vertex*>::const_iterator vxBegin=rhs.m_twoTrackVertices.begin();
      std::vector<const xAOD::Vertex*>::const_iterator vxEnd=rhs.m_twoTrackVertices.end();
      
      std::vector<const TrackParticleBase*>::const_iterator neuBegin=rhs.m_neutralTrackOfVertex.begin();
      std::vector<const TrackParticleBase*>::const_iterator neuEnd=rhs.m_neutralTrackOfVertex.end();
      
      
      for (std::vector<const xAOD::Vertex*>::const_iterator vxIter=vxBegin;
           vxIter!=vxEnd;++vxIter) 
      {
        if (*vxIter!=0)
        {
          const xAOD::Vertex* thisPtr=*vxIter;
          //const xAOD::Vertex* newVertex=thisPtr->clone();
          const xAOD::Vertex* newVertex = new xAOD::Vertex(*thisPtr);
          m_twoTrackVertices.push_back(newVertex);
        }
      }
      
      for (std::vector<const TrackParticleBase*>::const_iterator neuIter=neuBegin;
               neuIter!=neuEnd;++neuIter)
      {
        if (*neuIter!=0)
        {
          m_neutralTrackOfVertex.push_back(new TrackParticleBase(**neuIter));
        }
      }

    }
    
    return *this;
  }
  
     
  void TwoTrackVerticesInJet::setTwoTrackVertices(std::vector<const xAOD::Vertex*> & twoTrackVertices)
  {
    deleteAll(m_twoTrackVertices);
    m_twoTrackVertices=twoTrackVertices;
  }

  void TwoTrackVerticesInJet::setNeutralTrackOfVertices(std::vector<const TrackParticleBase*> & neutralTrackOfVertex)
  {
    deleteAll(m_neutralTrackOfVertex);
    m_neutralTrackOfVertex=neutralTrackOfVertex;
  }

  const std::vector<const xAOD::Vertex*> & TwoTrackVerticesInJet::getTwoTrackVertice() const {
    return m_twoTrackVertices;
  }
  
  const std::vector<const TrackParticleBase*> & TwoTrackVerticesInJet::getNeutralTrackOfVertices() const 
  {
    return m_neutralTrackOfVertex;
  }
  

  void TwoTrackVerticesInJet::deleteAll(std::vector<const xAOD::Vertex*> & twoTrackVertices)
  {
    std::vector<const xAOD::Vertex*>::iterator vxBegin=twoTrackVertices.begin();
    std::vector<const xAOD::Vertex*>::iterator vxEnd=twoTrackVertices.end();

    for (std::vector<const xAOD::Vertex*>::iterator vxIter=vxBegin;
         vxIter!=vxEnd;++vxIter) 
    {
      if (*vxIter!=0)
      {
        delete *vxIter;
        *vxIter=0;
      }
    }
  }
  
  void TwoTrackVerticesInJet::deleteAll(std::vector<const TrackParticleBase*> & neutralTrackOfVertex)
  {

    std::vector<const TrackParticleBase*>::iterator mapBegin=neutralTrackOfVertex.begin();
    std::vector<const TrackParticleBase*>::iterator mapEnd=neutralTrackOfVertex.end();

    for (std::vector<const TrackParticleBase*>::iterator mapIter=mapBegin;
         mapIter!=mapEnd;
         mapIter++)
    {
      if (*mapIter!=0)
      {
        delete *mapIter;
        *mapIter=0;
      }
    }
  }


}//end namespace Trk

  

        
