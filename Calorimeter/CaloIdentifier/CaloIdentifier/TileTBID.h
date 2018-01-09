/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileTBID.h,v 1.1 2009-03-30 11:19:27 tsulaia Exp $
//<version>	$Name: not supported by cvs2svn $

// TileTBID.h
//
// Atlas detector Identifier class for Tile Calorimeter
//

#ifndef TILEIDENTIFIER_TILETBID_H
#define TILEIDENTIFIER_TILETBID_H

#include "CaloIdentifier/TileID.h"

/** 
* @class TileTBID
* @brief Helper class for TileCal offline identifiers of ancillary testbeam detectors and MBTS
* @author Alexander Solodkov
*
*
* This class provides an interface to generate hardware identifier or
* a range for the Tile Calorimeter detector subsystem.
*
* Definition and the range of values for the elements of the identifier:
* <pre>
*
*  element     range       meaning
*  -------     -----       -------
*
*  type         2 to 6     Readout type
*                           2 - prototype modules (not used now)
*                           3 - all detectors with ADC readout
*                           4 - all detectors with TDC readout
*                           5 - all detectors with flash ADC readout
*                           6 - non-detectors
*  module       0 to 6    For type=2 - just module number (0-4)
*                         For type=5 - drawer number (0-5)
*                         for (type=3)        | for (type=4)          | for (type=6)
*                           0 - S1-S4         | beam ch1 (both X & Y) | CIS parameters
*                           1 - Ch1-Ch2       | beam ch2 (both X & Y) | Pattern Unit
*                           2 - Laser         | beam ch3 (both X & Y) |
*                           3 - back mu-wall  | beam ch4 (both X & Y) |
*                           4 - side mu-wall  | extra                 |
*                           5 - crack counter |                       |
*                           6 - phantom calo  |                       |
*  channel     0 to 63    channel number
*                        (for beam chambers: X=0, Y=1)
*
* if TileTBID is used for minimum bias scintillators, then
*
* type         -1 or +1  - eta side
* module        0 to 7   - phi slice number
* channel       0 to 1   - cell number along radius
* </pre>
*/

class TileTBID : public AtlasDetectorID {

public:

  typedef Identifier::size_type  size_type ;
      
  /** 
   * @brief Default constructor
   */
  TileTBID(void);

  /** 
   * @brief Default destructor
   */
  virtual ~TileTBID();

  /** Identifier numbering */
  enum        SECTION   { TILE_TESTBEAM = 4 /*TileID::TESTBEAM*/ };
  enum        RO_TYPE   { PROTO_TYPE = 2,
                          ADC_TYPE   = 3,
                          TDC_TYPE   = 4,
                          FADC_TYPE  = 5,
                          INT_TYPE   = 6 };
  enum        DET_NUM   { S_COUNTER  = 0, CIS_PAR = 0,
                          CH_COUNTER = 1, PATTERN_UNIT = 1,
                          LASER      = 2,
                          BACK_WALL  = 3,
                          SIDE_WALL  = 4,
                          CRACK_WALL = 5,
                          PHANTOM_CALO = 6 };
  

  /**
   * @brief identifer for all Tile testbeam detectors
   */
  Identifier          tiletb_id               ( void )                        const;
  /**
   * @brief identifer for Tile testbeam detectors of a give type
   */
  Identifier          type_id                 ( int type)                     const;
  /**
   * @brief identifer for one module of a Tile testbeam detector
   */
  Identifier          module_id               ( int type, int module )        const;
  /**
   * @brief identifer for one channel of a Tile testbeam detector
   */
  Identifier          channel_id              ( int type, int module, 
                                                int channel )                 const;

  /**
   * @brief extract module ID from channel ID
   */
  Identifier          module_id               ( const Identifier& id )        const; 
  /**
   * @brief create channel ID from module ID
   */
  Identifier          channel_id              ( const Identifier& id,
                                                int channel )                 const; 

  /**
   * @brief Test ID if it is TileTBID
   */
  bool	        is_tiletb               ( const Identifier& id )        const;

  /**
   * @brief extract system field from TileTB identifier
   */
  inline int    system  ( const Identifier & id )  const    { return m_system_impl.unpack(id); }
  /**
   * @brief extract section field from TileTB identifier
   */
  inline int    section ( const Identifier & id )  const    { return m_section_impl.unpack(id); }
  /**
   * @brief extract type field from TileTB identifier
   */
  inline int    type    ( const Identifier & id )  const    { return m_type_impl.unpack(id); }
  /**
   * @brief extract module field from TileTB identifier
   */
  inline int    module  ( const Identifier & id )  const    { return m_module_impl.unpack(id); }
  /**
   * @brief extract channel field from TileTB identifier
   */
  inline int    channel ( const Identifier & id )  const    { return m_channel_impl.unpack(id); }

  /** define synonyms for minimum bias scintillators */

