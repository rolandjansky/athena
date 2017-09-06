/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1SysConf                          //
//                                                            //
//  Description: Utilities to be called at the end of plugin  //
//  constructors to set up standard system options and        //
//  inter-system connections                                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1SYSCONF_H
#define VP1SYSCONF_H

#include <QStringList>

class VP1GuideLineSystem;
class VP1GeometrySystem;
class VP1TrackSystem;
class VP1PrepRawDataSystem;
class VP1VertexSystem;

class VP1SysConf {
public:

  //Flag determining whether setting geometry system up for automatic
  //muon system configuration, lower curved surface complexity,
  //etc. (EVENTSTUDIES), or just optimise it for geometry
  //(GEOMSTUDIES)
  enum CHANNELMODE { GEOMSTUDIES, EVENTSTUDIES };

  static void setupStandardConnectionsAndOptions( VP1GuideLineSystem*,
						  VP1GeometrySystem*,
						  VP1TrackSystem *,
						  VP1PrepRawDataSystem *,
              VP1VertexSystem*,
						  CHANNELMODE cm = GEOMSTUDIES );
  //NB: It is ok to pass null pointers for some of the systems.

  //Add extra geometry and track systems?
  static QStringList extraGeometrySystems();
  static QStringList extraTrackSystems();

private:

  VP1SysConf();
  ~VP1SysConf();

};

#endif
