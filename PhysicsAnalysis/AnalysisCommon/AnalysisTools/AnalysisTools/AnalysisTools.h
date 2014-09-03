/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOOLS_ANALYSISTOOLS_H
#define ANALYSISTOOLS_ANALYSISTOOLS_H

/**
   This class structure is needed for component library

   @author Tadashi Maeno
 */

#include "AnalysisTools/IAnalysisTools.h"

// interface ID
static const InterfaceID IID_IAnalysisTools("AnalysisTools", 1, 0);

class AnalysisTools : public IAnalysisTools
{
public:

  /** constructor
   */
  AnalysisTools ( const std::string& type, const std::string& name,
		  const IInterface* parent ) ;

  /** initialize
   */
  StatusCode initialize();

  /** InterfaceID
   */
  static const InterfaceID& interfaceID( )
  { return IID_IAnalysisTools; }


};

#endif
