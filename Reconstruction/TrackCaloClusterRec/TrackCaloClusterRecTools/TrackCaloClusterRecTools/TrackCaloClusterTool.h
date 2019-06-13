// this file is -*- C++ -*-
/*
  Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
///
/// \file TrackCaloClusterTool.h  
///
/// Defines implementions of ITrackCaloClusterTool in charge of building different taste of TCC or UFO
/// 
/// \authors : Naomie Calace, P-A Delsart, Jennifer Roloff
///////////////////////////////////////////////////////////////////////

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTER_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_TRACKCALOCLUSTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrackCaloClusterRecInterfaces/ITrackCaloClusterTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "TrackCaloClusterRecInterfaces/IClusterFilterTool.h"



///////////////////////////////////////////////////////////////////////
/// \class TrackCaloClusterBaseTool
/// A base class for tools implementing ITrackCaloClusterTool
///
/// simply holds a few properties shared by most implementations.
class TrackCaloClusterBaseTool : virtual public ITrackCaloClusterTool, public AthAlgTool {
public:
  TrackCaloClusterBaseTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrackCaloClusterBaseTool();

  virtual StatusCode initialize();

  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & tccInfo ) const =0;
    
protected:    

  /// The tool used to make sure a track is associated to PV0
  ToolHandle<CP::ITrackVertexAssociationTool> m_loosetrackvertexassoTool;
  
  /// use cluster energy or pt?
  bool m_useEnergy;
  
  /// Particle to CaloEntry container name
  std::string m_caloEntryMapName;

  /// flag to add dectetor eta decoration onto the produced TrackCaloClusters
  bool m_saveDetectorEta;       
};



///////////////////////////////////////////////////////////////////////
/// \class TCCCombinedTool
///
/// Implements the creation of "combined" TCC objects
///
class TCCCombinedTool : public TrackCaloClusterBaseTool {
public:
  TCCCombinedTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TCCCombinedTool() {}

  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & tccInfo ) const ;

protected:  
  // enable origin correction
  bool m_doOriginCorrection;
  bool m_storeCorrectedPosition;
};


///////////////////////////////////////////////////////////////////////
/// \class TCCChargedTool
///
/// Implements the creation of "charged" TCC objects (that is tracks matched 0 clusters)
///
class TCCChargedTool : public TrackCaloClusterBaseTool {
public:
  TCCChargedTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TCCChargedTool() {}

  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & tccInfo ) const ;

protected:
  
};



///////////////////////////////////////////////////////////////////////
/// \class TCCNeutralTool
///
/// Implements the creation of "neutral" TCC objects (that is clusters matched to 0 tracks)
///
class TCCNeutralTool : public TrackCaloClusterBaseTool {
public:
  TCCNeutralTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TCCNeutralTool() {}

  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & tccInfo ) const ;
  virtual StatusCode initialize();

protected:
  /// optionnal tool to filter cluster we don't want to consider as TCC
  ToolHandle<IClusterFilterTool> m_clusterFilterTool;
  bool m_applyFilter;
};



///////////////////////////////////////////////////////////////////////
/// \class UFOTool
///
/// Implements the creation of UFO objects. These are build similarly as TCC but using PFO in the place of CaloClusters.
/// All Combined, Charged and Neutral are build together within this class.
///
class UFOTool : public TrackCaloClusterBaseTool {
public:
  UFOTool(const std::string&,const std::string&,const IInterface*);
  virtual ~UFOTool() {}

  virtual StatusCode fillTCC(xAOD::TrackCaloClusterContainer* container, const TrackCaloClusterInfo & tccInfo ) const ;

protected:
  /// Prefix to the input PFO container needed (m_pfoPrefix+"ChargedParticleFlowObjects" and m_pfoPrefix+"NeutralParticleFlowObjects")
  std::string m_pfoPrefix;

  /// cluster with E below this cut won't be considered in the TCC alg. WARNING cut must be configured as in TrackCaloClusterInfoUFOAlg
  float m_clusterEcut;
};

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_PARTICLETOCALOEXTRAPOLATIONTOOL_H
