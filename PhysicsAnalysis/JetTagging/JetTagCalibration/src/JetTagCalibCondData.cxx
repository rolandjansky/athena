/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    this->deleteBdts();
}

void JetTagCalibCondData::deleteHistos() {
    MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
    std::map<std::string, TH1*>::const_iterator iter_hist;
    for(unsigned int i=0;i<m_histos.size();i++) {
      iter_hist = m_histos[i].begin();
      for(;iter_hist!=m_histos[i].end();++iter_hist) {
        log << MSG::DEBUG << "#BTAG# delete histo of tagger " << i << " for channel " << iter_hist->first << endmsg;
        delete iter_hist->second;
      }
    }
    m_histos.clear();
}

void JetTagCalibCondData::deleteBdts() {
    MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
    for( auto const &tagger : m_bdts) {
      for( auto const &channel : tagger.second) {
        log << MSG::DEBUG << "#BTAG# Delete BDT of tagger " << tagger.first << " for channel " << channel.first << endmsg;
        delete channel.second;
      }
    }
    m_bdts.clear();
}


void JetTagCalibCondData::resize(const std::vector<std::string>& taggers) {
    m_histos.reserve(taggers.size());
    for(uint i=0;i<taggers.size();i++) {
      m_taggers.push_back(taggers[i]);
      std::map<std::string, TH1* > hmap;
      m_histos.push_back(hmap);
    }
}

void JetTagCalibCondData::clear() {
    m_channelAliasesMap.clear();
}

void JetTagCalibCondData::addHisto(const unsigned int indexTagger, const std::string& name, TH1* obj) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");

  m_histos[indexTagger].insert(std::make_pair(name, obj));
  log << MSG::DEBUG << "#BTAG# histo added " << name << " with pointer " << obj << endmsg;
  log << MSG::DEBUG << "#BTAG# m_histos size " << m_histos.size() << endmsg;
}

void JetTagCalibCondData::addBdt(const std::string&tagger, const std::string& channel, MVAUtils::BDT* bdt) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# Adding BDT of " << tagger << " in cond data for channel " << channel << endmsg;
  m_bdts[tagger].insert(std::make_pair(channel, bdt));
  log << MSG::DEBUG << "#BTAG# m_bdts size " << m_bdts.size() << endmsg;
}

void JetTagCalibCondData::addIPRNN(const std::string& tagger, const std::string& channel, const std::string& calstring) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# Adding RNN of " << tagger << " in cond data for channel " << channel << endmsg;
  m_IP_RNNConfig[tagger].insert(std::make_pair(channel, calstring));
}

void JetTagCalibCondData::addInputVars(const std::string&tagger, const std::string& name, const std::vector<std::string> &input) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# Adding input variables of the BDT for " << tagger << " in cond data for " << name << endmsg;
  m_inputVars[tagger].insert(std::make_pair(name, input));
  log << MSG::DEBUG << "#BTAG# m_inputVars size " << m_inputVars.size() << endmsg;
}

void JetTagCalibCondData::addDL1NN(const std::string&tagger, const std::string& channel, const lwt::JSONConfig& obj) {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# JSONConfig in cond data with " << obj.layers.size() << " layers" << endmsg;
  log << MSG::DEBUG << "#BTAG# JSONConfig in cond data for tagger " << tagger << endmsg;
  log << MSG::DEBUG << "#BTAG# JSONConfig in cond data for channel " << channel << endmsg;
  m_DL1_NNConfig[tagger].insert(std::make_pair(channel, obj));
  log << MSG::DEBUG << "#BTAG# m_DL1_NNConfig size " << m_DL1_NNConfig.size() << endmsg;
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
  std::map<std::string, TH1*>::const_iterator iter_hist;
  for(unsigned int i=0;i<m_histos.size();i++) {
    iter_hist = m_histos[i].begin();
    for(;iter_hist!=m_histos[i].end();++iter_hist) {
      log << MSG::DEBUG << "#BTAG# histogram name: "<< iter_hist->first << " with pointer " << iter_hist->second << endmsg;
    }
  }
}

