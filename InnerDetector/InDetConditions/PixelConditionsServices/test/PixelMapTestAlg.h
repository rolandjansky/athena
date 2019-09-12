/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXELMAPTESTALG_H
#define PIXELCONDITIONSSERVICES_PIXELMAPTESTALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"


class ISpecialPixelMapSvc;
class IInDetConditionsTool;
class PixelID;

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
  ToolHandle< IInDetConditionsTool > m_pixelConditionsSummarySvc;

  bool m_useSummarySvc;
  bool m_writeTextfile;
  bool m_dummy;
  bool m_overlay;
  
  const PixelID* m_pixelID;
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

};

#endif
