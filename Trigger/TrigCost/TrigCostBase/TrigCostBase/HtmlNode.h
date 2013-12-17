/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_HTMLNODE_H
#define ANP_HTMLNODE_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : HtmlNode
 * @Author : Rustem Ospanov
 *
 * @Brief  : Wrapper class for html pages - used to write html/text files 
 *           on local file system.
 * 
 **********************************************************************************/

// C++
#include <iostream>
#include <sstream>
#include <string>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/Registry.h"
#include "TrigCostBase/UtilCore.h"

namespace Anp
{   
  class HtmlNode: virtual public Base {
  public:

    enum Type { kNONE, kROOT, kHTML, kGIF, kDIR, kTEXT, kLIST };

    typedef std::vector<Anp::Handle<Anp::HtmlNode> > NodeVec;    

  public:
    
    HtmlNode();
    HtmlNode(const std::string &path, const Type ntype);
    HtmlNode(const std::string &path, const std::string &name, const Type ntype);

    ~HtmlNode() {}

    void Config(const Registry &reg);
    
    bool Write();

    Handle<HtmlNode> FindChild(const std::string &key);

    Handle<HtmlNode> FindNode(const std::string &key) { return FindChild(key); }

    bool AddChild(Handle<HtmlNode> node);
    
    const std::string &GetAbsolutePath() const;

    void AddCont(const std::string &str) { fCont << str; }
    void AddLink(const std::string &text, const std::string &file, bool insert_endl = true);
    void AddAnchor(const std::string &text, const std::string &anchor, bool insert_endl = true);

    void SetName(const std::string &name) { fName = name; }
    void SetBase(const std::string &base) { fBase = base; }

    Type GetType() const { return fType; }

          std::stringstream&  GetCont()       { return fCont; }
    const std::stringstream&  GetCont() const { return fCont; }

    HtmlNode* GetParent() { return fParent; }
    HtmlNode* GetRoot();

    std::string GetLink(std::string text = "", const std::string &prefix = "") const;
    std::string GetAnchor(std::string text = "", const std::string &anchor = "") const;

    const std::string& GetName() const { return fName; }
    const std::string& GetPath() const { return fPath; }
    const std::string& GetBase() const { return fBase; }
    
    Handle<HtmlNode> AddNode(const std::string &path, 
			     const std::string &text,
			     bool noendl=false);


  private:

    void SetParent(HtmlNode *node) { fParent = node; }

    void InitRoot();

  private:
    
    std::stringstream      fCont;   // Content of html file

    std::string            fName;   // Node name (can be anything really)
    std::string            fPath;   // Path to this node (absolute or relative)
    std::string            fBase;   // Base part of path to this node

    Type                   fType;   // Node type

    HtmlNode              *fParent; // Parent node
    NodeVec                fChild;  // Children nodes
    NodeVec                fLink;   // Links references by html file
  };

  //
  // Comparison operators
  //
  bool operator==(Handle<HtmlNode> lhs, Handle<HtmlNode> rhs);

  bool operator==(Handle<HtmlNode> node, const std::string &key);
  bool operator==(const std::string &key, Handle<HtmlNode> node);
}

#endif
