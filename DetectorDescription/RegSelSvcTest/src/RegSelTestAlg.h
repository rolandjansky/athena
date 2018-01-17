/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef REGIONSELECTOR_REGSELTESTALG_H
#define REGIONSELECTOR_REGSELTESTALG_H 1

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "IRegionSelector/IRegSelSvc.h"


/**
 * @brief Testing Region selector for threading issues
 **/

class RegSelTestAlg
  : public ::AthReentrantAlgorithm
{ 
 public: 

  RegSelTestAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RegSelTestAlg(); 


  virtual StatusCode  initialize() override;
  virtual StatusCode  execute_r(const EventContext& context) const override;
  virtual StatusCode  finalize() override;
  StatusCode dump() const;
  StatusCode mt() const;


 private: 

  StatusCode dumpOne( const std::string& message, double eta, const std::vector<DETID>& detectors ) const;
  RegSelTestAlg();
  Gaudi::Property<bool> m_dump{ this, "Dump", false, "Generate series of RoIs and dump the ROBS & hashes for them in the output" };
  Gaudi::Property<bool> m_mt{ this, "Mt", false, "Generate series of RoIs and ask RegionSelctor for the ROBS and hashes from multiple threads" };
  
  ServiceHandle<IRegSelSvc> m_regSelSvc{ this, "RegSelSvc", "RegSelSvc/RegSelSvc", "Region selector service instance" };
}; 


#endif //> !REGIONSELECTOR_REGSELTESTALG_H
