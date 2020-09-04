/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// ATLAS Muon Identifier Helpers Package
// ******************************************************************************

#ifndef MUONIDHELPERS_CSCIDHELPER_H
#define MUONIDHELPERS_CSCIDHELPER_H

#include "MuonIdHelpers/MuonIdHelper.h"

// ******************************************************************************
// class CscIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs CSC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2001-014 provides a complete description
// of the hierarchical identifier scheme.  CscIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to "CSS", "CSL", etc.
// StationEta      [-1,1]              -1 for backward, 1 for forward endcap
// StationPhi      [1,8]               increases with Phi
// Technology      [1]                 maps to "CSC"
// ChamberLayer    [1,2]               increases with |Z|
// WireLayer       [1,4]               increases with |Z|
// MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
// Strip           [1,n]               increases with R   for MeasuresPhi=0
//                                     increases with Phi for MeasuresPhi=1
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
// BNL, February 27th, 2003
//
// ******************************************************************************

class CscIdHelper : public MuonIdHelper
{
 public:

  // Constructor

  CscIdHelper();

  // Destructor

  virtual ~CscIdHelper();

  ///////////// compact identifier stuff begins ////////////////////////////////////// 

  /// Initialization from the identifier dictionary
  int initialize_from_dictionary(const IdDictMgr& dict_mgr) override;

  // need to overwrite get_module_hash and get_detectorElement_hash for Run2 geometries (since they contain both 
  // CSC chamberLayer 1 and 2 although only chamberLayer 2 is actually built into ATLAS)
  // function checks whether chamberLayer 1 identifiers are around and in this case returns the correct module hash
  int get_module_hash(const Identifier& id, IdentifierHash& hash_id ) const override;
  int get_detectorElement_hash (const Identifier& id, IdentifierHash& hash_id) const override;
  
  // in some parts of athena (still) hashes which encode geometrical information for the CSCs are around,
  // therefore, need those additional hash functions here (feel free to fix it in the future)
  int get_geo_module_hash(const Identifier& id, IdentifierHash& hash_id ) const;
  int get_geo_detectorElement_hash(const Identifier& id, IdentifierHash& hash_id ) const;
  int get_geo_channel_hash(const Identifier&, IdentifierHash&) const;

  int get_hash_fromGeoHash(const IdentifierHash& geoHash, IdentifierHash& realHash, const IdContext* context) const;

  ///////////// compact identifier stuff ends ////////////////////////////////////// 
  

  // Identifier builders

