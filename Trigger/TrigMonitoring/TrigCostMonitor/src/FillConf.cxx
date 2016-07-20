/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C+
#include <cmath>
#include <sys/time.h>
#include <cstdlib>

// HLT trigger
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTUtils.h"

// LV1 trigger
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/Muctpi.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigCostMonitor/FillConf.h"

#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH

using namespace std;

//---------------------------------------------------------------------------------------
namespace Trig
{
   void AddOneVar(TrigMonConfig &confg, const std::string &key, const std::string &var)
   {
      if(!confg.keyExists(key)) {
         confg.addValue(key, var);
      }
   }

   std::string ReadVar(const std::string &name)
   {    
      const char *value = std::getenv(name.c_str());
      if(!value) return "";
    
      return std::string(value);
   }

   bool ParseAlg(const std::string &key,
                 std::string &alg_type,
                 std::string &alg_name)
   {
      if(std::count(key.begin(), key.end(), '/') != 1) {
         return false;
      }
    
      const std::string::size_type ipos = key.find("/");
      if(ipos == std::string::npos || ipos >= key.size()) {
         return false;
      }
    
      alg_type = key.substr(0, ipos);
      alg_name = key.substr(ipos+1, key.size()-ipos-1);
    
      return true;
   }

   void addAllThresholds(const std::string &item_name,
                         TrigConf::TriggerItemNode *node,
                         std::vector<TrigConfSeq> &svec, 
                         std::ostream &log) 
   {
      if(!node) return;       
    
      std::string node_type;
      if     (node->type() == TrigConf::TriggerItemNode::OBJ) node_type =  "OBJ";
      else if(node->type() == TrigConf::TriggerItemNode::NOT) node_type =  "NOT";
      else if(node->type() == TrigConf::TriggerItemNode::AND) node_type =  "AND";
      else if(node->type() == TrigConf::TriggerItemNode::OR)  node_type =  "OR ";
   
      log << item_name << ": " << node_type << endl;

      if(node->type() == TrigConf::TriggerItemNode::OBJ) {      
         std::string th_name;

         if(node->isInternalTrigger()) {
            // th_name = TrigConf::L1DataDef::internalTriggerName(node->internalTrigger()); 
            th_name = node->thresholdName(); // TODO: check that this is doing the right thing for internal triggers - Joerg
            TrigConfSeq seq(TrigConf::HLTUtils::string2hash(th_name, "L1_THR"), 0, th_name);
            svec.push_back(seq);
         }
         else {
            TrigConf::TriggerThreshold *th = node->triggerThreshold();
            if(th) {
               th_name = th->name();
               TrigConfSeq seq(TrigConf::HLTUtils::string2hash(th_name, "L1_THR"), 0, th_name);
               for(int i = 0; i < node->multiplicity(); ++i) {
                  svec.push_back(seq);
               }
            }
         }

         log << "  " << item_name << ": " << th_name << endl;
      }
      else {
         BOOST_FOREACH(TrigConf::TriggerItemNode* child, node->children())
            addAllThresholds(item_name, child, svec, log);
      }

      return;
   }
}

