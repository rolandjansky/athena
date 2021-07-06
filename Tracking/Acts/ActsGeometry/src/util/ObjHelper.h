/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// This file was largely imported from the Acts testing framework

#pragma once

#include <fstream>
#include <vector>
#include "Acts/Definitions/Algebra.hpp"


namespace ObjHelper {

  /// This is the counter struct for keeping track of the vertices
  struct VtnCounter
  {
    unsigned int vcounter  = 0;
    unsigned int vtcounter = 0;
    unsigned int ncounter  = 0;
  };

  /// This will write a vertex to the fstream
  /// @param stream is the stream where to write to
  /// @param vertex is the vertex to be written out
  /// @param cvertex is the current vertex number
  void
  writeVTN(std::ofstream&        stream,
           VtnCounter&           vtnCounter,
           double                scalor,
           const Acts::Vector3& vertex,
           const std::string&    vtntype = "v",
           bool                  point   = false);

  /// construct vertical faces
  /// this takes a range and constructs faces
  void
  constructVerticalFaces(std::ofstream&                   stream,
                         unsigned int                     start,
                         const std::vector<unsigned int>& vsides);

  /// This will write a planar face
  /// - normal is given by cross product
  ///
  /// @param stream is the stream where to write to
  /// @param face is the face to be written out
  /// @param cvertex is the current vertex number
  /// @param thickness is the (optional) thickness
  void
  writePlanarFace(std::ofstream&                     stream,
                  VtnCounter&                        vtnCounter,
                  double                             scalor,
                  const std::vector<Acts::Vector3>& vertices,
                  double                             thickness = 0.,
                  const std::vector<unsigned int>&   vsides    = {});

  /// This will write a cylindrical object
  ///
  /// @param stream is the stream where to write to
  void
  writeTube(std::ofstream&           stream,
            VtnCounter&              vtnCounter,
            double                   scalor,
            unsigned int             nSegments,
            const Acts::Transform3& transform,
            double                   r,
            double                   hZ,
            double                   thickness = 0.);

}  // enf of namespace

