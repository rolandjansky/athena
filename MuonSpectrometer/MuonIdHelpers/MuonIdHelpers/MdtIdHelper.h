/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: MdtIdHelper.h,v 1.34 2009-01-20 22:44:13 kblack Exp $
//<version>   $Name: not supported by cvs2svn $

#ifndef MUONIDHELPERS_MDTIDHELPER_H
#define MUONIDHELPERS_MDTIDHELPER_H

// Includes

#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class MdtIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs MDT identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2002-??? provides a complete description
// of the hierarchical identifier scheme.  MdtIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to "BIL", "EMS" ,etc.
// StationEta      [-6,-1]             backward endcap (-1 at lowest R)
//                 [-8,8]              barrel (increases with Z)
//                 [-6,-1]+[1,6]       forward endcap (1 at lowest R)
// StationPhi      [1,8]               increases with phi
// Technology      [0]                 maps to "MDT"
// Multilayer      [1,2]               barrel: increases with R
//                                     endcap: increases with |Z|
// TubeLayer       [1,4]               barrel: increases with R
//                                     endcap: increases with |Z|
// Tube            [1,n]               barrel: increases with |Z|
//                                     endcap: increases with R
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from MuonIdHelpers/MuonIdHelper.
//
// Author
// ------
// Steven Goldfarb <Steven.Goldfarb@cern.ch>
//
// Compact Id implementation by
// Ketevi A. Assamagan <ketevi@bnl.gov>
// BNL, March 10th, 2003
//
// ******************************************************************************

class MdtIdHelper : public MuonIdHelper
{
 public:


  // Constructor

  MdtIdHelper();

  // Destructor

  virtual ~MdtIdHelper();


  ////////////// compact identifier stuff begins ////////////////////////////////////// 

  /// Initialization from the identifier dictionary
  virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);
  virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id ) const;
  virtual int get_detectorElement_hash (const Identifier& id, IdentifierHash& hash_id ) const;

  ///////////// compact identifier stuff ends   ////////////////////////////////////// 

  // Atlas Identifier builders

  Identifier elementID(int stationName, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(std::string stationNameStr, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(const Identifier& channelID) const;
  Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube, bool check=false, bool* isValid=0) const;
  Identifier channelID(std::string stationNameStr, int stationEta, int stationPhi, int multilayer, int tubeLayer, int tube, bool check=false, bool* isValid=0) const;
  Identifier channelID(const Identifier& id, int multilayer, int tubeLayer, int tube, bool check=false, bool* isValid=0) const;

  Identifier parentID (const Identifier& id) const;

  Identifier multilayerID(const Identifier& channeldID) const;
  Identifier multilayerID(const Identifier& moduleID, int multilayer, bool check=false, bool* isValid=0) const;
    
  //for an Identifier id, get the list of the daughter readout channel ids
  void idChannels (const Identifier& id, std::vector<Identifier>& vect) const;

  // Access to levels: missing field returns 0 

  int channel(const Identifier& id) const;

  int multilayer(const Identifier& id) const;
  int tubeLayer (const Identifier& id) const;
  int tube      (const Identifier& id) const;

  int numberOfMultilayers(const Identifier& id) const;

  // Access to min and max of level ranges

  int stationEtaMin(bool barrel) const;
  int stationEtaMax(bool barrel) const;
  int stationPhiMin() const;
  int stationPhiMax() const;
  int multilayerMin() const;
  int multilayerMax() const;
  int tubeLayerMin() const;
  int tubeLayerMax() const;
  int tubeMin() const;
  int tubeMax() const;

  // Access to min and max of level ranges

  int stationEtaMin(const Identifier& id) const;
  int stationEtaMax(const Identifier& id) const;
  int stationPhiMin(const Identifier& id) const;
  int stationPhiMax(const Identifier& id) const;
  int multilayerMin(const Identifier& id) const;
  int multilayerMax(const Identifier& id) const;
  int tubeLayerMin (const Identifier& id) const;
  int tubeLayerMax (const Identifier& id) const;
  int tubeMin      (const Identifier& id) const;
  int tubeMax      (const Identifier& id) const;

  // Public validation of levels

  bool valid(const Identifier& id) const;
  bool validElement(const Identifier& id) const;


  /// the gas-gap function for the MDT's returns the tube layer 
  int gasGap(const Identifier& id) const; 
  /// always false for MDTs 
  bool measuresPhi(const Identifier& id) const; 
  /// is this an sMDT chamber
  bool isSmallMdt(const Identifier& id) const;
  /// is this a BMG chamber
  bool isBMG(const Identifier& id) const;

 private:

  int init_id_to_hashes();
  unsigned int m_module_hashes[60][20][8];
  unsigned int m_detectorElement_hashes[60][20][8][3];

  // compact id indices
  size_type   m_TUBELAYER_INDEX;

  IdDictFieldImplementation   m_mla_impl;
  IdDictFieldImplementation   m_lay_impl;
  IdDictFieldImplementation   m_tub_impl;

  // Private validation of levels

  bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
  bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi,int multilayer, int tubeLayer, int tube) const;

  // Utility method

  int  mdtTechnology() const;
  bool barrelChamber(int stationName) const;

  // Level indices

  enum MdtIndices
    {
      MultilayerIndex = 5,
      TubeLayerIndex  = 6,
      TubeIndex       = 7
    };

  // Level ranges

  enum MdtRanges
    {
      StationEtaBarrelMin = -8,
      StationEtaBarrelMax =  8,
      StationEtaEndcapMin = -6,
      StationEtaEndcapMax =  6,
      StationPhiMin       =  1,
      StationPhiMax       =  8,
      MultilayerMin       =  1,
      MultilayerMax       =  2,
      TubeLayerMin        =  1,
      TubeLayerMax        =  4,
      TubeMin             =  1,
      TubeMax             = 78
    };
  inline virtual void create_mlog() const;
};

