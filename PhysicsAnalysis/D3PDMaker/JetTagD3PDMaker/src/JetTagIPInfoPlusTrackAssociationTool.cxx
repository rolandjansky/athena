/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagIPInfoPlusTrackAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "Particle/TrackParticle.h"
#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfo/IPTrackInfo.h"

namespace D3PD {


JetTagIPInfoPlusTrackAssociationTool::JetTagIPInfoPlusTrackAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Rec::TrackParticle>(type, name, parent),
      m_finder (*this, "IPInfoPlus", true),
      m_trkItr (0),
      m_trkEnd (0)
{
  declareProperty("FillVariables", m_fillVariables=true);
  book().ignore();  // Avoid coverity warnings.
}


StatusCode JetTagIPInfoPlusTrackAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Rec::TrackParticle> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagIPInfoPlusTrackAssociationTool::book()
{

  if(m_fillVariables){

    CHECK( addVariable ("d0val", m_d0val) ); 
    CHECK( addVariable ("d0sig", m_d0sig) ); 
    CHECK( addVariable ("z0val", m_z0val) ); 
    CHECK( addVariable ("z0sig", m_z0sig) ); 
    CHECK( addVariable ("w2D", m_w2D) );
    CHECK( addVariable ("w3D", m_w3D) );
    CHECK( addVariable ("pJP", m_pJP) );
    CHECK( addVariable ("pJPneg", m_pJPneg) );
    CHECK( addVariable ("grade", m_grd) );
    CHECK( addVariable ("isFromV0", m_vzero) ); 
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagIPInfoPlusTrackAssociationTool::reset (const  Jet& p)
{
  m_ipInfoPlus = m_finder.getTagInfo<Analysis::IPInfoPlus> (p);
 
  m_trkEnd = 0;
  m_trkItr = 0;

  if(m_ipInfoPlus){
 
    /// valid variable not filled
   //  if(!m_ipInfoPlus->isValid()){
//       return StatusCode::SUCCESS;
//     }

    m_trkEnd=m_ipInfoPlus->numTrackInfo();

  }  

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Rec::TrackParticle* JetTagIPInfoPlusTrackAssociationTool::next()
{

  // reached the end
  if(m_trkItr == m_trkEnd) return 0;

  Analysis::IPTrackInfo trackInfo = m_ipInfoPlus->getTrackInfo(m_trkItr);
  const Rec::TrackParticle* trk = trackInfo.track();

  if(m_fillVariables){
    *m_d0val = trackInfo.d0Value();
    *m_d0sig = trackInfo.d0Significance();
    *m_z0val = trackInfo.z0Value();
    *m_z0sig = trackInfo.z0Significance();
    *m_w2D = trackInfo.trackWeight2D(); 
    *m_w3D = trackInfo.trackWeight3D();
    *m_pJP = trackInfo.trackProbJP(); 
    *m_pJPneg = trackInfo.trackProbJPneg();
    *m_grd = trackInfo.trackGrade(); /// implicite cast from Analysis::TrackGrade
    *m_vzero = trackInfo.isFromV0();
  }

  if(!trk){
    ATH_MSG_WARNING("NULL track pointer in IPTrackInfo, association terminated");
    //// if this happens we can hack it by creating a dummy track 
    ////  so that it does not terminate the association and keep it synchronized with the values above
  }

  ++m_trkItr;

  return trk;


}

} // namespace D3PD
 
