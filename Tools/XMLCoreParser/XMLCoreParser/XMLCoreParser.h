/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLCoreParser_hpp
#define XMLCoreParser_hpp
 
#include <string>
#include <map>
#include <memory>

class XMLCoreParser;
class XMLCoreParserImpl;

namespace CoreParser
{
  class DOMNode;
}

class XMLCoreNode
{
public:
  XMLCoreNode (const CoreParser::DOMNode* node)
    : m_node (node),
      m_owns (false)
  {
  }

  XMLCoreNode (std::unique_ptr<CoreParser::DOMNode> node)
    : m_node (node.release()),
      m_owns (true)
  {
  }

  XMLCoreNode (const XMLCoreNode& other)
    : m_node (other.m_node),
      m_owns (false)
  {
  }

  XMLCoreNode (XMLCoreNode&& other)
    : m_node (other.m_node),
      m_owns (other.m_owns)
  {
    other.m_node = nullptr;
    other.m_owns = false;
  }

  XMLCoreNode&  operator= (const XMLCoreNode& other);
    
  XMLCoreNode&  operator= (XMLCoreNode&& other);

  ~XMLCoreNode();
    
  operator const CoreParser::DOMNode& () const
      {
        return (*m_node);
      }

  const CoreParser::DOMNode& get_node () const
      {
        return (*m_node);
      }

private:
  const CoreParser::DOMNode* m_node;
  bool m_owns;
};

class XMLCoreFactory 
{ 
public: 
   
  virtual ~XMLCoreFactory(); 

  void start (XMLCoreParser& parser, const XMLCoreNode& node); 
  void end (XMLCoreParser& parser, const XMLCoreNode& node); 
  void comment (XMLCoreParser& parser, const std::string& comment);
 
  virtual void do_start (XMLCoreParser& parser, const XMLCoreNode& node); 
  virtual void do_end (XMLCoreParser& parser, const XMLCoreNode& node); 
  virtual void do_comment (XMLCoreParser& parser, const std::string& comment);
  
  std::string get_name  (const XMLCoreNode& node);
  int sibling_number (const XMLCoreNode& node);
  
  static int attribute_number (const XMLCoreNode& node);
  
  static bool has_attribute (const XMLCoreNode& node, const std::string& name); 
  static int get_int (const XMLCoreNode& node, const std::string& name); 
  static double get_double (const XMLCoreNode& node, const std::string& name); 
  static bool get_boolean (const XMLCoreNode& node, const std::string& name); 
  static std::string get_ID (const XMLCoreNode& node, const std::string& name); 
  static std::string get_value (const XMLCoreNode& node, const std::string& name); 
  static std::string get_name  (const XMLCoreNode& node, int index); 
  static std::string get_token (const XMLCoreNode& node, const std::string& name); 
  
  static bool check_int  (const int n, const XMLCoreNode& node, const std::string& name); 
  static bool check_double (const int n, const XMLCoreNode& node, const std::string& name);
 
protected: 
 
  std::string m_xmlelementname; 
}; 

class XMLCoreParser 
{ 
public: 
 
  XMLCoreParser (); 
  ~XMLCoreParser (); 
  void set_validation_scheme (const char* parm); 
  void set_do_namespaces (); 
  void set_do_schema (); 
  void set_validation_schema_full_checking (); 
  void set_create_entity_reference_nodes (); 
  XMLCoreNode parse (const std::string& file_name); 
  void visit (const std::string& file_name); 
 
  void register_default_factory (std::unique_ptr<XMLCoreFactory> factory); 
  void register_factory (const std::string& name,
                         std::unique_ptr<XMLCoreFactory> factory); 
  void register_external_entity (const std::string& name, const std::string& file_name); 
  void register_text_entity (const std::string& name, const std::string& text);
 
  
private: 
 
  void visit (const XMLCoreNode& node);
  void terminate ();
  XMLCoreFactory* find_factory (const std::string& name);


  typedef std::map <std::string, std::unique_ptr<XMLCoreFactory> > FactoryMap; 
  FactoryMap m_factories;
  std::unique_ptr<XMLCoreFactory> m_default_factory;
}; 
 
#endif 
