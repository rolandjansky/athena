/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONIDHELPERS_RPCIDHELPER_H
#define MUONIDHELPERS_RPCIDHELPER_H

#include "MuonIdHelpers/MuonIdHelper.h"

class MsgStream;
// ******************************************************************************
// class RpcIdHelper
// ******************************************************************************
//
// Description
// -----------
// This factory class constructs RPC identifiers and ranges and provides access
// to the levels.  ATLAS note ATL-MUON-2002-??? provides a complete description
// of the hierarchical identifier scheme.  RpcIdHelper provides an interface to the
// following fields of the identifier.
//
// Field           Range               Notes
// ==============================================================================
// StationName     unsigned integer    maps to "BIL","BMS",etc.
// StationEta      [-8,8]              increases with Z
// StationPhi      [1,8]               increases with phi
// Technology      [2]                 maps to "RPC"
// DoubletR        [1,2]               increases with R
// DoubletZ        [1,3]               increases with |Z|
// DoubletPhi      [1,2]               increases with phi
// GasGap          [1,2]               increases with R
// MeasuresPhi     [0,1]               0 if measures Z, 1 if measures phi
// Strip           [1,n]               increases with Z for MeasuresPhi=0
//                                     increases with phi for MeasuresPhi=1
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
// BNL, March 11th, 2003
//
// ******************************************************************************

class RpcIdHelper : public MuonIdHelper
{
 public:

  RpcIdHelper();
  virtual ~RpcIdHelper()=default;

  ///////////// compact identifier stuff begins ////////////////////////////////////// 

