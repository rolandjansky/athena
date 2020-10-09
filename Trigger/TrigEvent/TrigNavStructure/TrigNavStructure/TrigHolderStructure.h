// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TRIGHOLDERSTRUCTURE
#define TRIGNAVSTRUCTURE_TRIGHOLDERSTRUCTURE
#include <map>
#include <memory>
#include <string>

#include <boost/variant.hpp>

#include "AsgMessaging/AsgMessaging.h"
#include "TrigNavStructure/Types.h"
#include "TrigNavStructure/BaseHolder.h"
#include "TrigNavStructure/TriggerElement.h"

namespace HLT{
  class TrigHolderStructure : public asg::AsgMessaging {
  public:
    TrigHolderStructure();

    void reset();

    bool registerHolder(const std::shared_ptr<BaseHolder>& holder);

    template<typename HolderType = BaseHolder>
    HolderType* getHolder(class_id_type clid, const boost::variant<sub_index_type,std::string>& stiOrLabel) const {
      return getCastHolder<HolderType>(getBaseHolder(clid,getSubTypeIndex(clid,stiOrLabel)));
    }

    template<typename HolderType = BaseHolder>
    HolderType* getHolderForFeature(const TriggerElement::FeatureAccessHelper& fea) const {
      return getCastHolder<HolderType>(getBaseHolder(fea.getCLID(),fea.getIndex().subTypeIndex()));
    }

    template<typename HolderType = BaseHolder>
    std::vector<HolderType*> getAllHolders() const {
      std::vector<HolderType*> result;
      for(auto& clid_indexmap : m_holderByClidAndIndex){
	for(auto& index_holder : clid_indexmap.second){
	  result.push_back(getCastHolder<HolderType>(index_holder.second.get()));
	}
      }
      return result;
    }

    template<typename HolderType = BaseHolder>
    std::vector<HolderType*> getHoldersOfClid(class_id_type clid) const {
      std::vector<HolderType*> result;
      auto lookup_it = m_holderByClidAndIndex.find(clid);
      if(lookup_it == m_holderByClidAndIndex.end()) return result;

      for(auto& index_holder : lookup_it->second){
	result.push_back(getCastHolder<HolderType>(index_holder.second.get()));
      }
      return result;
    }


    sub_index_type getSubTypeIndex(class_id_type clid, const index_or_label_type& stiOrLabel) const {
      //short circuit for sub_index_type because that case is valid whether we have a map or not. (visitor only works with map)
      if (stiOrLabel.which() == 0){
	return boost::get<sub_index_type>(stiOrLabel);
      }

      auto lookup_it = m_lookupSubIndex.find(clid);
      if(lookup_it == m_lookupSubIndex.end()) return invalid_sub_index;

      auto it = lookup_it->second.find(boost::get<std::string>(stiOrLabel));
      if(it==lookup_it->second.end()) return invalid_sub_index;

      return  it->second;
    }

    std::string getLabel(class_id_type clid, const index_or_label_type& stiOrLabel) const {
      //short circuit for sub_index_type because that case is valid whether we have a map or not. (visitor only works with map)
      if (stiOrLabel.which() == 1){
	return boost::get<std::string>(stiOrLabel);
      }

      std::string invalid_label = "inavalid_label";

      auto lookup_it = m_lookupLabels.find(clid);
      if(lookup_it == m_lookupLabels.end()) return invalid_label;

      auto it = lookup_it->second.find(boost::get<sub_index_type>(stiOrLabel));
      if(it==lookup_it->second.end()) return invalid_label;

      return  it->second;
    }


  private:

    BaseHolder* getBaseHolder(class_id_type clid,sub_index_type sti) const;

    template<typename T>
    T* getCastHolder(BaseHolder* holder) const {
      if(!holder){
	return 0;
      }
      auto cast_holder = dynamic_cast<T*>(holder);
      if(!cast_holder){
	ATH_MSG_ERROR("cast failed");
      }
      return cast_holder;
    }

    typedef std::map<sub_index_type, std::shared_ptr<BaseHolder> > IndexToHolderMap;
    typedef std::map<std::string,sub_index_type> LabelToSubIndexMap;
    typedef std::map<sub_index_type,std::string> SubIndexToLabelMap;

    std::map<class_id_type, IndexToHolderMap> m_holderByClidAndIndex;

    std::map<class_id_type, SubIndexToLabelMap > m_lookupLabels;
    std::map<class_id_type, LabelToSubIndexMap > m_lookupSubIndex;

  };
}//end namespace HLT
#endif
