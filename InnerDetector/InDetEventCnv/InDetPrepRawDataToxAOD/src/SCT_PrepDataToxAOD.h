/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_PrepDataToxAOD.h
//   Header file for class SCT_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef SCT_PREPDATATOXAOD_H
#define SCT_PREPDATATOXAOD_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "InDetSimEvent/SiHitCollection.h"
#include "xAODTracking/TrackMeasurementValidation.h"


#include <string>

class SCT_ID;
class SiHit;
class InDetSimDataCollection;
class SCT_RDORawData;
class Identifier;

namespace InDet
{
  class SCT_Cluster;
}


class SCT_PrepDataToxAOD : public AthAlgorithm, virtual public IIncidentListener {

public:
  // Constructor with parameters:
  SCT_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual void handle(const Incident& inc);
  
private:


  void addSDOInformation( xAOD::TrackMeasurementValidation* xprd,
                          const InDet::SCT_Cluster* prd,
                          const InDetSimDataCollection* sdoCollection ) const;


  void addSiHitInformation( xAOD::TrackMeasurementValidation* xprd, 
                             const InDet::SCT_Cluster* prd,
                             const SiHitCollection* collection) const;

  std::vector<SiHit>  findAllHitsCompatibleWithCluster(const InDet::SCT_Cluster* prd,
                                                       const SiHitCollection* collection) const;

  void addRDOInformation(xAOD::TrackMeasurementValidation*, const InDet::SCT_Cluster*) const;
 
 
  ServiceHandle<IIncidentSvc>                         m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent
 
  const SCT_ID *m_SCTHelper;
  std::string  m_clustercontainer;
  std::string  m_SDOcontainer;
  std::string  m_sihitContainer;
  std::string  m_multiTruth;

  bool  m_useTruthInfo;
  bool  m_writeRDOinformation;
  bool  m_writeSDOs;
  bool  m_writeSiHits;
  
  std::map< Identifier, const SCT_RDORawData* > m_IDtoRAWDataMap;

  // --- private members
  bool m_firstEventWarnings;
  
};


#endif 
