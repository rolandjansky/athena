/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                      VxJetCandidate.cxx  -  Description
                             -------------------
    begin   : Autumn 2006
    authors : Giacinto Piacquadio (Freiburg University)
              Christian Weiser (Freiburg University)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
              christian.weiser@cern.ch
    changes : new!

 ***************************************************************************/


#include "VxJetVertex/VxJetCandidate.h"

#include "VxJetVertex/RecVertexPositions.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/VxJetFitterDebugInfo.h"
#include "VxJetVertex/VxClusteringTable.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"

#include "AthLinks/ElementLink.h"

namespace Trk {

VxJetCandidate::VxJetCandidate() : 
  VxCandidate(),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) { }

VxJetCandidate::VxJetCandidate(const Trk::RecVertex& recVertex,
                               const std::vector<Trk::VxTrackAtVertex*>& vxTrackAtVertex):
  VxCandidate(recVertex,vxTrackAtVertex),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) { }


VxJetCandidate::VxJetCandidate(const std::vector<std::pair<const Trk::TrackParticleBase*,
                               const Trk::TrackParticleBaseCollection*> > & vectorOfTrackParticles):
  VxCandidate(),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) {

    std::vector<std::pair<const Trk::TrackParticleBase*,const Trk::TrackParticleBaseCollection*> >::const_iterator 
      vTPBegin=vectorOfTrackParticles.begin();

    std::vector<std::pair<const Trk::TrackParticleBase*,const Trk::TrackParticleBaseCollection*> >::const_iterator 
      vTPEnd=vectorOfTrackParticles.end();

    for (std::vector<std::pair<const Trk::TrackParticleBase*,const Trk::TrackParticleBaseCollection*> >::const_iterator 
         vTPIter=vTPBegin;vTPIter!=vTPEnd;++vTPIter) {

      if ((*vTPIter).first!=0) {
        ElementLink<TrackParticleBaseCollection> link;
        link.setElement((*vTPIter).first);
        LinkToTrackParticleBase * linkTT = new LinkToTrackParticleBase(link);
        linkTT->setStorableObject(*((*vTPIter).second));
        VxTrackAtVertex* newVxTrack=new VxTrackAtVertex(linkTT);
        m_vxTrackAtVertex.push_back(newVxTrack);
        std::vector<VxTrackAtVertex*> temp_vector_tracksAtVertex;
        temp_vector_tracksAtVertex.push_back(newVxTrack);
        m_verticesOnJetAxis.push_back(new VxVertexOnJetAxis(temp_vector_tracksAtVertex));
        //every track is added at the beginning to a new additional vertex (all vertices have only 1 track at the beginning)
      } else {
        std::cout << "************************************ ------------- *********************************************" << std::endl;
        std::cout << "ERROR in VxJetCandidate constructor: inconsistency in VxTrackAtVertex tracks. Skipping track..." << std::endl;
        std::cout << "************************************ ------------- *********************************************" << std::endl;
      }
    }
  }

VxJetCandidate::VxJetCandidate(const std::vector<const Trk::TrackParticleBase*> & vectorOfTrackParticles):
  VxCandidate(),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) {

    std::vector<const Trk::TrackParticleBase*>::const_iterator vTPBegin=vectorOfTrackParticles.begin();
    std::vector<const Trk::TrackParticleBase*>::const_iterator vTPEnd=vectorOfTrackParticles.end();

    for (std::vector<const Trk::TrackParticleBase*>::const_iterator vTPIter=vTPBegin;vTPIter!=vTPEnd;++vTPIter) {

      if (*vTPIter!=0) {
        ElementLink<TrackParticleBaseCollection> link;
        link.setElement(*vTPIter);
        LinkToTrackParticleBase * linkTT = new LinkToTrackParticleBase(link);
        VxTrackAtVertex* newVxTrack=new VxTrackAtVertex(linkTT);
        m_vxTrackAtVertex.push_back(newVxTrack);
        std::vector<VxTrackAtVertex*> temp_vector_tracksAtVertex;
        temp_vector_tracksAtVertex.push_back(newVxTrack);
        m_verticesOnJetAxis.push_back(new VxVertexOnJetAxis(temp_vector_tracksAtVertex));
        //every track is added at the beginning to a new additional vertex (all vertices have only 1 track at the beginning)
      } else {
        std::cout << "************************************ ------------- *********************************************" << std::endl;
        std::cout << "ERROR in VxJetCandidate constructor: inconsistency in VxTrackAtVertex tracks. Skipping track..." << std::endl;
        std::cout << "************************************ ------------- *********************************************" << std::endl;
      }

    }
  }


