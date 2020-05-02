/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITruthNtupleTool.h
//   Header file for interface of TruthNtupleTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann -at- cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRUTHNTUPLETOOL_H
#define TRK_ITRUTHNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

#include "TrkParameters/TrackParameters.h"
//#include "TrkValInterfaces/ITrackTruthClassifier.h"

#include "AtlasHepMC/GenParticle_fwd.h"
namespace Trk {
    class ITrackTruthClassifier;

static const InterfaceID IID_ITruthNtupleTool("ITruthNtupleTool",1,0);

/** @class ITruthNtupleTool
    provides the interface for validation tools which write special information
    about truth particles into ntuples.

    @author Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
*/

class ValidationTrackTruthData {
public:
    const HepMC::GenParticle*                   genParticle;
    const Trk::TrackParameters*                 truthPerigee;
    std::vector< std::vector<unsigned int> >    truthToTrackIndices;
    std::vector< std::vector<float> >           truthToTrackMatchingProbabilities;
    unsigned int                                truthToJetIndex;
    std::vector<unsigned int>                   classifications;

    ValidationTrackTruthData() :
        genParticle(0),
        truthPerigee(0),
        truthToTrackIndices(0),
        truthToTrackMatchingProbabilities(0),
        truthToJetIndex(0),
        classifications(0)
        { }
};

class ITruthNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** fill ntuple data for a given vector of truth particle */
    virtual StatusCode writeTruthData (
                const std::vector< Trk::ValidationTrackTruthData >& ) const = 0;

    virtual unsigned int getNumberOfTreeRecords() const = 0;

    virtual StatusCode initBranches(const std::vector<const Trk::ITrackTruthClassifier*>& classifiers,
                                    bool,
                                    const std::vector<std::string> trackCollectionNames) const = 0;
};

inline const InterfaceID& Trk::ITruthNtupleTool::interfaceID() {
    return IID_ITruthNtupleTool;
}

} // end of namespace

#endif // TRK_ITRUTHNTUPLETOOL_H
