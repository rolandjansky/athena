/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1DetInfo                          //
//                                                            //
//  Description: Convenient and safe access to detector       //
//               managers and identifier decoding             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1DETINFO_H
#define VP1DETINFO_H

class IVP1System;
class StoreGateSvc;
namespace InDetDD { class TRT_DetectorManager; }
namespace InDetDD { class PixelDetectorManager; }
namespace InDetDD { class SCT_DetectorManager; }
namespace MuonGM { class MuonDetectorManager; }
class TileDetDescrManager;
class EMBDetectorManager;
class HECDetectorManager;
class EMECDetectorManager;
class LArDetectorManager;
class FCALDetectorManager;
class CaloIdManager;

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class sTgcIdHelper;
class MdtIdHelper;
class MmIdHelper;

class Identifier;

class VP1DetInfo {
public:
  static void ensureInit( IVP1System* ) {}
  static void ensureInit( StoreGateSvc* ) {}

  //NB: Any of the methods returning pointers in this class might
  //return null. And it is never allowed to delete any of the returned
  //pointers!

  static const InDetDD::PixelDetectorManager * pixelDetMgr();
  static const InDetDD::SCT_DetectorManager * sctDetMgr();
  //  static const InDetDD::InDetServMatManager * inDetServMatDetMgr();
  static const InDetDD::TRT_DetectorManager * trtDetMgr();
  static const MuonGM::MuonDetectorManager * muonDetMgr();
  static const TileDetDescrManager * tileDetMgr();
  static const EMBDetectorManager * embDetMgr();
  static const HECDetectorManager * hecDetMgr();
  static const EMECDetectorManager * emecDetMgr();
  static const LArDetectorManager * larDetMgr();
  static const FCALDetectorManager * fcalDetMgr();

  static const CaloIdManager * caloIDMgr();

  //Base identifier helper:
  static const AtlasDetectorID * atlasIDHelper();

  //Common specialised identifier helpers:
  static const CscIdHelper * cscIDHelper();
  static const RpcIdHelper * rpcIDHelper();
  static const TgcIdHelper * tgcIDHelper();
  static const sTgcIdHelper * stgcIDHelper();
  static const MmIdHelper * mmIDHelper();
  static const MdtIdHelper * mdtIDHelper();
  static const PixelID * pixelIDHelper();
  static const SCT_ID * sctIDHelper();
  static const TRT_ID * trtIDHelper();
  //Todo: Add more as needed.

  //Combines use of the atlasIDHelper and VP1JobConfigInfo to spot
  //unsafe identifiers (i.e. a pixel identifier is unsafe if the.pixel
  //geometry is not lodaded):
  static bool isUnsafe(const Identifier&);

private:
  VP1DetInfo(){}
  ~VP1DetInfo(){}
  class Imp;
};


#endif
