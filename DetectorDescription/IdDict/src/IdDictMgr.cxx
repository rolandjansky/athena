/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/DetectorDescription/IdDict/src/IdDictMgr.cxx,v 1.43 2008-12-09 09:49:43 dquarrie Exp $  
  
#include <fstream>  
#include <iostream>  
 
#include <stdio.h>
#include <cstdlib>  
  
#include "IdDict/IdDictDefs.h"  
 
class Debugger 
{ 
public: 
  static bool debug () 
  { 
    static bool first = true; 
    static bool debug_state = false; 
 
    if (first) 
      { 
	first = false; 
	if (::getenv ("IDDEBUG") == 0) 
	  { 
	    debug_state = false; 
	  } 
	else 
	  { 
	    debug_state = true; 
	  } 
      } 
 
    return (debug_state); 
  } 
 
  static void up () 
  { 
    level (1); 
  } 
 
  static void down () 
  { 
    level (-1); 
  } 
 
  static void tab (int nspaces = 0) 
  { 
    int n = (nspaces) ? nspaces : level (); 
 
    for (int i = 0; i < n; ++i) 
      { 
	std::cout << " "; 
      } 
  } 
 
private: 
 
  static int level (int d = 0) 
  { 
    static int n = 0; 
 
    n += d; 
 
    return (n); 
  } 
 
}; 

static bool isNumber(const std::string& str) 
{
    bool result = true;
    for (unsigned int i=0; i<str.size(); ++i) {
	if(!isdigit(str[i])) return (false);
    }
    if (0 == str.size()) return (false);
    
    return (result);
}

    

/** 
 * 
 */ 
class TypeIdBuilder  
{  
public:  
  static int build_id ();  
 
};  
  
template <class T>  
class TypeId  
{  
public:  
  operator int ()  
      {  
        static int i = TypeIdBuilder::build_id ();  
        return (i);  
      }  
};  
  
int TypeIdBuilder::build_id ()  
{  
  static int i = 0;  
  i++;  
  return (i);  
}  
 
  
/** 
 * 
 */ 
IdDictMgr::IdDictMgr()
    : 
    m_resolved_references(false),
    m_generated_implementation(false),
    m_do_checks(false),
    m_do_neighbours(true)
{
}

IdDictMgr::~IdDictMgr()
{
    clear();
}

const std::string&   	IdDictMgr::tag() const
{
    return m_tag;
}

const std::string&   	IdDictMgr::DTD_version() const
{
    return m_DTD_version;
}

bool IdDictMgr::do_checks               () const
{
    return m_do_checks;
}

void		
IdDictMgr::set_do_checks	(bool do_checks)
{
    m_do_checks = do_checks;
    for (const auto& p : m_dictionaries) {
        IdDictDictionary* d = p.second; 
	d->set_do_checks(do_checks);
    } 
}

bool IdDictMgr::do_neighbours               () const
{
    return m_do_neighbours;
}

void		
IdDictMgr::set_do_neighbours	(bool do_neighbours)
{
    m_do_neighbours = do_neighbours;
    for (const auto& p : m_dictionaries) {
        IdDictDictionary* d = p.second; 
	d->set_do_neighbours(do_neighbours);
    } 
}

const std::string&
IdDictMgr::find_metadata           (const std::string& name) const
{
    metadata_map::const_iterator it = m_metadata.find(name);
    static std::string empty;
    if (it != m_metadata.end()) return (it->second);
    else return empty;
}

void       
IdDictMgr::add_metadata            (const std::string& name, const std::string& value)
{
    if(!m_metadata.insert(metadata_map::value_type(name, value)).second) {
        std::cout << "IdDictMgr::add_metadata> unable to add name/value " << name << "/" << value << std::endl; 
    }
}

void
IdDictMgr::set_DTD_version(const std::string& DTD_version)
{
    m_DTD_version = DTD_version;
}


const IdDictMgr::dictionary_map& IdDictMgr::get_dictionary_map () const 
{ 
  return (m_dictionaries); 
} 
 
IdDictDictionary* IdDictMgr::find_dictionary (const std::string& name) const  
{  
  dictionary_map::const_iterator it;  
  
  it = m_dictionaries.find (name);  
  
  if (it == m_dictionaries.end ()) return (0);  
  
  return ((*it).second);  
}  
  
void IdDictMgr::add_dictionary (IdDictDictionary* dictionary)  
{  
  if (dictionary == 0) return;  
  
  std::string& name = dictionary->m_name;  

  // Delete entry if already there
  dictionary_map::iterator it = m_dictionaries.find(name);
  if (it != m_dictionaries.end()) delete (*it).second;
 
  m_dictionaries[name] = dictionary;  
 
  if (Debugger::debug ()) 
    { 
      dictionary_map::iterator it; 
 
      for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it) 
	{ 
	  std::string s = (*it).first; 
	  IdDictDictionary* d = (*it).second; 
 
	  std::cout << "IdDictMgr::add_dictionary> d[" << s << "]=" << d << std::endl; 
	} 
    } 
}  

void IdDictMgr::add_subdictionary_name (const std::string& name)
{
    m_subdictionary_names.insert(name);
}

  
void IdDictMgr::resolve_references ()  
{  
  dictionary_map::iterator it;  
  
  for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it)  
    {  
      // From mgr, only resolve refs for top-level dictionaries
      IdDictDictionary* dictionary = (*it).second;  
      if (m_subdictionary_names.find(dictionary->m_name) != m_subdictionary_names.end()) continue;
      dictionary->resolve_references (*this);  
    }  
} 
 
void IdDictMgr::generate_implementation (std::string tag)  
{  

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictMgr::generate_implementation>" << std::endl; 
    } 

  // Must reset the implementation for multiple passes, this resets
  // the generated flags
  if (m_generated_implementation && tag != m_tag) reset_implementation();
  
  if (!m_generated_implementation) {

      m_tag = tag;
      dictionary_map::iterator it;  
      for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it) {  
	  // From mgr, only generate impl for top-level dictionaries
	  IdDictDictionary* dictionary = (*it).second;  
	  if (m_subdictionary_names.find(dictionary->m_name) != m_subdictionary_names.end()) continue;
	  dictionary->generate_implementation (*this, tag);  
      } 
      m_generated_implementation = true;
    }  
} 
 
void IdDictMgr::reset_implementation ()
{

    std::cout << "IdDictMgr::reset_implementation" << std::endl;
    

  if (m_generated_implementation) {
  
      dictionary_map::iterator it;  
      for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it) {  
	  // From mgr, only generate impl for top-level dictionaries
	  IdDictDictionary* dictionary = (*it).second;  
	  if (m_subdictionary_names.find(dictionary->m_name) != m_subdictionary_names.end()) continue;
	  dictionary->reset_implementation ();  
      } 
      m_generated_implementation = false;
    }  
}


bool IdDictMgr::verify () const 
{ 
  dictionary_map::const_iterator it;  
  
  for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it)  
    {  
      const IdDictDictionary* dictionary = (*it).second;  
      if (!dictionary->verify ()) return (false);  
    } 
 
  return (true); 
} 
 
void IdDictMgr::clear () 
{ 
  dictionary_map::iterator it;  
  
  for (it = m_dictionaries.begin (); it != m_dictionaries.end (); ++it)  
    {  
      IdDictDictionary* dictionary = (*it).second;  
      dictionary->clear (); 
      delete dictionary; 
    } 
 
  m_dictionaries.clear (); 
  m_resolved_references      = false;
  m_generated_implementation = false;

} 

/** 
 * 
 */ 
IdDictDictionary::IdDictDictionary () 
    : 
    m_parent_dict(0),
    //m_resolved_references(false),
    m_generated_implementation(false),
    m_do_checks(false),
    m_do_neighbours(true)
{} 
 
IdDictDictionary::~IdDictDictionary () 
{ 
} 
 
IdDictField* IdDictDictionary::find_field (const std::string& name) const 
{ 
  std::map <std::string, IdDictField*>::const_iterator it;  
  
  it = m_fields.find (name);  
  
  if (it == m_fields.end ()) {
      // If parent exists, look for field there
      if(m_parent_dict) {
	  it = m_parent_dict->m_fields.find (name);  
	  if (it == m_parent_dict->m_fields.end ()) {
	      return (0);  
	  }
      }
      else {
	  return (0);  
      }
  }
  
  return ((*it).second);  
} 

IdDictLabel* IdDictDictionary::find_label (const std::string& field, const std::string& label) const
{
    IdDictField* idField = find_field(field);
    if (!idField) return (0);
    return (idField->find_label(label));
}

int IdDictDictionary::get_label_value (const std::string& field, const std::string& label, int& value) const
{
    IdDictLabel* idLabel = find_label(field, label);
    if (!idLabel || !idLabel->m_valued) return (1);
    value = idLabel->m_value;
    return (0);
}
 
void IdDictDictionary::add_field (IdDictField* field) 
{ 
  if (field == 0) return;  
  
  std::string& name = field->m_name;  
  
  m_fields[name] = field;  
} 
  
IdDictSubRegion* IdDictDictionary::find_subregion (const std::string& name) const 
{ 
  std::map <std::string, IdDictSubRegion*>::const_iterator it;  
  
  it = m_subregions.find (name);  
  
  if (it == m_subregions.end ()) return (0);  
  
  return ((*it).second);  
} 

IdDictRegion* IdDictDictionary::find_region (const std::string& region_name) const
{
  return find_region (region_name, "");
}

IdDictRegion* IdDictDictionary::find_region (const std::string& region_name, const std::string& group_name) const
{ 
  for (size_t i = 0; i < m_regions.size (); ++i) 
    { 
      IdDictRegion* region = m_regions[i]; 
      if ( (group_name!= "") && (region->group_name() != group_name) ) continue;
      if ((region != 0) && (region_name == region->m_name)) return (region); 
    } 
 
  return (0); 
} 

IdDictGroup* IdDictDictionary::find_group (const std::string& group_name) const
{ 
  for (size_t i = 0; i < m_groups.size (); ++i) 
    { 
      IdDictGroup* group = m_groups[i]; 
      if ((group != 0) && (group->name() == group_name)) return (group); 
    } 
 
  return (0); 
} 

void IdDictDictionary::add_subregion (IdDictSubRegion* subregion) 
{ 
  if (subregion == 0) return;  
  
  std::string& name = subregion->m_name;  
  
  m_subregions[name] = subregion;  
} 
 
void IdDictDictionary::add_subdictionary_name (const std::string& name)
{
    m_subdictionary_names.push_back(name);
}



void IdDictDictionary::add_dictentry (IdDictDictEntry* region)
{ 
    // Add region to corresponding group
    IdDictGroup* group = find_group(region->group_name());
    if (0 == group) {
	group = new IdDictGroup(region->group_name());
	m_groups.push_back(group);
    }
    group->add_dictentry (region);
} 
  
void IdDictDictionary::resolve_references (const IdDictMgr& idd) 
{ 
  {  
    std::map<std::string, IdDictField*>::iterator it;  
  
    for (it = m_fields.begin (); it != m_fields.end (); ++it)  
      {  
        IdDictField* field = (*it).second;  
        field->resolve_references (idd);  
      }  
  }  
  {  
    std::map<std::string, IdDictSubRegion*>::iterator it;  
  
    for (it = m_subregions.begin (); it != m_subregions.end (); ++it)  
      {  
        IdDictSubRegion* subregion = (*it).second;  
        subregion->resolve_references (idd, *this);  
      }  
  }  
  {  
    size_t index = 0; 
 
    IdDictDictionary::groups_it it; 
    for (it = m_groups.begin (); it != m_groups.end (); ++it)  
      {  
        (*it)->resolve_references (idd, *this, index);  
      }  
  }  
} 
 
typedef std::vector <IdDictRegion*> RV; 
 
