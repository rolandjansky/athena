/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/MultiSVInfoPlus.h"

namespace Analysis
{

  MultiSVInfoPlus::MultiSVInfoPlus() : BaseTagInfo(),
				       m_NGTinJet(-1),
				       m_NGTinSvx(-1), 
				       m_N2Tpair(-1),
				       m_normdist(-1.),
				       m_vtxInfo(std::vector<MSVVtxInfo*>()){}

  MultiSVInfoPlus::MultiSVInfoPlus(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
								 m_NGTinJet(-1),
								 m_NGTinSvx(-1), 
								 m_N2Tpair(-1),
								 m_normdist(-1.),
								 m_vtxInfo(std::vector<MSVVtxInfo*>())
  {
    m_tagLikelihood.clear();
  }


  MultiSVInfoPlus::MultiSVInfoPlus(const MultiSVInfoPlus& rhs) : BaseTagInfo(rhs),
								 m_NGTinJet(rhs.m_NGTinJet),
								 m_NGTinSvx(rhs.m_NGTinSvx), 
								 m_N2Tpair(rhs.m_N2Tpair),
								 m_normdist(rhs.m_normdist)
  {
    m_vtxInfo.clear();
    for(unsigned int i=0; i<rhs.m_vtxInfo.size(); ++i){
      m_vtxInfo.push_back(new MSVVtxInfo( *(rhs.m_vtxInfo[i]) ) );
    }
  }

  MultiSVInfoPlus& MultiSVInfoPlus::operator= (const MultiSVInfoPlus& rhs)
  {
    if (this!=&rhs)
      {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_NGTinJet = rhs.m_NGTinJet;
	m_NGTinSvx = rhs.m_NGTinSvx; 
	m_N2Tpair  = rhs.m_N2Tpair;
	m_normdist = rhs.m_normdist;
	for(unsigned int i=0; i<m_vtxInfo.size(); ++i)delete m_vtxInfo[i];
	m_vtxInfo.clear();
	for(unsigned int i=0; i<rhs.m_vtxInfo.size(); ++i){
	  m_vtxInfo.push_back(new MSVVtxInfo( *(rhs.m_vtxInfo[i]) ) );
	}
        m_vtxInfo = rhs.m_vtxInfo;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
      }
    return *this;
  }

  MultiSVInfoPlus& MultiSVInfoPlus::operator= (MultiSVInfoPlus&& rhs)
  {
    if (this!=&rhs)
      {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_NGTinJet = rhs.m_NGTinJet;
	m_NGTinSvx = rhs.m_NGTinSvx; 
	m_N2Tpair  = rhs.m_N2Tpair;
	m_normdist = rhs.m_normdist;
	for(unsigned int i=0; i<m_vtxInfo.size(); ++i)
          delete m_vtxInfo[i];
        m_vtxInfo = std::move(rhs.m_vtxInfo);
        m_tagJetInfoType=rhs.m_tagJetInfoType;
      }
    return *this;
  }

  MultiSVInfoPlus::~MultiSVInfoPlus(){
    for(unsigned int i=0; i<m_vtxInfo.size(); ++i)delete m_vtxInfo[i];
  }

   
}

