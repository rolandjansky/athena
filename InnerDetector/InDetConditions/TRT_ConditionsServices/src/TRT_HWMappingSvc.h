/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HWMappingSvc_H
#define TRT_HWMappingSvc_H

/*----------------------------------------------------------
 *  @file TRT_HWMappingSvc.h
 *  @Service to provide offline -> hardware mapping
 *  @author Denver Whittington <Denver.Whittington@cern.ch>
 * updated March 2019 Peter Hansen <phansen@nbi.dk>
 *///-------------------------------------------------------

// Header Includes
#include "TRT_ConditionsServices/ITRT_HWMappingSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TRT_ConditionsData/HWMap.h"


class ITRT_StrawNeighbourSvc;
class TRT_ID;
class Identifier;

/// AlgTool providing offline -> hardware mapping information
class TRT_HWMappingSvc : public AthService, virtual public ITRT_HWMappingSvc {

 public:

  TRT_HWMappingSvc( const std::string&, ISvcLocator* );
  virtual ~TRT_HWMappingSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID&, void** );

  /// Returns the HV pad for a barrel identifier
  int get_HV_BarrelPadNum( const Identifier );

  /// Returns the HV cell for an endcap identifier
  int get_HV_EndcapCellNum( const Identifier );

  /// Returns the 4-plane wheel number for an endcap identifier
  int get_HV_Endcap4PlaneNum( const Identifier );

  /// Returns the fuse number (0-3) for an endcap identifier
  int get_HV_EndcapFuseNum( const Identifier );

  /// Returns the HV line logical name in COOL channel format (":","/" -> "_")
  std::string get_HV_CoolChanName( const Identifier );

  /// Returns the COOL channel number for an identifier
  int get_HV_CoolChanNum( const Identifier );

  /// Hashes a Barrel HV pad by sector/module/pad#
  int hashThisBarrelPad( int, int, int );

  /// Hashes an Endcap HV cell by sector/wheel/layer/cell#
  int hashThisEndcapCell( int, int, int, int );

  /// Dump HV-line/pad maps
  void DumpMaps();

 private:

  ServiceHandle<StoreGateSvc> m_detStore;

  /// jobOptions properties
  std::string m_Barrel_HV_COOLFolderName;
  std::string m_EndcapA_HV_COOLFolderName;
  std::string m_EndcapC_HV_COOLFolderName;

  /// Straw Helpers
  const TRT_ID* m_TRT_ID_Helper;
  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;

  SG::ReadCondHandleKey<TRTCond::HWMap> m_HWMapReadKey{this,"HWMapReadKey","HWMap","HV map in-key"};

};

/// Query Interface
inline StatusCode TRT_HWMappingSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( ITRT_HWMappingSvc::interfaceID() == riid ) {
    *ppvIf = dynamic_cast<ITRT_HWMappingSvc*>(this);
    addRef();
  } else {
    return AthService::queryInterface( riid, ppvIf );
  }
  return StatusCode::SUCCESS;
}

#endif // TRT_HWMappingSvc_H
