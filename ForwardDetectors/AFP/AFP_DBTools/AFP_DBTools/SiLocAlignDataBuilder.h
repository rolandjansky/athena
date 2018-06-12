/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiLocAlignDataBuilder.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-17
 * 
 * @brief  Header file for class SiLocAlignDataBuilder.
 * 
 */

#ifndef AFP_DBTOOLS_SILOCALIGNDATABUILDER_H
#define AFP_DBTOOLS_SILOCALIGNDATABUILDER_H

#include"DBObjectBuilder.h"
#include"SiLocAlignData.h"

#include<string>

namespace AFP
{
  /// Class for building tracking detectors local alignment info from conditions database
  class SiLocAlignDataBuilder : public DBObjectBuilder<SiLocAlignData>
  {
  public:

    /// @copydoc DBObjectBuilder::addBuilderCommand
    virtual bool addBuilderCommand (const std::string& name) override;
  };

} // namespace AFP

#endif	//  AFP_DBTOOLS_SILOCALIGNDATABUILDER_H