  /// Initialization from the identifier dictionary
  virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);
  virtual int get_module_hash     (const Identifier& id,
				   IdentifierHash& hash_id ) const;
  virtual int get_detectorElement_hash (const Identifier& id,
					IdentifierHash& hash_id ) const;

  ///////////// compact identifier stuff ends   ////////////////////////////////////// 

  // Identifier builders

  Identifier elementID(int stationName, int stationEta, int stationPhi,
		       int doubletR, bool check=false, bool* isValid=0, bool noPrint=false) const;
  Identifier elementID(std::string stationNameStr, int stationEta,
		       int stationPhi, int doubletR, bool check=false, bool* isValid=0) const;
  Identifier elementID(const Identifier& elementID, int doubletR, bool check=false, bool* isValid=0) const;
  Identifier elementID(const Identifier& channelID) const;
  Identifier channelID(int stationName, int stationEta, int stationPhi,
		       int doubletR, int doubletZ, int doubletPhi,
		       int gasGap, int measuresPhi, int strip, bool check=false, bool* isValid=0, bool noPrint=false) const;
  Identifier channelID(std::string stationNameStr, int stationEta,
		       int stationPhi, int doubletR, int doubletZ,
		       int doubletPhi, int gasGap, int measuresPhi, int strip, bool check=false, bool* isValid=0) const;
  Identifier channelID(const Identifier& id,  int doubletZ, int doubletPhi,
		       int gasGap, int measuresPhi, int strip, bool check=false, bool* isValid=0) const;

  Identifier parentID (const Identifier& id) const;
  Identifier doubletZID(const Identifier& id) const;

  Identifier padID    (const Identifier& elementID, int doubletZ, int doubletPhi, bool check=false, bool* isValid=0) const;
  Identifier padID    (int stationName, int stationEta, int stationPhi, int doubletR,
		       int doubletZ, int doubletPhi, bool check=false, bool* isValid=0) const;

  Identifier panelID  (const Identifier& padID, int gasGap, int measuresPhi, bool check=false, bool* isValid=0) const;
  Identifier panelID  (const Identifier& channelID) const;
  Identifier panelID  (int stationName, int stationEta, int stationPhi, int doubletR,
		       int doubletZ, int doubletPhi,int gasGap, int measuresPhi, bool check=false, bool* isValid=0) const;

  Identifier gapID  (const Identifier& padID, int gasGap, bool check=false, bool* isValid=0) const;
  Identifier gapID  (const Identifier& panelID) const;
  Identifier gapID  (int stationName, int stationEta, int stationPhi, int doubletR,
		     int doubletZ, int doubletPhi,int gasGap, bool check=false, bool* isValid=0) const;



  //for an Identifier id, get the list of the daughter detector channel ids
  void idChannels (const Identifier& id, std::vector<Identifier>& vect) const;

  // Access to levels: missing field returns 0 

  int channel(const Identifier& id) const;

  int doubletR(const Identifier& id) const;
  int doubletZ(const Identifier& id) const;
  int doubletPhi(const Identifier& id) const;
  int gasGap(const Identifier& id) const;
  bool measuresPhi(const Identifier& id) const;
  int strip(const Identifier& id) const;

  // Access to min and max of level ranges

  int stationEtaMin() const;
  int stationEtaMax() const;
  int stationPhiMin() const;
  int stationPhiMax() const;
  int doubletRMin() const;
  int doubletRMax() const;
  int doubletZMin() const;
  int doubletZMax() const;
  int doubletPhiMin() const;
  int doubletPhiMax() const;
  int gasGapMin() const;
  int gasGapMax() const;
  int measuresPhiMin() const;
  int measuresPhiMax() const;
  int stripMin() const;
  int stripMax() const;

  // Access to min and max of level ranges

  int stationEtaMin (const Identifier& id) const;
  int stationEtaMax (const Identifier& id) const;
  int stationPhiMin (const Identifier& id) const;
  int stationPhiMax (const Identifier& id) const;
  int doubletRMin   (const Identifier& id) const;
  int doubletRMax   (const Identifier& id) const;
  int doubletZMin   (const Identifier& id) const;
  int doubletZMax   (const Identifier& id) const;
  int doubletPhiMin (const Identifier& id) const;
  int doubletPhiMax (const Identifier& id) const;
  int gasGapMin     (const Identifier& id) const;
  int gasGapMax     (const Identifier& id) const;
  int measuresPhiMin(const Identifier& id) const;
  int measuresPhiMax(const Identifier& id) const;
  int stripMin      (const Identifier& id) const;
  int stripMax      (const Identifier& id) const;

  // Public validation of levels

  bool valid(const Identifier& id) const;
  bool validElement(const Identifier& id) const;
  bool validPad(const Identifier& id) const;

 private:

  virtual int init_detectorElement_hashes(void);
  int init_id_to_hashes();
  int zIndex(const Identifier& id) const;
  int zIndex(std::string name, int eta, int dR, int dZ, int dP) const;
  unsigned int m_module_hashes[60][20][8][2];
  unsigned int m_detectorElement_hashes[60][20][8][2][4];

  // compact id indices
  size_type	m_DOUBLETR_INDEX; 
  size_type   m_DOUBLETZ_INDEX;
  size_type	m_DOUBLETPHI_INDEX; 
  size_type   m_GASGAP_INDEX;
  size_type   m_MEASURESPHI_INDEX;

  IdDictFieldImplementation   m_dbr_impl;
  IdDictFieldImplementation   m_dbz_impl;
  IdDictFieldImplementation   m_dbp_impl;
  IdDictFieldImplementation   m_gap_impl;
  IdDictFieldImplementation   m_mea_impl;
  IdDictFieldImplementation   m_str_impl;


  // Private validation of levels

  bool validElement(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi,int doubletR, bool noPrint=false) const;
  bool validChannel(const Identifier& id, int stationName, int stationEta, 
		    int stationPhi,int doubletR, int doubletZ, 
		    int doubletPhi, int gasGap,int measuresPhi, 
		    int strip, bool noPrint=false) const;
  bool validPad    (const Identifier& id, int stationName, int stationEta, 
		    int stationPhi,int doubletR, int doubletZ, 
		    int doubletPhi) const;

  // Utility methods

  int rpcTechnology() const;

  // Level indices

  enum RpcIndices
    {
      DoubletRIndex    = 5, 
      DoubletZIndex    = 6,
      DoubletPhiIndex  = 7,
      GasGapIndex      = 8,
      MeasuresPhiIndex = 9,
      StripIndex       = 10
    };

  // Level ranges

  enum RpcRanges
    {
      StationEtaMin  = -8,
      StationEtaMax  =  8,
      StationPhiMin  =  1,
      StationPhiMax  =  8,
      DoubletRMin    =  1,
      DoubletRMax    =  2,
      DoubletZMin    =  1,
      DoubletZMax    =  3,
      DoubletPhiMin  =  1,
      DoubletPhiMax  =  2,
      GasGapMin      =  1,
      GasGapMax      =  4,
      MeasuresPhiMin =  0,
      MeasuresPhiMax =  1,
      StripMin       =  1,
      StripMax       = 99
    };
    unsigned int m_gasGapMax; // maximum number of gas gaps

};

