/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MiniConfig.cxx,v 1.4 2009-03-30 12:55:40 ponyisi Exp $
// **********************************************************************

#include <fstream>
#include <iostream>
#include <sstream>
//#include <algorithm>

#include "boost/algorithm/string/case_conv.hpp"
#include "boost/algorithm/string/trim.hpp"

#include "DataQualityInterfaces/MiniConfig.h"
//#include "DataQualityInterfaces/Conditions.h"


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::MiniConfig)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

MiniConfig::
MiniConfig()
    : tree(0)
    , propagateDown(true)
{
}


MiniConfig::
~MiniConfig()
{
  delete tree;
}


void
MiniConfig::
AddKeyword( std::string keyword_ )
{
  KeySet_t::value_type keyval( keyword_ );
  keywords.insert( keyval );
}


void
MiniConfig::
AddAttributeKeyword( std::string keyword_ )
{
  KeySet_t::value_type keyval( keyword_ );
  attKeywords.insert( keyval );
}

void
MiniConfig::
SetAttribKeywordPropagateDown( bool propagateDown )
{
    this->propagateDown = propagateDown;
}

bool
MiniConfig::
ReadFile( std::string fileName )
{
  bool success(true);
  
  delete tree;
  tree = new MiniConfigTreeNode( "global", 0 );
  tree->SetAttribKeywordPropagateDown( propagateDown );
  MiniConfigTreeNode* node = tree;
  
  std::ifstream file( fileName.c_str() );
  if( !file ) {
    std::cerr << "MiniConfig::ReadFile(): "
              << "cannot read from file: " << fileName << "\n";
    return false;
  }
  
  std::string line;
  char c;
  std::string key;
  std::string id;
  std::string sep;
  std::string att;
  std::string val;
  int skipCount(0);
  int lineNumber = 0;
  
  while( getline(file,line) ) {
    ++lineNumber;
    std::istringstream linestream(line);
    c = 0;
    
    while( linestream.get(c) ) {
      // ignore leading whitespace
      if( !isspace(c) ) {
        break;
      }
    }
    
    // ignore empty lines
    if( c == 0 || isspace(c) ) {
      continue;
    }
    
    // ignore comments
    if( c == '#' ) {
      continue;
    }
    
    linestream.putback(c);
    
    // check for: }
    linestream >> sep;
    if( !linestream ) {
      std::cerr << "MiniConfig::ReadFile(): "
                << "badly formatted line: \"" << line << "\", line number " << lineNumber << "\n";
      success = false;
      continue;
    }
    if( sep == "}" ) {
      if( skipCount > 0 ) {
        --skipCount;
      }
      else {
        node = node->GetParent();
        if( node == 0 ) {
          std::cerr << "MiniConfig::ReadFile(): "
                    << "unmatched \"}\", line number " << lineNumber << "\n";
          success = false;
          node = tree;
        }
      }
      continue;
    }
    
    // check for: <att> = <val>
    att = sep;
    linestream >> sep;    
    if( !linestream ) {
      std::cerr << "MiniConfig::ReadFile(): "
                << "badly formatted line: \"" << line << "\", line number " << lineNumber << "\n";
      success = false;
      continue;
    }
    if( sep == "=" ) {
      val = line.substr(linestream.tellg(), std::string::npos);
      boost::trim(val);
      //linestream >> val;
      if( val.size() == 0 ) {
        std::cerr << "MiniConfig::ReadFile(): "
                  << "badly formatted line: \"" << line << "\", line number " << lineNumber << "\n";
        success = false;
        continue;
      }
      if( skipCount == 0 ) {
	node->SetAttribute( att, val, false );
      }
      continue;
    }
    
    // check for: keyword <identifier> {
    key = att;
    const std::string& lokey = boost::algorithm::to_lower_copy(key);
    //const std::string lokey = key;
    id = sep;
    linestream >> sep;
    if( !linestream ) {
      std::cerr << "MiniConfig::ReadFile(): "
                << "badly formatted line: \"" << line << "\", line number " << lineNumber << "\n";
      success = false;
      continue;
    }
    if( sep == "{" ) {
      if( keywords.find(key) != keywords.end() 
	  || keywords.find(lokey) != keywords.end() ) {
        node = node->GetNewDaughter( id );
      }
      else if( attKeywords.find(key) != attKeywords.end()
	       || attKeywords.find(lokey) != attKeywords.end() ) {
	node->SetAttribute( id, node->GetPathName(), true );
        node = node->GetNewDaughter( id );
      }
      else {
        skipCount++;
      }
      continue;
    }
    
    std::cerr << "MiniConfig::ReadFile(): "
              << "badly formatted line: \"" << line << "\", line number " << lineNumber << "\n";
    success = false;
  }
  
  return success;
}


