/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarSiSpacePointMakerTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasRecoTools/PlanarSiSpacePointMakerTool.h"

// Space points
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"

namespace iFatras{

  static const InterfaceID IID_IPlanarSiSpacePointMakerTool
  ("PlanarSiSpacePointMakerTool", 252891434, 0);
  
  const InterfaceID& PlanarSiSpacePointMakerTool::interfaceID() { 
    return IID_IPlanarSiSpacePointMakerTool; 
  }

  // Constructor with parameters:
  PlanarSiSpacePointMakerTool::PlanarSiSpacePointMakerTool(const std::string &type,
						       const std::string &name,
						       const IInterface *parent) :
    AthAlgTool(type,name,parent){
    
    declareInterface< PlanarSiSpacePointMakerTool>( this );
    
  }
  
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  // virtual destructor
  PlanarSiSpacePointMakerTool::~PlanarSiSpacePointMakerTool()
  {}
  
  //--------------------------------------------------------------------------
  StatusCode PlanarSiSpacePointMakerTool::initialize()  {
    StatusCode sc;
    sc = AlgTool::initialize();
    return sc;
  }
  
  //--------------------------------------------------------------------------
  StatusCode PlanarSiSpacePointMakerTool::finalize() {
    StatusCode sc = AlgTool::finalize(); 
    return sc;
  }
  
  void PlanarSiSpacePointMakerTool::fillPixelSpacePointCollection(const PlanarClusterCollection* clusters, SpacePointCollection* spacepointCollection){
    
    IdentifierHash idHash = clusters->identifyHash(); 
    
    iFatras::PlanarClusterCollection::const_iterator clusStart = clusters->begin(); 
    iFatras::PlanarClusterCollection::const_iterator clusFinish = clusters->end(); 
    if ((*clusStart)->detectorElement()!=0) {
      // Since all the clusters in a PixelClusterCollection and in a pixel PlanarClusterCollection 
      // are from the same detector element, it is enough to test on the 
      // first cluster.
      spacepointCollection->reserve(spacepointCollection->size()+clusters->size());
      for(; clusStart!=clusFinish; ++clusStart){
	Trk::SpacePoint* sp(new InDet::PixelSpacePoint(idHash, *clusStart)); 
	spacepointCollection->push_back(sp);
      }
    }
  }
  
  
}
