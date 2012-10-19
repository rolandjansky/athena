/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUTRACKTOOLS_ITAUSEEDTRUTHMATCHTOOL_H
#define TAUTRACKTOOLS_ITAUSEEDTRUTHMATCHTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "PanTauEvent/TauSeedCollection.h"      // typedef
#include "PanTauEvent/SeedTruthMap.h"           // typedef
//#include "McParticleEvent/TruthParticleContainer.h"

class TruthParticleContainer;

namespace TauID {
//class SeedTruthMap;

static const InterfaceID IID_ITauSeedTruthMatchTool("PanTau::ITauSeedTruthMatchTool", 1, 0);

/** @class ITauSeedTruthMatchTool
    @brief Interface for matching tau seeds to truth taus.

    @author Sebastian Fleischmann
*/
class ITauSeedTruthMatchTool : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID( ) ;

    /** create a tau seed to truth map. The map belongs to
        you, the user, and so you must take care of deletion of it.*/
    virtual const PanTau::SeedTruthMap* matchSeedsToTruth(
                                                            const TruthParticleContainer& truthTauCont,
                                                            const PanTau::TauSeedCollection& tauSeedColl ) const = 0;


};

inline const InterfaceID& TauID::ITauSeedTruthMatchTool::interfaceID() {
    return IID_ITauSeedTruthMatchTool;
}


}
#endif // PANTAUINTERFACES_ITAUSEEDTRUTHMATCHTOOL_H
