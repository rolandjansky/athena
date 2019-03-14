/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/DetectorDescription/IdDictParser/src/IdDictParser.cxx,v 1.13 2008-12-09 09:55:22 dquarrie Exp $  
 
#include "IdDictParser/IdDictParser.h"  
 
#include <cstdlib>
#include <iostream> 

namespace IdDictParserNS
{
 
  class Debugger 
  { 
  public: 
    static bool get_debug_state()
    {
      return ::getenv ("XMLDEBUG") != 0;
    }
    static bool debug ()
    {
      static const bool debug_state = get_debug_state();
      return debug_state;
    }
    
    static void tab (int n) 
    {
      for (int i = 0; i < n; ++i) 
	{ 
	  std::cout << " "; 
	} 
    } 
  }; 
  
  class IdDictBaseFactory : public XMLCoreFactory  
  { 
  public:  
    void do_start (XMLCoreParser& parser, const XMLCoreNode& node);  
    void do_end (XMLCoreParser& parser, const XMLCoreNode& node);  
    
    virtual void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    virtual void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class IdDictMgrFactory : public IdDictBaseFactory  
  {  
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class DictionaryFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class FieldFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class LabelFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class AltRegionsFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end   (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class RegionFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class SubRegionFactory : public IdDictBaseFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class RegionEntryFactory : public IdDictBaseFactory  
  {   
  public:  
    virtual void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
    virtual void idd_end (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class RangeFactory : public RegionEntryFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class ReferenceFactory : public RegionEntryFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
  };  
  
  class DictionaryRefFactory : public RegionEntryFactory  
  {   
  public:  
    void idd_start (IdDictParser& parser, const XMLCoreNode& node);  
  };  
}

using namespace IdDictParserNS;
 
IdDictParser::IdDictParser () : XMLCoreParser ()  
{  
  register_factory ("IdDict",            std::make_unique<IdDictMgrFactory>());  
  register_factory ("IdDictionary",      std::make_unique<DictionaryFactory>());  
  register_factory ("field",             std::make_unique<FieldFactory>());  
  register_factory ("label",             std::make_unique<LabelFactory>());  
  register_factory ("alternate_regions", std::make_unique<AltRegionsFactory>());  
  register_factory ("region",            std::make_unique<RegionFactory>());  
  register_factory ("subregion",         std::make_unique<SubRegionFactory>());  
  register_factory ("range",             std::make_unique<RangeFactory>());  
  register_factory ("reference",         std::make_unique<ReferenceFactory>());  
  register_factory ("dictionary",        std::make_unique<DictionaryRefFactory>());
 
  m_dictionary = 0; 
  m_field      = 0; 
  m_region     = 0; 
  m_subregion  = 0; 
  m_altregions = 0;
  m_regionentry = 0;
} 
  
IdDictMgr& IdDictParser::parse (const std::string& file_name, std::string tag)  
{ 
  m_idd.clear (); 
 
  XMLCoreParser::visit (file_name);  
 
  if (Debugger::debug ()) 
    { 
      std::cout << "IdDictParser::parse1>" << std::endl; 
    } 
 
  m_idd.resolve_references (); 
 
  if (Debugger::debug ()) 
    { 
      std::cout << "IdDictParser::parse2>" << std::endl; 
    } 
 
  m_idd.generate_implementation (tag); 
 
  if (Debugger::debug ()) 
    { 
      std::cout << "IdDictParser::parse3>" << std::endl; 
    } 
 
  return (m_idd); 
}  
  
void IdDictBaseFactory::do_start (XMLCoreParser& parser,   
				  const XMLCoreNode& node)  
{  
  parser.up (); 
  idd_start ((IdDictParser&) parser, node);  
}  
  
void IdDictBaseFactory::do_end (XMLCoreParser& parser, const XMLCoreNode& node)  
{  
  idd_end ((IdDictParser&) parser, node);  
  parser.down (); 
}  
  
void IdDictBaseFactory::idd_start (IdDictParser& /*parser*/,   
				   const XMLCoreNode& /*node*/)  
{  
}  
  
void IdDictBaseFactory::idd_end (IdDictParser& /*parser*/,   
				 const XMLCoreNode& /*node*/)  
{  
}  
 
 
 
 
 
 
 
  
  
void IdDictMgrFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "IdDictMgrFactoryFactory::idd_start>" << std::endl; 
    } 
  parser.m_idd.set_DTD_version(get_value (node, "DTD_version"));
}  
  
void IdDictMgrFactory::idd_end (IdDictParser& parser,   
				const XMLCoreNode& /*node*/)  
{ 
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "IdDictMgrFactoryFactory::idd_end>" << std::endl; 
    } 
} 
  
void DictionaryFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "DictionaryFactory::idd_start>" << std::endl; 
    } 
 
  parser.m_dictionary = new IdDictDictionary; 
 
  parser.m_dictionary->m_name    = get_value (node, "name");  
  parser.m_dictionary->m_version = get_value (node, "version");  
  parser.m_dictionary->m_date    = get_value (node, "date");  
  parser.m_dictionary->m_author  = get_value (node, "author");  
}  
  
void DictionaryFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{ 
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "DictionaryFactory::idd_end> d=" << parser.m_dictionary << std::endl; 
    } 
 
  if (parser.m_dictionary != 0) 
    { 
      parser.m_idd.add_dictionary (parser.m_dictionary); 
 
      parser.m_dictionary = 0; 
    } 
}  
  
void FieldFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "FieldFactory::idd_start>" << std::endl; 
    } 
 
  IdDictField* field = new IdDictField; 
  parser.m_field = field; 
  field->m_name = get_value (node, "name");  

//    std::cout << "FieldFactory::idd_start> name=" << field->m_name <<  
//        " has begin_fixed_bits=" << has_attribute (node, "begin_fixed_bits") << std::endl; 
//    if (has_attribute (node, "fixed_bits"))
//      {
//        field->m_begin_fixed_bits = get_value (node, "fixed_bits");
//        std::cout << "FieldFactory::idd_start> name=" << field->m_name <<  
//    	  " begin_fixed_bits=" << field->m_begin_fixed_bits << std::endl; 
//    }

}  
  
void FieldFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "FieldFactory::idd_end>" << std::endl; 
    } 

  if (parser.m_field != 0) 
    { 
      if (parser.m_dictionary != 0) parser.m_dictionary->add_field (parser.m_field);  
      else delete parser.m_field; 
 
      parser.m_field = 0; 
    } 
}  
  
void LabelFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "LabelFactory::idd_start>" << std::endl; 
    } 
 
  IdDictLabel* label = new IdDictLabel; 
  label->m_name = get_value (node, "name"); 
  if (has_attribute (node, "value")) 
    { 
      label->m_valued = true; 
      label->m_value = get_int (node, "value"); 
    } 
  else 
    { 
      label->m_valued = false; 
      label->m_value = 0; 
    } 

  if (parser.m_field != 0) parser.m_field->add_label (label);  
  else delete label; 
}  
  
void LabelFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "LabelFactory::idd_end>" << std::endl; 
    } 
}  
  
void AltRegionsFactory::idd_start (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "AltRegionsFactory::idd_start>" << std::endl; 
    } 

  IdDictAltRegions* altregions = new IdDictAltRegions;

  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "AltRegionsFactory::idd_start> previous=" << parser.m_altregions
                << " new=" << altregions
                << std::endl; 
    } 

  parser.m_altregions = altregions;
}
  
void AltRegionsFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "AltRegionsFactory::idd_end>" << std::endl; 
    } 
 
  if (parser.m_altregions != 0) 
    { 

      // Set altregions to point to the default region
      IdDictAltRegions::map_iterator region_it = parser.m_altregions->m_regions.find ("");

      if (region_it == parser.m_altregions->m_regions.end())
	{
	  std::cout << "AltRegionsFactory::idd_end could not find default region. Keys in map " << std::endl;
	  IdDictAltRegions::map_iterator first = parser.m_altregions->m_regions.begin();
	  IdDictAltRegions::map_iterator last  = parser.m_altregions->m_regions.end();
	  int i = 0;
	  for (; first != last; ++first, ++i) 
	    {
	      std::cout << " i " << i << " key " << (*first).first;
	    }
	  std::cout << std::endl;
	}
      else
	{
	  parser.m_altregions->m_selected_region = (*region_it).second;
	}

      // add to dict
      if (parser.m_dictionary != 0) parser.m_dictionary->add_dictentry (parser.m_altregions);  
      else delete parser.m_altregions; 
 
      // reset pointer to altregions
      parser.m_altregions = 0; 
    }

}  
  
void RegionFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RegionFactory::idd_start>" << std::endl; 
    } 
 
  parser.m_region = new IdDictRegion; 
  if (has_attribute (node, "name"))
    {
      parser.m_region->m_name = get_value (node, "name");
    }

  // check for tag
  if (has_attribute (node, "tag")) 
    { 
      parser.m_region->m_tag = get_value (node, "tag"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//            " tag=" << parser.m_region->m_tag << std::endl; 
    } 

  // check for group name
  if (has_attribute (node, "group")) 
    { 
      parser.m_region->m_group = get_value (node, "group"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " group name =" << parser.m_region->m_group << std::endl; 
    } 

  // check for next region in absolute eta
  if (has_attribute (node, "next_abs_eta")) 
    { 
      parser.m_region->m_next_abs_eta_name = get_value (node, "next_abs_eta"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
    } 

  // Look for regions in the previous sampling
  if (has_attribute (node, "prev_samp")) { 
      std::string names = get_value (node, "prev_samp"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
 
      std::string::size_type pos = 0; 
  
      for (;;) { 
          std::string::size_type next; 
 
          next = names.find_first_not_of (" ", pos); 
          if (next == std::string::npos) break; 
          pos = next; 
          next = names.find_first_of (" ", pos); 
 
          std::string name; 
          name = names.substr (pos, next - pos); 
          parser.m_region->m_prev_samp_names.push_back (name); 
 
          if (next == std::string::npos) { 
              break; 
	  } 
          else {
	      pos = next; 
	  } 
      } 
  }
  
  // Look for regions in the next sampling
  if (has_attribute (node, "next_samp")) { 
      std::string names = get_value (node, "next_samp"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
 
      std::string::size_type pos = 0; 
  
      for (;;) { 
          std::string::size_type next; 
 
          next = names.find_first_not_of (" ", pos); 
          if (next == std::string::npos) break; 
          pos = next; 
          next = names.find_first_of (" ", pos); 
 
          std::string name; 
          name = names.substr (pos, next - pos); 
          parser.m_region->m_next_samp_names.push_back (name); 
 
          if (next == std::string::npos) { 
              break; 
	  } 
          else {
	      pos = next; 
	  } 
      } 
  }

  // Look for regions in the prev subdet
  if (has_attribute (node, "prev_subdet")) { 
      std::string names = get_value (node, "prev_subdet"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " prev_abs_eta name =" << parser.m_region->m_prev_abs_eta << std::endl; 
 
      std::string::size_type pos = 0; 
  
      for (;;) { 
          std::string::size_type prev; 
 
          prev = names.find_first_not_of (" ", pos); 
          if (prev == std::string::npos) break; 
          pos = prev; 
          prev = names.find_first_of (" ", pos); 
 
          std::string name; 
          name = names.substr (pos, prev - pos); 
          parser.m_region->m_prev_subdet_names.push_back (name); 
 
          if (prev == std::string::npos) { 
              break; 
	  } 
          else {
	      pos = prev; 
	  } 
      } 
  }
  
  // Look for regions in the next subdet
  if (has_attribute (node, "next_subdet")) { 
      std::string names = get_value (node, "next_subdet"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
 
      std::string::size_type pos = 0; 
  
      for (;;) { 
          std::string::size_type next; 
 
          next = names.find_first_not_of (" ", pos); 
          if (next == std::string::npos) break; 
          pos = next; 
          next = names.find_first_of (" ", pos); 
 
          std::string name; 
          name = names.substr (pos, next - pos); 
          parser.m_region->m_next_subdet_names.push_back (name); 
 
          if (next == std::string::npos) { 
              break; 
	  } 
          else {
	      pos = next; 
	  } 
      } 
  }
  
  // check for eta 0
  if (has_attribute (node, "eta0")) 
    { 
      parser.m_region->m_eta0 = get_double (node, "eta0"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
    } 

  // check for deta 
  if (has_attribute (node, "deta")) 
    { 
      parser.m_region->m_deta = get_double (node, "deta"); 
 
//        std::cout << "RegionFactory::idd_start> name=" << parser.m_region->m_name <<  
//  	  " next_abs_eta name =" << parser.m_region->m_next_abs_eta << std::endl; 
    } 

  // check for phi 0
  if (has_attribute (node, "phi0")) 
    { 
      parser.m_region->m_phi0 = get_double (node, "phi0"); 
    } 

  // check for dphi 
  if (has_attribute (node, "dphi")) 
    { 
      parser.m_region->m_dphi = get_double (node, "dphi"); 
    } 


  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RegionFactory::idd_start> name, group, tag, next eta, prev/next samp, prev/next subdet "
                << parser.m_region->m_name << " " 
                << parser.m_region->m_group << " " 
                << parser.m_region->m_tag << " " 
                << parser.m_region->m_next_abs_eta << " ";
      for (unsigned int i = 0; i < parser.m_region->m_prev_samp_names.size(); ++i) {
	  std::cout << parser.m_region->m_prev_samp_names[i] << " ";
      }
      for (unsigned int i = 0; i < parser.m_region->m_next_samp_names.size(); ++i) {
	  std::cout << parser.m_region->m_next_samp_names[i] << " ";
      }
      for (unsigned int i = 0; i < parser.m_region->m_prev_subdet_names.size(); ++i) {
	  std::cout << parser.m_region->m_prev_subdet_names[i] << " ";
      }
      for (unsigned int i = 0; i < parser.m_region->m_next_subdet_names.size(); ++i) {
	  std::cout << parser.m_region->m_next_subdet_names[i] << " ";
      }
      std::cout << parser.m_region->m_eta0 << " "
		<< parser.m_region->m_deta << " "
		<< parser.m_region->m_phi0 << " "
		<< parser.m_region->m_dphi << " "
		<< std::endl; 
    } 

}
  
void RegionFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RegionFactory::idd_end>" << std::endl; 
    } 
 
  if (parser.m_region != 0) 
    { 

      if (parser.m_altregions != 0) 
	{
	  //  	  std::cout << "RegionFactory::idd_end> add region to altregions, tag " 
	  //  		    <<  parser.m_region->m_tag << std::endl; 
	  parser.m_altregions->m_regions[parser.m_region->m_tag] = parser.m_region;
	  if (parser.m_dictionary != 0) parser.m_dictionary->m_all_regions.push_back (parser.m_region);  

	  // Check whether region is empty, i.e. no region entries have
	  // been found and added
	  if (parser.m_region->m_entries.size() == 0) 
	    {
	      parser.m_region->m_is_empty = true;
	    }
      
	} 
      else if (parser.m_dictionary != 0) 
	{
	  parser.m_dictionary->add_dictentry (parser.m_region);  
	  parser.m_dictionary->m_all_regions.push_back (parser.m_region);  

	  // Check whether region is empty, i.e. no region entries have
	  // been found and added
	  if (parser.m_region->m_entries.size() == 0) 
	    {
	      parser.m_region->m_is_empty = true;
	    }
	}
      else
	{
	  delete parser.m_region; 
	}
 
      parser.m_region = 0; 
    } 
}  
  
void SubRegionFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "SubRegionFactory::idd_start>" << std::endl; 
    } 
 
  parser.m_subregion = new IdDictSubRegion; 
  parser.m_subregion->m_name = get_value (node, "name"); 
} 
  
void SubRegionFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "SubRegionFactory::idd_end>" << std::endl; 
    } 
 
  if (parser.m_subregion != 0) 
    { 
      if (parser.m_dictionary != 0) parser.m_dictionary->add_subregion (parser.m_subregion);  
      else delete parser.m_subregion; 
 
      parser.m_subregion = 0; 
    } 
}  
  