static void compute_bits (const RV& regions, size_t level, const std::string& group)
{ 
 
    //
    // Compute the OR of all fields at <level>, for the
    // subset of overlapping regions
    //
 
    Range::field ored_field; 
    unsigned int k;
 
//    std::cout << "compute bits - level " << level << std::endl;
  

    for (k = 0; k < regions.size (); ++k) { 
        IdDictRegion* region = regions[k];
       
        if (region->m_implementation.size () <= level) continue; 

        const IdDictFieldImplementation& f = region->m_implementation[level]; 
      
        const Range::field fff = f.field(); 
       
        if (k == 0) ored_field = fff; 
        else ored_field |= fff; 

        // std::cout << "Oring bits for region/field " << region->m_name 
        //           << " " << f.range()->m_field_name << std::endl;
        
 
//        std::cout << " " << (std::string) fff ; 
    } 
 
//    std::cout << " -> " << (std::string) ored_field ; 
 
//  int bits = ored_field.get_bits (); 
 
//    std::cout << " " << bits << "(bits)" << std::endl; 


    //
    // Now that we have the ored width of all first fields of
    // all regions, we opgrade the bit width in the corresponding
    // field implementations.
    //

   
    for (k = 0; k < regions.size (); ++k) { 
        IdDictRegion* region = regions[k];
       
        if (region->m_implementation.size () <= level) continue; 

//      std::cout << "Updating level " << level << " ored field " << (std::string)(ored_field) << " group, index " << region->m_group << " " << region->m_index << std::endl; 
//      std::cout << "Updating level " << level << " of range " << (std::string)(region->build_range()) << " group, index " << region->m_group << " " << region->m_index << std::endl; 

        // Don't set ored bits for regions outside of group - ok to
        // calculate with them only
        if (group != region->m_group) continue;  

        IdDictFieldImplementation& f = region->m_implementation[level]; 
        f.set_ored_field(ored_field); 
    }    
}

static void get_bits (const RV& regions, size_t level, const std::string& group) 
{ 

    // This function is recusively called for each level to get the
    // number of bits. By "definition" we require regions in the same
    // group to overlap to have a uniform bit-allocation for a group.
    //
    // For each call, the first region within the specified "group"
    // which has a (non-empty) field value at the current level is
    // used as reference. According to this reference, the regions are
    // divided into two sets: a set overlapping with the reference and
    // a non-overlapping set. For the overlapping set, get_bits is
    // called again, but for the next level. When control comes back,
    // then a reference is chosen from the non-overlapping set and the
    // procedure continues.
    //
    // The calculation of bits needed for a particular level is done
    // in compute_bits before the overlapping is check for this level.
    //


//      Debugger::tab (level); 
//      std::cout << " get_bits : level " << level;
//      std::cout << " regions size " << regions.size() << std::endl;

    //  Invalid call - return
    if (regions.size () == 0) return;

    if (regions.size () == 1) { 
        /* 
           There is only one region (ie one range)
           therefore the bitsets are unambiguous

           The bit width of each field implementation is
           simply the bit width of the corresponding field.
        */ 
 
        IdDictRegion* region = regions[0];
      
        for (size_t k = level; k < region->m_implementation.size (); ++k) { 
            IdDictFieldImplementation& f = region->m_implementation[k]; 
          
            f.set_ored_field(f.field()); 

//  	  std::cout << " field " << (std::string) f.ored_field() ; 
 
//  	  std::cout << " " << f.bits() << "(bits)" << std::endl; 

        } 

//        std::cout << "Updating levels from " << level << " of range " << (std::string)(region->build_range()) << std::endl; 

 
        return; 
    } 
 
    size_t k; 
 
    // Copy the regions provided in the arguments to a local clone. 
 
    RV mr = regions; 
 
    //    Debugger::tab (level); 
    //    std::cout << " or-ing fields : level " << level;
    //    std::cout << " name " << mr[0]->m_implementation[level].m_range->m_field_name ; 


//  std::cout << "compute bits: nregions, level " << mr.size() << " " << level << std::endl;

    // Compute the number of bits needed at this level
    compute_bits (mr, level, group);
  

    /** 
     *   Split mr into two RV : 
     * 
     *    overlapping contains all Regions that overlap with their field  at <level>
     *    temp        contains all other Regions 
     */ 
 
    RV overlapping; 
    RV orig; 
    RV temp; 
    overlapping.reserve(mr.size()); 
    orig.reserve(mr.size());
    temp.reserve(mr.size());
  
    for (;;) { 
        if (mr.size () == 0)  { 
            //Debugger::tab (level); std::cout << "empty RV" << std::endl; 
            break; 
        } 
 
        overlapping.clear (); 
        temp.clear (); 
 
        // Find the first non empty Region within the specified group
 
        IdDictRegion* reference_region = 0; 
 
//        std::cout << "k,mr.size() 1 " << k << " " << mr.size() << std::endl;

        for (k = 0; k < mr.size (); ++k) { 
            reference_region = mr[k];
            if (reference_region->m_implementation.size () > level) { 


//                std::cout << "k in level " << k << " " << level << std::endl;

		/*
		  Debugger::tab (level); std::cout << "Install the non empty Region : " <<  
                  reference_region->m_index; 
                   
                  const IdDictFieldImplementation& f = reference_region->m_implementation[level]; 
                   
                  const Range::field& fff = f.field(); 
                   
                  std::cout << " field " << (std::string) fff << std::endl; 
		*/
 
	      	if (group == reference_region->m_group) {
		    overlapping.push_back (reference_region); 
		    break; 
		}
		else {
		    temp.push_back(reference_region); 
		}
            } 
        } 

        // Should be redundant with the mr.size() test above, but put here
        // to try to avoid coverity warnings that reference_region may be null.
        if (reference_region == 0) break;
  
        if (overlapping.size () == 0) { 
//              Debugger::tab (level); std::cout << "RV only contains empty Regions" << std::endl; 
            break; 
        } 
 
        // Now really split mr into the two subsets 
 
        ++k; 
        const IdDictFieldImplementation& f1 = reference_region->m_implementation[level]; 

        Range::field ored_field = f1.field(); 
      
        // copy into original test sample, some may have already been
        // added to temp above.

//        std::cout << "k,mr.size() 2 " << k << " " << mr.size() << std::endl;

        for (; k < mr.size (); ++k) { 
            IdDictRegion* region = mr[k];
            temp.push_back (region); 

//            std::cout << "k, save temp " << k << std::endl;
            

	}


        bool found_overlap  = false;

        // Compare the reference field at this level to those in the
        // remaining regions and check for overlap.
        //
        // The comparison must be done repeatedly because the overlap
        // field gets larger and may overlap with previously checked
        // field of a region.
        //
        // By "definition" we require regions in the same group to
        // overlap to have a uniform bit-allocation for a group.
        //
        // For regions outside the current group, the overlap is
        // allowed for the top level fields with the same names. As
        // soon as one finds a field with a different name which is in
        // a region outside the group, this and subsequent fields are
        // ignored.
        //
        do {

            // copy temp into orig, set overlap to false for this loop over regions
            orig.clear();
            orig = temp;
            temp.clear();
            found_overlap = false;

            for (size_t i = 0; i < orig.size (); ++i) { 
                IdDictRegion* region = orig[i];
 
                if (region->m_implementation.size () <= level) continue; 
 
//  	  Debugger::tab (level); std::cout << "Check a non empty Region : " << region->m_index; 
 
                bool overlap = false; 

                const IdDictFieldImplementation& f = region->m_implementation[level]; 
 
                const Range::field& fff = f.field(); 
 
//  	      std::cout << " field " << (std::string) fff2; 

//                std::cout << "f1/f names " << f1.range()->m_field_name << " " << f.range()->m_field_name; // << std::endl;
                
                // Now expand bits by or'ing them with other regions
                // at this level, requiring the name to be the same.
                if (f1.range()->m_field_name == f.range()->m_field_name) 
                    overlap = ored_field.overlaps_with (fff); 

//                std::cout << " has overlap " << overlap << std::endl;
                

//    	    std::cout << " field ored, other, overlap " << (std::string) ored_field
//    		      << " " << (std::string)fff << " " << overlap << " group " 
//  		      << region->m_group << " " << region->m_index << std::endl; 

                // Check for either an overlap or force overlap for
                // regions in the same group
                if (overlap || (region->m_group == group)) { 
                    overlapping.push_back (region); 

                    // std::cout << " field ored, other, overlap " << (std::string) ored_field
                    //           << " " << (std::string)fff;
                    // std::cout << " group " 
                    //           << region->m_group << " " << region->m_index << std::endl; 

                    ored_field |= fff; 
                    found_overlap = true;
                } 
                else { 
                    temp.push_back (region); 
                } 
            }
        } while(found_overlap);
      
        // Check overlapping collection - if there are only regions from
        // the same group, we can safely throw away "temp" the other
        // collection
        bool all_within_group = true;
        bool none_within_group = true;
        for (size_t i = 0; i < overlapping.size (); ++i) { 
            IdDictRegion* region = overlapping[i];
            if (group == region->m_group) {
                none_within_group = false;
            }
            else {
                all_within_group = false;
            }
        }
        // Remove temp is no longer needed
        if (all_within_group) temp.clear();


//        Debugger::tab (level); std::cout << "now we have " << overlapping.size () <<  
//                         " vs " <<  
//                         temp.size () << " regions" << std::endl; 

        // Recurse on the remaining fields of the overlapping regions 
        //
        // Continue only if there are regions still within the current
        // group
        if(!none_within_group) {
            get_bits (overlapping, level + 1, group); 
        }
      
        // Continue for the other not-yet-overlapping regions. 

        mr = temp; 
    } 
} 
 
void IdDictDictionary::generate_implementation (const IdDictMgr& idd,
						std::string tag) 
{ 

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictDictionary::generate_implementation>" << std::endl; 
    } 
  
  if (!m_generated_implementation) {
      
      // Propagate to each region and copy their generation into the
      // dict's vector.
      IdDictDictionary::groups_it it; 
      for (it = m_groups.begin (); it != m_groups.end (); ++it) { 
	  (*it)->generate_implementation (idd, *this, tag);  
	  // Get regions from group and save in m_regions
	  const regions_type& regions = (*it)->regions();
	  IdDictDictionary::regions_const_it it1; 
	  for (it1 = regions.begin(); it1 != regions.end(); ++it1) {
	      m_regions.push_back(*it1);
	  }
      }  

      // Loop again over groups and set the bit-packing - starting at
      // level 0
      for (it = m_groups.begin (); it != m_groups.end (); ++it) { 
	  // Copy to temporary vector all regions in the group. And
	  // look for regions in local m_regions vector for any
	  // regions "dummy", which come from reference
	  // dictionaries.

//          std::cout << "Group: " << (*it)->name() << std::endl;
	  
//  	  IdDictDictionary::regions_type regions((*it)->regions());
//  	  IdDictDictionary::regions_const_it it1; 
//  	  for (it1 = m_regions.begin (); it1 != m_regions.end (); ++it1) { 
//  	      if ("dummy" == (*it1)->m_name) {
//  		  regions.push_back(*it1);
//  		  std::cout << "adding " << (*it1)->m_index << std::endl;
//  	      }
//  	  }

	  // Skip special group
	  if ("dummy" == (*it)->name()) continue;
	  
	  get_bits (m_regions, 0, (*it)->name()); 
//	  get_bits (regions, 0, (*it)->name()); 
      }

      // Set integral over the number of bits
      integrate_bits ();

      // Set neighbours for regions
      IdDictDictionary::regions_it itr; 
      for (itr = m_regions.begin (); itr != m_regions.end (); ++itr) { 
	  (*itr)->find_neighbours(*this);  
      }
      
      m_generated_implementation = true;
  } 
} 

void IdDictDictionary::reset_implementation () 
{ 
  if (m_generated_implementation) {
      m_regions.clear();
      IdDictDictionary::groups_it it; 
      for (it = m_groups.begin (); it != m_groups.end (); ++it) { 
	  (*it)->reset_implementation();  
      }  
      m_generated_implementation = false;
  } 
} 