// For backwards compatibility

typedef RpcIdHelper  RPC_ID;

CLASS_DEF(RpcIdHelper, 4172, 1)

// Construct ID from components

     inline Identifier RpcIdHelper::elementID(int stationName, int stationEta, int stationPhi,
					      int doubletR, bool check, bool* isValid, bool noPrint) const {

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (rpc_field_value(),result);
  m_dbr_impl.pack (doubletR,result);
  if ( check ) {
    val = this->validElement(result,stationName,stationEta,
			     stationPhi,doubletR,noPrint);
    if ( isValid ) *isValid = val;
  }
  return result;

}

inline Identifier RpcIdHelper::elementID(std::string stationNameStr, int stationEta,
					 int stationPhi, int doubletR, bool check, bool* isValid) const {

  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = elementID(stationName, stationEta, stationPhi, doubletR, check, isValid);
  return id;
}

inline Identifier RpcIdHelper::elementID(const Identifier& id, int doubletR, bool check, bool* isValid) const {

  Identifier result(id);
  bool val = false;
  m_dbr_impl.pack(doubletR,result);
  if ( check ) {
    val = this->validElement(result);
    if ( isValid ) *isValid = val;
  } 

  return result;

}

inline Identifier RpcIdHelper::elementID(const Identifier& id) const
{
  return parentID(id);
}


/*     Identifier panelID  (const Identifier& padID, int gasGap, bool check=false, bool* isValid=0) const; */
/*     Identifier panelID  (const Identifier& channelID, bool check=false, bool* isValid=0) const; */
/*     Identifier panelID  (int stationName, int stationEta, int stationPhi, int doubletR, */
/* 		         int doubletZ, int doubletPhi,int gasGap, bool check=false, bool* isValid=0) const; */




inline Identifier RpcIdHelper::panelID(int stationName, int stationEta, int stationPhi,
				       int doubletR, int doubletZ, int doubletPhi,
				       int gasGap, int measuresPhi,
				       bool check, bool* isValid) const
{

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (rpc_field_value(),result);
  m_dbr_impl.pack (doubletR,result);
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  m_gap_impl.pack (gasGap,result);
  m_mea_impl.pack (measuresPhi,result);
  if ( check ) {
    val = this->valid(result);  
    if ( isValid ) *isValid = val;
  }
  return result;

}



inline Identifier RpcIdHelper::panelID(const Identifier& channelID) const
{

  Identifier result(channelID);
  m_str_impl.reset(result);
  return result;

}

inline Identifier RpcIdHelper::panelID(const Identifier& padID, int gasGap, int measuresPhi,bool check, bool* isValid) const
{

  Identifier result(padID);
  bool val = false;
  m_gap_impl.pack (gasGap,result);
  m_mea_impl.pack (measuresPhi,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  } 
  return result;

}


inline Identifier RpcIdHelper::gapID(int stationName, int stationEta, int stationPhi,
				     int doubletR, int doubletZ, int doubletPhi,
				     int gasGap,
				     bool check, bool* isValid) const
{

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (rpc_field_value(),result);
  m_dbr_impl.pack (doubletR,result);
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  m_gap_impl.pack (gasGap,result);
  if ( check ) {
    val = this->valid(result);  
    if ( isValid ) *isValid = val;
  }
  return result;

}


inline Identifier RpcIdHelper::gapID(const Identifier& panelID) const
{

  Identifier result(panelID);
  m_mea_impl.reset(result);
  return result;

}

inline Identifier RpcIdHelper::gapID(const Identifier& padID, int gasGap,bool check, bool* isValid) const
{

  Identifier result(padID);
  bool val = false;
  m_gap_impl.pack (gasGap,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  } 
  return result;

}





