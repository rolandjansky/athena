/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class DumpGeo                           //
//                                                         //
//  update: Riccardo-Maria BIANCHI <rbianchi@cern.ch>      //
//          23 May 2014                                    //
//                                                         //
//  This is the Athena algorithm to dump the geometry      //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef DumpGeo_DumpGeo
#define DumpGeo_DumpGeo

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>
#include <vector>

class GeoExporter;

class DumpGeo: public AthAlgorithm,
	      public IIncidentListener
{
 public:
  DumpGeo(const std::string& name, ISvcLocator* pSvcLocator);
  ~DumpGeo()=default;

  StatusCode initialize();
  StatusCode execute();

  void handle(const Incident& inc);

 private:
  IToolSvc* m_toolSvc;
  GeoExporter* m_geoExporter;

  bool m_noGui;//For testing job-options in RTT
};

#endif