int
IdDictDictionary::find_region(const ExpandedIdentifier& id, size_type& index) const
{

    // Find first region that matches id

    IdDictDictionary::regions_const_it it; 

    size_type i = 0;
    for (it = m_regions.begin (); it != m_regions.end (); ++it, ++i) { 
	const IdDictRegion& region = *(*it);

	Range range = region.build_range();  

	if (range.match(id) && range.fields() >= id.fields()) {
	    index = i;  
	    return (0);
	}
    }  

    return (1);
}


IdDictRegion* IdDictDictionary::find_region(const ExpandedIdentifier& id) const
{
  return find_region (id, "");
}

IdDictRegion* IdDictDictionary::find_region(const ExpandedIdentifier& id, const std::string& group_name) const
{
    // Find first region that matches id

  IdDictRegion* pRegion = 0; 
    IdDictDictionary::regions_const_it it; 

    for (it = m_regions.begin (); it != m_regions.end (); ++it) { 
	IdDictRegion& region = *(*it);
        if ( (group_name!= "") && (region.group_name() != group_name) ) continue;

	Range range = region.build_range();  

	if (range.match(id) && range.fields() >= id.fields()) {
	    pRegion = &region;  
	}
    }  

    return (pRegion);
}

void 
IdDictDictionary::integrate_bits ()
{
    // For each region, loop over its levels and set the bit offset
    // for each FieldImplementation

    for (IdDictRegion* region : m_regions) {
	size_t bits_offset = 0; 

        for (IdDictFieldImplementation& impl : region->m_implementation) {
	    impl.optimize(); // optimize for decoding
	    impl.set_bits_offset(bits_offset);
	    bits_offset += impl.bits();

	    // Set whether or not to decode index
	    Range::field field = impl.ored_field();
	    if (Range::field::both_bounded != field.get_mode() ||
		0 != field.get_minimum())
            {
		impl.set_decode_index(true); 
	    }
	}
    }
}

 
/** 
 * 
 *   Here, we verify global constraints : 
 *   (this must only be applied after the resolve_references and  
 *    generate_implementation operations) 
 * 
 *   a) There should be no overlap between any region duet. 
 * 
 *   b) In a dictionary, a given field (ie. with a given name) must always be 
 *     referenced at the same depth across all regions and subregions. 
 * 
 *   c) When a region specifies a reference to a sub-dictionary :  
 *      - the reference must be the last entry in the region 
 *      - the first region entries of all regions should not overlap (regardless 
 *        of the definition of the sub-dictionary). IE the first fields in  
 *        these regions must be sufficient to unambiguously select the  
 *        sub-dictionary. 
 * 
 *   d) Within a dictionary, a given field must always be described either through 
 *     an enumeration, or through its bounds. However, the various usages of a  
 *     given field may specify different tag set (for enumerations) or bounds (for 
 *     a bounded field). 
 * 
 */ 
bool IdDictDictionary::verify () const 
{ 
    // check #1 

  MultiRange mr = build_multirange (); 
  if (mr.has_overlap ()) return (false); 

    // check #2 
    // check #3 
    // check #4 
 
  return (true); 
} 
 
/** 
 * 
 *   Sort:
 *   
 *      Prerequisite: first verify the dictionary - no overlaps
 *     
 *      Then loop over regions and sort according to their first
 *      identifier
 * 
 */
void IdDictDictionary::sort () 
{ 
    // verify
    if (verify()) {

	std::map< ExpandedIdentifier, IdDictDictEntry* > regions;
      
	IdDictDictionary::groups_it it; 

	for (it = m_groups.begin (); it != m_groups.end (); ++it) { 

	    (*it)->sort();
	}
    }
    else {
	std::cout << "IdDictDictionary::sort - WARNING verify is FALSE - cannot sort "
		  << std::endl; 
    }
} 
 
void IdDictDictionary::clear () 
{ 
  {  
    std::map<std::string, IdDictSubRegion*>::iterator it;  
    
    for (it = m_subregions.begin (); it != m_subregions.end (); ++it)  
      {  
        IdDictSubRegion* subregion = (*it).second;  
        subregion->clear ();  
        delete subregion; 
      }  
 
    m_subregions.clear (); 
  }  
 
  {  
    std::map<std::string, IdDictField*>::iterator it;  
  
    for (it = m_fields.begin (); it != m_fields.end (); ++it)  
      {  
        IdDictField* field = (*it).second;  
        field->clear ();  
        delete field; 
      }  
 
    m_fields.clear (); 
  }  
 
  {  
    IdDictDictionary::groups_it it; 
  
    for (it = m_groups.begin (); it != m_groups.end (); ++it) {

	IdDictGroup* group = *it;
        group->clear ();  
        delete group; 
      }  
 
    m_groups.clear (); 
  }  
} 
 
MultiRange IdDictDictionary::build_multirange () const 
{ 
  MultiRange result; 
 
  IdDictDictionary::groups_const_it it; 

  for (it = m_groups.begin (); it != m_groups.end (); ++it) 
    { 
      const IdDictGroup& group = *(*it);

      MultiRange group_mr = group.build_multirange();

      for (unsigned int i = 0; i < group_mr.size(); ++i) {
	  const Range& range = group_mr[i];
	  result.add (range);
      }
    } 

//    IdDictDictionary::regions_const_it it; 

//    for (it = m_regions.begin (); it != m_regions.end (); ++it) 
//      { 
//        const IdDictRegion& region = *(*it);

//        // skip regions created from parents
//        if("dummy" == region.m_name) continue;

//        // skip empty regions - may arise from alternate_regions
//        // where a tag selects an empty region
//        if(region.m_is_empty) continue;

//        result.add (region.build_range ()); 

//      } 
 
  return (result); 
} 

MultiRange IdDictDictionary::build_multirange (const ExpandedIdentifier& region_id,
					       const Range& prefix,
					       const std::string& last_field) const
{
    MultiRange result; 

    IdDictDictionary::regions_const_it it; 
    if ("" == last_field) {
	// Take all fields
	for (it = m_regions.begin (); it != m_regions.end (); ++it) { 

	    const IdDictRegion& region = *(*it);

	    // skip regions created from parents
	    if("dummy" == region.m_name) continue;

	    // skip empty regions - may arise from alternate_regions
	    // where a tag selects an empty region
	    if(region.m_is_empty) continue;

	    Range range(region.build_range ()); 
	    // Check region selection
	    if (range.match(region_id))result.add (std::move(range));
	} 
    }
    else {
	// Not all fields required
	for (it = m_regions.begin (); it != m_regions.end (); ++it) { 
	    const IdDictRegion& region = *(*it);
	    
	    // skip regions created from parents
	    if("dummy" == region.m_name) continue;

	    // skip empty regions - may arise from alternate_regions
	    // where a tag selects an empty region
	    if(region.m_is_empty) continue;

	    Range range(region.build_range ()); 
	    // Check region selection
	    if (range.match(region_id)) {
		// Build new range up to last_field and add it to result -
		// remove duplicate ranges with addRangeToMR
		Range new_range(prefix); // Prepend with prefix

		std::vector <IdDictFieldImplementation>::const_iterator fit; 
		for (fit = region.m_implementation.begin ();  
		     fit != region.m_implementation.end (); 
		     ++fit) { 
		    const IdDictFieldImplementation& impl = *fit; 
 
//  		    new_range.add(impl.m_field);
		    new_range.add(impl.range()->build_range());

		    if (last_field == impl.range()->m_field->m_name) {
			break;
		    }
		} 
		result.add(std::move(new_range));
	    }
	} 
    }
  
    return (result); 
}


MultiRange IdDictDictionary::build_multirange (const ExpandedIdentifier& region_id,
					       const std::string& group_name,
					       const Range& prefix,
					       const std::string& last_field) const 
{
    MultiRange result; 

    IdDictDictionary::regions_const_it it; 
    if ("" == last_field) {
	// Take all fields
	for (it = m_regions.begin (); it != m_regions.end (); ++it) { 

	    const IdDictRegion& region = *(*it);

	    // skip regions created from parents
	    if("dummy" == region.m_name) continue;

	    // skip empty regions - may arise from alternate_regions
	    // where a tag selects an empty region
	    if(region.m_is_empty) continue;

	    Range range(region.build_range ()); 
	    // Check region selection
	    if (range.match(region_id) && region.group_name() == group_name)result.add (std::move(range));
	} 
    }
    else {
	// Not all fields required
	for (it = m_regions.begin (); it != m_regions.end (); ++it) { 
	    const IdDictRegion& region = *(*it);
	    
	    // skip regions created from parents
	    if("dummy" == region.m_name) continue;

	    // skip empty regions - may arise from alternate_regions
	    // where a tag selects an empty region
	    if(region.m_is_empty) continue;

	    Range range(region.build_range ()); 
	    // Check region selection
	    if (range.match(region_id) && region.group_name() == group_name) {
		// Build new range up to last_field and add it to result -
		// remove duplicate ranges with addRangeToMR
		Range new_range(prefix); // Prepend with prefix

		std::vector <IdDictFieldImplementation>::const_iterator fit; 
		for (fit = region.m_implementation.begin ();  
		     fit != region.m_implementation.end (); 
		     ++fit) { 
		    const IdDictFieldImplementation& impl = *fit; 
 
//  		    new_range.add(impl.m_field);
		    new_range.add(impl.range()->build_range());

		    if (last_field == impl.range()->m_field->m_name) {
			break;
		    }
		} 
		result.add(std::move(new_range));
	    }
	} 
    }
  
    return (result); 
}


  
/** 
 *   Pack to 32bits the subset of id between (inclusive) index1
 *   and index2 - this is generic, i.e. not the most efficient
 *
 *   Assumptions: 
 *     - packedId is initialized to 0
 *     - the expanded id begins at index = 0
 *     - index1,2 correspond to the level/field number, this is
 *       thus the same as the index into the expanded id due to the
 *       previous assumption.
 *
 */

int 
IdDictDictionary::pack32 (const ExpandedIdentifier& id, 
			  size_t index1, 
			  size_t index2,
			  Identifier& packedId) const
{ 
    packedId  = 0;
 
//    std::cout << "IdDictDictionary::pack32: index1,2, fields " 
//  	    << index1 << " " << index2 << " " << id.fields() 
//  	    << " " << (std::string)id << std::endl; 
	  
    // Preconditions... 
 
    if (index2 < index1) { 
        // bad parameters. 
        return (1); 
    } 
 
    if (index1 >= id.fields ()) { 
        // nothing very useful !! 
        return (1); 
    } 
 
    if (index2 >= id.fields ()) { 
        // bad parameter... 
        return (1); 
    } 
 
    /** 
     *   First we need to check whether the specified identifier 
     *   really matches the Dictionary specifications. 
     * 
     *    However, we stop the search in the set of regions at the first 
     *   matching region. 
     *    Should we request that only one region matches ???? 
     */ 
 
    for (size_t k = 0; k < m_regions.size (); ++k) { 
        bool selected = true; 
 
        const IdDictRegion& region = *m_regions[k];

        // Must skip empty regions - can arise when a tag selects an
        // empty region
        if (region.m_is_empty) continue;
 
        for (size_t i = 0; i < region.m_implementation.size (); ++i) { 
            if (i >= id.fields ()) break; 
 
            const IdDictFieldImplementation& impl = region.m_implementation[i]; 
 
            if (!impl.field().match (id[i]))  { 
//                std::cout << "Region #" << region.m_index << 
//  		  " field " << impl.range()->m_field_name << 
//  		  " #" << i << " (" << (std::string) impl.field() << ") does not match " << 
//  		  id[i] << std::endl; 
 
                selected = false; 
                break; 
            } 
        } 
 
        if (selected) { 
            size_t position = Identifier::NBITS;
 
//      	  std::cout << "Region #" << region.m_index << " selected" << std::endl; 
 
            // We have the proper region. 
            for (size_t i = index1; i <= index2; ++i) { 
                const IdDictFieldImplementation& impl = region.m_implementation[i]; 
 
                Identifier::value_type index = impl.ored_field().get_value_index (id[i]);
 
                if (0 == position && impl.bits() > 0) {
                    return(1);
                }
	      
                position -= impl.bits(); 
                packedId |= (index << position); 
 
//                std::cout << "  pack " << id[i] << " using field " << 
//                    impl.range()->m_field_name << 
//  		  " #" << i << " (" << (std::string) impl.ored_field() << ")" << 
//  		  " -> index=" << index << 
//  		  " @bit" << position << 
//  		  std::endl; 
 
            } 
            break; 
        } 
    } 

//    std::cout << "  packed " << hex << packedId << dec << std::endl; 
 
    return (0); 
} 
 


