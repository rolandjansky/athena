/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauClassificationUtility.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TAUCLASSIFICATIONUTILITY_H
#define PANTAUALGS_TAUCLASSIFICATIONUTILITY_H

#include "PanTauEvent/TauClassificationTypes.h"     // enum
//#include "GaudiKernel/MsgStream.h"

class MsgStream;

namespace TauID{
class TruthTau;
}

namespace Analysis{
class TauJet;
}

namespace PanTau {
class TauSeed;

/** @class TauClassificationUtility

    This utility classifies TruthTaus, TauSeeds and TauJets.

    @author  Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>
*/

class TauClassificationUtility {
public:

    /** Standard Constructor */
    TauClassificationUtility() {};
    /** Default Destructor */
    ~TauClassificationUtility() {};

    /** get truth tau type */
    TauClassificationTypes::TruthTauType    getTruthTauType(const TauID::TruthTau& truthTau, MsgStream& log ) const;
    /** get tau seed type */
    TauClassificationTypes::PanTauRecoMode  getSeedTauType(const PanTau::TauSeed& seed, MsgStream& log ) const;
    /** get tau seed type of TauJet*/
    TauClassificationTypes::PanTauRecoMode  getSeedTauType(const Analysis::TauJet& tauJet, MsgStream& log) const;
};
} // end of namespace

#endif //PANTAUALGS_TAUCLASSIFICATIONUTILITY_H


