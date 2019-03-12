/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDDICT_IDDICTDEFS_H
#define IDDICT_IDDICTDEFS_H
 
#include "Identifier/Range.h" 
#include "Identifier/Identifier.h" 
#include "IdDict/IdDictFieldImplementation.h"

#include <vector>  
#include <set>  
#include <string>
#include <map>
  
class IdDictMgr;  
class IdDictDictionary; 
class IdDictField;  
class IdDictLabel;  
class IdDictDictEntry; 
class IdDictGroup;
class IdDictAltRegions; 
class IdDictRegion; 
class IdDictRegionEntry; 
class IdDictSubRegion;  
class IdDictRange; 
class IdDictReference; 
class IdDictDictionaryRef; 
  
class IdDictMgr  
{  
public:  
    typedef std::map<std::string, IdDictDictionary*> dictionary_map; 
    typedef std::map<std::string, std::string>       metadata_map; 

    IdDictMgr();
    ~IdDictMgr();
 
    /// Version tag
    const std::string&    tag                     () const;

    /// Access dictionary by name
    IdDictDictionary*     find_dictionary         (const std::string& name) const;  

    /// Access to all dictionaries
    const dictionary_map& get_dictionary_map      () const;  

    /// DTD version
    const std::string&    DTD_version             () const;

    /// Check whether or not to do checks for ids
    bool                  do_checks               () const;

    /// Check whether or not to init neighbours
    bool                  do_neighbours           () const;

    /// Access to meta data, name/value pairs
    const std::string&    find_metadata           (const std::string& name) const;

    ///  Fillers:
    void                  add_dictionary          (IdDictDictionary* dictionary);  
    void                  add_subdictionary_name  (const std::string& name);  
    void                  add_metadata            (const std::string& name, const std::string& value);
    void                  set_DTD_version         (const std::string& DTD_version);
    void                  set_do_checks           (bool do_checks);
    void                  set_do_neighbours       (bool do_neighbours);
    
    
    ///  Construct dictionary after parsing
    void                  resolve_references      ();  
    void                  generate_implementation (std::string tag = "");  

    ///  Reset of implementation
    void                  reset_implementation    ();  
    void                  clear                   (); 
    
    bool                  verify                  () const;  
    
 
private:

    void                  find_subdicts(IdDictDictionary* dict);

    std::string           m_DTD_version;  
    std::string           m_tag; 
    dictionary_map        m_dictionaries; 
    metadata_map          m_metadata;
    std::set<std::string> m_subdictionary_names; 
    bool                  m_resolved_references;
    bool                  m_generated_implementation;
    mutable bool          m_do_checks;
    mutable bool          m_do_neighbours;
};  
  
class IdDictDictionary  
{  
public:  
    typedef Identifier::value_type value_type;
    typedef Identifier::size_type size_type; 
 
    IdDictDictionary ();  
    ~IdDictDictionary ();  
  
    IdDictField* find_field (const std::string& name) const;  
    IdDictLabel* find_label (const std::string& field, const std::string& label) const;
    int get_label_value (const std::string& field, const std::string& label, int& value) const;  // > 0 == error
    IdDictSubRegion* find_subregion (const std::string& subregion_name) const;  
    IdDictRegion* find_region (const std::string& region_name) const;  
    IdDictRegion* find_region (const std::string& region_name, const std::string& group_name) const;  
    IdDictGroup* find_group (const std::string& group_name) const;


    void add_field (IdDictField* field);  
    void add_subregion (IdDictSubRegion* subregion);  
    void add_dictentry (IdDictDictEntry* entry);
    void add_subdictionary_name (const std::string& name);  
    void resolve_references (const IdDictMgr& idd);  
    void generate_implementation (const IdDictMgr& idd, std::string tag = "");  
    void reset_implementation ();  
    bool verify   () const;
    void sort     ();  
    void clear    (); 


    /// Find first region that matches id
    int find_region(const ExpandedIdentifier& id, size_type& index) const;
    IdDictRegion* find_region(const ExpandedIdentifier& id) const;
    IdDictRegion* find_region(const ExpandedIdentifier& id,const std::string& group_name) const;
    