VxJetCandidate::VxJetCandidate(const std::vector<std::pair<const Track*,const TrackCollection*> > & vectorOfTracks):
  VxCandidate(),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) {

    std::vector<std::pair<const Trk::Track*,const TrackCollection*> >::const_iterator 
      vTBegin=vectorOfTracks.begin();

    std::vector<std::pair<const Trk::Track*,const TrackCollection*> >::const_iterator 
      vTEnd=vectorOfTracks.end();

    for (std::vector<std::pair<const Trk::Track*,const TrackCollection*> >::const_iterator 
         vTIter=vTBegin;vTIter!=vTEnd;++vTIter) {

      if ((*vTIter).first!=0) {
        ElementLink<TrackCollection> link;
        link.setElement((*vTIter).first);
        LinkToTrack * linkTT = new LinkToTrack(link);
        linkTT->setStorableObject(*(*vTIter).second);
        VxTrackAtVertex* myPointer=new VxTrackAtVertex(linkTT);
        m_vxTrackAtVertex.push_back(myPointer);
        std::vector<VxTrackAtVertex*> temp_vector_tracksAtVertex;
        temp_vector_tracksAtVertex.push_back(myPointer);
        VxVertexOnJetAxis* myVertexOnJetAxis=new VxVertexOnJetAxis(temp_vector_tracksAtVertex);
        //	myVertexOnJetAxis->setLinearizationPosition(0.);//should already be zero
        //HERE
        m_verticesOnJetAxis.push_back(myVertexOnJetAxis);
        //every track is added at the beginning to a new additional vertex (all vertices have only 1 track at the beginning)
      } else {
        std::cout << "************************************ ------------- *********************************************" << std::endl;
        std::cout << "ERROR in VxJetCandidate constructor: inconsistency in VxTrackAtVertex tracks. Skipping track..." << std::endl;
        std::cout << "************************************ ------------- *********************************************" << std::endl;
      }
    }

  }


VxJetCandidate::VxJetCandidate(const std::vector<const Track*> & vectorOfTracks):
  VxCandidate(),
  m_primaryVertex(nullptr),
  m_debugInfo(nullptr),
  m_clusteringTable(nullptr) {

    std::vector<const Trk::Track*>::const_iterator vTBegin=vectorOfTracks.begin();
    std::vector<const Trk::Track*>::const_iterator vTEnd=vectorOfTracks.end();

    for (std::vector<const Trk::Track*>::const_iterator
         vTIter=vTBegin;vTIter!=vTEnd;++vTIter) {

      if (*vTIter!=0) {
        ElementLink<TrackCollection> link;
        link.setElement(*vTIter);
        LinkToTrack * linkTT = new LinkToTrack(link);
        VxTrackAtVertex* myPointer=new VxTrackAtVertex(linkTT);
        m_vxTrackAtVertex.push_back(myPointer);
        std::vector<VxTrackAtVertex*> temp_vector_tracksAtVertex;
        temp_vector_tracksAtVertex.push_back(myPointer);
        VxVertexOnJetAxis* myVertexOnJetAxis=new VxVertexOnJetAxis(temp_vector_tracksAtVertex);
        //      myVertexOnJetAxis->setLinearizationPosition(0.);//should already be zero
        //HERE
        m_verticesOnJetAxis.push_back(myVertexOnJetAxis);
        //every track is added at the beginning to a new additional vertex (all vertices have only 1 track at the beginning)
      } else {
        std::cout << "************************************ ------------- *********************************************" << std::endl;
        std::cout << "ERROR in VxJetCandidate constructor: inconsistency in VxTrackAtVertex tracks. Skipping track..." << std::endl;
        std::cout << "************************************ ------------- *********************************************" << std::endl;
      }

    }

  }