inline Identifier RpcIdHelper::channelID(int stationName, int stationEta, int stationPhi,
					 int doubletR, int doubletZ, int doubletPhi,
					 int gasGap, int measuresPhi, int strip,
					 bool check, bool* isValid, bool noPrint) const
{

  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (rpc_field_value(),result);
  m_dbr_impl.pack (doubletR,result);
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  m_gap_impl.pack (gasGap,result);
  m_mea_impl.pack (measuresPhi,result);
  m_str_impl.pack (strip,result);
  if ( check ) {
    val = this->validChannel(result,stationName,stationEta,
			     stationPhi,doubletR,doubletZ,
			     doubletPhi,gasGap,measuresPhi,strip,noPrint);  
    if ( isValid ) *isValid = val;
  }
  return result;

}

inline Identifier RpcIdHelper::channelID(std::string stationNameStr, int stationEta,
					 int stationPhi, int doubletR, int doubletZ,
					 int doubletPhi, int gasGap, int measuresPhi,
					 int strip, bool check, bool* isValid) const {

  Identifier id;
  int stationName = stationNameIndex(stationNameStr);
  id = channelID(stationName, stationEta, stationPhi, doubletR, doubletZ,
		 doubletPhi, gasGap, measuresPhi, strip, check, isValid);
  return id;
}

inline Identifier RpcIdHelper::channelID(const Identifier& id, 
					 int doubletZ, int doubletPhi, int gasGap,
					 int measuresPhi, int strip, bool check, bool* isValid) const {

  // pack fields independently
  Identifier result(id);
  bool val = false;
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  m_gap_impl.pack (gasGap,result);
  m_mea_impl.pack (measuresPhi,result);
  m_str_impl.pack (strip,result);
  if ( check ) {
    val = this->valid(result);
    if ( isValid ) *isValid = val;
  } 
  return result;
}

// get the parent id from the strip identifier
inline Identifier RpcIdHelper::parentID(const Identifier& id) const {
  assert(is_rpc(id));
  Identifier result(id);
  m_dbz_impl.reset(result);
  m_dbp_impl.reset(result);
  m_gap_impl.reset(result);
  m_mea_impl.reset(result);
  m_str_impl.reset(result);
  return result;
}

// doubletZ Identifier
inline Identifier RpcIdHelper::doubletZID(const Identifier& id) const {
  assert(is_rpc(id));
  Identifier result(id);
  m_dbp_impl.reset(result);
  m_gap_impl.reset(result);
  m_mea_impl.reset(result);
  m_str_impl.reset(result);
  return result;
}

inline Identifier RpcIdHelper::padID(int stationName, int stationEta, int stationPhi,
				     int doubletR, int doubletZ, int doubletPhi, bool check, bool* isValid) const
{
  // pack fields independently
  Identifier result((Identifier::value_type)0);
  bool val = false;
  m_muon_impl.pack(muon_field_value(),result);
  m_sta_impl.pack (stationName,result);
  m_eta_impl.pack (stationEta,result);
  m_phi_impl.pack (stationPhi,result);
  m_tec_impl.pack (rpc_field_value(),result);
  m_dbr_impl.pack (doubletR,result);
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  if ( check ) {
    val = this->validPad(result,stationName,stationEta,
			 stationPhi,doubletR,doubletZ,doubletPhi);
    if ( isValid ) *isValid = val;
  }
  return result;

}

inline Identifier RpcIdHelper::padID(const Identifier& id, 
				     int doubletZ, int doubletPhi, bool check, bool* isValid) const {

  // pack fields independently
  Identifier result(id);
  bool val = false;
  m_dbz_impl.pack (doubletZ,result);
  m_dbp_impl.pack (doubletPhi,result);
  if ( check ) {
    val = this->validPad(result);
    if ( isValid ) *isValid = val;
  }
  return result;

}

// Access to components of the ID

inline  int RpcIdHelper::doubletR(const Identifier& id) const {

  return m_dbr_impl.unpack(id);

}

inline  int RpcIdHelper::doubletZ(const Identifier& id) const {

  return m_dbz_impl.unpack(id);

}

inline  int RpcIdHelper::doubletPhi(const Identifier& id) const {

  return m_dbp_impl.unpack(id);

}

inline  int RpcIdHelper::gasGap(const Identifier& id) const {

  return m_gap_impl.unpack(id);

}

