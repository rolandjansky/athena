/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <stdint.h>

// ROOT
#include "TDirectory.h"
#include "TH1.h"

// Local
#include "TrigCostBase/UtilCore.h"

// XML
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlversion.h"
#include "libxml/xmlstring.h"

using namespace std;

//-----------------------------------------------------------------------------
void Anp::StringTok(std::vector<std::string>& ls,
		    const std::string& str,
		    const std::string& tok)
{
  //======================================================================
  // Split a long string into a set of shorter strings spliting along
  // divisions makers by the characters listed in the token string
  //======================================================================
  const string::size_type S = str.size();
  string::size_type  i = 0;  

  while (i < S) {
    // eat leading whitespace
    while (i < S && tok.find(str[i]) != string::npos) {
      ++i;
    }
    if (i == S) break;  // nothing left but WS
    
    // find end of word
    string::size_type  j = i+1;
    while (j < S && tok.find(str[j]) == string::npos) {
      ++j;
    }
    
    // add word
    ls.push_back(str.substr(i,j-i));
    
    // set up for next loop
    i = j+1;
  }  
}

//-----------------------------------------------------------------------------
TDirectory* Anp::GetDir(std::string path, TDirectory *dir)
{
  //
  // Recursively create directory path in dir
  //
  if(!dir || path.empty() || path == "/") {
    return dir;
  }
  
  // remove trailing slash if present
  if(path.substr(path.size() - 1, 1) == "/") {
    path = path.substr(0, path.size() - 1);
  }
  
  std::string::size_type pos = path.find_last_of('/');
  if(pos != std::string::npos) {
    // recursively walk upward until all slash characters are removed
    dir = Anp::GetDir(path.substr(0, pos), dir);
    
    // make directory path of last substring without slash
    path = path.substr(pos + 1, path.size() - pos);
  }
  
  if(dir -> Get(path.c_str())) {
    return dynamic_cast<TDirectory *>(dir -> Get(path.c_str()));
  }
  
  return dir -> mkdir(path.c_str());
}

//-----------------------------------------------------------------------------
TDirectory* Anp::GetDir(TDirectory *dir, std::string path)
{
  return Anp::GetDir(path, dir);
}

//-----------------------------------------------------------------------------
TH1* Anp::SetDir(TH1 *h, TDirectory *dir, const string &name)
{
  if(!h) return h;

  if(!name.empty()) {
    h -> SetName(name.c_str());
  }

  h -> SetDirectory(dir);

  return h;
}

//---------------------------------------------------------------------------------
pair<string, string>  Anp::Round2Pair(double value, double error, int)
{
   //
   // Round value using error
   //
   stringstream valueS, errorS;
  
   if(error < 0.0)
   {
      valueS << static_cast<int64_t>(value);
      return pair<string, string>(valueS.str(), errorS.str());  
   }
   if(!(error > 0.0))
   {
      valueS << value;
      errorS << error;
      return pair<string, string>(valueS.str(), errorS.str());  
   }
   
   //
   // Get base 10 power for decimal place in error
   //
   short elogi = 0;
   if(error < 1.0)
   {
      elogi = -static_cast<int>(std::ceil(std::fabs(std::log10(error))));
   }
   else
   {
      elogi = +static_cast<int>(std::floor(std::fabs(std::log10(error))));
   }
      
   //
   // Scale factor to place top two digits in error between 0 and 99
   //
   const double factor = 10.0*std::pow(10.0, -elogi);
      
   //
   // Scale value and error up and round off parts after decimal place
   //
   const double valueD = factor*std::fabs(value);
   const double errorD = factor*error;
      
   int64_t errorI = static_cast<int64_t>(std::floor(errorD));
   int64_t valueI = static_cast<int64_t>(std::floor(valueD));
   
   if(errorD - std::floor(errorD) > 0.5) ++errorI;
   if(valueD - std::floor(valueD) > 0.5) ++valueI;  

   //
   // Get precision for error
   //
   int64_t valueP = 0;
   if     (elogi == 0) valueP = 1;
   else if(elogi  < 0) valueP = 1 - elogi;

   if(value < 0.0) valueS << "-";

   valueS << std::fixed
	  << std::setprecision(valueP) << static_cast<double>(valueI)/factor;
   errorS << std::fixed
	  << std::setprecision(valueP) << static_cast<double>(errorI)/factor;
   
   return pair<string, string>(valueS.str(), errorS.str());
}

