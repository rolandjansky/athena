/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXELMAPTESTALG_H
#define PIXELCONDITIONSSERVICES_PIXELMAPTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"


class ISpecialPixelMapSvc;
class IInDetConditionsSvc;
class PixelID;

namespace InDetDD{ 
  class PixelDetectorManager; 
}

/**
 * Illustrates the usage of the SpecialPixelMapSvc.
 *
 * This algorithm can be used to experiment with special pixel maps, for example reading
 * maps from the database and printing them. The algorithm can also easily be modified
 * to read a set of maps from text (or ROOT) files and write them to a database.
 *
 * ruwiedel@physik.uni-bonn.de
 *
 */


class PixelMapTestAlg: public AthAlgorithm{
  
 public:
  PixelMapTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~PixelMapTestAlg();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle< ISpecialPixelMapSvc > m_specialPixelMapSvc;
  ServiceHandle< IInDetConditionsSvc > m_pixelConditionsSummarySvc;

  bool m_useSummarySvc;
  bool m_writeTextfile;
  bool m_dummy;
  bool m_overlay;
  
  const PixelID* m_pixelID;
  const InDetDD::PixelDetectorManager* m_pixman;

};

#endif
