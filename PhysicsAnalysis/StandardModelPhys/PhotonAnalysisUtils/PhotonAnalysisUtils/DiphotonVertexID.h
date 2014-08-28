/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DiphotonVertexID_h
#define DiphotonVertexID_h

#include "AthenaBaseComps/AthAlgTool.h"
#include "PhotonAnalysisUtils/IDiphotonVertexID.h"
#include "GaudiKernel/ToolHandle.h"
#include "TFile.h"
#include "TH1.h"

#include <vector>
#include <string>

class egamma;
class StoreGateSvc;
class IComputePointingQuantities;
class IBeamCondSvc;

namespace TMVA { class Reader; }
namespace Trk { class RecVertex; class VxTrackAtVertex; class VxCandidate; }
namespace Rec { class TrackParticle; }

class DiphotonVertexID : public AthAlgTool, virtual public IDiphotonVertexID
{
  public :
    DiphotonVertexID(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~DiphotonVertexID();
    
     StatusCode initialize();
     StatusCode finalize();

    std::vector< const Trk::VxCandidate* > getVertex(egamma *photon1, 
                                                      egamma *photon2, 
                                                      bool ignoreConv=false);

    /* Last case treated */
    int getCase() { return m_case; }
    
    float getZcommon() { return m_zCommon; }
    float getZerror() { return m_zError; }
    std::vector<float> getMLP() { return mlp_values; }
    
    static const Rec::TrackParticle* getTrackParticle( const Trk::VxTrackAtVertex* );
    static float getDiphotonPhi(egamma *photon1, egamma *photon2);

    
  private:
    
    ToolHandle<IComputePointingQuantities> m_PAUComputePointingQuantities;
    StoreGateSvc *m_storeGate;
    IBeamCondSvc* m_beamCondSvc;
    
    // Configurables: maximum number of vertices, MVA thresholds
    // and cuts to reject Si conversions
    std::string m_vertexContainer;
    std::string m_beamCondSvcName;

    bool m_fixedBeamSpread;
    float m_beamSpread;
    bool m_isData;
    
    unsigned int m_maxVertices;
    float m_classifierCut1;
    float m_classifierCut2;
    float m_convPtCut;
    float m_deltaphiCut;
    
    // Case treated
    int m_case;
    
    // zCommon and error
    float m_zCommon, m_zError;
    
    // MVA inputs
    float log_PV_pt_scal;
    float log_PV_pt_scal2;
    float PV_deltaphi;
    float PV_deltaz;
    
    // MVA objects and outputs
    TMVA::Reader *m_reader1;
    TMVA::Reader *m_reader2;
    std::vector<float> mlp_values;
    std::vector<const Trk::VxCandidate*> mlp_ranking;
    
    // List of converted photons
    std::vector< egamma* > m_convertedPhotons;
    
    /** Return the primary vertex associated to the photon if any **/
    const Trk::VxCandidate* getPhotonVertex(egamma *photon);
    
    /** Return true if photon is converted (with Si track and passing pt and deltaPhi cut) 
      * Must be called after getPhotonVertex **/
    bool isConvertedPhoton(egamma *photon);
    
    std::pair<float, float> getZandError(egamma *photon);
    
    void getZandError(egamma *photon1, egamma *photon2);
    
    float getMVAResponse(float diphotonPhi, const Trk::VxCandidate*, TMVA::Reader*);

    TH1F* h_zCorr;
};
      
#endif 
