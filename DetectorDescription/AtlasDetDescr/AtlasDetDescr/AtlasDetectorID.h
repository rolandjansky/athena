/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATLASDETDESCR_ATLASDETECTORID_H
#define ATLASDETDESCR_ATLASDETECTORID_H
/**
 * @file AtlasDetectorID.h
 *
 * @brief This class provides an interface to generate or decode an
 *   identifier for the upper levels of the detector element
 *   hierarchy, i.e. Atlas, the detector systems and detector
 *   subsystems. As well general "print out" methods are provide for
 *   all detector systems. All id helper classes derive from this
 *   class. This class also provides access to the version tags of the
 *   id dictionaries used by the helper.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AtlasDetectorID.h,v 1.38 2007-02-23 09:33:30 prieur Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "Identifier/IdHelper.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AtlasDetTechnology.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictDictionary;
class AtlasDetectorIDHelper;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 * @class AtlasDetectorID
 *   
 * @brief This class provides an interface to generate or decode an
 *   identifier for the upper levels of the detector element
 *   hierarchy, i.e. Atlas, the detector systems and detector
 *   subsystems. As well general "print out" methods are provide for
 *   all detector systems. All id helper classes derive from this
 *   class. This class also provides access to the version tags of the
 *   id dictionaries used by the helper.
 **/

class AtlasDetectorID : public IdHelper
{
public:
    
    /// @name strutors
    //@{
    AtlasDetectorID();
    AtlasDetectorID(const AtlasDetectorID& other);
    ~AtlasDetectorID(void);
    AtlasDetectorID& operator= (const AtlasDetectorID& other);
    //@}
    
    /// @name Detector system ids
    //@{
    /// Detector systems:
    Identifier          indet           (void) const;
    Identifier          lar             (void) const;
    Identifier          tile            (void) const;
    Identifier          muon            (void) const;
    Identifier          calo            (void) const;
    //@}

    /// @name Inner Detector subsystem ids
    //@{
    Identifier          pixel           (void) const;
    Identifier          sct             (void) const;
    Identifier          trt             (void) const;
    //@}

    /// @name LAr subsystem ids
    //@{
    Identifier          lar_em          (void) const;
    Identifier          lar_hec         (void) const;
    Identifier          lar_fcal        (void) const;
    //@}

    /// @name Muon subsystem ids
    //@{
    Identifier          mdt             (void) const;
    Identifier          csc             (void) const;
    Identifier          rpc             (void) const;
    Identifier          tgc             (void) const;
    Identifier          stgc            (void) const;
    Identifier          mm              (void) const;
    //@}

    /// @name level 1 and dead material ids
    //@{
    Identifier          lar_lvl1        (void) const;
    Identifier          lar_dm          (void) const;
    Identifier          tile_dm         (void) const;
    //@}

    /// @name Contexts to provide id length - e.g. for use in generic decoding
    //@{
    /// IdContext (indicates id length) for detector systems
    IdContext           detsystem_context (void) const;

    /// IdContext (indicates id length) for sub-detector
    IdContext           subdet_context  (void) const;
    //@}


    /// @name Generic conversion between identifier and idhash
    //@{
    /// Create compact id from hash id (return == 0 for OK)
    virtual int         get_id          (const IdentifierHash& hash_id,
                                         Identifier& id,
                                         const IdContext* context = 0) const;
    
    /// Create hash id from compact id (return == 0 for OK)
    virtual int         get_hash        (const Identifier& id, 
                                         IdentifierHash& hash_id,
                                         const IdContext* context = 0) const;
    //@}

    /// @name Initialization and version name 
    //@{
    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    // retrieve version of the dictionary
    virtual std::string   dictionaryVersion  (void) const;
    //@}


    /// @name Generic printing of identifiers
    //@{
    /// Short print out of any identifier (optionally provide
    /// separation character - default is '.'):
    void                show            (Identifier id, 
                                         const IdContext* context = 0,
                                         char sep = '.' ) const;
    /// or provide the printout in string form
    std::string         show_to_string  (Identifier id, 
                                         const IdContext* context = 0,
                                         char sep = '.'  ) const;

    /// Expanded print out of any identifier
    void                print           (Identifier id,
                                         const IdContext* context = 0) const;
    /// or provide the printout in string form
    std::string         print_to_string (Identifier id,
                                         const IdContext* context = 0) const;
    //@}

