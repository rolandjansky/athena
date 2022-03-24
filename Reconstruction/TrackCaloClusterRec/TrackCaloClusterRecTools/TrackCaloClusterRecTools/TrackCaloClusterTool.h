// this file is -*- C++ -*-
/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
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
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "StoreGate/ReadDecorHandleKey.h"

#include "TrackCaloClusterRecInterfaces/ITrackCaloClusterTool.h"
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

  virtual StatusCode initialize() override ;

  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & tccInfo ) const override =0;
    
protected:    

  /// The tool used to make sure a track is associated to PV0
  ToolHandle<CP::ITrackVertexAssociationTool> m_trackVertexAssoTool {this ,"TrackVertexAssoTool", ""};
  
  /// use cluster energy or pt?
  Gaudi::Property<bool> m_useEnergy {this, "UseEnergy", false, "if false will use pT for weights evaluation"};


  SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_assoClustersKey{this, "AssoClustersDecor", "TCCAssoClusters", "Key to access clusters associated to a track. IMPORTANT must be constistent with the TrackParticleContainer the parent alg will operate on"};
  

  
  SG::ReadDecorHandleKey<xAOD::TrackParticleContainer> m_caloEntryParsDecor {this, "CaloEntryParsDecorKey" , "","IMPORTANT must be constistent with the TrackParticleContainer the parent alg will operate on, REQUIRED if SaveDetectorEta or DoOriginCorrection, else leave blank so the scheduler won't require it unnecessarily."};

  /// flag to add dectetor eta decoration onto the produced TrackCaloClusters
  Gaudi::Property<bool> m_saveDetectorEta {this, "SaveDetectorEta", false, ""};       
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

  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & tccInfo ) const override;

protected:  
  // enable origin correction
  Gaudi::Property<bool> m_doOriginCorrection {this, "DoOriginCorrection", false, ""};
  Gaudi::Property<bool> m_storeCorrectedPosition{this, "StoreCorrectedPosition", false, ""};
};


///////////////////////////////////////////////////////////////////////
/// \class TCCChargedTool
///
/// Implements the creation of "charged" TCC objects (that is tracks matched 0 clusters)
///
class TCCChargedTool : public TrackCaloClusterBaseTool {
public:
  TCCChargedTool(const std::string&,const std::string&,const IInterface*);

  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & tccInfo ) const override;

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

  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & tccInfo ) const override ;
  virtual StatusCode initialize() override;

protected:
  /// optionnal tool to filter cluster we don't want to consider as TCC
  ToolHandle<IClusterFilterTool> m_clusterFilterTool {this, "ClusterFilterTool", ""};
  bool m_applyFilter ; // set automatically according to ClusterFilterTool
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
    virtual StatusCode initialize() override;// force override of base class to add extra handles  
  virtual StatusCode fillTCC(xAOD::FlowElementContainer* container, const TrackCaloClusterInfo & tccInfo ) const override;

protected:

  SG::ReadHandleKey<xAOD::FlowElementContainer> m_inputPFOHandle {this, "InputPFO", "CHSParticleFlowObjects", ""};
  SG::ReadDecorHandleKey<xAOD::FlowElementContainer> m_orig_pfo{this, "OriginPFO", "Origin_pfo", "Key to access original un-modified pfo"};

  /// cluster with E below this cut won't be considered in the TCC alg. WARNING cut must be configured as in TrackCaloClusterInfoUFOAlg
  Gaudi::Property<float> m_clusterEcut{this , "ClusterECut", 0, " Impotant !! : must be the same value as in TrackCaloClusterInfoUFOAlg"};
};

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECTOOLS_PARTICLETOCALOEXTRAPOLATIONTOOL_H
