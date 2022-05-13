/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagCalibration/JetTagCalibCondData.h"

#include "TObject.h"
#include "TH1.h"
#include "TTree.h"

/**
 * @file JetTagCalibCondData.cxx
 * @author emmanuel.le.guirriec@cern.ch
**/


namespace Analysis {

JetTagCalibCondData::JetTagCalibCondData() :
  AthMessaging("JetTagCalibCondData") {
    ATH_MSG_DEBUG("#BTAG# addChannelAlias : map size" << m_channelAliasesMap.size());
    m_channelAliasesMap.clear();
}

JetTagCalibCondData::~JetTagCalibCondData() {
    m_histos.clear();
    m_bdts.clear();
}

void JetTagCalibCondData::resize(const std::vector<std::string>& taggers) {
    m_histos.resize(taggers.size());
    for(const std::string& t : taggers) {
      m_taggers.push_back(t);
    }
}

void JetTagCalibCondData::clear() {
    m_channelAliasesMap.clear();
}

void JetTagCalibCondData::addHisto(const unsigned int indexTagger, const std::string& name, std::unique_ptr<TH1> obj) {
  m_histos[indexTagger].insert(std::make_pair(name, std::move(obj)));
  ATH_MSG_DEBUG("#BTAG# histo added " << name << " with pointer " << obj.get()
                << ", m_histos size " << m_histos.size());
}

void JetTagCalibCondData::addBdt(const std::string&tagger, const std::string& channel, std::unique_ptr<MVAUtils::BDT> bdt) {
  m_bdts[tagger].insert(std::make_pair(channel, std::move(bdt)));
  ATH_MSG_DEBUG("#BTAG# Adding BDT of " << tagger << " in cond data for channel " << channel
                << ", m_bdts size " << m_bdts.size());
}

void JetTagCalibCondData::addIPRNN(const std::string& tagger, const std::string& channel, const std::string& calstring) {
  ATH_MSG_DEBUG("#BTAG# Adding RNN of " << tagger << " in cond data for channel " << channel);
  m_IP_RNNConfig[tagger].insert(std::make_pair(channel, calstring));
}

void JetTagCalibCondData::addInputVars(const std::string&tagger, const std::string& name, const std::vector<std::string> &input) {

  m_inputVars[tagger].insert(std::make_pair(name, input));
  ATH_MSG_DEBUG("#BTAG# Adding input variables of the BDT for " << tagger << " in cond data for " << name
                << ", m_inputVars size " << m_inputVars.size());
}

void JetTagCalibCondData::addDL1NN(const std::string&tagger, const std::string& channel, const lwt::JSONConfig& obj) {
  m_DL1_NNConfig[tagger].insert(std::make_pair(channel, obj));
  ATH_MSG_DEBUG("#BTAG# JSONConfig in cond data with " << obj.layers.size() << " layers"
                << " for tagger " << tagger << " and channel " << channel
                << ", m_DL1_NNConfig size " << m_DL1_NNConfig.size());
}

void JetTagCalibCondData::addChannelAlias(const std::string& channel, const std::string& alias) {
  ATH_MSG_DEBUG("#BTAG# addChannelAlias : " << channel << " : " << alias);
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
  msg() << MSG::DEBUG << "#BTAG# final registered aliases ";
  std::map<std::string,std::string>::const_iterator iter = m_channelAliasesMap.begin();
  for(;iter!=m_channelAliasesMap.end();++iter) {
    msg() << MSG::DEBUG << "#BTAG# Channel alias " << iter->first <<"->"<<iter->second;
  }
  msg() << endmsg;
}

void JetTagCalibCondData::printHistosStatus() const {
  msg() << MSG::DEBUG << "#BTAG# histograms retrieved from DB" << endmsg;
  for( const auto& hists : m_histos ) {
    for( const auto& [name, hist] : hists) {
      msg() << MSG::DEBUG << "#BTAG# histogram name: "<< name << " with pointer " << hist.get() << endmsg;
    }
  }
}

void JetTagCalibCondData::printBdtsStatus() const {
  msg() << MSG::DEBUG << "#BTAG# There are " << m_bdts.size() << " BDTs build with TTree retrieved from DB" << endmsg;
  for( auto const &tagger : m_bdts) {
    msg() << MSG::DEBUG << "#BTAG# BDT for Tagger " << tagger.first << endmsg;
    for( auto const &channel : tagger.second) {
      msg() << MSG::DEBUG << "#BTAG# Channel "<< channel.first << " has many trees " << channel.second->GetNTrees() << endmsg;
    }
  }
}

TH1* JetTagCalibCondData::retrieveHistogram(const std::string& folder, const std::string& channel, const std::string& hname) const {
  return this->retrieveTObject<TH1>(folder,channel,hname);
}

MVAUtils::BDT* JetTagCalibCondData::retrieveBdt(const std::string& tagger, const std::string& channel) const {
  MVAUtils::BDT* bdt(nullptr);
  std::string channelAlias = this->getChannelAlias(channel);

  ATH_MSG_DEBUG("#BTAG# retrieving BDT for " << tagger
                << " (channel " << channel << " -> " << channelAlias << ")");

  auto mI = m_bdts.find(tagger);
  if (mI != m_bdts.end()) {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " BDT config found");
    auto mJ = mI->second.find(channelAlias);
    if (mJ != mI->second.end()) {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " BDT config found for jet collection " << channel);
      bdt = mJ->second.get();
    }
    else {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " BDT config not found for jet collection " << channel);
    }
  }
  else {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " BDT config not found");
  }

  return bdt;
}