    ///  Set up integral of bits for efficient unpacking
    void integrate_bits ();

    ///  Get MultiRange for full dictionary
    MultiRange build_multirange () const; 

    /**
     *   Get MultiRange for a specific region_id up to and including
     *   'last_field' If last_field == "", all fields are
     * taken. Prepend prefix if provided.  */
    MultiRange build_multirange (const ExpandedIdentifier& region_id,
                                 const Range& prefix = Range(),
                                 const std::string& last_field = "") const; 

    /**
     *   Get MultiRange for a specific region_id, constrained to be
     *   within a specific named group. The depth of the multirange is
     *   up to and including 'last_field' If last_field == "", all
     *   fields are taken. Prepend prefix if provided.  */

    MultiRange build_multirange (const ExpandedIdentifier& region_id,
                                 const std::string& group_name,
                                 const Range& prefix = Range(),
                                 const std::string& last_field = "") const; 
 
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
    int pack32 (const ExpandedIdentifier& id, 
                size_t index1, 
                size_t index2,
                Identifier& packedId) const; 
 
    /** 
     *   Pack to 32bits the subset of id between (inclusive) index1
     *   and index2. Here one must provide the region index, which can
     *   be found with find_region. This is more efficient than the
     *   above pack32.
     *
     *   Assumptions: (different than the previous pack32)
     *     - packedId is NOT initialized to 0 - allows to just
     *       overwrite the specified fields
     *     - the field array begins at index1, i.e. NOT the first field
     *     - the field array field array has a length of index2-index1+1 
     *     - index1,2 correspond to the level/field number
     *     - first_field_index defines the offset for
     *       packing. Normally, this is this is 0. But this may be a
     *       field > 0, but of course <= index1
     *
     * */
    int pack32 (const int* fields, 
                size_t index1, 
                size_t index2,
                size_t region_index,
                Identifier& packedId,
                size_t first_field_index=0) const; 
 
    /** 
     *   Reset fields from index1 to index2.
     * */
    int reset (size_t index1, 
               size_t index2,
               size_t region_index,
               Identifier& packedId) const; 
 


    /** 
     *  Unpack the value_type id to an expanded Identifier, considering
     *  the provided prefix (result will include the prefix) and up to
     *  index2 - (index1 is assumed to be 0, i.e. part of prefix). 
     */
    int unpack (const Identifier& id, 
                const ExpandedIdentifier& prefix,
                size_t index2,
                ExpandedIdentifier& unpackedId) const; 
   
    /** 
     *  Unpack the value_type id to a string, considering the provided
     *  prefix (result will include the prefix) and up to index2 -
     *  (index1 is assumed to be 0, i.e. part of prefix).
     */
    int unpack (const Identifier& id, 
                const ExpandedIdentifier& prefix,
                size_t index2,
                const std::string& sep,
                std::string& unpackedId) const; 
   
    /** 
     **  Unpack a single field of the value_type id. One specifies the
     **  field index desired and passes in the region index to be used
     **  for decoding. The region index is obtained from
     **  find_region. The first_field_index is normally 0. It is
     **  non-zero when fields 0 to first_field_index -1 are missing
     **  from the compact 
     */
    int unpack (const Identifier& id, 
                size_t first_field_index, 
                size_t field_index,
                size_t region_index,
                int& field) const; 
 
   
    /** 
     **  Copy a number of fields of the value_type id into another value_type
     **  id. One specifies the begin and end (inclusive) field indexes
     **  desired and passes in the region index to be used for
     **  decoding. The region index is obtained from find_region.  The
     **  first_field_index is normally 0. It is non-zero when fields 0
     **  to first_field_index -1 are missing from the compact 
     */
    int copy (const Identifier& idin, 
              size_t first_field_index, 
              size_t begin_field_index,
              size_t end_field_index,
              size_t region_index,
              Identifier& idout) const; 
 