int 
IdDictDictionary::pack32 (const int* fields,
			  size_t index1, 
			  size_t index2,
			  size_t region_index,
			  Identifier& packedId,
			  size_t first_field_index) const
{ 
 
//    std::cout << "IdDictDictionary::pack32: index1,2, fields " 
//  	    << index1 << " " << index2 << " " << id.fields() 
//  	    << " " << (std::string)id << std::endl; 
	  
    // Preconditions... 

    if (m_do_checks) {
	
	if (index2 < index1) { 
	    // bad parameters. 
	    std::cout << "IdDictDictionary::pack32 - index2 < index1 - 1,2"
		      << index1 << " " << index2 << std::endl;
	    return (1); 
	} 
 
	if (region_index >= m_regions.size()) {
	    // bad parameters. 
	    std::cout << "IdDictDictionary::pack32 - region index incorrect - index,size"
		      << region_index << " " << m_regions.size() << std::endl;
	    return (1); 
	}
    }
    

    // Get the region
    const IdDictRegion& region = *m_regions[region_index];

    if (m_do_checks) {
	if (region.m_is_empty) {
	    std::cout << "IdDictDictionary::pack32 - region id empty" << std::endl;
	    // bad parameters. 
	    return (1); 
	}
	if(index1 < first_field_index) {
	    std::cout << "IdDictDictionary::pack32 - first_field_index > index1 " 
		      << first_field_index << " " << index1 <<  std::endl;
	    return (1); 
	}
    }
    
    // Set the starting position
    size_t position = Identifier::NBITS;
    if(!first_field_index) {
	const IdDictFieldImplementation& impl = region.m_implementation[index1]; 
	position -= impl.bits_offset(); 
    }
    
    size_t field_index = 0;
    for (size_t i = index1; i <= index2; ++i, ++field_index) { 

	const IdDictFieldImplementation& impl = region.m_implementation[i]; 

	if (m_do_checks) {

	    // Field should be within allowed range
	    if (!impl.ored_field().match(fields[field_index])) {
		std::cout << "IdDictDictionary::pack32 - field does NOT match: value, allowed values " 
			  << fields[field_index] << " " << (std::string)impl.ored_field()
			  << std::endl;
		// bad parameters. 
		return (1); 
	    }

	    // Check that we don't try to go below 0
	    if (0 == position && impl.bits() > 0) {
		std::cout << "IdDictDictionary::pack32 - going past 0" << std::endl;
		return(1);
	    }
	}

        Identifier::value_type index;
	if (impl.decode_index()) {
	    index = impl.ored_field().get_value_index (fields[field_index]); 
	}
	else {
            index = (Identifier::value_type)fields[field_index];
	}
	
	position -= impl.bits(); 
	packedId |= (index << position); 


//                std::cout << "  pack " << id[field_index] << " using field " << 
//                    impl.range()->m_field_name << 
//  		  " #" << i << " (" << (std::string) impl.ored_field() << ")" << 
//  		  " -> index=" << index << 
//  		  " @bit" << position << 
//  		  std::endl; 
 
    } 

    //std::cout << "  packed " << std::hex << packedId << std::dec << std::endl; 
 
    return (0); 
} 
 
/** 
 *   Reset fields from index1 to index2.
 * */
int IdDictDictionary::reset (size_t index1, 
			     size_t index2,
			     size_t region_index,
			     Identifier& packedId) const
{

    // Preconditions... 

    if (m_do_checks) {
	
	if (index2 < index1) { 
	    // bad parameters. 
	    std::cout << "IdDictDictionary::pack32 - index2 < index1 - 1,2"
		      << index1 << " " << index2 << std::endl;
	    return (1); 
	} 
 
	if (region_index >= m_regions.size()) {
	    // bad parameters. 
	    std::cout << "IdDictDictionary::pack32 - region index incorrect - index,size"
		      << region_index << " " << m_regions.size() << std::endl;
	    return (1); 
	}
    }
    

    // Get the region
    const IdDictRegion& region = *m_regions[region_index];

    if (m_do_checks) {
	if (region.m_is_empty) {
	    std::cout << "IdDictDictionary::pack32 - region id empty" << std::endl;
	    // bad parameters. 
	    return (1); 
	}
    }
    
    size_t field_index = 0;
    for (size_t i = index1; i <= index2; ++i, ++field_index) { 

	const IdDictFieldImplementation& impl = region.m_implementation[i]; 

	size_t position = Identifier::NBITS - impl.bits_offset() - impl.bits(); 

	Identifier::value_type mask = (((Identifier::value_type)1 << impl.bits()) - 1) << position;

	mask ^= Identifier::ALL_BITS;

	packedId &= (mask);

    } 
    return (0); 
}



  /** 
   *  Unpack the bits32 id to an expanded Identifier, considering the  
   *  provided prefix (result will include the prefix) 
   */ 
int
IdDictDictionary::unpack (const Identifier& id, 
			  const ExpandedIdentifier& prefix,
			  size_t index2,
			  ExpandedIdentifier& unpackedId) const
{ 
 
  ExpandedIdentifier localPrefix (prefix);
  unpackedId.clear ();
  if (0 < localPrefix.fields ()) unpackedId = localPrefix;

    /** 
     *   First we need to check whether the specified identifier prefix 
     *   really matches the Dictionary specifications. 
     */ 
 
  size_t index1   = 0;  // field index
  size_t position = Identifier::NBITS; // overall bit position - used for debugging only
 
  for (size_t k = 0; k < m_regions.size (); ++k) 
    { 
      bool selected = false; 
 
      const IdDictRegion& region = *m_regions[k];
 
      // Must skip empty regions - can arise when a tag selects an
      // empty region
      if (region.m_is_empty) continue;
 
//      for (size_t i = index1; i < region.m_implementation.size (); ++i) 
      for (size_t i = 0; i < region.m_implementation.size (); ++i) 
        { 
          if (i >= localPrefix.fields ()) 
            { 
                /** 
                 *   ok 
                 *   we require that the region is at least as large as the prefix.  
                 */ 
              selected = true; 
              index1 = i; 
              break; 
            } 
 
          const IdDictFieldImplementation& impl = region.m_implementation[i]; 
 
          if (!impl.field().match (localPrefix[i]))  
            { 
//                std::cout << "Region #" << region.m_index << 
//                    " field " << impl.range()->m_field_name << 
//                    " #" << i << " (" << (std::string) impl.field() << ") does not match " << 
//                    localPrefix[i] << std::endl; 
 
              break; 
            } 
        } 
 
      if (selected) 
        { 
            /** 
             *   We have one region that matches the prefix. 
             *   Let's now try to expand the bits32 from the fields of the region 
             *   that are beyond the prefix. 
             */ 
 
//            bits32 temp = id; 
 
//  	  std::cout << "Region #" << region.m_index << " selected" << std::endl; 
 
          for (size_t i = index1; i < region.m_implementation.size (); ++i) 
            { 
              const IdDictFieldImplementation& impl = region.m_implementation[i]; 
 
              if (impl.bits() == 0) continue; 
 
              Identifier::value_type mask = (static_cast<Identifier::value_type>(1) << impl.bits()) - 1; 

	      if (position < impl.bits()) break;  // Nothing more to get
              size_t index = id.extract(position - impl.bits(), mask);
	      
//  	      std::cout << "Region #" << region.m_index << 
//  		  " field " << impl.range()->m_field_name << 
//  		  " #" << i << " (" << (std::string) impl.field() << ") index " << 
//  		  index << " bits " << impl.bits() << " mask " << hex << mask << " id " << 
//  		  id << dec << std::endl; 

//                size_t index = temp & mask; 
//                temp >>= impl.bits(); 
 
              if (index >= impl.ored_field().get_indices ()) 
                { 
                    /** 
                     *  this index extracted from the bits32 does not seem to 
                     * match this field in the region... 
                     * Let's try another region 
                     */ 
 
//                    std::cout << "  unpack " << index << " using field " << 
//                        impl.range()->m_field_name <<  
//                        " #" << i << " (" << (std::string) impl.ored_field() << ")" <<  
//                        " -> mismatch" <<  
//                        " @bit" << position <<  
//                        std::endl;  
 
                  selected = false; 
                  break; 
                } 
 
              ExpandedIdentifier::element_type value = impl.ored_field().get_value_at (index); 
 
	      /** 
	       *  this index extracted from the bits32 does not 
	       * match this field in the region... 
	       * Let's try another region 
	       */ 

	      if (!impl.field().match (value))  
		  { 
//  		      std::cout << "Region #" << region.m_index << 
//  			  " field " << impl.range()->m_field_name << 
//  			  " #" << i << " (" << (std::string) impl.field() << ") does not match " << 
//  			  value << std::endl; 

		      selected = false; 
		      break; 
		  } 

//                std::cout << "Region #" << region.m_index << " sel " << selected <<
//  		  "  unpack " << index << " using field " << 
//                    impl.range()->m_field_name << 
//                    " #" << i << " (" << (std::string) impl.ored_field() << ")" << 
//                    " -> value=" << value << 
//                    " @bit" << position << " nbits " << impl.bits() <<
//                    std::endl; 

	      // Found value

              unpackedId.add (value); 
	      localPrefix.add (value);
 
              position -= impl.bits(); // overall bit position

//  	      id = temp; // set id to shifted value


	      index1++;  // next field

	      if (index1 > index2) break; // quit at index2
            } 
 
          if (selected) break; 
        } 
    } 
 
  return (0); 
} 

/** 
 *  Unpack the bits32 id to a string, considering the provided
 *  prefix (result will include the prefix) and up to index2 -
 *  (index1 is assumed to be 0, i.e. part of prefix).
 */
