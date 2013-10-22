/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENALGS_ALIGNSOLVE_H
#define TRKALIGNGENALGS_ALIGNSOLVE_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrkAlignInterfaces/IAlignTool.h"

#include <string>
#include <fstream>

/**
   @file AlignSolve.h
   @class AlignSolve

   @brief This class is used for alignment with tracks. 
   The algorithm is used to align the modules of the Inner Detector, the 
   Muon Spectrometer, or both.
   It takes as an input a list of files containing data from 
   the running of AlignAlg.  This algorithm reads in from the 
   input files and calls the solve method of the AlignTool.

   @author roberth@bu.edu  
   @date 4/3/09
*/  

class TFile;
namespace Trk {
  
  class IGeometryManagerTool;
  //class IFillNtupleTool;
  class ITrkAlignDBTool;

  class AlignSolve : public AthAlgorithm {
    
  public:

    /** constructor */
    AlignSolve(const std::string& name, ISvcLocator* pSvcLocator);

    /** destructor */
    virtual ~AlignSolve();
    
    /** initialize method*/
    virtual StatusCode  initialize();

    /** beginOfRun method, sets up geometry*/
    virtual StatusCode  beginRun();

    /** execute method, sets up geometry in first event, calls accumulateFromFiles method of AlignTool */
    virtual StatusCode  execute();

    /** endOfRun method, processes information to determine alignment parameters*/
    virtual StatusCode  endRun();

    /** finalize method, processes information accumulated in execute method to determine alignment parameters*/
    virtual StatusCode  finalize();

  private:

    /** Pointer to alignTool */
    ToolHandle <IAlignTool>  m_alignTool;

    /** Pointer to GeometryManagerTool, used to get lists of chambers for which alignment parameters will be determined */
    ToolHandle <IGeometryManagerTool> m_geometryManagerTool;

    /** Pointer to TrkAlignDBTool, used for reading/writing alignment parameters from/to the database */
    ToolHandle <ITrkAlignDBTool> m_trkAlignDBTool;

    /** Pointer to FillNtupleTool, used to write track information to ntuple */
    //ToolHandle <IFillNtupleTool> m_fillNtupleTool;

    int    m_alignSolveLevel;      //!< Set the Alignment Solve Level
    bool   m_writeNtuple;          //!< write info to ntuple

    TFile * m_file;

    bool           m_writeLogfile;  //!< write a logfile for solving
    std::string    m_logfileName;   //!< name of the logfile
    std::ostream * m_logStream;     //!< logfile output stream

  };
  
} // end namespace
#endif // TRKALIGNGENALGS_ALIGNALG_H
