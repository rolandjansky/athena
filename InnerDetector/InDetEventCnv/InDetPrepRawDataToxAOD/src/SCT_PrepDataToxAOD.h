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

#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

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
  SG::ReadHandleKey<InDet::SCT_ClusterContainer>  m_clustercontainer;
  SG::ReadHandleKey<InDetSimDataCollection>  m_SDOcontainer;
  SG::ReadHandleKey<SiHitCollection>  m_sihitContainer;
  SG::ReadHandleKey<PRD_MultiTruthCollection>  m_multiTruth;
  SG::ReadHandleKey<SCT_RDO_Container>  m_rdoContainer;
  SG::WriteHandleKey<xAOD::TrackMeasurementValidationContainer>  m_xAodContainer;
  SG::WriteHandleKey<std::vector<unsigned int> >  m_xAodOffset;

  bool  m_useTruthInfo;
  bool  m_writeRDOinformation;
  bool  m_writeSDOs;
  bool  m_writeSiHits;
  
  std::map< Identifier, const SCT_RDORawData* > m_IDtoRAWDataMap;

  // --- private members
  bool m_firstEventWarnings;
  
};


#endif 
