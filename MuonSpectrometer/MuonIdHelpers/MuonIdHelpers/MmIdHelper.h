/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// Copyright (C) 2013 by ATLAS Collaboration
// ******************************************************************************

#ifndef MUONIDHELPERS_MMIDHELPER_H
#define MUONIDHELPERS_MMIDHELPER_H

// Includes
class MsgStream;
#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class MmIdHelper
// ******************************************************************************//
// Description
// -----------
// This factory class constructs MicroMegas (MM) identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.  
// MmIdHelper provides an interface to the following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// nectar: todo: update this section for MM
// StationName     unsigned integer    maps to T1F,T3E,etc.
// StationEta      [-5,-1]             backward endcap (-1 at lowest R)
//                 [1,5]               forward endcap (1 at lowest R)
// StationPhi      [1,48]              endcap: increases with phi
//                 [1,24]              forward: increases with phi
// Technology      [5]                 maps to MM --> <label name="MM" value="5" /> in IdDictMuonSpectrometer_R.01.xml
// Multilayer       [1,2]               barrel: increases with R
//                                     endcap: increases with |Z|
// GasGap          [1,2]               doublet: increases with |Z|
//                 [1,3]               triplet: increases with |Z|
// Channel         [1,n]               increases with R for IsStrip=0 (wire gang)
//                                     increases with phi for IsStrip=1 (strip)
// ==============================================================================
//
// Inheritance
// -----------
// Inherits from MuonIdHelpers/MuonIdHelpers
//
// Author
// ------
// Nektarios Chr. Benekos <nectarios.benekos@cern.ch> 
// Jochen Meyer <Jochen.Meyer@cern.ch>
// ******************************************************************************



class MmIdHelper : public MuonIdHelper {
 public:

  // Constructor
  MmIdHelper();

  // Destructor
  virtual ~MmIdHelper();

  ///////////// compact identifier stuff begins ////////////////////////////////////// 

  /// Initialization from the identifier dictionary
  virtual int initialize_from_dictionary(const IdDictMgr& dict_mgr);
  virtual int get_module_hash(const Identifier& id, IdentifierHash& hash_id ) const;
  virtual int get_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id ) const;

  ///////////// compact identifier stuff ends   ////////////////////////////////////// 

  // Identifier builders
  Identifier elementID(int stationName, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(std::string stationNameStr, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const ;
  Identifier elementID(const Identifier& channelID) const ;
  
  Identifier channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool check=false, bool* isValid=0) const ;
  Identifier channelID(std::string stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool check=false, bool* isValid=0) const ;
  Identifier channelID(const Identifier& id, int multilayer, int gasGap, int channel, bool check=false, bool* isValid=0) const ;
  
  Identifier parentID (const Identifier& id) const;

  Identifier multilayerID(const Identifier& channeldID) const;
  Identifier multilayerID(const Identifier& moduleID, int multilayer, bool check=false, bool* isValid=0) const;


  //for an Identifier id, get the list of the daughter readout channel ids
  void idChannels (const Identifier& id, std::vector<Identifier>& vect) const;

  // Access to levels: missing field returns 0 
  int gasGap(const Identifier& id) const;
  int multilayer(const Identifier& id) const;
  int channel(const Identifier& id) const;
  bool measuresPhi(const Identifier& id) const; //Returns false

  int numberOfMultilayers(const Identifier& id) const;

  // Access to min and max of level ranges
  // to be remove when we moved to compact ids
  int stationEtaMin() const;
  int stationEtaMax() const;
  int stationPhiMin() const;
  int stationPhiMax() const;
  int multilayerMin() const;
  int multilayerMax() const;
  int gasGapMin() const;
  int gasGapMax() const;
  int channelMin() const;
  int channelMax() const;

  // Access to min and max of level ranges
  int stationEtaMin (const Identifier& id) const;
  int stationEtaMax (const Identifier& id) const;
  int stationPhiMin (const Identifier& id) const;
  int stationPhiMax (const Identifier& id) const;
  int multilayerMin(const Identifier& id) const;
  int multilayerMax(const Identifier& id) const;
  int gasGapMin     (const Identifier& id) const;
  int gasGapMax     (const Identifier& id) const;
  int channelMin    (const Identifier& id) const;
  int channelMax    (const Identifier& id) const;

  // Utility methods
  int sectorType(std::string stationName, int stationEta) const;
  int sectorType(int stationName, int stationEta) const;

  // Public validation of levels
  bool valid(const Identifier& id) const;
  bool validElement(const Identifier& id) const;

 private:
  int init_id_to_hashes();
  unsigned int m_module_hashes[60][20][48];            //Nektar Probably need to change this
  unsigned int m_detectorElement_hashes[60][20][8][3]; //Nektar Probably need to change this

  // compact id indices
  size_type   m_GASGAP_INDEX; 

  IdDictFieldImplementation   m_mplet_impl;
  IdDictFieldImplementation   m_gap_impl;
  IdDictFieldImplementation   m_cha_impl;

  // Check level values
  bool validElement(const Identifier& id, int stationName, int stationEta, int stationPhi) const;
  bool validChannel(const Identifier& id, int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel) const;

  // Utility methods
  int  mmTechnology() const;
  bool LargeSector(int stationName) const;
  bool SmallSector(int stationName) const;

  // Level indices
  enum MmIndices {
    MultilayerIndex = 5,
    GasGapIndex  = 6,
    ChannelIndex = 7
  };

  // Level ranges
  enum MmRanges {
    StationEtaMin =  0,
    StationEtaMax =  3,
    StationPhiMin =  1,      //ED: change for MM
    StationPhiMax =  8,      //ED: change for MM
    MultilayerMin  =  1,
    MultilayerMax  =  2,
    GasGapMin     =  1,
    GasGapMax     =  4,
    ChannelMin    =  1,
    ChannelMax    = 200
  };
 
  inline virtual void create_mlog() const;
};    //end class MmIdHelper
/*******************************************************************************/ 
// For backwards compatibility  
typedef MmIdHelper  MM_ID;