    /// @name  Test of an Identifier to see if it belongs to a particular detector (sub)system:
    //@{
    bool                is_indet        (Identifier id) const;
    bool                is_lar          (Identifier id) const;
    bool                is_tile         (Identifier id) const;
    bool                is_muon         (Identifier id) const;
    bool                is_calo         (Identifier id) const;
    bool                is_lvl1_trig_towers(Identifier id) const;
    bool                is_lvl1_online(Identifier id) const;
    bool                is_pixel        (Identifier id) const;
    bool                is_sct          (Identifier id) const;
    bool                is_trt          (Identifier id) const;
    bool                is_lar_em       (Identifier id) const;
    bool                is_lar_hec      (Identifier id) const;
    bool                is_lar_fcal     (Identifier id) const;
    bool                is_lar_minifcal (Identifier id) const;
    /// LAr/Tile dead material:
    bool                is_lar_dm       (Identifier id) const; 
    bool                is_tile_dm      (Identifier id) const; 
    bool                is_mdt          (Identifier id) const;
    bool                is_csc          (Identifier id) const;
    bool                is_tgc          (Identifier id) const;
    bool                is_rpc          (Identifier id) const;
    bool                is_stgc         (Identifier id) const;
    bool                is_mm           (Identifier id) const;
    bool                is_forward      (Identifier id) const;
    bool                is_alfa         (Identifier id) const;
    bool                is_bcm          (Identifier id) const;
    bool                is_lucid        (Identifier id) const;
    bool                is_zdc          (Identifier id) const;
    //@}


    /// @name  Test of an Identifier to see if it belongs to a particular detector (sub)system (using expanded ids):
    //@{
    bool                is_indet        (const ExpandedIdentifier& id) const;
    bool                is_lar          (const ExpandedIdentifier& id) const;
    bool                is_tile         (const ExpandedIdentifier& id) const;
    bool                is_muon         (const ExpandedIdentifier& id) const;
    bool                is_calo         (const ExpandedIdentifier& id) const;
    bool                is_pixel        (const ExpandedIdentifier& id) const;
    bool                is_sct          (const ExpandedIdentifier& id) const;
    bool                is_trt          (const ExpandedIdentifier& id) const;
    bool                is_lar_em       (const ExpandedIdentifier& id) const;
    bool                is_lar_hec      (const ExpandedIdentifier& id) const;
    bool                is_lar_fcal     (const ExpandedIdentifier& id) const;
    bool                is_lar_minifcal (const ExpandedIdentifier& id) const;
    bool                is_mdt          (const ExpandedIdentifier& id) const;
    bool                is_csc          (const ExpandedIdentifier& id) const;
    bool                is_tgc          (const ExpandedIdentifier& id) const;
    bool                is_rpc          (const ExpandedIdentifier& id) const;
    bool                is_stgc         (const ExpandedIdentifier& id) const;
    bool                is_mm           (const ExpandedIdentifier& id) const;
    //@}

    /// @name  Dictionary versioning: provide access to dictionary names and versions. Note that a helper may correspond to one or more id dictionary
    //@{
    ///  Dictionary names
    std::vector<std::string>  dict_names(void) const;
    ///  File names for subdet dictionaries
    std::vector<std::string>  file_names(void) const;
    ///  Version tags for subdet dictionaries
    std::vector<std::string>  dict_tags (void) const;
    //@}

    /// @name  Checks are performed by default in debug compilation and NOT in optimized compilation. One can switch or query this mode for any idHelper with the following methods:
    //@{
    virtual bool        do_checks       (void) const;
    virtual void        set_do_checks   (bool do_checks);
    //@}

    /// @name neighbours are initialized by default. One can switch or query this mode with the following methods:
    //@{
    virtual bool        do_neighbours       (void) const;
    virtual void        set_do_neighbours   (bool do_neighbours);
    //@}

    /// @name setting pointer to the MessageService
    //@{
    virtual void setMessageSvc  (IMessageSvc* msgSvc);
    //@}

    /// Set flag for suppressing informational output.
    void set_quiet (bool quiet);


protected:

    friend class AtlasDetectorIDHelper;

    void                setDictVersion  (const IdDictMgr& dict_mgr, const std::string& name);

    std::string         to_range        (const ExpandedIdentifier& id) const;
    
    std::string         fix_barrel_ec   (const std::string& barrel_ec) const;

