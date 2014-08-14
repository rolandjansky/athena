/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCONVERSIONTAGGER_H
#define	TAUREC_TAUCONVERSIONTAGGER_H

#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"

/**
 * @brief This tool identifies if a tau track originates from a photon conversion track.
 * 
 * @author D. Varouchas
 */


class TauConversionTagger : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauConversionTagger(const std::string& type,
            const std::string& name,
            const IInterface* parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauConversionTagger();

    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();

private:
    
    std::string m_trackContainerName;

    int m_ConvTaggerVer; 
    bool m_TrkIsConv;
    bool m_storeFullSummary;
    bool m_doTRTRatio;
    float m_a_cut[2][2], m_b_cut[2][2];	    
    float m_TRTHighTOutliersRatio;
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

};

#endif