//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillLV1(TrigMonConfig &confg,
			    const TrigConf::CTPConfig &ctp_config)
{
   // 
   // Collect L1 config
   //

   const std::vector<float> prescalesL1 = ctp_config.prescaleSet().prescales_float();

   std::set<TrigConfSeq> myth_set;

   BOOST_FOREACH(TrigConf::TriggerItem *item, ctp_config.menu().items()) {
      if(!item) {
         m_error << "Null TriggerItem pointer" << endl;
         continue;
      }
    
      float prescale = 0;
      if(int(prescalesL1.size()) > item->ctpId()) {
         prescale = prescalesL1[item->ctpId()];
      } else {
         m_error << "Bad ctpId & prescale: " << item->ctpId() << ">" << prescalesL1.size() << endl;
      }

      //
      // Copy chain configuration
      //
      TrigConfChain mychn(item->name(),
                          item->ctpId(),
                          TrigConf::HLTUtils::string2hash(item->name(), "L1_ITEM"),
                          prescale);


      std::vector<TrigConfSeq> svec;
      Trig::addAllThresholds(item->name(), item->topNode(), svec, m_debug);
        
      //
      // Add thresholds to L1 item
      //
      TrigConfSig mysig(0, 0, "TriggerThresholds");

      m_debug << "Adding " << svec.size() << " thresholds for L1 item: " << item->name() << endl;

      for(unsigned t = 0; t < svec.size(); ++t) {
         const TrigConfSeq &seq = svec.at(t);
         mysig.addOutputTE(seq.getId());
         myth_set.insert(seq);
      
         m_debug << "  " << seq.getName() << endl;
      }

      //
      // Add L1 item to list of all configured triggers
      //
      mychn.addSignature(mysig);
      confg.add<TrigConfChain>(mychn);
   }

   //
   // Check for L1 thresholds which are not used by any L1 items
   //
   const std::vector<TrigConf::TriggerThreshold *> &thd_vec = ctp_config.menu().thresholdVector();
   std::map<std::string, TrigConf::TriggerThreshold *> th_map;

   for(unsigned i = 0; i < thd_vec.size(); ++i) {
      TrigConf::TriggerThreshold *th = thd_vec.at(i);
      if(!th) {
         m_error << "Null TriggerThreshold pointer" << endl;
         continue;
      }

      TrigConfSeq seq(TrigConf::HLTUtils::string2hash(th->name(), "L1_THR"), 0, th->name());
      myth_set.insert(seq);
   }
    
   unsigned icount_th = 0;

   for(std::set<TrigConfSeq>::const_iterator it = myth_set.begin(); it != myth_set.end(); ++it) {
      //
      // Make threshold copy and set unique index
      //
      TrigConfSeq seq(it->getId(), icount_th++, it->getName());
    
    
      if(!confg.add<TrigConfSeq>(seq)) {
         m_error << "Failed to add TriggerThreshold: " << seq.getName() << endl;
      }
      else {
         m_debug << "L1 threshold " 
                 << seq.getIndex() << ": " << seq.getName() << ", " << seq.getId() << endl;
      }
   }

   return true;
}

