/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++
#include <iomanip>
#include <fstream>

// ROOT
#include "TSystem.h"

// Local
#include "TrigCostBase/HtmlNode.h"

using namespace std;

//-----------------------------------------------------------------------------------------
Anp::HtmlNode::HtmlNode()
  :fCont(),
   fPath(),
   fType(kNONE)
{
}

//-----------------------------------------------------------------------------------------
Anp::HtmlNode::HtmlNode(const std::string &path,
			const Type ntype)
  :fName(),
   fPath(path),
   fType(ntype)
{
  InitRoot();
}

//-----------------------------------------------------------------------------------------
Anp::HtmlNode::HtmlNode(const std::string &path,
			const std::string &name,
			const Type ntype)
  :fName(name),
   fPath(path),
   fType(ntype)
{
  InitRoot();
}

//-----------------------------------------------------------------------------------------
bool Anp::HtmlNode::Write()
{
  //
  // Write children nodes
  //
  for(NodeVec::iterator it = fChild.begin(); it != fChild.end(); ++it) {
    (*it) -> Write();
  }

  if(fType != kHTML && fType != kTEXT) {
    return true;
  }

  //
  // Write out own content
  //
  if(!fParent) {
    cout << "HtmlNode::Write - " << GetName() << " has no parent node" << endl;
    return false;
  }

  //
  // Determine directory path
  //
  string apath = GetAbsolutePath();
  if(!apath.empty() && apath.at(apath.size()-1) != '/') {
    apath = apath + "/";
  }
  
  string dpath = apath + fPath;

  if(dpath.empty()) {
    cout << "HtmlNode::Write - " << GetName() << " has empty path" << endl;
    return false;
  }
  
  std::ofstream outf(dpath.c_str());
  if(!outf.is_open()) { 
    cout << "HtmlNode::Write - " << GetName() << "failed to open file: \"" << dpath << "\"" << endl;
    return false;
  }
  
  const string &cont = fCont.str();
  outf << cont;
  
  //
  // HACK!!! Insert missing </pre>
  //
  if(cont.find( "<pre>") != string::npos &&
     cont.find("</pre>") == string::npos) {
    outf << "</pre>" << endl;
  }
  
  //
  // Write META tags
  //
  if(fType == kHTML) {
    outf << "<META HTTP-EQUIV=\"PRAGMA\" CONTENT=\"NO-CACHE\">" << endl;  
  }
  
  outf.close();

  return false;
}

//-----------------------------------------------------------------------------------------
void Anp::HtmlNode::AddLink(const std::string &text, const std::string &file, bool insert_endl)
{
  //
  // Add link to html file only
  //
  if(fType != kHTML) {
    cout << "HtmlNode::AddLink - node is not html file" << endl;
    return;
  }

  fCont << "<a STYLE=\"text-decoration:none\" href=\"" << file
	<< "\"" << ">" << text << "</a>";

  if(insert_endl) fCont << endl;
}

//-----------------------------------------------------------------------------------------
void Anp::HtmlNode::AddAnchor(const std::string &text, const std::string &anchor, bool insert_endl)
{
  //
  // Add link to html file only
  //
  if(fType != kHTML) {
    cout << "HtmlNode::AddLink - node is not html file" << endl;
    return;
  }

  fCont << "<a STYLE=\"text-decoration:none\" name=\"" << anchor
	<< "\" font-size:15px;" << ">" << text << "</a>";

  if(insert_endl) fCont << endl;
}

//-----------------------------------------------------------------------------------------
bool Anp::HtmlNode::AddChild(Anp::Handle<Anp::HtmlNode> node)
{
  //
  // Add child link
  //
  if(!node.valid()) {
    cout << "HtmlNode::AddChild - node is invalid" << endl;
    return false;
  }

  NodeVec::const_iterator nit = std::find(fChild.begin(), fChild.end(), node);
  if(nit != fChild.end()) {
    cout << "HtmlNode::AddChild - node already exists: " << endl
	 << "   new: " << node   -> GetPath() << ": " << node   -> GetName() << endl
	 << "   old: " << (*nit) -> GetPath() << ": " << (*nit) -> GetName() << endl;
    return false;
  }

  // Set parent
  node -> SetParent(this);

  fChild.push_back(node);
  return true;
}

//-----------------------------------------------------------------------------------------
const string& Anp::HtmlNode::GetAbsolutePath() const
{
  //
  // Return absolute path stored at ROOT parent node
  //
  if(fType == kROOT) {
    return this->GetPath();
  }
  if(fParent) {
    return fParent->GetAbsolutePath();
  }
  
  cout << "HtmlNode::GetAbsolutePath - " << GetName() << " has no root" << endl;
  return fPath;
}

//-----------------------------------------------------------------------------------------
Anp::HtmlNode* Anp::HtmlNode::GetRoot()
{
  //
  // Return ROOT node
  //
  if(fType == kROOT) {
    return this;
  }
  if(fParent) {
    return fParent->GetRoot();
  }
  
  cout << "HtmlNode::GetRoot - " << GetName() << " has no parent" << endl;
  return 0;
}

