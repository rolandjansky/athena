/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DummySeedFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderTools/DummySeedFinder.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkTrack/Track.h"
#include <cmath>


namespace Trk
{

  DummySeedFinder::DummySeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p)
  {   
  }
  
  
  DummySeedFinder::~DummySeedFinder()
  {
  }


  StatusCode DummySeedFinder::initialize() 
  { 
    ATH_MSG_INFO( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }


  StatusCode DummySeedFinder::finalize() 
  {
    ATH_MSG_INFO( "Finalize successful"  );
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D
  DummySeedFinder::findSeed(const std::vector<const Trk::Track*> & /* VectorTrk */,
                            const xAOD::Vertex * /* constraint */) const
  {
    return Amg::Vector3D(0.,0.,0.);
  }
  

  Amg::Vector3D
  DummySeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & /* perigeeList */,
                            const xAOD::Vertex * /* constraint */) const
  {
    return Amg::Vector3D(0.,0.,0.);
  }


  std::vector<Amg::Vector3D>
  DummySeedFinder::findMultiSeeds (const std::vector<const Trk::Track*>& /* vectorTrk */,
                                   const xAOD::Vertex * /* constraint */) const
  {
    std::vector<Amg::Vector3D> retvec;
    retvec.emplace_back(0.,0.,0. );
    return retvec;
  }


  std::vector<Amg::Vector3D>
  DummySeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* vectorTrk */,
                                  const xAOD::Vertex * /* constraint */) const
  {
    std::vector<Amg::Vector3D> retvec;
    retvec.emplace_back(0.,0.,0. );
    return retvec;
  }


}
