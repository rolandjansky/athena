/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileDigitsGainFilter.h
//
// DESCRIPTION
// 
// Copy TileDigits from input container to output container
//
// Properties (JobOption Parameters):
//
//    InputDigitsContainer      string   Name of container with TileDigits to read
//    OutputDigitsContainer     string   Name of container with TileDigits to write
//
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************

#ifndef TILETBREC_TILEDIGITSGAINFILTER_H
#define TILETBREC_TILEDIGITSGAINFILTER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class TileHWID;

#include <string>
#include <vector>

/** 
 @class TileDigitsGainFilter
 @brief This algorithm copies TileDigits from input container to output container
 */
class TileDigitsGainFilter: public AthAlgorithm {
  public:
    // Constructor
    TileDigitsGainFilter(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileDigitsGainFilter();

    //Gaudi Hooks
    StatusCode initialize() override; //!< initialize method
    StatusCode execute() override;    //!< execute method
    StatusCode finalize() override;   //!< finalize method

  private:

    const TileHWID*    m_tileHWID;

    std::string m_inputContainer;  //!< Name of the input TileDigitsContainer
    std::string m_outputContainer; //!< Name of the output TileDigitsContainer

    int m_threshold;

};

#endif // TILETBREC_TILEDIGITSGAINFILTER_H
