/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DummySeedFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderTools/DummySeedFinder.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <math.h>


namespace Trk
{

  DummySeedFinder::DummySeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p)
  {   
    declareInterface<IVertexSeedFinder>(this);
  }
  
  DummySeedFinder::~DummySeedFinder() {}
  
  StatusCode DummySeedFinder::initialize() 
  { 
    msg(MSG::INFO) << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode DummySeedFinder::finalize() 
  {
    msg(MSG::INFO) << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D DummySeedFinder::findSeed(const std::vector<const Trk::Track*> & /* VectorTrk */,const xAOD::Vertex * /* constraint */) {
    
    return Amg::Vector3D(0.,0.,0.);
    
  }
  
  Amg::Vector3D DummySeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & /* perigeeList */,const xAOD::Vertex * /* constraint */)
   {
    
    return Amg::Vector3D(0.,0.,0.);
    
  }

  std::vector<Amg::Vector3D> DummySeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */)
  {

    std::vector<Amg::Vector3D> retvec;
    retvec.push_back( Amg::Vector3D(0.,0.,0.) );

    return retvec;
  }

  std::vector<Amg::Vector3D> DummySeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */)
  {

    std::vector<Amg::Vector3D> retvec;
    retvec.push_back( Amg::Vector3D(0.,0.,0.) );

    return retvec;
  }
 
  
}