int 
IdDictDictionary::unpack (const Identifier& id, 
			  const ExpandedIdentifier& prefix,
			  size_t index2,
			  const std::string& sep,
			  std::string& unpackedId) const
{ 
 
    ExpandedIdentifier localPrefix (prefix);
    //unpackedId.clear ();
    //if (0 < localPrefix.fields ()) unpackedId = localPrefix;

    /** 
     *   First we need to check whether the specified identifier prefix 
     *   really matches the Dictionary specifications. 
     */ 
 
    size_t index1   = 0;  // field index
    size_t position = Identifier::NBITS; // overall bit position - used for debugging only
 
    for (size_t k = 0; k < m_regions.size (); ++k) { 
	bool selected = false; 
 
	const IdDictRegion& region = *m_regions[k];
 
	// Must skip empty regions - can arise when a tag selects an
	// empty region
	if (region.m_is_empty) continue;
 
//      for (size_t i = index1; i < region.m_implementation.size (); ++i) 
	for (size_t i = 0; i < region.m_implementation.size (); ++i) { 
	    if (i >= localPrefix.fields ()) { 
		/** 
	       *   ok 
	       *   we require that the region is at least as large as the prefix.  
	       */ 
		selected = true; 
		index1 = i; 
		break; 
	    } 
 
	    const IdDictFieldImplementation& impl = region.m_implementation[i]; 
 
	    if (!impl.field().match (localPrefix[i])) { 
//                std::cout << "Region #" << region.m_index << 
//                    " field " << impl.range()->m_field_name << 
//                    " #" << i << " (" << (std::string) impl.field() << ") does not match " << 
//                    localPrefix[i] << std::endl; 
 
		break; 
	    } 
	} 
 
	if (selected) { 
	    /** 
	     *   We have one region that matches the prefix. 
	     *   Let's now try to expand the bits32 from the fields of the region 
	     *   that are beyond the prefix. 
	     */ 
 
//           	 bits32 temp = id; 
 
//  	  std::cout << "Region #" << region.m_index << " selected" << std::endl; 
 
	    for (size_t i = index1; i < region.m_implementation.size (); ++i) { 
		const IdDictFieldImplementation& impl = region.m_implementation[i]; 

		if (impl.bits() == 0) continue; 
 
		Identifier::value_type mask = (static_cast<Identifier::value_type>(1) << impl.bits()) - 1; 

		if (position < impl.bits()) break;  // Nothing more to get
		size_t index = id.extract(position - impl.bits(), mask);
	      
//  	      std::cout << "Region #" << region.m_index << 
//  		  " field " << impl.range()->m_field_name << 
//  		  " #" << i << " (" << (std::string) impl.field() << ") index " << 
//  		  index << " bits " << impl.bits() << " mask " << hex << mask << " id " << 
//  		  id << dec << std::endl; 

//                size_t index = temp & mask; 
//                temp >>= impl.bits(); 
 
		if (index >= impl.ored_field().get_indices ()) { 
		    /** 
		     *  this index extracted from the bits32 does not seem to 
		     * match this field in the region... 
		     * Let's try another region 
		     */ 
 
//                    std::cout << "  unpack " << index << " using field " << 
//                        impl.range()->m_field_name <<  
//                        " #" << i << " (" << (std::string) impl.ored_field() << ")" <<  
//                        " -> mismatch" <<  
//                        " @bit" << position <<  
//                        std::endl;  
		    selected = false; 
		    break; 
		} 
 
		ExpandedIdentifier::element_type value = impl.ored_field().get_value_at (index); 

		/** 
		 *  this index extracted from the bits32 does not 
		 * match this field in the region... 
		 * Let's try another region 
		 */ 

		if (!impl.field().match (value)) { 
//  		      std::cout << "Region #" << region.m_index << 
//  			  " field " << impl.range()->m_field_name << 
//  			  " #" << i << " (" << (std::string) impl.field() << ") does not match " << 
//  			  value << std::endl; 
		    selected = false; 
		    break; 
		} 	
	      
//                std::cout << "Region #" << region.m_index << " sel " << selected <<
//  		  "  unpack " << index << " using field " << 
//                    impl.range()->m_field_name << 
//                    " #" << i << " (" << (std::string) impl.ored_field() << ")" << 
//                    " -> value=" << value << 
//                    " @bit" << position << " nbits " << impl.bits() <<
//                    std::endl; 

		// Found value

		// Add value to string

		std::string str_value("nil");
		char temp[20];

		// The policy below is:
		//   - if a value is a character string or name, just add this name
		//   - if a value is a number, then prefix it with the
		//     name of the field
		//
		// NOTE: min/max is a number, but for value/label we
		// distinguish between number and name by looking for an IdDictLabel
		const IdDictRange*  range = impl.range(); 
		IdDictLabel* label = range->m_field->find_label(range->m_label);  
		switch (range->m_specification) { 
		case IdDictRange::by_minvalue: 
		case IdDictRange::by_maxvalue: 
		case IdDictRange::by_minmax: 
		    // For a range of values (numbers), add in the field name
		    str_value = range->m_field->m_name + ' ';
		    sprintf (temp, "%d", value);
		    str_value += temp;
		    break; 
		case IdDictRange::by_value: 
		case IdDictRange::by_label: 
		    str_value = "";
		    if (!label) {
			// Is a number, add in field name
			str_value += range->m_field->m_name + ' ';
		    }
		    str_value += range->m_label;
		    break; 
		case IdDictRange::by_values: 
		case IdDictRange::by_labels: 
		    str_value = "";
		    // Is a name
		    if (label) {
			// Found label with "find_label" on the field
			if (label->m_valued) {
			    str_value += range->m_label;
			}
		    }
		    else {
			// If not found with the "find" above, we must
			// get the value and name from the range
			// itself.

			unsigned int index1 = 0;
			for (; index1 < range->m_values.size(); ++index1) {
			    if (value == range->m_values[index1]) {
				break;
			    }
			}

			// In some cases we 
			if (index1 < range->m_labels.size()) {
			    if (isNumber(range->m_labels[index1])) {
				str_value += range->m_field->m_name + ' ';
			    }
			    str_value += range->m_labels[index1];
			}
			else {
			    std::cout << "IdDictDictionary::unpack - Could not find value." << std::endl;
			    std::cout << "value " << value << std::endl;
			    std::cout << "field values " << std::endl;
			    for (unsigned int i=0; i < range->m_values.size(); ++i) {
				std::cout << range->m_values[i] << " ";
			    }
			    std::cout << std::endl;
			}
		    }
		    break; 
		case IdDictRange::unknown: 

		    std::cout << "unknown" << std::endl;
		    
		    break; 
		}
		

		if (index1)unpackedId += sep;
		unpackedId += str_value;
		localPrefix.add (value);
 
		position -= impl.bits(); // overall bit position

//  	      id = temp; // set id to shifted value


		index1++;  // next field

		if (index1 > index2) break; // quit at index2
	    }
	    if (selected) break; 
	} 
  } 
 
  return (0); 
} 

 
/** 
 **  Unpack a single field of the bits32 id. One specifies the
 **  field index desired and passes in the region index to be used
 **  for decoding. The region index is obtained from
 **  find_region. The first_field_index is normally 0. It is
 **  non-zero when fields 0 to first_field_index -1 are missing
 **  from the compact 
 */
int 
IdDictDictionary::unpack (const Identifier& id, 
			  size_t first_field_index, 
			  size_t field_index,
			  size_t region_index,
			  int& field) const
{

    field = 0; 
    const IdDictRegion& region = *m_regions[region_index];
    const IdDictFieldImplementation& impl = region.m_implementation[field_index]; 
    size_t prefix_offset = 0;

    size_t position = Identifier::NBITS; // overall bit position

    if (m_do_checks) {
	
	// Check regions
	if (region_index >= m_regions.size()) {
	    std::cout << "IdDictDictionary::unpack - region index too large. Index, nregions " 
		      << region_index << " " << m_regions.size() << std::endl;
	    return (1);
	}
	// check number of fields
	if (field_index >= region.m_implementation.size()) {
	    std::cout << "IdDictDictionary::unpack - field index too large. Index, nfields " 
		      << field_index << " " << region.m_implementation.size() 
		      << std::endl;
	    return (1);
	}
	
    }
    
    // One or more fields missing from prefix, get the offset
    if (first_field_index) {
	if (m_do_checks) {
	    if (first_field_index >= region.m_implementation.size()) {
		std::cout << "IdDictDictionary::unpack - first_field_index too large. Index, nfields " 
			  << first_field_index << " " << region.m_implementation.size() 
			  << std::endl;
		return (1);
	    }
	}
	
	// One or more fields missing from prefix, get the offset
	prefix_offset = region.m_implementation[first_field_index].bits_offset();

	if (m_do_checks) {
	    // Should have a non-zero number of bits
	    if (impl.bits() == 0) {
		std::cout << "IdDictDictionary::unpack - no bits for this field. Region, field indexes " 
			  << region_index << " " << field_index << std::endl;
		return (1);
	    }

	    // Check the shift value
	    if (impl.bits() + impl.bits_offset() - prefix_offset > position) {
		std::cout << "IdDictDictionary::unpack - bits + offset too large. Region, field indexes, bits, offset " 
			  << region_index << " " << field_index << " "
			  << impl.bits() << " " << impl.bits_offset()  << " " << prefix_offset
			  << std::endl;
		return (1);
	    }
	}
    }

    Identifier::value_type mask = (static_cast<Identifier::value_type>(1) << impl.bits()) - 1; 

    size_t index = id.extract(position -= impl.bits() + impl.bits_offset() - prefix_offset, mask);

    field = index;
    if (impl.decode_index()) field = impl.ored_field().get_value_at (index); 


    return (0);
}

/** 
 **  Copy a number of fields of the bits32 id into another bits32
 **  id. One specifies the begin and end (inclusive) field indexes
 **  desired and passes in the region index to be used for
 **  decoding. The region index is obtained from find_region.  The
 **  first_field_index is normally 0. It is non-zero when fields 0 to
 **  first_field_index -1 are missing from the compact. In this case,
 **  idout is shift by the number of missing bits, if possible.
 */
int 
IdDictDictionary::copy (const Identifier& idin, 
			size_t first_field_index, 
			size_t begin_field_index,
			size_t end_field_index,
			size_t region_index,
			Identifier& idout) const
{

    idout = Identifier();
    if (region_index >= m_regions.size()) {
	std::cout << "IdDictDictionary::copy - region index too large. Index, nregions " 
		  << region_index << " " << m_regions.size() << std::endl;
	return (1);
    }
    
    const IdDictRegion& region = *m_regions[region_index];

    if (first_field_index >= region.m_implementation.size() ||
	begin_field_index >= region.m_implementation.size() ||
	end_field_index   >= region.m_implementation.size()) {
	std::cout << "IdDictDictionary::copy - field index too large. Indexes first, begin, end, nfields " 
		  << first_field_index << " " 
		  << begin_field_index << " " 
		  << end_field_index << " " 
		  << region.m_implementation.size() 
		  << std::endl;
	return (1);
    }

    size_t missing_offset = 0;
    if (first_field_index) {
	if (first_field_index > begin_field_index) {
	    std::cout << "IdDictDictionary::copy - first_field_index > begin_field_index. Indexes " 
		      << first_field_index << " " << begin_field_index << std::endl;
	    return (1);
	}
	// One or more fields missing from prefix, get the offset
	missing_offset = region.m_implementation[first_field_index].bits_offset();
    }
    
    size_t prefix_offset = 0;
    if (begin_field_index) {
	if (begin_field_index > end_field_index) {
	    std::cout << "IdDictDictionary::copy - begin_field_index > end_field_index. Indexes " 
		      << begin_field_index << " " << end_field_index << std::endl;
	    return (1);
	}
	// One or more fields missing from prefix, get the offset
	prefix_offset = region.m_implementation[begin_field_index].bits_offset();
    }
    
    const IdDictFieldImplementation& impl = region.m_implementation[end_field_index]; 
    size_t suffix_offset = impl.bits() + impl.bits_offset();
 
    size_t position = Identifier::NBITS; // overall bit position

    if (position < prefix_offset - missing_offset) {
	std::cout << "IdDictDictionary::copy - position < prefix + missing. " 
		      << prefix_offset << " " << missing_offset << std::endl;
	return (1);
    }
	
    
    if (position < suffix_offset + missing_offset) {
	std::cout << "IdDictDictionary::copy - position < suffix + missing. " 
		      << suffix_offset << " " << missing_offset << std::endl;
	return (1);
    }


    // prepare the mask for copying

    Identifier::value_type mask = Identifier::ALL_BITS;

    Identifier::value_type prefix_mask = prefix_offset ? (static_cast<Identifier::value_type>(1) << (position - prefix_offset + missing_offset)) - 1 : 0;
    
    Identifier::value_type suffix_mask = (static_cast<Identifier::value_type>(1) << (position - suffix_offset + missing_offset)) - 1;

    mask -= prefix_mask + suffix_mask;
    
    idout = idin.mask_shift(mask, missing_offset);

//      std::cout << hex << "pre, suff, in, out " << prefix_mask << " " 
//  	      << suffix_mask << " " 
//  	      << idin << " " 
//  	      << idout << " "
//  	      << missing_offset << " " 
//  	      << prefix_offset << " " 
//  	      << suffix_offset << " "
//  	      << std::endl;
    

    return (0);
}

 
 
bool		
IdDictDictionary::do_checks	(void) const
{
    return (m_do_checks);
}

void		
IdDictDictionary::set_do_checks	(bool do_checks)
{
    m_do_checks = do_checks;
}

bool		
IdDictDictionary::do_neighbours	(void) const
{
    return (m_do_neighbours);
}

void		
IdDictDictionary::set_do_neighbours	(bool do_neighbours)
{
    m_do_neighbours = do_neighbours;
}

 
/** 
 * 
 */ 
