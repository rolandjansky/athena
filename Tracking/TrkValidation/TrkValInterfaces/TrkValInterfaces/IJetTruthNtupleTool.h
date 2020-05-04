/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IJetTruthNtupleTool.h
//   Header file for interface of TruthNtupleTools
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_IJETTRUTHNTUPLETOOL_H
#define TRK_IJETTRUTHNTUPLETOOL_H

#include "GaudiKernel/IAlgTool.h"
//#include "TrkValEvent/GenParticleJet.h"

#include <vector>

#include "AtlasHepMC/GenParticle_fwd.h"
namespace Trk {
    class GenParticleJet;

static const InterfaceID IID_IJetTruthNtupleTool("IJetTruthNtupleTool",1,0);

/** @class IJetTruthNtupleTool
    provides the interface for validation tools which write special information
    about truth particles into ntuples.

    @author Loek Hooft van Huysduynen, Wolfgang Liebig
*/

class IJetTruthNtupleTool : virtual public IAlgTool {
public:
     /**Interface ID, declared here, and defined below*/
    static const InterfaceID& interfaceID();

    /** fill ntuple data for a given vector of truth jets */
    virtual StatusCode writeJetTruthData (const std::vector< Trk::GenParticleJet >&,
                                          const int& ) const = 0;

    virtual unsigned int getNumberOfTreeRecords() const = 0;
};

inline const InterfaceID& Trk::IJetTruthNtupleTool::interfaceID() {
    return IID_IJetTruthNtupleTool;
}

} // end of namespace

#endif // TRK_IJETTRUTHNTUPLETOOL_H
