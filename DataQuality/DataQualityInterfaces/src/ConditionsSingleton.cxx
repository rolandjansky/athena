/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/ConditionsSingleton.h"
#include "TMap.h"
#include "TObjString.h"
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>
#include <set>
#include "boost/tokenizer.hpp"
#include <boost/algorithm/string.hpp>
#include "boost/algorithm/string/erase.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"

namespace{
  boost::mutex ConditionsSingletonMutex;
}
namespace dqi{
  ConditionsSingleton* ConditionsSingleton::s_instance = 0;
  
  ConditionsSingleton::ConditionsCleanup::~ConditionsCleanup() {
    boost::mutex::scoped_lock guard(ConditionsSingletonMutex);
    
    if (ConditionsSingleton::s_instance)
      {
	delete ConditionsSingleton::s_instance;
	ConditionsSingleton::s_instance=0;
      }
  }

  ConditionsSingleton& ConditionsSingleton::getInstance(){
    static ConditionsCleanup cleanup;
    if(true){
      boost::mutex::scoped_lock guard(ConditionsSingletonMutex);
      if(!s_instance){
	s_instance=new ConditionsSingleton();
      }
    }
    return *s_instance;
  }

  void ConditionsSingleton::setCondition(const std::string& cond){
    m_currentConditions=cond;
  }

  std::string& ConditionsSingleton::getCondition(){
    return m_currentConditions;
  }
  int ConditionsSingleton::getNumReferenceHistos(){
    return m_numRefHisto;
  }

  void ConditionsSingleton::setRefSourceMapping(const TMap* refsourcedata) {
    if (! refsourcedata) {
      std::cerr << "You are setting a bad refsourcedata! This will cause you trouble later!" << std::endl;
    }
    m_refsourcedata = refsourcedata;
  }

  std::string ConditionsSingleton::getRefSourceData(const std::string& rawref) {
    if (! m_refsourcedata) {
      // Suppress error message as it will occur in cases of backwards compatibility
      // std::cerr << "Null refsourcedata: THIS IS REALLY BAD!!!" << std::endl;
      return "";
    }
    const TObjString* value = dynamic_cast<TObjString*>(m_refsourcedata->GetValue(rawref.c_str()));
    if (!value) {
      std::cerr << "Unable to figure out refsource mapping: THIS IS ALSO REALLY BAD!!!" << std::endl;
      std::cerr << "This happened for reference " << rawref << std::endl;
      return "";
    }
    return value->GetName();
  }

  std::string ConditionsSingleton::getNewRefHistoName(){
    std::stringstream oss;
    oss<<"reference-"<<std::setfill('0')<<std::setw(9)<<m_numRefHisto;
    m_numRefHisto++;
    //std::cout<<__PRETTY_FUNCTION__<<"new histo name"<<oss.str()<<std::endl;
    return oss.str();
  }

  std::vector<std::string> ConditionsSingleton::getAllReferenceNames(std::string inp){
    boost::algorithm::erase_all(inp," ");
    //    if(cleanCond.empty())return inp;
    //std::set<std::string> referenceSet;
    std::vector<std::string> refs;

    if(inp.find('=')!=std::string::npos){//we have conditions defined on reference
      typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
      boost::char_separator<char> refSep(";");//field seperator for conditional references
      boost::char_separator<char> condSep(",:"); //filed seperator for conditions and respective reference
      tokenizer referenceConditionPairs(inp, refSep);
      std::string defaultRef("");
      for(tokenizer::iterator tok_iter=referenceConditionPairs.begin();
	  tok_iter!=referenceConditionPairs.end();++tok_iter){//look at each condition reference pair
	if(std::string((*tok_iter)).find("=")!=std::string::npos){//have conditions defined 
	  if(std::string(*tok_iter).find(":")!=std::string::npos){//do a reference split
	    std::vector<std::string> conds;
	    boost::split(conds,*tok_iter,boost::is_any_of(std::string(":")));
	    if(conds.size()>2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	    }else if(conds.size()<2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	      continue;
	    }
	    refs.push_back(conds.at(conds.size()-1));
	    //referenceSet.insert(conds.at(conds.size()-1));
	  }
	}else{// no = sign. Then this should be a default reference.
	  if(defaultRef.empty()){
	    defaultRef=*tok_iter;
	  }else{
	    std::cout<<"WARNING! overwriting old default reference \""
		     <<defaultRef<<"\" with \""
		     <<*tok_iter<<"\""<<std::endl;
	    defaultRef=*tok_iter;
	  }
	  std::cout<<"Default reference = "<<defaultRef<<std::endl;
	}
	
      }
      // for(std::set<std::string>::iterator it=referenceSet.begin();it!=referenceSet.end();++it){
      // 	refs.push_back(*it);
      // }
      if(!defaultRef.empty())refs.push_back(defaultRef);
      return refs;
    }else{
      refs.push_back(inp);
      return refs;
    }
  }

