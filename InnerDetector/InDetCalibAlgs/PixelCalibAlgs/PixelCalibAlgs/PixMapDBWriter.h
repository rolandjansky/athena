/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXMAPDBWRITER_H
#define PIXELCONDITIONSSERVICES_PIXMAPDBWRITER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"

class ISpecialPixelMapSvc;
class PixelID;


/**
 *
 * PixMapDBWriter.h
 *
 * Reads a map of noisy pixels from a root file prepared
 * by the algorithm NoiseMapBuilder and passes it to the algorithm
 * OutputConditionsAlg for writing to the conditions database.
 *
 * ruwiedel@physik.uni-bonn.de
 *
 */


class PixMapDBWriter : public AthAlgorithm{
  
 public:
  PixMapDBWriter (const std::string& name, ISvcLocator* pSvcLocator);
  ~PixMapDBWriter();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle< ISpecialPixelMapSvc > m_specialPixelMapSvc;

  std::string m_specialPixelMapKey;
  bool m_doValidate;
  bool m_calculateOccupancy;
  unsigned int m_nBCReadout;
  std::string m_pixelPropertyName;
  unsigned int m_pixelStatus;
  bool m_listSpecialPixels;

  std::string m_pixMapFileName;

  const PixelID* m_pixelID;
};

#endif