    /// Detector systems:
    ExpandedIdentifier          indet_exp           (void) const;
    ExpandedIdentifier          lar_exp             (void) const;
    ExpandedIdentifier          tile_exp            (void) const;
    ExpandedIdentifier          muon_exp            (void) const;
    ExpandedIdentifier          calo_exp            (void) const;
    ExpandedIdentifier          fwd_exp             (void) const;

    /// Inner Detector:
    ExpandedIdentifier          pixel_exp           (void) const;
    ExpandedIdentifier          sct_exp             (void) const;
    ExpandedIdentifier          trt_exp             (void) const;

    /// LAr 
    ExpandedIdentifier          lar_em_exp          (void) const;
    ExpandedIdentifier          lar_hec_exp         (void) const;
    ExpandedIdentifier          lar_fcal_exp        (void) const;

    /// Forward
    ExpandedIdentifier          alfa_exp            (void) const;
    ExpandedIdentifier          bcm_exp             (void) const;
    ExpandedIdentifier          lucid_exp           (void) const;
    ExpandedIdentifier          zdc_exp             (void) const;

    /// Provide efficient access to individual field values, for
    /// subclass idhelpers
    int                 indet_field_value        () const;     
    int                 lar_field_value          () const;       
    int                 tile_field_value         () const;       
    int                 muon_field_value         () const;       
    int                 calo_field_value         () const;
    int                 pixel_field_value        () const;     
    int                 sct_field_value          () const;       
    int                 trt_field_value          () const;       
    int                 lar_em_field_value       () const;  
    int                 lar_hec_field_value      () const; 
    int                 lar_fcal_field_value     () const;
    int                 mdt_field_value          () const;
    int                 csc_field_value          () const;
    int                 rpc_field_value          () const;
    int                 tgc_field_value          () const;
    int                 stgc_field_value         () const;
    int                 mm_field_value           () const;
    int                 forward_field_value      () const;
    int                 alfa_field_value         () const;
    int                 bcm_field_value          () const;
    int                 lucid_field_value        () const;
    int                 zdc_field_value          () const;

    /// Register the file and tag names for a particular IdDict
    /// dictionary
    int                 register_dict_tag        (const IdDictMgr& dict_mgr,
                                                  const std::string& dict_name);

    /// Test whether an idhelper should be reinitialized based on the
    /// change of tags 
    bool                reinitialize             (const IdDictMgr& dict_mgr);

    /// Flag for subclasses to know whether or not to perform
    /// checks. In general, this is set to false in optimized mode.
    bool        m_do_checks;
    /// Flag for subclasses to know whether or not to perform
    /// neighbour initialization
    bool        m_do_neighbours;

    /// pointer to the message service
    IMessageSvc*        m_msgSvc;

    /// If true, suppress DEBUG/INFO messages.
    bool m_quiet;


    /// List of dictionary names used by this helper
    std::vector<std::string>  m_dict_names;

    /// List of dictionary file names used by this helper
    std::vector<std::string>  m_file_names;

    /// List of dictionary versions used by this helper
    std::vector<std::string>  m_dict_tags;
private:

    typedef Identifier::size_type                       size_type; 
    
    // Identifiear numbering:
    //   Temporary fixes - set LAR and MUON ids to single level 
    enum        ID   { ATLAS_ID		= static_cast<Identifier::value_type>(0),
		       MAX_BIT 		= Identifier::MAX_BIT,
		       // mask to distinguish a pixel id from lvl1 id (bit 1 set)
		       PIXEL_MASK	= static_cast<Identifier::value_type>(0xFF),  
		       ALL_BITS		= Identifier::ALL_BITS };

    int                 initLevelsFromDict(const IdDictMgr& dict_mgr);

    // dictionary version
    std::string         m_dict_version;
    bool                m_is_initialized_from_dict;
    size_type           m_DET_INDEX;
    size_type           m_SUBDET_INDEX;
    size_type           m_MUON_SUBDET_INDEX;
    int                 m_INDET_ID;     
    int                 m_LAR_ID;       
    int                 m_TILE_ID;      
    int                 m_MUON_ID;      
    int                 m_CALO_ID;
    int                 m_PIXEL_ID;     
    int                 m_SCT_ID;       
    int                 m_TRT_ID;       
    int                 m_LAR_EM_ID;  
    int                 m_LAR_HEC_ID; 
    int                 m_LAR_FCAL_ID;
    int                 m_LAR_FCAL_MODULE_INDEX;
    int                 m_MDT_ID;
    int                 m_CSC_ID;
    int                 m_RPC_ID;
    int                 m_TGC_ID;
    int                 m_STGC_ID;
    int                 m_MM_ID;
    int                 m_FWD_ID;
    int                 m_ALFA_ID;
    int                 m_BCM_ID;
    int                 m_LUCID_ID;
    int                 m_ZDC_ID;

