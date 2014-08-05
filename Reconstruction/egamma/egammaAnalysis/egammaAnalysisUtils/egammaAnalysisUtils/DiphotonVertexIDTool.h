/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DiphotonVertexIDTool_h
#define DiphotonVertexIDTool_h

#include <vector>
#include <TString.h>

namespace TMVA { class Reader; }

using namespace std;

class DiphotonVertexIDTool 
{
  public :
    DiphotonVertexIDTool(
      bool debug=false,
      TString="$ROOTCOREBIN/data/egammaAnalysisUtils/DiphotonVertex_MLP.weights.xml",
      TString="$ROOTCOREBIN/data/egammaAnalysisUtils/DiphotonVertex_MLP2.weights.xml",
      unsigned int maxVertices = 3,
      float classifier_cut_1 = 0.48,
      float classifier_cut_2 = 0.36,
      float cut_pt_track = 2e3,
      float cut_deltaphi_track_cluster = 0.04
    );
    
    virtual ~DiphotonVertexIDTool();

    vector<int> get_vertex( 
      unsigned int indexPhoton1,
      unsigned int indexPhoton2,
      float beamSpot_z,
      float beamSpot_sigma_z,
      vector<float> *PV_px,
      vector<float> *PV_py,
      vector<float> *PV_z,
      vector<float> *PV_sumPt,
      vector<float> *Photon_PV_sumpt2,
      vector<vector<int> > *PV_trk_index,
      vector<float> *ph_cl_E,
      vector<float> *ph_etas2,
      vector<float> *ph_cl_phi,
      vector<float> *ph_CaloPointing_zvertex,
      vector<float> *ph_CaloPointing_sigma_zvertex,
      vector<float> *ph_HPV_zvertex =0,
      vector<float> *ph_HPV_sigma_zvertex =0,      
      vector<int> *ph_convtrk1nPixHits =0,
      vector<int> *ph_convtrk1nSCTHits =0,
      vector<int> *ph_convtrk2nPixHits =0,
      vector<int> *ph_convtrk2nSCTHits =0,
      vector<float> *ph_pt1conv =0,
      vector<float> *ph_pt2conv =0,
      vector<vector<vector<int> > > *ph_vx_convTrk_index =0,
      vector<vector<vector<float> > > *ph_vx_paramsAtConvVtx_phi =0,
      vector<int> *GSF_trk_trk_index =0
    );

    /* Last case treated */
    int getCase() { return m_case; }
    
    float getZcommon() { return m_zCommon; }
    float getZerror() { return m_zError; }
    
    vector <int> get_ranking() {return mlp_ranking; }

    vector <float> get_mlp() { return mlp_values; }
    
    /** Return true if the track is a good conversion candidate, based on minPt
      *  and deltaPhi between track and cluster
      **/  
    bool acceptSiTrack(int iPh,
                       int iGSF,
                       vector<float> *ph_cl_phi,
                       vector<float> *GSF_trk_pt,
                       vector<float> *GSF_trk_phi
    );
    
    /** Return true if the track is a good conversion candidate, based on minPt
      *  and deltaPhi between track and cluster
      **/
    bool acceptSiTrack(float clusterPhi, float trkPt, float trkPhi);

    static float deltaPhi(float phi1, float phi2);
        
    /** Return a vector with the indices of Si-tracks from a photon conversion **/
    static vector<int> isSiConversion(unsigned int iPh,
                                      vector<int> *ph_convtrk1nPixHits,
                                      vector<int> *ph_convtrk2nPixHits,
                                      vector<int> *ph_convtrk1nSCTHits,
                                      vector<int> *ph_convtrk2nSCTHits);
    
    /** Return the index of the track in the GSF block given the index of the
      * photon and the index of the conversion track (0 or 1)
      **/
    static int getGSFTrkIndex(int iPh,
                              int iPhTrk,
                              vector<vector<vector<int> > > *ph_vx_convTrk_index
    );

    /** Return the index of the vertex to which the GSF track belongs (or -1) **/
    static int getVertexFromTrack(int iGSF,
                                  vector<int> *GSF_trk_trk_index,
                                  vector<vector<int> > *PV_trk_index);
    
    /** Return phi of the diphoton system **/
    static float getDiphotonPhi(vector<int> *indice_photon,
                                vector<float> *ph_cl_E,
                                vector<float> *ph_etas2,
                                vector<float> *ph_cl_phi);
    
    static float square(float x) { return x*x; }

    
  private:
    // Parameters from c-tor: maximum number of vertices, MVA thresholds
    // and cuts to reject Si conversions
    bool m_debug;
    unsigned int m_maxVertices;
    float m_classifierCut1;
    float m_classifierCut2;
    float m_convPtCut;
    float m_deltaphiCut;
    
    // Case treated
    int m_case;
    
    // zCommon and error
    float m_zCommon, m_zError;
    
    // Converted photons
    vector<int> m_convertedPhotons;
    
    // MVA inputs
    float log_PV_pt_scal;
    float log_PV_pt_scal2;
    float PV_deltaphi;
    float PV_deltaz;
    
    // MVA objects and outputs
    TMVA::Reader *m_reader1;
    TMVA::Reader *m_reader2;
    std::vector<float> mlp_values;
    std::vector<int> mlp_ranking;

    /** Determine the case to treat:
      * Case 0 = at least one photon with a track associated to a PV (return -vertex)
      * Case 1 = at least one converted photon with Si track(s) but not in case 0
      * Case 2 = all the others (unconverted photons or TRT conversions)
      **/
    int get_case(vector <int> *photonIndices,
      vector<vector<vector<int> > > *ph_vx_convTrk_index,
      vector<vector<vector<float> > > *ph_vx_paramsAtConvVtx_phi,
      vector<float> *ph_pt1conv,
      vector<float> *ph_pt2conv,
      vector<float> *ph_cl_phi,
      vector<int> *GSF_trk_trk_index,
      vector<vector<int> > *PV_trk_index,
      vector <int> *ph_convtrk1nPixHits,
      vector <int> *ph_convtrk2nPixHits,
      vector <int> *ph_convtrk1nSCTHits,
      vector <int> *ph_convtrk2nSCTHits
    );
    
    /** Return true if photon is a conversion with Si tracks (after selection) **/
    bool isConvertedPhoton(int photon);
    
};
      
#endif 
