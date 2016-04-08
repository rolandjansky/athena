/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagSVInfoPlusTrackAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/SVTrackInfo.h"

namespace D3PD {


JetTagSVInfoPlusTrackAssociationTool::JetTagSVInfoPlusTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Rec::TrackParticle>(type, name, parent),
      m_finder (*this, "SVInfoPlus", true),
      m_svInfoPlus(nullptr),
      m_trkItr(0),
      m_trkEnd(0)
{
}


StatusCode JetTagSVInfoPlusTrackAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Rec::TrackParticle> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagSVInfoPlusTrackAssociationTool::book()
{

  /// book variable here
  /// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagSVInfoPlusTrackAssociationTool::reset (const  Jet& p)
{
  m_svInfoPlus = m_finder.getTagInfo<Analysis::SVInfoPlus> (p);

  m_trkEnd = 0;
  m_trkItr = 0;

  if(m_svInfoPlus){
 
    /// valid variable not filled
   //  if(!m_svInfoPlus->isValid()){
//       return StatusCode::SUCCESS;
//     }

    m_trkEnd=m_svInfoPlus->numTrackInfo();

  }  

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagSVInfoPlusTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd) return 0;

  Analysis::SVTrackInfo trackInfo = m_svInfoPlus->getTrackInfo(m_trkItr);
  const Rec::TrackParticle* trk = trackInfo.track();

  ++m_trkItr;

  if(!trk){
    ATH_MSG_WARNING("NULL track pointer in SVTrackInfo");
    //// no need to keep it synchronized.
    return next();
  }

  return trk;


}

} // namespace D3PD
 
