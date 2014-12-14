/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EffiCollection.h
 *
 *  Created on: Jun 21, 2014
 *      Author: goblirsc
 */

#ifndef EFFICOLLECTION_H_
#define EFFICOLLECTION_H_

// @class EffiCollection
/// @brief Utility class to collect the separate histos
/// for Calo, Central and High Eta muon SF

#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODMuon/Muon.h"

#include <map>
#include <string>
#include <iostream>

namespace CP {

class EffiCollection {
public:
    EffiCollection();
    EffiCollection(std::string file_central,
            std::string file_calo,
            std::string file_forward,
            std::map<std::string,double> lumis_central,
            std::map<std::string,double> lumis_calo,
            std::map<std::string,double> lumis_forward,
            SystematicSet sys);

    EffiCollection (const EffiCollection & other);
    EffiCollection & operator = (const EffiCollection & other);
    
    /// return the correct SF type to provide, depending on eta and the author
    EfficiencyScaleFactor* operator()(const xAOD::Muon & mu);

    // return the name of the systematic variation being run
    std::string sysname ();

    // a consistency check
    bool CheckConsistency();


    virtual ~EffiCollection();

protected:

    EfficiencyScaleFactor* m_central_eff;
    EfficiencyScaleFactor* m_forward_eff;
    EfficiencyScaleFactor* m_calo_eff;
};
}
#endif /* EFFICOLLECTION_H_ */
