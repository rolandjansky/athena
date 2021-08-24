/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_DCS_CONDITIONSSVC_H
#define ITRT_DCS_CONDITIONSSVC_H

/** @file ITRT_DCS_ConditionsSvc.h
 *  Interface to read DCS conditions data from COOL
 * author Denver Whittington <Denver.Whittington@cern.ch>
 **/

#include "GaudiKernel/IService.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
class Identifier;

#ifndef TRT_DCS_FLAGS
#define TRT_DCS_FLAGS
namespace InDet {
  /** status flags derived from DCS conditions data */
  enum TRT_DCS_StatusFlag {
    TRT_DCS_NOINFO,
    TRT_DCS_GREEN,
    TRT_DCS_YELLOW,
    TRT_DCS_RED
  };
  /** DCS data available in COOL offline database */
  enum TRT_DCS_DataType {
    TRT_DCS_HV_VOLTAGE
  };
  /** data type stored in COOL DCS tables (usually float) */
  typedef float TRT_DCS_ValueType;
  /** Detector Components (for mapping db) */
  enum TRT_DCS_DetectorType {
    TRT_DCS_BARREL,
    TRT_DCS_ENDCAPA,
    TRT_DCS_ENDCAPC
  };
}
#endif // TRT_DCS_FLAGS

/// Service Interface for TRT_DCS_ConditionsTool
class ITRT_DCS_ConditionsSvc : virtual public IService {

 public:

  /// Returns the DCS conditions StatusFlag for a given identifier.
  virtual InDet::TRT_DCS_StatusFlag getFlag( const Identifier ) = 0;

  /// Returns the value for a given identifier and data type.
  /** Get the value for a given identifier and DCS conditions data type.
   */
  virtual StatusCode getValue( const Identifier, const InDet::TRT_DCS_DataType,
			       InDet::TRT_DCS_ValueType& ) = 0;

  /// Returns the value for a given folder/channel name.
  /** Get the value for a given folder/channel name.
   */
  virtual StatusCode getValue( const std::string & folderName, const std::string & channelName,
			       InDet::TRT_DCS_ValueType& ) = 0;

  /// Returns the value for a given folder and COOL channel number.
  /** Get the value for a given folder and COOL channel number.
   */
  virtual StatusCode getValue( const std::string & folderName, const int chanNum,
			       InDet::TRT_DCS_ValueType& ) = 0;

  virtual const CondAttrListCollection* getCollection( const std::string & collName) =0;

  virtual void monitorBarrel() = 0;
  virtual void monitorEndcapA() = 0;
  virtual void monitorEndcapC() = 0;

  /// Interface ID
  static const InterfaceID& interfaceID();

};

inline const InterfaceID& ITRT_DCS_ConditionsSvc::interfaceID() {
  static const InterfaceID IID("ITRT_DCS_ConditionsSvc",1,0);
  return IID;
}

#endif // ITRT_DCS_CONDITIONSSVC_H
