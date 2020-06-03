/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Muon Calibration Utilities
 * -----------------------------------------
 *
 * Authors       : Martin Woudstra, Zdenko van Kesteren, Peter Kluit
 * Creation Date: 21 March 2005
 * Last Update  : 18 April 2005
 ***************************************************************************/


#ifndef SRC_MUONFIXEDID_H
#define SRC_MUONFIXEDID_H
// std
#include <iostream>
#include <iomanip>
#include <string>

// other packages

// this package
//#include "MuonCalibIdentifier/MuonStationName.h"


/** @namespace MuonCalib
* namespace to identify classes related only to muon calibration 
*/

namespace MuonCalib {

/** @class MuonFixedId
 Implements fixed identifiers not dependent upon Athena Identifier for internal
 use in the calibration framework.
 * Fixed identifiers are encoded words which contain some muon generic fields
 (for the technology, the station, eta and phi) and some technology specific
 fields for Mdt, Csc, Rpc and Tgc. To each field XXX correspond 
 * an offset (a private static constant XXXShift) 
 * a minimum and a maximum storable value (derived from private static constantsXXXMin and XXXMask) 
 * Encoding or decoding of a value num in a given field proceeds via encoding
 * or decoding and index, which is defined as num-XXXMin. 
 */

class MuonFixedId {
public:
   /** default constructor */
   MuonFixedId(); 
   /** explicit constructor from an integer which corresponds to a MuonFixedId*/
   explicit MuonFixedId( unsigned int id );

   /** @return the fixed identifier in integer form */
   unsigned int getIdInt() const;

   void setMuonIdentifier(int id);

   /** clear the identifier */
   void clear();
   /** check validity of the identifier. @return false if invalid */
   bool isValid();

   /** @return true if the identifier corresponds to MDT technology */
   bool is_mdt() const;
   /** @return true if the identifier corresponds to TGC technology */
   bool is_tgc() const;
   /** @return true if the identifier corresponds to CSC technology */
   bool is_csc() const;
   /** @return true if the identifier corresponds to RPC technology */
   bool is_rpc() const;

   /** comparison operator */
   bool operator==( const MuonFixedId& rhs ) const;
   /** comparison operator */
   bool operator!=( const MuonFixedId& rhs ) const;
   /** comparison operator */
   bool operator<( const MuonFixedId& rhs ) const;
   
   /** dump all the information corresponding to the identifier */
   std::ostream& dump(std::ostream& os) const;

//   friend std::istream& operator>>( std::istream& is, MuonCalib::MuonFixedId& id );

// Muon generic code

   /** set technology index in the fixed identifier 
    * @return false if out of range */
   bool setTechnologyIndex( unsigned int idx );
   /** set station name index in the fixed identifier 
    * @return false if out of range */
   bool setStationNameIndex( unsigned int idx );
   /** set station eta index in the fixed identifier 
    * @return false if out of range */
   bool setStationEtaIndex( unsigned int idx );
   /** set station phi index in the fixed identifier 
    * @return false if out of range */
   bool setStationPhiIndex( unsigned int idx );

   /** set technology to num in the fixed identifier 
    * @return false if out of range */
   bool setTechnology( int num );
   /** set station name  to num in the fixed identifier
    * @return false if out of range */
   bool setStationName( int num );
   /** set station eta  to num in the fixed identifier
    * @return false if out of range */
   bool setStationEta( int num );
   /** set station phi  to num in the fixed identifier
    * @return false if out of range */
   bool setStationPhi( int num );

   /** @return technology index */
   int technologyIndex() const;
   /** @return station name index */
   int stationNameIndex() const;
   /** @return station eta index */
   int etaIndex() const;
   /** @return station phi index */
   int phiIndex() const;

   /** @return technology */
   int technology() const;
   /** @return station name */
   int stationName() const;
   /** @return station eta */
   int eta() const;
   /** @return station phi */
   int phi() const;
   
   /** @return the minimum index used to store technology */
   static int technologyMin();
   /** @return the minimum index used to store station name */
   static int stationNameMin();
   /** @return the minimum index used to store station eta */
   static int etaMin();
   /** @return the minimum index used to store station phi */
   static int phiMin();

   /** @return the maximum index used to store technology */
   static int technologyMax();
   /** @return the maximum index used to store station name */
   static int stationNameMax();
   /** @return the maximum index used to store station eta */
   static int etaMax();
   /** @return the maximum index used to store station phi */
   static int phiMax();

   /** @return the fixed station number for a station name 
     * @param[in] station the station name string
     */ 
   int stationStringToFixedStationNumber(const std::string station) const;
   /** @return the station name string given the fixed station number 
     * @param[in] station the station integer number 
     */ 
   std::string stationNumberToFixedStationString(const int station) const;


//  Mdt specific code

   /** Mdt specific: set index idx for tube 
    * @return false if out of range */
   bool setMdtTubeIndex( unsigned int idx ) ;
   /** Mdt specific: set index idx for layer 
    * @return false if out of range */
   bool setMdtTubeLayerIndex( unsigned int idx ) ;
   /** Mdt specific: set index idx for multilayer 
    * @return false if out of range */
   bool setMdtMultilayerIndex( unsigned int idx ) ;

   /** Mdt specific: set tube to num 
    * @return false if out of range */
   bool setMdtTube( int num );
   /** Mdt specific: set layer to num 
    * @return false if out of range */
   bool setMdtTubeLayer( int num );
   /** Mdt specific: set multilayer to num 
    * @return false if out of range */
   bool setMdtMultilayer( int num );