    /// Flag for slhc layout:
    bool                m_isSLHC;

    Range::field        m_lvl1_field;
    Range::field        m_lvl1_onl_field;
    Range::field        m_lar_dm_field;
    Range::field        m_tile_dm_field;
    IdDictDictionary*   m_atlas_dict;
    IdDictDictionary*   m_indet_dict;
    IdDictDictionary*   m_lar_dict;
    IdDictDictionary*   m_tile_dict;
    IdDictDictionary*   m_muon_dict;
    IdDictDictionary*   m_calo_dict;
    IdDictDictionary*   m_fwd_dict;
    AtlasDetectorIDHelper* m_helper;
    IdDictFieldImplementation m_det_impl;
    IdDictFieldImplementation m_indet_part_impl;
    IdDictFieldImplementation m_calo_side_impl;
    IdDictFieldImplementation m_lar_part_impl;
    IdDictFieldImplementation m_lar_fcal_module_impl;
    IdDictFieldImplementation m_muon_station_name_impl;
    IdDictFieldImplementation m_muon_mdt_impl;
    IdDictFieldImplementation m_muon_rpc_impl;
    IdDictFieldImplementation m_fwd_part_impl;

    // vector used to store info to distinguish between muon technologies.
    std::vector<unsigned int> m_muon_tech_bits;

    /* removed by Philipp */
    /* // enum defining the different muon technologies (MDT and RPC are */
    /* // distinguished using the technology bit inside the identifiers) */
    /* enum MuonTechBits */
    /* { */
    /*   unknown = 0, */
    /*   isMdtOrRpc = 1, */
    /*   isCsc = 2, */
    /*   isTgc = 3, */
    /*   isMm = 4, */
    /*   isStgc = 5 */
    /* }; */
    /* removed by Philipp */

};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(AtlasDetectorID, 164875623, 1)

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

