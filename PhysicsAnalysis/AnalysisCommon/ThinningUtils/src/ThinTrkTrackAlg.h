///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGUTILS_ThinTrkTrackAlg_H
#define THINNINGUTILS_ThinTrkTrackAlg_H 
/**
 @class ThinTrkTrackAlg
*/
// STL includes
#include <string>


// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class IThinningSvc;


class ThinTrkTrackAlg : public ::AthAlgorithm {
public:
  
  /// Constructor with parameters:
  ThinTrkTrackAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
  /// Destructor:
  virtual ~ThinTrkTrackAlg();
  
  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize() final;
  
  /// Athena algorithm's execute hook
  virtual StatusCode  execute() final;
  
  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize() final;
  
 private:
  /// Inline method
  StatusCode doElectrons() ;
  StatusCode doMuons() ;
  
  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;
  
  /// Should the thinning run?
  bool m_doElectrons;
  bool m_doMuons;
  ///Names of the containers of Objects of interest Electron/Muon
  std::string m_muonsKey;
  std::string m_electronsKey;
  /// Names of the containers to thin
  std::string m_CombinedMuonsTracksKey;
  std::string m_GSFTracksKey;

};


#endif 