  Identifier elementID(int stationName, int stationEta, int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(const std::string& stationNameStr, int stationEta,
		       int stationPhi, bool check=false, bool* isValid=0) const;
  Identifier elementID(const Identifier& channelID) const;
  Identifier channelID(int stationName, int stationEta,
		       int stationPhi, int chamberLayer, int wireLayer,
		       int measuresPhi, int strip, bool check=false, bool* isValid=0) const;
  Identifier channelID(const std::string& stationNameStr, int stationEta,
		       int stationPhi, int chamberLayer, int wireLayer,
		       int measuresPhi, int strip, bool check=false, bool* isValid=0) const;
  Identifier channelID(const Identifier& id, int chamberLayer, int wireLayer,
		       int measurePhi, int strip, bool check=false, bool* isValid=0) const;

  Identifier parentID (const Identifier& id) const;

  //for an Identifier id, get the list of the daughter readout channel ids
  void idChannels (const Identifier& id, std::vector<Identifier>& vect) const;

  // Access to levels: missing field returns 0 

  int channel(const Identifier& id) const override;

  int chamberLayer(const Identifier& id) const;
  int wireLayer(const Identifier& id) const;
  bool measuresPhi(const Identifier& id) const override;
  int strip(const Identifier& id) const;
  int gasGap(const Identifier& id) const override; //Returns chamber Layer here

  int sector(const Identifier& id) const;
  // Access to min and max of level ranges
  // to be remove when we moved to compact ids

  int stationEtaMin() const;
  int stationEtaMax() const;
  int stationPhiMin() const;
  int stationPhiMax() const;
  int chamberLayerMin() const;
  int chamberLayerMax() const;
  int wireLayerMin() const;
  int wireLayerMax() const;
  int measuresPhiMin() const;
  int measuresPhiMax() const;
  int stripMin() const;
  int stripMax() const;

  // Access to min and max of level ranges
 
  int stationEtaMin  (const Identifier& id) const;
  int stationEtaMax  (const Identifier& id) const;
  int stationPhiMin  (const Identifier& id) const;
  int stationPhiMax  (const Identifier& id) const;
  int chamberLayerMin(const Identifier& id) const;
  int chamberLayerMax(const Identifier& id) const;
  int wireLayerMin   (const Identifier& id) const;
  int wireLayerMax   (const Identifier& id) const;
  int measuresPhiMin (const Identifier& id) const;
  int measuresPhiMax (const Identifier& id) const;
  int stripMin       (const Identifier& id) const;
  int stripMax       (const Identifier& id) const;

  // Public validation of levels

  bool valid(const Identifier& id) const;
  bool validElement(const Identifier& id) const;

 private:

  int init_id_to_hashes();
  unsigned int m_module_hashes[60][3][8];
  unsigned int m_detectorElement_hashes[60][3][8][2];

  // compact id indices
  size_type   m_CHAMBERLAYER_INDEX; 
  size_type   m_WIRELAYER_INDEX;
  size_type   m_MEASURESPHI_INDEX;

  IdDictFieldImplementation   m_cla_impl;
  IdDictFieldImplementation   m_lay_impl;
  IdDictFieldImplementation   m_mea_impl;
  IdDictFieldImplementation   m_str_impl;

  int get_geo_hash_calc   (const Identifier& compact_id, IdentifierHash& hash_id, const IdContext* context) const;

  // Private validation of levels

  bool validElement(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi) const;
  bool validChannel(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi,int chamberLayer, int wireLayer, 
		    int measuresPhi, int strip) const;

  // Utility methods

  int cscTechnology() const;

  // hash offset of strips */
  int strip_hash_offsets();

  // Level indices

  enum CscIndices
    {
      ChamberLayerIndex = 5,
      WireLayerIndex    = 6,
      MeasuresPhiIndex  = 7,
      StripIndex        = 8
    };

  // Level ranges

  enum CscRanges
    {
      StationEtaMin   = -1,
      StationEtaMax   =  1,
      StationPhiMin   =  1,
      StationPhiMax   =  8,
      ChamberLayerMin =  1,
      ChamberLayerMax =  2,
      WireLayerMin    =  1,
      WireLayerMax    =  4,
      MeasuresPhiMin  =  0,
      MeasuresPhiMax  =  1,
      StripMin        =  1,
      StripMax        = 216 // up to AMDB P 
    };
  int m_hashOffset[2][2];

  unsigned int m_stripMaxPhi; // maximum number of strips for layer which measuresPhi
  unsigned int m_stripMaxEta; // maximum number of strips for layer which does not measure phi
  bool m_hasChamLay1;
};

// For backwards compatibility

typedef CscIdHelper  CSC_ID;

CLASS_DEF(CscIdHelper, 4171, 1)

/// Construct Atlas Identifiers from components

