/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SICLUSTERIZATIONTOOL_TruthClusterizationFactory_C
#define SICLUSTERIZATIONTOOL_TruthClusterizationFactory_C
 
/******************************************************
     @class TruthClusterizationFactory
     @author Roland Jansky
     Package : SiClusterizationTool 
     Created : April 2016
     DESCRIPTION: Emulates NN evaluation from truth (for ITK studies)
********************************************************/
 

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include "GaudiKernel/ToolHandle.h"

#include <TString.h>

#include <vector>
#include <string>
#include <map>

//this is a typedef: no forward decl possible
#include "TrkParameters/TrackParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "InDetSimData/InDetSimDataCollection.h"

//class InDetSimDataCollection;

namespace CLHEP{
  class HepRandomEngine;
}

namespace InDet {
 
  class PixelCluster;

  static const InterfaceID IID_TruthClusterizationFactory("InDet::NnClusterizationFactory", 1, 0);
  
  class TruthClusterizationFactory : public AthAlgTool  {
     
   public:
     
     /** AlgTool interface methods */
     static const InterfaceID& interfaceID() { return IID_TruthClusterizationFactory; };
 
     TruthClusterizationFactory(const std::string& name,
                     const std::string& n, const IInterface* p);
     ~TruthClusterizationFactory() = default;
     
    virtual StatusCode initialize();
    virtual StatusCode finalize() { return StatusCode::SUCCESS; };
	
    std::vector<double> estimateNumberOfParticles(const InDet::PixelCluster& pCluster) const;

    std::vector<Amg::Vector2D> estimatePositions(const InDet::PixelCluster&) const;
                                                      
   private:
	/** IncidentSvc to catch begining of event and end of event */   
    SG::ReadHandleKey<InDetSimDataCollection> m_simDataCollectionName {this, "InputSDOMap", "PixelSDO_Map", "sim data collection name"};

  protected:
    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service used in TruthClusterizationFactory"};
    Gaudi::Property<std::string> m_rndmEngineName {this, "RndmEngine", "TruthClustering", "Random Engine Name"};
    ATHRNG::RNGWrapper*      m_rndmEngine{nullptr};
    Gaudi::Property<bool> m_usePUHits{this, "usePUHits", false, "Take into account PU hits as contributing particles for NN emulation"};

   };
   
}//end InDet namespace
 
 #endif
