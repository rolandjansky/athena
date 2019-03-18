/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// JetTruthNtupleTool.h
//   Header file for JetTruthNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Wolfgang.Liebig@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_JETTRUTHNTUPLETOOL_H
#define TRK_JETTRUTHNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkValInterfaces/IJetTruthNtupleTool.h"
#include <vector>

class TTree;

namespace Trk {


/** @class JetTruthNtupleTool
    @brief Tool to fill basic information about truth jets, establishing
           track-to-jet association indices with the truth ntuple tree.

    @author Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
*/

class JetTruthNtupleTool : virtual public Trk::IJetTruthNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    JetTruthNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~JetTruthNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    /** fill ntuple data for a given vector of truth jets */
    virtual StatusCode writeJetTruthData (const std::vector< Trk::GenParticleJet >&,
                                          const int& ) const;

    virtual unsigned int getNumberOfTreeRecords() const;

private:

    // jobOptions
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file and dir name
    std::vector<double> m_etaBins;

    TTree* m_nt; //!< Pointer to the NTuple tree

    mutable unsigned int m_numberOfTreeEntries;

    // ntuple variables
    mutable int         m_runNumber;
    mutable int         m_eventNumber;

    // truth jet information
    std::vector<unsigned int>*   m_genParticleLinkIndex;
    std::vector<float>* m_mc_trackToJetAngle;
    mutable float       m_mc_jetMultiplicity;
    mutable float       m_mc_jetSphericity;
    mutable float       m_mc_jetThrust;
    mutable float       m_mc_jetEnergy;
    mutable float       m_mc_jetMomentum;

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};


} // end of namespace

#endif // TRK_TRUTHNTUPLETOOL_H