    /// Checks are performed by default in debug compilation and NOT
    /// in optimized compilation. One can switch or query this mode for
    /// any idHelper with the following methods:
    bool                do_checks       (void) const;
    void                set_do_checks   (bool do_checks);

    /// Neighbour initialization is performed by default
    /// One can switch or query this mode for
    /// any idHelper with the following methods:
    bool                do_neighbours           (void) const;
    void                set_do_neighbours       (bool do_neighbours);

    /// Access to file name
    std::string         file_name        (void) const;
    
    /// Access to the dictionary tag
    std::string         dict_tag         (void) const; 

    /// Set file name
    void                set_file_name    (const std::string& name);
   
    /// Set the dictionary tag
    void                set_dict_tag     (const std::string& tag);


    std::string m_name;  
    std::string m_version;  
    std::string m_date;  
    std::string m_author;  
  
    typedef std::vector<IdDictDictEntry*> entries_type;
    typedef entries_type::iterator        entries_it;
    typedef entries_type::const_iterator  entries_const_it;

    typedef std::vector<IdDictRegion*>    regions_type;
    typedef regions_type::iterator        regions_it;
    typedef regions_type::const_iterator  regions_const_it;

    typedef std::vector<IdDictGroup*>     groups_type;
    typedef groups_type::iterator         groups_it;
    typedef groups_type::const_iterator   groups_const_it;

    std::map<std::string, IdDictField*>   m_fields;  
    std::map<std::string, IdDictSubRegion*> m_subregions;  
    std::vector<IdDictRegion*>            m_regions;  // corresponding regions for REs
    std::vector<IdDictRegion*>            m_all_regions;  // all regions
    std::vector<IdDictGroup*>             m_groups;    
    std::vector<std::string>              m_subdictionary_names; 
    IdDictDictionary*                     m_parent_dict;

private:
    std::string m_file_name;  
    std::string m_dict_tag;  
    //bool m_resolved_references;
    bool m_generated_implementation;
    mutable bool  m_do_checks;
    mutable bool  m_do_neighbours;
};  
  
class IdDictField 
{  
public:  
    IdDictField ();  
    virtual ~IdDictField ();  
  
    IdDictLabel* find_label (const std::string& name) const;  
    void add_label (IdDictLabel* label);  
    size_t get_label_number () const;  
    const std::string& get_label (size_t index) const;  
    ExpandedIdentifier::element_type get_label_value (const std::string& name) const; 
    void resolve_references (const IdDictMgr& idd);  
    void generate_implementation (const IdDictMgr& idd, std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
    void clear (); 
    
    std::string                   m_name;  
    std::vector <IdDictLabel*>    m_labels; 
    size_t                        m_index; 

private:
    //bool m_resolved_references;
    //bool m_generated_implementation;
};  
  
class IdDictLabel 
{  
public:  
    IdDictLabel (); 
    ~IdDictLabel (); 
  
    std::string m_name;  
    bool m_valued;  
    int m_value;  
};  
 
class IdDictGroup
{ 
public: 
    IdDictGroup (); 
    IdDictGroup (const std::string& name); 
    virtual ~IdDictGroup (); 

    std::string  name();
    const std::vector<IdDictDictEntry*>& entries();
    const std::vector<IdDictRegion*>&    regions();

    ///  Get MultiRange for this group
    MultiRange build_multirange () const; 

    void add_dictentry (IdDictDictEntry* entry);

    void resolve_references (const IdDictMgr& idd,  
                             IdDictDictionary& dictionary,
                             size_t& index);  
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  std::string tag = "");
    void reset_implementation ();  

    bool verify () const;
    void sort   ();  
    void clear  (); 


private:

    std::string                   m_name;  
    std::vector<IdDictDictEntry*> m_entries;  // just the RegionEntries
    std::vector<IdDictRegion*>    m_regions;  // regions derived from entries
    //bool m_resolved_references;
    bool m_generated_implementation;
}; 
 
class IdDictDictEntry 
{ 
public: 
    IdDictDictEntry (); 
    virtual ~IdDictDictEntry (); 
 
    virtual Range build_range () const = 0; 
    