inline ExpandedIdentifier          
AtlasDetectorID::indet_exp           (void) const
{
    ExpandedIdentifier result;
    return (result << m_INDET_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::lar_exp             (void) const
{
    ExpandedIdentifier result;
    return (result << m_LAR_ID);  
}

inline ExpandedIdentifier          
AtlasDetectorID::tile_exp            (void) const
{
    ExpandedIdentifier result;
    return (result << m_TILE_ID); 
}

inline ExpandedIdentifier          
AtlasDetectorID::muon_exp            (void) const
{
    ExpandedIdentifier result;
    return (result << m_MUON_ID); 
}

inline ExpandedIdentifier          
AtlasDetectorID::calo_exp(void) const
{
    ExpandedIdentifier result;
    return (result << m_CALO_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::fwd_exp             (void) const
{
    ExpandedIdentifier result;
    return (result << m_FWD_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::pixel_exp           (void) const
{
    ExpandedIdentifier result(indet_exp());
    return (result << m_PIXEL_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::sct_exp             (void) const
{
    ExpandedIdentifier result(indet_exp());
    return (result << m_SCT_ID);  
}

inline ExpandedIdentifier          
AtlasDetectorID::trt_exp             (void) const
{
    ExpandedIdentifier result(indet_exp());
    return (result << m_TRT_ID);  
}

inline ExpandedIdentifier          
AtlasDetectorID::lar_em_exp          (void) const
{
    ExpandedIdentifier result(lar_exp());
    return (result << m_LAR_EM_ID);  
}

inline ExpandedIdentifier          
AtlasDetectorID::lar_hec_exp         (void) const
{
    ExpandedIdentifier result(lar_exp());
    return (result << m_LAR_HEC_ID); 
}

inline ExpandedIdentifier          
AtlasDetectorID::lar_fcal_exp        (void) const
{
    ExpandedIdentifier result(lar_exp());
    return (result << m_LAR_FCAL_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::alfa_exp             (void) const
{
    ExpandedIdentifier result(fwd_exp());
    return (result << m_ALFA_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::bcm_exp             (void) const
{
    ExpandedIdentifier result(fwd_exp());
    return (result << m_BCM_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::lucid_exp           (void) const
{
    ExpandedIdentifier result(fwd_exp());
    return (result << m_LUCID_ID);
}

inline ExpandedIdentifier          
AtlasDetectorID::zdc_exp             (void) const
{
    ExpandedIdentifier result(fwd_exp());
    return (result << m_ZDC_ID);
}

inline int                 
AtlasDetectorID::indet_field_value        () const {return (m_INDET_ID);}     

inline int                 
AtlasDetectorID::lar_field_value          () const {return (m_LAR_ID);}

inline int                 
AtlasDetectorID::tile_field_value         () const {return (m_TILE_ID);}       

inline int                 
AtlasDetectorID::muon_field_value         () const {return (m_MUON_ID);}       

inline int                 
AtlasDetectorID::calo_field_value         () const {return (m_CALO_ID);}

inline int                 
AtlasDetectorID::pixel_field_value        () const {return (m_PIXEL_ID);}     

inline int                 
AtlasDetectorID::sct_field_value          () const {return (m_SCT_ID);}       

inline int                 
AtlasDetectorID::trt_field_value          () const {return (m_TRT_ID);}       

inline int                 
AtlasDetectorID::lar_em_field_value       () const {return (m_LAR_EM_ID);}  

inline int                 
AtlasDetectorID::lar_hec_field_value      () const {return (m_LAR_HEC_ID);} 

inline int                 
AtlasDetectorID::lar_fcal_field_value     () const {return (m_LAR_FCAL_ID);}

inline int                 
AtlasDetectorID::mdt_field_value          () const {return (m_MDT_ID);}

inline int                 
AtlasDetectorID::csc_field_value          () const {return (m_CSC_ID);}

inline int                 
AtlasDetectorID::rpc_field_value          () const {return (m_RPC_ID);}

inline int                 
AtlasDetectorID::tgc_field_value          () const {return (m_TGC_ID);}

inline int                 
AtlasDetectorID::stgc_field_value          () const {return (m_STGC_ID);}

inline int                 
AtlasDetectorID::mm_field_value          () const {return (m_MM_ID);}

inline int                 
AtlasDetectorID::forward_field_value      () const {return (m_FWD_ID);}
inline int                 
AtlasDetectorID::alfa_field_value         () const {return (m_ALFA_ID);}
inline int                 
AtlasDetectorID::bcm_field_value          () const {return (m_BCM_ID);}
inline int                 
AtlasDetectorID::lucid_field_value        () const {return (m_LUCID_ID);}
inline int                 
AtlasDetectorID::zdc_field_value          () const {return (m_ZDC_ID);}


inline bool               
AtlasDetectorID::is_indet       (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_INDET_ID);
}

inline bool               
AtlasDetectorID::is_lar                 (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_LAR_ID);
}

inline bool               
AtlasDetectorID::is_tile                (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_TILE_ID);
}

inline bool               
AtlasDetectorID::is_muon                (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_MUON_ID);
}

inline bool                
AtlasDetectorID::is_calo(Identifier id) const
{
    return (m_det_impl.unpack(id) == m_CALO_ID);
}

inline bool                
AtlasDetectorID::is_lvl1_trig_towers(Identifier id) const
{
    bool result = false;
    if (is_calo(id)) {
        int temp = m_calo_side_impl.unpack(id);
        result = (m_lvl1_field.match(temp));
    }
    return result;
}

inline bool                
AtlasDetectorID::is_lvl1_online(Identifier id) const
{
    bool result = false;
    if (is_calo(id)) {
        int temp = m_calo_side_impl.unpack(id);
        result = (m_lvl1_onl_field.match(temp));
    }
    return result;
}


inline bool                
AtlasDetectorID::is_lar_dm(Identifier id) const
{
    bool result = false;
    if (is_calo(id)) {
        int temp = m_calo_side_impl.unpack(id);
        result = (m_lar_dm_field.match(temp));
    }
    return result;
}


inline bool                
AtlasDetectorID::is_tile_dm(Identifier id) const
{
    bool result = false;
    if (is_calo(id)) {
        int temp = m_calo_side_impl.unpack(id);
        result = (m_tile_dm_field.match(temp));
    }
    return result;
}


inline bool               
AtlasDetectorID::is_pixel       (Identifier id) const
{
    bool result = false;
    if(is_indet(id)) {
        result = (m_indet_part_impl.unpack(id) == m_PIXEL_ID);
    }
    return result;
}

inline bool               
AtlasDetectorID::is_sct         (Identifier id) const
{
    bool result = false;
    // Must check is_pixel to allow for special bits
    // (don't need this check for 64-bit identifier, but leave it in for now)
    if(is_indet(id) && !is_pixel(id)) { 
	result = (m_indet_part_impl.unpack(id) == m_SCT_ID);
    }
    return result;
}

inline bool               
AtlasDetectorID::is_trt         (Identifier id) const
{
    bool result = false;
    // Must check is_pixel to allow for special bits
    // (don't need this check for 64-bit identifier, but leave it in for now)
    if(is_indet(id) && !is_pixel(id)) {
        result = (m_indet_part_impl.unpack(id) == m_TRT_ID);
    }
    return result;
}

inline bool               
AtlasDetectorID::is_lar_em      (Identifier id) const
{
    bool result = false;
    if(is_lar(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_LAR_EM_ID);
    }
    return result;
}


inline bool               
AtlasDetectorID::is_lar_hec             (Identifier id) const
{
    bool result = false;
    if(is_lar(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_LAR_HEC_ID);
    }
    return result;
}

inline bool               
AtlasDetectorID::is_lar_fcal            (Identifier id) const
{
    bool result = false;
    if(is_lar(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_LAR_FCAL_ID);
    }
    return result;
}

inline bool               
AtlasDetectorID::is_lar_minifcal            (Identifier id) const
{
    bool result = false;
    if(is_lar_fcal(id)) {
        result = (m_lar_fcal_module_impl.unpack(id) == 0);
    }
    return result;
}

inline bool
AtlasDetectorID::is_mdt(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    // MDT and RPC can only be destinguished by the additional bit in the ID
    // since the station names are ambiguous
    if (is_muon(id) && ((techBit == AtlasDetDescr::fAtlasMDT)
                        || (techBit == AtlasDetDescr::fAtlasRPC))) {
      result = (m_muon_mdt_impl.unpack(id) == m_MDT_ID);
    }
    return result;
}

inline bool
AtlasDetectorID::is_rpc(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    // MDT and RPC can only be destinguished by the additional bit in the ID
    // since the station names are ambiguous
    if (is_muon(id) && ((techBit == AtlasDetDescr::fAtlasMDT)
                        || (techBit == AtlasDetDescr::fAtlasRPC))) {
      result = (m_muon_rpc_impl.unpack(id) == m_RPC_ID);
    }
    return result;
}

inline bool
AtlasDetectorID::is_csc(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    result = (is_muon(id) && (techBit == AtlasDetDescr::fAtlasCSC));
    return result;
}

inline bool
AtlasDetectorID::is_tgc(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    result = (is_muon(id) && (techBit == AtlasDetDescr::fAtlasTGC));
    return result;
}

inline bool
AtlasDetectorID::is_mm(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    result = (is_muon(id) && (techBit == AtlasDetDescr::fAtlasMM));
    return result;
}

inline bool
AtlasDetectorID::is_stgc(Identifier id) const
{
    bool result = false;
    unsigned int stationNameIndex = m_muon_station_name_impl.unpack(id);
    unsigned int techBit = m_muon_tech_bits[stationNameIndex];
    result = (is_muon(id) && (techBit == AtlasDetDescr::fAtlasSTGC));
    return result;
}

inline bool
AtlasDetectorID::is_forward      (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_FWD_ID);
}

inline bool
AtlasDetectorID::is_alfa          (Identifier id) const
{
    bool result = false;
    if(is_forward(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_ALFA_ID);
    }
    return result;
}

inline bool
AtlasDetectorID::is_bcm          (Identifier id) const
{
    bool result = false;
    if(is_forward(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_BCM_ID);
    }
    return result;
}

inline bool
AtlasDetectorID::is_lucid        (Identifier id) const
{
    bool result = false;
    if(is_forward(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_LUCID_ID);
    }
    return result;
}

inline bool
AtlasDetectorID::is_zdc          (Identifier id) const
{
    bool result = false;
    if(is_forward(id)) {
        result = (abs(m_lar_part_impl.unpack(id)) == m_ZDC_ID);
    }
    return result;
}


#endif // ATLASDETDESCR_ATLASDETECTORID_H
