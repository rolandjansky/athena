/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ITrackTruthClassifier.h
//   Header file for interface for TruthClassificationTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_ITRACKTRUTHCLASSIFIER_H
#define TRK_ITRACKTRUTHCLASSIFIER_H

#include <vector>
#include <string>
#include "GaudiKernel/IAlgTool.h"

class McEventCollection;
#include "AtlasHepMC/GenParticle_fwd.h"

namespace Trk {
  static const InterfaceID IID_ITrackTruthClassifier("ITrackTruthClassifier",1,0);

/** @class ITrackTruthClassifier
    @brief provides the interface for tools which classify gen particles
*/

class ITrackTruthClassifier : virtual public IAlgTool {
  public:
    /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();
    
    /** explain */
    virtual void initClassification(const McEventCollection&,
                                    const std::vector<const HepMC::GenParticle *>*) const=0;

    virtual unsigned int classify(const HepMC::GenParticle&) const = 0;

    virtual std::string nameOfClassifier() const = 0;

    virtual std::string classificationAsString(const unsigned int) const = 0;

    virtual unsigned int numberOfClassifiers() const = 0;

  };

inline const InterfaceID& Trk::ITrackTruthClassifier::interfaceID() {
    return IID_ITrackTruthClassifier;
}

} // end namespace
#endif
