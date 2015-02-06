/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIJetMaxOverMeanTool.h

#ifndef __HIJETREC_HIJETMAXOVERMEANTOOL_H__
#define __HIJETREC_HIJETMAXOVERMEANTOOL_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIJetMaxOverMeanTool
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief JetModifier that calculates moments in HI subtraction bootstrap.
/// The ET of each jet constituent (HI clusters) is considered
/// The maximum ET and maximum / mean are calculated from this set of values
/// and stored as moments MaxConstituentET and MaxOverMean, respectively.
///
////////////////////////////////////////////////////////////////////////////////

#include "JetRec/JetModifierBase.h"

class HIJetMaxOverMeanTool : public JetModifierBase
{

  ASG_TOOL_CLASS0(HIJetMaxOverMeanTool)

public:
    
  HIJetMaxOverMeanTool(const std::string& t);

  /// \brief Implementing abstract methods from base
  int modifyJet(xAOD::Jet& jet) const;

    
};

#endif
