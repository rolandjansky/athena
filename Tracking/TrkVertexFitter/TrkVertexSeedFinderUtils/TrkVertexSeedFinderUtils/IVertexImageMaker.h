/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXSEEDFINDERUTILS_IVERTEXIMAGEMAKER_H
#define TRKVERTEXSEEDFINDERUTILS_IVERTEXIMAGEMAKER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/Vertex.h"

namespace Trk
{
  /**
   @class IVertexImageMaker

   Interface tool for vertex image maker

   @author matthew.scott.rudolph@cern.ch

   ------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-19)

   EDM Migration to xAOD - from Trk::RecVertex to xAOD::Vertex

   */

  class VertexImage;

  class IVertexImageMaker : virtual public IAlgTool {

     public:
       DeclareInterfaceID( IVertexImageMaker, 1, 0);


       //Return a vertex image from the tracks centered around the constraint
       virtual std::unique_ptr<VertexImage>
       makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,
                        const xAOD::Vertex * constraint ) const = 0;
  };
}

#endif