void RegionEntryFactory::idd_start (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RegionEntryFactory::idd_start>" << std::endl; 
    } 
} 
 
void RegionEntryFactory::idd_end (IdDictParser& parser, const XMLCoreNode& /*node*/)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RegionEntryFactory::idd_end>" << std::endl; 
    } 
 
  if (parser.m_regionentry != 0) 
    { 
      if (parser.m_region != 0) parser.m_region->add_entry (parser.m_regionentry);  
      else if (parser.m_subregion != 0) parser.m_subregion->add_entry (parser.m_regionentry);  
      else delete parser.m_regionentry; 
 
      parser.m_regionentry = 0; 
    } 
}  
  
void RangeFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "RangeFactory::idd_start>" << std::endl; 
    } 
 
  IdDictRange* range = new IdDictRange; 
  parser.m_regionentry = range; 
  range->m_field_name = get_value (node, "field"); 
  range->m_field = 0; 
 
  range->m_specification = IdDictRange::unknown;
  range->m_continuation_mode = IdDictRange::none;
  range->m_prev_value = 0;
  range->m_next_value = 0;
 
  if (has_attribute (node, "value")) 
    { 
      range->m_specification = IdDictRange::by_label; 
      range->m_label = get_value (node, "value"); 
 
//      std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//          " value=" << range->m_label << std::endl; 
    } 
  else if (has_attribute (node, "values")) 
    { 
      range->m_specification = IdDictRange::by_labels; 
      std::string labels = get_value (node, "values"); 
 
//      std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//          " values=" << labels << std::endl; 
 
      std::string::size_type pos = 0; 
  
      for (;;) 
        { 
          std::string::size_type next; 
 
          next = labels.find_first_not_of (" ", pos); 
          if (next == std::string::npos) break; 
          pos = next; 
          next = labels.find_first_of (" ", pos); 
 
          std::string label; 
          label = labels.substr (pos, next - pos); 
          range->m_labels.push_back (label); 
 
          if (next == std::string::npos) 
            { 
              break; 
            } 
          else 
            { 
              pos = next; 
            } 
        } 
    } 
  else 
    { 
      if (has_attribute (node, "minvalue")) 
        { 
          range->m_specification = IdDictRange::by_minvalue; 
          range->m_minvalue = get_int (node, "minvalue"); 
 
//          std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//              " minvalue=" << range->m_minvalue << std::endl; 
        } 
 
      if (has_attribute (node, "maxvalue")) 
        { 
          if (range->m_specification == IdDictRange::by_minvalue) 
            range->m_specification = IdDictRange::by_minmax; 
          else 
            range->m_specification = IdDictRange::by_maxvalue; 
 
          range->m_maxvalue = get_int (node, "maxvalue"); 
 
//          std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//              " maxvalue=" << range->m_maxvalue << std::endl; 
        } 
    } 

