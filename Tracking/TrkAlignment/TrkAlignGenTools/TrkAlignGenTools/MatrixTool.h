/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_MATRIXTOOL_H
#define TRKALIGNGENTOOLS_MATRIXTOOL_H

/** @file MatrixTool.h
    @class MatrixTool

    @brief Based on SiGlobalChi2Algs.  
    AlgTool used to create a large matrix and vector for storing first- and 
    second-derivative terms needed to solve for alignment parameters.  Also 
    provides for methods to add terms to existing matrix entries, and to 
    solve for alignment parameters by inverting matrix and multiplying by 
    vector.
    
    @author Robert Harrington <roberth@bu.edu>, Daniel Kollar <daniel.kollar@cern.ch>
    @date 1/5/08
*/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IMatrixTool.h"

#include <TMatrixDSym.h>
#include "CLHEP/Matrix/SymMatrix.h"

#include <string>

class TString;

/** @class MatrixTool
    
    @brief AlgTool used to create a large matrix and vector for storing first- 
    and second-derivative terms needed to solve for alignment parameters.  
    Also provides for methods to add terms to existing matrix entries, and to 
    solve for alignment parameters by inverting matrix and multiplying by 
    vector.
    
    Uses the double inheritance structure; concrete tools should inherit from 
    this class and AlgTool with virtual public inheritance.   
*/

namespace Trk {

  class AlSymMatBase;
  class AlSymMat;
  class AlMat;
  class AlVec;

  class IAlignModuleTool;

  class MatrixTool : public AthAlgTool, virtual public IMatrixTool {    
  public:

    enum SolveOption { 
      NONE                 = 0, //!< not solve in any case (to be used when ipc)
      SOLVE                = 1, //!< solving after data accumulation (LAPACK)
      SOLVE_FAST           = 2, //!< Fast (Eigen method) solving after data accumulation
      DIRECT_SOLVE         = 3, //!< direct solving (LAPACK), already available matrix & vector
      DIRECT_SOLVE_FAST    = 4, //!< direct Fast (Eigen method) solving, already available matrix & vector
      DIRECT_SOLVE_CLUSTER = 5, //!< computation of alignment parameters from SCALAPAK already solved matrix
      SOLVE_ROOT           = 6, //!< computation using ROOT
      SOLVE_CLHEP          = 7  //!< computation using CLHEP
    };

    /** Constructor */
    MatrixTool(const std::string& type, const std::string& name,
	       const IInterface* parent);

    /** Virtual destructor */
    virtual ~MatrixTool();
    
    /** initialize */
    StatusCode initialize();
    
    /** initialize */
    StatusCode finalize();
    
    /** allocates memory for big matrix and big vector */
    StatusCode allocateMatrix(int nDoF=0); 
    
    /** reads/writes matrix entries from/to binary files as necessary*/
    void prepareBinaryFiles(int solveOption);

    /** adds first derivative to vector */
    void addFirstDerivatives(AlVec* vector);

    /** adds first derivative to vector for only some entries */
    void addFirstDerivatives(std::list<int,double>& derivatives);

    void addFirstDerivative(int irow, double firstderiv);  

    /** adds second derivatives to matrix */
    void addSecondDerivatives(AlSymMatBase* matrix);

    /** adds first derivative to vector for only some entries */
    void addSecondDerivatives(std::list<std::pair<int,int>,double >& derivatives);

    void addSecondDerivative(int irow, int icol, double secondderiv);


    /** accumulates derivates from files.  Flag decides if it is binary or TFiles */
    bool accumulateFromFiles();
    
    /** accumulates derivates from binary files */
    bool accumulateFromBinaries();

    /** solves for alignment parameters */
    int solve();

    
    /** Store Files in a tfile */
    void storeInTFile(TString filename);

    /** Store Files in a tfile */
    bool accumulateFromTFiles();

    void printModuleSolution(std::ostream & os, const AlignModule * module, const CLHEP::HepSymMatrix * cov) const;
    void printGlobalSolution(std::ostream & os, const CLHEP::HepSymMatrix * cov) const;
    void printGlobalSolution(std::ostream & os, const TMatrixDSym * cov) const;

  private:

    // private methods
    int solveROOT();
    int solveCLHEP();
    int solveLapack();
    int solveSparseEigen();
    int solveLocal();

