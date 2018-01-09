/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileHWID.h,v 1.11 2008-11-03 15:25:00 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $

// TileHWID.h
//
// Atlas detector Identifier class for Tile Calorimeter (hardware identifier)
//

#ifndef TILEIDENTIFIER_TILEHWID_H
#define TILEIDENTIFIER_TILEHWID_H

#include "CaloIdentifier/TileID.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

/** 
 * @class TileHWID
 * @brief Helper class for TileCal online (hardware) identifiers
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
 *  ros         1 to 4      ReadOutSystem number ( 1,2 = pos/neg Barrel (side A/C)
 *                                                 3,4 = pos/neg Ext.Barrel (side A/C) )
 *  drawer      0 to 63     64 drawers (modules) in one cylinder (phi-slices)
 *  channel     0 to 47     channel number in the drawer
 *  adc         0 to 1      ADC number for the channel (0 = low gain, 1 = high gain)
 * </pre>
 */


class TileHWID : public AtlasDetectorID {

public:

  typedef Identifier::size_type  size_type ;
      
  /** 
   * @brief Default constructor
   */
  TileHWID(void);

  /** 
   * @brief Default destructor
   */
  virtual ~TileHWID();

  /** Identifier numbering */
  enum        SECTION   { TILE_ONLINE = TileID::ONLINE };
  enum        ROSID     { BEAM_ROS   = 0,
                          BARREL_POS = 1,
                          BARREL_NEG = 2,
                          EXTBAR_POS = 3,
                          EXTBAR_NEG = 4 };
  enum        GAIN      { LOWGAIN  = TileID::LOWGAIN,
                          HIGHGAIN = TileID::HIGHGAIN };

  /**
   * @brief online identifer for whole Tile
   */
  HWIdentifier        tilehw_id               ( void )                        const;
  /**
   * @brief ROS HWIdentifer
   */
  HWIdentifier        ros_id                  ( int ros)                      const;
  /**
   * @brief ROS HWIdentifer
   */
  HWIdentifier        drawer_id               ( int frag )                    const;
  /**
   * @brief drawer HWIdentifer
   */
  HWIdentifier        drawer_id               ( int ros, int drawer )         const;
  /**
   * @brief channel HWIdentifer
   */
  HWIdentifier        channel_id              ( int ros, int drawer, 
                                                int channel )                 const;
  /**
   * @brief adc HWIdentifer
   */
  HWIdentifier        adc_id                  ( int ros, int drawer,
                                                int channel, int adc )        const;

  /**
   * @brief extract drawer ID from adc ID or channel ID 
   */
  HWIdentifier        drawer_id               ( const HWIdentifier& id )      const; 
  /**
   * @brief extract channel ID from adc ID 
   */
  HWIdentifier        channel_id              ( const HWIdentifier& id )      const; 
  /**
   * @brief create channel ID from drawer ID
   */
  HWIdentifier        channel_id              ( const HWIdentifier& id,
                                                int channel )                 const;
  /**
   * @brief create adc ID from drawer ID
   */
  HWIdentifier        adc_id                  ( const HWIdentifier& id,
                                                int channel, int adc )        const; 
  /**
   * @brief create adc ID from channel ID
   */
  HWIdentifier        adc_id                  ( const HWIdentifier& id,
                                                int adc )                     const;

  /**
   * @brief create channel ID from channel hash ID
   * @brief validity of the hash_id is not checked, assuming that it is done outside
   */
  inline HWIdentifier channel_id              ( IdentifierHash hash_id )      const
    { return m_channel_vec[hash_id]; }

  /**
   * @brief create adc ID from channel hash ID
   * @brief validity of the hash_id is not checked, assuming that it is done outside
   */
  HWIdentifier        adc_id                  ( IdentifierHash hash_id,
                                                int adc )                     const
    { return adc_id(m_channel_vec[hash_id],adc); }
        

  /**
   * @brief Test ID for Tile online ID
   */
  bool                is_tilehw               ( const HWIdentifier& id )      const;
  /**
   * @brief Test ID for low gain
   */
  bool                is_low_gain             ( const HWIdentifier& id )      const;
  /**
   * @brief Test ID for high gain
   */
  bool                is_high_gain            ( const HWIdentifier& id )      const;


