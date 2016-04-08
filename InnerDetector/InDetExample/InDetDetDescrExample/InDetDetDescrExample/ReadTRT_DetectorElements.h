/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ReadTRTDetectorElements.h
 * Header file for ReadTRT_DetectorElements class
 * @author Grant Gorfine
 * updated for Eigen (matrix library) migration Nick Styles, Shaun Roe
**/
#ifndef InDetDetDescrExample_ReadTRT_DetectorElements_h
#define InDetDetDescrExample_ReadTRT_DetectorElements_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GeoPrimitives/GeoPrimitives.h"


#include <string>

class Identifier;

namespace InDetDD{
  class TRT_DetectorManager;
  class TRT_BaseElement;
  class TRT_BarrelElement;
  class TRT_EndcapElement;
}

class IGeoModelSvc;
class TRT_ID;

/**
 * @brief Algorithm to demonstrate access to TRT detector elements, 
 * printing out parameters of specific straws
 * 
**/

class ReadTRT_DetectorElements : public AthAlgorithm {
public:
  //! Default Algorithm constructor with parameters
  ReadTRT_DetectorElements (const std::string& name, ISvcLocator* pSvcLocator);
  //@name Athena Algorithm methods
  //@{
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  //@}
  
  /// GeoInit callback
  StatusCode geoInitCallback(IOVSVC_CALLBACK_ARGS);
  StatusCode geoInitialize();

  void printAllElements();

  void printBarrelStraw(const InDetDD::TRT_BarrelElement * element, const unsigned int strawNum);
  void printEndcapStraw(const InDetDD::TRT_EndcapElement * element, const unsigned int strawNum);
  void printStraw(const InDetDD::TRT_BaseElement * element, const unsigned int strawNum);

  void compareBarrelStraw(const InDetDD::TRT_BarrelElement * element, const unsigned int strawNum);
  void compareEndcapStraw(const InDetDD::TRT_EndcapElement * element, const unsigned int strawNum);
  bool comparePos(const Amg::Vector3D & p1, const Amg::Vector3D & p2, const double err);
 
private:
  std::string m_managerName;
  bool m_doInit;
  bool m_doExec;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;

  const InDetDD::TRT_DetectorManager * m_manager;
  const TRT_ID * m_idHelper;

  double m_maxdiff;
  bool m_first;
};

#endif // InDetDetDescrExample_ReadTRT_DetectorElements_h
