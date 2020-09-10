/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: TgcIdHelper.h,v 1.32 2009-01-20 22:44:13 kblack Exp $
//<version>   $Name: not supported by cvs2svn $

#ifndef MUONIDHELPERS_TGCIDHELPER_H
#define MUONIDHELPERS_TGCIDHELPER_H

// Includes
class MsgStream;
#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class TgcIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs TGC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.  TgcIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to T1F,T3E,etc.
// StationEta      [-5,-1]             backward endcap (-1 at lowest R)
//                 [1,5]               forward endcap (1 at lowest R)
// StationPhi      [1,48]              endcap: increases with phi
//                 [1,24]              forward: increases with phi
// Technology      [3]                 maps to TGC
// GasGap          [1,2]               doublet: increases with |Z|
//                 [1,3]               triplet: increases with |Z|
// IsStrip         [0,1]               0 if wire, 1 if strip: corresponds to measuresPhi
// Channel         [1,n]               increases with R for IsStrip=0 (wire gang)
//                                     increases with phi for IsStrip=1 (strip)
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
// Compact ID implementation by
// Ketevi A. Assamagan <ketevi@bnl.gov>
// BNL, March 6th, 2003
//
// ******************************************************************************

class TgcIdHelper : public MuonIdHelper
{
 public:

  // Constructor

  TgcIdHelper();

  // Destructor

  virtual ~TgcIdHelper();

  ///////////// compact identifier stuff begins ////////////////////////////////////// 

  /// Initialization from the identifier dictionary
  virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);
  virtual int get_module_hash     (const Identifier& id,
				   IdentifierHash& hash_id ) const;
  virtual int get_detectorElement_hash     (const Identifier& id,
					    IdentifierHash& hash_id ) const;

  ///////////// compact identifier stuff ends   ////////////////////////////////////// 

  // Identifier builders

  Identifier elementID(int stationName, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(const std::string& stationNameStr, int stationEta,
		       int stationPhi, bool check=false, bool* isValid=0) const ;
  Identifier elementID(const Identifier& channelID) const ;
  Identifier channelID(int stationName, int stationEta, int stationPhi,
		       int gasGap, int isStrip, int channel, bool check=false, bool* isValid=0) const ;
  Identifier channelID(const std::string& stationNameStr, int stationEta,
		       int stationPhi, int gasGap, int isStrip, int channel, bool check=false, bool* isValid=0) const ;
  Identifier channelID(const Identifier& id, int gasGap, int isStrip,
		       int channel, bool check=false, bool* isValid=0) const ;

  Identifier parentID (const Identifier& id) const;

  //for an Identifier id, get the list of the daughter readout channel ids
  void idChannels (const Identifier& id, std::vector<Identifier>& vect) const;

  // Access to levels: missing field returns 0 

  int gasGap(const Identifier& id) const;
    
  /** isStrip corresponds to measuresPhi */
  int isStrip(const Identifier& id) const;
  bool measuresPhi(const Identifier& id) const;

  int channel(const Identifier& id) const;

  // Access to min and max of level ranges
  // to be remove when we moved to compact ids

  int stationEtaMin() const;
  int stationEtaMax() const;
  int stationPhiMin(bool endcap) const;
  int stationPhiMax(bool endcap) const;
  int gasGapMin() const;
  int gasGapMax(bool triplet) const;
  int isStripMin() const;
  int isStripMax() const;
  int channelMin() const;
  int channelMax() const;


  // Access to min and max of level ranges

  int stationEtaMin (const Identifier& id) const;
  int stationEtaMax (const Identifier& id) const;
  int stationPhiMin (const Identifier& id) const;
  int stationPhiMax (const Identifier& id) const;
  int gasGapMin     (const Identifier& id) const;
  int gasGapMax     (const Identifier& id) const;
  int isStripMin    (const Identifier& id) const;
  int isStripMax    (const Identifier& id) const;
  int channelMin    (const Identifier& id) const;
  int channelMax    (const Identifier& id) const;

  // Utility methods

  int chamberType(std::string stationName, int stationEta) const;
  int chamberType(int stationName, int stationEta) const;

  // Public validation of levels

  bool valid(const Identifier& id) const;
  bool validElement(const Identifier& id) const;

 private:

  int init_id_to_hashes();
  unsigned int m_module_hashes[60][20][48];

  // compact id indices
  size_type   m_GASGAP_INDEX; 
  size_type   m_ISSTRIP_INDEX;

  IdDictFieldImplementation   m_gap_impl;
  IdDictFieldImplementation   m_ist_impl;
  IdDictFieldImplementation   m_cha_impl;

  // Check level values

  bool validElement(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi) const;
  bool validChannel(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi,int gasGap, int isStrip, 
		    int channel) const;

  // Utility methods

  int  tgcTechnology() const;
  bool endcapChamber(int stationName) const;
  bool tripletChamber(int stationName) const;

  // Level indices

  enum TgcIndices
    {
      GasGapIndex  = 5,
      IsStripIndex = 6,
      ChannelIndex = 7
    };

  // Level ranges

  enum TgcRanges
    {
      StationEtaMin        =  -8,
      StationEtaMax        =   8,
      StationPhiEndcapMin  =   1,
      StationPhiEndcapMax  =  48,
      StationPhiForwardMin =   1,
      StationPhiForwardMax =  24,
      GasGapMin            =   1,
      GasGapDoubletMax     =   2,
      GasGapTripletMax     =   3,
      IsStripMin           =   0,
      IsStripMax           =   1,
      ChannelMin           =   1,
      ChannelMax           = 135
    };
};

// For backwards compatibility

typedef TgcIdHelper  TGC_ID;

