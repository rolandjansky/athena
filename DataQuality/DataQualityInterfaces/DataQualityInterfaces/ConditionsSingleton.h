/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CONDITIONSSINGLETON_H
#define __CONDITIONSSINGLETON_H
// sami
#include <string>
#include <vector>
//#include <utility>
#include <map>
class TMap;

namespace dqi{
  class ConditionsSingleton
  {
  private:
    friend class ConditionsCleanup;
    class ConditionsCleanup{
    public:
      ~ConditionsCleanup();
    };
    static ConditionsSingleton *s_instance;
    
  ConditionsSingleton():m_numRefHisto(0),m_refsourcedata(0){}
    ~ConditionsSingleton() {} 
    ConditionsSingleton(const ConditionsSingleton &);
    ConditionsSingleton & operator=(const ConditionsSingleton &);
    int m_numRefHisto;
    std::string m_currentConditions;
    std::map<std::string,std::string> m_referenceMap;
    const TMap* m_refsourcedata;
  public:
    static ConditionsSingleton &getInstance();
    void makeConditionMap(std::map<std::string, std::string>& cmap,
			  const std::string& condition);
    bool conditionsMatch(std::map<std::string, std::string>& refConds,
			 std::map<std::string, std::string>& currentConds);
    std::string conditionalSelect(std::string inp,const std::string& condition);
    void setCondition(const std::string& c);
    std::string& getCondition();
    int getNumReferenceHistos();
    std::string getNewRefHistoName();
    std::vector<std::string> getAllReferenceNames(std::string inp);
    std::vector<std::pair<std::string,std::string> > getConditionReferencePairs(std::string inp);
    void setNewReferenceName(const std::string&,const std::string&);
    std::string getNewReferenceName(const std::string&,bool quiet=false);
    void setRefSourceMapping(const TMap* refsourcedata);
    std::string getRefSourceData(const std::string& rawref);
  };
}

#endif