//    std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//        " has wraparound=" << has_attribute (node, "wraparound") << std::endl; 

  if (has_attribute (node, "wraparound"))
    {
	bool wraparound = get_boolean (node, "wraparound");
	if (wraparound) range->m_continuation_mode = IdDictRange::wrap_around;
//        std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//  	  " wraparound=" << range->m_wraparound << std::endl; 
    }

  if (has_attribute (node, "prev_value"))
    {
      range->m_prev_value = get_int (node, "prev_value");
      range->m_continuation_mode = IdDictRange::has_previous;
//        std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//    	  " prev_value=" << range->m_prev_value << std::endl; 
    }

  if (has_attribute (node, "next_value"))
    {
      range->m_next_value = get_int (node, "next_value");

      if (IdDictRange::has_previous == range->m_continuation_mode) 
	{
	  range->m_continuation_mode = IdDictRange::has_both;
	}
      else
	{
	  range->m_continuation_mode = IdDictRange::has_next;
	}
      
//        std::cout << "RangeFactory::idd_start> name=" << range->m_field_name <<  
//    	  " next_value=" << range->m_next_value << std::endl; 
    }

}  
  
void ReferenceFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "ReferenceFactory::idd_start>" << std::endl; 
    } 
 
  IdDictReference* reference = new IdDictReference; 
  parser.m_regionentry = reference; 
 
  reference->m_subregion_name = get_value (node, "subregion"); 
 
//  std::cout << "ReferenceFactory::idd_start> subregion=" << reference->m_subregion_name <<  
//      std::endl; 
}  
  
void DictionaryRefFactory::idd_start (IdDictParser& parser, const XMLCoreNode& node)  
{  
  if (Debugger::debug ()) 
    { 
      Debugger::tab (parser.level());
      std::cout << "DictionaryRefFactory::idd_start>" << std::endl; 
    } 
 
  IdDictDictionaryRef* dictionaryref = new IdDictDictionaryRef; 
  parser.m_regionentry = dictionaryref; 
  dictionaryref->m_dictionary_name = get_value (node, "name"); 

  // Add dictionary name to subdictionaries
  if (dictionaryref->m_dictionary_name != "") 
    { 
      parser.m_idd.add_subdictionary_name (dictionaryref->m_dictionary_name); 
      if (parser.m_dictionary != 0) parser.m_dictionary->add_subdictionary_name (dictionaryref->m_dictionary_name);  

    } 
}  
  