    virtual std::string group_name () const = 0; 
    
    virtual void set_index (size_t index) = 0;

    virtual void resolve_references (const IdDictMgr& idd,  
                                     IdDictDictionary& dictionary) = 0;  
    virtual void generate_implementation (const IdDictMgr& idd,  
                                          IdDictDictionary& dictionary, 
                                          std::string tag = "") = 0;
    virtual void reset_implementation () = 0;  
    virtual bool verify () const = 0;
 
    virtual void clear () = 0; 
 
}; 
 
class IdDictAltRegions : public IdDictDictEntry
{ 
public: 
    IdDictAltRegions (); 
    ~IdDictAltRegions (); 
 
    std::string group_name () const; 

    Range build_range () const; 

    void set_index (size_t index);

    void resolve_references (const IdDictMgr& idd, 
                             IdDictDictionary& dictionary);
    void generate_implementation (const IdDictMgr& idd, 
                                  IdDictDictionary& dictionary, 
                                  std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
 
    void clear (); 
    
    typedef std::map<std::string, IdDictRegion* > map_type;
    typedef map_type::iterator                    map_iterator;
    typedef map_type::value_type                  value_type;
    map_type      m_regions;
    IdDictRegion* m_selected_region;

}; 


class IdDictRegion : public IdDictDictEntry
{ 
public: 
    IdDictRegion (); 
    ~IdDictRegion (); 
 
    std::string group_name () const; 

    Range build_range () const; 

    void set_index (size_t index);
 
    void add_entry (IdDictRegionEntry* entry); 
  
    void resolve_references (const IdDictMgr& idd, 
                             IdDictDictionary& dictionary); 
    void generate_implementation (const IdDictMgr& idd, 
                                  IdDictDictionary& dictionary, 
                                  std::string tag = "");  
    void find_neighbours (const IdDictDictionary& dictionary);
    void reset_implementation ();  
    bool verify () const;  
 
    void clear (); 
    
 
    std::vector <IdDictRegionEntry*>        m_entries; 
    std::vector <IdDictFieldImplementation> m_implementation; 
    size_t                                  m_index; 
    std::string                             m_name; 
    std::string                             m_group; 
    std::string                             m_tag; 
    bool                                    m_is_empty;
    std::string                             m_next_abs_eta_name;  
    std::vector<std::string>                m_prev_samp_names;
    std::vector<std::string>                m_next_samp_names;
    std::vector<std::string>                m_prev_subdet_names;
    std::vector<std::string>                m_next_subdet_names;
    IdDictRegion*                           m_prev_abs_eta;
    IdDictRegion*                           m_next_abs_eta;
    std::vector<IdDictRegion*>              m_prev_samp;
    std::vector<IdDictRegion*>              m_next_samp;
    std::vector<IdDictRegion*>              m_prev_subdet;
    std::vector<IdDictRegion*>              m_next_subdet;
    float                                   m_eta0;
    float                                   m_deta;
    float                                   m_phi0;
    float                                   m_dphi;
    

protected:
    bool m_resolved_references;
    bool m_generated_implementation;
}; 
 
class IdDictSubRegion : public IdDictRegion 
{ 
public: 
    IdDictSubRegion (); 
    ~IdDictSubRegion (); 

    // Dummy: never called - subregions are only generated via
    // IdDictReference where one already has a region which is passed in
    // as an arg with the method below
    void generate_implementation (const IdDictMgr& idd, 
                                  IdDictDictionary& dictionary, 
                                  std::string tag = "");  
    // Generate implementation - pass region to be filled for this subregion
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  IdDictRegion& region,
                                  std::string tag = "");  
    void reset_implementation ();  
}; 
 
class IdDictRegionEntry 
{ 
public: 
    IdDictRegionEntry (); 
    virtual ~IdDictRegionEntry (); 
 
    virtual void resolve_references (const IdDictMgr& idd,  
                                     IdDictDictionary& dictionary, 
                                     IdDictRegion& region);  
    virtual void generate_implementation (const IdDictMgr& idd,  
                                          IdDictDictionary& dictionary, 
                                          IdDictRegion& region,
                                          std::string tag = "");
    virtual void reset_implementation ();  
    virtual bool verify () const;  
 
