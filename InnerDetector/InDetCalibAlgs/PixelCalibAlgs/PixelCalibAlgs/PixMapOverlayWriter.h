/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCALIBALGS_PIXMAPOVERLAYWRITER_H
#define PIXELCALIBALGS_PIXMAPOVERLAYWRITER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ServiceHandle.h"

#include<string>
#include<vector>


class PixelID;
class ISpecialPixelMapSvc;


/** 
 * Algorithm for masking pixel modules, frontends and column pairs. The list of
 * disabled areas is written to an sqlite file which can either be merged with 
 * the oracle datbase or read directly by an athena job
 *
 * ruwiedel@physik.uni-bonn.de 
 *
 */


class PixMapOverlayWriter: public AthAlgorithm{
  
 public:
  PixMapOverlayWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~PixMapOverlayWriter();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  ServiceHandle< ISpecialPixelMapSvc > m_specialPixelMapSvc;

  // offline module IDs (do not require connectivity DB)
  
  std::vector<unsigned int> m_modules;
  unsigned int m_moduleStatus;


  // the following require connectivity DB access

  std::vector<std::string> m_moduleIDFragments;
  std::vector<std::string> m_pp0s;
  std::vector<std::string> m_rods;
  std::vector<unsigned int> m_coolingLoops;
  std::vector<std::string> m_genericObjects;

  std::map< std::string, std::vector<int> > m_frontends;
  std::map< std::string, std::vector<int> > m_columnPairs;
  
  std::string m_connectionString;
  std::string m_connectivityTag;
  std::string m_aliasTag;

  const PixelID* m_pixelID;
};

#endif