     inline Identifier CscIdHelper::elementID(int stationName,
					      int stationEta, int stationPhi,
					      bool check, bool* isValid) const
{

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (csc_field_value(),result);
  if ( check ) {
    val = this->validElement(result,stationName,stationEta,stationPhi);
    if ( isValid ) *isValid = val;
  }
  return result;
}

inline Identifier CscIdHelper::elementID(const std::string& stationNameStr,
					 int stationEta, int stationPhi, bool check, bool* isValid) const
{
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = elementID(stationName, stationEta, stationPhi, check, isValid);
  return id;
}

inline Identifier CscIdHelper::elementID(const Identifier& id) const
{
  return parentID(id);
}

inline Identifier CscIdHelper::channelID(int stationName, int stationEta, int stationPhi,
					 int chamberLayer, int wireLayer, int measuresPhi,
					 int strip, bool check, bool* isValid) const
{
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (csc_field_value(),result);
  m_cla_impl.pack (chamberLayer,result);
  m_lay_impl.pack (wireLayer,result);
  m_mea_impl.pack (measuresPhi,result);
  m_str_impl.pack (strip,result);
  if ( check ) {
    val = this->validChannel(result,stationName,stationEta,stationPhi,
			     chamberLayer,wireLayer,measuresPhi,strip);
    if ( isValid ) *isValid = val;
  }
  return result;
}

inline Identifier CscIdHelper::channelID(const std::string& stationNameStr, int stationEta,
					 int stationPhi, int chamberLayer, int wireLayer,
					 int measuresPhi, int strip, bool check, bool* isValid) const
{
  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = channelID(stationName, stationEta, stationPhi, chamberLayer,
		 wireLayer, measuresPhi, strip, check, isValid);
  return id;
}

inline Identifier CscIdHelper::channelID(const Identifier& id, int chamberLayer,
					 int wireLayer, int measuresPhi, int strip, bool check, bool* isValid) const
{
  Identifier result(id);
  bool val = false;
  m_cla_impl.pack (chamberLayer,result);
  m_lay_impl.pack (wireLayer,result);
  m_mea_impl.pack (measuresPhi,result);
  m_str_impl.pack (strip,result);
  if ( check ) { 
    val = this->valid(result);
    if ( isValid ) *isValid = val; 
  }
  return result;
}

/// get parent id from channel id

inline Identifier CscIdHelper::parentID(const Identifier& id) const
{
  assert(is_csc(id));
  Identifier result(id);
  m_cla_impl.reset(result);
  m_lay_impl.reset(result);
  m_mea_impl.reset(result);
  m_str_impl.reset(result);
  return result;
}

// Access to components of the ID

inline int CscIdHelper::chamberLayer(const Identifier& id) const
{

  return m_cla_impl.unpack(id);

}


inline int CscIdHelper::wireLayer(const Identifier& id) const
{

  return m_lay_impl.unpack(id);

}

inline bool CscIdHelper::measuresPhi(const Identifier& id) const
{

  return m_mea_impl.unpack(id);

}

inline int CscIdHelper::strip(const Identifier& id) const
{

  return m_str_impl.unpack(id);

}

inline int CscIdHelper::channel(const Identifier& id) const
{

  return strip(id);

}

/// Access to min and max of level ranges

inline int CscIdHelper::stationEtaMin() const
{
  return StationEtaMin;
}

inline int CscIdHelper::stationEtaMax() const
{
  return StationEtaMax;
}

inline int CscIdHelper::stationPhiMin() const
{
  return StationPhiMin;
}

inline int CscIdHelper::stationPhiMax() const
{
  return StationPhiMax;
}

inline int CscIdHelper::chamberLayerMin() const
{
  return ChamberLayerMin;
}

inline int CscIdHelper::chamberLayerMax() const
{
  return ChamberLayerMax;
}

inline int CscIdHelper::wireLayerMin() const
{
  return WireLayerMin;
}

inline int CscIdHelper::wireLayerMax() const
{
  return WireLayerMax;
}

inline int CscIdHelper::measuresPhiMin() const
{
  return MeasuresPhiMin;
}

inline int CscIdHelper::measuresPhiMax() const
{
  return MeasuresPhiMax;
}

inline int CscIdHelper::stripMin() const
{
  return StripMin;
}

inline int CscIdHelper::stripMax() const
{
  return StripMax;
}

/// Utility methods

inline int CscIdHelper::cscTechnology() const
{
  int cscField = technologyIndex("CSC");
  if (m_dict)
    {
      cscField = csc_field_value();
    }
  return cscField;  
}


inline int CscIdHelper::sector(const Identifier& id) const {
  return stationEta(id) * ( 2*stationPhi(id) - (stationName(id)-49) + 1);
}


inline  int CscIdHelper::gasGap(const Identifier& id) const
{

  return chamberLayer(id);

}



#endif // MUONIDHELPERS_CSCIDHELPER_H