  void ConditionsSingleton::makeConditionMap(std::map<std::string, std::string>& cmap,
					     const std::string& condition){
    if (condition.empty()) return;

    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    std::string cleanCond(condition);
    boost::algorithm::erase_all(cleanCond," ");//delete all spaces
    boost::char_separator<char> condSep(","); //filed seperator for conditions and respective reference
    tokenizer conditionPairs(condition, condSep);
    for (tokenizer::const_iterator tok_iter = conditionPairs.begin();
	 tok_iter != conditionPairs.end(); ++tok_iter) {
      std::vector<std::string> splitpairs;
      boost::split(splitpairs, *tok_iter, boost::is_any_of(std::string("=")));
      if (splitpairs.size() != 2) {
	std::cerr << "WARNING: malformed condition \"" << *tok_iter << "\"" << std::endl;
	continue;
      } else {
	if (cmap.find(splitpairs[0]) != cmap.end()) {
	  std::cerr << "WARNING: redefinition of condition " << splitpairs[0] << std::endl;
	}
	cmap[splitpairs[0]] = splitpairs[1];
      }
    }
  }

  bool ConditionsSingleton::conditionsMatch(std::map<std::string, std::string>& refConds,
					    std::map<std::string, std::string>& currentConds) {
    for (std::map<std::string, std::string>::const_iterator cond_iter = refConds.begin();
	 cond_iter != refConds.end(); ++cond_iter) {
      // reject if reference key is not in current conditions
      if (currentConds.find((*cond_iter).first) == currentConds.end()) return false;
      if ((*cond_iter).second != currentConds[(*cond_iter).first]) return false;
    }
    // all specified reference conditions match
    return true;
  }
  
  std::string ConditionsSingleton::conditionalSelect(std::string inp,const std::string& condition){
    std::string cleanCond(condition);
    boost::algorithm::erase_all(cleanCond," ");//delete all spaces
    std::map<std::string, std::string> condition_map, reference_map;
    makeConditionMap(condition_map, condition);
    boost::algorithm::erase_all(inp," ");
    //    if(cleanCond.empty())return inp;
    if(inp.find('=')!=std::string::npos){//we have conditions defined on reference
      typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
      boost::char_separator<char> refSep(";");//field seperator for conditional references
      boost::char_separator<char> condSep(",:"); //filed seperator for conditions and respective reference
      tokenizer referenceConditionPairs(inp, refSep);
      std::string defaultRef("");
      std::map<std::string,std::string> conditionalReferences;
      for(tokenizer::iterator tok_iter=referenceConditionPairs.begin();
	  tok_iter!=referenceConditionPairs.end();++tok_iter){//look at each condition reference pair
	if(std::string((*tok_iter)).find("=")!=std::string::npos){//have conditions defined 
	  if(std::string(*tok_iter).find(":")!=std::string::npos){//do a reference split
	    std::vector<std::string> conds;
	    boost::split(conds,*tok_iter,boost::is_any_of(std::string(":")));
	    if(conds.size()>2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	    }else if(conds.size()<2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	      continue;
	    }
// 	    std::cout<<"Conditions = \""<<conds.at(conds.size()-2)
// 		     <<"\",reference= "<<conds.at(conds.size()-1)<<std::endl;
	    reference_map.clear();
	    makeConditionMap(reference_map, conds.at(conds.size()-2));
	    if (conditionsMatch(reference_map, condition_map)) {
	      return conds.at(conds.size()-1);
	    }
	    //conditionalReferences[conds.at(conds.size()-2)]=conds.at(conds.size()-1);
	  }
	}else{// no = sign. Then this should be a default reference.
	  if(defaultRef.empty()){
	    defaultRef=*tok_iter;
	  }else{
	    std::cout<<"WARNING! overwriting old default reference \""
		     <<defaultRef<<"\" with \""
		     <<*tok_iter<<"\""<<std::endl;
	    defaultRef=*tok_iter;
	  }
// 	  std::cout<<"Default reference = "<<defaultRef<<std::endl;
	}
      }
      //      if(cleanCond.empty()||conditionalReferences.find(cleanCond)==conditionalReferences.end()){
// 	std::cout<<"returning default reference "<<defaultRef<<std::endl;
//	return defaultRef;
//      }
//       std::cout<<"returning reference "<<conditionalReferences[cleanCond]<<std::endl;
//      return conditionalReferences[cleanCond];
      return defaultRef;
    }else{
      return inp;
    }
  }
  //
  //
  // This method is needed to replace the reference names in algorithm with conditions
  //

