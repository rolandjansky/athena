/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MDT_PrepDataToxAOD.h
//   Header file for class MDT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef MDT_PREPDATATOXAOD_H
#define MDT_PREPDATATOXAOD_H

#include "MuonPrepDataToxAOD.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"

class MDT_PrepDataToxAOD : public MuonPrepDataToxAOD<Muon::MdtPrepDataContainer,MuonSimDataCollection> {
public:
  // Constructor with parameters:
  MDT_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // algorithm execute:
  StatusCode initialize();
  StatusCode execute();

  // overload base class function to add technology specific information
  void addPRD_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::MdtPrepData& prd ) const;

  // overload this function to add detector specific information 
  void addSDO_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::MdtPrepData& prd, const MuonSimData* sdo ) const;

private:
  ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtRotCreator{this,"MdtDriftCircleOnTrackCreator","Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};
};


#endif 