  /**
   * @brief extract system field from HW identifier
   */
  inline int  system  ( const HWIdentifier & id )     const   { return m_system_impl.unpack(id); }
  /**
   * @brief extract section field from HW identifier
   */
  inline int  section ( const HWIdentifier & id )     const   { return m_section_impl.unpack(id); }
  /**
   * @brief extract ros field from HW identifier
   */
  inline int  ros     ( const HWIdentifier & id )     const   { return m_ros_impl.unpack(id); }
  /**
   * @brief extract drawer field from HW identifier
   */
  inline int  drawer  ( const HWIdentifier & id )     const   { return m_drawer_impl.unpack(id); }
  /**
   * @brief construct drawer index from HW identifier
   */
  inline int drawerIdx( const HWIdentifier & id )     const   { 
    return std::max(0,(int)(std::upper_bound(m_drawer_vec.begin(),m_drawer_vec.end(),id) - m_drawer_vec.begin())-1); 
  }
  /**
   * @brief extract frag field from HW identifier
   */
  inline int  frag    ( const HWIdentifier & id )     const   { return ((ros(id) << 8) | drawer(id)); }
  /**
   * @brief make frag from ROS and drawer
   */
  inline int  frag    ( int ros, int drawer )         const   { return ((ros << 8) | drawer); }
  /**
   * @brief extract channel field from HW identifier
   */
  inline int  channel ( const HWIdentifier & id )     const   { return m_channel_impl.unpack(id); }
  /**
   * @brief extract adc field from HW identifier
   */
  inline int  adc     ( const HWIdentifier & id )     const   { return m_adc_impl.unpack(id); }

  /**
   * @brief extract all fields from HW identifier
  HWIdentifier        get_all_fields          ( const HWIdentifier & id,
                                                unsigned int last=99 )        const;
  */
  
  /**
   * @brief convert HW identifier to string
   */
  std::string         to_string               ( const HWIdentifier & id, int level=0 )     const;

  /** access to IdContext's which define which levels of fields are contained in the id */

  /**
   * @brief idContext for ROSes
   */
  IdContext    ros_context      (void) const;
  /**
   * @brief idContext for drawers
   */
  IdContext    drawer_context   (void) const;
  /**
   * @brief idContext for channels
   */
  IdContext    channel_context  (void) const;
  /**
   * @brief idContext for ADCs
   */
  IdContext    adc_context      (void) const;

  /**
   * @brief create compact HW ID from hash id (return == 0 for OK)
   */
  virtual int  get_id    (const IdentifierHash& hash_id, HWIdentifier& id, const IdContext* context = 0 ) const;
  /**
   * @brief create compact ID from hash id (return == 0 for OK)
   */
  virtual int  get_id    (const IdentifierHash& hash_id,   Identifier& id, const IdContext* context = 0 ) const
    { HWIdentifier hwid(id); int stat=get_id(hash_id, hwid, context); id = hwid.get_compact(); return stat; }

  /**
   * @brief create hash id from compact ADC id without error checking
   */
  virtual IdentifierHash get_hash (const HWIdentifier& id) const;

  /**
   * @brief create hash id from compact HW id (return == 0 for OK)
   */
  virtual int  get_hash  (const HWIdentifier& id, IdentifierHash& hash_id, const IdContext* context = 0 ) const;
  /**
   * @brief create hash id from compact id (return == 0 for OK)
   */
  virtual int  get_hash  (const   Identifier& id, IdentifierHash& hash_id, const IdContext* context = 0 ) const
    { HWIdentifier hwid(id); return get_hash(hwid, hash_id, context); }

  /**
   * @brief create hash id from compact Channel id 
   */
  virtual IdentifierHash get_channel_hash (const HWIdentifier& id) const;


