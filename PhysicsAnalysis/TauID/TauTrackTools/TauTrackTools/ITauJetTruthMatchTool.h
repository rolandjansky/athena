/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUTOOLS_ITAUJETTRUTHMATCHTOOL_H
#define TAUTOOLS_ITAUJETTRUTHMATCHTOOL_H

#include "GaudiKernel/IAlgTool.h"
//#include "McParticleEvent/TruthParticleContainer.h"
#include "TauTrackEvent/TauJetTruthMap.h"   // typedef


class TruthParticleContainer;
namespace Analysis {
class TauJetContainer;
}
//class JetCollection;
//class TruthParticleContainer;

namespace TauID {

static const InterfaceID IID_ITauJetTruthMatchTool("TauID::ITauJetTruthMatchTool", 1, 0);

/** @class ITauJetTruthMatchTool
    @brief Interface for matching tau jets to truth taus.

    @author Sebastian Fleischmann
*/
class ITauJetTruthMatchTool : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID( ) ;

    /** create a tau jet to truth map. The map belongs to
        you, the user, and so you must take care of deletion of it.*/
    virtual const TauID::TauJetTruthMap* matchCandidatesToTruth(
                                            const TruthParticleContainer& truthTauCont,
                                            const Analysis::TauJetContainer& tauCandColl ) const = 0;

};

inline const InterfaceID& TauID::ITauJetTruthMatchTool::interfaceID() {
    return IID_ITauJetTruthMatchTool;
}


}
#endif // TAUTOOLS_ITAUJETTRUTHMATCHTOOL_H