// For backwards compatibility

typedef MdtIdHelper  MDT_ID;

CLASS_DEF(MdtIdHelper, 4170, 1)

// Construct Atlas Identifiers from components

inline Identifier MdtIdHelper::elementID(int stationName,
					 int stationEta, int stationPhi, bool check, bool* isValid) const
{
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (mdt_field_value(),result);
  if ( check ) { 
    val = this->validElement(result, stationName, stationEta, stationPhi);
    if ( isValid ) *isValid = val;
  }
  return result;
}

inline Identifier MdtIdHelper::elementID(std::string stationNameStr,
					 int stationEta, int stationPhi, bool check, bool* isValid) const
{
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id =  elementID(stationName, stationEta, stationPhi, check, isValid);
  return id;
}

inline Identifier MdtIdHelper::elementID(const Identifier& id) const
{
  return parentID(id);
}

inline Identifier MdtIdHelper::channelID(int stationName,
					 int stationEta, int stationPhi,
					 int multilayer, int tubeLayer, int tube,
					 bool check, bool* isValid) const
{

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (mdt_field_value(),result);
  m_mla_impl.pack (multilayer,result);
  m_lay_impl.pack (tubeLayer,result);
  m_tub_impl.pack (tube,result);
  if ( check ) { 
    val = this->validChannel(result, stationName, stationEta, stationPhi, 
			     multilayer, tubeLayer, tube);
    if ( isValid ) *isValid = val;
  }
  return result;
}

inline Identifier MdtIdHelper::channelID(std::string stationNameStr,
					 int stationEta, int stationPhi,
					 int multilayer, int tubeLayer, int tube,
					 bool check, bool* isValid) const
{
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = channelID(stationName, stationEta, stationPhi, multilayer,
		 tubeLayer, tube, check, isValid);
  return id;
}

inline Identifier MdtIdHelper::channelID(const Identifier& id,
					 int multilayer, int tubeLayer, int tube,
					 bool check, bool* isValid) const
{
  Identifier result(id);
  bool val = false;
  m_mla_impl.pack (multilayer,result);
  m_lay_impl.pack (tubeLayer,result);
  m_tub_impl.pack (tube,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  }
  return result;
}

/// get parent id from channel id

inline Identifier MdtIdHelper::parentID(const Identifier& id) const
{
  assert(is_mdt(id));
  Identifier result(id);
  m_mla_impl.reset(result);
  m_lay_impl.reset(result);
  m_tub_impl.reset(result);
  return result;
}

/// Access to components of the ID

inline int MdtIdHelper::multilayer(const Identifier& id) const
{

  return m_mla_impl.unpack(id);

}

inline int MdtIdHelper::tubeLayer(const Identifier& id) const
{
 
  return m_lay_impl.unpack(id);

}

inline int MdtIdHelper::tube(const Identifier& id) const
{

  return m_tub_impl.unpack(id);

}

inline  int MdtIdHelper::gasGap(const Identifier& id) const 
{

 return tubeLayer(id); 

}

inline bool MdtIdHelper::measuresPhi(const Identifier& /*id*/) const 
{

 return false; 

} 

inline bool MdtIdHelper::isSmallMdt(const Identifier& id) const
{
  int index=stationName(id);
  if(stationNameIndex("BME")==index || stationNameIndex("BMG")==index) return true;
  else return false;
}

inline bool MdtIdHelper::isBMG(const Identifier& id) const
{
  int index=stationName(id);
  if(stationNameIndex("BMG")==index) return true;
  else return false;
}

inline int MdtIdHelper::channel(const Identifier& id) const
{

  return tube(id);

}

/// Access to min and max of level ranges

inline int MdtIdHelper::stationEtaMin(bool barrel) const
{
  if (barrel)
    {
      return StationEtaBarrelMin;
    }
  else
    {
      return StationEtaEndcapMin;
    }
}

inline int MdtIdHelper::stationEtaMax(bool barrel) const
{
  if (barrel)
    {
      return StationEtaBarrelMax;
    }
  else
    {
      return StationEtaEndcapMax;
    }
}

inline int MdtIdHelper::stationPhiMin() const
{
  return StationPhiMin;
}

inline int MdtIdHelper::stationPhiMax() const
{
  return StationPhiMax;
}

inline int MdtIdHelper::multilayerMin() const
{
  return MultilayerMin;
}

inline int MdtIdHelper::multilayerMax() const
{
  return MultilayerMax;
}

inline int MdtIdHelper::tubeLayerMin() const
{
  return TubeLayerMin;
}

inline int MdtIdHelper::tubeLayerMax() const
{
  return TubeLayerMax;
}

inline int MdtIdHelper::tubeMin() const
{
  return TubeMin;
}

inline int MdtIdHelper::tubeMax() const
{
  return TubeMax;
}
/// Utility methods

inline int MdtIdHelper::mdtTechnology() const
{
  
  int mdtField = technologyIndex("MDT");
  if (m_dict)
    {
      mdtField =  mdt_field_value(); 
    }
  return mdtField;
}

#endif // MUONIDHELPERS_MDTIDHELPER_H