void VxJetCandidate::setTracksOrigin(const TrackCollection * trackTES) {

  std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=m_vxTrackAtVertex.begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=m_vxTrackAtVertex.end();

  for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
       tracksIter!=tracksEnd;++tracksIter) {

    ITrackLink* IlinkTT=(*tracksIter)->trackOrParticleLink();
    LinkToTrack* linkTT=dynamic_cast<LinkToTrack*>(IlinkTT);
    if(linkTT) {
      linkTT->setStorableObject(*trackTES);
    }
    (*tracksIter)->setOrigTrack(linkTT);

  }

}

void VxJetCandidate::setTrackParticlesOrigin(const Trk::TrackParticleBaseCollection * trackParticleTES) {

  std::vector<Trk::VxTrackAtVertex*>::iterator tracksBegin=m_vxTrackAtVertex.begin();
  std::vector<Trk::VxTrackAtVertex*>::iterator tracksEnd=m_vxTrackAtVertex.end();

  for (std::vector<Trk::VxTrackAtVertex*>::iterator tracksIter=tracksBegin;
       tracksIter!=tracksEnd;++tracksIter) {

    ITrackLink* IlinkTT=(*tracksIter)->trackOrParticleLink();
    LinkToTrackParticleBase* linkTT=dynamic_cast<LinkToTrackParticleBase*>(IlinkTT);
    if(linkTT) {
      linkTT->setStorableObject(*trackParticleTES);
    }
    (*tracksIter)->setOrigTrack(linkTT);

  }

}


VxJetCandidate::VxJetCandidate(const VxJetCandidate& rhs):
  VxCandidate(rhs)
{

  m_recVertex = rhs.m_recVertex;
  m_fittedPositions = rhs.m_fittedPositions;
  m_linearizationPositions = rhs.m_linearizationPositions;
  m_constraintPositions = rhs.m_constraintPositions;

  //you need this map to update all the VxVertexOnJetAxis which only have 
  //pointers to the objects... (with persistency it will be a mess anyway)
  std::map<Trk::VxTrackAtVertex*,Trk::VxTrackAtVertex*> oldToNewPointer;

  std::vector<Trk::VxTrackAtVertex*>::const_iterator RhsVxTracksEnd=rhs.m_vxTrackAtVertex.end();
  for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = rhs.m_vxTrackAtVertex.begin();
       itr != RhsVxTracksEnd; ++itr) {
    if (*itr==0) {
      std::cout << " Pointer of one of the tracks of the object to copy is zero. PROBLEM! Skipping track " << std::endl;
      continue;
    }
    Trk::VxTrackAtVertex* newVxTrackPointer=(*itr)->clone();
    oldToNewPointer[*itr]=newVxTrackPointer;
    this->m_vxTrackAtVertex.push_back(newVxTrackPointer);
  }

  m_primaryVertex=(rhs.m_primaryVertex!=nullptr) ? new VxVertexOnJetAxis(*rhs.m_primaryVertex) : nullptr;

  if (rhs.m_primaryVertex!=nullptr) {
    const std::vector<VxTrackAtVertex*> & primaryVectorTracks=rhs.m_primaryVertex->getTracksAtVertex();

    const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksBegin=primaryVectorTracks.begin();
    const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksEnd=primaryVectorTracks.end();

    std::vector<VxTrackAtVertex*> primaryVectorNew;

    for (std::vector<VxTrackAtVertex*>::const_iterator  primaryVectorIter=primaryVectorTracksBegin;
         primaryVectorIter!=primaryVectorTracksEnd;++primaryVectorIter) {
      VxTrackAtVertex* newTrackPointer=oldToNewPointer[*primaryVectorIter];
      if (newTrackPointer!=nullptr) {
        primaryVectorNew.push_back(newTrackPointer);
      } else {
        std::cout << "************************************ ---------------------------- ********************************" << std::endl;
        std::cout << "Not able to copy old pointer to new one when inizializing tracks of primary vertex to new pointers" << std::endl;
        std::cout << "************************************ ---------------------------- ********************************" << std::endl;
      }
    }

    m_primaryVertex->setTracksAtVertex(std::move(primaryVectorNew));

  } else {
    std::cout << "VxJetCandidate EDM class: Warning in COPY: no primary vertex present" << std::endl;
  }

  //copying over m_verticesOnJetAxis
  std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=rhs.m_verticesOnJetAxis.begin();
  std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=rhs.m_verticesOnJetAxis.end();

  for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;
       ++VtxIter) {

    if (*VtxIter!=0) {

      VxVertexOnJetAxis* newVertexOnJetAxis=new VxVertexOnJetAxis(**VtxIter);

      //now you still need to attach the new pointers...

      std::vector<VxTrackAtVertex*> vertexVectorNew;

      const std::vector<VxTrackAtVertex*> & TracksAtVertex=(*VtxIter)->getTracksAtVertex();

      const std::vector<VxTrackAtVertex*>::const_iterator TracksBegin=TracksAtVertex.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator TracksEnd=TracksAtVertex.end();

      for (std::vector<VxTrackAtVertex*>::const_iterator  TrackVectorIter=TracksBegin;
           TrackVectorIter!=TracksEnd;++TrackVectorIter) {
        VxTrackAtVertex* newTrackPointer=oldToNewPointer[*TrackVectorIter];
        if (newTrackPointer!=nullptr) {
          vertexVectorNew.push_back(newTrackPointer);
        } else {
          std::cout << "************************************ ---------------------------- **********************(((((***********" << std::endl;
          std::cout << "Not able to copy old pointer to new one when inizializing tracks of vertices on jet axis to new pointers" << std::endl;
          std::cout << "************************************ ---------------------------- **************************************" << std::endl;
        }
      }
      newVertexOnJetAxis->setTracksAtVertex(std::move(vertexVectorNew));
      m_verticesOnJetAxis.push_back(newVertexOnJetAxis);

    } else {
      std::cout << "************************************ ------------- *********************************************" << std::endl;
      std::cout << "ERROR in copy constructor of VxJetCandidate: one of the VxVertexOnJetAxis is an empty pointer..." << std::endl;
      std::cout << "************************************ ------------- *********************************************" << std::endl;
    }
  }

  if (rhs.m_debugInfo!=nullptr) {
    m_debugInfo=new VxJetFitterDebugInfo(*rhs.m_debugInfo);
  } else {
    m_debugInfo=nullptr;
  }

  //    if (rhs.m_clusteringTable!=0) {
  //      std::cout << "Warning: during copy of VxJetCandidate the clusteringTable is not copied over" << std::endl;
  m_clusteringTable=nullptr;//new VxClusteringTable(*rhs.m_clusteringTable);
  //    } else {
  //      m_clusteringTable=0;
  //    }
}

