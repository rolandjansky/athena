/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HTMLABC_H
#define ANP_HTMLABC_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : AlgEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Base class for writing webpages
 * 
 **********************************************************************************/

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/HtmlNode.h"

namespace Anp
{
  class HtmlMenu {
  public:

    HtmlMenu() {}
    virtual ~HtmlMenu() {}
    
    virtual Handle<HtmlNode> FindAlgName(const std::string &key) const = 0;
    virtual Handle<HtmlNode> FindAlgType(const std::string &key) const = 0;

    virtual Handle<HtmlNode> FindChnNode(const std::string &key) const = 0;
    virtual Handle<HtmlNode> FindSeqNode(const std::string &key) const = 0;

    virtual Handle<HtmlNode> FindChnNode(unsigned id) const = 0;
    virtual Handle<HtmlNode> FindSeqNode(unsigned id) const = 0;
    
    virtual std::string GetChainLink(const std::string &) const = 0;
  };

  class HtmlABC: public virtual Base {
  public:

    HtmlABC() {}
    virtual ~HtmlABC() {}
    
    virtual void Write(HtmlNode &inode, HtmlMenu &hmenu) = 0;

    virtual const Registry& GetIndexRegistry() const = 0;    
  };
}

#endif

