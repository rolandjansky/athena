/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKGLOBALCHI2ALIGNTOOLS_GLOBALCHI2ALIGNTOOL_H
#define TRKGLOBALCHI2ALIGNTOOLS_GLOBALCHI2ALIGNTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IAlignTool.h"
#include "TrkAlignInterfaces/IMatrixTool.h"

//#include "TMatrixD.h"

#include <string>

/** @file GlobalChi2AlignTool.h
    @class GlobalChi2AlignTool

    @brief Based on SiGlobalChi2Algs and TRT_AlignAlgs.  
    AlgTool used to calculate and store first- and second-derivatives for 
    global chi2 alignment of any set of detector modules defined using 
    AlignModule.  Also provides methods for solving for the final alignment 
    parameters.
    
    @author Robert Harrington <roberth@bu.edu>
    @date 1/5/08
*/

class TFile;
class TTree;

namespace Trk {
  class Track;
  class IMatrixTool;
  class IAlignModuleTool;

  class GlobalChi2AlignTool : virtual public IAlignTool, public AthAlgTool {    
  public:
    
    /** Constructor */
    GlobalChi2AlignTool(const std::string& type, const std::string& name,
	       const IInterface* parent);
    
    /** Virtual destructor */
    virtual ~GlobalChi2AlignTool();
    
    /** initialize */
    virtual StatusCode initialize();
    
    /** finalize */
    virtual StatusCode finalize();

    /** sets up matrix */
    virtual StatusCode firstEventInitialize();

    /** solves for alignment parameters */
    virtual StatusCode solve();

    /** accumulates information from an AlignTrack */
    bool accumulate(const AlignTrack* alignTrack);

    /** accumulates information from binary files */
    bool accumulateFromFiles();

    /** returns total number of accumulated tracks */
    unsigned int nAccumulatedTracks() { return m_ntracks; };

    /** set ntuple */
    void setNtuple(TFile* ntuple) { m_ntuple=ntuple; }

    /** sets the output stream for the logfile */
    void setLogStream(std::ostream * os);

    /** writes tree to ntuple */
    StatusCode fillNtuple();

  private:

    double getMaterialOnTrack(const Trk::Track* track);
  
    /** Pointer to MatrixTool, used to write to and solve matrix*/
    ToolHandle <Trk::IMatrixTool>      m_matrixTool;

    /** Pointer to AlignModuleTool*/
    ToolHandle <Trk::IAlignModuleTool> m_alignModuleTool;
    
    std::string m_pathbin;            //!< path binary files (in/out)
    std::string m_pathtxt;            //!< path ascii files (in/out)
    std::string m_prefixName;         //!< prefix string to filenames

    unsigned int m_ntracks; //!< number of accumulated tracks
    unsigned int m_nmeas;   //!< number of accumulated measurements
    unsigned int m_nhits;   //!< number of accumulated hits
    double m_chi2;          //!< total chi2
    unsigned int m_nDoF;    //!< number of degrees of freedom

    double m_secondDerivativeCut;
    bool   m_doTree;
    bool   m_writeActualSecDeriv;
    bool   m_storeLocalDerivOnly;

    /** output ntuple */
    TFile*   m_ntuple;
    TTree*   m_tree;   
    int      m_run;
    int      m_event;
    double   m_materialOnTrack;
    double   m_momentum;
    int      m_nChambers;
    int*     m_chamberIds;
    int      m_nMatrixIndices;
    int*     m_matrixIndices;
    int      m_nSecndDeriv;
    double*  m_secndDeriv;
    double*  m_firstDeriv;
    double*  m_actualSecndDeriv;
    double   m_eta;
    double   m_phi;
    double   m_perigee_x;
    double   m_perigee_y;
    double   m_perigee_z;
    int      m_trackInfo;
    int      m_bremFit;             
    int      m_bremFitSuccessful; 
    int      m_straightTrack;       
    int      m_slimmedTrack;            
    int      m_hardScatterOrKink; 
    int      m_lowPtTrack;      
              
    bool     m_fromFiles;

  }; // end of class

} // End of namespace 

#endif // TRKALIGNGENTOOLS_MATRIXTOOL_H



