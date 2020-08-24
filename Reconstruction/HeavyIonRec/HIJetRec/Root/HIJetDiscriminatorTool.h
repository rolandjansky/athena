/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetJetDiscriminatorTool.h

#ifndef __HIJETREC_HIJETDISCRIMINATORTOOL_H__
#define __HIJETREC_HIJETDISCRIMINATORTOOL_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetDiscriminatorTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief JetModifier that filters collection based on values of moments
/// MaxConstituentET and MaxOverMean set by HIJetMaxOverMeanTool.
/// Only jets w/ values of these moments larger than m_MaxOverMean_cut and
/// m_ET_min_cut respectively are kept.
/// values of these selection cuts are controlled by declareProperty w/ names
/// MinimumETMaxCut and MaxOverMeanCut respectively.
///
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "AsgTools/AsgTool.h"
#include "JetRec/JetModifierBase.h"


class HIJetDiscriminatorTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetDiscriminatorTool)

public:

  HIJetDiscriminatorTool(const std::string& t);

  /// \brief Implementing abstract methods from base
  StatusCode modify(xAOD::JetContainer& jets) const;

  /// \brief Implementing abstract methods from base, not used
  int modifyJet(xAOD::Jet&) const {return 0;};

protected:

  /// \brief minimum cut applied to MaxOverMean moment
  Gaudi::Property< float > m_MaxOverMeanCut { this, "MaxOverMeanCut", 4., "Minimum cut applied to MaxOverMean moment" };

  /// \brief minimum cut applied to MaxConstituentET moment
  Gaudi::Property< float > m_ETminCut { this, "MinimumETMaxCut", 3000., "Minimum cut applied to MaxConstituentET moment" };


};

#endif