  /**
   * @brief initialization from the identifier dictionary
   */
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);

  /**
   * @brief ROS hash table max size
   */
  inline size_type     ros_hash_max     (void)  const   { return m_ros_hash_max; }
  /**
   * @brief drawer hash table max size
   */
  inline size_type     drawer_hash_max  (void)  const   { return m_drawer_hash_max; }
  /**
   * @brief channel hash table max size
   */
  inline size_type     channel_hash_max (void)  const   { return m_channel_hash_max; }
  /**
   * @brief ADC hash table max size
   */
  inline size_type     adc_hash_max     (void)  const   { return m_adc_hash_max; }

  /**
   * @brief begin iterator for ROS Identifiers
   */
  std::vector<HWIdentifier>::const_iterator ros_begin     (void) const  { return(m_ros_vec.begin()); }
  /**
   * @brief end iterator for ROS Identifiers
   */
  std::vector<HWIdentifier>::const_iterator ros_end       (void) const  { return(m_ros_vec.end()); }

  /**
   * @brief begin iterator for drawer Identifiers
   */
  std::vector<HWIdentifier>::const_iterator drawer_begin  (void) const  { return(m_drawer_vec.begin()); }
  /**
   * @brief end iterator for drawer Identifiers
   */
  std::vector<HWIdentifier>::const_iterator drawer_end    (void) const  { return(m_drawer_vec.end()); }

  /**
   * @brief begin iterator for channel Identifiers
   */
  std::vector<HWIdentifier>::const_iterator channel_begin (void) const  { return(m_channel_vec.begin()); }
  /**
   * @brief end iterator for channel Identifiers
   */
  std::vector<HWIdentifier>::const_iterator channel_end   (void) const  { return(m_channel_vec.end()); }

  /**
   * @brief begin iterator for ADC Identifiers
   */
  std::vector<HWIdentifier>::const_iterator adc_begin     (void) const  { return(m_adc_vec.begin()); }
  /**
   * @brief end iterator for ADC Identifiers
   */
  std::vector<HWIdentifier>::const_iterator adc_end       (void) const  { return(m_adc_vec.end()); }

  enum {NOT_VALID_HASH = 64000};

private:

  /**
   * @brief create expanded Identifier from Identifier (return == 0 for OK)
   */
  int  get_expanded_id  (const HWIdentifier& id, ExpandedIdentifier& exp_id, const IdContext* context) const;

  /**
   * @brief unpack one field from Identifier
   */
  int                   get_field               ( const HWIdentifier & id,
                                                  size_type index )             const;

  int         initLevelsFromDict(void) ;

  int         init_hashes(void) ;

  bool        drawer_id  (const HWIdentifier& ros_id,     int drawer,              HWIdentifier& drawer_id  );
  bool        channel_id (const HWIdentifier& ros_id,     int drawer, int channel, HWIdentifier& channel_id );
  bool        channel_id (const HWIdentifier& drawer_id,  int channel,             HWIdentifier& channel_id );
  bool        adc_id     (const HWIdentifier& channel_id, int adc,                 HWIdentifier& adc_id );
  int         fill_vec      (std::set<HWIdentifier> & ids, 
                             unsigned int hash_max, std::vector<HWIdentifier> & vec);

  size_type                     m_tile_region_index;
  size_type                     m_SYSTEM_INDEX;
  size_type                     m_SECTION_INDEX;
  size_type                     m_ROS_INDEX;
  size_type                     m_DRAWER_INDEX;
  size_type                     m_CHANNEL_INDEX;
  size_type                     m_ADC_INDEX;

  const IdDictDictionary*       m_dict;

  IdDictFieldImplementation     m_system_impl;
  IdDictFieldImplementation     m_section_impl;
  IdDictFieldImplementation     m_ros_impl;
  IdDictFieldImplementation     m_drawer_impl;
  IdDictFieldImplementation     m_channel_impl;
  IdDictFieldImplementation     m_adc_impl;

  MultiRange                    m_full_ros_range;
  MultiRange                    m_full_drawer_range;
  MultiRange                    m_full_channel_range;
  MultiRange                    m_full_adc_range;
  size_type                     m_ros_hash_max;
  size_type                     m_drawer_hash_max;
  size_type                     m_channel_hash_max;
  size_type                     m_adc_hash_max;
  std::vector<HWIdentifier>     m_ros_vec;
  std::vector<HWIdentifier>     m_drawer_vec;
  std::vector<HWIdentifier>     m_channel_vec;
  std::vector<HWIdentifier>     m_adc_vec;

  HWIdentifier			m_base_tile_ros;

};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( TileHWID , 2902 , 1 )

#endif // TILEIDENTIFIER_TILEHWID_H