   /** Mdt specific: @return tube index */
   int mdtTubeIndex() const;
   /** Mdt specific: @return layer index */
   int mdtTubeLayerIndex() const;
   /** Mdt specific: @return multilayer index */
   int mdtMultilayerIndex() const;

   /** Mdt specific: @return tube number*/
   int mdtTube() const;
   /** Mdt specific: @return layer number*/
   int mdtTubeLayer() const;
   /** Mdt specific: @return multilayer number*/
   int mdtMultilayer() const;
   /** Mdt specific: compute the mezzanine number 
     * @return mezzanine number */
   int mdtMezzanine() const;

   /** Mdt specific: @return the minimum index used to store the tube number */ 
   static int mdtTubeMin();
   /** Mdt specific: @return the minimum index used to store the layer number */
   static int mdtTubeLayerMin();
   /** Mdt specific: @return the minimum index used to store the multilayer number */ 
   static int mdtMultilayerMin();

   /** Mdt specific: @return the maximum index used to store the tube number */ 
   static int mdtTubeMax();
   /** Mdt specific: @return the maximum index used to store the layer number */ 
   static int mdtTubeLayerMax();
   /** Mdt specific: @return the maximum index used to store the multilayer number */ 
   static int mdtMultilayerMax();
   
//  Csc specific code

   /** Csc specific: set chamber layer index to idx
    * @return false if out of range */
   bool setCscChamberLayerIndex( unsigned int idx );
   /** Csc specific: set wire layer index to idx 
    * @return false if out of range */
   bool setCscWireLayerIndex( unsigned int idx );
   /** Csc specific: set measures_phi index to idx
    * @return false if out of range */
   bool setCscMeasuresPhiIndex( unsigned int idx );
   /** Csc specific: set strip index to idx 
    * @return false if out of range */
   bool setCscStripIndex( unsigned int idx );

   /** Csc specific: set chamber layer to num
    * @return false if out of range */
   bool setCscChamberLayer( int num );
   /** Csc specific: set wire layer to num 
    * @return false if out of range */
   bool setCscWireLayer( int num );
   /** Csc specific: set measures_phi to num 
    * @return false if out of range */
   bool setCscMeasuresPhi( int num );
   /** Csc specific: set strip to num 
    * @return false if out of range */
   bool setCscStrip( int num );  

   /** Csc specific: @return chamber layer index */ 
   int cscChamberLayerIndex() const;
   /** Csc specific: @return wire layer index */ 
   int cscWireLayerIndex() const;
   /** Csc specific: @return measures_phi index */ 
   int cscMeasuresPhiIndex() const;
   /** Csc specific: @return strip index */ 
   int cscStripIndex() const;

   /** Csc specific: @return chamber layer*/
   int cscChamberLayer() const;
   /** Csc specific: @return wire layer*/
   int cscWireLayer() const;
   /** Csc specific: @return measures_phi */
   int cscMeasuresPhi() const;
   /** Csc specific: @return strip */
   int cscStrip() const;

   /** Csc specific: @return the minimum index used to store the chamber layer*/
   static int cscChamberLayerMin(); 
   /** Csc specific: @return the minimum index used to store the wire layer*/
   static int cscWireLayerMin(); 
   /** Csc specific: @return the minimum index used to store measures_phi */
   static int cscMeasuresPhiMin(); 
   /** Csc specific: @return the minimum index used to store the strip number */
   static int cscStripMin();
   
   /** Csc specific: @return the maximum index used to store the chamber layer*/
   static int cscChamberLayerMax(); 
   /** Csc specific: @return the maximum index used to store the wire layer*/
   static int cscWireLayerMax(); 
   /** Csc specific: @return the maximum index used to store measures_phi */
   static int cscMeasuresPhiMax(); 
   /** Csc specific: @return the maximum index used to store the strip number */
   static int cscStripMax();

//  Rpc specific code 

   /** Rpc specific: set index for doublet_R to idx
    * @return false if out of range */
   bool setRpcDoubletRIndex( unsigned int idx );
   /** Rpc specific: set index for doublet_Z to idx
    * @return false if out of range */
   bool setRpcDoubletZIndex( unsigned int idx );
   /** Rpc specific: set index for doublet_Phi to idx
    * @return false if out of range */
   bool setRpcDoubletPhiIndex( unsigned int idx );
   /** Rpc specific: set index for gas_gap to idx
    * @return false if out of range */
   bool setRpcGasGapIndex( unsigned int idx );
   /** Rpc specific: set index for measures_phi to idx
    * @return false if out of range */
   bool setRpcMeasuresPhiIndex( unsigned int idx );
   /** Rpc specific: set index for strip to idx
    * @return false if out of range */
   bool setRpcStripIndex( unsigned int idx );

   /** Rpc specific: set doublet_R to num 
    * @return false if out of range */
   bool setRpcDoubletR( int num );
   /** Rpc specific: set doublet_Z to num 
    * @return false if out of range */
   bool setRpcDoubletZ( int num );
   /** Rpc specific: set doublet_Phi to num 
    * @return false if out of range */
   bool setRpcDoubletPhi( int num );
   /** Rpc specific: set gas_gap to num 
    * @return false if out of range */
   bool setRpcGasGap( int num );
   /** Rpc specific: set measures_phi to num 
    * @return false if out of range */
   bool setRpcMeasuresPhi( int num );
   /** Rpc specific: set strip to num 
    * @return false if out of range */
   bool setRpcStrip( int num );
   
