/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * LumiWeightHandler.h
 *
 *  Created on: Jul 10, 2014
 *      Author: goblirsc
 */

#ifndef LUMIWEIGHTHANDLER_H_
#define LUMIWEIGHTHANDLER_H_

#include <string>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "TError.h"

#include <AsgTools/ToolHandle.h>
#include "AsgTools/AsgTool.h"
#include <PileupReweighting/IPileupReweightingTool.h>

/// @class LumiWeightHandler
/// @brief Utility class to work with user specific lumi inputs


namespace CP {

    class LumiWeightHandler {
    public:
        LumiWeightHandler();
        virtual ~LumiWeightHandler();

        /// read SF periods from SF files and assign luminosity information from PileupReweightingTool
        bool LoadLumiFromInput(std::map<std::string, double>  & map, std::string file, ToolHandle<IPileupReweightingTool> &prw_tool );

    private:
        typedef std::map<std::string,double> lumimap;
        typedef std::map<std::string,double>::iterator ilumimap;

    };

} /* namespace CP */

#endif /* LUMIWEIGHTHANDLER_H_ */
