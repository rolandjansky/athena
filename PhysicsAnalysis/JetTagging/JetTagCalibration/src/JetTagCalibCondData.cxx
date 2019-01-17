/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagCalibration/JetTagCalibCondData.h"
// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TObject.h"
#include "TH1.h"
#include "TTree.h"

/**
 * @file JetTagCalibCondData.cxx
 * @author emmanuel.le.guirriec@cern.ch
**/


namespace Analysis {

JetTagCalibCondData::JetTagCalibCondData()
  {
    MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
    log << MSG::DEBUG << "#BTAG# addChannelAlias : map size" << m_channelAliasesMap.size() << endmsg;
    m_channelAliasesMap.clear();
  }

JetTagCalibCondData::~JetTagCalibCondData() {
    this->deleteHistos();
}

void JetTagCalibCondData::deleteHistos() {
    std::map<std::string, TObject*>::const_iterator iter_hist;
    for(unsigned int i=0;i<m_histos.size();i++) {
      iter_hist = m_histos[i].begin();
      for(;iter_hist!=m_histos[i].end();++iter_hist) {
        MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
        log << MSG::DEBUG << "#BTAG# delete histo of " << iter_hist->first << endmsg;
        delete iter_hist->second;
      }
    }
}

void JetTagCalibCondData::resize(const std::vector<std::string> taggers) {
    m_histos.reserve(taggers.size());
    for(uint i=0;i<taggers.size();i++) {
      m_taggers.push_back(taggers[i]);
      std::map<std::string, TObject* > hmap;
      m_histos.push_back(hmap);
    }  
}

void JetTagCalibCondData::clear() {
    m_channelAliasesMap.clear();
}

void JetTagCalibCondData::addHisto(const unsigned int indexTagger, const std::string& name, TObject * obj) {
  m_histos[indexTagger].insert(std::make_pair(name, obj));
}

void JetTagCalibCondData::addDL1NN(const std::string&tagger, const std::string& channel, const lwt::JSONConfig& obj) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::WARNING << "#BTAG# JSONConfig in cond data with " << obj.layers.size() << " layers" << endmsg;
  log << MSG::WARNING << "#BTAG# JSONConfig in cond data for tagger " << tagger << endmsg;
  log << MSG::WARNING << "#BTAG# JSONConfig in cond data for channel " << channel << endmsg;
  m_DL1_NNConfig[tagger].insert(std::make_pair(channel, obj));
  log << MSG::WARNING << "#BTAG# m_DL1_NNConfig size " << m_DL1_NNConfig.size();
}

void JetTagCalibCondData::addChannelAlias(const std::string& channel, const std::string& alias) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# addChannelAlias : map size" << m_channelAliasesMap.size() << endmsg;
  log << MSG::DEBUG << "#BTAG# addChannelAlias : " << channel << " : " << alias << endmsg;
  std::pair<std::string,std::string> chan = std::make_pair(channel,alias);
  m_channelAliasesMap.insert(chan);
}

std::string JetTagCalibCondData::getChannelAlias(const std::string& originalChannel) const {
  std::string alias = originalChannel;
  std::map< std::string, std::string>::const_iterator pos;
  pos = m_channelAliasesMap.find(originalChannel);
  if (pos != m_channelAliasesMap.end()) alias = pos->second;
  return alias;
}

void JetTagCalibCondData::printAliasesStatus() const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# final registered aliases ";
  std::map<std::string,std::string>::const_iterator iter = m_channelAliasesMap.begin();
  for(;iter!=m_channelAliasesMap.end();++iter) {
    log << MSG::DEBUG << "#BTAG# Channel alias " << iter->first <<"->"<<iter->second;
  }
  log << endmsg;
}


void JetTagCalibCondData::printHistosStatus() const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# histograms retrieved from DB" << endmsg;
  std::map<std::string, TObject*>::const_iterator iter_hist;
  for(unsigned int i=0;i<m_histos.size();i++) {
    iter_hist = m_histos[i].begin();
    for(;iter_hist!=m_histos[i].end();++iter_hist) {
      log << MSG::DEBUG << "#BTAG# histogram name: "<< iter_hist->first << endmsg;
    }
  }
}

TH1* JetTagCalibCondData::retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const {
  return this->retrieveTObject<TH1>(folder,channel,hname);
}

lwt::JSONConfig JetTagCalibCondData::retrieveDL1NN(std::string& tagger, const std::string& channel) const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  lwt::JSONConfig  config;
  std::map< std::string , std::map<std::string, lwt::JSONConfig>>::const_iterator mI;
  mI = m_DL1_NNConfig.find(tagger);
  if (mI != m_DL1_NNConfig.end()) {
    log << MSG::DEBUG << "#BTAG# " << tagger << " NN config found"<< endmsg;
    std::map<std::string, lwt::JSONConfig>::const_iterator mJ = mI->second.find(channel);
    if (mJ != mI->second.end()) {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " NN config found for jet collection " << channel << endmsg;
      config = mJ->second;
    }
    else {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " NN config not found for jet collection " << channel << endmsg;
    }
  }
  else {
    log << MSG::DEBUG << "#BTAG# " << tagger << " NN config not found"<< endmsg;
  }

  return config;
}

std::string JetTagCalibCondData::channelName(const std::string& fname) const {
  const std::string delim("#");
  std::string::size_type sPos;
  sPos = fname.find_first_of(delim);
  std::string channel = fname.substr(0,sPos);
  return channel;
}

std::string JetTagCalibCondData::histoName(const std::string& fname) const {
  const std::string delim("#");
  std::string::size_type sPos;
  sPos = fname.find_first_of(delim);
  std::string hname = fname.substr(sPos+1);
  return hname;
}

std::string JetTagCalibCondData::fullHistoName(const std::string& channel, const std::string& histoName) const {
  std::string fname = channel + "#" + histoName;
  return fname;
}

}




