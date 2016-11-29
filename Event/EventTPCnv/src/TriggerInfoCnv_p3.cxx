/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/TriggerInfo.h"
#include "EventTPCnv/TriggerInfoCnv_p3.h"
#include "EventTPCnv/vectorize.h"

void TriggerInfoCnv_p3::persToTrans(std::vector<unsigned int>::const_iterator &i, TriggerInfo* trans, bool bugcompat) const{
    trans->setStatusElement           (*i); ++i;
    trans->setExtendedLevel1ID        (*i); ++i;
    trans->setLevel1TriggerType       (*i); ++i;
	
//	std::cout<<"persToTrans of TriggerInfo\t"<<trans->m_level1TriggerType<<std::endl;

    {
      unsigned int size=(*i); ++i;
      std::vector<TriggerInfo::number_type> l1info (i, i+size);  i += size;
      trans->setLevel1TriggerInfo (std::move (l1info));
    }

    {
      unsigned int size=(*i); ++i;
      std::vector<TriggerInfo::number_type> l2info (i, i+size);  i += size;
      trans->setLevel2TriggerInfo (std::move (l2info));
    }
                                   
    {
      unsigned int size=(*i); ++i;
      std::vector<TriggerInfo::number_type> efinfo (i, i+size);  i += size;
      trans->setEventFilterInfo (std::move (efinfo));
    }
                                   
//	std::cout<<"persToTrans of TriggerInfo\t"<<trans->m_eventFilterInfo.size()<<std::endl;
    {
      unsigned int size=(*i); ++i;
      std::vector<std::string> names(size);
      std::vector<std::string> types(size);
      for (unsigned int w=0;w<size;w++) {
        UITostr(names[w],i, bugcompat);
        UITostr(types[w],i, bugcompat);
      }

      std::vector<bool> t;
      UITobitmap(t, i, bugcompat);

      std::vector<TriggerInfo::StreamTag> st;
      st.reserve (size);
      for (unsigned int w=0;w<size;w++) {
        st.emplace_back (std::move(names[w]), std::move(types[w]), t[w]);
      }

      trans->setStreamTags (std::move (st));
    }
}

void TriggerInfoCnv_p3::transToPers(const TriggerInfo* trans, std::vector<unsigned int> &pers) const{
    pers.push_back(trans->statusElement());
    pers.push_back(trans->extendedLevel1ID());
    pers.push_back(trans->level1TriggerType());

    {
      const std::vector<TriggerInfo::number_type>& l1info = 
        trans->level1TriggerInfo();
      // std::cout<<"transToPers of TriggerInfo \t"<<trans->m_level1TriggerType<<std::endl;
      pers.push_back(l1info.size());
      std::copy (l1info.begin(), l1info.end(), std::back_inserter (pers));
    }

    {
      const std::vector<TriggerInfo::number_type>& l2info = 
        trans->level2TriggerInfo();
      pers.push_back(l2info.size());
      std::copy (l2info.begin(), l2info.end(), std::back_inserter (pers));
    }

    {
      const std::vector<TriggerInfo::number_type>& efinfo = 
        trans->eventFilterInfo();
      pers.push_back(efinfo.size());
      std::copy (efinfo.begin(), efinfo.end(), std::back_inserter (pers));
    }

    {
      size_t sz = trans->streamTags().size();
      // std::cout<<"transToPers of TriggerInfo \t"<<trans->m_eventFilterInfo.size()<<std::endl;
      pers.push_back(sz);
      std::vector<bool> t; t.reserve(sz);
      for (unsigned int i = 0; i < sz; ++i) {
        const TriggerInfo::StreamTag& st = trans->streamTags()[i];
        strToUI(st.name(), pers);
        strToUI(st.type(), pers);
        t.push_back(st.obeysLumiblock());
        // std::cout<<"transToPers of TriggerInfo "<<trans->m_streamTags[i].m_obeysLumiblock<<std::endl;
      }
      bitmapToUI(t, pers);
    }
}


