/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////
// McEventNtupleTool.h
//////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TRK_MCEVENTNTUPLETOOL_H
#define TRK_MCEVENTNTUPLETOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterValidationInterfaces/IMcEventNtupleTool.h"

class TTree;

namespace Trk {


/** @class McEventNtupleTool
    This validation tool writes basic information about monte carlo event
    into an ntuple.
    @author Tatjana.Lenz@cern.ch
*/

class McEventNtupleTool : virtual public Trk::IMcEventNtupleTool, public AthAlgTool {
public:

    // standard AlgToolmethods
    McEventNtupleTool(const std::string&,const std::string&,const IInterface*);
    ~McEventNtupleTool();

    // standard Athena methods
    StatusCode initialize();
    StatusCode finalize();


    /** fill and write ntuple data of a given VxCandidate */
    virtual StatusCode fillMcEventData (const HepMC::GenEvent&) const;
    
private:
	     
      std::string m_ntupleFileName;
      std::string m_ntupleDirName; 
      std::string m_ntupleMcTreeName;

      double m_ptCut;
      double m_etaCut;
      double m_radiusCut;
      double m_zPosCut;
      double m_radiusRes;
      double m_zPosRes;

      TTree * vtx_tree;
      TTree * quark_tree;
	      
        // vertex
      mutable int  m_numTrueVertices; //!< number of the gen vertices 
      mutable float m_true_pri_x; //!< x coordinate in mm of the gen vertex
      mutable float m_true_pri_y; //!< y coordinate in mm of the gen vertex
      mutable float m_true_pri_z; //!< z coordinate in mm of the gen vertex

      mutable std::vector<float> *m_true_sec_x; //!< x coordinate in mm of the gen vertex
      mutable std::vector<float> *m_true_sec_y; //!< y coordinate in mm of the gen vertex
      mutable std::vector<float> *m_true_sec_z; //!< z coordinate in mm of the gen vertex
};


} // end of namespace

#endif // TRK_MCEVENTNTUPLETOOL_H
