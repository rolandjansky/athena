/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNINTERFACES_IMATRIXTOOL_IH
#define TRKALIGNINTERFACES_IMATRIXTOOL_IH

/**
   @file IMatrixTool.h
   @class IMatrixTool

   @brief Interface for tool used to manipulate matrix and vector entries and 
   to solve for alignment parameters.
   
   Based on InDetAlignAlgs/SiGlobalChi2AlignAlgs.
   
   @author Robert Harrington <roberth@bu.edu>
   @date 1/5/08
*/

#include "GaudiKernel/IAlgTool.h"

#include <fstream>

namespace Trk {

  class AlignModule;

  class AlVec;
  class AlSymMatBase;

  static const InterfaceID
    IID_TRKALIGNINTERFACES_IMatrixTool("IMatrixTool",1,0);
  
  class IMatrixTool : virtual public IAlgTool {
    
  public:

    /** constructor */
    IMatrixTool();

    /** Virtual destructor */
    virtual ~IMatrixTool() {}
    
    /** Retrieve interface ID */
    static const InterfaceID& interfaceID();

    /** allocates space in memory for the big matrix and vector */
    virtual StatusCode allocateMatrix(int nDoF) = 0; 
    
    /** reads/writes matrix entries from/to binary files as necessary*/
    virtual void prepareBinaryFiles(int solveOption) = 0;

    /** adds first derivative to vector */
    virtual void addFirstDerivatives(AlVec* vector) = 0;

    /** adds first derivative to vector for only some entries */
    virtual void addFirstDerivatives(std::list<int,double>& derivatives) = 0;

    virtual void addFirstDerivative(int irow, double val) = 0;

    /** adds second derivatives to matrix */
    virtual void addSecondDerivatives(AlSymMatBase* matrix) = 0;
    
    /** adds first derivative to vector for only some entries */
    virtual void addSecondDerivatives(std::list<std::pair<int,int>,double >& derivatives) = 0;
    virtual void addSecondDerivative(int irow, int icol, double val) = 0;

    /** accumulates first and second derivatives from  files.  File type to be specified by configuring the tool*/
    virtual bool accumulateFromFiles() = 0;

    /** solves for alignment parameters */
    virtual int solve() = 0;
    
    void addModule(int alignModuleIndex, int nAlignParam);
    int  entryNumber(int alignModuleIndex);

    /** set module identifier */
    //virtual void setAlignModule(int irow, const AlignModule* alignmodule,
    //			int ipar) = 0;

    /** set number of hits */
    void setNHits(int n) { m_nHits = n; }
    int nHits() const { return m_nHits; }

    /** set number of tracks */
    void setNTracks(int n) { m_nTracks = n; }
    int nTracks() const { return m_nTracks; }

    /** set number of measurements */
    void setNMeasurements(int n) { m_nMeasurements = n; }
    int nMeasurements() const { return m_nMeasurements; }

    /** sets the output stream for the logfile */
    virtual void setLogStream(std::ostream * os) { m_logStream = os; }

  protected:
    std::ostream * m_logStream; //!< logfile output stream

    int m_nHits;
    int m_nTracks;
    int m_nMeasurements;

  private:
    std::map<int,int> m_alignModuleMap;
    int m_nentries;

  }; // end of class
  
  inline const InterfaceID& IMatrixTool::interfaceID() {
    return IID_TRKALIGNINTERFACES_IMatrixTool;
  }

  inline IMatrixTool::IMatrixTool() : m_logStream(0), m_nHits(0), m_nTracks(0), m_nMeasurements(0), m_nentries(0) {}

  inline void IMatrixTool::addModule(int alignModuleIndex, int nAlignParam) { m_alignModuleMap[alignModuleIndex]=m_nentries; m_nentries += nAlignParam; }

  inline int IMatrixTool::entryNumber(int alignModuleIndex) { if ( m_alignModuleMap.find(alignModuleIndex) == m_alignModuleMap.end()) return -1; else return m_alignModuleMap[alignModuleIndex]; }

} // end of namespace
  
#endif // TRKALIGNINTERFACES_IMATRIXTOOL_IH
