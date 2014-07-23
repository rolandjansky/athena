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

/// @class LumiWeightHandler
/// @brief Utility class to work with user specific lumi inputs


namespace CP {

    class LumiWeightHandler {
    public:
        LumiWeightHandler();


        LumiWeightHandler(std::map<std::string, double> user_weights);
        virtual ~LumiWeightHandler();

        /// set the user lumi weights
        void SetLumiWeights (std::map<std::string, double> user_weights);

        /// translate per-run into per-period lumis and vice versa.
        /// will always switch to the format used in the SF input file.
        bool LoadLumiFromInput(std::map<std::string, double>  & map, std::string file);

        /// assign runs to periods
        std::string PeriodName (std::string run);

        /// translate what the users provide into the time binning known by the input file
        std::map<std::string, double> TranslateLumiElement(std::map<std::string, double>::iterator  & iu, std::map<std::string, double>  & in_lumis, bool & warned_user, std::string filename);

    private:
        typedef std::map<std::string,double> lumimap;
        typedef std::map<std::string,double>::iterator ilumimap;

        lumimap m_user_lumi_weights;
};

} /* namespace CP */

#endif /* LUMIWEIGHTHANDLER_H_ */
