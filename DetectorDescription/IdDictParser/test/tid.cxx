/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/DetectorDescription/IdDictParser/test/tid.cxx,v 1.3 2005-04-29 16:11:22 schaffer Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/Identifier.h" 
 
#include <iostream> 
 
static void tab (size_t level) 
{ 
  for (size_t i = 0; i < level; ++i) std::cout << " "; 
} 
 
int main (int argc, char* argv[])  
{  
  if (argc < 2) return (1);  
  
  IdDictParser parser;  
  
  const IdDictMgr& idd = parser.parse (argv[1]);  
 
  const IdDictMgr::dictionary_map& dm = idd.get_dictionary_map (); 
  IdDictMgr::dictionary_map::const_iterator it;  
 
  int n = 0; 
 
  for (it = dm.begin (); it != dm.end (); ++it, ++n) 
    { 
      const IdDictDictionary& dictionary = *((*it).second); 
 
      std::cout << "---- " << n << " ----------------------------" << std::endl; 
      std::cout << "Dictionary " << dictionary.m_name << std::endl; 
 
      IdDictDictionary::regions_const_it rit; 
      for (rit = dictionary.m_regions.begin (); rit != dictionary.m_regions.end (); ++rit) 
        { 
          const IdDictRegion& region = *(*rit);
 
          std::cout << "region #" << region.m_index << std::endl; 
 
          std::vector <IdDictFieldImplementation>::const_iterator fit; 
           
          size_t width = 0; 
 
          for (fit = region.m_implementation.begin ();  
               fit != region.m_implementation.end (); 
               ++fit) 
            { 
              const IdDictFieldImplementation& impl = *fit; 
 
              size_t w = impl.range()->m_field->m_name.size (); 
 
              if (w > width) width = w; 
            } 
 
          int bits = 0; 
 
          for (fit = region.m_implementation.begin ();  
               fit != region.m_implementation.end (); 
               ++fit) 
            { 
              const IdDictFieldImplementation& impl = *fit; 
 
              size_t w = impl.range()->m_field->m_name.size (); 
 
              std::cout << "  implement field #" << impl.range()->m_field->m_index <<  
                  " " << impl.range()->m_field->m_name; 
 
              tab (width - w); 
 
              std::cout << " -> " << (std::string) impl.field() <<  
                  "/" << (std::string) impl.ored_field() <<  
                  " (" << impl.bits() << " bits)" <<  
                  std::endl; 
 
              bits += impl.bits(); 
            } 
 
          Range range = region.build_range (); 
 
          std::cout << " -> " << (std::string) range <<  
              " (cardinality=" << range.cardinality () << ")" << 
              " (" << bits << " bits)" << std::endl; 
        } 
    } 
 
  { 
    const IdDictDictionary* dictionary = idd.find_dictionary ("InnerDetector"); 
 
    if (dictionary != 0) 
      { 
        ExpandedIdentifier id ("1/0/0/21/6/319/191"); 
 
//  	    IdDictDictionary::bits32 b = dictionary->pack32 (id, 0, 6); 
	Identifier packedB((Identifier::value_type)0);
	dictionary->pack32 (id, 0, 6, packedB); 
 
        std::cout << "b=[" << packedB << "]" << std::endl; 
 
	ExpandedIdentifier id2;
	dictionary->unpack (packedB, ExpandedIdentifier (), 6, id2); 
 
        std::cout << "unpack->[" << (std::string) id2 << "]" << std::endl; 
      } 
  } 
 
  return 0;  
}  
  
 
 
 
 
 