    virtual void clear (); 
 
    virtual Range build_range () const = 0; 
    
}; 
 
class IdDictRange : public IdDictRegionEntry 
{ 
public: 
    IdDictRange (); 
    ~IdDictRange (); 
 
    void resolve_references (const IdDictMgr& idd,  
                             IdDictDictionary& dictionary, 
                             IdDictRegion& region);  
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  IdDictRegion& region,
                                  std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
 
    Range build_range () const; 

    std::string m_field_name; 
    IdDictField* m_field; 
 
    typedef enum 
    { 
        unknown, 
        by_value, 
        by_values, 
        by_label, 
        by_labels, 
        by_minvalue, 
        by_maxvalue, 
        by_minmax 
    } specification_type; 
 
    typedef enum 
    { 
        none, 
        has_next, 
        has_previous, 
        has_both,
        wrap_around
    } continuation_mode; 
 
    specification_type m_specification; 
 
    std::string m_tag; 
    std::string m_label; 
    int m_value; 
    int m_minvalue; 
    int m_maxvalue;
    int m_prev_value;
    int m_next_value;
    continuation_mode m_continuation_mode;
 
    std::vector <std::string> m_labels; 
    std::vector <int> m_values; 

private:
    bool m_resolved_references;
    //bool m_generated_implementation;
}; 
 
class IdDictRangeRef : public IdDictRegionEntry 
{ 
public: 
    IdDictRangeRef (); 
    ~IdDictRangeRef (); 
 
    void resolve_references (const IdDictMgr& idd,  
                             IdDictDictionary& dictionary, 
                             IdDictRegion& region);  
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  IdDictRegion& region,
                                  std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
 
    Range build_range () const; 
 
    IdDictRange* m_range; 

private:
    //bool m_resolved_references;
    //bool m_generated_implementation; 
}; 
  
class IdDictReference : public IdDictRegionEntry 
{ 
public: 
    IdDictReference (); 
    ~IdDictReference (); 
 
    void resolve_references (const IdDictMgr& idd,  
                             IdDictDictionary& dictionary, 
                             IdDictRegion& region);  
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  IdDictRegion& region,
                                  std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
 
    Range build_range () const; 
 
    std::string m_subregion_name; 
    IdDictSubRegion* m_subregion; 

private:
    bool m_resolved_references;
    //bool m_generated_implementation;
}; 
 
class IdDictDictionaryRef : public IdDictRegionEntry 
{ 
public: 
    IdDictDictionaryRef (); 
    ~IdDictDictionaryRef (); 
 
    void resolve_references (const IdDictMgr& idd,  
                             IdDictDictionary& dictionary, 
                             IdDictRegion& region);  
    void generate_implementation (const IdDictMgr& idd,  
                                  IdDictDictionary& dictionary, 
                                  IdDictRegion& region,
                                  std::string tag = "");  
    void reset_implementation ();  
    bool verify () const;  
 
    Range build_range () const; 
 
    std::string m_dictionary_name; 
    IdDictDictionary* m_dictionary; 

private:
    // We allow to regenerate the implementation with a tag. However,
    // propagation of information should only be done once.
    bool m_resolved_references;
    bool m_generated_implementation;
    bool m_propagated_information;
}; 


//-------------------
// inline definitions
//-------------------
 
/// Access to file name
inline std::string         
IdDictDictionary::file_name        (void) const
{
    return (m_file_name);
}

    
/// Access to the dictionary tag
inline std::string         
IdDictDictionary::dict_tag         (void) const
{
    return (m_dict_tag);
}


/// Set file name
inline void                
IdDictDictionary::set_file_name    (const std::string& name)
{
    m_file_name = name;
}

   
/// Set the dictionary tag
inline void                
IdDictDictionary::set_dict_tag     (const std::string& tag)
{
    m_dict_tag = tag;
}


#endif  
  
