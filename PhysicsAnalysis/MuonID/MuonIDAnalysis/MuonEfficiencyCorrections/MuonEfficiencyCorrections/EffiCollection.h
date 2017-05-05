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
            std::string file_lowpt_central,
            std::string file_lowpt_caloo,
//             std::string file_lowpt_forward,
            std::map<std::string,double> lumis_central,
            std::map<std::string,double> lumis_calo,
            std::map<std::string,double> lumis_forward,
            std::map<std::string,double> lumis_lowpt_central,
            std::map<std::string,double> lumis_lowpt_calo,
//             std::map<std::string,double> lumis_lowpt_forward,
            SystematicSet sys, std::string effType="EFF", double lowPtTransition = 20000.);

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
    EfficiencyScaleFactor* m_lowpt_central_eff;
    EfficiencyScaleFactor* m_lowpt_calo_eff;
//     EfficiencyScaleFactor* m_lowpt_forward_eff;

private:
    std::string m_effType;
    double m_lowpt_transition;
};
}
#endif /* EFFICOLLECTION_H_ */
