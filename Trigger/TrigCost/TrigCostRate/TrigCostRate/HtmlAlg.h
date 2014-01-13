/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HTMLALG_H
#define ANP_HTMLALG_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : HtmlAlg
 * @Author : Rustem Ospanov
 *
 * @Brief  : Generate basic web pages for trigger menu:
 *           - L1 items and HLT chains
 *           - sequences and algorithms
 * 
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/HtmlNode.h"

// Local
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"

namespace Anp
{
  class HtmlAlg: 
    public virtual AlgEvent,
    public virtual HtmlMenu {
  public:

    HtmlAlg();
    virtual ~HtmlAlg();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init() { return true; }

    void Exec() {}

    void Done();

    Handle<HtmlNode> FindAlgName(const std::string &key) const;
    Handle<HtmlNode> FindAlgType(const std::string &key) const;

    Handle<HtmlNode> FindChnNode(unsigned int id) const;
    Handle<HtmlNode> FindChnNode(const std::string &key) const;

    Handle<HtmlNode> FindSeqNode(unsigned int id) const;
    Handle<HtmlNode> FindSeqNode(const std::string &key) const;

    std::string GetChainLink(const std::string &) const;

  private:
    
    void Set(const TrigMonConfig &config);

    void Fill(const TrigConfChain &chain);
    void Fill(const TrigConfSig &sig, std::ostream &os);
    void Fill(const TrigConfSeq &seq);

    void FillName(const TrigConfAlg &alg);
    void FillType(const std::vector<TrigConfAlg> &alg_vec);
    
  private:

    Anp::Registry         fReg;          // Registry configuration
    Anp::Registry         fRegIndex;     // Index page registry configuration
    
    // Properties:    
    std::string           fPathHtml;     // Write html table to this path
    bool                  fDebug;        // Print debugging info
    bool                  fPrintAll;     // Print all HTML pages (not just the rate pages). True by default
    
    // Variables:
    unsigned              fConfigCount;  // Count number of configuration calls
    TrigMonConfig         fConfig;       // Local configuration copy
    Handle<ConfigABC>     fConf;         // Configuration algorithm

    Handle<HtmlNode>      fNodeRoot;     // Root directory node
    Handle<HtmlNode>      fNodeIndex;    // Index node
  };
}

#endif
