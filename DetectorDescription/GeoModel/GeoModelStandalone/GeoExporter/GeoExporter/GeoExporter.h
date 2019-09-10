/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class GeoExporter                      //
//                                                         //
//  Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>      //
//                                                         //
//  Initial version: Sept 2016                             //
//  Major updates:   Aug 2018
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef GeoExporter_H
#define GeoExporter_H

//NB: There should never be any Qt (or Athena of course) includes in this file!!!

#include <string>
#include <vector>

class StoreGateSvc;
class IToolSvc;
class ISvcLocator;

class GeoExporter {
public:

  GeoExporter(StoreGateSvc* sg, StoreGateSvc* detstore,
	          ISvcLocator* svclocator,IToolSvc*toolSvc);//,
  ~GeoExporter();

  //We can check the validity of the argument already during algorithm initialise:
  bool argumentsAreValid() const;

  //Called in the first algorithm refresh to launch the gui:
  void init();

private:

  GeoExporter(const GeoExporter & );
  GeoExporter & operator= (const GeoExporter & );

  class  Imp;
  Imp   *m_d;

};

#endif
