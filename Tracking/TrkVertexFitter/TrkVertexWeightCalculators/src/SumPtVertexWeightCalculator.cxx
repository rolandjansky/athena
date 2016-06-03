/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "TrkVertexWeightCalculators/SumPtVertexWeightCalculator.h"
 #include "VxVertex/VxTrackAtVertex.h"
 #include "TrkParameters/TrackParameters.h"

 #include "xAODTracking/Vertex.h"
 #include "xAODTracking/TrackParticle.h"
 
namespace Trk{
 
  StatusCode SumPtVertexWeightCalculator::initialize()
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
  
  StatusCode SumPtVertexWeightCalculator::finalize()
  {
    MsgStream log(msgSvc(), name());
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }
 
  //class constructor implementation
  SumPtVertexWeightCalculator::SumPtVertexWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p):
          AthAlgTool(t,n,p),
          m_doSumPt2Selection(true)
  {
    declareProperty("DoSumPt2Selection",m_doSumPt2Selection);
    declareInterface<IVertexWeightCalculator>(this);
  }
 
  //destructor
  SumPtVertexWeightCalculator::~SumPtVertexWeightCalculator(){}
   
  double  SumPtVertexWeightCalculator::estimateSignalCompatibility(const xAOD::Vertex& vertex)
  { 
    double total_pt=0;
    ATH_MSG_DEBUG("Estimating vertex sorting score from " << vertex.nTrackParticles() << " tracks at vertex.");
    for(const auto& elTrackParticle : vertex.trackParticleLinks()) {

      if (not elTrackParticle.isValid()) {
	ATH_MSG_WARNING("No valid link to tracks in xAOD::Vertex object. Skipping track for signal compatibility (may be serious).");
	continue;
      }

      const Trk::Perigee& perigee = (*elTrackParticle.cptr())->perigeeParameters();
      if (m_doSumPt2Selection)
      {
        total_pt+=std::pow(1./perigee.parameters()[Trk::qOverP]*sin(perigee.parameters()[Trk::theta])/1000.,2);
      }
      else
      {
        total_pt+= std::fabs(1./perigee.parameters()[Trk::qOverP])*sin(perigee.parameters()[Trk::theta])/1000.;
      }
    }
    return  total_pt;     
  }
  
   
 }///End!!!

