// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//
#ifndef TRACKINGANALYSISALGORITHMS_VERTEXSELECTIONALG_H
#define TRACKINGANALYSISALGORITHMS_VERTEXSELECTIONALG_H

// Framework include(s):
#include "AnaAlgorithm/AnaAlgorithm.h"

// System include(s):
#include <string>

namespace CP {

   /// Algorithm meant to select events with "some" vertex requirements
   ///
   /// This algorithm is meant to be used to pose vertex requirements for the
   /// analysed event. Most commonly requiring at least one primary vertex with
   /// some number of track particles associated to it.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class VertexSelectionAlg final : public EL::AnaAlgorithm {

   public:
      /// Algorithm constructor
      VertexSelectionAlg( const std::string& name, ISvcLocator* svcLoc );

      /// @name Function(s) inherited from @c EL::AnaAlgorithm
      /// @{

      /// Function initialising the algorithm
      virtual StatusCode initialize() override;

      /// Function executing the algorithm
      virtual StatusCode execute() override;

      /// @}

   private:
      /// @name Algorithm properties
      /// @{

      /// Event store key of the vertex container
      std::string m_vertexKey;
      /// Number of vertices required in the event
      unsigned m_minVertices;
      /// Number of track particles required per vertex
      unsigned m_minTracks;

      /// @}

   }; // class VertexSelectionAlg

} // namespace CP

#endif // TRACKINGANALYSISALGORITHMS_VERTEXSELECTIONALG_H
