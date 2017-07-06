/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanOutput_h
#define dqiHanOutput_h

#include <map>
#include <memory>
#include <set>
#include <string>

#include <TFile.h>
#include <TTree.h>

#include "dqm_core/Output.h"

class TDirectory;

namespace dqm_core {
  class Region;
}


namespace dqi {

class HanConfig;


class HanOutput : public dqm_core::Output {
public:

  typedef std::map<std::string,TSeqCollection *> DQOutputMap_t;

  HanOutput( std::string rootFileName, DQOutputMap_t* outputMap, TSeqCollection *outputList );
  
  virtual ~HanOutput();
  
  virtual void addListener( const std::string& name, dqm_core::OutputListener* listener )
    throw (dqm_core::Exception);
  
  virtual void addListener( const dqm_core::Parameter& parameter, dqm_core::OutputListener* listener );
  
  virtual void publishResult( const std::string& name, const dqm_core::Result& result )
    throw (dqm_core::Exception);

  virtual void flushResults();
  
  virtual void activate();
  
  virtual void deactivate();

  virtual void setConfig( HanConfig* config );
  
  virtual void publishMissingDQPars();

  virtual void setInput( TDirectory* input );
  
protected:
  
  class Result {
  public:
    Result( TDirectory* dir );
    virtual ~Result();
    virtual void fill( const dqm_core::Result& result );
    virtual void write();
    
  protected:
    void copyString( char* to, const std::string& from );
    std::auto_ptr<TTree> m_result;
    char* m_status;
    static const int s_charArrSize;
  };
  
  class RegionNameComp {
  public:
    bool operator()( const dqm_core::Region* a, const dqm_core::Region* b );
  };

  
  typedef std::map<std::string,dqm_core::Region*>                      DQParMap_t;
  typedef std::multimap<dqm_core::Region*,std::string,RegionNameComp>  DQRegMap_t;  
  typedef std::map<dqm_core::Region*,int,RegionNameComp>               DQRegCount_t;  
  typedef std::map<std::string,dqm_core::Result*>                      DQResultMap_t;
  typedef std::set<std::string>                                        DQParSet_t;

  std::string           m_fileName;
  std::auto_ptr<TFile>  m_file;
	bool                  m_retainUnpubData;
  
  DQParSet_t            m_unpublishedDQPars;
  DQParMap_t            m_dqPars;
  DQRegMap_t            m_dqRegs;
  DQRegCount_t          m_dqRegCounts;
  DQResultMap_t         m_dqResults;
  DQOutputMap_t *       m_outputMap;

  TSeqCollection *      m_outputList;
  HanConfig *           m_config;
  DQParSet_t            m_regexlist;
  TDirectory*     m_input;

private:

  HanOutput();
  
};

} // namespace dqi

#endif