inline  bool RpcIdHelper::measuresPhi(const Identifier& id) const {

  return m_mea_impl.unpack(id);

}

inline  int RpcIdHelper::strip(const Identifier& id) const {

  return m_str_impl.unpack(id);

}

inline  int RpcIdHelper::channel(const Identifier& id) const {

  return strip(id);

}

// Access to min and max of level ranges

inline  int RpcIdHelper::stationEtaMin() const
{
  return StationEtaMin;
}

inline  int RpcIdHelper::stationEtaMax() const
{
  return StationEtaMax;
}

inline  int RpcIdHelper::stationPhiMin() const
{
  return StationPhiMin;
}

inline int RpcIdHelper::stationPhiMax() const
{
  return StationPhiMax;
}

inline int RpcIdHelper::doubletRMin() const
{
  return DoubletRMin;
}

inline int RpcIdHelper::doubletRMax() const
{
  return DoubletRMax;
}

inline int RpcIdHelper::doubletZMin() const
{
  return DoubletZMin;
}

inline int RpcIdHelper::doubletZMax() const
{
  return DoubletZMax;
}

inline int RpcIdHelper::doubletPhiMin() const
{
  return DoubletPhiMin;
}

inline int RpcIdHelper::doubletPhiMax() const
{
  return DoubletPhiMax;
}

inline  int RpcIdHelper::gasGapMin() const
{
  return GasGapMin;
}

inline  int RpcIdHelper::gasGapMax() const
{
  return m_gasGapMax;
}

inline  int RpcIdHelper::measuresPhiMin() const
{
  return MeasuresPhiMin;
}

inline  int RpcIdHelper::measuresPhiMax() const
{
  return MeasuresPhiMax;
}

inline int RpcIdHelper::stripMin() const
{
  return StripMin;
}

inline int RpcIdHelper::stripMax() const
{
  return StripMax;
}

/// Utility methods

inline int RpcIdHelper::rpcTechnology() const
{
  int rpcField = technologyIndex("RPC");
  if (m_dict)
    {
      rpcField = rpc_field_value();
    }
  return rpcField;
}

inline int RpcIdHelper::zIndex(const Identifier& id) const {
  int station      = stationName(id);
  int eta          = stationEta(id);
  int dR           = doubletR(id);
  int dZ           = doubletZ(id);
  int dP           = doubletPhi(id);
  std::string name = stationNameString(station); 
  return zIndex(name, eta, dR, dZ, dP);
}

inline int RpcIdHelper::zIndex(std::string name, int eta, int dR, int dZ, int dP) const {

  /** - from Stefania 
      BMS5 which has the following structure:
      for dbr=1 there are 3 dbZ, first and second are made of a single
      RpcReadoutElement and the 3rd is made of 2 separate
      RpcReadoutElements corresponding to dbPhi=1,2 respectively ( 2,
      according to the identifier scheme, is at larger global phi than 1)
      for dbr=2 the situation is identical to dbr=1

      and

      BMS6 is done this way:
      at dbr=1, there are only 2 dbZ, the first corrsponding to a single
      RpcReadoutElement, the second one corresponding to two different
      chambers and, therefore, two different RpcReadoutElements;
      in dbr=2, there are 3 doubletZ, the first two are standard (1
      RpcReadoutElemet each), the third one has two chambers -> 2
      RpcReadoutElements

      Notice that 5 and 6 is a subtype naming which does not have any
      correspondence to the offline identifier scheme: so you have to know where
      BMS5 and BMS6 are located in order to treat them in a special way: so ...

      BMS 5 are at StEta = +/- 2 and StPhi = 1,2,3,4,5,8
      BMS 6 are at StEta = +/- 4 and StPhi = 1,2,3,4,5,8
  */
  int dbz_index    = dZ;

  if ( name == "BMS" ) { 
    if (abs(eta) == 2 && dZ ==3 )
      {
        if (dP == 2) dbz_index++;
      }
    else if (abs(eta) == 4 && dR==2 && dZ==3)
      {
        if (dP == 2) dbz_index++;
      }
    else if (abs(eta) == 4 && dR==1 && dZ==2) 
      {
        if (dP == 2) dbz_index++;
      }
  }      
  return dbz_index;
}

#endif // MUONIDHELPERS_RPCIDHELPER_H