   /** Rpc specific:
    * @return doublet_R index*/
   int rpcDoubletRIndex() const;
   /** Rpc specific:
    * @return doublet_Z index*/
   int rpcDoubletZIndex() const;
   /** Rpc specific:
    * @return doublet_Phi index*/
   int rpcDoubletPhiIndex() const;
   /** Rpc specific:
    * @return gas_gap index*/
   int rpcGasGapIndex() const;
   /** Rpc specific:
    * @return measures_phi index*/
   int rpcMeasuresPhiIndex() const;
   /** Rpc specific:
    * @return strip index*/
   int rpcStripIndex() const;
   
   /** Rpc specific:
    * @return doublet_R */
   int rpcDoubletR() const;
   /** Rpc specific:
    * @return doublet_Z */
   int rpcDoubletZ() const;
   /** Rpc specific:
    * @return doublet_Phi */
   int rpcDoubletPhi() const;
   /** Rpc specific:
    * @return gas_gap */
   int rpcGasGap() const;
   /** Rpc specific:
    * @return measures_phi */
   int rpcMeasuresPhi() const;
   /** Rpc specific:
    * @return strip */
   int rpcStrip() const;

//  Tgc specific code

   /** Tgc specific: set index for gas_gap to idx
    * @return false if out of range */
   bool setTgcGasGapIndex( unsigned int idx );  
   /** Tgc specific: set index for is_strip to idx
    * @return false if out of range */
   bool setTgcIsStripIndex( unsigned int idx );
   /** Tgc specific: set index for channel to idx
    * @return false if out of range */
   bool setTgcChannelIndex( unsigned int idx );  

   /** Tgc specific: set gas_gap to num 
    * @return false if out of range */
   bool setTgcGasGap( int num );
   /** Tgc specific: set is_strip to num 
    * @return false if out of range */
   bool setTgcIsStrip( int num );
   /** Tgc specific: set channel to num 
    * @return false if out of range */
   bool setTgcChannel( int num );

   /** Tgc specific:
    * @return gas_gap index */
   int tgcGasGapIndex() const;   
   /** Tgc specific:
    * @return is_strip index */
   int tgcIsStripIndex() const;   
   /** Tgc specific:
    * @return channel index */
   int tgcChannelIndex() const;   

   /** Tgc specific:
    * @return gas_gap */
   int tgcGasGap() const;
   /** Tgc specific:
    * @return is_strip */
   int tgcIsStrip() const;
   /** Tgc specific:
    * @return channel */
   int tgcChannel() const;

private:

//   Muon generic

   static const unsigned int kTechnologyMask   = 3;
   static const unsigned int kTechnologyShift  = 30;
   static const unsigned int kTechnologyMin    = 0;

   static const unsigned int kStationNameMask      = 63;
   static const unsigned int kStationNameShift     = 24;
   static const unsigned int kStationNameMin       = 1;

   static const unsigned int kStationEtaMask          = 31;
   static const unsigned int kStationEtaShift         = 19;
   static const int kStationEtaMin                    = -8;

   static const unsigned int kStationPhiMask          = 63;
   static const unsigned int kStationPhiShift         = 13;
   static const unsigned int kStationPhiMin           = 1;

   unsigned int m_data;

//   Mdt specific code 

   static const unsigned int kMdtMultilayerMask    = 1;
   static const unsigned int kMdtMultilayerShift   = 9;
   static const unsigned int kMdtMultilayerMin     = 1;

   static const unsigned int kMdtTubeLayerMask     = 3;
   static const unsigned int kMdtTubeLayerShift    = 7;
   static const unsigned int kMdtTubeLayerMin      = 1;

   static const unsigned int kMdtTubeMask          = 127;
   static const unsigned int kMdtTubeShift         = 0;
   static const unsigned int kMdtTubeMin           = 1;

//  Csc specific code

   static const unsigned int kCscChamberLayerMask  = 1;
   static const unsigned int kCscChamberLayerShift = 11;
   static const unsigned int kCscChamberLayerMin   = 1;

   static const unsigned int kCscWireLayerMask     = 3;
   static const unsigned int kCscWireLayerShift    = 9;
   static const unsigned int kCscWireLayerMin      = 1;

   static const unsigned int kCscMeasuresPhiMask   = 1;
   static const unsigned int kCscMeasuresPhiShift  = 8;
   static const unsigned int kCscMeasuresPhiMin    = 0;

   static const unsigned int kCscStripMask         = 255;
   static const unsigned int kCscStripShift        = 0;
   static const unsigned int kCscStripMin          = 1;

//  Rpc specific code

   static const unsigned int kRpcDoubletRMask      = 1;
   static const unsigned int kRpcDoubletRShift     = 12; 
   static const unsigned int kRpcDoubletRMin       = 1;

   static const unsigned int kRpcDoubletZMask      = 3;
   static const unsigned int kRpcDoubletZShift     = 10; 
   static const unsigned int kRpcDoubletZMin       = 1;

   static const unsigned int kRpcDoubletPhiMask    = 1;
   static const unsigned int kRpcDoubletPhiShift   = 9; 
   static const unsigned int kRpcDoubletPhiMin     = 1;

