/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/FilterParams.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/MonitorBase.h"
#include "LArCafJobs/Definitions.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
    
using namespace LArSamples;
using namespace std;


bool ChannelSpec::match(const CellInfo& info) const
{
  if (channel >= 0 && info.channel() != channel) return false;
  if (feedThrough >= 0 && info.feedThrough() != feedThrough) return false;
  if (slot >= 0 && info.slot() != slot) return false;
  if (calo != UNKNOWN_CALO && !Id::matchCalo(info.calo(), calo)) return false;
  return true;
}

bool FuncCut::pass(const Data& data) const 
{
  double val = func.val(data, args); 
  if (gotMin() && gotMax() && min > max) return (val == min); // for ==
  if (gotMin() && val < min) return false; // for > (Actually a >=)
  if (gotMax() && val >= max) return false; // for <
  return true;
}


FilterParams::FilterParams(const FilterParams& other) : 
      m_runs(other.m_runs), m_events(other.m_events), m_hashIds(other.m_hashIds), m_lbs(other.m_lbs), 
      m_calos(other.m_calos), m_partitions(other.m_partitions), m_channels(other.m_channels), m_layers(other.m_layers),
      m_gains(other.m_gains), m_status(other.m_status), m_requireSat(other.m_requireSat),
      m_cuts(other.m_cuts), m_requireConvergence(other.m_requireConvergence), 
      m_channelSel(other.m_channelSel), m_indexParity(other.m_indexParity) 
{ }


bool FilterParams::set(const char* selection)
{
  TString error = "Allowed syntax is \" condition1 && condition2 && ... && conditionN\n";
  error += "where conditionX is of the form status==XXX, saturate==yes|no, energy>XX, eta>XX, eta<XX, adcMax>XX, sampleMax>XX\n";
  error += "chi2<XX, time>XX, time<XX, converged=true, calo==XX, cell==[FT,slot,channel], run==XX, event=XX\n";
  error += "or an or list (item1 || item2 ||...  itemN) of such statements.";
  
  TString sel = selection, var = "", arg = "", op = "";
  
  sel.ReplaceAll(" ", "");
  TObjArray* tokens = sel.Tokenize("&&");
  for (int i = 0; i < tokens->GetEntries(); i++) {
    TString& token = ((TObjString*)tokens->At(i))->String();    
    if (TString(token[0]) == "(" && TString(token[token.Length() - 1]) == ")") {
      TObjArray* orTokens = TString(token(1, token.Length() - 2)).Tokenize("||");
      TString oring = "";
      for (int j = 0; j < orTokens->GetEntries(); j++) {
        TString& orToken = ((TObjString*)orTokens->At(j))->String();    
	if (!splitToken(orToken, var, op, arg)) {
	  cout << "could not process token " << orToken << endl << error << endl;
	  delete tokens;
          delete orTokens;
	  return false;
	}
        if (oring != "" && oring != var) {
          cout << "inconsistent or-list " << token << endl << error << endl;
          delete orTokens;
          delete tokens;          
          return false;
        }
	TString result = processToken(var, op, arg, oring);
	if (result != "") {
	  cout << "Error while processing token " << orToken << " : " << result << endl << error << endl;
	  delete tokens;
          delete orTokens;
	  return false;
	}
        if (oring == "")
          oring = var;
      }
      delete orTokens;
      continue;
    }
    if (!splitToken(token, var, op, arg)) {
      cout << "could not process token " << token << endl << error << endl;
      delete tokens;
      return false;
    }
    TString result = processToken(var, op, arg);
    if (result != "") {
      cout << "Error while processing token " << token << " : " << result << endl << error << endl;
      delete tokens;
      return false;
    }
  }
  delete tokens;
  return true;
}


bool FilterParams::splitToken(const TString& token, TString& var, TString& op, TString& arg)
{
  //cout << token << endl;
  if (token.Index("!=") >= 0) {
    TObjArray* neTokens = token.Tokenize("!=");
    if (neTokens->GetEntries() == 2) {
      var = ((TObjString*)neTokens->At(0))->String();
      arg = ((TObjString*)neTokens->At(1))->String();
      op = "!=";
      delete neTokens;
      return true;    
    }
    delete neTokens;
  }
  if (token.Index("==") >= 0) {
    TObjArray* eqTokens = token.Tokenize("==");
    if (eqTokens->GetEntries() == 2) {
      var = ((TObjString*)eqTokens->At(0))->String();
      arg = ((TObjString*)eqTokens->At(1))->String();
      op = "==";
      delete eqTokens;
      return true;    
    }
    delete eqTokens;
  }
  if (token.Index(">") >= 0) {
    TObjArray* gtTokens = token.Tokenize(">");
    if (gtTokens->GetEntries() == 2) {
      var = ((TObjString*)gtTokens->At(0))->String();
      arg = ((TObjString*)gtTokens->At(1))->String();
      op = ">";
      delete gtTokens;
      return true;    
    }
    delete gtTokens;
  }
  if (token.Index("<") >= 0) {
    TObjArray* ltTokens = token.Tokenize("<");
    if (ltTokens->GetEntries() == 2) {
      var = ((TObjString*)ltTokens->At(0))->String();
      arg = ((TObjString*)ltTokens->At(1))->String();
      op = "<";
      delete ltTokens;
      return true;    
    }
    delete ltTokens;
  }
  return false;
}


