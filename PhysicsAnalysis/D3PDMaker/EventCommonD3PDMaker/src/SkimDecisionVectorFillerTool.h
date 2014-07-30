// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/SkimDecisionVectorFillerTool.h
 * @author jgoodson <jgoodson@cern.ch>
 * @date Oct, 2010
 * @brief Block filler tool for SkimDecisions
 */



#ifndef EVENTCOMMOND3PDMAKER_SKIMDECISIONVECTORFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_SKIMDECISIONVECTORFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include <string>

class SkimDecisionCollection;


namespace D3PD {

  
class SkimDecisionVectorFillerTool
  : public BlockFillerTool<SkimDecisionCollection>
{
public:
  /// Regular AlgTool constructor
  SkimDecisionVectorFillerTool( const std::string& type,
                                const std::string& name, 
                                const IInterface* parent);

  /// Function booking the variables
  StatusCode book();
  /// Function filling the variables
  StatusCode fill(const SkimDecisionCollection& x );

private:
  // Properties
  std::vector<std::string> m_skimDecisionMatchName;
  bool m_doAllSkimDecisions;

  std::vector<std::string> *m_skimDecisionName;
  std::vector<int> *m_skimPass;

}; // class SkimDecisionVectorFillerTool


} // namespace D3PD


#endif // EVENTCOMMOND3PDMAKER_SKIMDECISIONVECTORFILLERTOOL_H