VxJetCandidate& VxJetCandidate::operator= (const VxJetCandidate& rhs)
{
  if (this!=&rhs)
  {
    //	this->operator=(rhs);
    // delete objects where pointers pointed to
    // (otherwise -> memory leak)
    std::vector<Trk::VxTrackAtVertex*>::iterator VxTracksEnd=this->m_vxTrackAtVertex.end();
    for (std::vector<Trk::VxTrackAtVertex*>::iterator i = this->m_vxTrackAtVertex.begin();
         i !=VxTracksEnd ; ++i) {
      delete (*i);
      (*i) = 0;
    }

    // and clear the vector
    this->m_vxTrackAtVertex.clear();

    //deleting m_verticesOnJetAxis

    std::vector<VxVertexOnJetAxis*>::const_iterator DelVtxBegin=m_verticesOnJetAxis.begin();
    std::vector<VxVertexOnJetAxis*>::const_iterator DelVtxEnd=m_verticesOnJetAxis.end();

    for (std::vector<VxVertexOnJetAxis*>::const_iterator DelVtxIter=DelVtxBegin;DelVtxIter!=DelVtxEnd;
         ++DelVtxIter) {

      if (*DelVtxIter!=0) {
        delete *DelVtxIter;
      } else {
        std::cout << "************************************ ---------------------------- ***********************************************" << std::endl;
        std::cout << "ERROR in constructor of VxJetCandidate when deleting: one of the VxVertexOnJetAxis is already an empty pointer..." << std::endl;
        std::cout << "************************************ ---------------------------- ***********************************************" << std::endl;
      }
    }

    m_verticesOnJetAxis.clear();//you own them... you need to delete them


    //you need this map to update all the VxVertexOnJetAxis which only have 
    //pointers to the objects... (with persistency it will be a mess anyway)
    std::map<Trk::VxTrackAtVertex*,Trk::VxTrackAtVertex*> oldToNewPointer;

    std::vector<Trk::VxTrackAtVertex*>::const_iterator RhsVxTracksEnd=rhs.m_vxTrackAtVertex.end();
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = rhs.m_vxTrackAtVertex.begin();
         itr != RhsVxTracksEnd; ++itr) {
      if (*itr==0) {
        std::cout << " Pointer of one of the tracks of the object to copy is zero. PROBLEM! Skipping track " << std::endl;
        continue;
      }
      Trk::VxTrackAtVertex* newVxTrackPointer=(*itr)->clone();
      oldToNewPointer[*itr]=newVxTrackPointer;
      this->m_vxTrackAtVertex.push_back(newVxTrackPointer);
    }

    m_primaryVertex=(rhs.m_primaryVertex!=nullptr) ? new VxVertexOnJetAxis(*rhs.m_primaryVertex) : nullptr;

    if (rhs.m_primaryVertex!=nullptr) {
      const std::vector<VxTrackAtVertex*> & primaryVectorTracks=rhs.m_primaryVertex->getTracksAtVertex();

      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksBegin=primaryVectorTracks.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksEnd=primaryVectorTracks.end();

      std::vector<VxTrackAtVertex*> primaryVectorNew;

      for (std::vector<VxTrackAtVertex*>::const_iterator  primaryVectorIter=primaryVectorTracksBegin;
           primaryVectorIter!=primaryVectorTracksEnd;++primaryVectorIter) {
        VxTrackAtVertex* newTrackPointer=oldToNewPointer[*primaryVectorIter];
        if (newTrackPointer!=nullptr) {
          primaryVectorNew.push_back(newTrackPointer);
        } else {
          std::cout << "************************************ ---------------------------- ********************************" << std::endl;
          std::cout << "Not able to copy old pointer to new one when inizializing tracks of primary vertex to new pointers" << std::endl;
          std::cout << "************************************ ---------------------------- ********************************" << std::endl;
        }
      }

      m_primaryVertex->setTracksAtVertex(std::move(primaryVectorNew));

    } else {
      std::cout << "VxJetCandidate EDM class: Warning in COPY: no primary vertex present" << std::endl;
    }

    //copying over m_verticesOnJetAxis
    std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=rhs.m_verticesOnJetAxis.begin();
    std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=rhs.m_verticesOnJetAxis.end();

    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;
         ++VtxIter) {

      if (*VtxIter!=0) {

        VxVertexOnJetAxis* newVertexOnJetAxis=new VxVertexOnJetAxis(**VtxIter);

        //now you still need to attach the new pointers...

        std::vector<VxTrackAtVertex*> vertexVectorNew;

        const std::vector<VxTrackAtVertex*> & TracksAtVertex=(*VtxIter)->getTracksAtVertex();

        const std::vector<VxTrackAtVertex*>::const_iterator TracksBegin=TracksAtVertex.begin();
        const std::vector<VxTrackAtVertex*>::const_iterator TracksEnd=TracksAtVertex.end();

        for (std::vector<VxTrackAtVertex*>::const_iterator  TrackVectorIter=TracksBegin;
             TrackVectorIter!=TracksEnd;++TrackVectorIter) {
          VxTrackAtVertex* newTrackPointer=oldToNewPointer[*TrackVectorIter];
          if (newTrackPointer!=nullptr) {
            vertexVectorNew.push_back(newTrackPointer);
          } else {
            std::cout << "************************************ ---------------------------- **********************(((((***********" << std::endl;
            std::cout << "Not able to copy old pointer to new one when inizializing tracks of vertices on jet axis to new pointers" << std::endl;
            std::cout << "************************************ ---------------------------- **************************************" << std::endl;
          }
        }
        newVertexOnJetAxis->setTracksAtVertex(std::move(vertexVectorNew));
        m_verticesOnJetAxis.push_back(newVertexOnJetAxis);

      } else {
        std::cout << "************************************ ------------- *********************************************" << std::endl;
        std::cout << "ERROR in copy constructor of VxJetCandidate: one of the VxVertexOnJetAxis is an empty pointer..." << std::endl;
        std::cout << "************************************ ------------- *********************************************" << std::endl;
      }
    }

    if (rhs.m_debugInfo!=nullptr) {
      m_debugInfo=new VxJetFitterDebugInfo(*rhs.m_debugInfo);
    } else {
      m_debugInfo=nullptr;
    }

    //    if (rhs.m_clusteringTable!=0) {
    //      std::cout << "Warning: during copy of VxJetCandidate the clusteringTable is not copied over" << std::endl;
    m_clusteringTable=nullptr;//new VxClusteringTable(*rhs.m_clusteringTable);
    //    } else {
    //      m_clusteringTable=0;
    //    }
  }
  return *this;
}

