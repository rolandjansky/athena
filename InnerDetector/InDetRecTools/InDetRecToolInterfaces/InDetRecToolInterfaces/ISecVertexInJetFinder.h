/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ISecVertexInJetFinder.h  -  Description
                             -------------------
    begin   : 30-03-2007
    authors : CSC b-tagging vertexing note members :-)
    work started by: Giacinto Piacquadio (Freiburg University)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
    changes :
    04/2014 :  Add possibility to work with xAOD objects
 ***************************************************************************/


#ifndef InDetRecToolInterfaces_ISecVertexInJetFinder_H
#define InDetRecToolInterfaces_ISecVertexInJetFinder_H
#include "GaudiKernel/AlgTool.h"
#include "xAODTracking/Vertex.h"
#include <vector>

/* Vertex Finder Tool.

    Abstract class which provides the possibility to implement more than one 
    vertex finder tool (every vertex finder tool inherits from 
    this class).

    Base class for secondary vertex finders inside jets (takes as input:
    1) primary vertex
    2) jet direction (jet momentum is provided)
    3) list of tracks
    and gives as output:
    --> VxSecVertexInfo class

*/

/* Forward declarations */
class VxSecVertexInfo;

namespace Trk {
  class RecVertex;
  class VxSecVertexInfo;
  class TrackParticleBase;
}

namespace xAOD {
  class IParticle;
}

namespace InDet
{
static const InterfaceID IID_ISecVertexInJetFinder("ISecVertexInJetFinder", 1, 0);
class ISecVertexInJetFinder : virtual public IAlgTool
  {
public:
    virtual ~ISecVertexInJetFinder() {};
    static const InterfaceID& interfaceID()
    {
        return IID_ISecVertexInJetFinder;
    };
    
   
    virtual Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & primaryVertex,
						      const TLorentzVector & jetMomentum,
						      const std::vector<const xAOD::IParticle*> & inputTracks) const=0;
    
    
  };
}
#endif
