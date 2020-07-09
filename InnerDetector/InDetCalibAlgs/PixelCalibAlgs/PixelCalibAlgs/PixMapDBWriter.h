/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXMAPDBWRITER_H
#define PIXELCONDITIONSSERVICES_PIXMAPDBWRITER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

class ISpecialPixelMapSvc;
class PixelID;

#include <string>
#include <sstream>
#include <vector>
#include <fstream>

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
  bool m_isIBL;
  std::string m_pixMapFileName;

  const PixelID* m_pixelID;

  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

  std::vector< std::pair< std::string, std::vector<int> > > m_pixelMapping;
  std::vector<int> getPositionFromDCSID (const std::string& DCSID);
  std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);

  std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
    return elems;
  };

  std::vector<std::string> splitter(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitter(s, delim, elems);
    return elems;
  };

  bool is_file_exist(const char *fileName)
  {
    std::ifstream infile;
    infile.open(fileName);
    return infile.good();
  };
};

#endif
