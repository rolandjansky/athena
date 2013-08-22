/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_HWMappingSvc_H
#define ITRT_HWMappingSvc_H

/*----------------------------------------------------------
 *  @file ITRT_HWMappingSvc.h
 *  @Service Interface for TRT_HWMappingSvc
 *  @author Denver Whittington <Denver.Whittington@cern.ch>
 *///-------------------------------------------------------

// Header Includes
#include "GaudiKernel/IService.h"

class Identifier;

// Class definition for this Service
class ITRT_HWMappingSvc : virtual public IService {

 public:

  /// Returns the HV pad for a barrel identifier
  virtual int get_HV_BarrelPadNum( const Identifier ) = 0;

  /// Returns the HV cell for an endcap identifier
  virtual int get_HV_EndcapCellNum( const Identifier ) = 0;

  /// Returns the 4-plane wheel number for an endcap identifier
  virtual int get_HV_Endcap4PlaneNum( const Identifier ) = 0;

  /// Returns the fuse number (0-3) for an endcap identifier
  virtual int get_HV_EndcapFuseNum( const Identifier ) = 0;

  /// Returns the HV line logical name in COOL channel format (":","/" -> "_")
  virtual std::string get_HV_CoolChanName( const Identifier ) = 0;

  /// Returns the COOL channel number for an identifier
  virtual int get_HV_CoolChanNum( const Identifier ) = 0;

  /// Hashes a Barrel HV pad by sector/module/pad#
  virtual int hashThisBarrelPad( int, int, int ) = 0;

  /// Hashes an Endcap HV cell by sector/wheel/layer/cell#
  virtual int hashThisEndcapCell( int, int, int, int ) = 0;

  /// Builds the HV-line/pad map for barrel
  virtual StatusCode build_BarrelHVLinePadMap() = 0;

  /// Builds the HV-line/pad maps for endcaps
  virtual StatusCode build_EndcapHVLinePadMaps() = 0;

  /// Dump HV-line/pad maps
  virtual void DumpMaps() = 0;

  /// Interface ID declaration
  static const InterfaceID& interfaceID();

};

/// Interface ID for this AlgTool
inline const InterfaceID& ITRT_HWMappingSvc::interfaceID() {
  static const InterfaceID IID("ITRT_HWMappingSvc",1,0);
  return IID;
}

#endif // ITRT_HWMappingSvc_H