IdDictField::IdDictField () 
    : 
    m_index(0)
    //m_resolved_references(false)
    //m_generated_implementation(false)
{
}
  
IdDictField::~IdDictField () 
{ 
} 
  
void IdDictField::resolve_references (const IdDictMgr& /*idd*/) 
{ 
} 
 
void IdDictField::generate_implementation (const IdDictMgr& /*idd*/, 
					   std::string /*tag*/) 
{ 
} 
 
void IdDictField::reset_implementation () 
{ 
} 
 
bool IdDictField::verify () const 
{ 
  return (true); 
} 
 
IdDictLabel* IdDictField::find_label (const std::string& name) const 
{ 
  for (size_t i = 0; i < m_labels.size (); ++i) 
    { 
      IdDictLabel* label = m_labels[i]; 
      if ((label != 0) && (label->m_name == name)) return (label); 
    } 
 
  return (0); 
} 
 
void IdDictField::add_label (IdDictLabel* label) 
{ 
  m_labels.push_back (label); 
} 
  
size_t IdDictField::get_label_number () const 
{ 
  return (m_labels.size ()); 
} 
 
const std::string& IdDictField::get_label (size_t index) const 
{ 
  return ((m_labels[index])->m_name); 
} 
 
ExpandedIdentifier::element_type IdDictField::get_label_value (const std::string& name) const 
{ 
  ExpandedIdentifier::element_type value = 0; 
 
  int int_value; 
 
  if (sscanf (name.c_str (), "%80d", &int_value) == 1)  
    { 
      value = (ExpandedIdentifier::element_type) int_value; 
      return (value); 
    } 
 
  for (size_t i = 0; i < m_labels.size (); ++i) 
    { 
      IdDictLabel* label = m_labels[i]; 
      if (label == 0) continue; 
 
      if (label->m_valued) value = label->m_value; 
 
      if (label->m_name == name)  
        { 
          return (value); 
        } 
 
      value++; 
    } 
 
  std::cerr << "Warning : label " << name << " not found" << std::endl; 
 
  return (0); 
} 
 
void IdDictField::clear () 
{ 
  for (size_t i = 0; i < m_labels.size (); ++i) 
    { 
      IdDictLabel* label = m_labels[i]; 
      delete label; 
    } 
 
  m_labels.clear (); 
} 
 
 
 
 
/** 
 * 
 */ 
IdDictLabel::IdDictLabel () 
  : m_valued (false),
    m_value (0)
{ 
} 
 
IdDictLabel::~IdDictLabel () 
{ 
} 
 
/** 
 * 
 */ 
IdDictGroup::IdDictGroup ()
    :
    //m_resolved_references(false),
    m_generated_implementation(false)
{
}

IdDictGroup::IdDictGroup (const std::string& name)
    :
    m_name(name),
    //m_resolved_references(false),
    m_generated_implementation(false)
{
}

IdDictGroup::~IdDictGroup ()
{
}

std::string  IdDictGroup::name()
{
    return (m_name);
}

const std::vector<IdDictDictEntry*>& 
IdDictGroup::entries()
{
    return (m_entries);
}

const std::vector<IdDictRegion*>&    
IdDictGroup::regions()
{
    return (m_regions);
}


MultiRange 
IdDictGroup::build_multirange () const
{
  MultiRange result; 
 
  IdDictDictionary::regions_const_it it; 

  for (it = m_regions.begin (); it != m_regions.end (); ++it) 
    { 
      const IdDictRegion& region = *(*it);

      // skip regions created from parents
      if("dummy" == region.m_name) continue;

      // skip empty regions - may arise from alternate_regions
      // where a tag selects an empty region
      if(region.m_is_empty) continue;

      Range r = region.build_range();
      result.add (std::move(r));

    } 
 
  return (result); 
}

void 
IdDictGroup::add_dictentry (IdDictDictEntry* region)
{ 
    m_entries.push_back (region);
} 

void 
IdDictGroup::resolve_references (const IdDictMgr& idd,  
				 IdDictDictionary& dictionary, 
				 size_t& index)
{
    IdDictDictionary::entries_it it; 
    for (it = m_entries.begin (); it != m_entries.end (); ++it) {  
        (*it)->set_index(index);  
        index++; 
 
        (*it)->resolve_references (idd, dictionary);  
    }  
}

void 
IdDictGroup::generate_implementation (const IdDictMgr& idd,  
				      IdDictDictionary& dictionary, 
				      std::string tag)
{
  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictGroup::generate_implementation>" << std::endl; 
    } 

  if (!m_generated_implementation) {
      
      // Loop over entries and fill regions vec with selected region
      // (AltRegions have a selection)
      IdDictDictionary::entries_it it; 
      for (it = m_entries.begin (); it != m_entries.end (); ++it) { 
	  (*it)->generate_implementation (idd, dictionary, tag);  
	  // Get region and save in m_regions
	  IdDictRegion* region = dynamic_cast<IdDictRegion*> (*it);
	  if(region) {
	      m_regions.push_back(region);
	  }
	  else {
	      IdDictAltRegions* altregions = dynamic_cast<IdDictAltRegions*> (*it);
	      if(altregions) {
		  m_regions.push_back(altregions->m_selected_region);
	      }
	  }
      }  

      if (m_regions.size() != m_entries.size()) {
	  std::cout << "IdDictGroup::generate_implementation - mismatch of sizes: regions/entries " 
		    << m_regions.size() << " " << m_entries.size()
		    << std::endl; 
      }
 
      m_generated_implementation = true;
  } 
}

void 
IdDictGroup::reset_implementation ()
{
  if (m_generated_implementation) {
      
      m_regions.clear();
      IdDictDictionary::entries_it it; 
      for (it = m_entries.begin (); it != m_entries.end (); ++it) { 
	  (*it)->reset_implementation();  
      }  
      m_generated_implementation = false;
  } 
}


bool 
IdDictGroup::verify () const
{
    // Should check that all regions have the same number of levels,
    // which is part of the definition of a group
    return (true);
}

/** 
 * 
 *   Sort:
 *
 *      Loop over regions and sort according to their first identifier
 * 
 **/

void IdDictGroup::sort () 
{ 

    std::map< ExpandedIdentifier, IdDictDictEntry* > regions;
      
    IdDictDictionary::regions_it it; 

    for (it = m_regions.begin (); it != m_regions.end (); ++it) { 

	const IdDictRegion& region = *(*it);
	Range range = region.build_range ();
	Range::identifier_factory first = range.factory_begin();
	Range::identifier_factory last  = range.factory_end();
	if (first != last) {
	    regions[*first] = *it;
	}
	else {
	    std::cout << "IdDictDictionary::sort - WARNING empty region cannot sort "
		      << std::endl; 
	}
    } 
    if (regions.size() == m_regions.size()) {
	// Reorder the regions
	std::map< ExpandedIdentifier, IdDictDictEntry* >::iterator mapIt = regions.begin();
	std::vector<IdDictRegion*>::size_type vecIt = 0; 
	for (; mapIt != regions.end (); ++mapIt, ++vecIt) { 
	    m_entries[vecIt] = (*mapIt).second;
	}
    }
    else {
	std::cout << "IdDictGroup::sort - WARNING region map size is NOT the same as the vector size. Map size "
		  << regions.size() << " vector size " << m_regions.size()
		  << std::endl; 
    }
} 
 
void 
IdDictGroup::clear ()
{
    IdDictDictionary::entries_it it; 
  
    for (it = m_entries.begin (); it != m_entries.end (); ++it) {

	IdDictDictEntry* region = *it;
        region->clear ();  
        delete region; 
    }  
 
    m_entries.clear (); 
}
 
  
/** 
 * 
 */ 
IdDictDictEntry::IdDictDictEntry ()
{
}

IdDictDictEntry::~IdDictDictEntry ()
{
}

/** 
 * 
 */ 
IdDictAltRegions::IdDictAltRegions()
    :
    m_selected_region(0)
{
}

IdDictAltRegions::~IdDictAltRegions()
{
    std::map<std::string, IdDictRegion* >::iterator first = m_regions.begin();
    std::map<std::string, IdDictRegion* >::iterator last  = m_regions.end();
    for (; first != last; ++first) {
	delete (*first).second;
    }
}

std::string 
IdDictAltRegions::group_name () const
{
    std::string result;
    if (1 <= m_regions.size()) result = (*m_regions.begin()).second->group_name();
    return (result);
}

 
void 
IdDictAltRegions::set_index (size_t index)
{
    map_iterator first = m_regions.begin();
    map_iterator last  = m_regions.end();
    for (; first != last; ++first) {
	(*first).second->set_index (index);
    }
}


void 
IdDictAltRegions::resolve_references (const IdDictMgr& idd,  
				      IdDictDictionary& dictionary)
{
    // We assume that it is not necessary to select only those with
    // the correct tag -> send to all in map
    map_iterator first = m_regions.begin();
    map_iterator last  = m_regions.end();
    for (; first != last; ++first) {
	(*first).second->resolve_references (idd, dictionary);
    }
}

void 
IdDictAltRegions::generate_implementation (const IdDictMgr& idd,  
					  IdDictDictionary& dictionary, 
					  std::string tag)
{
    // Find the region given by the tag
    map_iterator region_it = m_regions.find(tag);
    if(region_it == m_regions.end()) {
	std::cout << "IdDictAltRegions::generate_implementation could not find region for tag " 
		  << tag << " Keys in map " << std::endl;
	map_iterator first = m_regions.begin();
	map_iterator last  = m_regions.end();
	int i = 0;
	for (; first != last; ++first, ++i) {
	    std::cout << " i " << i << " key " << (*first).first;
	}
	std::cout << std::endl;
	return;
    }
    m_selected_region = (*region_it).second;
//      std::cout << "IdDictAltRegions::generate_implementation found region for tag " 
//  	      << tag << " " 
//  	      << m_selected_region->m_entries.size() << " "
//  	      << m_selected_region->m_implementation.size() << " "
//  	      << std::endl;

    m_selected_region->generate_implementation(idd, dictionary, tag);    
//      std::cout << "IdDictAltRegions::generate_implementation found region for tag " 
//  	      << tag << " " 
//  	      << (std::string)m_selected_region->build_range() << std::endl;
}

void 
IdDictAltRegions::reset_implementation ()
{
    if(m_selected_region)m_selected_region->reset_implementation();
}


bool 
IdDictAltRegions::verify () const
{
    return (true);
}

void 
IdDictAltRegions::clear ()
{
    map_iterator first = m_regions.begin();
    map_iterator last  = m_regions.end();
    for (; first != last; ++first) {
	(*first).second->clear();
	delete (*first).second;
    }
    m_regions.clear();
}
 
Range 
IdDictAltRegions::build_range () const
{
    Range result;
    
    if(m_selected_region)result = m_selected_region->build_range();
    return (result);
}

 
/** 
 * 
 */ 
IdDictRegion::IdDictRegion () 
    :
    m_index(0),
    m_is_empty(false),
    m_prev_abs_eta(0),
    m_next_abs_eta(0),
    m_eta0(0.0),
    m_deta(0.0),
    m_phi0(0.0),
    m_dphi(0.0),
    m_resolved_references(false),
    m_generated_implementation(false)
{ 
} 
 
IdDictRegion::~IdDictRegion () 
{ 
} 
 
std::string 
IdDictRegion::group_name () const
{
    return (m_group);
}

void 
IdDictRegion::set_index (size_t index)
{
    m_index = index;
}

void IdDictRegion::add_entry (IdDictRegionEntry* entry) 
{ 
  m_entries.push_back (entry); 
} 
 
void IdDictRegion::resolve_references (const IdDictMgr& idd, 
				       IdDictDictionary& dictionary) 
{ 
  std::vector<IdDictRegionEntry*>::iterator it;  
  
  for (it = m_entries.begin (); it != m_entries.end (); ++it)  
    {  
      IdDictRegionEntry* entry = *it;  
      entry->resolve_references (idd, dictionary, *this);  
    } 
} 
  