void JetTagCalibCondData::printBdtsStatus() const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# There are " << m_bdts.size() << " BDTs build with TTree retrieved from DB" << endmsg;
  for( auto const &tagger : m_bdts) {
    log << MSG::DEBUG << "#BTAG# BDT for Tagger " << tagger.first << endmsg;
    for( auto const &channel : tagger.second) {
      log << MSG::DEBUG << "#BTAG# Channel "<< channel.first << " has many trees " << channel.second->GetNTrees() << endmsg;
    }
  }
}

TH1* JetTagCalibCondData::retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# retrieving Histo for folder " << folder
	   << " channel " << channel << " and hname " << hname << endmsg;
  return this->retrieveTObject<TH1>(folder,channel,hname);
}

MVAUtils::BDT* JetTagCalibCondData::retrieveBdt(const std::string& tagger, const std::string& channel) const {
  MVAUtils::BDT* bdt(nullptr);
  std::string channelAlias = this->getChannelAlias(channel);
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# retrieving BDT for " << tagger
	   << " (channel " << channel << " -> " << channelAlias << ") " << endmsg;
  std::map< std::string , std::map<std::string, MVAUtils::BDT*>>::const_iterator mI;
  mI = m_bdts.find(tagger);
  if (mI != m_bdts.end()) {
    log << MSG::DEBUG << "#BTAG# " << tagger << " BDT config found"<< endmsg;
    std::map<std::string, MVAUtils::BDT*>::const_iterator mJ = mI->second.find(channelAlias);
    if (mJ != mI->second.end()) {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " BDT config found for jet collection " << channel << endmsg;
      bdt = mJ->second;
    }
    else {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " BDT config not found for jet collection " << channel << endmsg;
    }
  }
  else {
    log << MSG::DEBUG << "#BTAG# " << tagger << " BDT config not found"<< endmsg;
  }

  return bdt;
}

std::vector<std::string> JetTagCalibCondData::retrieveInputVars(const std::string& tagger, const std::string& channel, const std::string& hname) const {
  std::vector<std::string> inputVars;
  std::string channelAlias = this->getChannelAlias(channel);
  std::string fname = this->fullHistoName(channelAlias,hname);
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  log << MSG::DEBUG << "#BTAG# retrieving input variables of BDT for " << tagger
	   << " channel " << channel << " and hname " << fname << endmsg;
  std::map< std::string , std::map<std::string, std::vector<std::string>>>::const_iterator mI;
  mI = m_inputVars.find(tagger);
  if (mI != m_inputVars.end()) {
    log << MSG::DEBUG << "#BTAG# " << tagger << " BDT config found"<< endmsg;
    std::map<std::string, std::vector<std::string>>::const_iterator mJ = mI->second.find(fname);
    if (mJ != mI->second.end()) {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " BDT config found for jet collection " << channel << endmsg;
      inputVars = mJ->second;
    }
    else {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " BDT config not found for jet collection " << channel << endmsg;
    }
  }
  else {
    log << MSG::DEBUG << "#BTAG# " << tagger << " BDT config not found"<< endmsg;
  }

  return inputVars;
}

lwt::JSONConfig JetTagCalibCondData::retrieveDL1NN(const std::string& tagger, const std::string& channel) const {
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

std::string JetTagCalibCondData::retrieveIPRNN(const std::string& tagger, const std::string& channel) const {
  MsgStream log(Athena::getMessageSvc(), "JetTagCalibCondData");
  std::string  config;
  std::map< std::string , std::map<std::string, std::string>>::const_iterator mI;
  mI = m_IP_RNNConfig.find(tagger);
  if (mI != m_IP_RNNConfig.end()) {
    log << MSG::DEBUG << "#BTAG# " << tagger << "RNN config found"<< endmsg;
    std::map<std::string, std::string>::const_iterator mJ = mI->second.find(channel);
    if (mJ != mI->second.end()) {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " RNN config found for jet collection " << channel << endmsg;
      config = mJ->second;
    }
    else {
      log << MSG::DEBUG << "#BTAG# "<< tagger << " RNN config not found for jet collection " << channel << endmsg;
    }
  }
  else {
    log << MSG::DEBUG << "#BTAG# " << tagger << " RNN config not found"<< endmsg;
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