std::string
MiniConfig::
GetStringAttribute( std::string objName, std::string attName ) const
{
  if( tree == 0 ) {
    std::cerr << "MiniConfig::GetStringAttribute(): "
              << "not configured (no file has been read)\n";
    return std::string("");
  }
  
  const MiniConfigTreeNode* node = tree->GetNode( objName );
  if( node == 0 ) {
    std::cerr << "MiniConfig::GetStringAttribute(): "
              << "\"" << objName << "\" does not exist\n";
    return std::string("");
  }
  return node->GetAttribute( attName );
}


int
MiniConfig::
GetIntAttribute( std::string objName, std::string attName ) const
{
  if( tree == 0 ) {
    std::cerr << "MiniConfig::GetIntAttribute(): "
              << "not configured (no file has been read)\n";
    return 0;
  }
  
  const MiniConfigTreeNode* node = tree->GetNode( objName );
  if( node == 0 ) {
    std::cerr << "MiniConfig::GetIntAttribute(): "
              << "\"" << objName << "\" does not exist\n";
    return 0;
  }
  
  int val;
  std::string valstring = node->GetAttribute( attName );
  std::istringstream valstream(valstring);
  valstream >> val;
  if( !valstream ) {
    std::cerr << "MiniConfig::GetIntAttribute(): "
              << "\"" << attName << "\" not an integer type\n";
    return 0;
  }
  
  return val;
}


float
MiniConfig::
GetFloatAttribute( std::string objName, std::string attName ) const
{
  if( tree == 0 ) {
    std::cerr << "MiniConfig::GetFloatAttribute(): "
              << "not configured (no file has been read)\n";
    return 0;
  }
  
  const MiniConfigTreeNode* node = tree->GetNode( objName );
  if( node == 0 ) {
    std::cerr << "MiniConfig::GetFloatAttribute(): "
              << "\"" << objName << "\" does not exist\n";
    return 0;
  }
  
  float val;
  std::string valstring = node->GetAttribute( attName );
  std::istringstream valstream(valstring);
  valstream >> val;
  if( !valstream ) {
    std::cerr << "MiniConfig::GetFloatAttribute(): object \"" << objName << "\""
              << ": \"" << attName << "\" not a floating-point type\n";
    return 0;
  }
  
  return val;
}


void
MiniConfig::
GetAttributeNames( std::string objName, std::set<std::string>& attSet ) const
{
  attSet.clear();
  
  if( tree == 0 ) {
    std::cerr << "MiniConfig::GetAttributeNames(): "
              << "not configured (no file has been read)\n";
    return;
  }
  
  const MiniConfigTreeNode* node = tree->GetNode( objName );
  if( node == 0 ) {
    std::cerr << "MiniConfig::GetAttributeNames(): "
              << "\"" << objName << "\" does not exist\n";
    return;
  }
  
  node->GetAttributeNames( attSet );
}


void
MiniConfig::
SendVisitor( const MiniConfigTreeNode::Visitor& visitor ) const
{
  if( tree == 0 ) {
    std::cerr << "MiniConfig::SendVisitor(): "
              << "not configured (no file has been read)\n";
  } else {
    tree->Accept(visitor);
  }
}


} // namespace dqi

