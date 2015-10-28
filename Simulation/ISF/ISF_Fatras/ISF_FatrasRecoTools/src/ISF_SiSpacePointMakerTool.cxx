/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISF_SiSpacePointMakerTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasRecoTools/ISF_SiSpacePointMakerTool.h"

// Space points
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"

namespace iFatras{

  static const InterfaceID IID_IISF_SiSpacePointMakerTool
  ("ISF_SiSpacePointMakerTool", 252891434, 0);
  
  const InterfaceID& ISF_SiSpacePointMakerTool::interfaceID() { 
    return IID_IISF_SiSpacePointMakerTool; 
  }

  // Constructor with parameters:
  ISF_SiSpacePointMakerTool::ISF_SiSpacePointMakerTool(const std::string &type,
						       const std::string &name,
						       const IInterface *parent) :
    AthAlgTool(type,name,parent){
    
    declareInterface< ISF_SiSpacePointMakerTool>( this );
    
  }
  
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  // virtual destructor
  ISF_SiSpacePointMakerTool::~ISF_SiSpacePointMakerTool()
  {}
  
  //--------------------------------------------------------------------------
  StatusCode ISF_SiSpacePointMakerTool::initialize()  {
    StatusCode sc;
    sc = AlgTool::initialize();
    return sc;
  }
  
  //--------------------------------------------------------------------------
  StatusCode ISF_SiSpacePointMakerTool::finalize() {
    StatusCode sc = AlgTool::finalize(); 
    return sc;
  }
  
  void ISF_SiSpacePointMakerTool::fillPixelSpacePointCollection(const PlanarClusterCollection* clusters, SpacePointCollection* spacepointCollection){
    
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
