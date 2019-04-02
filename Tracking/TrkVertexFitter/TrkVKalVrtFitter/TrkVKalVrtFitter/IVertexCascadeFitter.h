/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IVertexCascadeFitter.h  - 
//---------------------------------------------------------------
#ifndef _TrkVKalVrtFitter_IVertexCascadeFitter_H
#define _TrkVKalVrtFitter_IVertexCascadeFitter_H
// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/IToolSvc.h"
//
#include  "TrkVKalVrtFitter/VxCascadeInfo.h"
#include  "xAODTracking/TrackParticle.h" 


namespace Trk{

 typedef int VertexID;

//------------------------------------------------------------------------
  static const InterfaceID IID_IVertexCascadeFitter("IVertexCascadeFitter", 1, 0);

  class IVertexCascadeFitter : virtual public IAlgTool {
    public:
      static const InterfaceID& interfaceID() { return IID_IVertexCascadeFitter;}
//---------------------------------------------------------------------------
//Interface itself

      virtual VertexID startVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
				   double massConstraint = 0.) = 0;
 
      virtual VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
				   double massConstraint = 0.) = 0;
 
      virtual VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
		                   const  std::vector<VertexID> &precedingVertices,
				   double massConstraint = 0.) = 0;

      virtual VxCascadeInfo * fitCascade(const Vertex * primVertex = 0, bool FirstDecayAtPV = false ) = 0;

      virtual StatusCode  addMassConstraint(VertexID Vertex,
                                         const std::vector<const xAOD::TrackParticle*> & tracksInConstraint,
                                         const std::vector<VertexID> &verticesInConstraint, 
				         double massConstraint ) = 0;

   };

} //end of namespace

#endif
