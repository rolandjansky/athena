// Dear emacs, this is -*- c++ -*-
/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/

#ifndef ALP_FILEDATASOURCE_H
#define ALP_FILEDATASOURCE_H
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <unordered_map>
#include "hltinterface/DataSource.h"
#include "eformat/compression.h"

namespace EventStorage{
  class DataReader;
  class DataWriter;
  struct run_parameters_record;
}

namespace eformat{
  namespace read{
    class FullEventFragment;
  }
}

namespace ALPNS{

  class FileDataSource:public hltinterface::DataSource{
  public:
    FileDataSource();
    virtual ~FileDataSource();
    virtual bool configure(const boost::property_tree::ptree &args);
    virtual bool prepareForRun(const boost::property_tree::ptree &args);
    virtual void sendResult(const bool accept,const uint32_t l1id, const hltinterface::HLTResult& res);
  
    virtual uint32_t collect(std::vector<hltinterface::DCM_ROBInfo>& data,
			     const uint32_t lvl1_id, const std::vector<uint32_t>& ids) override;
  
    virtual uint32_t collect(std::vector<hltinterface::DCM_ROBInfo>& data, uint32_t lvl1_id) override;
  
    virtual void reserveROBData(const uint32_t lvl1_id, const std::vector<uint32_t>& ids) override;

    virtual  void getL1Result(std::vector<eformat::ROBFragment<const uint32_t*> > &l1r,
			      uint32_t &lvl1_id,
			      uint64_t &gid, 
			      uint64_t &lumiBlock) override;

    virtual bool finalize(const boost::property_tree::ptree &args);
    virtual bool prepareWorker(const boost::property_tree::ptree &args);
    virtual bool finalizeWorker(const boost::property_tree::ptree &args);
  private:
    bool nextFile();
    uint32_t* getNextEvent();
    bool skipEvents(uint num);
    bool m_loopFiles;
    int m_currFile;
    std::vector<std::string> *m_fileNames;
    std::set<uint32_t> *m_collectedRobs;
    EventStorage::DataReader* m_currReader;
    std::unique_ptr<EventStorage::run_parameters_record> m_runParams;
    std::unique_ptr<EventStorage::DataWriter> m_writer;
    eformat::read::FullEventFragment *m_currEvent;
    std::unordered_map<uint32_t, const uint32_t*> *m_IDmap; ///< The ID <-> ROB map    
    std::vector<eformat::ROBFragment<const uint32_t*> > *m_l1r; ///< The LVL1 result
    std::string m_outFileName;
    eformat::Compression m_comp;
    unsigned int m_compLevel;
    const uint32_t * m_blob;
    uint32_t m_stride;
    uint32_t m_start;
    uint32_t m_currEventInFile;
    int m_nMaxEvents;
    int m_nEventsToRead,m_nEvents;

  };
}

#endif
