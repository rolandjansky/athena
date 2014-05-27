/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_IPCMATRIXTOOL_H
#define TRKALIGNGENTOOLS_IPCMATRIXTOOL_H

/** @file IPCMatrixTool.h
    @class IPCMatrixTool

    @brief Based on SiGlobalChi2Algs.  
    AlgTool used with 64-big parallel processing to create IPC matrix and 
    vector for storing first- and second-derivative terms needed to solve 
    for alignment parameters.  Also provides for methods to add terms to 
    existing matrix entries, and to solve for alignment parameters by 
    inverting matrix and multiplying by vector. 

    @author Robert Harrington <roberth@bu.edu>
    @date 1/5/08
*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/IMatrixTool.h"

namespace Trk {

  class AlVec;
  class AlSymMatBase;
  class IPCMat;

  class IPCMatrixTool : public AthAlgTool, virtual public IMatrixTool {      
  public:

    /** Constructor */
    IPCMatrixTool(const std::string& type, const std::string& name,
		  const IInterface* parent);
    
    /** Virtual destructor */
    virtual ~IPCMatrixTool();
    
    /** initialize */
    StatusCode initialize();
    
    /** initialize */
    StatusCode finalize();
    
    /** allocates memory for IPC matrix and vector */
    StatusCode allocateMatrix(int nDoF); 
    
    /** reads/writes matrix entries from/to binary files as necessary*/
    void prepareBinaryFiles(int solveOption);

    /** adds first derivative to vector */
    void addFirstDerivatives(AlVec* vector);

    /** adds first derivative to vector for only some entries */
    void addFirstDerivatives(std::list<int,double>& derivatives);

    /** adds second derivatives to matrix */
    void addSecondDerivatives(AlSymMatBase* matrix);

    /** adds first derivative to vector for only some entries */
    void addSecondDerivatives(std::list<std::pair<int,int>,double >& derivatives);

    /** accumulates from  files */
    bool accumulateFromFiles();

    /** solves for alignment parameters */
    int solve();

    void addFirstDerivative(int irow, double firstderiv);    
    void addSecondDerivative(int irow, int icol, double secondderiv);

    //void setAlignModule(int, const AlignModule*, int) {}

  private:
    
    /** Pointer to IPCMat, used for parallel processing */
    IPCMat* m_ipcmat; 
    
    std::string m_ipcmatMatName;      //!< IPC binary matrix filename
    std::string m_ipcmatVecName;      //!< IPC binary vector filename

    /** remove spurious + align. pars from IPC matrix before writing */
    bool m_IPCremoval;

  }; // end of class

  inline void IPCMatrixTool::addFirstDerivative(int, double) { /*m_bigvector[i]+=firstderiv;*/ }
  
  inline void IPCMatrixTool::addSecondDerivative(int, int, double) { /*m_bigmatrix[irow][icol]+=secondderiv;*/ }


} // end of namespace

#endif // TRKALIGNGENTOOLS_IPCMATRIXTOOL_H





