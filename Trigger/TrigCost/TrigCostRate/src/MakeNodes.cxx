/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <limits>

// Local
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilHack.h"
#include "TrigCostRate/MakeNodes.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::MakeNodes::NodeData::~NodeData()
{
  if(nodeL1)    delete nodeL1;
  if(nodePS)    delete nodePS;
  if(nodePT)    delete nodePT;
  if(nodeDecis) delete nodeDecis;
  if(nodeCPS)   delete nodeCPS;
}

//-----------------------------------------------------------------------------
Anp::MakeNodes::MakeNodes()
  :fDebug(false),
   fDoExpress(false),
   fDoCPS(false),
   fDoPTOnly(false),
   fDecisArray(0)
{
}

//-----------------------------------------------------------------------------
Anp::MakeNodes::~MakeNodes()
{
  //
  // Delete owned pointers
  //
  if(fDecisArray) {
    delete [] fDecisArray;
  }

  for(unsigned i = 0; i < fChains.size(); ++i) {
    delete fChains.at(i).first;
  }
  for(unsigned i = 0; i < fData.size(); ++i) {
    delete fData.at(i);
  }
  for(unsigned i = 0; i < fRoot.size(); ++i) {
    delete fRoot.at(i);
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::ConfigReg(const Registry &reg)
{
  //
  // Read parent's registry
  //
  reg.Get("MakeRates", "DoCPS", fDoCPS);
  reg.Get("MakeRates", "Debug", fDebug);

  cout << "MakeNodes::ConfigReg" << endl
       << "   Use coherent PS=" << fDoCPS << endl;
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::ConfigTrigger(const std::vector<TrigConfChain> &config)
{
  //
  // Read trigger configuration
  //
  if(!fChains.empty() || fDecisArray) {
    log() << "ConfigTrigger - can not be configured twice" << endl;
    return;
  }
  
  fDecisArray = new unsigned[config.size()];

  for(unsigned i = 0; i < config.size(); ++i) {
    const TrigConfChain &chain = config.at(i);    
    fDecisArray[i] = 0;
    unsigned *decision = &(fDecisArray[i]);
    
    TrigConfChain *test = GetChain(chain.getName());
    if(test) {     
      log() << "ConfigTrigger - error! Duplicate chain: " << chain.getName() << endl;
      continue;
    }

    fChains.push_back(pair<TrigConfChain *, unsigned *>(new TrigConfChain(chain), decision));
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::UpdateTrigger(const std::vector<TrigConfChain> &)
{
  //
  // Read trigger configuration
  //
  log() << "UpdateTrigger - not implemented" << endl;
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::SetTrigger(const RateBits &bits)
{
  //
  // Set trigger decisions for new event
  //
  for(TrigVec::iterator it = fChains.begin(); it != fChains.end(); ++it) {
    const TrigConfChain &chain = *(it->first);
    
    //
    // Set default decision to 0
    //
    unsigned &decision = *(it->second);
    decision = bits.TestTriggerBit(chain.getLevelId(), chain.getCounter());
  }
}

//-----------------------------------------------------------------------------
Anp::RateNode* Anp::MakeNodes::CreateNode(const std::vector<TrigConfChain> &chains, 
					  const Registry &reg)
{
  //
  // Extract parameters from registry
  //
  fDoExpress = false;
  fDoPTOnly  = false;
  string name;

  reg.Get("MakeRates", "DoExpress",  fDoExpress);
  reg.Get("MakeRates", "DoPTOnly",   fDoPTOnly);
  reg.Get("MakeRates", "ElemName",   name);

  //
  // Check input chains for consistent level
  //
  if(chains.empty()) {
    if(fDebug) {
      log() << "CreateElem - no chains for: " << name << endl;
    }
    return 0;
  }

  unsigned level = 0;

  for(unsigned i = 0; i < chains.size(); ++i) {
    const TrigConfChain &chain = chains.at(i);
    
    //
    // Check level consistency
    //
    if(level == 0) {
      level = chain.getLevelId();
    }
    else if(level != chain.getLevelId()) {
      log() << "CreateElem - level mismatch for: " << chain.getName() << endl;
      return 0;
    }
  }

  //
  // Failed to determine level - empty/bad input list
  //
  if(level == 0) {
    log() << "CreateElem - failed to determine level: " << name << ", #chains=" << chains.size() << endl;
    for(unsigned i = 0; i < chains.size(); ++i) {
      cout << "   " << chains.at(i).getName() << endl;
    }
    return 0;
  }

  //
  // Convert input chains to local pointers
  //
  const vector<TrigConfChain *> inputs = ConvertInput(chains);
 
  //
  // Find chains at all three levels - L2 and/or EF chains can be missing
  //
  vector<TrigConfChain *> chainsL1, chainsL2, chainsEF;

  if(level == 1) {
    chainsL1 = inputs;
  }
  else if(level == 2) {
    chainsL2 = inputs;
    chainsL1 = GetInputChains(chainsL2);
  }
  else if(level == 3) {
    chainsEF = inputs;
    chainsL2 = GetInputChains(chainsEF);
    chainsL1 = GetInputChains(chainsL2);
  }

  //
  // Create root node for full trigger decision tree
  //
  RateNode *rootNode = new RateNode();
  fRoot.push_back(rootNode);
  
  //
  // Map for all nodes created in this function
  //
  NodeMap nodes;

  //
  // Make L1 nodes
  //
  vector<NodeData *> dataL1;

  for(unsigned i = 0; i < chainsL1.size(); ++i) {
    TrigConfChain *chain = chainsL1.at(i);
    
    //
    // Check for existing chains
    //
    if(nodes.find(chain) != nodes.end()) {
      log() << "CreatElem - duplicate L1 item: " << chain->getName() << endl;
      continue;
    }

    //
    // Create and save L1 nodes
    //
    NodeData *data = MakeData(nodes, chain);
    dataL1.push_back(data);
    
    rootNode->AddChild(data->nodeL1);
  }

  //
  // Filter out coherent PS chains
  //
  const map<std::string, std::vector<TrigConfChain *> > cps_map = GetCPS(chainsL2);
  
  //
  // Create L2 nodes 
  // 
  FillTreeHLT(nodes, chainsL2);
  
  //
  // Process coherent PS 
  //
  for(map<string, vector<TrigConfChain *> >::const_iterator it = cps_map.begin(); it != cps_map.end(); ++it) {
    FillTreeCPS(nodes, it->second);
  }
  
  //
  // Create EF nodes 
  // 
  FillTreeHLT(nodes, chainsEF);

  //
  // Print debug info
  //
  if(fDebug && name.find("CPS") != string::npos) {
    cout << "--------------------------------------------" << endl;
    cout << name << endl;
    Print(dataL1);
    cout << "********************************************" << endl;
    rootNode->Print(0, true);
    cout << "--------------------------------------------" << endl;
  }

  if(fDebug) {
    cout << name << ": # of nodes: " << rootNode->CountNodes() << endl;
  }

  return rootNode;
}
//-----------------------------------------------------------------------------
Anp::RateNode* Anp::MakeNodes::CreateMultiSeedNode(const TrigConfChain &chain, const Registry &reg)
{
  //
  // Extract parameters from registry
  //
  fDoExpress = false;
  fDoPTOnly  = false;
  string name;

  reg.Get("Name", name);

  //
  // Failed to determine level - empty/bad input list
  //
  if(chain.getLevelId() != 2 && chain.getLevelId() != 3) {
    log() << "CreateMultiSeedElem - wrong level for chain: " << chain.getName() << endl;
    return 0;
  }

  //
  // Convert input chains to local pointers
  //
  TrigConfChain *input = GetChain(chain.getName());
  if(!input) {
    log() << "CreateMultiSeedElem - unknown chain: " << chain.getName() << endl;
    return 0;   
  }

  //
  // Create root node for HLT trigger decision tree
  //
  RateNode *rootNode = new RateNode();
  fRoot.push_back(rootNode);

  NodeMap nodes;

  if(chain.getLevelId() == 2) {
    //
    // Case for unseeded L2 chain:
    //   - connect HLT directly to RootNode
    //
    NodeData *dataL2 = MakeData(nodes, input);    
    rootNode->AddChild(dataL2->nodePS);
  }
  else if(chain.getLevelId() == 3) {
    const vector<TrigConfChain *> chainsL2 = GetInputChains(*input);
    if(chainsL2.size() == 1) {
      //
      // Case for unseeded L2 chain: input is EF chain seeded by this L2 chain
      //
     NodeData *dataEF = MakeData(nodes, input);
     NodeData *dataL2 = MakeData(nodes, chainsL2.front());
     
     rootNode->AddChild(dataL2->nodePS);
     dataL2->nodeDecis->AddChild(dataEF->nodePS);
    }
    else {
      //
      // Case for unseeded/multi-seeded EF chain - same as unseeded L2 chain  
      //
      NodeData *dataEF = MakeData(nodes, input);    
      rootNode->AddChild(dataEF->nodePS);         
    }
  }

  if(fDebug) {
    cout << name << ": # of nodes: " << rootNode->CountNodes() << endl;
  }

  return rootNode;
}

//-----------------------------------------------------------------------------
std::vector<TrigConfChain *> Anp::MakeNodes::ConvertInput(const std::vector<TrigConfChain> &chains)
{
  //
  // Convert input chains to local on heap objects
  //
  std::vector<TrigConfChain *> inputs; 

  for(unsigned i = 0; i < chains.size(); ++i) {
    const string &name = chains.at(i).getName();
    TrigConfChain *chain = GetChain(name);

    if(chain) {
      inputs.push_back(chain);
    }
    else {
      if(fDebug) {
	log() << "ConvertInput - missing input for: " << name << endl;
      }
      continue;
    }
  }

  //
  // Sort pointers
  //
  std::sort(inputs.begin(), inputs.end());

  //
  // Remove duplicates
  //
  const unsigned isize = inputs.size();
  
  std::vector<TrigConfChain *>::iterator it = std::unique(inputs.begin(), inputs.end());
  inputs.erase(it, inputs.end());
  
  if(isize != inputs.size()) {
    log() << "ConvertInputs - removed " << isize - inputs.size() << " duplicate trigger(s)" << endl;
  }

  return inputs;
}

//-----------------------------------------------------------------------------
std::vector<TrigConfChain *> Anp::MakeNodes::GetInputChains(const TrigConfChain &chain)
{
  //
  // Find input chains 
  //
  std::vector<TrigConfChain *> inputs;

  if(chain.getLowerName().empty()) {
    //
    // Add all previous level chains for unseeded chain
    //
    for(unsigned i = 0; i < fChains.size(); ++i) {
      TrigConfChain *input = fChains.at(i).first;

      if(input -> getLevelId() == chain.getLevelId() - 1) {
	inputs.push_back(input);
      }
    }
  }
  else {
    //
    // Collect input chains for single or multi-seeded case
    //
    std::vector<std::string> inames;
    Anp::StringTok(inames, chain.getLowerName(), ", ");
    
    for(unsigned j = 0; j < inames.size(); ++j) {
      TrigConfChain *input = GetChain(inames.at(j));

      if(!input) {
	if(fDebug) {
	  log() << "GetInputChains - missing input for: " << chain.getName() << endl;
	}
	continue;
      }

      inputs.push_back(input);
    }
  }
 
  return inputs;
}

//-----------------------------------------------------------------------------
std::vector<TrigConfChain *> Anp::MakeNodes::GetInputChains(const std::vector<TrigConfChain *> &chains)
{
  //
  // Find input chains
  //
  std::vector<TrigConfChain *> inputs;

  for(unsigned i = 0; i < chains.size(); ++i) {
    const TrigConfChain *chain = chains.at(i);
    if(chain) {
      const std::vector<TrigConfChain *> ivec = GetInputChains(*chain);
      inputs.insert(inputs.end(), ivec.begin(), ivec.end());
    }
  }
  
  //
  // Sort pointers
  //
  std::sort(inputs.begin(), inputs.end());

  //
  // Remove duplicates
  //
  std::vector<TrigConfChain *>::iterator it = std::unique(inputs.begin(), inputs.end());
  inputs.erase(it, inputs.end());

  return inputs;
}

//-----------------------------------------------------------------------------
TrigConfChain* Anp::MakeNodes::GetChain(const string &chain)
{
  //
  // Return pointer to matching chain by name
  //
  for(unsigned i = 0; i < fChains.size(); ++i) {
    TrigConfChain *conf = fChains.at(i).first;
    if(conf -> getName() == chain) {
      return conf;
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
map<string, vector<TrigConfChain *> > Anp::MakeNodes::GetCPS(vector<TrigConfChain *> &chains) const
{
  //
  // Make vector for each group of coherent prescales
  //
  map<string, vector<TrigConfChain *> > cmap;
  
  if(!fDoCPS) return cmap;

  vector<TrigConfChain *>::iterator it = chains.begin();
  while(it != chains.end()) {
    TrigConfChain *chain = *it;
    
    //
    // No CPS in EF
    //
    if(chain->getLevelId() != 2) {
      continue;
    }

    //
    // Check CPS groups
    // 
    std::string group_cps;
    unsigned count_cps = 0;

    for(unsigned int j = 0; j < chain->getGroup().size(); ++j) {
      const string &group = chain->getGroup().at(j);
      if(group.find("CPS") != string::npos) {
	count_cps++;
	group_cps = group;
      }
    }
    
    if(count_cps > 1) {
      log() << "GetCPS - error: " << chain->getName() << " has " << count_cps << " CPS groups" << endl;
    }
    
    if(group_cps.empty()) {
      it++;
    }
    else {
      cmap[group_cps].push_back(chain);
      it = chains.erase(it);
    }
  }

  //
  // Consistency check for CPS group
  //
  for(map<string, vector<TrigConfChain *> >::const_iterator it = cmap.begin(); it != cmap.end(); ++it) {
    const vector<TrigConfChain *> &cvec = it->second;
    string lower_name;

    for(unsigned i = 0; i < cvec.size(); ++i) {
      const TrigConfChain *chain = cvec.at(i);
      
      if(chain->getLevelId() != 2) {
	log() << "GetCPS - error! CPS chain is not L2: " << chain->getName() << endl;
	continue;
      }

      if(i == 0) {
	lower_name = chain->getLowerName();
      }
      else if(lower_name != chain->getLowerName()) {
	log() << "GetCPS - error! Mismatched input for CPS chain " << chain->getName() 
	      << ": \"" << lower_name << "\"!=\"" << chain->getLowerName() << "\"" << endl;
	continue;
      }
    }

    if(fDebug) {
      log() << "GetCPS - for coherent group " << it->first << " input=\"" << lower_name << "\"" << endl;
    }
  }

  return cmap;
}

//-----------------------------------------------------------------------------
Anp::RateNode* Anp::MakeNodes::MakeNode(RateNode *parent, 
					TrigConfChain *chain, 
					RateNode::NodeType type)
{
  //
  // Create and save RateNode: 
  //  - use single function to create nodes in case we want to add pool factory
  //
  unsigned *decision = 0;
  
  //
  // Save decision RateNode
  //
  if(type == RateNode::kNodeDecis || type == RateNode::kNodeL1) {
    //
    // Find decision pointer for nodes which check decision bit
    //
    for(TrigVec::iterator it = fChains.begin(); it != fChains.end(); ++it) {
      TrigConfChain *conf_chain = it->first;
      if(conf_chain == chain) {
	decision = it->second;
	break;
      }
    }
    
    if(!decision) {
      log() << "MakeNode - failed to find chain: " << chain->getName() << endl;
    }
  }

  RateNode *node = new RateNode(type, parent, decision);

  return node;
}

//-----------------------------------------------------------------------------
Anp::MakeNodes::NodeData* Anp::MakeNodes::MakeData(NodeMap &nodes, TrigConfChain *chain)
{
  //
  // Find matching node data - create new one if necessary
  //
  if(!chain) {
    log() << "MakeData - null TrigConfChain pointer" << endl;
    return 0;    
  }

  NodeMap::iterator it = nodes.find(chain);
  if(it != nodes.end()) {
    log() << "MakeData - duplicate trigger: " << chain->getName() << endl;
    return it->second;
  }
  
  NodeData *data = new NodeData();
  fData.push_back(data);
  nodes[chain] = data;
  
  if(chain->getLevelId() == 1) {
    //
    // Create and set weight for L1 node
    //
    data->chain  = chain;
    data->nodeL1 = MakeNode(0, chain, RateNode::kNodeL1);

    const float ps = data->chain->getPrescale();    
    if(data->nodeL1 && !(ps < 1.0)) { 
      data->nodeL1->SetWeight(1.0/ps);
    }
  }
  else {
    data->chain     = chain;
    data->nodePS    = MakeNode(0,            chain, RateNode::kNodePS);
    data->nodeDecis = MakeNode(data->nodePS, chain, RateNode::kNodeDecis);
  
    //
    // Weight for HLT PS node
    //
    const float ps = data->chain->getPrescale();    
    if(!(ps < 1.0)) { 
      data->nodePS->SetWeight(1.0/ps);
    }
    else {
      data->nodePS->SetWeight(0.0);
    }
    
    //
    // Weight for PT: create only if PT >= 1.0
    //    
    const float pt = data->chain->getPassThrough();
    if(!(pt < 1.0)) { 
      data->nodePT = MakeNode(data->nodePS, chain, RateNode::kNodePT);
      data->nodePT->SetWeight(1.0/pt);
    }
    
    //
    // Weight for trigger decision node
    //
    if(fDoPTOnly) {
      data->nodeDecis->SetWeight(0.0);
    }
    else {
      data->nodeDecis->SetWeight(1.0);      
    }
  }

  return data;
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::FillTreeHLT(NodeMap &nodes, const std::vector<TrigConfChain *> &chains)
{
  //
  // Create and connect HLT nodes
  //
  map<NodeData *, vector<NodeData *> > mapL2toEF;

  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain *chain = chains.at(i);
    
    //
    // Create and save HLT nodes
    //
    NodeData *data = MakeData(nodes, chain);

    //
    // Find input chains:
    //
    std::vector<TrigConfChain *> ivec = GetInputChains(*chain);
    
    for(unsigned j = 0; j < ivec.size(); ++j) {
      TrigConfChain *input = ivec.at(j);
      
      NodeMap::const_iterator inode = nodes.find(input);
      if(inode == nodes.end()) {
	log() << "FillTreeHLT - " << chain->getName() << " missing input: " << input->getName() << endl;
	continue;
      }
      
      NodeData *idata = inode->second;
      
      if(idata->nodeL1) {
	//
	// Previous level is L1: connect to L1 node
	// 
	idata->nodeL1 -> AddChild(data->nodePS);
      }
      else if(idata->nodeDecis) {
	//
	// Previous level is L2: connect to decision node
	// 
	idata->nodeDecis -> AddChild(data->nodePS);
      }

      idata->children.push_back(data);
      
      if(fDoExpress && HasExpressStream(*(data->chain)) && data->chain->getLevelId() == 3) {
	mapL2toEF[idata].push_back(data);
      }
    }
  }

  //
  // Set EF express stream PS 
  //
  for(map<NodeData *, vector<NodeData *> >::iterator it = mapL2toEF.begin(); it != mapL2toEF.end(); ++it) {
    NodeData *dataL2 = it->first;
    vector<NodeData *> &vecEF = it->second;
    

    if(dataL2->nodePT) {
      //
      // PT at L2 case: apply ES weight at L2 PS nodes
      //      
      if(vecEF.size() > 1) {
	log() << "FillTreeHLT - error! Multiple EF express chains seeded by single PT L2 chain: "
	      << dataL2->chain->getName() << endl;
      }

      double weightES = 0.0;
      
      for(unsigned i = 0; i < vecEF.size(); ++i) {
	NodeData *dataEF = vecEF.at(i);
	const double es = GetExpressPS(*(dataEF->chain));
	
	if(!(es < 1.0)) {
	  weightES = weightES + (1.0-weightES)/es;
	}
      }
     
      //
      // Set express stream weight for L2 PT node
      //
      dataL2->nodePS->SetWeight(dataL2->nodePS->GetNodeWeight()*weightES);
    }
    else {
      //
      // No PT at L2 case: apply ES weight at EF PS nodes
      //      
      for(unsigned i = 0; i < vecEF.size(); ++i) {
	NodeData *dataEF = vecEF.at(i);
	const double es = GetExpressPS(*(dataEF->chain));       
	
	//
	// Scale PS node weight by express stream PS if available
	//
	if(!(es < 1.0)) {
	  dataEF->nodePS->SetWeight(dataEF->nodePS->GetNodeWeight()/es);
	}
	else {
	  dataEF->nodePS->SetWeight(0.0);
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
namespace Anp
{
  struct ChainSortCPS {

    bool operator()(const TrigConfChain *lhs, const TrigConfChain *rhs) 
    {
      //
      // all what is below 1 means infinity (n omater if it is 0 or -1
      //
      const float ps1 = lhs->getPrescale() < 1. ? std::numeric_limits<float>::infinity() : lhs->getPrescale();
      const float ps2 = rhs->getPrescale() < 1. ? std::numeric_limits<float>::infinity() : rhs->getPrescale();
    
      //
      // if PS factors are equal the order does not matter in fact ... but we need to stably 
      // order them (otherwise sorting will fail)
      //
      if ( ps1 == ps2 ) {
	return lhs->getCounter() < rhs->getCounter();
      }
      
      return ps1 < ps2;
    }
  };
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::FillTreeCPS(NodeMap &nodes, std::vector<TrigConfChain *> chains)
{
  //
  // Create and connect HLT nodes
  //
  if(chains.empty()) {
    return;
  }

  //
  // Sort by PS
  //
  std::sort(chains.begin(), chains.end(), ChainSortCPS());
  
  //
  // Find input chains:
  //
  std::vector<TrigConfChain *> inputs;

  if(fDebug) {
    log() << "FillTreeCPS - number of coherent chains: " << chains.size() << endl;
  }

  for(unsigned i = 0; i < chains.size(); ++i) {
    const TrigConfChain *chain = chains.at(i);
    
    std::vector<TrigConfChain *> ivec = GetInputChains(*chain);
    std::sort(ivec.begin(), ivec.end());

    if(i == 0) {
      inputs = ivec;
    }
    else if(inputs != ivec) {
      log() << "FillTreeCPS - input error!" << endl;
      return;
    }
    
    if(fDebug) {
      cout << "   " << chain->getName() << " PS=" << chain->getPrescale() << endl;
    }
  }

  //
  // Make tree of coherent PS triggers
  //
  double prev_ps = 1.0;
  
  NodeData *initData = 0;
  NodeData *prevData = 0;

  for(unsigned i = 0; i < chains.size(); ++i) {
    TrigConfChain *chain = chains.at(i);
    
    //
    // Create nodes for single chain
    //
    NodeData *data = MakeData(nodes, chain);
    if(!initData) initData = data;
    
    // 
    // Make CPS node
    //
    data->nodeCPS = MakeNode(0, chain, RateNode::kNodeCPS);
    data->nodeCPS->AddChild(data->nodePS);
    
    if(prevData) {
      prevData->nodeCPS->AddChild(data->nodeCPS);
      prevData->children.push_back(data);
    }
    
    prevData = data;
    
    if(!(chain->getPrescale() < 1.0)) {
      data->nodeCPS->SetWeight(prev_ps/chain->getPrescale());
      prev_ps = chain->getPrescale();
    }
    else {
      data->nodeCPS->SetWeight(0.0);
      prev_ps = 0.0;
    }

    data->nodePS->SetWeight(1.0);
  }

  for(unsigned j = 0; j < inputs.size(); ++j) {
    TrigConfChain *input = inputs.at(j);
    
    NodeMap::iterator inode = nodes.find(input);
    if(inode == nodes.end()) {
      log() << "FillTreeCPS - missing input: " << input->getName() << endl;
      continue;
    }
    
    NodeData *idata = inode->second;
    if(idata->nodeL1) {
      //
      // Previous level is L1: connect to L1 node
      // 
      idata->nodeL1->AddChild(initData->nodeCPS);
    }
    else {
      //
      // Previous level is L2: connect to decision node
      // 
      idata->nodeDecis->AddChild(initData->nodeCPS);
    }
    
    idata->children.push_back(initData);
  }    
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::Print(const std::vector<NodeData *> &vec, unsigned margin)
{
  //
  // Print NodeData tree
  //
  const string pad(margin, ' ');

  for(unsigned i = 0; i < vec.size(); ++i) {
    const NodeData *data = vec.at(i);
    
    cout << pad << "NodeData: " << data->chain->getName() << ": " << data << endl;

    if(data->nodeL1)    data->nodeL1   ->Print(margin+2);
    if(data->nodeCPS)   data->nodeCPS  ->Print(margin+2);
    if(data->nodePS)    data->nodePS   ->Print(margin+2);
    if(data->nodeDecis) data->nodeDecis->Print(margin+2);
    if(data->nodePT)    data->nodePT   ->Print(margin+2);
    
    Print(data->children, margin+4);
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeNodes::Print(const RateNode *node, unsigned margin)
{
  //
  // Print RateNode tree
  //
  const string pad(margin, ' ');  
  cout << pad << node->GetTypeAsString() << " weight=" << node->ComputeWeight() << endl;
  
  const std::vector<RateNode *> &cvec = node->GetChildren();
  
  for(unsigned i = 0; i < cvec.size(); ++i) {
    Print(cvec.at(i), margin+2);
  }
}

//-----------------------------------------------------------------------------
std::ostream& Anp::MakeNodes::log() const
{
  std::cout << "MakeNodes::";
  return cout;
}
