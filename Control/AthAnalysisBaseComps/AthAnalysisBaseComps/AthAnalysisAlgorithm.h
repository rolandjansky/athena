///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisAlgorithm.h 
// Header file for class AthAnalysisAlgorithm
// Inherits from AthHistogramAlgorithm
// Author: W.Buttinger<will@cern.ch>
/////////////////////////////////////////////////////////////////// 

#ifndef ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
#define ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H 1

/** @class AthAnalysisAlgorithm AthAnalysisAlgorithm.h AthAnalysisBaseComps/AthAnalysisAlgorithm.h
 *
 *  Update Feb 2016: Made inherit from AthHistogramAlgorithm, since that has nice histogram booking features

 *  @author Will Buttinger
 *  @date   2015
 */ 

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools!

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#include "TFile.h"

class AthAnalysisAlgorithm
  : public ::AthHistogramAlgorithm
{
public:

  /// Constructor taking just a name
  AthAnalysisAlgorithm(const std::string& name);

  /// Constructor with parameters: 
  AthAnalysisAlgorithm(const std::string& name
		       , ISvcLocator* pSvcLocator
		       , const std::string& version=PACKAGE_VERSION);

  virtual ~AthAnalysisAlgorithm() override;

  /// @name Functions providing access to the input/output metadata
  /// @{

  /// Accessor for the input metadata store
  ServiceHandle< StoreGateSvc >& inputMetaStore() const;
  /// Accessor for the output metadata store
  ServiceHandle< StoreGateSvc >& outputMetaStore() const;

  /// @}

  /// Function initialising the tool in the correct way in Athena
  virtual StatusCode sysInitialize() override;
  /// override to do firstEvent method
  virtual StatusCode sysExecute(const EventContext&) override;

protected:
  void updateEvtStore(Gaudi::Details::PropertyBase& prop);

  /// Function called when first execute is encountered
  /// user can read event information with evtStore()
  virtual StatusCode firstExecute();

private:
  /// Object accessing the input metadata store
  mutable ServiceHandle< StoreGateSvc > m_inputMetaStore;
  /// Object accessing the output metadata store
  mutable ServiceHandle< StoreGateSvc > m_outputMetaStore;

  bool m_doneFirstEvent{false};
};

#endif //> !ATHANALYSISBASECOMPS_ATHANALYSISALGORITHM_H
