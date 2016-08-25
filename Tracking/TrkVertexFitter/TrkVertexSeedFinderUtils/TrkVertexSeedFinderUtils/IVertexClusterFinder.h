/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMode1dFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXSEEDFINDERUTILS_IVERTEXCLUSTERFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_IVERTEXCLUSTERFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk
{

  //                                                          What does 1,0 mean here???
  static const InterfaceID IID_IVERTEXCLUSTERFINDER("IVertexClusterFinder", 1, 0);

  /**
   @class IVertexClusterFinder

   Interface tool for Vertex cluster utils found in same package

   @author lars.egholm.pedersen@cern.ch

   ------------------------------------
   Changes:

   David Shope <david.richard.shope@cern.ch> (2016-04-19)

   EDM Migration to xAOD - from Trk::Vertex to Amg::Vector3D

   */

  class VertexImage;

  class IVertexClusterFinder : virtual public IAlgTool {

     public:

       /** Virtual destructor */
       virtual ~IVertexClusterFinder(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IVERTEXCLUSTERFINDER; };

       //Get a vector of vertices based on input histogram, defined by hist_rs;
       //Will return bin coordinates, which can be coverted to coordinates at a 
       //later stage
       virtual std::vector<Amg::Vector3D> findVertexClusters( const VertexImage & image ) = 0;
  };
}

#endif

