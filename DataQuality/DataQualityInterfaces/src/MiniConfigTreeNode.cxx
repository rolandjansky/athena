/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: MiniConfigTreeNode.cxx,v 1.5 2008-12-04 16:40:16 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/MiniConfigTreeNode.h"


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::MiniConfigTreeNode)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

MiniConfigTreeNode::
MiniConfigTreeNode( std::string name_, MiniConfigTreeNode* parent_ )
  : m_name(name_)
  , m_parent(parent_)
  , m_propagateDown(true)
{
}


MiniConfigTreeNode::
~MiniConfigTreeNode()
{
  NodeIter_t daugEnd = m_daughters.end();
  for( NodeIter_t i = m_daughters.begin(); i != daugEnd; ++i ) {
    MiniConfigTreeNode* inode = i->second;
    delete inode;
  }
}


const char*
MiniConfigTreeNode::
GetName() const
{
  return m_name.c_str();
}


const char*
MiniConfigTreeNode::
GetPathName() const
{
  m_path = "";
  if( m_parent != 0 ) {
    std::string parentPath( m_parent->GetPathName() );
    if( parentPath != "" ) {
      m_path += parentPath;
      m_path += std::string("/");
    }
    m_path += m_name;
  }
  return m_path.c_str();
}


MiniConfigTreeNode*
MiniConfigTreeNode::
GetNewDaughter( std::string name_ )
{
  NodeIter_t i = m_daughters.find( name_ );
  if( i != m_daughters.end() ) {
    return i->second;
  }

  MiniConfigTreeNode* node = new MiniConfigTreeNode( name_, this );
  node->SetAttribKeywordPropagateDown(this->m_propagateDown);
  NodeMap_t::value_type nodeVal( name_, node );
  m_daughters.insert( nodeVal );
  return node;
}


MiniConfigTreeNode*
MiniConfigTreeNode::
GetDaughter( std::string name_ ) const
{
  NodeIter_t i = m_daughters.find( name_ );
  if( i != m_daughters.end() ) {
    return i->second;
  }
  return 0;
}


MiniConfigTreeNode*
MiniConfigTreeNode::
GetParent() const
{
  return m_parent;
}


std::map<std::string,MiniConfigTreeNode*>
MiniConfigTreeNode::
GetDaughters() const
{
  return m_daughters;
}

const MiniConfigTreeNode*
MiniConfigTreeNode::
GetNode( std::string name_ ) const
{
  if( m_daughters.size() == 0 ) {
    return this;
  }

  std::string::size_type k = name_.find_first_of('/');
  if( k != std::string::npos ) {
    std::string dName( name_, 0, k );
    std::string pName( name_, k+1, std::string::npos );
    if( dName != "" ) {
      NodeIter_t i = m_daughters.find( dName );
      if( i == m_daughters.end() ) {
        return this;
      }
      MiniConfigTreeNode* node = i->second;
      return node->GetNode( pName );
    }
    return GetNode( pName );
  }

  NodeIter_t i = m_daughters.find( name_ );
  if( i == m_daughters.end() ) {
    return this;
  }
  return i->second;
}


void
MiniConfigTreeNode::
SetAttribute( std::string attName, std::string attValue, bool isAttribKeyword )
{
    AttMap_t::value_type attMapVal( attName, AttMap_t::mapped_type(attValue, isAttribKeyword) );
  m_attributes.insert( attMapVal );
}


std::string
MiniConfigTreeNode::
GetAttribute( std::string attName, bool calledFromDaughter ) const
{
  AttIter_t i = m_attributes.find( attName );
  if( i == m_attributes.end() ) {
    if( m_parent != 0 )
      return m_parent->GetAttribute( attName, true );
    else
      return std::string("");
  }
  if (calledFromDaughter && ! m_propagateDown && i->second.second) {
      return std::string("");
  }
  return i->second.first;
}

// Like GetAttributes, but only returns attributes of *this* node,
// not inherited ones
std::string
MiniConfigTreeNode::
GetAttributeLocal( std::string attName ) const
{
  AttIter_t i = m_attributes.find( attName );
  if( i == m_attributes.end() ) {
    return std::string("");
  }
  return i->second.first;
}


void
MiniConfigTreeNode::
GetAttributeNames( std::set<std::string>& attSet, bool calledFromDaughter ) const
{
  if (calledFromDaughter && ! m_propagateDown) {
      return;
  }
  AttIter_t attEnd = m_attributes.end();
  for( AttIter_t i = m_attributes.begin(); i != attEnd; ++i ) {
    if (calledFromDaughter && ! m_propagateDown && i->second.second) {
    } else {
	std::set<std::string>::value_type setVal( i->first );
	attSet.insert( setVal );
    }
  }
  if( m_parent !=0 ) {
    m_parent->GetAttributeNames( attSet, true );
  }
}

// Like GetAttributeNames, but only returns attributes of this node
void
MiniConfigTreeNode::
GetAttributeNamesLocal( std::set<std::string>& attSet ) const
{
  AttIter_t attEnd = m_attributes.end();
  for( AttIter_t i = m_attributes.begin(); i != attEnd; ++i ) {
    std::set<std::string>::value_type setVal( i->first );
    attSet.insert( setVal );
  }
}

void
MiniConfigTreeNode::
Accept( const Visitor& visitor ) const
{
  visitor.Visit(this);
  NodeIter_t daugEnd = m_daughters.end();
  for( NodeIter_t i = m_daughters.begin(); i != daugEnd; ++i ) {
    MiniConfigTreeNode* node = i->second;
    node->Accept(visitor);
  }
}

void
MiniConfigTreeNode::
Accept(Writer& writer )
{
  writer.Write(this);
  NodeIter_t daugEnd = m_daughters.end();
  for( NodeIter_t i = m_daughters.begin(); i != daugEnd; ++i ) {
    MiniConfigTreeNode* node = i->second;
    node->Accept(writer);
  }
}

void
MiniConfigTreeNode::
SetAttribKeywordPropagateDown( bool propagateDown )
{
  this->m_propagateDown = propagateDown;
}

bool
MiniConfigTreeNode::
GetAttribKeywordPropagateDown() const
{
  return this->m_propagateDown;
}

} // namespace dqi
