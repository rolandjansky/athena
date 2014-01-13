/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

// ROOT
#include "TSystem.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/HtmlAlg.h"

REGISTER_ANP_OBJECT(AlgEvent,HtmlAlg)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::HtmlAlg::HtmlAlg()
  :fDebug(false),
   fPrintAll(true),
   fConfigCount(0)
{
}

//-----------------------------------------------------------------------------
Anp::HtmlAlg::~HtmlAlg()
{
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Config(const Registry &reg)
{
  //
  // Get configuration
  //
  fReg = reg;
  
  reg.Get("HtmlAlg", "HtmlPath", fPathHtml);
  reg.Get("HtmlAlg", "Debug",    fDebug);
  reg.Get("HtmlAlg::RegIndex",   fRegIndex);
  reg.Get("HtmlAlg", "PrintAll", fPrintAll);
  
  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "HtmlAlg::ConfAlg", fConf, this);

  if(fPathHtml.empty()) {
    log() << "Config - no valid html path" << endl;
  }
  else {
    //
    // Check if directory exists
    //
    FileStat_t dstat;
    bool is_valid = false;

    if(gSystem -> GetPathInfo(fPathHtml.c_str(), dstat) == 0) {
      if(!(dstat.fMode & kS_IFDIR)) { 
	log() << "Config - bad directory status: " << fPathHtml << endl;
	exit(1);
      }
      else {
	is_valid = true;
      }
    }
    else {
      //
      // Attempt to mkdir    
      //
      if(gSystem -> mkdir(fPathHtml.c_str()) != 0) { 
	log() << "Config - failed to mdkir: " << fPathHtml << endl;
	exit(1);
      }
      else {
	is_valid = true;
      }
    }

    //
    // Initiliaze root and index HTML nodes
    //
    if(is_valid) {
      fNodeRoot  = Handle<HtmlNode>(new HtmlNode(fPathHtml, HtmlNode::kROOT));
      fNodeIndex = Handle<HtmlNode>(new HtmlNode("index.html", "index", HtmlNode::kHTML));
      fNodeRoot -> AddChild(fNodeIndex);

      log() << "Config - html path: " << fPathHtml << endl;
    }
    else {
      log() << "Config - invalid html directory: " << fPathHtml << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") { 
    return;
  }

  if(!fConf.valid()) { 
    log() << "Signal - invalid configuration algorithm" << endl;
    return;
  }

  if(fConfigCount == 0) {
    Set(fConf->GetConfig()); 
  }
  
  ++fConfigCount;
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Done()
{
  //
  // Create html page for each rate element
  //
  if(!fNodeRoot.valid() || !fNodeIndex.valid()) return;

  fNodeIndex -> GetCont() << "<pre>" << endl;

  //
  // Get list of html algorithms
  //
  vector<string> htmls;
  fReg.Get("HtmlAlg::HtmlList", htmls);

  log() << "Done - processing " << htmls.size() << " algorithm(s)" << endl;

  for(unsigned i = 0; i < htmls.size(); ++i) {
    cout << "   " << htmls.at(i) << endl;

    Handle<HtmlABC> alg = Factory<HtmlABC>::Instance().Get(htmls[i]);
    if(alg.valid()) { 
      fRegIndex.Merge(alg->GetIndexRegistry());
    }
  }

  //
  // Read and set dataset names
  //
  vector<string> dsets;
  fReg.Get("HtmlAlg::Datasets", dsets);
  
  for(unsigned i = 0; i < dsets.size(); ++i) {
    stringstream dkey;
    dkey << "Input dataset " << i;
    fRegIndex.Set(dkey.str(), dsets.at(i));
  }

  //
  // Write out content of index Registry
  //
  const std::vector<DataPair<string, string> > &str_vec = fRegIndex.GetStr();
  
  unsigned kwidth = 0;
  for(unsigned i = 0; i < str_vec.size(); ++i) {
    kwidth = max<unsigned>(kwidth, str_vec[i].Key().size());
  }

  for(unsigned i = 0; i < str_vec.size(); ++i) {
    fNodeIndex -> GetCont() << Anp::PadStrBack(str_vec[i].Key()+":", kwidth+1) 
			    << " " << str_vec[i].Data() << endl;
  }

  fNodeIndex -> GetCont() << endl;

  //
  // Write html pages
  //
  for(unsigned i = 0; i < htmls.size(); ++i) {
    const string &halg = htmls.at(i);
    Handle<HtmlABC> alg = Factory<HtmlABC>::Instance().Get(halg);
    if(alg.valid()) { 
      log() << "Done - " << halg << " - processing..."<< endl;
      alg->Write(fNodeIndex.ref(), *this);
      log() << "Done - " << halg << " - done" << endl;
    }
  }

  fNodeIndex -> GetCont() 
    << "\n\n"
    << "<a href=\"https://twiki.cern.ch/twiki/bin/view/Atlas/AtlasTriggerRates\">Trigger rate tools</a>"
    << " are supported by: atlas-trigger-rate-expert@cernSPAMNOT.ch" << endl
    << "</pre>" << endl;

  log() << "Done - releasing index node..." << endl;
  fNodeIndex.release();

  log() << "Done - writing out and releasing root node..." << endl;
  fNodeRoot -> Write();
  fNodeRoot.release();

  log() << "Done - finished." << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Set(const TrigMonConfig &config)
{
  //
  // Prepare trigger configuratuion web pages
  //
  if(!fNodeRoot.valid() || !fNodeIndex.valid()) return;

  // Copy configuration
  fConfig = config;

  //
  // Copy keys/variables for index page summary
  //
  vector<string> index_keys;
  fReg.Get("HtmlAlg::IndexKeys", index_keys);

  for(unsigned i = 0; i < index_keys.size(); ++i) {
    const std::string &key = index_keys[i]; std::string var;
    if(fConfig.readValue(key, var) && var.size() > 0) fRegIndex.Set(key, var);
  }
    
  map<string, TrigConfAlg> alg_name;
  map<string, vector<TrigConfAlg> > alg_type;

  if(fPrintAll) {
    //
    // Create html page for sequences and algorithms name/type
    //
    Handle<HtmlNode> rootN(new HtmlNode("ALG_NAME", HtmlNode::kLIST));
    Handle<HtmlNode> rootT(new HtmlNode("ALG_TYPE", HtmlNode::kLIST));
    Handle<HtmlNode> rootS(new HtmlNode("SEQ_ROOT", HtmlNode::kLIST));
      
    fNodeRoot -> AddChild(rootN);
    fNodeRoot -> AddChild(rootT);
    fNodeRoot -> AddChild(rootS);
      
      
    //
    // Create all html pages BEFORE filling because we need all cross links
    //
      
    //
    // Make SEQ html nodes
    //
    for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
      const TrigConfSeq &seq = fConfig.at<TrigConfSeq>(i);
      
      stringstream baseS;
      baseS << "SEQ_" << setfill('0') << setw(10) << seq.getId();
      
      Handle<HtmlNode> nodeS(new HtmlNode(baseS.str()+".html",
					  seq.getName(), HtmlNode::kHTML));
      rootS -> AddChild(nodeS);    
      nodeS -> SetBase(baseS.str());
      nodeS -> GetCont() << "<pre>";
      nodeS -> AddLink("top ", "../", false);
      nodeS -> AddLink("index", "index.html", false);
      nodeS -> GetCont() << "</pre>" << endl;

      for(unsigned int j = 0; j < seq.getNAlg(); ++j) {
	const TrigConfAlg &alg = seq.getAlg(j);
	
	alg_name[alg.getName()] = alg;
	  
	//
	// Fill list of unique algorithm instances by type
	//
	vector<TrigConfAlg> &algs = alg_type[alg.getType()];
	if(std::find(algs.begin(), algs.end(), alg) == algs.end()) {
	  algs.push_back(alg);
	}
      }
    }

    //
    // Make algorithm instance html nodes
    //
    for(map<string, TrigConfAlg>::iterator it = alg_name.begin(); it != alg_name.end(); ++it) {
      const TrigConfAlg &alg = it->second;
      
      stringstream baseA;
      baseA << "ALG_NAME_" << setfill('0') << setw(10) << alg.getNameId();
      
      Handle<HtmlNode> nodeA(new HtmlNode(baseA.str()+".html", alg.getName(), HtmlNode::kHTML));
      rootN -> AddChild(nodeA);
      nodeA -> SetBase(baseA.str());
      nodeA -> GetCont() << "<pre>";
      nodeA -> AddLink("top ", "../", false);
      nodeA -> AddLink("index", "index.html", false);
      nodeA -> GetCont() << "</pre>" << endl;
    }

    //
    // Make algorithm type html nodes
    //
    for(map<string, vector<TrigConfAlg> >::iterator it = alg_type.begin(); it != alg_type.end(); ++it) {
      const vector<TrigConfAlg> &alg_vec = it->second;
	
      if(!alg_vec.empty()) {
	const TrigConfAlg &alg = alg_vec.front();
	
	stringstream baseA;
	baseA << "ALG_TYPE_" << setfill('0') << setw(10) << alg.getTypeId();
	
	Handle<HtmlNode> nodeA(new HtmlNode(baseA.str()+".html", alg.getType(), HtmlNode::kHTML));
	rootT -> AddChild(nodeA);
	nodeA -> SetBase(baseA.str());
	nodeA -> GetCont() << "<pre>";
	nodeA -> AddLink("top ", "../", false);
	nodeA -> AddLink("index", "index.html", false);
	nodeA -> GetCont() << "</pre>" << endl;
      }
    }
  }

  //
  // Make chain html nodes
  //
    for(unsigned int i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);

    stringstream baseC;
    baseC << "CHN_" << setfill('0') << setw(10) << chain.getEncodedId();

    Handle<HtmlNode> nodeC(new HtmlNode(baseC.str()+".html", chain.getName(), HtmlNode::kHTML));
    fNodeRoot -> AddChild(nodeC);

    nodeC -> SetBase(baseC.str());
    nodeC -> GetCont() << "<pre>";
    nodeC -> AddLink("top ", "../", false);
    nodeC -> AddLink("index", "index.html", false);
    nodeC -> GetCont() << "</pre>" << endl;
  }

  //
  // Fill html pages AFTER web pages created above
  //
  for(unsigned int i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    Fill(fConfig.at<TrigConfChain>(i));
  }
  if(fPrintAll) {
    for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
      Fill(fConfig.at<TrigConfSeq>(i));
    }
    for(map<string, TrigConfAlg>::iterator it = alg_name.begin(); it != alg_name.end(); ++it) {
      FillName(it->second);
    }
    for(map<string, vector<TrigConfAlg> >::iterator it = alg_type.begin(); it != alg_type.end(); ++it) {
      FillType(it->second);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Fill(const TrigConfChain &chain)
{
  //
  // Write header for chain webpage
  //
  Handle<HtmlNode> nodeE = FindChnNode(chain.getName());
  if(!nodeE.valid()) {
    cerr << "HtmlAlg::Set - error! Missing chain node: " << chain.getName() << endl;
    return;
  }

  stringstream &outf = nodeE->GetCont();
  outf << "<pre>"        << endl
       << "Chain name:   " << std::left << chain.getName()    << endl
       << "Counter:      " << std::left << chain.getCounter() << endl;

  // Add links to lower chains
  outf << "Lower chains: " ;
  for(unsigned int j = 0; j < fConfig.size<TrigConfChain>(); ++j) {
    const TrigConfChain &lower_chain = fConfig.at<TrigConfChain>(j);
    if(chain.getLowerName() == lower_chain.getName()) {
      Handle<HtmlNode> nodeL = FindChnNode(lower_chain.getName());
      if(nodeL.valid()) outf << nodeL -> GetLink() << " "; 
    }
    
    std::vector<string> lchains;
    Anp::StringTok(lchains, chain.getLowerName(), ",");
    
    if(lchains.size() > 1) {
      vector<string>::iterator lit = std::find(lchains.begin(), lchains.end(), lower_chain.getName());
      if(lit != lchains.end()) {
	Handle<HtmlNode> nodeL = FindChnNode(*lit);
	if(nodeL.valid()) outf << nodeL -> GetLink() << " "; 
      }
    }
  }
  outf << endl;
  
  // Add links to upper chains
  outf << "Upper chains: " ;
  for(unsigned int j = 0; j < fConfig.size<TrigConfChain>(); ++j) {
    const TrigConfChain &upper_chain = fConfig.at<TrigConfChain>(j);
    if(upper_chain.getLowerName() == chain.getName()) {
      Handle<HtmlNode> nodeU = FindChnNode(upper_chain.getName());
      if(nodeU.valid()) outf << nodeU -> GetLink() << " "; 
    }
  }
  outf << endl;

  // Insert link to configuration
  if(fPrintAll) {
    stringstream confName;
    confName << "CHN_" << setfill('0') << setw(10) << chain.getId() << "_CONFIG.html";
    
    Handle<HtmlNode> nodeC(new HtmlNode(confName.str(), HtmlNode::kHTML));
    fNodeRoot -> AddChild(nodeC);      
    
    outf << endl << nodeC->GetLink("Chain configuration") << endl << endl;

    // Print signatures, sequences, algorithms
    nodeC->GetCont() << "<pre>" << endl
		     << "Chain: " << chain.getName() << endl << endl;
    
    const std::vector<TrigConfSig> &sig_list = chain.getSignature();
    for(unsigned int i = 0; i < sig_list.size(); ++i) {
      Fill(sig_list[i], nodeC->GetCont());
      nodeC->GetCont() << endl;
    }
    nodeC->GetCont() << "</pre>" << endl;
  }

  // Print groups and streams
  const std::vector<std::string> &gvec = chain.getGroup();
  const std::vector<std::string> &svec = chain.getStream();  

  outf << "Groups:  ";
  for(unsigned int i = 0; i < gvec.size(); ++i) {
    outf << gvec[i] << " ";
  }
  outf << endl;
  
  outf << "Streams: ";
  for(unsigned int i = 0; i < svec.size(); ++i) {
    outf << svec[i] << " ";
  }

  outf << endl;
  outf << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Fill(const TrigConfSig &sig, std::ostream &os)
{
  //
  // Write one signature step
  //
  os << "Signature step " << sig.getCounter() << ": " << sig.getLabel() << endl;

  // Write sequences with multiplicity
  map<TrigConfSeq, unsigned> tmap;

  const std::vector<uint32_t> &tes = sig.getOutputTEs();
  for(unsigned i = 0; i < tes.size(); ++i) {
    vector<TrigConfSeq>::const_iterator sit = fConfig.findSeq(tes[i]);
    if(sit == fConfig.end<TrigConfSeq>()) {
      cerr << "HtmlAlg::Fill - missing sequence: " << tes[i] << endl;
      continue;
    }
    
    tmap.insert(pair<TrigConfSeq, unsigned>(*sit, 0)).first->second++;
  }

  for(map<TrigConfSeq, unsigned>::const_iterator sit = tmap.begin(); sit != tmap.end(); ++sit) {
    const TrigConfSeq &seq = sit->first;
    Handle<HtmlNode> snode = FindSeqNode(seq.getName());
    
    string sname = seq.getName();
    if(snode.valid()) sname = snode->GetLink(sname);

    os << "   Sequence and multiplicty: " << sname << " X " << sit->second << endl;
    
    const std::vector<TrigConfAlg> &avec = seq.getAlg();
    unsigned int awidth = 0;

    for(unsigned j = 0; j < avec.size(); ++j) {
      const TrigConfAlg &alg = avec[j];
      awidth = max<unsigned>(awidth, alg.getName().size());
    }

    for(unsigned j = 0; j < avec.size(); ++j) {
      const TrigConfAlg &alg = avec[j];      
      string algN = Anp::PadStrBack(alg.getName(), awidth);
      string algT = alg.getType();

      Handle<HtmlNode> nodeN = FindAlgName(alg.getName());
      Handle<HtmlNode> nodeT = FindAlgType(alg.getType());      

      if(nodeN.valid()) algN = nodeN->GetLink(algN);
      if(nodeT.valid()) algT = nodeT->GetLink(algT);
      
      os << "      Algorithm instance/class: " << algN << "/" << algT << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::Fill(const TrigConfSeq &seq)
{
  //
  // Write header for sequence webpage
  //
  Handle<HtmlNode> nodeS = FindSeqNode(seq.getName());
  if(!nodeS.valid()) {
    cerr << "HtmlAlg::Fill - error! Missing seq node: " << seq.getName() << endl;
    return;
  }

  stringstream &os = nodeS->GetCont();
  os << "<pre>" << endl
     << "Sequence name:   " << seq.getName() << endl
     << "Sequence id:     " << seq.getId()   << endl
     << "Input sequences: ";

  for(unsigned int i = 0; i < seq.getInputTEs().size(); ++i) {
    Handle<HtmlNode> nodeI = FindSeqNode(seq.getInputTEs().at(i));  
    if(nodeI.valid()) os << nodeI->GetLink() << " ";
  }
  os << endl;

  os << endl << "Algorithm instance/class: " << endl;  
  for(unsigned int j = 0; j < seq.getNAlg(); ++j) {
    const TrigConfAlg &alg = seq.getAlg(j);
    
    Handle<HtmlNode> nodeN = FindAlgName(alg.getName());
    Handle<HtmlNode> nodeT = FindAlgType(alg.getType());

    if(nodeN.valid() && nodeT.valid()) { 
      os << "   " << nodeN->GetLink() << "/" << nodeT->GetLink() << endl; 
    }
  }
  os << endl;  
  
  // Add links to chains
  os << "Chains: " << endl;
  for(unsigned int j = 0; j < fConfig.size<TrigConfChain>(); ++j) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(j);
    
    if(chain.matchOutputTE(seq.getId())) {
      Handle<HtmlNode> nodeC = FindChnNode(chain.getName());
      if(nodeC.valid()) os << nodeC -> GetLink() << " "; 
    }
  }
  os << endl;  

  os << "<pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::FillName(const TrigConfAlg &alg)
{
  //
  // Fill web page for algorithm instance
  //
  Handle<HtmlNode> node = FindAlgName(alg.getName());
  if(!node.valid()) {
    cerr << "HtmlAlg::FillName - error! Missing alg node: " << alg.getName() << endl;
    return;
  }

  string alg_type = alg.getType();

  Handle<HtmlNode> nodeT = FindAlgType(alg_type);  
  if(nodeT.valid()) {
    alg_type = nodeT->GetLink(alg.getType());
  }

  stringstream &os = node->GetCont();
  os << "<pre>" << endl
     << "<b>Algorithm name: " << alg.getName() << "</b>" << endl
     << "C++ class name: " << alg_type << endl
     << endl;
  
  std::vector<string> seq_vec, chn_vec;
  
  for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = fConfig.at<TrigConfSeq>(i);
    
    for(unsigned j = 0; j < seq.getNAlg(); ++j) {
      if(alg.getName() == seq.getAlg(j).getName()) { 
	seq_vec.push_back(seq.getName());
      }
    }
  }

  for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
    
    bool matched = false;
    const vector<TrigConfSeq> chain_seq = Trig::GetSeq(chain, fConfig);
    for(unsigned j = 0; j < seq_vec.size(); ++j) {
      if(std::find(chain_seq.begin(), chain_seq.end(), seq_vec[j]) != chain_seq.end()) {
	matched = true;
	break;
      }
    }

    if(matched) chn_vec.push_back(chain.getName());
  }
  
  //
  // Make links to two html pages to reduce cluter:
  //  - one for parent sequence links
  //  - one for parent chain links
  //
  Handle<HtmlNode> nodeChn(new HtmlNode(node->GetBase()+"_exec_chn.html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeSeq(new HtmlNode(node->GetBase()+"_exec_seq.html", HtmlNode::kHTML));

  fNodeRoot->AddChild(nodeChn);
  fNodeRoot->AddChild(nodeSeq);

  stringstream strSeq, strChn;
  
  strChn << chn_vec.size() << " chain(s) execute this algorithm instance";
  strSeq << seq_vec.size() << " sequence(s) execute this algorithm instance";
  
  os << nodeChn->GetLink(strChn.str()) << endl
     << nodeSeq->GetLink(strSeq.str()) << endl << endl;

  nodeChn->GetCont() << "<pre>" << endl 
		     << chn_vec.size() << " chain(s) execute " << alg.getName() << ":"
		     << endl << endl;

  nodeSeq->GetCont() << "<pre>" << endl 
		     << seq_vec.size() << " sequence(s) execute " << alg.getName() << ":"
		     << endl << endl;
  
  Trig::PrintColumns(chn_vec, nodeChn->GetCont(), 1, fNodeRoot);
  Trig::PrintColumns(seq_vec, nodeSeq->GetCont(), 1, fNodeRoot->FindChild("SEQ_ROOT"));

  nodeChn -> GetCont() << "</pre>" << endl;
  nodeSeq -> GetCont() << "</pre>" << endl;

  // HACK - do not add closing </pre> - this will be inserted by HtmlNode.cxx
}

//-----------------------------------------------------------------------------
void Anp::HtmlAlg::FillType(const vector<TrigConfAlg> &algs)
{
  //
  // Fill web page for algorithm instance
  //

  string alg_type;

  for(unsigned i = 0; i < algs.size(); ++i) {
    const TrigConfAlg &alg = algs.at(i);
    if(alg_type.empty()) {
      alg_type = alg.getType();
    }
    else if(alg_type != alg.getType()) {
      cerr << "HtmlAlg::FillType - alg type mismatch: " << alg_type << "!=" << alg.getType() << endl;
      return;
    }
  }

  Handle<HtmlNode> node = FindAlgType(alg_type);
  if(!node.valid()) {
    cerr << "HtmlAlg::FillType - error! Missing alg node: " << alg_type << endl;
    return;
  }

  stringstream &os = node->GetCont();   
  os << "<pre>" << endl;

  //
  // Create new node to list all algorithm instances
  //
  Handle<HtmlNode> nodeI(new HtmlNode(node->GetBase()+"_instances.html", HtmlNode::kHTML));
  fNodeRoot->AddChild(nodeI);
  nodeI->GetCont() << "<pre>" << endl;

  stringstream linkI;
  linkI << algs.size() << " algorithm instances(s)";

  os << "<pre>" << endl
     << "Algorithm name: " << nodeI->GetLink(linkI.str()) << endl
     << "<b>C++ class name: " << alg_type << "</b>" << endl
     << endl;
  
  for(unsigned i = 0; i < algs.size(); ++i) {
    Handle<HtmlNode> nodeA = FindAlgName(algs.at(i).getName());
    if(nodeA.valid()) {
      nodeI->GetCont() << nodeA->GetLink(algs.at(i).getName()) << endl;
    }
    else {
      nodeI->GetCont() << algs.at(i).getName() << endl;
    }
  }
  
  nodeI->GetCont() << "</pre>" << endl;

  //
  // Add links to chains and sequences which use this algorithm type
  //
  std::vector<string> seq_vec, chn_vec;
  
  for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = fConfig.at<TrigConfSeq>(i);
    
    for(unsigned j = 0; j < seq.getNAlg(); ++j) {
      if(alg_type == seq.getAlg(j).getType()) { 
	seq_vec.push_back(seq.getName());
      }
    }
  }

  for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
    
    bool matched = false;
    const vector<TrigConfSeq> chain_seq = Trig::GetSeq(chain, fConfig);

    for(unsigned j = 0; j < seq_vec.size(); ++j) {
      if(std::find(chain_seq.begin(), chain_seq.end(), seq_vec.at(j)) != chain_seq.end()) {
	matched = true;
	break;
      }
    }

    if(matched) chn_vec.push_back(chain.getName());
  }
  
  //
  // Make links to two html pages to reduce cluter:
  //  - one for parent sequence links
  //  - one for parent chain links
  //
  Handle<HtmlNode> nodeChn(new HtmlNode(node->GetBase()+"_exec_chn.html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeSeq(new HtmlNode(node->GetBase()+"_exec_seq.html", HtmlNode::kHTML));

  fNodeRoot->AddChild(nodeChn);
  fNodeRoot->AddChild(nodeSeq);

  stringstream strSeq, strChn;
  
  strChn << chn_vec.size() << " chain(s) execute this algorithm class";
  strSeq << seq_vec.size() << " sequence(s) execute this algorithm class";
  
  os << nodeChn->GetLink(strChn.str()) << endl;
  os << nodeSeq->GetLink(strSeq.str()) << endl << endl;

  nodeChn->GetCont() << "<pre>" << endl 
		     << chn_vec.size() << " chain(s) execute " << alg_type << ":"
		     << endl << endl;

  nodeSeq->GetCont() << "<pre>" << endl 
		     << seq_vec.size() << " sequence(s) execute " << alg_type << ":"
		     << endl << endl;
  
  Trig::PrintColumns(chn_vec, nodeChn->GetCont(), 1, fNodeRoot);
  Trig::PrintColumns(seq_vec, nodeSeq->GetCont(), 1, fNodeRoot->FindChild("SEQ_ROOT"));

  nodeChn -> GetCont() << "</pre>" << endl;
  nodeSeq -> GetCont() << "</pre>" << endl;

  // HACK - do not add closing </pre> - this will be inserted by HtmlNode.cxx
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindAlgName(const string &key) const
{
  //
  // Find html node for alg by name
  //  
  if(!fNodeRoot.valid()) return Handle<HtmlNode>(0);
  
  // Find parent node
  Handle<HtmlNode> nodeP = fNodeRoot -> FindChild("ALG_NAME");
  if(!nodeP.valid()) Handle<HtmlNode>(0);
  
  return nodeP -> FindChild(key);
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindAlgType(const string &key) const
{
  //
  // Find html node for alg by name
  //  
  if(!fNodeRoot.valid()) return Handle<HtmlNode>(0);
  
  // Find parent node
  Handle<HtmlNode> nodeP = fNodeRoot -> FindChild("ALG_TYPE");
  if(!nodeP.valid()) Handle<HtmlNode>(0);
  
  return nodeP -> FindChild(key);
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindChnNode(const string &key) const
{
  //
  // Find html node for chn by name
  //  
  if(!fNodeRoot.valid()) return Handle<HtmlNode>(0);
  
  return fNodeRoot -> FindChild(key);
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindChnNode(unsigned int id) const
{
  //
  // Find html node by id
  //
  stringstream fileS;
  fileS << "CHN_" << setfill('0') << setw(10) << id << ".html";

  return FindChnNode(fileS.str());
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindSeqNode(const string &key) const
{
  //
  // Find html node by name
  //  
  if(!fNodeRoot.valid()) return Handle<HtmlNode>(0);
  
  // Find parent node
  Handle<HtmlNode> nodeP = fNodeRoot -> FindChild("SEQ_ROOT");
  if(!nodeP.valid()) Handle<HtmlNode>(0);
  
  return nodeP -> FindChild(key);
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlAlg::FindSeqNode(unsigned int id) const
{
  //
  // Find html node by id
  //
  stringstream fileS;
  fileS << "SEQ_" << setfill('0') << setw(10) << id << ".html";

  return FindSeqNode(fileS.str());
}


//-----------------------------------------------------------------------------
std::string Anp::HtmlAlg::GetChainLink(const std::string &chain) const
{
  //
  // Return link to chain node if it exists
  //
  Handle<HtmlNode> nodeChain = FindChnNode(chain);
  if(nodeChain.valid()) {
    return nodeChain->GetLink(chain);
  }
  
  return chain;
}