void IdDictRegion::generate_implementation (const IdDictMgr& idd, 
					    IdDictDictionary& dictionary,
					    std::string tag) 
{ 

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictRegion::generate_implementation>" << std::endl; 
    } 
  
//    std::cout << "IdDictRegion::generate_implementation - name, tag " 
//  	    << m_name << " " << m_tag << std::endl; 

  if (!m_generated_implementation) {
      
      std::vector<IdDictRegionEntry*>::iterator it;  
  
      for (it = m_entries.begin (); it != m_entries.end (); ++it)  {  
	  IdDictRegionEntry* entry = *it;  
	  entry->generate_implementation (idd, dictionary, *this, tag);  
      } 
      m_generated_implementation = true;
  }
} 
  
void IdDictRegion::find_neighbours (const IdDictDictionary& dictionary)
{
    // Find the neighbours
    IdDictRegion* region = 0;
    if ("" != m_next_abs_eta_name) {
	region = dictionary.find_region(m_next_abs_eta_name,m_group );
	if (region) {
	    region->m_prev_abs_eta = this;
	    m_next_abs_eta         = region;
//  	    std::cout << "prev/next eta " << m_name << " " << region->m_name << std::endl;
	}
	else {
//  	    std::cout << "no region found for next eta " << m_name << " " << m_next_abs_eta_name << std::endl;
	}
	  
    }
    for (unsigned int i = 0; i < m_prev_samp_names.size(); ++i) {
	if ("" != m_prev_samp_names[i]) {
	    region = dictionary.find_region(m_prev_samp_names[i],m_group );
	    if (region) {
		m_prev_samp.push_back(region);
//  		std::cout << "prev samp " << m_name << " " << region->m_name << std::endl;
	    }
	    else {
//  		std::cout << "no region found for prev samp " << m_name << " " << m_prev_samp_names[i] << std::endl;
	    }
	      
	}
    }
    for (unsigned int i = 0; i < m_next_samp_names.size(); ++i) {
	if ("" != m_next_samp_names[i]) {
	    region = dictionary.find_region(m_next_samp_names[i],m_group );
	    if (region) {
		m_next_samp.push_back(region);
//  		std::cout << "next samp " << m_name << " " << region->m_name << std::endl;
	    }
	    else {
//  		std::cout << "no region found for next samp " << m_name << " " << m_next_samp_names[i] << std::endl;
	    }
	}
    }

    for (unsigned int i = 0; i < m_prev_subdet_names.size(); ++i) {
	if ("" != m_prev_subdet_names[i]) {
	    region = dictionary.find_region(m_prev_subdet_names[i],m_group );
	    if (region) {
		m_prev_subdet.push_back(region);
//  		std::cout << "prev subdet " << m_name << " " << region->m_name << std::endl;
	    }
	    else {
//  		std::cout << "no region found for prev subdet " << m_name << " " << m_prev_subdet_names[i] << std::endl;
	    }
	}
    }
    for (unsigned int i = 0; i < m_next_subdet_names.size(); ++i) {
	if ("" != m_next_subdet_names[i]) {
	    region = dictionary.find_region(m_next_subdet_names[i],m_group );
	    if (region) {
		m_next_subdet.push_back(region);
//  		std::cout << "next subdet " << m_name << " " << region->m_name << std::endl;
	    }
	    else {
//  		std::cout << "no region found for next subdet " << m_name << " " << m_next_subdet_names[i] << std::endl;
	    }
	}
    }
}


void IdDictRegion::reset_implementation () 
{ 

  if (m_generated_implementation) {
      
      m_implementation.clear();  // remove implementation

      std::vector<IdDictRegionEntry*>::iterator it;  
  
      for (it = m_entries.begin (); it != m_entries.end (); ++it)  {  
	  IdDictRegionEntry* entry = *it;  
	  entry->reset_implementation ();  
      } 

      // reset neighbours
      m_prev_abs_eta = 0;
      m_next_abs_eta = 0;
      m_prev_samp.clear();
      m_next_samp.clear();
      m_prev_subdet.clear();
      m_next_subdet.clear();

      m_generated_implementation = false;
  }
} 
  
bool IdDictRegion::verify () const 
{ 
  return (true); 
} 
 
void IdDictRegion::clear () 
{ 
  std::vector<IdDictRegionEntry*>::iterator it;  
  
  for (it = m_entries.begin (); it != m_entries.end (); ++it)  
    {  
      IdDictRegionEntry* entry = *it;  
      entry->clear (); 
      delete entry; 
    }  
 
  m_entries.clear (); 
} 
  
Range IdDictRegion::build_range () const 
{
  Range result; 
  std::vector <IdDictRegionEntry*>::const_iterator it;
  for (it = m_entries.begin (); it != m_entries.end (); ++it)
  { 
      const IdDictRegionEntry& entry = *(*it); 
      Range r = entry.build_range (); 


      //std::cout << "IdDictRegion::build_range> wrap " << r[0].wrap_around() << std::endl; 


      //std::cout << "IdDictRegion::build_range> [" << (std::string) r << "]" << std::endl; 
      result.add (std::move(r)); 
  } 

  return (result); 
//    // Use max size
//    return build_range(m_implementation.size ());
} 
  
  //   Provide range for all levels up to and including index2
//  Range IdDictRegion::build_range (size_t index2) const
//  { 
//    Range result; 
 
//      /* 
//    std::vector <IdDictRegionEntry*>::const_iterator it; 
//    for (it = m_entries.begin (); it != m_entries.end (); ++it) 
//      { 
//        const IdDictRegionEntry& entry = *(*it); 
//        Range r = entry.build_range (); 
//          //std::cout << "IdDictRegion::build_range> [" << (std::string) r << "]" << std::endl; 
//        result.add (r); 
//      } 
//      */ 

//    size_t index = 0;
//    std::vector <IdDictFieldImplementation>::const_iterator it; 
//    for (it = m_implementation.begin (); it != m_implementation.end (); ++it, ++index) 
//      { 
//        const IdDictFieldImplementation& impl = *it; 
//        result.add (impl.field()); 
//        if (index2 <= index)
//      } 
 
//    return (result); 
//  } 
  
 
 
 
 
 
/** 
 * 
 */ 
IdDictSubRegion::IdDictSubRegion () 
{ 
} 
 
IdDictSubRegion::~IdDictSubRegion () 
{ 
} 
 
void 
IdDictSubRegion::generate_implementation (const IdDictMgr& /*idd*/, 
					  IdDictDictionary& /*dictionary*/, 
					  std::string /*tag*/)
{
    std::cout << "IdDictSubRegion::generate_implementation - SHOULD NEVER BE CALLED "  << std::endl;
}


void 
IdDictSubRegion::generate_implementation (const IdDictMgr& idd,  
					  IdDictDictionary& dictionary, 
					  IdDictRegion& region,
					  std::string tag) 
{ 

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictSubRegion::generate_implementation>" << std::endl; 
    } 
  
    // NOTE: we DO NOT protect this method with
    // m_generated_implementation because a subregion is a "reference"
    // and must be looped over to fully implement a region.

      std::vector<IdDictRegionEntry*>::iterator it;  
  
      for (it = m_entries.begin (); it != m_entries.end (); ++it) { 
	  IdDictRegionEntry* entry = *it;  
	  entry->generate_implementation (idd, dictionary, region, tag);  
      } 
} 
 
void IdDictSubRegion::reset_implementation ()
{ 
      std::vector<IdDictRegionEntry*>::iterator it;  
  
      for (it = m_entries.begin (); it != m_entries.end (); ++it) { 
	  IdDictRegionEntry* entry = *it;  
	  entry->reset_implementation ();  
      } 
} 
 
  
 
 
 
 
/** 
 * 
 */ 
IdDictRegionEntry::IdDictRegionEntry () 
{ 
} 
 
IdDictRegionEntry::~IdDictRegionEntry () 
{ 
} 
 
void IdDictRegionEntry::resolve_references (const IdDictMgr& /*idd*/,  
					    IdDictDictionary& /*dictionary*/, 
					    IdDictRegion& /*region*/) 
{ 
} 
  
void IdDictRegionEntry::generate_implementation (const IdDictMgr& /*idd*/,  
						 IdDictDictionary& /*dictionary*/, 
						 IdDictRegion& /*region*/,
						 std::string /*tag*/) 
{ 
} 
  
void IdDictRegionEntry::reset_implementation ()
{ 
} 

bool IdDictRegionEntry::verify () const 
{ 
  return (true); 
} 
 
void IdDictRegionEntry::clear () 
{ 
} 

/** 
 * 
 */ 
IdDictRange::IdDictRange () 
    :
    m_field (0),
    m_specification (unknown),
    m_value(0),
    m_minvalue(0),
    m_maxvalue(0),
    m_prev_value(0),
    m_next_value(0),
    m_continuation_mode(none),
    m_resolved_references(false)
    //m_generated_implementation(false)
{
} 
 
IdDictRange::~IdDictRange () 
{ 
} 
 
void IdDictRange::resolve_references (const IdDictMgr& /*idd*/,  
				      IdDictDictionary& dictionary, 
				      IdDictRegion& /*region*/) 
{ 
    if(!m_resolved_references) {
	m_field = dictionary.find_field (m_field_name); 
	if (m_field == 0)  { 
	    m_field = new IdDictField; 
	    m_field->m_name = m_field_name; 
 
	    dictionary.add_field (m_field); 
	} 
 
	if (m_specification == unknown) { 
	    /** 
	     *  The range values were unspecified in the range element. 
	     * 
	     *  Therefore, the set of values must be obtained from the 
	     * field definition (if it exists!!). 
	     * 
	     *  If the field is undefined, then too bad, this range will be dummy. 
	     * 
	     */ 
	    unsigned int labels = m_field->get_label_number (); 
 
	    if (labels == 1)    { 
		m_specification = by_label; 
		m_label = m_field->get_label (0); 
	    } 
	    else if (labels > 1)    { 
		m_specification = by_labels; 
		for (size_t i = 0; i < labels; ++i) { 
		    m_labels.push_back (m_field->get_label (i)); 
		} 
	    } 
	} 
 
	if (m_specification == by_label) { 
	    m_value = m_field->get_label_value (m_label); 
 
//      std::cout << "IdDictRange::resolve_references> field=" << m_field_name <<  
//          " " << m_label << "=" << m_value << std::endl; 
	} 
	else if (m_specification == by_labels)  { 
	    m_values.clear (); 
	    for (size_t i = 0; i < m_labels.size (); ++i) { 
		const std::string& label = m_labels[i]; 
		int value = m_field->get_label_value (label); 
 
		m_values.push_back (value); 
 
//          std::cout << "IdDictRange::resolve_references> field=" << m_field_name <<  
//              " " << label << "=" << value << std::endl; 
	    } 
	} 
	m_resolved_references = true;
    }
} 
  
void IdDictRange::generate_implementation (const IdDictMgr& /*idd*/,  
					   IdDictDictionary& dictionary, 
					   IdDictRegion& region,
					   std::string /*tag*/) 
{ 

    // Add IdDictFieldImplementation to this region

    // NOTE: we DO NOT protect this method with
    // m_generated_implementation because the same object may be
    // called more than once because there are IdDictRangeRef's which
    // point to the same IdDictRange's.

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictRange::generate_implementation>" << std::endl; 
    } 