//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillSeq(TrigMonConfig &confg,
			     const TrigConf::HLTSequenceList &seq_list)
{
  //
  // Fill HLT sequences
  //

  std::map<std::string, TrigConf::HLTSequence *> seqmap;
  BOOST_FOREACH(TrigConf::HLTSequence *hseq, seq_list) {
    if(!hseq) {
      m_error << "Null HLTSequence pointer" << endl;
      continue;
    }
    
    const TrigConf::HLTTriggerElement *out_te = hseq -> outputTE();
    if(!out_te) {
      m_error << "Null ouput HLTTriggerElement pointer" << endl;
      continue;
    }

    const std::pair<std::string, TrigConf::HLTSequence *> pairTE(out_te->name(), hseq);
    
    if(!seqmap.insert(pairTE).second) {
      m_error << "Failed to insert sequence: " << out_te->name() << endl;
    }
  }

  //
  // Insert sequence sorted by name, first 512 index values are used by L1
  //
  unsigned icountSeq = 512;
  
  for(std::map<std::string, TrigConf::HLTSequence *>::const_iterator sit = seqmap.begin();
      sit != seqmap.end(); ++sit) {
    TrigConf::HLTSequence *hseq = sit->second;
    if(!hseq) {
      m_error << "Null HLTSequence pointer" << endl;
      continue;
    }
    
    const TrigConf::HLTTriggerElement *out_te = hseq -> outputTE();
    if(!out_te) {
      m_error << "Null ouput HLTTriggerElement pointer" << endl;
      continue;
    }

    //
    // New sequence configuration
    //
    TrigConfSeq myseq(out_te->id(), icountSeq++, out_te->name());
  
    //
    // Save name of topoStartTE
    //
    if(hseq->topoStartTE()) {
      myseq.setTopoTE(hseq->topoStartTE()->id());
    }

    //
    // Iterate over algorithms
    //
    const std::vector<std::string> &algv = hseq -> algorithms();
    for(unsigned int ialg = 0; ialg < algv.size(); ++ialg) {
      //
      // Extract algorithm name (instance name), type (class name) and base class name
      //
      std::string alg_type, alg_name;
      
      if(!ParseAlg(algv[ialg], alg_type, alg_name)) {
	m_error << "Failed to parse key: " << algv[ialg] << endl;
	continue;
      }
      
      // Get hash ids for name and type
      const unsigned int id_name = TrigConf::HLTUtils::string2hash(alg_name, "ALG");
      const unsigned int id_type = TrigConf::HLTUtils::string2hash(alg_type, "ALG");
      
      // Add new algorithm configuration       
      myseq.addAlg(TrigConfAlg(0, ialg, alg_name, alg_type, id_name, id_type));
    }
    
    //
    // Iterate over input TEs
    //
    const vector<TrigConf::HLTTriggerElement *> & inputTEs = hseq -> inputTEs();
    for(unsigned ite = 0; ite < inputTEs.size(); ++ite) {
      TrigConf::HLTTriggerElement *inp_TE = inputTEs.at(ite);
      if(!inp_TE) {
	m_error << "Null input TE pointer" << endl;
	continue;	
      }
      
      myseq.addInputTE(inp_TE->id());
    }

    if(!confg.add<TrigConfSeq>(myseq)) {
      m_error << "Found duplicate sequence: " << out_te->name() << endl;
      continue;
    }
  }

  m_debug << "Prepared " << confg.size<TrigConfSeq>() << " TrigCongSeq items" << endl;

  for(unsigned int i = 0; i < confg.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = confg.at<TrigConfSeq>(i);
    m_debug << seq.getIndex() << ": " << seq.getName() << ", " << seq.getId() << endl;
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillHLT(TrigMonConfig &confg,
				   const TrigConf::HLTChainList &chn_list)
{
  //
  // Iterate over configured chains
  //
  BOOST_FOREACH(TrigConf::HLTChain *hchn, chn_list) {
    // Get singlechain configuration
    if(!hchn) {
      m_error << "Null HLTChain pointer" << endl;
      continue;
    }

    //
    // Create new chain configuration
    //
    TrigConfChain mychn(hchn->chain_name(),
			hchn->chain_counter(),
			hchn->chain_hash_id(),
			hchn->level(),
			hchn->lower_chain_name(),
			hchn->lower_chain_counter(),
			hchn->lower_chain_hash_id(),
			hchn->prescale(),
			hchn->pass_through());
    
    //
    // Iterate over configured signatures for this chain
    //
    const std::vector<TrigConf::HLTSignature*> &sig_vec = hchn->signatureList();
    for(unsigned int isig = 0; isig < sig_vec.size(); ++isig) {
      const TrigConf::HLTSignature *hsig = sig_vec[isig];
      if(!hsig) continue;
     
      // Create signature object
      TrigConfSig mysig(hsig->signature_counter(), hsig->logic(), hsig->label());

      // Iterate over configured sequences for this chain
      const std::vector<TrigConf::HLTTriggerElement*> &seq_vec = hsig->outputTEs();
      for(unsigned int iseq = 0; iseq < seq_vec.size(); ++iseq) {
	const TrigConf::HLTTriggerElement *hseq = seq_vec[iseq];
	if(!hseq) continue;
	
	// Save output TE signature name
	mysig.addOutputTE(hseq->id());

	// Find sequence configuration
	vector<TrigConfSeq>::const_iterator iseq_2 = confg.findSeq(hseq->name());
	if(iseq_2 == confg.end<TrigConfSeq>()) {
	  m_error << "Failed to find TrigConfSeq with id " << hseq->name() << endl;
	  continue;
	}
      }

      // Save signature
      mychn.addSignature(mysig);
    }

    //
    // Iterate over and add streams and groups
    //
    const vector<TrigConf::HLTStreamTag*> &str_vec = hchn->streams();
    for(unsigned int i = 0; i < str_vec.size(); ++i) {
      TrigConf::HLTStreamTag *htag = str_vec[i];
      if(htag) mychn.addStream(htag->stream(), htag->prescale());
    }
    BOOST_FOREACH(const string& grp, hchn->groups()) {
       mychn.addGroup(grp);
    }

    //
    // Add multiple input chains
    //
    const vector<unsigned int> lowids = hchn->lower_chain_hash_ids();
    
    for(unsigned ilow = 0; ilow < lowids.size(); ++ilow) {
      mychn.addLowerChainId(lowids[ilow]);
    }

    // Store chain configuration
    confg.add<TrigConfChain>(mychn);
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillStreams(TrigMonConfig &confg,
                                 const TrigConf::HLTPrescaleSet &pss)
{

   // loop over the config chain and add stream information 
   // (note that for stream prescales in FillHLT, the stream ps info is not correctly stored HLTFrame)
   TrigConf::HLTLevel level = TrigConf::HLT;
   std::string levels = "HLT";

   BOOST_FOREACH(const TrigConf::HLTPrescaleSet::ScalingMap_t::value_type& x, pss.getPrescales(level)) {
      unsigned int chain_counter = x.first;
      const TrigConf::HLTPrescale& prescale = x.second;

      // find matching confChain (by counter)
      TrigConfChain* confChain(0);
      BOOST_FOREACH(TrigConfChain& monchain, confg.getVec<TrigConfChain>()) {
         if( monchain.getCounter() == chain_counter && monchain.getLevel()==levels) {
            confChain = &monchain;
            break;
         }            
      }
      if(confChain==0) continue;
      BOOST_FOREACH(const TrigConf::HLTPrescale::PrescaleMap_t::value_type& stream_ps, prescale.getStreamPrescales()) {
         // stream name
         const std::string& streamnametype = stream_ps.first;
         std::string::size_type colPos = streamnametype.rfind(':');
         const std::string streamname = streamnametype.substr(0,colPos);
         // prescale
         float prescale = stream_ps.second;

         confChain->addStream(streamname, prescale);
      }
   }
//   // Loop over all streams
//   for(TrigConf::HLTPrescaleSet::streams_cit strIt = pss.firststream(); strIt != pss.laststream(); ++strIt ) {
//      const std::vector<std::pair<float,float> >& streamPrescales = strIt->second;

//      // stream name
//      const std::string& streamnametype = strIt->first;
//      std::string::size_type colPos = streamnametype.rfind(':');
//      const std::string streamname = streamnametype.substr(0,colPos);

//      // Looping over all chains 
//      for(unsigned i = 0; i < confg.size<TrigConfChain>(); ++i) {
//         const TrigConfChain chain = confg.at<TrigConfChain>(i);
      
//         // no streams for L1 or L2
//         if(chain.getLevelId() == 1 || chain.getLevelId() == 2) continue;

//         // prescales are ordered by chain counter
//         if(((unsigned int)chain.getCounter()) < streamPrescales.size()) {
//            float stream_ps = (float) streamPrescales[chain.getCounter()].second;

//            std::vector<TrigConfChain>::iterator iter = confg.findChainIter(chain.getId());
//            if(iter != confg.end<TrigConfChain>()) iter->addStream(streamname, stream_ps);
//         }
//      }
//   }

  return true;

}  
//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillEBHypo(TrigMonConfig &confg, const TrigConf::JobOptionTable &jot)
{
  
  // loop over the config chains, for the EB chains
  // load the L2 Hypo information, if needed
      
  // loop over all job options
  const std::vector<TrigConf::JobOption> & jobOptions = jot.jobOptionVector();
  foreach(TrigConf::JobOption jo, jobOptions) {
    if(jo.component_alias().find("_eb_") !=string::npos && 
       jo.name() == "L1ItemNames" &&
       jo.value().length() > 0 ) {

      // Looping over all chains 
      for(unsigned i = 0; i < confg.size<TrigConfChain>(); ++i) {
	const TrigConfChain chain = confg.at<TrigConfChain>(i);
      
	// only interested in L2 Hypo
	std::string jobOp_name = jo.component_alias();
        jobOp_name.replace(jobOp_name.find("Hypo_"), 5, "");
	if(chain.getName() == jobOp_name){
	  std::vector<TrigConfChain>::iterator iter = confg.findChainIter(chain.getId());
	  if(iter != confg.end<TrigConfChain>()) iter->addEBHypo(jo.value());
	}
      }
    }
  }

  return true;

}  

//---------------------------------------------------------------------------------------
void Trig::FillConf::UpdateLV1(TrigMonConfig &confg,
				     const TrigConf::CTPConfig &ctp_config)
{
  // 
  // Update L1 config
  //

  const std::vector<float> prescalesL1 = ctp_config.prescaleSet().prescales_float();

  BOOST_FOREACH(TrigConf::TriggerItem *item, ctp_config.menu().items()) {
    if(!item) {
      m_error << "Null TriggerItem pointer" << endl;
      continue;
    }
    
    float prescale = 0;
    if(int(prescalesL1.size()) > item->ctpId()) {
      prescale = prescalesL1[item->ctpId()];
    } else {
      m_error 
	    << "Bad ctpId & prescale: " << item->ctpId() << ">" << prescalesL1.size() << endl;
    }

    std::vector<TrigConfChain>::iterator iter = confg.findChainIter(item->name());
    if(iter != confg.end<TrigConfChain>()) {      
      if(std::fabs(iter->getPrescale() - prescale) > 0.0) {
	m_debug << "Changed LV1 PS for " << item->name() 
		<< " from " << iter->getPrescale() << " to " << prescale << endl;
      }

      iter -> setPrescale(prescale);
    }
    else {
      m_debug << "Failed to find LV1 PS: " << item->name() << endl;
    }
  }
}

//---------------------------------------------------------------------------------------
void Trig::FillConf::UpdateHLT(TrigMonConfig &confg,
				     const TrigConf::HLTChainList &chn_list)
{
  //
  // Iterate over configured chains
  //
//   const std::vector<TrigConf::HLTChain*> &chnv = chn_list.chains();
  
  BOOST_FOREACH(TrigConf::HLTChain* hchn, chn_list) {
     //for(unsigned int ichn = 0; ichn < chnv.size(); ++ichn) {
     // Get singlechain configuration
//      TrigConf::HLTChain *hchn = chnv[ichn];
//      if(!hchn) {
//         m_error << "Null HLTChain pointer" << endl;
//         continue;
//      }

     std::vector<TrigConfChain>::iterator iter = confg.findChainIter(hchn->chain_hash_id());
     if(iter != confg.end<TrigConfChain>()) {
        if(std::fabs(iter->getPrescale() - hchn->prescale()) > 0.0) {
           m_debug << "Changed HLT PS for " << hchn->chain_name() 
                   << " from " << iter->getPrescale() << " to " << hchn->prescale() << endl;
        }
      
        iter -> setPrescale(hchn->prescale());
        iter -> setPassThrough(hchn->pass_through());
     }
     else {
        m_debug << "Failed to find HLT PS: " << hchn->chain_name() << endl;
     }
  }
}

//---------------------------------------------------------------------------------------
bool Trig::FillConf::FillVar(TrigMonConfig &confg,
			     std::string &menu, 
			     std::string &lv1_ps_name, 
			     std::string &hlt_ps_name)
{
  //
  // Save time, hostname and pid of this process
  // 
  
  // 
  // Get my CPU speed. We may have many CPUs, but we'll assume we've been assinged just one core.
  //
  std::system("export COSTMONCPUINFO=`cat /proc/cpuinfo | grep 'cpu MHz' | head -n 1`");

  //
  // Get current time
  //
  struct timeval curTime; curTime.tv_sec = 0; curTime.tv_usec = 0;
  gettimeofday(&curTime, 0);

  std::stringstream sec_str, usc_str;
  sec_str << curTime.tv_sec;
  usc_str << curTime.tv_usec;
  
  Trig::AddOneVar(confg, "sec",  sec_str.str());
  Trig::AddOneVar(confg, "usec", usc_str.str());

  //
  // Get hostname and pid
  //
  char my_host[256];
  if(gethostname(my_host, 256) == 0) {
    std::stringstream pid_str;
    pid_str << getpid();
    
    Trig::AddOneVar(confg, "host", my_host);
    Trig::AddOneVar(confg, "pid",  pid_str.str());
  }



  Trig::AddOneVar(confg, "hostCPUSpeed", ReadVar("COSTMONCPUINFO"));
  Trig::AddOneVar(confg, "triggerMenuSetup", menu);
  Trig::AddOneVar(confg, "L1PrescaleSet",    lv1_ps_name);
  Trig::AddOneVar(confg, "HLTPrescaleSet",   hlt_ps_name);
  Trig::AddOneVar(confg, "AtlasVersion",     ReadVar("AtlasVersion"));
  Trig::AddOneVar(confg, "AtlasProject",     ReadVar("AtlasProject"));
  Trig::AddOneVar(confg, "CMTPATH",          ReadVar("CMTPATH"));

  m_debug
  << "triggerMenuSetup=" << menu << endl
  << "L1PrescaleSe="     << lv1_ps_name << endl
  << "HLTPrescaleSet="   << hlt_ps_name << endl
  << "AtlasVersion="     << ReadVar("AtlasVersion") << endl
  << "AtlasProject="     << ReadVar("AtlasProject") << endl
  << "CMTPATH="          << ReadVar("CMTPATH") << endl
  << "hostCPUSpeed="     << ReadVar("COSTMONCPUINFO") << endl;

  return true;
}
