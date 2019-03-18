/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <iostream> 
#include <map> 
 
#include "DOMNode.h"

CoreParser::DOMNode::DOMNode () : m_type (DOCUMENT_NODE), m_parent (0)
{
}

CoreParser::DOMNode::DOMNode (NodeType type, const std::string& name, DOMNode* parent) : 
  m_type (type), m_name (name), m_parent (parent)
{
  if (parent != 0)
    {
      parent->m_siblings.push_back (this);
    }
  
  if (type == COMMENT_NODE)
    {
      m_value = name;
      m_name = "";
    }
}

CoreParser::DOMNode::DOMNode (NodeType type, const std::string& name, const std::string& value, DOMNode* parent) : 
  m_type (type), m_name (name), m_value (value), m_parent (parent)
{
  if (parent != 0)
    {
      parent->m_siblings.push_back (this);
    }
  
  if (type == COMMENT_NODE)
    {
      m_value = name;
      m_name = "";
    }
}

CoreParser::DOMNode::~DOMNode ()
{
  DOMSiblings::iterator sit;

  for (sit = m_siblings.begin (); sit != m_siblings.end (); ++sit)
    {
      const DOMNode* n = *sit;
      
      delete n;
    }
}

CoreParser::DOMNode::NodeType CoreParser::DOMNode::get_type () const
{
  return (m_type);
}

const CoreParser::DOMNamedNodeMap& CoreParser::DOMNode::get_attributes() const
{
  return (m_attributes);
}

const std::string& CoreParser::DOMNode::get_name () const
{
  return (m_name);
}

const std::string& CoreParser::DOMNode::get_value () const
{
  return (m_value);
}

const CoreParser::DOMSiblings& CoreParser::DOMNode::get_siblings() const
{
  return (m_siblings);
}

unsigned int CoreParser::DOMNode::sibling_number () const
{
  return (m_siblings.size ());
}

CoreParser::DOMNode* CoreParser::DOMNode::get_first_child ()
{
  m_it = m_siblings.begin ();
  if (m_it == m_siblings.end ()) return (0);
  return (*m_it);
}

CoreParser::DOMNode* CoreParser::DOMNode::get_next_sibling ()
{
  if (m_it == m_siblings.end ()) return (0);
  ++m_it;
  if (m_it == m_siblings.end ()) return (0);
  return (*m_it);
}

void CoreParser::DOMNode::print (const std::string& header,
                                 int depth /*= 0*/) const
{
  if (header != "")
    {
      std::cout << header << std::endl;
    }
  
  int i;

  if (m_type == COMMENT_NODE)
    {
      for (i = 0; i < depth; i++) std::cout << "  ";
      std::cout << "<!--" << m_value << "-->" << std::endl;
    }
  /*
  else if (m_type == DOCUMENT_NODE)
    {
    }
  */
  else
    {
      for (i = 0; i < depth; i++) std::cout << "  ";
      std::cout << "<" << m_name;
  
      std::map <std::string, std::string>::const_iterator ait;
      
      for (ait = m_attributes.begin (); ait != m_attributes.end (); ++ait) 
	{
	  const std::string& n = (*ait).first;
	  const std::string& v = (*ait).second;
	  
	  std::cout << " " << n << "='" << v << "'";
	}
      
      std::cout << ">" << std::endl;
      
      DOMSiblings::const_iterator sit;
      
      for (sit = m_siblings.begin (); sit != m_siblings.end (); ++sit)
	{
	  const DOMNode* n = *sit;
	  
	  n->print ("", depth+1);
	}
      
      for (i = 0; i < depth; i++) std::cout << "  ";
      std::cout << "</" << m_name << ">" << std::endl;
    }
}