VxJetCandidate::~VxJetCandidate()
{

  if (m_primaryVertex!=nullptr) {
    delete m_primaryVertex;
    m_primaryVertex=nullptr;
  }


  //deleting m_verticesOnJetAxis
  std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=m_verticesOnJetAxis.begin();
  std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=m_verticesOnJetAxis.end();

  for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;
       ++VtxIter) {

    if (*VtxIter!=0) {
      delete *VtxIter;
    } else {
      std::cout << "************************************ ------------- ***********************************************" << std::endl;
      std::cout << "ERROR in destructor of VxJetCandidate: one of the VxVertexOnJetAxis is already an empty pointer..." << std::endl;
      std::cout << "************************************ ------------- ***********************************************" << std::endl;
    }
  }

  if (m_debugInfo!=nullptr) {
    delete m_debugInfo;
    m_debugInfo=nullptr;
  }

  if (m_clusteringTable!=nullptr) {
    delete m_clusteringTable;
    m_clusteringTable=nullptr;
  }

}
const Trk::RecVertexPositions & VxJetCandidate::getRecVertexPositions() const {
  return m_fittedPositions;
}

Trk::RecVertexPositions & VxJetCandidate::getRecVertexPositions() {//non-const overload
  return m_fittedPositions;
}

void VxJetCandidate::setRecVertexPositions(const Trk::RecVertexPositions & newPositions) {
  m_fittedPositions=newPositions;
}

