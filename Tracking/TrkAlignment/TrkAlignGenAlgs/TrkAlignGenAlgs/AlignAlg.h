/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENALGS_ALIGNALG_H
#define TRKALIGNGENALGS_ALIGNALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrkAlignInterfaces/IAlignTrackCreator.h"
#include "TrkAlignInterfaces/IAlignTrackDresser.h"
#include "TrkAlignInterfaces/IAlignTool.h"

#include <string>
#include <fstream>

/**
   @file AlignAlg.h
   @class AlignAlg

   @brief This class is the main algorithm for the alignment with tracks. 
   The algorithm is used to align the modules of the Inner Detector, the 
   Muon Spectrometer, or both.

   @author roberth@bu.edu
   @author Daniel Kollar <daniel.kollar@cern.ch>
*/  

namespace Trk {
  
  class IGeometryManagerTool;
  class IFillNtupleTool;
  class ITrkAlignDBTool;
  class IAlignTrackPreProcessor;
  class ITrackCollectionProvider;

  class AlignAlg : public AthAlgorithm {
    
  public: 

    /** constructor */
    AlignAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /** destructor */
    virtual ~AlignAlg();
    
    /** initialize method */
    virtual StatusCode initialize();

    /** set up geometry and prepare the tools */
    virtual StatusCode start();

    /** loops over tracks in event, and accumulates information necessary for alignmnet */
    virtual StatusCode execute();

    /** processes information accumulated in execute method to determine alignment parameters */
    virtual StatusCode stop();

    /** finalize method */
    virtual StatusCode finalize();

    /** dumps statistics accumulated in each event */
    void showStatistics();
    
    /* 
    int nDoF() { return m_nDoF; }
   
    const AlignModuleList* moduleList() const { return m_modules; }

    bool doPixel() const { return m_doPixel; } 
    bool doSCT()   const { return m_doSCT; } 
    bool doTRT()   const { return m_doTRT; } 
    bool doMDT()   const { return m_doMDT; } 

    ToolHandle<IFillNtupleTool>* fillNtupleTool() { return &m_fillNtupleTool; }
    */
    
  private:
    
    ToolHandle <ITrackCollectionProvider> m_trackCollectionProvider;

    /** Pointer to AlignTrackPreProcessor, used to select hits on tracks and/or tracks before passing to AlignTrackCreator */
    ToolHandle<IAlignTrackPreProcessor> m_alignTrackPreProcessor;

    /** Pointer to alignTrackCreator, used to convert Trk::Track to vector of AlignTrack */
    ToolHandle <IAlignTrackCreator>  m_alignTrackCreator;
    
    /** Pointer to alignTrackDresser, used to add residuals, derivatives, etc. to vector of AlignTrack */
    ToolHandle <IAlignTrackDresser>  m_alignTrackDresser;
    
    /** Pointer to alignTool */
    ToolHandle <IAlignTool>  m_alignTool;
        
    /** Pointer to GeometryManagerTool, used to get lists of chambers for which alignment parameters will be determined */
    ToolHandle <IGeometryManagerTool> m_geometryManagerTool;
    
    /** Pointer to TrkAlignDBTool, used for reading/writing alignment parameters from/to the database */
    ToolHandle <ITrkAlignDBTool> m_trkAlignDBTool;
    
    /** Pointer to FillNtupleTool, used to write track information to ntuple */
    ToolHandle <IFillNtupleTool> m_fillNtupleTool;


    // various job options
    std::string m_filename;  //!< name of ntuple file
    std::string m_filepath;  //!< path to ntuple file

    bool m_solveOnly;           //!< only do the solving (accumulate from binaries)
    bool m_writeNtuple;         //!< write track and event information to ntuple

    int m_alignSolveLevel;      //!< Set the Alignment Solve Level

    int m_nDoF;     //!< Number of degrees of freedom = sum over chambers(DoF per chamber)

    TFile*         m_ntuple;        //!< output ntuple
    bool           m_writeLogfile;  //!< write a logfile for solving
    std::string    m_logfileName;   //!< name of the logfile
    std::ostream * m_logStream;     //!< logfile output stream

    int m_nevents;    //!< number of processed events
    int m_ntracks;    //!< number of processed tracks
    int m_ntracksSel; //!< number of selected tracks
    int m_ntracksProc;  //!< number of tracks successfully processed
    int m_ntracksDress; //!< number of tracks successfully dressed
    int m_ntracksAccum; //!< number of tracks successfully accumulated

    std::string m_alignTracksName; //!< name of the AlignTrack collection in the StoreGate

   };

} // end namespace

#endif // TRKALIGNGENALGS_ALIGNALG_H