    StatusCode spuriousRemoval();
    int        fillVecMods();
    void       postSolvingLapack(AlVec * dChi2, AlSymMat * d2Chi2, AlVec &w, AlMat &z, int size);

    void writeHitmap();
    void readHitmaps();

    // private variables
    ToolHandle<IAlignModuleTool> m_alignModuleTool;

    /** matrix to contain second derivative terms to be used for alignment */
    AlSymMatBase* m_bigmatrix;     
    
    /** vector to contain first derivative terms to be used for alignment */
    AlVec* m_bigvector; 
    
    /** flag to use AlSpaMat for the big matrix (default is AlSymMat) */
    bool m_useSparse;    

    bool m_diagonalize;           //!< run diagonalization instead of inversion
    double m_eigenvaluethreshold; //!< cut on the minimum eigenvalue

    int   m_nDoF;
    int   m_solveOption;     //!< solving option
    int   m_modcut;          //!< cut on the weak modes which number is <par_modcut
    int   m_minNumHits;      //!< cut on the minimum number of hits per module
    int   m_minNumTrks;      //!< cut on the minimum number of tracks per module
    float m_pullcut;         //!< pull cut for the automatic weak mode removal method
    float m_eigenvalueStep;  //!< eigenvalue step for the second pass in the automatic weak mode removal method
    float m_Align_db_step;   //!< corr in the diagonal basis step for the third pass in the auto weak mode removal method
    
    bool m_calDet;         //!< Compute bigmatrix's determinant ?
    bool m_wSqMatrix;      //!< write a triangular matrix by default (true: square format) ?
    bool m_writeMat;       //!< write big matrix and vector into files ?
    bool m_writeMatTxt;    //!< also write big matrix and vector into txt files ?
    bool m_writeEigenMat;    //!< write eigenvalues and eigenvectors into files ?
    bool m_writeEigenMatTxt; //!< also write eigenvalues and eigenvectors into txt files ?
    bool m_writeModuleNames; //!< write module name instead of Identifier to vector file

    bool m_writeHitmap;    //!< write hitmap into file
    bool m_writeHitmapTxt; //!< write hitmap into text file
    bool m_readHitmaps;    //!< accumulate hitymap from files

    bool m_writeTFile;    //!< write out files to a root file
    bool m_readTFiles;    //!< write out files to a root file

    bool m_runLocal;       //!< Run solving using Local method

    double m_scale;        //!< scale for big matrix and vector normalization
    bool m_scaleMatrix;    //!< scale matrix by number of hits before solving
    
    float m_matVersion;    //!< version of matrix format (based on m_stdUnits and m_angleUnit)
    float m_vecVersion;    //!< version of vector format (based on m_stdUnits and m_angleUnit)

    double  m_softEigenmodeCut; //!< add constant to diagonal to effectively cut on weak eigenmodes

    bool m_removeSpurious; //!< run spurious removal

    bool m_calculateFullCovariance; //?< calculate full covariance matrix for Lapack

    std::string m_pathbin;      //!< path binary files (in/out)
    std::string m_pathtxt;      //!< path ascii files (in/out)
    std::string m_prefixName;   //!< prefix string to filenames

    std::string m_tfileName;   //!< prefix string to filenames


    std::string m_scalaMatName;       //!< Scalapack matrix name
    std::string m_scalaVecName;       //!< Scalapack vector name



    std::vector<std::string> m_inputMatrixFiles; //!< input binary files containing matrix terms
    std::vector<std::string> m_inputVectorFiles; //!< input binary files containing vector terms

    std::vector<std::string> m_inputHitmapFiles; //!< input binary files containing the hitmaps

    std::vector<std::string> m_inputTFiles; //!< input binary files containing matrix terms

    std::vector<int> m_activeIndices; //!< vector of indices which pass the min-hits cut
    int m_aNDoF;                      //!< number of active DoF (size of m_activeIndices)

    int m_maxReadErrors;	//!< maximum number of reading TFile errors 
    bool m_AlignIBLbutNotPixel;
    bool m_AlignPixelbutNotIBL;


  }; // end of class
  
  

} // End of namespace 



#endif // TRKALIGNGENTOOLS_MATRIXTOOL_H



