/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_UTILTRIG_H
#define ANP_UTILTRIG_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Global utility functions for manipulating trigger data
 * 
 **********************************************************************************/

// C/C++
#include <iostream>
#include <cmath>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/HtmlNode.h"

class TCanvas;

namespace Trig
{  
  //
  // Get all chain names which depend on this chain
  //
  int Prune(std::vector<TrigConfChain> &ivec, std::vector<TrigConfChain> &cvec);

  //
  // Make input chains using cvec for pvec chains with positive pass-through PT
  //
  bool MakeLowerVecForPT(const std::vector<TrigConfChain> &cvec,
			 const std::vector<TrigConfChain> &pvec,
			 std::vector<TrigConfChain> &ovec);

  //
  // Make input chains using cvec for pvec chains with positive pass-through PT
  //
  void MakeGroupCompliment(const std::vector<TrigConfChain> &confg_vec,
			   const std::vector<TrigConfChain> &group_vec,
			   std::vector<TrigConfChain> &result_vec);

  std::string GetLevel(unsigned int level);

  std::string CombinedName(const std::vector<std::string> &cvec);

  void PrintVec(const std::vector<TrigConfChain> &cvec,
		const std::string &option = "PS PT newline");

  void WidthPad(const std::vector<TrigConfChain> &cvec, std::vector<std::string> &nvec);

  void PrintColumns(std::vector<std::string> svec, 
		    std::ostream &os, 
		    const int ncolumn,
		    Anp::Handle<Anp::HtmlNode> pnode);

  std::vector<TrigConfSeq> GetSeq(const std::vector<TrigConfChain> &chains, 
				  const std::vector<TrigConfSeq> &seq_vec);
  std::vector<TrigConfSeq> GetSeq(const TrigConfChain &chain,
				  const std::vector<TrigConfSeq> &seq_vec);
  std::vector<TrigConfSeq> GetSeq(const TrigConfChain &chain,
				  const TrigMonConfig &config);

  //
  // Read Group XML
  //
  struct GrData {

    GrData() {}
    
    std::string name;
    std::string type;
    std::vector<std::string> chains;
  };

  
  std::vector<GrData> GroupXMLparser(const std::string &fname,
		                                bool debug = false);   

  //
  // Read Prescale/Passthrough/Express stream PS from XML
  //
  struct PSData {
    PSData() : counter(0), ps(0.0), pt(0.0), es_ps(0.0), 
	 has_ps(false), has_pt(false), has_es(false) {}

    unsigned counter;
    double   ps;
    double   pt;
    double   es_ps;
    bool     has_ps;
    bool     has_pt;
    bool     has_es;
  };

  std::map<std::string, PSData> PSXMLparser(const std::string &fname, bool debug = false);

  struct BunchGroup 
  {
    BunchGroup() : keynum(-1), size(0) {}
    
    std::string   name;
    std::string   key;
    
    int           keynum;
    int           size;
    std::set<int> bcids;
    
    void PrintBunchGroup(std::ostream &os = std::cout) const;
  };
  std::map<std::string, BunchGroup> BunchXMLparser(const std::string &fname, bool debug = false);

  std::map<std::string, PSData> PSXMLCost   (Anp::Registry &PSdef, bool debug);
  std::map<std::string, PSData> PSXMLMenuLV1(Anp::Registry &PSdef, bool debug);
  std::map<std::string, PSData> PSXMLMenuHLT(Anp::Registry &PSdef, bool debug);

  //
  // Update TrigMonConfig using XML file(s)
  //
  // update prescales
  void UpdateChainPS(TrigMonConfig &config, 
		     const std::map<std::string, Trig::PSData> &data, bool debug = false);
  // update groups
  void UpdateGroups(TrigMonConfig &config, 
		    const std::vector<GrData> &grplist, bool debug = false);

  //
  // Return packed buts as string
  //
  std::string BitsAsString_uint32(uint32_t val);
  std::string BitsAsString_uint16(uint16_t val);

  //
  // Sequence state as a word
  //
  std::string SeqStateAsString(const TrigMonSeq &entry);
}

namespace Anp
{
  //
  // Convert trigger decision enum to/from string
  //
  std::string AsString(TrigMonL1Item::Decision decs);
  std::string AsString(TrigMonChain::Decision decs);

  TrigMonL1Item::Decision AsTriggerBitLV1(const std::string &b);
  TrigMonChain::Decision  AsTriggerBitHLT(const std::string &b);

  void PrintHist(HtmlNode &inode,
		 TH1 *h,
		 TCanvas *canvas,
		 std::string canvas_name,
		 std::string text, 
		 std::string opts = "");

  unsigned GetWidth(const double val);

  void SetROOT_defaults();

}

#endif