CLASS_DEF(MmIdHelper, 4175, 1)

// Construct ID from components
     inline Identifier MmIdHelper::elementID(int stationName, int stationEta, int stationPhi, bool check, bool* isValid) const {
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (mm_field_value(),result);
  if ( check ) { 
    val = this->validElement(result,stationName,stationEta,stationPhi);
    if ( isValid ) *isValid = val; 
  }
  return result;
}
/*******************************************************************************/ 
inline Identifier MmIdHelper::elementID(std::string stationNameStr, int stationEta, int stationPhi, bool check, bool* isValid) const {
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = elementID(stationName, stationEta, stationPhi, check, isValid);
  return id;
}
/*******************************************************************************/ 
inline Identifier MmIdHelper::elementID(const Identifier& id) const {
  return parentID(id);
}
/*******************************************************************************/ 
inline Identifier MmIdHelper::channelID(int stationName, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool check, bool* isValid) const {
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (mm_field_value(),result);
  m_mplet_impl.pack (multilayer,result);
  m_gap_impl.pack (gasGap,result);
  m_cha_impl.pack (channel,result);
  if ( check ) { 
    val = validChannel(result,stationName,stationEta,stationPhi,multilayer,gasGap,channel);
    if ( isValid ) *isValid = val;
  }
  return result;
}
/*******************************************************************************/ 
inline Identifier MmIdHelper::channelID(std::string stationNameStr, int stationEta, int stationPhi, int multilayer, int gasGap, int channel, bool check, bool * isValid) const {
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = channelID(stationName, stationEta, stationPhi, multilayer, gasGap, channel, check, isValid);
  return id;
}
/*******************************************************************************/ 
inline Identifier MmIdHelper::channelID(const Identifier& id, int multilayer, int gasGap, int channel, bool check, bool* isValid) const {
  Identifier result(id);
  bool val = false;
  m_mplet_impl.pack (multilayer,result);
  m_gap_impl.pack (gasGap,result);
  m_cha_impl.pack (channel,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  }
  return result;
}
/*******************************************************************************/ 
// get parent id from strip or gang identifier
inline Identifier MmIdHelper::parentID(const Identifier& id) const {
  assert(is_mm(id));
  Identifier result(id);
  m_mplet_impl.reset(result);
  m_gap_impl.reset(result);
  m_cha_impl.reset(result);
  return result;
}
/*******************************************************************************/ 
// Access to components of the ID
inline int MmIdHelper::multilayer(const Identifier& id) const
{
  return m_mplet_impl.unpack(id);
}
/*******************************************************************************/ 
inline int MmIdHelper::gasGap(const Identifier& id) const {
  return m_gap_impl.unpack(id);
}
/*******************************************************************************/ 
inline int MmIdHelper::channel(const Identifier& id) const {
  return m_cha_impl.unpack(id);
}
/*******************************************************************************/ 
// Access to min and max of level ranges
inline int MmIdHelper::stationEtaMin() const {
  return StationEtaMin;
}
/*******************************************************************************/ 
inline int MmIdHelper::stationEtaMax() const {
  return StationEtaMax;
}
/*******************************************************************************/ 
inline int MmIdHelper::stationPhiMin() const {
  return StationPhiMin;
}
/*******************************************************************************/ 
inline int MmIdHelper::stationPhiMax() const {
  return StationPhiMax;
}
/*******************************************************************************/ 
inline int MmIdHelper::multilayerMin() const
{
  return MultilayerMin;
}
/*******************************************************************************/ 
inline int MmIdHelper::multilayerMax() const
{
  return MultilayerMax;
}
/*******************************************************************************/ 
inline int MmIdHelper::gasGapMin() const {
  return GasGapMin;
}
/*******************************************************************************/ 
inline bool MmIdHelper::measuresPhi(const Identifier& /*id*/) const{
  return false; 
}
/*******************************************************************************/ 
inline int MmIdHelper::gasGapMax() const {
  return GasGapMax;
}
/*******************************************************************************/ 
inline int MmIdHelper::channelMin() const {
  return ChannelMin;
}
/*******************************************************************************/ 
inline int MmIdHelper::channelMax() const {
  return ChannelMax;
}
/*******************************************************************************/ 
/// Utility methods
inline int MmIdHelper::mmTechnology() const
{
  int mmField = technologyIndex("MM");
  if (m_dict)
    {
      mmField = mm_field_value();
    }
  return mmField;
}
/*******************************************************************************/ 
inline bool MmIdHelper::LargeSector(int stationName) const {
  return ('L' == stationNameString(stationName)[2]);
}
/*******************************************************************************/ 
inline bool MmIdHelper::SmallSector(int stationName) const {
  return ('S' == stationNameString(stationName)[2]);
}
/*******************************************************************************/ 
// Nektar: Modified for MicroMegas, but is almost certainly wrong
inline int MmIdHelper::sectorType(std::string stationName, int stationEta) const {
  if ('L' == stationName[2]) { return (abs(stationEta) + 1 ); }
  else if ('S' == stationName[2]) { return (abs(stationEta) + 12 ); }
  assert(0);
  return -1;
}
/*******************************************************************************/ 
inline int MmIdHelper::sectorType(int stationName, int stationEta) const {
  std::string name = stationNameString(stationName);
  return sectorType(name, stationEta);
}

#endif // MUONIDHELPERS_MMIDHELPER_H
