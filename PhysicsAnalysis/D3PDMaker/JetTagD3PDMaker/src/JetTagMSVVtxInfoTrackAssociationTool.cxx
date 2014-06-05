/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMSVVtxInfoTrackAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "JetTagInfo/MSVVtxInfo.h"
#include "JetTagInfo/SVTrackInfo.h"

namespace D3PD {


JetTagMSVVtxInfoTrackAssociationTool::JetTagMSVVtxInfoTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Analysis::MSVVtxInfo,Rec::TrackParticle>(type, name, parent),
      m_mSVVtxInfo(0),
      m_trkItr(0),
      m_trkEnd(0)
{

}


StatusCode JetTagMSVVtxInfoTrackAssociationTool::initialize()
{

  typedef MultiAssociationTool<Analysis::MSVVtxInfo,Rec::TrackParticle> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagMSVVtxInfoTrackAssociationTool::book()
{

  /// book variable here
  /// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagMSVVtxInfoTrackAssociationTool::reset (const  Analysis::MSVVtxInfo& info)
{


  m_mSVVtxInfo = &info;

  m_trkItr = 0;
  m_trkEnd=m_mSVVtxInfo->numTrackInfo();

    

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagMSVVtxInfoTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd) return 0;

  Analysis::SVTrackInfo trackInfo = m_mSVVtxInfo->getTrackInfo(m_trkItr);
  const Rec::TrackParticle* trk = trackInfo.track();

  ++m_trkItr;

  if(!trk){
    ATH_MSG_WARNING("NULL track pointer in SVTrackInfo");
    return next();
  }


  return trk;


}

} // namespace D3PD
 
