/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JobInfo.h"

#include "TROOT.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "Cintex/Cintex.h"
#include "Api.h"
#include "G__ci.h"
#include "Class.h"
#endif

//================ Constructor =================================================

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
namespace G__functionscope {
  extern int sm_tagdefining;
}
#endif

JobInfo::JobInfo(const std::string& name, ISvcLocator* pSvcLocator)
  :
  AthAlgorithm(name,pSvcLocator),
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  m_events(0),
  m_last_entries(0),
  m_max_entries(G__functionscope::sm_tagdefining),
  m_no_warnings(true),
#endif
  m_printFATAL(false)
{
  declareProperty("PrintFATAL", m_printFATAL,"flag to decide if a FATAL should be printed - protection for Tier0");
}

StatusCode JobInfo::execute()
{
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  int nc=Cint::G__ClassInfo::GetNumClasses();
  if ( m_events < 2 || nc != m_last_entries )
    {
      ATH_MSG_INFO("root's CINTDictionaryArray G__struct : " << nc << " entries are in use");
      ATH_MSG_INFO(" probably around " << m_max_entries << " entries are available (unreliable - use with care !");
      if ( m_no_warnings and nc > 0.9 * m_max_entries )
	{
	  ATH_MSG_WARNING("CINTDictionary : over 90% filled !!");
	  m_no_warnings=false;
	}
      // will be printed at most twice (m_last_entries will be equal to nc)
      if ( nc >= m_max_entries )
	{
	  if ( m_printFATAL )
	    ATH_MSG_FATAL("root's CINTDictionaryArray G__struct : too many entries !! You might experience strange crashes !!");
	  else
	    ATH_MSG_WARNING("root's CINTDictionaryArray G__struct : too many entries !! You might experience strange crashes !!");
	}
	
	if(msgLvl(MSG::DEBUG))
	  for ( int i=m_last_entries; i < nc; ++i )
	    ATH_MSG_DEBUG("class " << i << " " << Cint::G__ClassInfo(i).Fullname() );
	m_last_entries=nc;
    }
  ++m_events;
#endif  
  return StatusCode::SUCCESS;
}
