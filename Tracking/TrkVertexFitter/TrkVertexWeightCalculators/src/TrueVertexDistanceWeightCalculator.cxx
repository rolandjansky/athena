/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "TrkVertexWeightCalculators/TrueVertexDistanceWeightCalculator.h"
 #include "VxVertex/VxTrackAtVertex.h"
 #include "GeneratorObjects/McEventCollection.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

namespace Trk{
 
  StatusCode TrueVertexDistanceWeightCalculator::initialize()
  {
    StatusCode sc = AthAlgTool::initialize();
    
    //initializing the AlgTool itself
    if(sc.isFailure())
    {
      msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endreq;
      return StatusCode::FAILURE;
    }
    
    msg(MSG::INFO)<<"Initialization successfull"<<endreq;
    return StatusCode::SUCCESS;
  }//end of initialize method
  
  StatusCode TrueVertexDistanceWeightCalculator::finalize()
  {
    MsgStream log(msgSvc(), name());
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }
 
  //class constructor implementation
  TrueVertexDistanceWeightCalculator::TrueVertexDistanceWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p):
          AthAlgTool(t,n,p),
          m_McEventCollectionName("TruthEvent")
  {
    declareProperty("McTruthCollection",m_McEventCollectionName);
    declareInterface<IVertexWeightCalculator>(this);
  }
 
  //destructor
  TrueVertexDistanceWeightCalculator::~TrueVertexDistanceWeightCalculator(){}
   
  double  TrueVertexDistanceWeightCalculator::estimateSignalCompatibility(const xAOD::Vertex& vertex)
  { 
    return mEstimateSignalCompatibility(vertex.position());
  }

  double TrueVertexDistanceWeightCalculator::mEstimateSignalCompatibility(const Amg::Vector3D& vtxPosition)
  {
    const McEventCollection * mcColl = 0;
    StatusCode sc = StatusCode::SUCCESS;
    sc = evtStore()->retrieve( mcColl, m_McEventCollectionName);

     if ( sc.isFailure() )
    {
      ATH_MSG_WARNING("Could not retrieve McEventCollection " << m_McEventCollectionName 
		      << " from StoreGate. Returning 0 distance.");
      return 0;
    } 

    McEventCollection::const_iterator itr = mcColl->begin();
    const HepMC::GenEvent* myEvent=(*itr);
    if (!myEvent)
    {
      msg(MSG::ERROR) << " Cannot find truth event...  Returning 0 distance... " << endreq;
      return 0;
    }
    
    HepMC::GenEvent::vertex_const_iterator Vert = myEvent->vertices_begin();
    ATH_MSG_DEBUG("Resulting MC seed:  x: " << (*Vert)->position().x()  << 
		  " y: " << (*Vert)->position().y() << 
		  " z: " << (*Vert)->position().z());
    double ztrue=(*Vert)->position().z();
  
    double z_vtx_pos=vtxPosition.z();

    double distance = fabs(z_vtx_pos - ztrue);
    
    ATH_MSG_DEBUG("Distance of pV from truth:" << distance);

    if (distance==0.)
    {
      ATH_MSG_WARNING(" Suspicious 0 distance.");
      return 1./0.000001;
    }
    
    ATH_MSG_DEBUG(" Returning distance: " << 1./distance);

    return 1./distance;

  }
   
}///End!!!

