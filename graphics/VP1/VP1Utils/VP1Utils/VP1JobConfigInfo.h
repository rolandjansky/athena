/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1JobConfigInfo                    //
//                                                            //
//  Description: Provides convenient and safe information     //
//               about the configuration of a given job       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1JOBCONFIGINFO_H
#define VP1JOBCONFIGINFO_H

class IVP1System;
class StoreGateSvc;
class GeoPhysVol;
class GeoPVConstLink;

class VP1JobConfigInfo {
public:

  static void ensureInit( IVP1System* ) {}//obsolete
  static void ensureInit( StoreGateSvc* ) {}//obsolete

  //These next methods tells us what geomodel parts are initialised:
  static bool hasGeoModelExperiment();//If GeoModelExperiment/"ATLAS" can be retrieved

  static bool hasPixelGeometry();
  static bool hasSCTGeometry();
  static bool hasTRTGeometry();

  static bool hasInDetServiceMaterialGeometry();
  static bool hasBeamPipeGeometry();

  static bool hasLArGeometry();
  static bool hasTileGeometry();

  static bool hasMuonGeometry();
  static bool hasMuonNSWGeometry();

  static bool hasLUCIDGeometry();
  static bool hasCavernInfraGeometry();//fixme: update so it doesn't just return false!
  static bool hasBCMGeometry();

  //TODO: Add ZeroDegree and ALFA when they become available.

  //Top geomodel volume link (not strictly "JobConfig", but here it is):
  static const GeoPVConstLink * geoModelWorld();//might return 0

private:
  VP1JobConfigInfo(){}
  ~VP1JobConfigInfo(){}
  class Imp;
};

#endif