  /**
   * @brief extract side field from MBTS identifier
   */
  inline int side  ( const Identifier & id )  const { return type(id); }
  /**
   * @brief extract phi field from MBTS identifier
   */
  inline int phi   ( const Identifier & id )  const { return module(id); }
  /**
   * @brief extract eta field from MBTS identifier
   */
  inline int eta   ( const Identifier & id )  const { return channel(id); }
  

  /**
   * @brief extract all fields from TileTB identifier
  Identifier          get_all_fields          ( const Identifier & id,
                                                unsigned int last=99 )        const;
  */
  
  /**
   * @brief convert TileTB identifier to string
   */
  std::string         to_string               ( const Identifier & id, int level=0 )     const;

  /** access to IdContext's which define which levels of fields are contained in the id */

  /**
   * @brief idContext for TYPEs
   */
  IdContext    type_context     (void) const;
  /**
   * @brief idContext for modules
   */
  IdContext    module_context   (void) const;
  /**
   * @brief idContext for channels
   */
  IdContext    channel_context  (void) const;


  /**
   * @brief create compact id from hash id (return == 0 for OK)
   */
  virtual int  get_id    (const IdentifierHash& hash_id, Identifier& id, const IdContext* context = 0 ) const;

  /**
   * @brief create hash id from compact id (return == 0 for OK)
   */
  virtual int  get_hash  (const Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0 ) const;

  /**
   * @brief initialization from the identifier dictionary
   */
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

  /**
   * @brief TYPE hash table max size
   */
  inline size_type      type_hash_max   (void) const    { return m_type_hash_max; }
  /**
   * @brief module hash table max size
   */
  inline size_type      module_hash_max (void) const    { return m_module_hash_max; }
  /**
   * @brief channel hash table max size
   */
  inline size_type      channel_hash_max(void) const    { return m_channel_hash_max; }

  /**
   * @brief begin iterator for TYPE Identifiers
   */
  std::vector<Identifier>::const_iterator type_begin     (void) const   { return(m_type_vec.begin()); }
  /**
   * @brief end iterator for TYPE Identifiers
   */
  std::vector<Identifier>::const_iterator type_end       (void) const   { return(m_type_vec.end()); }

  /**
   * @brief begin iterator for moulde Identifiers
   */
  std::vector<Identifier>::const_iterator module_begin  (void) const    { return(m_module_vec.begin()); }
  /**
   * @brief end iterator for moulde Identifiers
   */
  std::vector<Identifier>::const_iterator module_end    (void) const    { return(m_module_vec.end()); }

  /**
   * @brief begin iterator for channel Identifiers
   */
  std::vector<Identifier>::const_iterator channel_begin (void) const    { return(m_channel_vec.begin()); }
  /**
   * @brief begin iterator for channel Identifiers
   */
  std::vector<Identifier>::const_iterator channel_end   (void) const    { return(m_channel_vec.end()); }

  enum {NOT_VALID_HASH = 64000};

private:

  /**
   * @brief create expanded Identifier from Identifier (return == 0 for OK)
   */
  int  get_expanded_id  (const Identifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  /**
   * @brief unpack one field from Identifier
   */
  int                   get_field               ( const Identifier & id,
                                                  size_type index )             const;

  int         initLevelsFromDict(void) ;

  int         init_hashes(void) ;

  bool        module_id  (const Identifier& type_id,    int module,              Identifier& module_id  );
  bool        channel_id (const Identifier& type_id,    int module, int channel, Identifier& channel_id );
  bool        channel_id (const Identifier& module_id,  int channel,             Identifier& channel_id );
  int         fill_vec      (std::set<Identifier> & ids, 
                             unsigned int hash_max, std::vector<Identifier> & vec);

  size_type                     m_tile_region_index;
  size_type                     m_SYSTEM_INDEX;
  size_type                     m_SECTION_INDEX;
  size_type                     m_TYPE_INDEX;
  size_type                     m_MODULE_INDEX;
  size_type                     m_CHANNEL_INDEX;

  const IdDictDictionary*       m_dict;

  IdDictFieldImplementation     m_system_impl;
  IdDictFieldImplementation     m_section_impl;
  IdDictFieldImplementation     m_type_impl;
  IdDictFieldImplementation     m_module_impl;
  IdDictFieldImplementation     m_channel_impl;

  MultiRange                    m_full_type_range;
  MultiRange                    m_full_module_range;
  MultiRange                    m_full_channel_range;
  size_type                     m_type_hash_max;
  size_type                     m_module_hash_max;
  size_type                     m_channel_hash_max;
  std::vector<Identifier>       m_type_vec;
  std::vector<Identifier>       m_module_vec;
  std::vector<Identifier>       m_channel_vec;

  Identifier			m_base_tile_type;
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( TileTBID , 2903 , 1 )

#endif // TILEIDENTIFIER_TILETBID_H

