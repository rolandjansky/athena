/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  //                                                          What does 1,0 mean here???
  static const InterfaceID IID_IVERTEXIMAGEMAKER("IVertexImageMaker", 1, 0);

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

       /** Virtual destructor */
       virtual ~IVertexImageMaker(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVERTEXIMAGEMAKER; };

       //Return a vertex image from the tracks centered around the constraint
       virtual VertexImage makeVertexImage( const std::vector<const Trk::TrackParameters*>& parametersList,const xAOD::Vertex * constraint ) = 0;
  };
}

#endif

