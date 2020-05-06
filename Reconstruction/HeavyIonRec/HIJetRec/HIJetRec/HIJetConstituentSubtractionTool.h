/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// HIJetConstituentSubtractionTool.h

#ifndef __HIJETREC_HIJETCONSTITUENTSUBTRACTIONTOOL_H__
#define __HIJETREC_HIJETCONSTITUENTSUBTRACTIONTOOL_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetConstituentSubtractionTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief JetModifier that actual does the background subtraction.
/// This tool is the most important one applied to HI jets!
///
/// Given an HIEventShapeContainer and subtractor tool it gets the subtracted
/// kinematics for each constituent in a jet.
/// The details of how the subtraction actually happens, e.g. navigating back
/// to cells, is encapsulated in the subtractor tool.
///
/// Forseen that this may serve as a base class, thus private members have
/// protected set/get functions. Initialization of these members is unambiguous
/// as it is done by the base class.
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "JetRec/JetModifierBase.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "HIJetRec/IHISubtractorTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "AsgTools/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODTracking/VertexContainer.h"

class HIJetConstituentSubtractionTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetConstituentSubtractionTool);

public:

  HIJetConstituentSubtractionTool(const std::string& t);

  virtual StatusCode initialize() override;

  /// \brief Implementing abstract methods from base
  StatusCode modify(xAOD::JetContainer& jets) const;

  /// \brief Implementing abstract methods from base, not used
  int modifyJet(xAOD::Jet&) const {return 1;};

private:

  // Migration to data handles
  /// \brief Name of HIEventShapeContainer
  SG::ReadHandleKey< xAOD::HIEventShapeContainer > m_eventShapeKey { this, "EventShapeKey", "", "The input HI Event Shape"};

  SG::ReadHandleKey< xAOD::VertexContainer > m_vertexContainer { this, "VertexContainer", "PrimaryVertices", "Vertex container for primary vertices"};

  //That looks useless. commented out for the moment
  //std::string m_modulation_key;
  /// \brief Subtracted jet kinematics are stored
  /// using this string like a signal state/moment
  /// Jet::setJetP4 ( std::string, JetFourMom_t)
  /// not a key - but we bring it
  Gaudi::Property< std::string > m_momentName { this, "MomentName", HIJetRec::subtractedJetState(), "Subtracted Jet Kineamtics are stored using this string like a signal/state moment" }  ;

  /// \brief if this flag is set, the primary jet
  /// kinematics are also set to reflect this subtraction
  /// eventually do this w/ a signal state
  Gaudi::Property< bool > m_momentOnly { this, "SetMomentOnly", true,
          "If this flag is set, the primary jet kinematics are also set to reflect this subtraction eventually do this w/ a signal state" }  ;
  /// \brief handle to IHISubtractorTool that determines the
  /// subtracted kinematics for each constituent
  ToolHandle<IHISubtractorTool> m_subtractorTool { this, "Subtractor", "HIJetClusterSubtractorTool", "" };
  ToolHandle<IHIUEModulatorTool> m_modulatorTool { this, "Modulator" , "HIUEModulatorTool ", "" };

  Gaudi::Property< bool > m_originCorrection { this, "ApplyOriginCorrection", false, "Apply Origin Correction boolean switch"};

protected:
  /// \brief Protected set/get functions provide access to private data
  /// members in derived classes
  inline SG::ReadHandleKey< xAOD::HIEventShapeContainer > EventShapeKey()  { return m_eventShapeKey; };
  //That looks useless. commented out for the moment
  //inline std::string ModulationKey() const {return m_modulation_key;};
  inline std::string momentName() const { return m_momentName; };
  inline bool momentOnly() const { return m_momentOnly; };

  inline void setEventShapeKey( SG::ReadHandleKey< xAOD::HIEventShapeContainer > key ) { m_eventShapeKey=key; };
  inline void setMomentName(std::string key) { m_momentName=key; };
  inline void setMomentOnly(bool x) { m_momentOnly=x; };

};

#endif