TString FilterParams::processToken(const TString& var, const TString& op, const TString& arg, const TString& oring)
{
  //cout << var << " " << op << " " << arg << " " << oring << endl;  
  if (var == "status" && op == "==") {
    if (m_status != 0xffffffff) return "Variable " + var + " already set";
    setStatus(arg.Atoi());
    return "";
  }
  if (var == "saturate" && op == "==") {
    if (m_requireSat != 0) return "Variable " + var + " already set";
    if (arg == "yes") { setRequireSaturation(); cout << "---> setRequireSaturation" << endl; return ""; }
    if (arg == "no") { setRequireNoSaturation(); cout << "---> setRequireNoSaturation" << endl;  return ""; }
    return "Invalid argument " + arg;
  }
  if (var == "cell") {
    if (oring != "cell" && !m_channels.empty()) return "Variable " + var + " already set";
    if (op == "==") {
      if (m_channelSel == -1) return "Cannot have positive channel selections and vetos in the same command";
      cout << "---> set channel sel == 1" << endl;
      m_channelSel = 1;
    }
    if (op == "!=") {
      if (m_channelSel == +1) return "Cannot have positive channel selections and vetos in the same command";
      cout << "---> set channel sel == -1" << endl;
      m_channelSel = -1;
    }    
    TObjArray* chTokens = nullptr;
    if (TString(arg[0]) == "[" || TString(arg[arg.Length() - 1]) == "]")
      chTokens = TString(arg(1, arg.Length() - 2)).Tokenize(",");
    else 
      chTokens = arg.Tokenize("/");
    if (chTokens->GetEntries() != 4) { delete chTokens; return "Invalid argument " + arg; }
    TString calo = ((TObjString*)chTokens->At(0))->String();
    TString ft   = ((TObjString*)chTokens->At(1))->String();
    TString slot = ((TObjString*)chTokens->At(2))->String();
    TString channel = ((TObjString*)chTokens->At(3))->String();
    delete chTokens;
    CaloId caloId = Id::caloId(calo);
    if (caloId == UNKNOWN_CALO) return "Invalid argument " + arg;
    int ftNum = -99, slotNum = -99, channelNum = -99;
    if (ft.IsDigit()) 
      ftNum = ft.Atoi();
    else if (ft == "?" || ft == "x" || ft == "-1") 
      ftNum = -1;    
    if (ftNum == -99) return "Invalid FT argument " + ft;
    if (slot.IsDigit()) 
     slotNum = slot.Atoi();
    else if (slot == "?" || slot == "x" || slot == "-1") 
      slotNum = -1;    
    if (slotNum == -99) return "Invalid slot argument " + slot;
    if (channel.IsDigit()) 
      channelNum = channel.Atoi();
    else if (channel == "?" || channel == "x" || channel == "-1") 
      channelNum = -1;    
    if (channelNum == -99) return "Invalid channel argument " + channel;
    addChannel(caloId, ftNum, slotNum, channelNum);
    cout << "---> addChannel " << Id::str(caloId) << " " << ftNum << " " << slotNum << " " << channelNum << endl;
    return "";
  }
  if (var == "calo" && op == "==") {
    if (oring != "calo" && !m_calos.empty()) return "Variable " + var + " already set";
    CaloId caloId = Id::caloId(arg);
    if (caloId == UNKNOWN_CALO) return "Invalid argument " + arg;
    addCalo(caloId);
    cout << "---> addCalo " << Id::str(caloId) << endl;
    return "";
  }
  if (var == "partition" && op == "==") {
    if (oring != "partition" && !m_calos.empty()) return "Variable " + var + " already set";
    PartitionId partId = Id::partitionId(arg);
    if (partId == UNKNOWN_PARTITION) return "Invalid argument " + arg;
    addPartition(partId);
    cout << "---> addPartition " << Id::str(partId) << endl;
    return "";
  }
  if (var == "layer" && op == "==") {
    if (oring != "layer" && !m_layers.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addLayer(arg.Atoi());
    cout << "---> addLayer " << arg.Atoi() << endl;
    return "";
  }
  if (var == "gain" && op == "==") {
    if (oring != "gain" && !m_gains.empty()) return "Variable " + var + " already set";
    CaloGain::CaloGain gain = CaloGain::UNKNOWNGAIN;
    if (arg == "HIGH")   gain = CaloGain::LARHIGHGAIN;
    if (arg == "MEDIUM") gain = CaloGain::LARMEDIUMGAIN;
    if (arg == "LOW")    gain = CaloGain::LARLOWGAIN;
    if (gain == CaloGain::UNKNOWNGAIN) return "Invalid gain argument " + arg + ", expecting HIGH/MEDIUM/LOW";
    addGain(gain);
    cout << "---> addGain " << Data::gainStr(gain) << endl;
    return "";
  }
  if (var == "runNum" && op == "==") {
    if (oring != "runNum" && !m_runs.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addRun(arg.Atoi());
    cout << "---> addRun " << arg.Atoi() << endl;
    return "";
  }
  if (var == "eventNum" && op == "==") {
    if (oring != "eventNum" && !m_events.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addEvent(arg.Atoi());
    cout << "---> addEvent " << arg.Atoi() << endl;
    return "";
  }
  if (var == "eventSpec" && op == "==") {
    if (oring != "eventSpec" && !m_events.empty()) return "Variable " + var + " already set";
    TObjArray* evTokens = arg.Tokenize("/");
    if (evTokens->GetEntries() != 2) { delete evTokens; return "Invalid argument " + arg; }
    TString evt = ((TObjString*)evTokens->At(0))->String();
    TString run = ((TObjString*)evTokens->At(1))->String();
    delete evTokens;
    if (!run.IsDigit()) return "Invalid run argument in " + arg;
    if (!evt.IsDigit()) return "Invalid event argument in " + arg;
    addRun(run.Atoi());
    addEvent(evt.Atoi());
    cout << "---> addRun/addEvent " << run.Atoi() << "/" << evt.Atoi() << endl;
    return "";
  }
  if (var == "eventFile" && op == "==") {
    if (oring != "eventFile" && !m_events.empty()) return "Variable " + var + " already set";
    ifstream fs(arg);
    if (!fs.is_open() || fs.eof()) return "File " + arg + " does not contain a valid event list";
    while (!fs.eof()) {
      char line[200];
      fs.getline(line, 200);
      //line = line.Substr(0, line.Length() - 1); // remove trailing linefeed
      TObjArray* evTokens = arg.Tokenize("/");
      if (evTokens->GetEntries() != 2) { delete evTokens; return "Invalid argument " + arg; }
      TString evt = ((TObjString*)evTokens->At(0))->String();
      TString run = ((TObjString*)evTokens->At(1))->String();
      delete evTokens;
      if (!run.IsDigit()) return "Invalid run argument in " + arg;
      if (!evt.IsDigit()) return "Invalid event argument in " + arg;
      addRun(run.Atoi());
      addEvent(evt.Atoi());
      cout << "---> addRun/addEvent " << run.Atoi() << "/" << evt.Atoi() << endl;
    }
    fs.close();
  }
  if (var == "lb" && op == "==") {
    if (oring != "lb" && !m_lbs.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addLB(arg.Atoi());
    cout << "---> addLB " << arg.Atoi() << endl;
    return "";
  }
  if (var == "hashId" && op == "==") {
    if (oring != "hashId" && !m_hashIds.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addHashId(arg.Atoi());
    cout << "---> addHashId " << arg.Atoi() << endl;
    return "";
  }
  if (var == "hashId" && op == "<") {
    if (oring != "hashId" && !m_hashIds.empty()) return "Variable " + var + " already set";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    addHashIdRange(0, arg.Atoi());
    cout << "---> addHashIdRange 0 " << arg.Atoi() << endl;
    return "";
  }
  if (var == "indexParity" && op == "==") {
    if (oring != "") return "Variable " + var + " cannot be or'ed";
    if (!arg.IsDigit()) return "Invalid argument " + arg;
    setIndexParity(arg.Atoi());
    cout << "---> indexParity " << arg.Atoi() << endl;
    return "";
  }

  std::vector<TString> vars;
  std::vector<DataFuncSet> funcs;
  std::vector<DataFuncArgs> args;
  bool status = MonitorBase::parseVariables(var, vars, funcs, args);
  if (status && vars.size() == 1) {
    std::map<TString, FuncCut>::iterator cut = m_cuts.find(var);
    if (cut == m_cuts.end()) {
      m_cuts[var] = FuncCut(funcs[0], args[0]);
      cut = m_cuts.find(var);
    }
    if (op == ">") {
      if (cut->second.gotMin()) return "Min value for variable " + var + " was already set"; 
      cut->second.min = arg.Atof();
      cout << "---> set " << var << " >= " << arg.Atof() << endl;
    }
    if (op == "<") {
      if (cut->second.gotMax()) return "Max value for variable " + var + " was already set"; 
      cut->second.max = arg.Atof();
      cout << "---> set " << var << " < " << arg.Atof() << endl;
    }
    if (op == "==") {
      if (cut->second.gotMin() || cut->second.gotMax()) return "A cut for variable " + var + " was already set"; 
      cut->second.setEqual(arg.Atof());
      cout << "---> set " << var << " == " << arg.Atof() << endl;
    }
    return "";
  }

  return "Invalid variable or operator";
}


void FilterParams::addRunEventFromData(const Data& data)
{
  addRun(data.run());
  addEvent(data.event());
}


void FilterParams::addChannel(CaloId calo, int feedThrough, int slot, int channel)
{
  m_channels.emplace_back(calo, feedThrough, slot, channel);
}


void FilterParams::addChannels(const char* fileName)
{
  // bad channels list in usual format: 1 per line, | barrel_ec | pos_neg | ft | slot | channel | pb | 

  ifstream f(fileName);
  int barrel_ec, pos_neg, ft, slot, channel, status;
  std::string pb;
  
  while (!f.eof()) {
    f >> barrel_ec >> pos_neg >> ft >> slot >> channel >> status >> pb;
    //cout << barrel_ec << " " << pos_neg << " " << ft << " " <<  slot << " " << channel << " " << pb << endl;
    CaloId calo = UNKNOWN_CALO;
    if (barrel_ec == 0 && pos_neg == 0) calo = EMB_C;
    if (barrel_ec == 0 && pos_neg == 1) calo = EMB_A;
    if (barrel_ec == 1 && pos_neg == 0) calo = EMEC_C;
    if (barrel_ec == 1 && pos_neg == 1) calo = EMEC_A;
    if (calo != UNKNOWN_CALO) addChannel(calo, ft, slot, channel);
  }
  
/*  for (unsigned int k = 0; k < m_channels.size(); k++)
    cout << k << " " << Id::str(m_channels[k].calo) << " " <<  m_channels[k].feedThrough << " " << m_channels[k].slot << " " << m_channels[k].channel << endl;*/
}


bool FilterParams::pass(unsigned int hashId, const History& history, unsigned int k) const
{
  if (!passHash(hashId)) return false;
  if (!passCell(*history.cellInfo())) return false;
  if (!passEvent(*history.data(k))) return false;
  return true;
}


bool FilterParams::passHash(unsigned int hashId) const
{
  if (!m_hashIds.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_hashIds.size(); k++)
      if (m_hashIds[k].first <= (int)hashId && (int)hashId < m_hashIds[k].second) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }
  return true;
}


bool FilterParams::passEvent(const Data& data) const
{
  for (const auto& p : m_cuts) {
    if (!p.second.pass(data)) return false;
  }
  
  if (m_requireConvergence && data.adcMax() < 0) return false;
  if (!(m_status & data.status()) && data.status()) return false;
  if ((m_requireSat & 1) && data.maxSample() < m_saturation) return false;
  if ((m_requireSat & 2) && data.maxSample() > m_saturation) return false;
  if (m_indexParity > 0 && (data.index() % 2) != (m_indexParity % 2)) return false;
  if (!m_runs.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_runs.size(); k++)
      if (m_runs[k].first <= data.run() && data.run() < m_runs[k].second) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }
  
  if (!m_events.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_events.size(); k++)
      if (m_events[k].first <= data.event() && data.event() < m_events[k].second) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }

  if (!m_lbs.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_lbs.size(); k++)
      if (m_lbs[k].first <= data.lumiBlock() && data.lumiBlock() < m_lbs[k].second) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }
 
  if (!m_gains.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_gains.size(); k++)
      if (m_gains[k] == data.gain()) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }
  
  return true;
}


bool FilterParams::passCell(const CellInfo& info) const
{
  if (!m_calos.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_calos.size(); k++)
      if (Id::matchCalo(info.calo(), m_calos[k])) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }

  if (!m_partitions.empty()) {
    bool pass = false;
    for (unsigned int k = 0; k < m_partitions.size(); k++)
      if (info.partition() == m_partitions[k]) {
        pass = true;
        break;
      }
    if (!pass) return false;
  }
  
  if (!m_layers.empty()) {
    bool found = false;
    for (unsigned int k = 0; k < m_layers.size(); k++)
      if (m_layers[k] == info.layer()) {
        found = true;
        break;
      }
    if (!found) return false; 
  }
  
  if (!m_channels.empty()) {
    bool found = false;
    for (unsigned int k = 0; k < m_channels.size(); k++)
      if (m_channels[k].match(info)) {
        found = true;
        break;
      }
    //cout << found << " " << m_channelSel << endl;
    if (!found && m_channelSel == +1) return false;
    if ( found && m_channelSel == -1) return false;
  }
  
  return true;
}

