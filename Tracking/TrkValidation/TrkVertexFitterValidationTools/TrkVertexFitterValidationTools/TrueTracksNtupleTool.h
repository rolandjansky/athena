/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// TrueTracksNtupleTool.h
//////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRUETRACKSNTUPLETOOL_H
#define TRK_TRUETRACKSNTUPLETOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterValidationInterfaces/ITrueTracksValidationNtupleTool.h"

class TTree;

namespace Trk {

/** @class TrueTracksNtupleTool
    This validation tool writes basic information about Trk::TrackCollection used by the vertex finding
    into an ntuple.
    @author Tatjana.Lenz@cern.ch
*/

class TrueTracksNtupleTool : virtual public Trk::ITrueTracksValidationNtupleTool, public AthAlgTool {
public:

    // standard AlgTool methods
    TrueTracksNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~TrueTracksNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** fill and write ntuple data of a given VxCandidate */
    virtual StatusCode fillTrueTracksInfo(const TrackCollection& trkcoll, const TrackTruthCollection& trk_true_coll) const;


private:

	      std::string	m_ntupleFileName;
	      std::string	m_ntupleDirName;     
	      std::string	m_ntupleTreeName;

	      TTree * tree;     
              
        //variables for track collection
        mutable std::vector<float> *m_prod_x; //!< x coordinate in mm of
        mutable std::vector<float> *m_prod_y; //!< y coordinate in mm of 
        mutable std::vector<float> *m_prod_z; //!< z coordinate in mm of 


        mutable std::vector<int> *m_particle_id;
        mutable std::vector<int> *m_parent_id;
  
        mutable int m_num_trks;

};


} // end of namespace

#endif // TRK_TRUETRACKSNTUPLETOOL_H
