/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HTMLRATES_H
#define ANP_HTMLRATES_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : HtmlRates
 * @Author : Rustem Ospanov, Josh Kunkle, Elliot Lipeles
 *
 * @Brief  : Algorithm for writing trigger rates results as html pages and xml file
 * 
 **********************************************************************************/

// Local
#include "TrigCostBase/AlgEvent.h"

// Local
#include "TrigCostRate/RateChain.h"
#include "TrigCostRate/RateGroup.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostRate/RateABC.h"

namespace Anp
{
  class HtmlRates: 
    public virtual AlgEvent,
    public virtual HtmlABC {
  public:

    HtmlRates();
    virtual ~HtmlRates();
    
    void Config(const Registry &reg);

    bool Init() { return true; }

    void Exec() {}

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }
   
  private:

    void WriteTotalRates(HtmlNode &inode, HtmlMenu &hmenu);

    void WriteGroupTable(Handle<HtmlNode> node,
			 HtmlMenu &hmenu,
			 unsigned level, 
			 const std::string &option,
			 std::vector<Handle<RateGroup> > groups);

    void WriteGroupRates(Handle<HtmlNode> node,
			 HtmlMenu &hmenu,
			 const std::string &option,
			 std::vector<Handle<RateGroup> > group_vec);

    void WriteChainTable(Handle<HtmlNode> node, 
			 HtmlMenu &hmenu,
			 unsigned level, 
			 const std::string &option,
			 const std::string &callee,
			 std::vector<Handle<RateChain> > chains);

    void WriteOverlRates(Handle<HtmlNode> node,
			 const std::string &option,
			 std::vector<Handle<RateGroup> > groups,
			 std::vector<Handle<RateGroup> > unions);

    void WriteMultiSeeds(Handle<HtmlNode> node,
			 HtmlMenu &hmenu,
			 const std::string &option);

    void WriteCountsByLB(std::ostream &os, HtmlNode &inode, HtmlMenu &hmenu);
    void WriteCountsByLB(std::ostream &os, HtmlNode &inode, HtmlMenu &hmenu, unsigned run);

    void WriteBandwidths(std::ostream &os);

    void WriteXML(std::ostream &os);

  private:

    typedef std::map<std::string, Handle<HtmlNode> > GroupMap;

  private:

    Anp::Registry               fRegIndex;   // Registry configuration for index page
    
    std::string                 fXmlPath;    // Write XML file to this path
    std::string                 fRatesName;  // Name of trigger rates algorithm
    std::vector<std::string>    fConfigKeys; // Configuration keys

    bool                        fDebug;           // Print debugging info
    bool                        fDoOverlapXML;    // Write overlap    groups in XML
    bool                        fDoCumultvXML;    // Write cumulative groups in XML
    bool                        fWriteLBCount;    // Write cumulative groups in XML

    Handle<RateABC>             fRates;           // Rates algorithm
    GroupMap                    fGroup;           // Map group names to html node
  };
}


#endif
