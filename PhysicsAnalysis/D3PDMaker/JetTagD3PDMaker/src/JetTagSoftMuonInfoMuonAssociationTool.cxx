/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagSoftMuonInfoMuonAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/SMTrackInfo.h"
#include "muonEvent/Muon.h"


namespace D3PD {


JetTagSoftMuonInfoMuonAssociationTool::JetTagSoftMuonInfoMuonAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Analysis::Muon>(type, name, parent),
      m_finder (*this, "SoftMuonTag", true),
      m_muItr(0),
      m_muEnd(0)
{
  declareProperty("FillVariables", m_fillVariables=true);
  book().ignore(); // Avoid coverity warnings.
}


StatusCode JetTagSoftMuonInfoMuonAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Analysis::Muon> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagSoftMuonInfoMuonAssociationTool::book()
{

  if(m_fillVariables){
    CHECK( addVariable ("w", m_w) ); /// is this different than TagBaseInfo::weight? added to JetTagNtuple!!
    CHECK( addVariable ("pTRel", m_pTRel) );  
    CHECK( addVariable ("dRJet", m_dRJet) );
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagSoftMuonInfoMuonAssociationTool::reset (const  Jet& p)
{

//   std::vector<const JetTagInfoBase*> infoVector = p.jetTagInfoVector();
//   for(unsigned int iInfo = 0; iInfo < infoVector.size(); iInfo++) {
//     if(0==infoVector[iInfo]) continue;
//     ATH_MSG_WARNING("JetTagInfoObjects: " << infoVector[iInfo]->infoType() << " is of type "
// 		    <<typeid(*(infoVector[iInfo])).name() );
//   }

  m_jet = &p;

  m_softMuonInfo = m_finder.getTagInfo<Analysis::SoftMuonInfo> (p);

  m_muEnd = 0;
  m_muItr = 0;

  if(m_softMuonInfo){
 
    /// valid variable not filled
   //  if(!m_ipInfoPlus->isValid()){
//       return StatusCode::SUCCESS;
//     }

    m_muEnd=m_softMuonInfo->numTrackInfo();

  }  

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Analysis::Muon* JetTagSoftMuonInfoMuonAssociationTool::next()
{

  // reached the end
  if(m_muItr == m_muEnd) return 0;

  Analysis::SMTrackInfo sminfo = m_softMuonInfo->getTrackInfo(m_muItr);
  const Analysis::Muon* muon = sminfo.muon();

  std::vector<double> l = sminfo.tagLikelihood();

  if(m_fillVariables){
    *m_w = -100;
    if(l.size()==1){
      *m_w =  l[0];
    }
    if(l.size()>1){
      double pb = l[0];
      double pu = l[1];
      double w=0;
      if(pb<=0.) {
	w = -30.;
      } else if (pu<=0.) {
	w = +100.;
      } else {
	w = log(pb/pu);
      }
      *m_w = w;
    }
    *m_pTRel = sminfo.pTrel();
    if(muon)*m_dRJet = m_jet->hlv().deltaR(muon->hlv());
    else *m_dRJet=9999;
  }

  if(!muon){
    ATH_MSG_WARNING("NULL muon pointer in SoftMuonInfo, association terminated");
    //// if this happens we can hack it by creating a dummy track 
    ////  so that it does not terminate the association
  }

  ++m_muItr;

  return muon;


}

} // namespace D3PD
 