   static const unsigned int kRpcGasGapMask        = 1;
   static const unsigned int kRpcGasGapShift       = 8;  
   static const unsigned int kRpcGasGapMin         = 1;

   static const unsigned int kRpcMeasuresPhiMask   = 1;
   static const unsigned int kRpcMeasuresPhiShift  = 7;  
   static const unsigned int kRpcMeasuresPhiMin    = 0;

   static const unsigned int kRpcStripMask         = 127;
   static const unsigned int kRpcStripShift        = 0; 
   static const unsigned int kRpcStripMin          = 1;

// Tgc specific code 
   static const unsigned int kTgcGasGapMask        = 3;
   static const unsigned int kTgcGasGapShift       = 9;
   static const unsigned int kTgcGasGapMin         = 1;

   static const unsigned int kTgcIsStripMask       = 1;
   static const unsigned int kTgcIsStripShift      = 8;
   static const unsigned int kTgcIsStripMin        = 0;

   static const unsigned int kTgcChannelMask       = 255;
   static const unsigned int kTgcChannelShift      = 0;
   static const unsigned int kTgcChannelMin        = 1;
   
};
 
// Muon generic methods
 
inline MuonFixedId::MuonFixedId()
     : m_data(0xFFFFFFFF){
}
 
inline MuonFixedId::MuonFixedId( unsigned int id ) : m_data(id){
}
 
inline unsigned int MuonFixedId::getIdInt() const{
     return m_data;
}
 
inline void MuonFixedId::setMuonIdentifier(int id) { 
     if( id>-1 || id<(int)0xFFFFFFFF ) m_data = id; 
     else  m_data = 0xFFFFFFFF; 
 } 
 
inline void MuonFixedId::clear() {
     m_data = 0xFFFFFFFF;
}

inline bool MuonFixedId::isValid(){
  return m_data != 0xFFFFFFFF;
}

inline bool MuonFixedId::is_mdt() const{
  if(m_data == 0xFFFFFFFF){
    return false;
  } else { 
    if( technology() == 0 ){
      return true;
    } else return false;
  }
}

inline bool MuonFixedId::is_tgc() const{
  if(m_data == 0xFFFFFFFF){
    return false;
  } else { 
    if( technology() == 2 ){
      return true;
    } else return false;
  }
}

inline bool MuonFixedId::is_csc() const{
  if(m_data == 0xFFFFFFFF){
    return false;
  } else { 
    if( technology() == 1 ){
      return true;
    } else return false;
  }
}

inline bool MuonFixedId::is_rpc() const{
  if(m_data == 0xFFFFFFFF){
    return false;
  } else { 
    if( technology() == 3 ){
      return true;
    } else return false;
  }
}

inline bool MuonFixedId::operator==( const MuonFixedId& rhs ) const {
   return m_data == rhs.m_data;
}

inline bool MuonFixedId::operator!=( const MuonFixedId& rhs ) const {
   return m_data != rhs.m_data;
}

inline bool MuonFixedId::operator<( const MuonFixedId& rhs ) const {
   return m_data < rhs.m_data;
}

inline bool MuonFixedId::setTechnologyIndex( unsigned int idx ) { 
  if ( idx > kTechnologyMask ) {
      clear();
      return false;
   }
   m_data &= ~(kTechnologyMask << kTechnologyShift);
   m_data |= (idx & kTechnologyMask) << kTechnologyShift;
   return true;
}

inline bool MuonFixedId::setTechnology( int num ) { 
   return setTechnologyIndex( num - kTechnologyMin );
}

inline int MuonFixedId::technologyIndex() const { 
   return (m_data >> kTechnologyShift) & kTechnologyMask;
}

inline int MuonFixedId::technology() const {
   return technologyIndex() + kTechnologyMin;
}


inline bool MuonFixedId::setStationNameIndex( unsigned int idx ) {
  if ( idx > kStationNameMask ) {
      clear();
      return false;
   }
   m_data &= ~(kStationNameMask << kStationNameShift);
   m_data |= (idx & kStationNameMask) << kStationNameShift;
   return true;
}

inline bool MuonFixedId::setStationName( int num ) {
   return setStationNameIndex( num - kStationNameMin );
}

inline int MuonFixedId::stationNameIndex() const {
   return (m_data >> kStationNameShift) & kStationNameMask;
}

inline int MuonFixedId::stationName() const {
   return stationNameIndex() + kStationNameMin;
}


inline bool MuonFixedId::setStationEtaIndex( unsigned int idx ) {
  if ( idx > kStationEtaMask ) {
      clear();
      return false;
   }
   m_data &= ~(kStationEtaMask << kStationEtaShift);
   m_data |= (idx & kStationEtaMask) << kStationEtaShift;
   return true;
}

inline bool MuonFixedId::setStationEta( int num ) {
   return setStationEtaIndex( num - kStationEtaMin );
}

inline int MuonFixedId::etaIndex() const {
   return (m_data >> kStationEtaShift) & kStationEtaMask;
}

inline int MuonFixedId::eta() const {
   return etaIndex() + kStationEtaMin;
}


inline bool MuonFixedId::setStationPhiIndex( unsigned int idx ) {
  if ( idx > kStationPhiMask ) {
      clear();
      return false;
   }
   m_data &= ~(kStationPhiMask << kStationPhiShift);
   m_data |= (idx & kStationPhiMask) << kStationPhiShift;
   return true;
}

inline bool MuonFixedId::setStationPhi( int num ) {
   return setStationPhiIndex( num - kStationPhiMin );
}

inline int MuonFixedId::phiIndex() const {
   return (m_data >> kStationPhiShift) & kStationPhiMask;
}

inline int MuonFixedId::phi() const {
   return phiIndex() + kStationPhiMin;
}
  
inline int MuonFixedId::technologyMin(){
  return kTechnologyMin;
}

inline int MuonFixedId::stationNameMin(){
  return kStationNameMin;
}

inline int MuonFixedId::etaMin(){ 
  return kStationEtaMin;
}

inline int MuonFixedId::phiMin(){
  return kStationPhiMin;
}

inline int MuonFixedId::technologyMax(){
  return kTechnologyMin + kTechnologyMask;
}

inline int MuonFixedId::stationNameMax(){ 
  return kStationNameMin + kStationNameMask;
}

inline int MuonFixedId::etaMax(){ 
  return kStationEtaMin + kStationEtaMask;
}

inline int MuonFixedId::phiMax(){
   return kStationPhiMin + kStationPhiMask; 
}

inline int MuonFixedId::stationStringToFixedStationNumber(const std::string station ) const
{
  if(station == "BIL" ) return 1;
  if(station == "BIS" ) return 2;
  if(station == "BML" ) return 3;
  if(station == "BMS" ) return 4;
  if(station == "BOL" ) return 5;
  if(station == "BOS" ) return 6;
  if(station == "BEE" ) return 7;
  if(station == "BIR" ) return 8;
  if(station == "BMF" ) return 9;
  if(station == "BOF" ) return 10;
  if(station == "BOG" ) return 11;
  if(station == "BME" ) return 12;
  if(station == "BIM" ) return 13;
  if(station == "EIC" ) return 14;
  if(station == "EIL" ) return 15;
  if(station == "EEL" ) return 16;
  if(station == "EES" ) return 17;
  if(station == "EMC" ) return 18;
  if(station == "EML" ) return 19;
  if(station == "EMS" ) return 20;
  if(station == "EOC" ) return 21;
  if(station == "EOL" ) return 22;
  if(station == "EOS" ) return 23;
  if(station == "EIS" ) return 24;
  if(station == "T1F" ) return 25;
  if(station == "T1E" ) return 26;
  if(station == "T2F" ) return 27;
  if(station == "T2E" ) return 28;
  if(station == "T3F" ) return 29;
  if(station == "T3E" ) return 30;
  if(station == "T4F" ) return 31;
  if(station == "T4E" ) return 32;
  if(station == "CSS" ) return 33;
  if(station == "CSL" ) return 34;
  if(station == "BMG" ) return 35;
  if(station == "XXX" ) return -1;
  return -1;
}

inline std::string MuonFixedId::stationNumberToFixedStationString(const int station) const
{
  if(station ==  1 ) return "BIL" ;
  if(station ==  2 ) return "BIS" ;
  if(station ==  3 ) return "BML" ;
  if(station ==  4 ) return "BMS" ;
  if(station ==  5 ) return "BOL" ;
  if(station ==  6 ) return "BOS" ;
  if(station ==  7 ) return "BEE" ;
  if(station ==  8 ) return "BIR" ;
  if(station ==  9 ) return "BMF" ;
  if(station == 10 ) return "BOF" ;
  if(station == 11 ) return "BOG" ;
  if(station == 12 ) return "BME" ;
  if(station == 13 ) return "BIM" ;
  if(station == 14 ) return "EIC" ;
  if(station == 15 ) return "EIL" ;
  if(station == 16 ) return "EEL" ;
  if(station == 17 ) return "EES" ;
  if(station == 18 ) return "EMC" ;
  if(station == 19 ) return "EML" ;
  if(station == 20 ) return "EMS" ;
  if(station == 21 ) return "EOC" ;
  if(station == 22 ) return "EOL" ;
  if(station == 23 ) return "EOS" ;
  if(station == 24 ) return "EIS" ;
  if(station == 25 ) return "T1F" ;
  if(station == 26 ) return "T1E" ;
  if(station == 27 ) return "T2F" ;
  if(station == 28 ) return "T2E" ;
  if(station == 29 ) return "T3F" ;
  if(station == 30 ) return "T3E" ;
  if(station == 31 ) return "T4F" ;
  if(station == 32 ) return "T4E" ;
  if(station == 33 ) return "CSS" ;
  if(station == 34 ) return "CSL" ;
  if(station == 35 ) return "BMG" ;
  else return "XXX" ;
}

// Mdt specific methods

inline bool MuonFixedId::setMdtTubeIndex( unsigned int idx ) {
  if ( idx > kMdtTubeMask ) {
      clear();
      return false;
   }
   m_data &= ~(kMdtTubeMask << kMdtTubeShift);
   m_data |= (idx & kMdtTubeMask) << kMdtTubeShift;
   return true;
}

inline bool MuonFixedId::setMdtTube( int num ) {
   return setMdtTubeIndex( num - kMdtTubeMin );
}

inline int MuonFixedId::mdtTubeIndex() const {
   return (m_data >> kMdtTubeShift) & kMdtTubeMask;
}

inline int MuonFixedId::mdtTube() const {
   return mdtTubeIndex() + kMdtTubeMin;
}

inline int MuonFixedId::mdtMezzanine() const {
   int Ichamber =  stationName() ;
   int Ieta = eta() ;
   int Iphi = phi() ;
   int Iml  = mdtMultilayer() ;
   int Itube = mdtTube() ;
   if (Ieta < 0 ) Ieta = -Ieta*10 ;
   int ImezzTubes = 8 ;
   if (Ichamber==1 || Ichamber==2 || Ichamber==8 || Ichamber==13) ImezzTubes=6 ;
   int Imezz = ((Itube-1)/ImezzTubes)*2+(Iml-1) ;
   Imezz = 9*100000000 + Ichamber*1000000 + Ieta*10000 + Iphi*100 + Imezz ;

   // std::cout << "Imezz Ichamber Ieta Iphi Iml Itube "
   // << Imezz <<" " << Ichamber  <<" "<< Ieta <<" " << Iphi
   // <<" " << Iml <<" " << Itube << std::endl ;
   return Imezz ;
}



inline bool MuonFixedId::setMdtTubeLayerIndex( unsigned int idx ) {
  if ( idx > kMdtTubeLayerMask ) {
      clear();
      return false;
   }
   m_data &= ~(kMdtTubeLayerMask << kMdtTubeLayerShift);
   m_data |= (idx & kMdtTubeLayerMask) << kMdtTubeLayerShift;
   return true;
}

inline bool MuonFixedId::setMdtTubeLayer( int num ) {
   return setMdtTubeLayerIndex( num - kMdtTubeLayerMin );
}

inline int MuonFixedId::mdtTubeLayerIndex() const {
   return (m_data >> kMdtTubeLayerShift) & kMdtTubeLayerMask;
}

inline int MuonFixedId::mdtTubeLayer() const {
   return mdtTubeLayerIndex() + kMdtTubeLayerMin;
}


inline bool MuonFixedId::setMdtMultilayerIndex( unsigned int idx ) {
  if ( idx > kMdtMultilayerMask ) {
      clear();
      return false;
   }
   m_data &= ~(kMdtMultilayerMask << kMdtMultilayerShift);
   m_data |= (idx & kMdtMultilayerMask) << kMdtMultilayerShift;
   return true;
}

inline bool MuonFixedId::setMdtMultilayer( int num ) {
   return setMdtMultilayerIndex( num - kMdtMultilayerMin );
}

inline int MuonFixedId::mdtMultilayerIndex() const {
   return (m_data >> kMdtMultilayerShift) & kMdtMultilayerMask;
}

inline int MuonFixedId::mdtMultilayer() const {
   return mdtMultilayerIndex() + kMdtMultilayerMin;
}

inline int MuonFixedId::mdtTubeMin(){
  return kMdtTubeMin;
}

inline int MuonFixedId::mdtTubeMax(){
  return kMdtTubeMin + kMdtTubeMask;
}

inline int MuonFixedId::mdtTubeLayerMin(){
  return kMdtTubeLayerMin;
}

inline int MuonFixedId::mdtTubeLayerMax(){
  return kMdtTubeLayerMin + kMdtTubeLayerMask;
}

inline int MuonFixedId::mdtMultilayerMin(){
  return kMdtMultilayerMin;
}

inline int MuonFixedId::mdtMultilayerMax(){
  return kMdtMultilayerMin + kMdtMultilayerMask;
}

// Csc specific methods

inline bool MuonFixedId::setCscChamberLayerIndex( unsigned int idx ) {
  if ( idx > kCscChamberLayerMask ) {
      clear();
      return false;
   }
   m_data &= ~( kCscChamberLayerMask <<  kCscChamberLayerShift );
   m_data |= (idx &  kCscChamberLayerMask) << kCscChamberLayerShift;
   return true;
}

inline bool MuonFixedId::setCscChamberLayer( int num ) {
   return setCscChamberLayerIndex( num -  kCscChamberLayerMin );
}

inline int MuonFixedId::cscChamberLayerIndex() const {
   return (m_data >> kCscChamberLayerShift) & kCscChamberLayerMask;
}

inline int MuonFixedId::cscChamberLayer() const {
   return cscChamberLayerIndex() + kCscChamberLayerMin;
}


inline bool MuonFixedId::setCscWireLayerIndex( unsigned int idx ) {
  if ( idx > kCscWireLayerMask ) {
      clear();
      return false;
   }
   m_data &= ~( kCscWireLayerMask <<  kCscWireLayerShift );
   m_data |= (idx &  kCscWireLayerMask) << kCscWireLayerShift;
   return true;
}

inline bool MuonFixedId::setCscWireLayer( int num ) {
  return setCscWireLayerIndex( num -  kCscWireLayerMin );
}

inline int MuonFixedId::cscWireLayerIndex() const {
   return (m_data >> kCscWireLayerShift) & kCscWireLayerMask;
}

inline int MuonFixedId::cscWireLayer() const {
   return cscWireLayerIndex() + kCscWireLayerMin;
}


inline bool MuonFixedId::setCscMeasuresPhiIndex( unsigned int idx ) {
  if ( idx > kCscMeasuresPhiMask ) {
      clear();
      return false;
   }
   m_data &= ~( kCscMeasuresPhiMask <<  kCscMeasuresPhiShift );
   m_data |= (idx &  kCscMeasuresPhiMask) << kCscMeasuresPhiShift;
   return true;
}

inline bool MuonFixedId::setCscMeasuresPhi( int num ) {
  return setCscMeasuresPhiIndex( num -  kCscMeasuresPhiMin );
}

inline int MuonFixedId::cscMeasuresPhiIndex() const {
   return (m_data >> kCscMeasuresPhiShift) & kCscMeasuresPhiMask;
}

inline int MuonFixedId::cscMeasuresPhi() const {
   return cscMeasuresPhiIndex() + kCscMeasuresPhiMin;
}


inline bool MuonFixedId::setCscStripIndex( unsigned int idx ) {
  if ( idx > kCscStripMask ) {
      clear();
      return false;
   }
   m_data &= ~( kCscStripMask <<  kCscStripShift );
   m_data |= (idx &  kCscStripMask) << kCscStripShift;
   return true;
}

inline bool MuonFixedId::setCscStrip( int num ) {
  return setCscStripIndex( num -  kCscStripMin );
}

inline int MuonFixedId::cscStripIndex() const {
   return (m_data >> kCscStripShift) & kCscStripMask;
}

inline int MuonFixedId::cscStrip() const {
   return cscStripIndex() + kCscStripMin;
}

inline int MuonFixedId::cscChamberLayerMin(){
  return kCscChamberLayerMin;
}

inline int MuonFixedId::cscChamberLayerMax(){
  return kCscChamberLayerMin + kCscChamberLayerMask;
}

inline int MuonFixedId::cscWireLayerMin(){
  return kCscWireLayerMin;
}

inline int MuonFixedId::cscWireLayerMax(){
  return kCscWireLayerMin + kCscWireLayerMask;
}

inline int MuonFixedId::cscMeasuresPhiMin(){
  return kCscMeasuresPhiMin;
}

inline int MuonFixedId::cscMeasuresPhiMax(){
  return kCscMeasuresPhiMin + kCscMeasuresPhiMask;
}

inline int MuonFixedId::cscStripMin(){
  return kCscStripMin;
}

inline int MuonFixedId::cscStripMax(){
  return kCscStripMin + kCscStripMask;
}
// Rpc specific methods

inline bool MuonFixedId::setRpcDoubletRIndex( unsigned int idx ) {
  if ( idx > kRpcDoubletRMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcDoubletRMask <<  kRpcDoubletRShift );
   m_data |= (idx &  kRpcDoubletRMask) << kRpcDoubletRShift;
   return true;
}

inline bool MuonFixedId::setRpcDoubletR( int num ) {
  return setRpcDoubletRIndex( num -  kRpcDoubletRMin );
}

inline int MuonFixedId::rpcDoubletRIndex() const {
   return (m_data >> kRpcDoubletRShift) & kRpcDoubletRMask;
}

inline int MuonFixedId::rpcDoubletR() const {
   return rpcDoubletRIndex() + kRpcDoubletRMin;
}


inline bool MuonFixedId::setRpcDoubletZIndex( unsigned int idx ) {
  if ( idx > kRpcDoubletZMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcDoubletZMask <<  kRpcDoubletZShift );
   m_data |= (idx &  kRpcDoubletZMask) << kRpcDoubletZShift;
   return true;
}

inline bool MuonFixedId::setRpcDoubletZ( int num ) {
  return setRpcDoubletZIndex( num -  kRpcDoubletZMin );
}

inline int MuonFixedId::rpcDoubletZIndex() const {
   return (m_data >> kRpcDoubletZShift) & kRpcDoubletZMask;
}

inline int MuonFixedId::rpcDoubletZ() const {
   return rpcDoubletZIndex() + kRpcDoubletZMin;
}


inline bool MuonFixedId::setRpcDoubletPhiIndex( unsigned int idx ) {
  if ( idx > kRpcDoubletPhiMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcDoubletPhiMask <<  kRpcDoubletPhiShift );
   m_data |= (idx &  kRpcDoubletPhiMask) << kRpcDoubletPhiShift;
   return true;
}

inline bool MuonFixedId::setRpcDoubletPhi( int num ) {
  return setRpcDoubletPhiIndex( num -  kRpcDoubletPhiMin );
}

inline int MuonFixedId::rpcDoubletPhiIndex() const {
   return (m_data >> kRpcDoubletPhiShift) & kRpcDoubletPhiMask;
}

inline int MuonFixedId::rpcDoubletPhi() const {
   return rpcDoubletPhiIndex() + kRpcDoubletPhiMin;
}


inline bool MuonFixedId::setRpcGasGapIndex( unsigned int idx ) {
  if ( idx > kRpcGasGapMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcGasGapMask <<  kRpcGasGapShift );
   m_data |= (idx &  kRpcGasGapMask) << kRpcGasGapShift;
   return true;
}

inline bool MuonFixedId::setRpcGasGap( int num ) {
  return setRpcGasGapIndex( num -  kRpcGasGapMin );
}

inline int MuonFixedId::rpcGasGapIndex() const {
   return (m_data >> kRpcGasGapShift) & kRpcGasGapMask;
}

inline int MuonFixedId::rpcGasGap() const {
   return rpcGasGapIndex() + kRpcGasGapMin;
}


inline bool MuonFixedId::setRpcMeasuresPhiIndex( unsigned int idx ) {
  if ( idx > kRpcMeasuresPhiMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcMeasuresPhiMask <<  kRpcMeasuresPhiShift );
   m_data |= (idx &  kRpcMeasuresPhiMask) << kRpcMeasuresPhiShift;
   return true;
}

inline bool MuonFixedId::setRpcMeasuresPhi( int num ) {
  return setRpcMeasuresPhiIndex( num -  kRpcMeasuresPhiMin );
}

inline int MuonFixedId::rpcMeasuresPhiIndex() const {
   return (m_data >> kRpcMeasuresPhiShift) & kRpcMeasuresPhiMask;
}

inline int MuonFixedId::rpcMeasuresPhi() const {
   return rpcMeasuresPhiIndex() + kRpcMeasuresPhiMin;
}


inline bool MuonFixedId::setRpcStripIndex( unsigned int idx ) {
  if ( idx > kRpcStripMask ) {
      clear();
      return false;
   }
   m_data &= ~( kRpcStripMask <<  kRpcStripShift );
   m_data |= (idx &  kRpcStripMask) << kRpcStripShift;
   return true;
}

inline bool MuonFixedId::setRpcStrip( int num ) {
  return setRpcStripIndex( num -  kRpcStripMin );
}

inline int MuonFixedId::rpcStripIndex() const {
   return (m_data >> kRpcStripShift) & kRpcStripMask;
}

inline int MuonFixedId::rpcStrip() const {
   return rpcStripIndex() + kRpcStripMin;
}


//  Tgc specific methods

inline bool MuonFixedId::setTgcGasGapIndex( unsigned int idx ) {
  if ( idx > kTgcGasGapMask ) {
      clear();
      return false;
   }
   m_data &= ~( kTgcGasGapMask <<  kTgcGasGapShift );
   m_data |= (idx &  kTgcGasGapMask) << kTgcGasGapShift;
   return true;
}

inline bool MuonFixedId::setTgcGasGap( int num ) {
  return setTgcGasGapIndex( num -  kTgcGasGapMin );
}

inline int MuonFixedId::tgcGasGapIndex() const {
   return (m_data >> kTgcGasGapShift) & kTgcGasGapMask;
}

inline int MuonFixedId::tgcGasGap() const {
   return tgcGasGapIndex() + kTgcGasGapMin;
}

inline bool MuonFixedId::setTgcIsStripIndex( unsigned int idx ) {
  if ( idx > kTgcIsStripMask ) {
      clear();
      return false;
   }
   m_data &= ~( kTgcIsStripMask <<  kTgcIsStripShift );
   m_data |= (idx &  kTgcIsStripMask) << kTgcIsStripShift;
   return true;
}

inline bool MuonFixedId::setTgcIsStrip( int num ) {
  return setTgcIsStripIndex( num -  kTgcIsStripMin );
}

inline int MuonFixedId::tgcIsStripIndex() const {
   return (m_data >> kTgcIsStripShift) & kTgcIsStripMask;
}

inline int MuonFixedId::tgcIsStrip() const {
   return tgcIsStripIndex() + kTgcIsStripMin;
}


inline bool MuonFixedId::setTgcChannelIndex( unsigned int idx ) {
  if ( idx > kTgcChannelMask ) {
      clear();
      return false;
   }
   m_data &= ~( kTgcChannelMask <<  kTgcChannelShift );
   m_data |= (idx &  kTgcChannelMask) << kTgcChannelShift;
   return true;
}

inline bool MuonFixedId::setTgcChannel( int num ) {
  return setTgcChannelIndex( num -  kTgcChannelMin );
}

inline int MuonFixedId::tgcChannelIndex() const {
   return (m_data >> kTgcChannelShift) & kTgcChannelMask;
}

inline int MuonFixedId::tgcChannel() const {
   return tgcChannelIndex() + kTgcChannelMin;
}

inline std::ostream& MuonFixedId::dump(std::ostream& os) const
{
  os << technology() << ": stnEta " << eta() << ", stnPhi " << phi()  ; 
  if( is_mdt() ){
     os << "- MDT: " << stationNumberToFixedStationString( stationName() ) << ", mdtMultiLayer  " << mdtMultilayer() << ", mdtTubeLayer "
	<< mdtTubeLayer() << ", mdtTube " << mdtTube() ; 
  } else if( is_rpc() ) { 
    os << " RPC: " << stationNumberToFixedStationString( stationName() ) << ", rpcDoubletR " << rpcDoubletR() << ", rpcDoubletZ "
       << rpcDoubletZ() << ", rpcDoubletPhi " << rpcDoubletPhi() << ", rpcGasGap " << rpcGasGap() << ", rpcMeasuresPhi "
       << rpcMeasuresPhi() << ", rpcStrip " << rpcStrip() ; 
  } else if( is_tgc() ) { 
    os << " TGC: " << stationNumberToFixedStationString( stationName() ) << ", tgcGasGap " << tgcGasGap() << ", tgcIsStrip " 
       <<  tgcIsStrip() << ", tgcChannel " << tgcChannel() ; 
  } else if( is_csc() ) {
    os << " CSC: " << stationNumberToFixedStationString( stationName() ) << ", cscChamberLayer " << cscChamberLayer() << ", cscWireLayer "
       << cscWireLayer()  << ", cscMeasuresPhi " << cscMeasuresPhi() << ", cscStrip " << cscStrip() ; 
  } else
    os << "NO MuonFixedId" ;
  
  return os;
}

} //MuonCalib namespace 

std::ostream& operator<<( std::ostream& os, const MuonCalib::MuonFixedId& id );

/*
***** must solve conflicts between MuonCalib namespace and friendship
inline std::istream& operator>>( std::istream& is, MuonCalib::MuonFixedId& id){
  is >> id.m_data ;
  return is;
}
*/
#endif // SRC_MUONFIXEDID_H