//-----------------------------------------------------------------------------
std::string Anp::PadStrFront(std::string str, int width)
{
  // Pad str with blanks
  if(width < 1) return str; 

  const int osize = str.size();
  for(int i = 0; i < width; ++i) {
    if(i >= osize) str.insert(str.begin(), ' ');
  }
  
  return str;
}

//-----------------------------------------------------------------------------
std::string Anp::PadStrBack(std::string str, int width)
{
  // Pad str with blanks
  if(width < 1) return str; 

  const int osize = str.size();
  for(int i = 0; i < width; ++i) {
    if(i >= osize) str.push_back(' ');
  }
  
  return str;
}

//------------------------------------------------------------------------------------------
double Anp::file_size(const std::string &path)
{
   //
   // Return file size in megabytes
   //
   struct stat result;
   if(stat(path.c_str(), &result) != 0)
   {
      return 0.0;
   }
   
   return double(result.st_size)/1038336.0;
}

//------------------------------------------------------------------------------------------
std::string Anp::MakeLink(const std::string &link, const std::string &text)
{
  //
  // Return html code to hyperlink to this group
  //
  stringstream lstr;
  lstr << "<a STYLE=\"text-decoration:none\" href=\""
       << link
       << "\"" << ">" << text << "</a>";
  
  return lstr.str();
}

//-----------------------------------------------------------------------------------------      
// Helper functions for converting comma delimated string to vector of values
//
vector<int> Anp::GetIntVec(const string &list)
{
   vector<string> namelist;
   Anp::StringTok(namelist, list, ", ");
   
   vector<int> ivec;
   for(vector<string>::const_iterator sit = namelist.begin(); sit != namelist.end(); ++sit)
   {
      const int newbase = std::atoi(sit -> c_str());
      if(newbase != 0)
      {
	 ivec.push_back(newbase);
      }
   }
   
   return ivec;
}

vector<string> Anp::GetStringVec(const string &list)
{
   vector<string> namelist;
   Anp::StringTok(namelist, list, ", ");

   vector<string> nvec;
   for(vector<string>::const_iterator sit = namelist.begin(); sit != namelist.end(); ++sit)
   {
      if(!sit -> empty())
      {
	 nvec.push_back(*sit);
      }
   }
   
   return nvec;
}

//-----------------------------------------------------------------------------------------      
namespace Anp
{
  //
  // Helper function for libxml2 parser:
  //   - read attribute properties for a node
  //
  unsigned CountAttr(xmlNode *node)
  {
    if(!node || !node->properties) {
      return 0;
    }
    
    unsigned icount = 0;

    for(xmlAttr *cur_attr = node->properties; cur_attr; cur_attr = cur_attr->next) {
      if(cur_attr->children) ++icount; 
    }
    
    return icount;
  }

  //
  // Helper function for libxml2 parser:
  //   - count number of children elements node
  //
  unsigned CountChildrenElements(xmlNode *node)
  {
    if(!node || !node->children) {
      return 0;
    }
    
    unsigned icount = 0;

    for(xmlNode *cur_node = node->children; cur_node; cur_node = cur_node->next) {
      if(cur_node->type == XML_ELEMENT_NODE) ++icount; 
    }
    
    return icount;
  }

  //
  // Helper function for libxml2 parser:
  //   - read attribute properties for a node
  //
  void ReadAttr(xmlNode *node, Anp::Registry &reg)
  {
    if(!node || !node->properties) {
      return;
    }

    for(xmlAttr *cur_attr = node->properties; cur_attr; cur_attr = cur_attr->next) {
      
      std::stringstream atr_text, atr_name;
      atr_text << xmlNodeGetContent(cur_attr->children);
      atr_name << cur_attr->name;
      
      double atr_double = -1.0e100;
      atr_text >> atr_double;
      
      if(atr_text.str().size() > 0 &&
	 atr_text.rdstate() != std::ios_base::failbit &&
	 atr_text.rdstate() == std::ios_base::eofbit) {
	reg.Set(atr_name.str(), atr_double);
      }
      else {
	reg.Set(atr_name.str(), atr_text.str());
      }
    }
  }

