/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiGlobAlignDataBuilder.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-02-23
 * 
 * @brief  Header file for class SiGlobAlignDataBuilder.
 * 
 */

#ifndef AFP_DBTOOLS_SIGLOBALIGNDATABUILDER_H
#define AFP_DBTOOLS_SIGLOBALIGNDATABUILDER_H

#include"DBObjectBuilder.h"
#include"SiGlobAlignData.h"

#include<string>

namespace AFP
{
  /// Class for building tracking detectors local alignment info from conditions database
  class SiGlobAlignDataBuilder : public DBObjectBuilder<SiGlobAlignData>
  {
  public:

    /// @copydoc DBObjectBuilder::addBuilderCommand
    virtual bool addBuilderCommand (const std::string& name) override;
  };

} // namespace AFP

#endif	//  AFP_DBTOOLS_SIGLOBALIGNDATABUILDER_H
