/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIClusterSubtraction.h

#ifndef __HIJETREC_HICLUSTERSUBTRACTION_H__
#define __HIJETREC_HICLUSTERSUBTRACTION_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIClusterSubtraction
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Algorithm that applies background subtraction to clusters.
/// The subtraction needs to be handled separately from the making.
/// The unsubtracted clusters are inputs into the jet finding which
/// in turn defines the background (HIEventShapeContainer).
///
////////////////////////////////////////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetExecuteTool.h"
#include <xAODCaloEvent/CaloClusterContainer.h>
#include "AsgTools/ToolHandle.h"
#include <HIJetRec/IHISubtractorTool.h>
#include <HIJetRec/IHIUEModulatorTool.h>
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "xAODTracking/VertexContainer.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

class HIClusterSubtraction : virtual public asg::AsgTool,
			     virtual public IJetExecuteTool
{
  ASG_TOOL_CLASS(HIClusterSubtraction,IJetExecuteTool)
public:
  HIClusterSubtraction(std::string name);
  virtual StatusCode initialize();
  virtual ~HIClusterSubtraction() {};

  virtual int execute() const;

	bool doOriginCorrection( xAOD::CaloCluster* cl, const xAOD::Vertex* origin, xAOD::IParticle::FourMom_t& p4_cl ) const;


private:
  /// \brief Name of input cluster container
	SG::ReadHandleKey< xAOD::CaloClusterContainer > m_inClusterKey { this, "ClusterKey", "ClusterKey", "Name of the input Cluster Container"};
	/// |brief New writeHandleKey to store the shallow copy used for new CaloClusterTreatment
	SG::WriteHandleKey< xAOD::CaloClusterContainer > m_outClusterKey { this, "OutClusterKey", "OutClusterKey", "Name of the output Cluster Container (deep Copy)"};
  /// \brief Name of HIEventShapeContainer defining background
	SG::ReadHandleKey< xAOD::HIEventShapeContainer > m_eventShapeKey { this, "EventShapeKey", "EventShapeKey", "Name of HIEventShapeContainer defining background"};
  /// |brief Name of Vertex Container for origin correction
	SG::ReadHandleKey< xAOD::VertexContainer > m_vertexContainer { this, "VertexContainer", "PrimaryVertices", "Vertex container for primary vertices"};

	// Tool handles
  ToolHandle<IHISubtractorTool> m_subtractorTool { this, "Subtractor", "HIJetSubtractorToolBase", "Handle to IHISubtractorTool which does calculates subtracted kinematics" };
  ToolHandle<IHIUEModulatorTool> m_modulatorTool { this, "Modulator" , "HIUEModulatorTool" , "Handle to IHIModulatorTool" };
	ToolHandle<IHIEventShapeMapTool> m_eventShapeMapTool { this, "EventShapeMapTool", "HIEventShapeMapTool", "Handle to Event Shape Map Tool"};
	ToolHandleArray<CaloClusterCollectionProcessor> m_clusterCorrectionTools { this, "ClusterCorrectionTools", {}, "" };

	// Booleans
	Gaudi::Property< bool > m_setMoments { this, "SetMoments", true, "Set Moments boolean switch"};
	Gaudi::Property< bool > m_updateMode  { this, "UpdateOnly", false, "Update Mode boolean switch"};
  //Moving the origin correction directly here
	Gaudi::Property< bool > m_originCorrection { this, "ApplyOriginCorrection", false, "Apply Origin Correction boolean switch"};

};
#endif