//-----------------------------------------------------------------------------------------
std::string Anp::HtmlNode::GetLink(std::string text, const std::string &prefix) const
{
  //
  // Get link to this file
  //
  if(fType == kNONE || fType == kROOT) {
    cout << "HtmlNode::AddLink - requesting link for wrong type" << endl;
    return "";
  }
  
  // Insert name for empty text
  if(text.empty()) {
    if(fName.empty()) text = fPath;
    else              text = fName;
  }

  return "<a STYLE=\"text-decoration:none\" href=\""
    + prefix + fPath + "\"" + ">" + text + "</a>";
}

//-----------------------------------------------------------------------------------------
std::string Anp::HtmlNode::GetAnchor(std::string text, const std::string &anchor) const
{
  //
  // Get link to this file
  //
  if(fType == kNONE || fType == kROOT) {
    cout << "HtmlNode::AddLink - requesting link for wrong type" << endl;
    return "";
  }
  
  // Insert name for empty text
  if(text.empty()) {
    if(fName.empty()) text = fPath;
    else              text = fName;
  }

  return "<a STYLE=\"text-decoration:none\" href=\"#"
    + anchor + "\"" + ">" + text + "</a>";
}

//-----------------------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlNode::FindChild(const string &key)
{
  //
  // Find child node
  // 
  if(key.empty()) {
    cout << "HtmlNode::FindChild - " << GetName() << ": key is empty" << endl;
    return Anp::Handle<Anp::HtmlNode>(0);        
  }

  //
  // Search for / to separate levels of nodes
  //
  const std::string::size_type ipos = key.find("/");

  if(ipos == string::npos) {
    //
    // First search by name
    //
    for(NodeVec::const_iterator it = fChild.begin(); it != fChild.end(); ++it) {
      if((*it) -> GetName() == key) {
	return *it;
      }
      
    }

    //
    // Then search by path
    //
    for(NodeVec::const_iterator it = fChild.begin(); it != fChild.end(); ++it) {
      if((*it) -> GetPath() == key) {
	return *it;
      }
    }
    
    //
    // Descend and search children
    //
    for(NodeVec::const_iterator it = fChild.begin(); it != fChild.end(); ++it) {
      Handle<HtmlNode> h = (*it) -> FindChild(key);
      if(h.valid()) {
	return h;
      }
    }
  }
  else {
    //
    // Recursively walk upward until all slash characters are removed
    //
    Handle<HtmlNode> node = FindChild(key.substr(0, ipos));
      
    //
    // Search in Nodes
    //
    if(node.valid()) {
      return node -> FindChild(key.substr(ipos+1));
    }
  }

  return Anp::Handle<Anp::HtmlNode>(0);
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::HtmlNode> Anp::HtmlNode::AddNode(const std::string &path, 
						  const std::string &text,
						  bool noendl) 
{
  //
  // Add node for rates table
  //
  HtmlNode::Type htype = HtmlNode::kHTML;
  
  if(path.find("html") == string::npos) { 
    htype = HtmlNode::kTEXT;
  }
  
  Handle<HtmlNode> node(new HtmlNode(path, htype));
  this->AddChild(node);    
  this->GetCont() << node->GetLink(text);
  if (!noendl) {
    this->GetCont() << endl;
  }

  return node;
}



//-----------------------------------------------------------------------------------------
bool Anp::operator==(Anp::Handle<Anp::HtmlNode> lhs, Anp::Handle<Anp::HtmlNode> rhs)
{
  if     (!lhs.valid() && !rhs.valid()) return true;
  else if(!lhs.valid()) return false;
  else if(!rhs.valid()) return false;
  
  if(lhs->GetName().empty() || rhs->GetName().empty()) {
    return lhs->GetPath() == rhs->GetPath();
  }

  return lhs->GetName() == rhs->GetName();
}

//-----------------------------------------------------------------------------------------
bool Anp::operator==(Anp::Handle<Anp::HtmlNode> node, const std::string &key)
{
  //
  // Compare node with string using name or path
  //
  if(!node.valid()) return false;

  if(node->GetName().empty()) {
    return node -> GetPath() == key;
  }

  return node -> GetName() == key;
}

//-----------------------------------------------------------------------------------------
bool Anp::operator==(const std::string &key, Anp::Handle<Anp::HtmlNode> node)
{
  return (node == key);
}

//-----------------------------------------------------------------------------------------
void Anp::HtmlNode::InitRoot()
{
  //
  // Make/check system directory for ROOT and DIR types
  //
  if(fType == kROOT || fType == kDIR) {
    //
    // Check if directory exists
    //
    FileStat_t dstat;
    if(gSystem -> GetPathInfo(fPath.c_str(), dstat) == 0) {
      if(!(dstat.fMode & kS_IFDIR)) { 
	cout << "HtmlNode::InitRoot - " << GetName() << " bad directory status" << endl;
	return;
      }
    }
    else {
      //
      // Attempt to mkdir    
      //
      if(gSystem -> mkdir(fPath.c_str()) != 0) { 
	cout << "HtmlNode::InitRoot - " << GetName() << " failed to mdkir: \"" << fPath << "\"" << endl;
	return;	
      }
    }
  }
}
