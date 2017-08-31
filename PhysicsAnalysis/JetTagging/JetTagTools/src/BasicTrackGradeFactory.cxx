/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/BasicTrackGradeFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "JetTagInfo/TrackGrade.h"
#include "JetTagInfo/TrackGradesDefinition.h"

#include "xAODTracking/TrackParticle.h"



namespace Analysis
{

BasicTrackGradeFactory::BasicTrackGradeFactory( const std::string& t, const std::string& n, const IInterface* p ) :
        AthAlgTool( t, n, p )
{

    declareProperty("useSharedHitInfo", m_useSharedHitInfo = false);
    declareProperty("nSharedBLayer", m_nSharedBLayer = 0);
    declareProperty("nSharedPix", m_nSharedPix = 0);
    declareProperty("nSharedSct", m_nSharedSct = 1);
    declareProperty("nSharedSi", m_nSharedSi = 999);

    declareInterface<ITrackGradeFactory>( this );
}

BasicTrackGradeFactory::~BasicTrackGradeFactory()
{}

StatusCode BasicTrackGradeFactory::initialize()
{

  std::vector<TrackGrade> myGrades;

  if (!m_useSharedHitInfo)
  {
    myGrades.push_back(TrackGrade(0,std::string("Good")));
  }
  else
  {
    myGrades.push_back(TrackGrade(0,std::string("Good")));
    myGrades.push_back(TrackGrade(1,std::string("Shared")));
  }
  
  m_trackGradesDefinition=TrackGradesDefinition(myGrades);

  return StatusCode::SUCCESS;
}

StatusCode BasicTrackGradeFactory::finalize()
{
    return StatusCode::SUCCESS;
}


const TrackGradesDefinition & BasicTrackGradeFactory::getTrackGradesDefinition() const
{
  return m_trackGradesDefinition;
}


  TrackGrade* BasicTrackGradeFactory::getGrade(const xAOD::TrackParticle & track, const xAOD::IParticle::FourMom_t &) const
{

  bool sharedClass(false);

  if (m_useSharedHitInfo)
  {

    uint8_t nbs=0, nps=0, nss=0;
    if (!track.summaryValue(nbs, xAOD::numberOfInnermostPixelLayerSharedHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfBLayerSharedHists for TrackGrade!");
    }

    if (!track.summaryValue(nps, xAOD::numberOfPixelSharedHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfPixelSharedHists for TrackGrade!");
    }

    if (!track.summaryValue(nss, xAOD::numberOfSCTSharedHits)){
      ATH_MSG_ERROR("#BTAG# Cannot retrieve numberOfSCTSharedHists for TrackGrade!");
    }
    int nsht =  nps+nss ;
    
    
    
    if(nbs>m_nSharedBLayer) {
      sharedClass = true;
    }
    if(nps>m_nSharedPix) {
      sharedClass = true;
    }
    if(nss>m_nSharedSct) {
      sharedClass = true;
    }
    if(nsht>m_nSharedSi) {
      sharedClass = true;
    }
  }
  
  const TrackGrade * gradeToReturn(0);

  if (sharedClass)
  {
//    gradeToReturn=trackGradesDefinition.getGrade(std::string("Shared"));
    gradeToReturn=m_trackGradesDefinition.getGrade(1);
  }
  else
  {
//    gradeToReturn=trackGradesDefinition.getGrade(std::string("Good"));
    gradeToReturn=m_trackGradesDefinition.getGrade(0);
  }
      
  if (gradeToReturn==0)
  {
    ATH_MSG_ERROR("#BTAG# Grade not found. Internal error... ");
  }

  return new TrackGrade(*gradeToReturn);
  
}
 
}

