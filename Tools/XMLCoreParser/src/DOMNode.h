/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLCOREPARSER_DOMNODE_H
#define XMLCOREPARSER_DOMNODE_H

#include <string>
#include <vector>
#include <iostream> 
#include <map> 

namespace CoreParser
{
  class DOMNode;

  typedef std::map <std::string, std::string> DOMNamedNodeMap;
  typedef std::vector <DOMNode*> DOMSiblings;

  class DOMNode
    {
    public:
      typedef enum
	{
	  DOCUMENT_NODE,
	  ELEMENT_NODE,
	  COMMENT_NODE,
	  ENTITY_NODE,
	  ENTITY_REFERENCE_NODE
	} NodeType;
      
      DOMNode ();
      DOMNode (NodeType type, const std::string& name, DOMNode* parent = 0);
      DOMNode (NodeType type, const std::string& name, const std::string& value, DOMNode* parent = 0);
      ~DOMNode ();
      NodeType get_type () const;
      const DOMNamedNodeMap& get_attributes() const;
      const std::string& get_name () const;
      const std::string& get_value () const;
      const DOMSiblings& get_siblings() const;
      unsigned int sibling_number () const;
      DOMNode* get_first_child ();
      DOMNode* get_next_sibling ();
      void print (const std::string& header = "", int depth=0) const;
      
      NodeType m_type;
      std::string m_name;
      std::string m_value;
      DOMNamedNodeMap m_attributes;
      DOMSiblings m_siblings;
      DOMSiblings::iterator m_it;
      DOMNode* m_parent;
    };
}



#endif
