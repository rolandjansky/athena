/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TruthNtupleTool.h
//   Header file for TruthNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRUTHNTUPLETOOL_H
#define TRK_TRUTHNTUPLETOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkValInterfaces/ITruthNtupleTool.h"
#include "TrkValInterfaces/ITrackTruthClassifier.h"
#include <vector>


class TH1D;
class TTree;

namespace Trk {


/** @class TruthNtupleTool
    @brief  Tool to fill basic information about the truth record.

        It is called directly by the ntuple writer but provides information also
        for particles which have failed to be reconstructed. Furthermore it manages
        indices to link to ntuple entries of the associated reconstructed particles.

    @author Sebastian.Fleischmann -at- cern.ch, Wolfgang.Liebig -at- cern.ch
*/

class TruthNtupleTool : virtual public Trk::ITruthNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    TruthNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~TruthNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();

    virtual StatusCode writeTruthData ( const std::vector< Trk::ValidationTrackTruthData >& truthData ) const;

    virtual unsigned int getNumberOfTreeRecords() const;

    virtual StatusCode initBranches(const std::vector<const Trk::ITrackTruthClassifier*>& classifiers,
                                    bool,
                                    const std::vector<std::string> trackCollectionNames) const;

private:

    // jobOptions
    std::string m_ntupleTreeName;       //!< jobOption: Ntuple tree name
    std::string m_ntupleFileName;       //!< jobOption: Ntuple file and dir name
    std::vector<double> m_etaBins;
    mutable bool m_fillJets;            //!< jO: jet filling, set from external call

    //NTuple::Tuple* p_ntuple;          //!< Pointer to the ntuple
    TTree* m_nt; //!< Pointer to the NTuple tree

    mutable unsigned int m_numberOfTreeEntries;

    // ntuple variables
    mutable int         m_runNumber;       //!< run number to which this MC truth particle belongs
    mutable int         m_eventNumber;     //!< event number to which this MC truth particle belongs

    mutable std::vector< std::vector<unsigned int>* >   m_TrackLinkIndex;
    mutable std::vector< std::vector<float>* >          m_mc_prob;
    mutable std::vector<unsigned int>                   m_classifications;
    // Truth information
    mutable float       m_mc_d0;            //!< d0 of MC truth particle's perigee parameters
    mutable float       m_mc_z0;            //!< z of MC truth particle's perigee parameters
    mutable float       m_mc_phi0;          //!< phi of MC truth particle's perigee parameters
    mutable float       m_mc_theta;         //!< theta of MC truth particle's perigee parameters
    mutable float       m_mc_qOverP;        //!< q/p of MC truth particle's perigee parameters
    mutable float       m_mc_qOverPt;       //!< q/pT of MC truth particle's perigee parameters
    mutable float       m_mc_eta;           //!< eta of MC truth particle's perigee parameters

    mutable int         m_mc_particleID;    //!< PDG ID of MC truth particle
    mutable int         m_mc_barcode;       //!< MC truth particle's barcode
    mutable float       m_mc_energy;        //!< MC truth particle's energy at production vertex
    mutable int         m_mc_jetLinkIndex;  //!< link to jet this particle belongs to (if jet tree is ON)
    mutable float       m_mc_prodR; //!< Rxy of particle's production vertex
    mutable float       m_mc_prodz; //!< z coordinate of particle's production vertex

    // statistics
    mutable std::vector< std::vector<TH1D*> >   m_recoTrackCounts;
    mutable std::vector< std::vector<TH1D*> >   m_truthTrackCounts;

    mutable std::vector<const Trk::ITrackTruthClassifier*> m_trackTruthClassifiers;     //!< the truth classifiers

    SG::ReadHandleKey<xAOD::EventInfo>    m_evt  {this, "EvtInfo", "EventInfo", "EventInfo name"};
};


} // end of namespace

#endif // TRK_TRUTHNTUPLETOOL_H