const Trk::VertexPositions & VxJetCandidate::getLinearizationVertexPositions() const {
  return m_linearizationPositions;
}

Trk::VertexPositions & VxJetCandidate::getLinearizationVertexPositions() {//non-const overlaod
  return m_linearizationPositions;
}

void VxJetCandidate::setLinearizationVertexPositions(const Trk::VertexPositions & vtxPositions) {
  m_linearizationPositions=vtxPositions;
}

const std::vector<VxVertexOnJetAxis*> & VxJetCandidate::getVerticesOnJetAxis(void) const {
  return m_verticesOnJetAxis;
}

void VxJetCandidate::setVerticesOnJetAxis(const std::vector<VxVertexOnJetAxis*> & newVertices) {
  m_verticesOnJetAxis=newVertices;
}

const VxVertexOnJetAxis* VxJetCandidate::getPrimaryVertex(void) const {
  return m_primaryVertex;
}

VxVertexOnJetAxis* VxJetCandidate::getPrimaryVertex(void) {//non-const overload
  return m_primaryVertex;
}

void VxJetCandidate::setPrimaryVertex(const VxVertexOnJetAxis* newPrimary) {
  if (m_primaryVertex!=nullptr) { delete m_primaryVertex;
}
  m_primaryVertex=new VxVertexOnJetAxis(*newPrimary);
}

Trk::VxJetFitterDebugInfo * & VxJetCandidate::getDebugInfo(void) {
  return m_debugInfo;
}

Trk::VxClusteringTable* & VxJetCandidate::getClusteringTable(void) {
  return m_clusteringTable;
}

void VxJetCandidate::setConstraintVertexPositions(const Trk::RecVertexPositions & vtxPositions) {
  m_constraintPositions=vtxPositions;
}

const Trk::RecVertexPositions & VxJetCandidate::getConstraintVertexPositions() const {
  return m_constraintPositions;
}

Trk::RecVertexPositions & VxJetCandidate::getConstraintVertexPositions()  {//non-const overload
  return m_constraintPositions;
}



}//end of namespace definitions