  //
  // Helper function for libxml2 parser:
  //   - recursively step through the tree, create a registry at each new level
  //
  void ReadTree(xmlNode *node, Anp::Registry &reg, bool debug)
  {
    if(!node) {
      return;
    }

    for(xmlNode *sig_node = node->children; sig_node; sig_node = sig_node->next) {
      
      if(!sig_node || !(sig_node->name)) {
	std::cerr << "ReadTree - error: null node name" << endl;
	continue;
      }
      
      std::stringstream sig_name;
      sig_name << sig_node->name;

      if(sig_node->type != XML_ELEMENT_NODE) {
	continue;
      }

      //
      // Make child Registry
      //
      Anp::Registry child_reg;
      child_reg.AllowNonUniqueKeys();

      //
      // Extract data only from nodes without children
      //
      const unsigned nattr = CountAttr(sig_node);
      const unsigned nelem = CountChildrenElements(sig_node); 

      if(nelem == 0) {
	//
	// Node has no children nodes, just text data and/or attributes
	//
	std::stringstream sig_text;
	
	double val_double = -1.0e100;
	bool has_double_data = false;

	if(sig_node->children && 
	   sig_node->children->type == XML_TEXT_NODE &&
	   !xmlIsBlankNode(sig_node->children)) {

	  sig_text << xmlNodeGetContent(sig_node->children);	
	  sig_text >> val_double;

	  if(sig_text.str().size() > 0 &&
	     sig_text.rdstate() != std::ios_base::failbit &&
	     sig_text.rdstate() == std::ios_base::eofbit) {
	    has_double_data = true;
	  }
	}
	
	if(nattr == 0) {
	  if(has_double_data) {
	    reg.Set(sig_name.str(), val_double);
	  }
	  else {
	    reg.Set(sig_name.str(), sig_text.str());
	  }
	}
	else {
	  if(has_double_data) {
	    child_reg.Set(sig_name.str(), val_double);
	  }
	  else {
	    child_reg.Set(sig_name.str(), sig_text.str());
	  }
	}
      }
      
      if(nelem > 0 || nattr > 0) {
	//
	// Decend further into the tree: recursive call
	//
	ReadTree(sig_node, child_reg, debug); 
	
	//
	// Read attribute properties
	//
	ReadAttr(sig_node, child_reg);

	//
	// Save child registry
	//
	reg.Set(sig_name.str(), child_reg);
      }
    }
  }
}

//
// Parse XML file into Registry using libxml2
//
Anp::Registry Anp::ParseXml(const std::string &fname, const bool debug)
{  
  LIBXML_TEST_VERSION
    
  Anp::Registry reg;
  reg.AllowNonUniqueKeys();
  
  //
  // Parse the file and get the DOM 
  //
  xmlDoc *doc = xmlReadFile(fname.c_str(), NULL, 0);
  
  if(doc == NULL) {
    cout << "ParseXml - failed to parse file: " << fname << endl;
    return reg;
  }

  //
  //Get the root element node 
  //
  xmlNode *root_element = xmlDocGetRootElement(doc);

  if(root_element == NULL || !(root_element->name)) {
    cout << "ParseXml - null root element pointer or missing name" << endl;
    xmlFreeDoc(doc);
    return reg;
  }

  std::stringstream root_name;
  root_name << root_element->name;
  
  reg.Set("RootElement", root_name.str());

  //
  // Parse XML tree
  //
  ReadTree(root_element, reg, debug);
  
  if(debug) reg.Print();

  //
  // Free document and free global variables that may have been allocated
  //
  xmlFreeDoc(doc);
  xmlCleanupParser();

  return reg;
}
