/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IVertexCascadeFitter.h  - 
//---------------------------------------------------------------
#ifndef TRKVKALVRTFITTER_IVERTEXCASCADEFITTER_H
#define TRKVKALVRTFITTER_IVERTEXCASCADEFITTER_H
// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/EventContext.h"
//
#include  "VxVertex/Vertex.h"
#include  "TrkVKalVrtFitter/VxCascadeInfo.h"
#include  "TrkVKalVrtFitter/IVKalState.h"
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

     /*
      * Context aware method
      */
      virtual std::unique_ptr<IVKalState> makeState(const EventContext& ctx) const = 0;
      
     /*
      * For non-migrated clients whcih should always use the context aware method
      */
      virtual std::unique_ptr<IVKalState> makeState() const 
          {
              return makeState(Gaudi::Hive::currentContext());
          }
      
    virtual VertexID startVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
                                   IVKalState& istate,
				   double massConstraint = 0.) const = 0;
 
      virtual VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
                                   IVKalState& istate,
				   double massConstraint = 0.) const = 0;
 
      virtual VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                                   const  std::vector<double>& particleMass,
		                   const  std::vector<VertexID> &precedingVertices,
                                   IVKalState& istate,
				   double massConstraint = 0.) const = 0;

      virtual VxCascadeInfo * fitCascade(IVKalState& istate,
                                         const Vertex * primVertex = 0, bool FirstDecayAtPV = false ) const = 0;

      virtual StatusCode  addMassConstraint(VertexID Vertex,
                                         const std::vector<const xAOD::TrackParticle*> & tracksInConstraint,
                                         const std::vector<VertexID> &verticesInConstraint, 
                                         IVKalState& istate,
				         double massConstraint ) const = 0;

   };

} //end of namespace

#endif
