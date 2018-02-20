// JetOriginCorrectionTool.h  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetMomentTools_JetOriginCorrectionTool_H
#define JetMomentTools_JetOriginCorrectionTool_H

////////////////////////////////////////////////
/// \file JetOriginCorrectionTool.h
/// \class JetOriginCorrectionTool
/// 
/// This tool computes the constituent scale 4-vector of the jet 
/// relative to a PV vertex and stores it in as an attribute  
/// of the jet. 
/// This tool DOES NOT change the scale of the jet, just adds an attribute.
///
/// This tool is a wrapper : it only calls functions defined in JetUtils/JetOriginHelpers.h
///
/// Properties:
///  - VertexContainer: Name of the vertex container. The 0th vertex is used to compute the correction
///  - OriginCorrectedName: Name of the output attribute
////////////////////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"

class JetOriginCorrectionTool : public asg::AsgTool,
                                virtual public IJetModifier {
  ASG_TOOL_CLASS(JetOriginCorrectionTool, IJetModifier)
    
public:

  /// Constructor from tool name.
  JetOriginCorrectionTool(const std::string& myname);

  /// Inherited method to modify a jet. Compute the origin-corrected
  /// momentum and put it in the jet
  virtual int modify(xAOD::JetContainer& jet) const;

 protected:
  
  std::string m_vtxContainerName;
  std::string m_correctionName;
  std::string m_eInfoName;

  bool m_onlyAssignPV;
  bool m_doForceEMScale;
};

#endif