//    std::cout << "IdDictRange::generate_implementation - field name " 
//  	    << m_field_name << " " << m_field << " "
//  	    << m_labels.size() << " " << m_values.size() 
//  	    << std::endl;


  region.m_implementation.resize (region.m_implementation.size () + 1); 
  IdDictFieldImplementation& impl = region.m_implementation.back (); 
  impl.set_range(this); 
  if (m_field->m_index == 0)  { 
      m_field->m_index = region.m_implementation.size () - 1; 
//        std::cout << "Good field index for " << m_field_name <<  
//  	  " index " << m_field->m_index <<
//  	  " in dictionary " << dictionary.m_name <<  
//  	  " region #" << region.m_index
//  		<<  " size " << (region.m_implementation.size () - 1)
//  		<< std::endl; 
  } 
  else if (m_field->m_index != (region.m_implementation.size () - 1))  { 
      std::cout <<  "Bad field index for " << m_field_name 
		<<  " index " << m_field->m_index 
		<<  " in dictionary " << dictionary.m_name   
		<<  " region #" << region.m_index 
		<<  " size " << (region.m_implementation.size () - 1)
		<< std::endl; 
  } 

  size_t index = region.m_implementation.size () - 1;
  if (region.m_implementation.size () <= index) {
      std::cout << "IdDictRange::generate_implementation: index >= impl size - "
		<< index << " " << region.m_implementation.size ()
		<< std::endl;
      return;
  }

  Range::field field;
  switch (m_specification) { 
  case by_value: 
  case by_label: 
      impl.set_field(Range::field(m_value, m_value)); 
      break; 
  case by_values: 
  case by_labels: 
  { 
      Range::field::element_vector v; 
 
      for (size_t i = 0; i < m_values.size (); ++i) 
	  { 
	      v.push_back (m_values[i]); 
	  } 
      field.set(v);
      impl.set_field(field); 
  } 
  break; 
  case by_minvalue: 
      field.set_minimum (m_minvalue); 
      impl.set_field(field); 
      break; 
  case by_maxvalue: 
      field.set_maximum (m_maxvalue); 
      impl.set_field(field); 
      break; 
  case by_minmax: 
      field.set (m_minvalue, m_maxvalue); 
      impl.set_field(field); 
      break; 
  case unknown: 
      break; 
  }
} 
  
void IdDictRange::reset_implementation () 
{} 

bool IdDictRange::verify () const 
{ 
  return (true); 
} 
 
Range IdDictRange::build_range () const 
{ 
  Range result; 
 
  Range::field field; 
 
  switch (m_specification) 
    { 
    case by_value: 
    case by_label: 
      field.set (m_value, m_value); 
//      std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//          " value=" << m_value << std::endl; 
      break; 
    case by_values: 
    case by_labels: 
      { 
        Range::field::element_vector v; 
 
        for (size_t i = 0; i < m_values.size (); ++i) 
          { 
            v.push_back (m_values[i]); 
//            std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//                " add value=" << m_values[i] << std::endl; 
          } 
 
        field.set (v); 
      } 
      break; 
    case by_minvalue: 
      field.set_minimum (m_minvalue); 
//      std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//          " minvalue=" << m_minvalue << std::endl; 
      break; 
    case by_maxvalue: 
      field.set_maximum (m_maxvalue); 
//      std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//          " maxvalue=" << m_maxvalue << std::endl; 
      break; 
    case by_minmax: 
      field.set (m_minvalue, m_maxvalue); 
//      std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//          " minvalue=" << m_minvalue << " maxvalue=" << m_maxvalue << std::endl; 
      break; 
    case unknown: 
//      std::cout << "IdDictRange::build_range> field=" << m_field_name <<  
//          " unknown" << std::endl; 
      break; 
    } 
 
//  std::cout << "IdDictRange::build_range> [" << (std::string) field << "]" << std::endl; 

  if (wrap_around == m_continuation_mode) {
      field.set(true);
  }
  else if (has_previous == m_continuation_mode) {
      field.set_previous(m_prev_value);
  }
  else if (has_next == m_continuation_mode) {
      field.set_next(m_next_value);
  }
  else if (has_both == m_continuation_mode) {
      field.set_previous(m_prev_value);
      field.set_next(m_next_value);
  }

//    std::cout << "IdDictRange::build_range> field=" << m_field_name 
//  	    << " wrap " << m_wraparound 
//  	    << " " << field.wrap_around() << std::endl; 

  result.add (std::move(field));
  return (result); 
} 
 
/** 
 * 
 */ 
IdDictRangeRef::IdDictRangeRef () 
    :
    m_range(0)
    //m_resolved_references(false),
    //m_generated_implementation(false)
{ 
} 
 
IdDictRangeRef::~IdDictRangeRef () 
{ 
} 
 
void IdDictRangeRef::resolve_references (const IdDictMgr& idd,  
					 IdDictDictionary& dictionary, 
					 IdDictRegion& region) 
{ 
    if (m_range) m_range->resolve_references (idd, dictionary, region);
} 
  
void IdDictRangeRef::generate_implementation (const IdDictMgr& idd,  
					      IdDictDictionary& dictionary, 
					      IdDictRegion& region,
					      std::string tag) 
{ 
    if (m_range) m_range->generate_implementation (idd, dictionary, region, tag);
} 
  
void IdDictRangeRef::reset_implementation () 
{ 
    if (m_range) m_range->reset_implementation ();
} 
  
bool IdDictRangeRef::verify () const 
{ 
    if (m_range) return (m_range->verify());
    return (true); 
} 
 
Range IdDictRangeRef::build_range () const 
{ 
  Range result; 
  if (m_range) result = m_range->build_range();
  return (result);
}
 
 
 
 
/** 
 * 
 */ 
IdDictReference::IdDictReference () 
    :
    m_subregion(0),
    m_resolved_references(false)
    //m_generated_implementation(false)
{ 
} 
 
IdDictReference::~IdDictReference () 
{ 
} 
 
void IdDictReference::resolve_references (const IdDictMgr& /*idd*/,  
					  IdDictDictionary& dictionary, 
					  IdDictRegion& /*region*/) 
{ 
    if(!m_resolved_references) {
	m_subregion = dictionary.find_subregion (m_subregion_name); 
	m_resolved_references = true;
    }
} 
  
void IdDictReference::generate_implementation (const IdDictMgr& idd,  
					       IdDictDictionary& dictionary, 
					       IdDictRegion& region,
					       std::string tag) 
{ 

  if (Debugger::debug ()) 
    { 
      Debugger::tab (); std::cout << "IdDictReference::generate_implementation>" << std::endl; 
    } 
  
  if (m_subregion != 0) m_subregion->generate_implementation (idd, dictionary, region, tag); 
} 
  
void IdDictReference::reset_implementation () 
{ 
  if (m_subregion != 0) m_subregion->reset_implementation (); 
} 
  
bool IdDictReference::verify () const 
{ 
  return (true); 
} 
 
Range IdDictReference::build_range () const 
{ 
  Range result; 
 
  result = m_subregion->build_range (); 
 
//  std::cout << "IdDictReference::build_range> [" << (std::string) result << "]" << std::endl; 
 
  return (result); 
} 
 
 
 
 
 
 
/** 
 * 
 */ 
IdDictDictionaryRef::IdDictDictionaryRef () 
    :
    m_dictionary (0),
    m_resolved_references(false),
    m_generated_implementation(false),
    m_propagated_information(false)
{ 
} 
 
IdDictDictionaryRef::~IdDictDictionaryRef () 
{ 
} 
 
void IdDictDictionaryRef::resolve_references (const IdDictMgr& idd,  
					      IdDictDictionary& dictionary, 
					      IdDictRegion& /*region*/) 
{ 
    if(!m_resolved_references) {
	m_dictionary = idd.find_dictionary (m_dictionary_name); 
	if(m_dictionary) {
	    m_dictionary->resolve_references (idd);  
	    m_dictionary->m_parent_dict = &dictionary;
	}
	m_resolved_references = true;
    }
} 
  
void IdDictDictionaryRef::generate_implementation (const IdDictMgr& idd,  
						   IdDictDictionary& dictionary, 
						   IdDictRegion& region,
						   std::string tag) 
{ 
    if(!m_generated_implementation) {
	if(m_dictionary) {
	    if (!m_propagated_information) {
	    
		// Propagate information to referenced dictionary:
		//
		// 1) Loop over ranges in this region and add them to the
		// referenced dictionary, then propagate the generate
		// implementation

		// 2) Duplicate the regions of the current dictionary in
		// referenced dictionary. Only the top level range(s) need
		// to be propagated to correctly calculate the bits of the
		// upper levels.

		// Save a vector of entries to prepend, inverting their order
		std::vector<IdDictRegionEntry*> prepend_entries;  
		std::vector<IdDictRegionEntry*>::iterator it;  
		for (it = region.m_entries.begin (); it != region.m_entries.end (); ++it) {  
		    IdDictRegionEntry* entry = *it;  
		    if(this == entry) break; // end when we get to the dictionary (this)
		    // If this is a range entry, add a duplicate to all
		    // regions in the subdictionary
		    IdDictRange* range = dynamic_cast<IdDictRange*> (entry);
		    if(range) {
			prepend_entries.insert(prepend_entries.begin(), entry); 
		    }
		}
	    


//  		std::cout << "nreg " << m_dictionary->m_all_regions.size() << " "
//  			  << m_dictionary->m_regions.size() << " " 
//  			  << m_dictionary->m_entries.size() << " " << std::endl;
		

		// Now prepend list to each region and generate each region
		IdDictDictionary::regions_it it2; 
		for (it2 = m_dictionary->m_all_regions.begin (); it2 != m_dictionary->m_all_regions.end (); ++it2) {  
		    IdDictRegion& region2 = *(*it2);
		    for (it = prepend_entries.begin(); it != prepend_entries.end(); ++it) {
			IdDictRegionEntry* entry = *it;  
			IdDictRange* range = dynamic_cast<IdDictRange*> (entry);
			if(range) {
			    IdDictRangeRef* new_range = new IdDictRangeRef;
			    new_range->m_range = range;
			    region2.m_entries.insert(region2.m_entries.begin(), new_range);
			}  
		    }
		} 

		// Now copy all prefixes into new regions in the
		// referenced dictionary
		if (prepend_entries.size() > 0) {

		    // Save region number
		    const IdDictRegion& region2 = *m_dictionary->m_all_regions.back();
		    size_t region_number = region2.m_index + 1;

		    IdDictDictionary::regions_it it; 
  
		    // Loop over all regions of current dict, add to ref dict (m_dictionary)
		    for (it = dictionary.m_all_regions.begin (); it != dictionary.m_all_regions.end (); ++it, ++region_number) { 
			IdDictRegion& region3 = *(*it);
			IdDictRegion* new_region = new IdDictRegion;  
			new_region->m_name  = "dummy";
			new_region->m_group = "dummy";
			new_region->m_index = region_number;

			// to all region vectors
			m_dictionary->m_all_regions.push_back(new_region); 
			// to the entries of the dictionary
			m_dictionary->add_dictentry(new_region); 
			
			// Now add in only the ranges
			std::vector<IdDictRegionEntry*>::iterator it;  
			size_t i = 0;
			for (it = region3.m_entries.begin (); it != region3.m_entries.end (); ++it, ++i) {  
			    if (i >= prepend_entries.size()) continue;
			
			    IdDictRegionEntry* entry = *it;  
			    IdDictRange* range = dynamic_cast<IdDictRange*> (entry);
			    if(range) {
				IdDictRangeRef* new_range = new IdDictRangeRef;
				new_range->m_range = range;
				new_region->m_entries.push_back(new_range);
			    }
			}
		    }
		}
		m_propagated_information   = true;
	    }
	    m_dictionary->generate_implementation (idd, tag);  
	    m_generated_implementation = true;
	}
	else {
              std::cout << "IdDictDictionaryRef::generate_implementation: - WARNING no dictionary found, cannot generate implementation "
                        << std::endl; 
	}
    }
} 

void IdDictDictionaryRef::reset_implementation () 
{
    if (m_generated_implementation) {
	m_dictionary->reset_implementation ();  
	m_generated_implementation = false;
    }
} 
  
bool IdDictDictionaryRef::verify () const 
{ 
  return (true); 
} 
 
Range IdDictDictionaryRef::build_range () const 
{ 
  Range result; 
 
//  std::cout << "IdDictDictionaryRef::build_range> []" << std::endl; 
 
  return (result); 
} 
 
 
/* 
int IdDictSet::type_id () const  
{  
  return ((int) TypeId <IdDictSet> ());  
}  
*/ 
 
  
