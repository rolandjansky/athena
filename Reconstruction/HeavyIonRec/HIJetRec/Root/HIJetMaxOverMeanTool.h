/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "StoreGate/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgTools/AsgTool.h"

class HIJetMaxOverMeanTool : public asg::AsgTool,
                             virtual public IJetDecorator
{

  ASG_TOOL_CLASS0(HIJetMaxOverMeanTool)

public:

  HIJetMaxOverMeanTool(const std::string& t);

  virtual StatusCode initialize() override;

  //The modifyJet function has to be replaced by decorate
  //virtual int modifyJet(xAOD::Jet& ) const ;
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private:

  SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetMaxConstituentETKey { this, "MaxConstituentETKey", "MaxConstituentET", "Key for MaxConstituentET tile Jet attribute"};
  SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetMaxOverMeanKey { this, "MaxOverMeanKey", "MaxOverMean", "Key for MaxOverMean tile Jet attribute"};

  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};

};

#endif
