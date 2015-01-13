/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCalibAlgs/CompareModulesTDAQandBytestream.h"
#include "PixelCalibAlgs/PixelConvert.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelConditionsServices/IPixelTDAQSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "InDetIdentifier/PixelID.h"

CompareModulesTDAQandBytestream::CompareModulesTDAQandBytestream(const std::string& name,
                                                                 ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_bsSvc("PixelByteStreamErrorsSvc", name),
  m_tdaqSvc("PixelTDAQSvc", name),
  n_lumi_blocks(0),
  m_pixelID(0)
{
}

CompareModulesTDAQandBytestream::~CompareModulesTDAQandBytestream(){
}

StatusCode CompareModulesTDAQandBytestream::initialize(){

  ATH_MSG_DEBUG("initialize");

  if(!m_bsSvc.retrieve().isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve PixelByteStreamErrorsSvc");
    return StatusCode::FAILURE;
  }

  if(!m_tdaqSvc.retrieve().isSuccess()){
    ATH_MSG_FATAL("Unable to retrieve PixelTDAQSvc");
    return StatusCode::FAILURE;
  }

  if(!detStore()->retrieve( m_pixelID, "PixelID" ).isSuccess()){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CompareModulesTDAQandBytestream::execute(){

  ATH_MSG_DEBUG("execute");

  const EventInfo * evt = 0;
  if(evtStore()->retrieve(evt, "ByteStreamEventInfo").isFailure() || 0 == evt){
    ATH_MSG_ERROR("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  unsigned int run_number = evt->event_ID()->run_number();
  unsigned int lumi_block = evt->event_ID()->lumi_block();
  unsigned int my_hash = run_number * 10000 + lumi_block;
  ATH_MSG_DEBUG("run " << run_number
                << "\tlumi block "<< lumi_block
                << "\tmy_hash " << my_hash);

  //chek if this lumi block was already prozessed
  std::pair<std::map<unsigned int, unsigned int>::iterator, bool> lb
    = runs_and_lumi_blocks.insert(std::make_pair(my_hash, 0));
  if(!lb.second)
    return StatusCode::SUCCESS;

  ATH_MSG_DEBUG("new lumi block:  run " << run_number
                << " lumi block " << lumi_block
                << " my_hash " << my_hash);


  //new limi block
  //check for disabled modules
  unsigned int n_differences = 0;
  std::vector<std::string> disabled_modules_lb;
  for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i ++){

    bool bs = m_bsSvc->getReadEvents(IdentifierHash(i));
    bool td = m_tdaqSvc->tdaq_module_enabled(IdentifierHash(i));

    ATH_MSG_DEBUG("hash: " << i
                  << "\tbs: " << bs
                  << "\ttd: " << td
                  );

    if(bs != td){
      Identifier moduleId = m_pixelID->wafer_id(i);
      ATH_MSG_WARNING("Disagreement for module hash: " << i <<
                      ", OfflineID: " << PixelConvert::OfflineID(moduleId.get_identifier32().get_compact()) <<
                      ", OnlineID: " <<  PixelConvert::OnlineID(moduleId.get_identifier32().get_compact()) <<
                      "\t bytestream: " << bs <<
                      "\t tdaq: " << td);
      ++n_differences;
      std::string info = PixelConvert::OnlineID(moduleId.get_identifier32().get_compact());
      if(bs)
        info += "\t enabled";
      else
        info += "\t disabled";
      if(td)
        info += "\t enabled";
      else
        info += "\t disabled";

      disabled_modules_lb.push_back(info);
    }
  }
  if(n_differences > 0){
    ++n_lumi_blocks;
    lb.first->second = n_differences;
    disabled_modules[my_hash] = disabled_modules_lb;
  }

  return StatusCode::SUCCESS;
}

StatusCode CompareModulesTDAQandBytestream::finalize(){

  ATH_MSG_DEBUG("finalize");

  std::string line(80, '-');
    ATH_MSG_INFO(line);
  if(n_lumi_blocks > 0){
    ATH_MSG_WARNING("Disagreement between bytestream and tdaq in "
                 << n_lumi_blocks << " lumi blocks");
      ATH_MSG_WARNING(" |   run\tlumi block \tdisagreement in # modules");
    for(std::map<unsigned int, unsigned int>::const_iterator i
          = runs_and_lumi_blocks.begin(); i != runs_and_lumi_blocks.end(); ++i){
      if(i->second > 0){
        ATH_MSG_WARNING(" |  " << i->first / 10000
                        << "\t   " << i->first % 10000
                        << "\t\t\t" << i->second
                        << "\t\tbytestream\t   tdaq");
        for(unsigned int j=0; j<disabled_modules[i->first].size(); ++j)
          ATH_MSG_WARNING(" |\t\t\t\t" << disabled_modules[i->first][j]);
      }
    }
  }
  else
    ATH_MSG_INFO("No disagreement between bytestream and tdaq");
  ATH_MSG_INFO(line);

  return StatusCode::SUCCESS;
}
