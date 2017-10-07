/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "HIJetRec/IHISubtractorTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "AsgTools/ToolHandle.h"


class HIJetConstituentSubtractionTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetConstituentSubtractionTool);

public:
    
  HIJetConstituentSubtractionTool(const std::string& t);
  
  /// \brief Implementing abstract methods from base
  int modify(xAOD::JetContainer& jets) const;

  /// \brief Implementing abstract methods from base, not used
  int modifyJet(xAOD::Jet&) const {return 1;};

private:

  /// \brief Name of HIEventShapeContainer
  std::string m_event_shape_key;
  std::string m_modulation_key;
  /// \brief Subtracted jet kinematics are stored 
  /// using this string like a signal state/moment
  /// Jet::setJetP4 ( std::string, JetFourMom_t)
  std::string m_moment_name;

  /// \brief if this flag is set, the primary jet 
  /// kinematics are also set to reflect this subtraction
  /// eventually do this w/ a signal state
  bool m_moment_only;
  /// \brief handle to IHISubtractorTool that determines the
  /// subtracted kinematics for each constituent
  ToolHandle<IHISubtractorTool> m_subtractor_tool;
  ToolHandle<IHIUEModulatorTool> m_modulator_tool;
  bool m_originCorrection;
  std::string m_vertexContainer;

protected:
  /// \brief Protected set/get functions provide access to private data
  /// members in derived classes
  inline std::string EventShapeKey() const {return m_event_shape_key;};
  inline std::string ModulationKey() const {return m_modulation_key;};
  inline std::string MomentName() const {return m_moment_name;};
  inline bool MomentOnly() const {return m_moment_only;};

  inline void SetEventShapeKey(std::string key) {m_event_shape_key=key;};
  inline void SetMomentName(std::string key) {m_moment_name=key;};
  inline void SetMomentOnly(bool x) {m_moment_only=x;};

};

#endif