CLASS_DEF(TgcIdHelper, 4173, 1)

// Construct ID from components

     inline Identifier TgcIdHelper::elementID(int stationName, int stationEta, int stationPhi, 
					      bool check, bool* isValid) const {
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (tgc_field_value(),result);
  if ( check ) { 
    val = this->validElement(result,stationName,stationEta,stationPhi);
    if ( isValid ) *isValid = val; 
  }
  return result;
}

inline Identifier TgcIdHelper::elementID(const std::string& stationNameStr, int stationEta,
					 int stationPhi, bool check, bool* isValid) const {

  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = elementID(stationName, stationEta, stationPhi, check, isValid);
  return id;
}

inline Identifier TgcIdHelper::elementID(const Identifier& id) const {

  return parentID(id);
}

inline Identifier TgcIdHelper::channelID(int stationName, int stationEta, int stationPhi,
					 int gasGap, int isStrip, int channel, bool check, bool* isValid) const {
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (tgc_field_value(),result);
  m_gap_impl.pack (gasGap,result);
  m_ist_impl.pack (isStrip,result);
  m_cha_impl.pack (channel,result);
  if ( check ) { 
    val = validChannel(result,stationName,stationEta,stationPhi,
		       gasGap,isStrip,channel);
    if ( isValid ) *isValid = val;
  }
  return result;
}

inline Identifier TgcIdHelper::channelID(const std::string& stationNameStr, int stationEta,
					 int stationPhi, int gasGap, int isStrip,
					 int channel, bool check, bool * isValid) const {
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = channelID(stationName, stationEta, stationPhi, gasGap, isStrip, channel, check, isValid);
  return id;
}

inline Identifier TgcIdHelper::channelID(const Identifier& id,
					 int gasGap, int isStrip, int channel, bool check, bool* isValid) const {

  Identifier result(id);
  bool val = false;
  m_gap_impl.pack (gasGap,result);
  m_ist_impl.pack (isStrip,result);
  m_cha_impl.pack (channel,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  }
  return result;

}

// get parent id from strip or gang identifier
inline Identifier TgcIdHelper::parentID(const Identifier& id) const {
  assert(is_tgc(id));
  Identifier result(id);
  m_gap_impl.reset(result);
  m_ist_impl.reset(result);
  m_cha_impl.reset(result);
  return result;
}

// Access to components of the ID

inline  int TgcIdHelper::gasGap(const Identifier& id) const {

  return m_gap_impl.unpack(id);
}

/** returns measuresPhi */
inline  int TgcIdHelper::isStrip(const Identifier& id) const {

  return m_ist_impl.unpack(id);
}

inline  bool TgcIdHelper::measuresPhi(const Identifier& id) const {

  return isStrip(id);
}

inline  int TgcIdHelper::channel(const Identifier& id) const {

  return m_cha_impl.unpack(id);
}

// Access to min and max of level ranges

inline  int TgcIdHelper::stationEtaMin() const
{
  return StationEtaMin;
}

inline  int TgcIdHelper::stationEtaMax() const
{
  return StationEtaMax;
}

inline  int TgcIdHelper::stationPhiMin(bool endcap) const
{
  if (endcap)
    {
      return StationPhiEndcapMin;
    }
  else
    {
      return StationPhiForwardMin;
    }
}

inline  int TgcIdHelper::stationPhiMax(bool endcap) const
{
  if (endcap)
    {
      return StationPhiEndcapMax;
    }
  else
    {
      return StationPhiForwardMax;
    }
}

inline  int TgcIdHelper::gasGapMin() const
{
  return GasGapMin;
}

inline  int TgcIdHelper::gasGapMax(bool triplet) const
{
  if (triplet)
    {
      return GasGapTripletMax;
    }
  else
    {
      return GasGapDoubletMax;
    }
}

inline  int TgcIdHelper::isStripMin() const
{
  return IsStripMin;
}

inline  int TgcIdHelper::isStripMax() const
{
  return IsStripMax;
}

inline  int TgcIdHelper::channelMin() const
{
  return ChannelMin;
}

inline  int TgcIdHelper::channelMax() const
{
  return ChannelMax;
}
/// Utility methods

inline int TgcIdHelper::tgcTechnology() const
{
  int tgcField = technologyIndex("TGC");
  if (m_dict)
    {
      tgcField = tgc_field_value();
    }
  return tgcField;
}

inline bool TgcIdHelper::endcapChamber(int stationName) const
{
  std::string name = stationNameString(stationName);
  return ('E' == stationNameString(stationName)[2]);
}

inline bool TgcIdHelper::tripletChamber(int stationName) const
{
  std::string name = stationNameString(stationName);
  return ('1' == name[1]);
}

inline  int TgcIdHelper::chamberType(std::string stationName, int stationEta) const
{
  if ('1' == stationName[1])
    {
      if ('F' == stationName[2]) return 1;
      else return (abs(stationEta) + 1);
    }
  else if ('2' == stationName[1])
    {
      if ('F' == stationName[2]) return 6;
      else return (abs(stationEta) + 6);
    }
  else if ('3' == stationName[1])
    {
      if ('F' == stationName[2]) return 12;
      else return (abs(stationEta) + 12);
    }
  else if ('4' == stationName[1])
    {
      if ('F' == stationName[2]) return 18;
      else return (abs(stationEta) + 18);
    }
  assert(0);
  return -1;
}

inline  int TgcIdHelper::chamberType(int stationName, int stationEta) const {

  std::string name = stationNameString(stationName);
  return chamberType(name, stationEta);
}

#endif // MUONIDHELPERS_TGCIDHELPER_H