std::vector<std::string> JetTagCalibCondData::retrieveInputVars(const std::string& tagger, const std::string& channel, const std::string& hname) const {
  std::vector<std::string> inputVars;
  std::string channelAlias = this->getChannelAlias(channel);
  std::string fname = this->fullHistoName(channelAlias,hname);

  ATH_MSG_DEBUG("#BTAG# retrieving input variables of BDT for " << tagger <<
                " channel " << channel << " and hname " << fname);
  std::map< std::string , std::map<std::string, std::vector<std::string>>>::const_iterator mI;
  mI = m_inputVars.find(tagger);
  if (mI != m_inputVars.end()) {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " BDT config found");
    std::map<std::string, std::vector<std::string>>::const_iterator mJ = mI->second.find(fname);
    if (mJ != mI->second.end()) {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " BDT config found for jet collection " << channel);
      inputVars = mJ->second;
    }
    else {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " BDT config not found for jet collection " << channel);
    }
  }
  else {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " BDT config not found");
  }

  return inputVars;
}

lwt::JSONConfig JetTagCalibCondData::retrieveDL1NN(const std::string& tagger, const std::string& channel) const {
  lwt::JSONConfig  config;
  std::map< std::string , std::map<std::string, lwt::JSONConfig>>::const_iterator mI;
  mI = m_DL1_NNConfig.find(tagger);
  if (mI != m_DL1_NNConfig.end()) {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " NN config found");
    std::map<std::string, lwt::JSONConfig>::const_iterator mJ = mI->second.find(channel);
    if (mJ != mI->second.end()) {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " NN config found for jet collection " << channel);
      config = mJ->second;
    }
    else {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " NN config not found for jet collection " << channel);
    }
  }
  else {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " NN config not found");
  }

  return config;
}

std::string JetTagCalibCondData::retrieveIPRNN(const std::string& tagger, const std::string& channel) const {
  std::string  config;
  std::map< std::string , std::map<std::string, std::string>>::const_iterator mI;
  mI = m_IP_RNNConfig.find(tagger);
  if (mI != m_IP_RNNConfig.end()) {
    ATH_MSG_DEBUG("#BTAG# " << tagger << "RNN config found");
    std::map<std::string, std::string>::const_iterator mJ = mI->second.find(channel);
    if (mJ != mI->second.end()) {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " RNN config found for jet collection " << channel);
      config = mJ->second;
    }
    else {
      ATH_MSG_DEBUG("#BTAG# "<< tagger << " RNN config not found for jet collection " << channel);
    }
  }
  else {
    ATH_MSG_DEBUG("#BTAG# " << tagger << " RNN config not found");
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




