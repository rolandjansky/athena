/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// BasicVtxValidationNtupleTool.h
//////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_BASICVTXVALIDATIONNTUPLETOOL_H
#define TRK_BASICVTXVALIDATIONNTUPLETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterValidationInterfaces/IVtxValidationNtupleTool.h"

class TTree;
namespace Trk {


/** @class BasicVtxValidationNtupleTool
    This validation tool writes basic information about Trk::VxCandidate
    into an ntuple.
    @author Tatjana.Lenz@cern.ch
*/

class BasicVtxValidationNtupleTool : virtual public Trk::IVtxValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    BasicVtxValidationNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~BasicVtxValidationNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** fill and write ntuple data of a given VxCandidate */
    virtual StatusCode fillVxCandidateData (const Trk::VxCandidate&) const;
    virtual StatusCode fillEventInfo(int &) const;
    virtual StatusCode fillTrueTrackAtVertexInfo(const Trk::VxCandidate&, const TrackCollection&, const TrackTruthCollection&) const;
    
private:

      std::string         m_ntupleFileName;
      std::string         m_ntupleDirName;            
      std::string         m_ntupleTreeName;
      std::string         m_ntupleVtxTreeName;
      std::string         m_ntupleTrkAtVxTreeName; 


      TTree * tree;
      TTree * vtx_tree;
      TTree * trk_at_vxtree;

      mutable unsigned int m_lastEventNumber;

      mutable int m_runNumber;
      mutable int m_eventNumber;

      // vertex
      mutable int  m_numVertices; //!< number of the fitted vertices 
      mutable float m_x; //!< x coordinate in mm of the fitted vertex
      mutable float m_y; //!< y coordinate in mm of the fitted vertex
      mutable float m_z; //!< z coordinate in mm of the fitted vertex

      mutable float m_err_x;  //!< x coordinate error
      mutable float m_err_y; //!< y coordinate error
      mutable float m_err_z; //!< z coordinate error

      mutable float m_chi2;  //!< chi2 is the sum over all chi2_per_track per ndf
      mutable float m_chi2prob; //!< chi2 propobility of chi2 above
      mutable int m_numTracksPerVertex; //!< number of the tracks per vertex

      //tracks  
      mutable int   m_numTracks; //!< total number of the tracks

      // perigee of original tracks with vertex point as origin
      mutable std::vector<float> *m_d0;
      mutable std::vector<float> *m_z0;
      mutable std::vector<float> *m_phi0;
      mutable std::vector<float> *m_theta;
      mutable std::vector<float> *m_qOverP;

      // perigee errors of original tracks with vertex point as origin
      mutable std::vector<float> *m_err_d0;
      mutable std::vector<float> *m_err_z0;
      mutable std::vector<float> *m_err_phi0;
      mutable std::vector<float> *m_err_theta;
      mutable std::vector<float> *m_err_qOverP;
      
      mutable std::vector<float> *m_chi2_per_track; //!< chi2 per track
      
      // initial track parameters
      mutable std::vector<float> *m_initial_d0;
      mutable std::vector<float> *m_initial_z0;
      mutable std::vector<float> *m_initial_phi0;
      mutable std::vector<float> *m_initial_theta;
      mutable std::vector<float> *m_initial_qOverP;
      
      mutable std::vector<float> *m_err_initial_d0;
      mutable std::vector<float> *m_err_initial_z0;
      mutable std::vector<float> *m_err_initial_phi0;
      mutable std::vector<float> *m_err_initial_theta;
      mutable std::vector<float> *m_err_initial_qOverP;

      //variables for vxTrackAtVertex collection
      mutable std::vector<float> *m_vxprod_x; //!< x coordinate in mm of
      mutable std::vector<float> *m_vxprod_y; //!< y coordinate in mm of 
      mutable std::vector<float> *m_vxprod_z; //!< z coordinate in mm of 


      mutable std::vector<int> *m_vxparticle_id;
      mutable std::vector<int> *m_vxparent_id;
      mutable int m_vxnum_trks;

};


} // end of namespace

#endif // TRK_BASICVTXVALIDATIONNTUPLETOOL_H