  std::vector<std::pair<std::string,std::string> > ConditionsSingleton::getConditionReferencePairs(std::string inp){
    std::vector<std::pair<std::string,std::string> > condPairs;
    std::map<std::string,std::string>  pairMap;//unique condition-reference pairs;
    boost::algorithm::erase_all(inp," ");
    std::string defaultRef("");
    if(inp.find('=')!=std::string::npos){//we have conditions defined on reference
      typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
      boost::char_separator<char> refSep(";");//field seperator for conditional references
      boost::char_separator<char> condSep(",:"); //filed seperator for conditions and respective reference
      tokenizer referenceConditionPairs(inp, refSep);
      std::map<std::string,std::string> conditionalReferences;
      for(tokenizer::iterator tok_iter=referenceConditionPairs.begin();
	  tok_iter!=referenceConditionPairs.end();++tok_iter){//look at each condition reference pair
	if(std::string((*tok_iter)).find("=")!=std::string::npos){//have conditions defined 
	  if(std::string(*tok_iter).find(":")!=std::string::npos){//do a reference split
	    std::vector<std::string> conds;
	    boost::split(conds,*tok_iter,boost::is_any_of(std::string(":")));
	    if(conds.size()>2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	    }else if(conds.size()<2){
	      std::cerr<<"Warning malformed reference \""<<inp<<"\""<<std::endl;
	      continue;
	    }
	    condPairs.push_back(std::make_pair(conds.at(conds.size()-2),conds.at(conds.size()-1)));
	    //pairMap[conds.at(conds.size()-2)]=conds.at(conds.size()-1);
	  }
	}else{// no = sign. Then this should be a default reference.
	  if(defaultRef.empty()){
	    defaultRef=*tok_iter;
	  }else{
	    std::cout<<"WARNING! overwriting old default reference \""
		     <<defaultRef<<"\" with \""
		     <<*tok_iter<<"\""<<std::endl;
	    defaultRef=*tok_iter;
	  }
	  // 	  std::cout<<"Default reference = "<<defaultRef<<std::endl;
	}
      }
      // for(std::map<std::string,std::string>::iterator it=pairMap.begin();it!=pairMap.end();++it){
      // 	condPairs.push_back(std::make_pair<std::string,std::string>(it->first,it->second));
      // }
      if(!defaultRef.empty())condPairs.push_back(std::make_pair("",defaultRef));
	  //return condPairs;
    }else{
      condPairs.push_back(std::make_pair("",inp));
    }
    return condPairs;
  }
  void ConditionsSingleton::setNewReferenceName(const std::string& oldName,const std::string& newName){
    if(oldName.empty()){
      std::cerr<<"Warning old name of the reference is empty. New name is \""<<newName<<"\""<<std::endl;
      return;
    }
    if(newName.empty()){
      std::cerr<<"Warning new name of the reference is empty. Old name is \""<<oldName<<"\""<<std::endl;
      return;
    }
    if(m_referenceMap.find(oldName)==m_referenceMap.end()){
      m_referenceMap[oldName]=newName;
    }else{
      std::cerr<<"Warning reference \""<<oldName<<"\" is added to map before as \""
	       << m_referenceMap[oldName]<<"\". New name is \""<<newName<<"\""<<std::endl;
      return;
    }
  }
  std::string ConditionsSingleton::getNewReferenceName(const std::string& oldName,bool quiet){
    std::string newName("");
    if(oldName.empty()){
      std::cerr<<"Reference must have a name"<<std::endl;
      return newName;
    }
    if(m_referenceMap.find(oldName)==m_referenceMap.end()){
      if(!quiet)std::cerr<<"Non-existent reference\""<<oldName<<"\". Returning empty string"<<std::endl;
      return newName;
    }
    return m_referenceMap[oldName];
  }

}

