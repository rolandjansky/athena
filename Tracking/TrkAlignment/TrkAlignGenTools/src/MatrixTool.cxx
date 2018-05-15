/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MatrixTool.cxx

// AlgTool for creating big matrix and vector, manipulation of entries, and
// solving for alignment parameters
// Robert Harrington, started 1/5/08 based on SiGlobalChi2Align

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/AlgTool.h"

#include "TrkAlgebraUtils/AlVec.h"
#include "TrkAlgebraUtils/AlSymMatBase.h"
#include "TrkAlgebraUtils/AlMat.h"
#include "TrkAlgebraUtils/AlSymMat.h"
#include "TrkAlgebraUtils/AlSpaMat.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignGenTools/MatrixTool.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

#include <TMatrixT.h>
#include <TMatrixDSym.h>
#include <TMatrixDSparse.h>
#include <TVectorD.h>
#include <TVectorT.h>
#include <TString.h>
#include <TFile.h>


#include <TDecompBK.h>

#include <cmath>
#include <ctime>
#include <algorithm>
#include <iterator>

//restore ostream 
#include <boost/io/ios_state.hpp>



#include <sys/resource.h>

namespace Trk {

  //_______________________________________________________________________
  MatrixTool::MatrixTool(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : IMatrixTool()
    , AthAlgTool(type,name,parent)
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_bigmatrix(0)
    , m_bigvector(0)
    , m_scale(-1.)
    , m_aNDoF(0)
  {
    declareInterface<IMatrixTool>(this);

    declareProperty("UseSparse",          m_useSparse       = false);
    declareProperty("SolveOption",        m_solveOption     = NONE);

    declareProperty("Diagonalize",          m_diagonalize     = true);
    declareProperty("EigenvalueThreshold",  m_eigenvaluethreshold   = 0.);

    declareProperty("ModCut",             m_modcut        = 0);
    declareProperty("PullCut",            m_pullcut       = 1.0);
    declareProperty("EigenvalueStep",     m_eigenvalueStep    = 1e3);
    declareProperty("AlignCorrDBStep",    m_Align_db_step     = 10);
    declareProperty("MinNumHitsPerModule",  m_minNumHits      = 0);
    declareProperty("MinNumTrksPerModule",  m_minNumTrks      = 0);
    declareProperty("RunLocalMethod",     m_runLocal      = true);

    declareProperty("MatrixDet",          m_calDet          = false);
    declareProperty("WriteSquareMatrix",  m_wSqMatrix       = false);
    declareProperty("WriteMat",           m_writeMat        = true);
    declareProperty("WriteMatTxt",        m_writeMatTxt       = true);
    declareProperty("WriteEigenMat",      m_writeEigenMat     = true);
    declareProperty("WriteEigenMatTxt",   m_writeEigenMatTxt    = true);
    declareProperty("WriteModuleNames",   m_writeModuleNames    = false);
    
    declareProperty("WriteTFile",     m_writeTFile      = false);
    // if True then files will be read from TFiles instead of Binary files
    declareProperty("ReadTFile",    m_readTFiles      = false);
      

    std::vector<std::string> defaultMatInput,defaultVecInput,defaultTFile;
    defaultMatInput.push_back("matrix.bin");
    defaultVecInput.push_back("vector.bin");
    defaultTFile.push_back("AlignmentTFile.root");

    declareProperty("InputMatrixFiles",   m_inputMatrixFiles    = defaultMatInput);
    declareProperty("InputVectorFiles",   m_inputVectorFiles    = defaultVecInput);
    declareProperty("InputTFiles",        m_inputTFiles     = defaultTFile);

    declareProperty("AlignModuleTool",    m_alignModuleTool);

    declareProperty("PathBinName",        m_pathbin               = "./");
    declareProperty("PathTxtName",        m_pathtxt               = "./");
    declareProperty("PrefixName",         m_prefixName            = "");
    
    declareProperty("TFileName",          m_tfileName       = "AlignmentTFile.root");

    declareProperty("SoftEigenmodeCut",   m_softEigenmodeCut    = 0.);
    declareProperty("RemoveSpurious",     m_removeSpurious    = false);
    declareProperty("CalculateFullCovariance",  m_calculateFullCovariance = true);

    // ScaLapack
    declareProperty("ScalapackMatrixName",  m_scalaMatName      = "eigenvectors.bin");
    declareProperty("ScalapackVectorName",  m_scalaVecName      = "eigenvalues.bin");

    declareProperty("ScaleMatrix",        m_scaleMatrix       = false);

    // Hitmap
    declareProperty("WriteHitmap",        m_writeHitmap       = false);
    declareProperty("WriteHitmapTxt",     m_writeHitmapTxt    = false);
    declareProperty("ReadHitmaps",        m_readHitmaps       = false);
    std::vector<std::string> defaultHitmapInput;
    defaultMatInput.push_back("hitmap.bin");
    declareProperty("InputHitmapFiles",   m_inputHitmapFiles    = defaultHitmapInput);
  
    declareProperty("MaxReadErrors",    m_maxReadErrors     = 10);
    declareProperty("AlignIBLbutNotPixel",    m_AlignIBLbutNotPixel = false);
    declareProperty("AlignPixelbutNotIBL",    m_AlignPixelbutNotIBL = false);

  }

  //_______________________________________________________________________
  MatrixTool::~MatrixTool()
  {
    delete m_bigmatrix;
    delete m_bigvector;
  }

  //_______________________________________________________________________
  StatusCode MatrixTool::initialize()
  {
    ATH_MSG_DEBUG("initialize() of MatrixTool");

    // get AlignModuleTool
    if (m_alignModuleTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved " << m_alignModuleTool);
    else{
      msg(MSG::FATAL) << "Could not get " << m_alignModuleTool << endreq;
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Retrieving data from the following files: ");
    for (int i=0;i<(int)m_inputVectorFiles.size();i++) {
      ATH_MSG_INFO(m_pathbin+m_inputVectorFiles[i]);
    }    

    return StatusCode::SUCCESS;
  }

  //_______________________________________________________________________
  StatusCode MatrixTool::finalize()
  {
    ATH_MSG_DEBUG("finalize() of MatrixTool");

    return StatusCode::SUCCESS;
  }


  //_______________________________________________________________________
  StatusCode MatrixTool::allocateMatrix(int nDoF)
  {
    ATH_MSG_INFO("allocating matrix and vector with nDoF = "<<nDoF);

    if (0!=m_bigmatrix || 0!=m_bigvector)
      ATH_MSG_ERROR("big matrix already allocated!");

    // Decide upon the big matrix representation:
    if( m_useSparse )
      m_bigmatrix = new AlSpaMat(nDoF);
    else
      m_bigmatrix = new AlSymMat(nDoF);

    m_bigvector       = new AlVec(nDoF);

    ATH_MSG_INFO(" After Matrix and Vector allocation");

    // set paths for matrix and vector output
    m_bigmatrix->SetPathBin(m_pathbin+m_prefixName);
    m_bigmatrix->SetPathTxt(m_pathtxt+m_prefixName);
    m_bigvector->SetPathBin(m_pathbin+m_prefixName);
    m_bigvector->SetPathTxt(m_pathtxt+m_prefixName);

    ATH_MSG_INFO("set path to "<<m_pathbin+m_prefixName);
    return StatusCode::SUCCESS;
  }

  //_______________________________________________________________________
  void MatrixTool::prepareBinaryFiles(int)
  {
    return;
  }

  //_______________________________________________________________________
  int MatrixTool::solveROOT()
  {
    ATH_MSG_INFO("solving Global using ROOT");
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"**************  solving using Global method  ****************"<<std::endl;
      *m_logStream<<"**************          using ROOT           ****************"<<std::endl;
      *m_logStream<<"*************************************************************"<<std::endl;
    }

    // start measuring time
    clock_t starttime = clock();

    DataVector<AlignPar>*  alignParList = m_alignModuleTool->alignParList1D();
    //const AlignModuleList* alignModules = m_alignModuleTool->alignModules1D();

    int nDoF=m_alignModuleTool->nAlignParameters();

    // some debugging output
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE)<<"dumping matrix and vector to screen"<<endreq;
      for (int i=0;i<nDoF;i++)
        for (int j=0;j<nDoF;j++)
        //if (std::fabs((*m_bigmatrix)[i][j])>.0001)
          msg(MSG::VERBOSE)<<i<<", "<<j<<" : "<<(*m_bigmatrix)[i][j] <<endreq;

      for (int i=0;i<nDoF;i++)
        msg(MSG::VERBOSE)<<i <<" : "<<(*m_bigvector)[i]<<endreq;
    }

    // get rescaled first and second derivatives
    double * secderiv   = new double[m_aNDoF*m_aNDoF];
    double * firstderiv = new double[m_aNDoF];
    for (int iActive=0;iActive<m_aNDoF;iActive++) {
      int i = m_activeIndices[iActive];
      firstderiv[iActive] = (*m_bigvector)[i];
      for (int jActive=0;jActive<m_aNDoF;jActive++) {
        int j = m_activeIndices[jActive];
        secderiv[iActive*m_aNDoF+jActive] = (*m_bigmatrix)[i][j];
      }
    }

    // attention, the dimension of matrix a and b is m_aNDoF not nDoF,
    // this means some alignment parameters have not been calculated
    // if the corresponding modules did not satify the select cut

    TMatrixDSym a(m_aNDoF,secderiv);
    TVectorD    b(m_aNDoF,firstderiv);

    if(msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG)<<"First derivatives:"<<endreq;
      b.Print();
      msg(MSG::DEBUG)<<"Second derivatives:"<<endreq;
      a.Print();
    }

    TDecompBK c(a);
    Bool_t status;
    TMatrixDSym ainv(c.Invert(status));

    TVectorD r(b.GetNrows());
    if(status)
      r = c.Solve(b,status);

    // stop measuring time
    clock_t stoptime = clock();
    double totaltime = (stoptime-starttime)/double(CLOCKS_PER_SEC);
    ATH_MSG_INFO("Time spent in solveROOT: "<<totaltime<<" s");

    if(!status) {
      msg(MSG::ERROR)<<"ROOT inversion failed"<<endreq;
      if(m_logStream) {
        *m_logStream<<"ROOT inversion failed"<<std::endl;
        *m_logStream<<std::endl;
      }
    }
    else {
      ATH_MSG_INFO("ROOT inversion ok");

      ATH_MSG_DEBUG("Alignment constants:");
      for (int iAdof=0;iAdof<m_aNDoF;iAdof++) {

        int idof = m_activeIndices[iAdof];
        AlignPar * alignPar=(*alignParList)[idof];

        double sigma = alignPar->sigma();
        double param = -r[iAdof] * sigma;
        double err = std::sqrt(2.*std::fabs(ainv(iAdof,iAdof))) * sigma;

        ATH_MSG_DEBUG(iAdof <<" : "<< param << " +/- "<< err);
        ATH_MSG_DEBUG("ainv("<<iAdof<<")="<<ainv(iAdof,iAdof)<<", sigma: "<<sigma);
        ATH_MSG_DEBUG("init par: "<<alignPar->initPar());
        alignPar->setPar(param,err);
        ATH_MSG_DEBUG("set param to "<<param<<" for alignPar "<<alignPar);
        ATH_MSG_DEBUG(*(*alignParList)[idof]);
      }

      if(m_logStream)
      {
        *m_logStream<<"ROOT inversion ok"<<std::endl;

        printGlobalSolution(*m_logStream,&ainv);

        // norm of first derivative
        *m_logStream<<"norm of first derivative :            "<<sqrt(b.Norm2Sqr())<<std::endl;

        // distance to solution
        double dist = sqrt( ( b - (a * r) ).Norm2Sqr() );
        *m_logStream<<"distance to solution :                "<<dist<<std::endl;

        // calculate chi2 of the alignment change
        double chi2 = a.Similarity(r) * .5;
        *m_logStream<<"delta(chi2) of the alignment change : "<<chi2<<" / "<<m_aNDoF<<std::endl;

        // time spent here
        *m_logStream<<"time spent in solve :                 "<<totaltime<<" s"<<std::endl;
      }
    }

    delete [] secderiv;
    delete [] firstderiv;

    return 1;
  }

  //_______________________________________________________________________
  int MatrixTool::solveCLHEP()
  {
    ATH_MSG_INFO("solving Global using CLHEP");
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"**************  solving using Global method  ****************"<<std::endl;
      *m_logStream<<"**************          using CLHEP          ****************"<<std::endl;
      *m_logStream<<"*************************************************************"<<std::endl;
    }

    // start measuring time
    clock_t starttime = clock();

    DataVector<AlignPar> * alignParList = m_alignModuleTool->alignParList1D();
    //const AlignModuleList* alignModules = m_alignModuleTool->alignModules1D();
    for (int i=0;i<(int)alignParList->size();i++)
      ATH_MSG_DEBUG("ap["<<i<<"]="<<(*alignParList)[i]);

    int nDoF = m_alignModuleTool->nAlignParameters();

    // some debugging output
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG)<<"dumping matrix and vector to screen"<<endreq;
      for (int i=0;i<nDoF;i++)
        for (int j=0;j<nDoF;j++)
        //if (std::fabs((*m_bigmatrix)[i][j])>.0001)
          msg(MSG::DEBUG)<<i<<", "<<j<<" : "<<(*m_bigmatrix)[i][j] <<endreq;

      for (int i=0;i<nDoF;i++)
        msg(MSG::DEBUG)<<i <<" : "<<(*m_bigvector)[i]<<endreq;
    }

    // get rescaled first and second derivatives
    CLHEP::HepSymMatrix * d2Chi2 = new CLHEP::HepSymMatrix(m_aNDoF,0);
    CLHEP::HepVector    * dChi2  = new CLHEP::HepVector(m_aNDoF,0);
    for (int iActive=0;iActive<m_aNDoF;iActive++) {
      int i = m_activeIndices[iActive];
      (*dChi2)[iActive] = (*m_bigvector)[i];
      for (int jActive=0;jActive<m_aNDoF;jActive++) {
        int j = m_activeIndices[jActive];
        (*d2Chi2)[iActive][jActive] = (*m_bigmatrix)[i][j];
      }
    }

    ATH_MSG_DEBUG("First derivatives:" << (*dChi2));
    ATH_MSG_DEBUG("Second derivatives:" << (*d2Chi2));

    CLHEP::HepSymMatrix cov(m_aNDoF,0);
    CLHEP::HepVector delta(m_aNDoF,0);
    CLHEP::HepVector deltafull(m_aNDoF,0);

    bool status=true;
    int ierr(0);
    if(!m_diagonalize) {
      // ==========================================================
      //  Run Matrix Inversion
      ATH_MSG_INFO("Running matrix inversion");
      if(m_logStream)
        *m_logStream<<"Running matrix inversion"<<std::endl;

      cov = *d2Chi2;
      cov.invert(ierr);
      if(ierr>0)
        msg(MSG::ERROR)<<"CLHEP inversion status flag = "<<ierr<<endreq;
      else
        ATH_MSG_INFO("CLHEP inversion OK");
      if(m_logStream)
        *m_logStream<<"CLHEP inversion status flag = "<<ierr<<std::endl;

      // calculate corrections
      delta = cov * (*dChi2);

      // the covariance matrix is actually defined as 2 * d2Chi2^-1
      ATH_MSG_DEBUG("Result: "<<delta);
      ATH_MSG_DEBUG("cov: "<<cov*2);

      // -----------------------
      // calculate also for matrix and vector multiplied by factor 0.5
      // this should make no difference if everything is correct but
      // it can go wrong if insensitive DoF is included
      CLHEP::HepSymMatrix cov2 = *d2Chi2 * .5;
      // invert the matrix
      int ierr2 = 0;
      cov2.invert(ierr2);
      if(ierr2>0)
        msg(MSG::WARNING)<<"Second CLHEP inversion status flag = "<<ierr2<<endreq;

      CLHEP::HepVector delta2 = cov2 * (*dChi2) * .5;
      for (int i=0;i<delta.num_row(); ++i)
        if ( fabs((delta[i] - delta2[i])/delta[i]) > 1e-5 ) {
          msg(MSG::WARNING)<<"Something's wrong with the matrix inversion: delta["<<i<<"] = "<<delta[i]<<"    delta2["<<i<<"] = "<<delta2[i]<<endreq;
          status=false;
          break;
        }

      if(m_logStream && (ierr2>0 || !status)) {
        *m_logStream<<"CLHEP inversion status flag for halfed matrix = "<<ierr2<<std::endl;
        *m_logStream<<"Matrix inversion check failed"<<std::endl;
        *m_logStream<<std::endl;
      }
      // -- end of check of matrix inversion
    }
    else {
      // ==========================================================
      // Run Diagonalization
      ATH_MSG_INFO("Running diagonalization");
      if(m_logStream)
        *m_logStream<<"Running diagonalization"<<std::endl;

      CLHEP::HepSymMatrix D = *d2Chi2;
      CLHEP::HepMatrix U = CLHEP::diagonalize( &D );

      ATH_MSG_INFO("Diagonalization done");
      //sold = U*sdiag*U.T.

      // reorder eigenvalues ascending
      // eigenvectors need to be reordered consistently
      ATH_MSG_DEBUG(" Reordering eigenvalues ascending ");
      for (int i=0; i<m_aNDoF-1; i++)
        for (int j=i+1; j<m_aNDoF; j++)
          if(D[j][j] < D[i][i]) {
            // swap eigenvalues
            double ei = D[i][i];
            D[i][i] = D[j][j];
            D[j][j] = ei;
            // swap eigenvectors
            for(int k=0;k<m_aNDoF; k++) {
              double ev = U[k][i];
              U[k][i] = U[k][j];
              U[k][j] = ev;
            }
          }

      // how do I now get the eigenvalues? this cannot be the most
      // efficient way ...  CLHEP::HepSymMatrix D = d2Chi2->similarityT( U );
      CLHEP::HepVector eigenvector(m_aNDoF);

      if(m_logStream)
        *m_logStream<<"/------ The Eigenvalue Spectrum -------"<<std::endl;

      ATH_MSG_DEBUG("Calculating eigenvalues");
      for(int imode=0; imode<m_aNDoF; ++imode) {

        // get the relevant eigenvector
        for(int irow=0; irow<m_aNDoF; ++irow)
          eigenvector[irow] = U[irow][imode];

        // calculate the eigenvalue
        //double eigenvalue = d2Chi2->similarity( eigenvector );
        double eigenvalue = D[imode][imode];
        ATH_MSG_DEBUG("eigenvalue "<<eigenvalue);

        double evdotb = dot(*dChi2,eigenvector);
        CLHEP::HepVector thisdelta = evdotb/eigenvalue * eigenvector;
        deltafull += thisdelta;

        if(imode<m_modcut) {
          ATH_MSG_INFO("skipping eigenvalue "<<imode<<" : "<<eigenvalue<<" , modcut is "<<m_modcut);
          if(m_logStream)
             *m_logStream<<"| skipping eigenvalue "<<eigenvalue<<std::endl;
        }
        else if( eigenvalue < m_eigenvaluethreshold ) {
          ATH_MSG_INFO("skipping eigenvalue "<<eigenvalue<<" , cut is "<<m_eigenvaluethreshold);
          if(m_logStream)
             *m_logStream<<"| skipping eigenvalue "<<eigenvalue<<std::endl;
        }
        else {
          if(m_logStream)
            *m_logStream<<"| "<<eigenvalue<<std::endl;

          delta += thisdelta;

          // this is the time consuming part
          for(int irow=0; irow<m_aNDoF; ++irow)
            for(int icol=0; icol<=irow; ++icol)
              cov[irow][icol] += eigenvector[irow] * eigenvector[icol] / eigenvalue;
        }
      }

      // the covariance matrix is actually defined as 2 * d2Chi2^-1

      ATH_MSG_DEBUG("Result: "<<delta);
      ATH_MSG_DEBUG("cov: "<<cov);

      if(m_logStream)
        *m_logStream<<"\\----- End of Eigenvalue Spectrum -----"<<std::endl;

      // end of diagonalization
      // ==========================================================
    }

    // stop measuring time
    clock_t stoptime = clock();
    double totaltime = (stoptime-starttime)/double(CLOCKS_PER_SEC);
    ATH_MSG_INFO("Time spent in solveCLHEP: "<<totaltime<<" s");

    if(ierr==0 && status)
    {
      ATH_MSG_DEBUG("Alignment constants:");
      for (int iAdof=0;iAdof<m_aNDoF;iAdof++) {

        int idof = m_activeIndices[iAdof];
        AlignPar * alignPar=(*alignParList)[idof];

        double sigma = alignPar->sigma();
        double param = -delta[iAdof] * sigma;
        double err = std::sqrt(2.*std::fabs(cov[iAdof][iAdof])) * sigma;

        ATH_MSG_DEBUG(iAdof <<" : "<< param << " +/- "<< err);
        ATH_MSG_DEBUG("cov("<<iAdof<<")="<<cov[iAdof][iAdof]<<", sigma: "<<sigma);
        ATH_MSG_DEBUG("init par: "<<alignPar->initPar());
        alignPar->setPar(param,err);
        ATH_MSG_DEBUG("set param to "<<param<<" for alignPar "<<alignPar);
        ATH_MSG_DEBUG(*(*alignParList)[idof]);
      }

      if(m_logStream) {
        printGlobalSolution(*m_logStream,&cov);

        // norm of first derivative
        *m_logStream<<"norm of first derivative :            "<<dChi2->norm()<<std::endl;

        // distance to solution
        double dist = ( - (*d2Chi2) * deltafull + (*dChi2) ).norm();
        *m_logStream<<"distance to solution :                "<<dist<<std::endl;

        // calculate chi2 of the alignment change
        double chi2 = d2Chi2->similarity(delta) * .5;
        *m_logStream<<"delta(chi2) of the alignment change : "<<chi2<<" / "<<m_aNDoF<<std::endl;

        // time spent here
        *m_logStream<<"time spent in solve :                 "<<totaltime<<" s"<<std::endl;
      }
    }

    delete d2Chi2;
    delete dChi2;

    return 1;
  }

  //_______________________________________________________________________
  int MatrixTool::solveLocal()
  {
    ATH_MSG_INFO("solving using Local method");
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"**************  solving using Local method  *****************"<<std::endl;
      *m_logStream<<"*************************************************************"<<std::endl;
    }

    int    totalNDoF(0);
    double totalChi2(0.);

    const AlignModuleList * alignModules = m_alignModuleTool->alignModules1D();

    AlignModuleList::const_iterator imod     = alignModules->begin();
    AlignModuleList::const_iterator imod_end = alignModules->end();
    for( ; imod!=imod_end; ++imod) {

      AlignModule * module = *imod;

      ATH_MSG_INFO("Solving for module: "<<module->name());

      DataVector<AlignPar> * alignPars = m_alignModuleTool->getAlignPars(module);

      int thisNDoF = alignPars->size();

      CLHEP::HepSymMatrix d2Chi2(thisNDoF,0);
      CLHEP::HepVector dChi2(thisNDoF,0);
      for (int i=0;i<thisNDoF;++i) {
        int ipar = alignPars->at(i)->index();
        dChi2[i] = (*m_bigvector)[ipar];
        for (int j=0;j<thisNDoF;++j) {
          int jpar = alignPars->at(j)->index();
          d2Chi2[i][j] = (*m_bigmatrix)[ipar][jpar];
        }
      }

//      ATH_MSG_INFO("First derivatives:" << dChi2);
//      ATH_MSG_INFO("Second derivatives:" << d2Chi2);

      if(module->nHits() < m_minNumHits || module->nTracks() < m_minNumTrks) {
        ATH_MSG_INFO("Not enough hits in module \'"<<module->name()<<"\':  "
         <<module->nHits()<<" < "<<m_minNumHits<<" or "
         <<module->nTracks()<<" <  "<<m_minNumTrks
         <<". Skipping...");

        // print module summary even when solving is not done
        if(m_logStream)
          printModuleSolution(*m_logStream,module,0);

        continue;
      }

      ATH_MSG_DEBUG("First derivatives:" << dChi2);
      ATH_MSG_DEBUG("Second derivatives:" << d2Chi2);


      totalNDoF += thisNDoF;

      CLHEP::HepSymMatrix cov(d2Chi2);

      // invert the matrix
      int ierr = 0;
      cov.invert(ierr);
      if(ierr>0)
        ATH_MSG_WARNING("CLHEP inversion status flag = "<<ierr);
      else
        ATH_MSG_DEBUG("CLHEP inversion status flag = "<<ierr);

      // calculate corrections
      CLHEP::HepVector delta = cov * dChi2;
      //ATH_MSG_DEBUG("d2Chi2: "<<d2Chi2);
      //ATH_MSG_DEBUG("cov: "<<cov);
      //ATH_MSG_DEBUG("d2Chi2*cov: "<<d2Chi2*cov);
      //ATH_MSG_DEBUG("dChi2: "<<dChi2);
      ATH_MSG_DEBUG("Result: "<<delta);

      ATH_MSG_DEBUG("Alignment constants:");
      for (int idof=0;idof<thisNDoF;++idof) {
        AlignPar * alignPar = alignPars->at(idof);

        double sigma = alignPar->sigma();
        double param = -delta[idof] * sigma;
        double err = std::sqrt(2.*std::fabs(cov[idof][idof])) * sigma;

        ATH_MSG_DEBUG(idof <<" : "<< param << " +/- "<< err);
        ATH_MSG_DEBUG("cov("<<idof<<")="<<cov[idof][idof]<<", sigma: "<<sigma);
        ATH_MSG_DEBUG("init par: "<<alignPar->initPar());

        ATH_MSG_DEBUG("Filling constants obtained using Local method");
        alignPar->setPar(param,err);
        ATH_MSG_DEBUG("set param to "<<param<<" for alignPar "<<alignPar);
        ATH_MSG_DEBUG(*alignPar);
      }

      if(m_logStream) {
        printModuleSolution(*m_logStream,module,&cov);

        *m_logStream<<"CLHEP inversion status flag = "<<ierr<<std::endl;

        // calculate chi2 of the alignment change
        double chi2 = d2Chi2.similarity(delta) * .5;
        totalChi2 += chi2;
        *m_logStream<<"delta(chi2) of the alignment change : "<<chi2<<" / "<<thisNDoF<<std::endl;
      }
    }

    if(m_logStream) {
      *m_logStream<<"--------------------------------------------------------------------------------"<<std::endl;
      *m_logStream<<"Total delta(chi2) of the alignment change from the local method : "<<totalChi2<<" / "<<totalNDoF<<std::endl;
      *m_logStream<<std::endl;
    }

    return 1;
  }
  
   //________________________________________________________________________
  bool MatrixTool::accumulateFromFiles()
  {
    
    if(m_readTFiles){ 
      ATH_MSG_INFO("Info to obtained from from TFiles");
      return accumulateFromTFiles();
    }else{ 
      ATH_MSG_INFO("Info to obtained from from Binary files");
      return accumulateFromBinaries();
    }
  }

  //________________________________________________________________________
  bool MatrixTool::accumulateFromBinaries()
  {
    
    
    DataVector<AlignPar>* alignParList = m_alignModuleTool->alignParList1D();
    int nDoF=alignParList->size();

    std::map<int,unsigned long long> modIndexMap;
    float dummyVersion(0.);
    double totalscale=0.;
    for (int ivec=0;ivec<(int)m_inputVectorFiles.size();ivec++) {

      ATH_MSG_INFO("Reading vector "<<ivec<<" from file "<<m_inputVectorFiles[ivec]);

      AlVec newVector(nDoF);
      std::map<int,unsigned long long> newModIndexMap;
      newVector.SetPathBin(m_pathbin+m_prefixName);
      newVector.SetPathTxt(m_pathtxt+m_prefixName);
      double scale=0;
      StatusCode sc = newVector.ReadPartial(m_inputVectorFiles[ivec],scale,newModIndexMap,dummyVersion);
      totalscale += scale;
      if (sc==StatusCode::FAILURE) {
        msg(MSG::FATAL)<<"Problem reading vector from "<<m_inputVectorFiles[ivec]<<endreq;
        return false;
      }
      if (newVector.size()!=m_bigvector->size()) {
        msg(MSG::FATAL) <<"vector wrong size!  newVector size "<<newVector.size()
                        <<", bigvector size "<<m_bigvector->size()<<endreq;
        return false;
      }

      // check modIndexMaps to make sure they are the same
      if (ivec==0)
        modIndexMap = newModIndexMap;
      else if (modIndexMap!=newModIndexMap) {
        msg(MSG::FATAL)<<"module index maps don't agree!"<<endreq;
        return false;
      }
      if (ivec>0)
        *m_bigvector += newVector;
      else
        *m_bigvector  = newVector;
    }

    m_scale = totalscale;

    AlSymMat * symBigMatrix=dynamic_cast<AlSymMat*>(m_bigmatrix);
    AlSpaMat * spaBigMatrix=dynamic_cast<AlSpaMat*>(m_bigmatrix);


    for (int imat=0;imat<(int)m_inputMatrixFiles.size();imat++) {
      ATH_MSG_INFO("Reading matrix "<<imat<<" from file "<<m_inputMatrixFiles[imat]);

      // create new matrix to read data from current file
      int nDoF=modIndexMap.size();
      bool triang;
      StatusCode sc;
      if (symBigMatrix) {
        AlSymMat newMatrix(nDoF);
        sc = newMatrix.Read(m_inputMatrixFiles[imat],nDoF,triang,dummyVersion);
        if (sc==StatusCode::SUCCESS)
          *symBigMatrix += newMatrix;
      }
      else {
        if (!spaBigMatrix) {
          throw std::logic_error("Unhandled matrix type");
        }

        AlSpaMat newMatrix(nDoF);
        sc = newMatrix.Read(m_inputMatrixFiles[imat],nDoF,triang,dummyVersion);

        if (sc==StatusCode::SUCCESS) {
          if (imat>0)
            *spaBigMatrix += newMatrix;
          else
            *spaBigMatrix  = newMatrix;
        }
      }

      if (sc==StatusCode::FAILURE) {
        msg(MSG::FATAL)<<"problem reading matrix from "<<m_inputMatrixFiles[imat]<<endreq;
        return false;
      }

      if (!m_useSparse && triang==m_wSqMatrix) {
        ATH_MSG_WARNING("matrix not expected format!  Changing m_wSqMatrix to "<<!triang);
        m_wSqMatrix=!triang;
      }

    }

    // accumulate hitmap from hitmap files
    if(m_readHitmaps)
      readHitmaps();

    return true;
  }

  //_______________________________________________________________________
  void MatrixTool::storeInTFile(TString filename)
  {
    //Store reults in a single TFile....
    //Including Matrix Vector Hitmap.. Soluton EV’s etc.
    
    ATH_MSG_INFO("Writing Results to a TFile");
    
    DataVector<AlignPar> * alignParList = m_alignModuleTool->alignParList1D();
    int nDoF = alignParList->size();
  
    TMatrixDSparse* myTMatrix = m_bigmatrix->makeTMatrix();

    ATH_MSG_INFO( "Created TMatrixDSparse" );

  
    double *val = new double[nDoF];
    for (int i=0;i<nDoF;i++) {
      val[i] = (*m_bigvector)[i];
    }
  
    TVectorD myTVector(nDoF, val);
    delete [] val;
  
    ATH_MSG_INFO( "Created TVectorD" );

  
    const AlignModuleList * moduleList = m_alignModuleTool->alignModules1D();
    int nModules = moduleList->size();
  
    double *hitmapA = new double[nModules];
    double *hitmapB = new double[nModules];
    AlignModuleList::const_iterator imod     = moduleList->begin();
    AlignModuleList::const_iterator imod_end = moduleList->end();
    int index(0);
    for(; imod != imod_end; ++imod) {
      AlignModule * module = *imod;
      hitmapA[index] = (double)module->nHits();
      hitmapB[index] = (double)module->nTracks();
      index++;
    }
  
    TVectorD hitmapHits(nModules, hitmapA); 
    TVectorD hitmapTracks(nModules, hitmapB);
    
    delete [] hitmapA;
    delete [] hitmapB;
    


    TFile myFile(filename,"recreate");
    hitmapHits.Write("Hits");
    hitmapTracks.Write("Tracks");
    myTMatrix->Write("Matrix");
    myTVector.Write("Vector");
    
    TVectorD scale(1, &m_scale) ;
    scale.Write("Scale");
    
   
    double *moduleInfoA = new double[nDoF];//unsigned long long 
    double *dofInfoA = new double[nDoF];//int 
  
    if (sizeof(unsigned long long) != sizeof(double))      
      ATH_MSG_ERROR("Module Identifiers will not be saved.  sizeof(double)!=sizeof(ulonglong)");
    else{      
      
      DataVector<AlignPar>* alignPars = m_alignModuleTool->alignParList1D();
      for (int i=0;i<(int)alignPars->size();i++) {
        //Do a direct memory copy to store unsigned long long in the momory of a double
        double target;
        uint64_t  id = (*alignPars)[i]->alignModule()->identify().get_compact();
        memcpy(&target, &id, sizeof(target));
        moduleInfoA[i]=target;
        //moduleInfoB[i]=(*alignPars)[i]->alignModule()->name();
        uint64_t dof = (*alignPars)[i]->paramType();
        memcpy(&target, &dof, sizeof(target));
        dofInfoA[i]=target;
      }
  
      TVectorD moduleIDs(nDoF, moduleInfoA) ;
      TVectorD moduleDoFs(nDoF,dofInfoA);
      delete [] moduleInfoA;
      moduleIDs.Write("ModuleID");
      moduleDoFs.Write("dof");
    }
    
    myFile.Write();
    myFile.Close();
    
    delete myTMatrix;
    ATH_MSG_INFO("Finshed writing TFILE");

  }

//________________________________________________________________________
  bool MatrixTool::accumulateFromTFiles()
  {
    DataVector<AlignPar>* alignParList = m_alignModuleTool->alignParList1D();
    int nDoF=alignParList->size();
    ATH_MSG_INFO("OPENING TFILES");

    std::map<int,unsigned long long> modIndexMap;
    std::map<int,unsigned long long> DoFMap;
    double totalscale=0.;

    AlSymMat * symBigMatrix=dynamic_cast<AlSymMat*>(m_bigmatrix);
    AlSpaMat * spaBigMatrix=dynamic_cast<AlSpaMat*>(m_bigmatrix);
    //TMatrixDSparse *accumMatrix(0);
    AlSpaMat *accumMatrix = 0;
    
    const AlignModuleList * moduleList = m_alignModuleTool->alignModules1D();
    int nModules = moduleList->size();
    
    TVectorD TotalHits(nModules);
    TVectorD TotalTracks(nModules);
   
    int numberOfReadErrors = 0; 
   
    struct rusage myusage;
    int itworked =  getrusage(RUSAGE_SELF,&myusage);
    if(itworked)//note: rusage returns zero if it succeeds!
		  ATH_MSG_DEBUG("ItWorked");

		long intialMemUse = myusage.ru_maxrss;

    for (int ifile = 0; ifile < (int)m_inputTFiles.size(); ifile++) {
      if (numberOfReadErrors > m_maxReadErrors){
        msg(MSG::FATAL) << " number of errors when reading the TFiles already exceed " << m_maxReadErrors << endreq;
        return false;
      }
   
      ATH_MSG_INFO("Reading File number " << ifile << ",  " << m_inputTFiles[ifile]);
      
      itworked =  getrusage(RUSAGE_SELF,&myusage);
      ATH_MSG_INFO("Memory usage [MB], total " << myusage.ru_maxrss/1024 << ", increase  " << (myusage.ru_maxrss-intialMemUse)/1024);
       
      TFile* myFile = TFile::Open(m_inputTFiles[ifile].c_str()); 
      
      if ( myFile->IsZombie() || !(myFile->IsOpen()) ) {
        ++numberOfReadErrors;
        ATH_MSG_ERROR( " Problem reading TFile " << m_inputTFiles[ifile] );
        continue;
      }
      
      std::map<int,unsigned long long> newModIndexMap;
      
      TVectorD* myModuleIDs;
      myModuleIDs = (TVectorD*)myFile->Get("ModuleID");
      if( !myModuleIDs ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Modules ID not read!!!");
        continue;
      }
      
      for (int i(0); i<myModuleIDs->GetNrows(); ++i){
        //Coverting back from a double to a unvi signed long long
        double source = (*myModuleIDs)(i);
        uint64_t target;
        memcpy(&target, &source, sizeof(target));
        newModIndexMap[i]=target;
        //std::cout << i<< " " <<target <<std::endl; 
      }
      
      delete myModuleIDs;
      
      std::map<int,unsigned long long> newDoFMap;
      
      TVectorD* myDoFs;
      myDoFs = (TVectorD*)myFile->Get("dof");
      if( !myDoFs ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("DoFs not read!!!");
        continue;
      }
      
      for (int i(0); i<myDoFs->GetNrows(); ++i){
        //Coverting back from a double to a unsigned long long
        double source = (*myDoFs)(i);
        uint64_t target;
        memcpy(&target, &source, sizeof(target));
        newDoFMap[i]=target;
      }
      delete myDoFs;
      
      
      TVectorD* Scale;
      Scale = (TVectorD*)myFile->Get("Scale");
      if( !Scale ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Scale not read!!!");
        continue;
      }
      
      double scale=(*Scale)(0);
      totalscale += scale;
      delete Scale;
      
            
      ATH_MSG_INFO("Reading Vector");
      TVectorD* vector = (TVectorD*)myFile->Get("Vector");
      if( !vector ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Vector not read!!!");
        continue;
      }
      
      AlVec* newVector = new AlVec(nDoF);
      newVector->SetPathBin(m_pathbin+m_prefixName);
      newVector->SetPathTxt(m_pathtxt+m_prefixName);
      
      if (newVector->size()  != m_bigvector->size() ) {
         msg(MSG::FATAL) << "vector wrong size!  newVector size " << newVector->size()
                         << ", bigvector size " << m_bigvector->size()<<endreq;
         delete newVector;
         delete vector;
         return false;
      }
      
      if (m_bigvector->size() != vector->GetNrows() ) {
         msg(MSG::FATAL) << "File vector wrong size!  File Vector size " << vector->GetNrows()
                         << ", bigvector size " << m_bigvector->size()<<endreq;
         delete newVector;
         delete vector;
         return false;
      }
      
        
      for (int i=0;i<nDoF;i++) {
        (*newVector)[i] = (*vector)(i);
      }
      delete vector;
      
      // check modIndexMaps to make sure they are the same
      if (ifile == 0){
        DoFMap = newDoFMap;
      } else if (DoFMap!=newDoFMap) {
        msg(MSG::FATAL) << "module dofs don't agree!" << endreq;
        return false;
      }
      
      if (ifile == 0){
         modIndexMap = newModIndexMap;
      } else if (modIndexMap!=newModIndexMap) {
         msg(MSG::FATAL) << "module index maps don't agree!" << endreq;
         return false;
      }
      
      if (ifile>0){
        *m_bigvector += *newVector;
        delete newVector;  
      } else {
        delete m_bigvector;
        m_bigvector  = newVector;
      }
      
      
      ATH_MSG_INFO("Reading matrix ");
      TMatrixDSparse* matrix = (TMatrixDSparse*)myFile->Get("Matrix");
      
      if( !matrix ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Matrix not read!!!");
        continue;
      }
      
      
      if (ifile == 0 ){ 
      
        accumMatrix = new AlSpaMat(nDoF);
        ATH_MSG_INFO("Matrix size b4 "<< accumMatrix->ptrMap()->size()  );
        
        //This method is ok for large matrix files... really only access the non zero elements
        for (int ii=0;ii<nDoF;ii++) {
          const TMatrixTSparseRow_const<double> myRow = (*matrix)[ii];
          int i =  myRow.GetRowIndex();
          for (int jj=0;jj<myRow.GetNindex();jj++) {
            int j = (myRow.GetColPtr())[jj];
            const double  myElement= (myRow.GetDataPtr())[jj];
            if (i<j){
              ATH_MSG_INFO("i < j " );
              j = i;
              i = (myRow.GetColPtr())[jj];
            }
            (*accumMatrix)[i][j] = myElement;
          }
        }         
        ATH_MSG_INFO("Matrix size AF "<< accumMatrix->ptrMap()->size()  );
  
      } else if ( accumMatrix) {        
        ATH_MSG_INFO("Matrix size b4 "<< accumMatrix->ptrMap()->size()  );
  
        for (int ii=0;ii<nDoF;ii++) {
          const TMatrixTSparseRow_const<double> myRow = (*matrix)[ii];
          int i =  myRow.GetRowIndex();
          for (int jj=0;jj<myRow.GetNindex();jj++) {
            int j = (myRow.GetColPtr())[jj];
            const double  myElement= (myRow.GetDataPtr())[jj];
            if (i<j){
              ATH_MSG_INFO("i < j " );
              j = i;
              i = (myRow.GetColPtr())[jj];
            }
            (*accumMatrix)[i][j] += myElement;
          }
        }
        ATH_MSG_INFO("Matrix size AF "<< accumMatrix->ptrMap()->size()  );
 
      } else {
        delete matrix;
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Matrix allocation error!!!");
        continue;
      }
            
      delete matrix;
      
      TVectorD* hits;
      TVectorD* tracks;
      
      ATH_MSG_INFO("Reading hitmap ");
      hits = (TVectorD*)myFile->Get("Hits");
      if( !hits ){
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Hitmap 1 not read!!!");
        continue;
      }
      
      tracks = (TVectorD*)myFile->Get("Tracks");
      if( !tracks ){
        delete hits;
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Hitmap 2 not read!!!");
        continue;
      }
      
      if(hits->GetNrows() != TotalHits.GetNrows() ){
        delete hits;
        delete tracks; 
        ++numberOfReadErrors;
        ATH_MSG_ERROR("Hitmap size incorrect!!!");
        continue;
      }
      
      TotalHits   += (*hits);    
      TotalTracks += (*tracks);
      
      delete hits;
      delete tracks;
      
      myFile->Close("R");
      delete myFile;

      itworked =  getrusage(RUSAGE_SELF,&myusage);
      ATH_MSG_INFO("Memory usage [MB], total " << myusage.ru_maxrss/1024 << ", increase  " << (myusage.ru_maxrss-intialMemUse)/1024);

    }



    // create new matrix to read data from current file
    if(accumMatrix){
      if (symBigMatrix) {
        AlSymMat newMatrix(nDoF);
        //This method is ok for small matrix files
        for (int i=0;i<nDoF;i++) {
          for (int j=0;j<=i;j++) {
            newMatrix[i][j] = (*accumMatrix)[i][j];
          }
        }       

        *symBigMatrix += newMatrix;
        delete accumMatrix;
      } else if (spaBigMatrix) { 
        ATH_MSG_INFO( "should reassign matrix "<< spaBigMatrix->ptrMap()->size() );        
        *spaBigMatrix  += *accumMatrix;     
        ATH_MSG_INFO( "??????  "<< spaBigMatrix->ptrMap()->size() );
        delete accumMatrix;
      }
    }

        ATH_MSG_INFO( "??????  "<< m_bigmatrix->ptrMap()->size() );

    AlignModuleList::const_iterator imod     = moduleList->begin();
    AlignModuleList::const_iterator imod_end = moduleList->end();
    int index = 0;
    int totalhits = 0;
    for(; imod != imod_end; ++imod, ++index ) {
          AlignModule * module = *imod;
          module->setNHits((int)TotalHits(index));
          module->setNTracks((int)TotalTracks(index));
          totalhits += (int)TotalHits(index);
    }


    m_nHits = totalhits;
    m_nTracks = 0;
    m_nMeasurements = 0;
    m_scale = totalscale;

    return true;
  }




  //_______________________________________________________________________
  int MatrixTool::solve()
  {
    // ============
    // solve
    // ============
    ATH_MSG_DEBUG("in MatrixTool::solve()");    

    // set normalization scale to number of hits for now
    if(m_scale<0)
      m_scale = m_nHits;

    //-------------------------------------------------------
    // write matrix and vector to file
    if (m_writeMat) {
      // version has to be 2 to for reading matrices and vectors back in to work properly
      double dummyVersion(2.);

      // make map of matrix entry to module index (set by geometry manager tool)
      std::map<int,unsigned long long> modIndexMap;
      std::map<int,std::string>        modNameMap;
      DataVector<AlignPar>* alignPars = m_alignModuleTool->alignParList1D();
      for (int i=0;i<(int)alignPars->size();i++) {
        modIndexMap[i]=(*alignPars)[i]->alignModule()->identify().get_compact();
        modNameMap [i]=(*alignPars)[i]->alignModule()->name();
      }

      // binary files
      ATH_MSG_DEBUG("writing binary files");
      StatusCode sc1 = m_bigmatrix->Write("matrix.bin",true,m_wSqMatrix,m_scale,dummyVersion);
      StatusCode sc2 = m_bigvector->WritePartial("vector.bin",true,m_scale,modIndexMap,dummyVersion);
      if (!sc1.isSuccess() || !sc2.isSuccess()) {
        msg(MSG::ERROR)<<"problem writing matrix or vector"<<endreq;
        return -1;
      }

      if (m_writeMatTxt) {

        // text files
        ATH_MSG_DEBUG("writing text files");
        sc1 = m_bigmatrix->Write("matrix.txt",false,m_wSqMatrix,m_scale,dummyVersion);
        sc2 = m_writeModuleNames ? 
          m_bigvector->WritePartial("vector.txt",false,m_scale,modNameMap,dummyVersion) :
          m_bigvector->WritePartial("vector.txt",false,m_scale,modIndexMap,dummyVersion);
  
        if (!sc1.isSuccess() || !sc2.isSuccess()) {
          msg(MSG::ERROR)<<"problem writing matrix or vector"<<endreq;
          return -1;
        }
      }

      ATH_MSG_DEBUG("matrix and vector written to: "<<m_pathbin+m_prefixName<<"matrix.bin (.txt) and "<<m_pathbin+m_prefixName<<"vector.bin (.txt)");
    }

    //-------------------------------------------------------
    // write hitmap to file
    if (m_writeHitmap)
      writeHitmap();

    if(m_writeTFile)
      storeInTFile(m_pathbin+m_prefixName+m_tfileName);


    if(!m_runLocal && m_solveOption==0) {
      ATH_MSG_INFO("No solving requested.");
      return 1;
    }

    //-------------------------------------------------------
    // rescale the vector and the matrix according to sigmas
    // and apply soft mode cut

    ATH_MSG_INFO("rescaling the matrix/vector and applying the soft-mode-cut");

    DataVector<AlignPar>* alignParList = m_alignModuleTool->alignParList1D();
    int nDoF = alignParList->size();


    const AlSymMat * chkMatrix = dynamic_cast<const AlSymMat*>(m_bigmatrix);
    if(chkMatrix){
      // Method when using the dense matrix
      for (int i=0;i<nDoF;i++) {
        // scale the vector
        double sigma_i = (*alignParList)[i]->sigma();
        double softCut = 2 * pow( (*alignParList)[i]->softCut() , -2 );
        (*m_bigvector)[i] *= sigma_i;
  
        for (int j=0;j<=i;j++) {
          // scale the matrix
          if ((*chkMatrix)[i][j] != 0.) {
            double sigma_j = (*alignParList)[j]->sigma();
            (*m_bigmatrix)[i][j] *= sigma_i * sigma_j;
          }
          // apply soft-mode-cut
          if (i==j && m_softEigenmodeCut>0.){
            (*m_bigmatrix)[i][j] += m_softEigenmodeCut * softCut;
         
          }
  
          // set first and second derivatives on AlignPar
          if (i==j) {
            (*alignParList)[i]->setFirstDeriv((*m_bigvector)[i]/sigma_i);
            (*alignParList)[i]->setSecndDeriv((*m_bigmatrix)[i][i]/sigma_i/sigma_i);
          }
        }
      }    
    } else {
      // Method when using the sparse matrix
      const_mapiterator pos = m_bigmatrix->ptrMap()->begin();
      for( ; pos!=m_bigmatrix->ptrMap()->end(); pos++){
        int i = pos->first.first;
        int j = pos->first.second;
  
        // Scale matrix
        double sigma_i = (*alignParList)[i]->sigma();
        double sigma_j = (*alignParList)[j]->sigma();
        
        (*m_bigmatrix)[i][j] *= sigma_i * sigma_j;
  
      }
  
  
      for (int i=0;i<nDoF;i++) {
        // scale the vector
        double sigma_i = (*alignParList)[i]->sigma();
        (*m_bigvector)[i] *= sigma_i;
        if (m_softEigenmodeCut >0. ){
          double softCut = 2 * pow( (*alignParList)[i]->softCut() , -2 );
          (*m_bigmatrix)[i][i] += m_softEigenmodeCut * softCut;
          ATH_MSG_INFO( "DOF "<< i <<" Nhits "<< (*alignParList)[i]->alignModule()->nHits() << " soft-mode-cut "<< (*alignParList)[i]->softCut() <<" -> " << m_softEigenmodeCut * softCut  << " sigma_i "<< sigma_i << " Matrix: " << (*m_bigmatrix)[i][i] << " Vector: " << (*m_bigvector)[i]);
        }
        (*alignParList)[i]->setFirstDeriv((*m_bigvector)[i]/sigma_i);
        (*alignParList)[i]->setSecndDeriv((*m_bigmatrix)[i][i]/sigma_i/sigma_i);
      }
    }
    
    unsigned long long OldPixelIdentifier = 37769216; //Identifier for the Pixel Detector
    unsigned long long IBLIdentifier      = 33574912; //Identifier for the Pixel Detector
    

    

    ATH_MSG_INFO("rescaling done");

    // select modules with non-zero tracks
    for(int i=0;i<nDoF;i++)
      {

	if (m_AlignIBLbutNotPixel) //If m_AlignIBLbutNotPixel is set to True, Pixel would be skiped in the solving.
	  if  ((*alignParList)[i]->alignModule()->identify32()== OldPixelIdentifier)  
	    {ATH_MSG_INFO( "Pixel DOF have been skiped in the solving because AlignIBLbutNotPixel is set to True");
	      continue;} 
	
	if (m_AlignPixelbutNotIBL) //If m_AlignPixelbutNotIBL is set to True, IBL would be skiped in the solving.
	  if  ((*alignParList)[i]->alignModule()->identify32()== IBLIdentifier)  
	    {ATH_MSG_INFO( "IBL DOF have been skiped in the solving because AlignPixelbutNotIBL is set to True");
	      continue;}
	
	if((*alignParList)[i]->alignModule()->nHits() >= m_minNumHits && (*alignParList)[i]->alignModule()->nTracks() >= m_minNumTrks)
	  m_activeIndices.push_back(i);
      }
    m_aNDoF = m_activeIndices.size();
    ATH_MSG_DEBUG("aNDoF/nDoF: "<<m_aNDoF<<"/"<<nDoF);

    // --------------------
    // now do the SOLVING
    // --------------------

    int info = 0;

    // first Local solving
    if (m_runLocal)
      info = solveLocal();

    // remove spurious modules and resize
    if (m_removeSpurious) {

      ATH_MSG_INFO("Spurious removal not implemented at the moment.");
/*      if (StatusCode::SUCCESS != spuriousRemoval()) {
        ATH_MSG_ERROR("Problem while trying to remove spurious. Stopping solving");
        return -1;
      }

      // if nDoF=0, bad job...
      int NDoF = m_alignModuleTool->nAlignParameters();
      if(NDoF==0) {
        ATH_MSG_WARNING("Removal removed everything: NDoF=" << NDoF << " !!!");
        return 1;
      }
      ATH_MSG_DEBUG("NDoF: " << NDoF);
*/
    }

    // --------------------
    // now Global solving
    switch(m_solveOption) {

      case NONE:
        ATH_MSG_INFO("No global solving requested.");
        break;

      case SOLVE_ROOT:
        info = solveROOT();
        break;

      case SOLVE_CLHEP:
        info = solveCLHEP();
        break;

      case SOLVE:
      case DIRECT_SOLVE:
      case DIRECT_SOLVE_CLUSTER:
        info = solveLapack();
        break;

      case SOLVE_FAST:
      case DIRECT_SOLVE_FAST:
        info = solveSparseEigen();
        break;

      default:
        ATH_MSG_INFO("Unknown solving option.");
        info = 0;
        break;
    }

    ATH_MSG_INFO("Return value from solving: "<<info);

    return info;
  }


  //_______________________________________________________________________
  void MatrixTool::addFirstDerivatives(AlVec* )
  {
    return;
  }

  //_______________________________________________________________________
  void MatrixTool::addSecondDerivatives(AlSymMatBase* )
  {
    return;
  }

  //________________________________________________________________________
  void MatrixTool::addFirstDerivatives(std::list<int,double>& )
  {
    return;
  }

  //________________________________________________________________________
  void MatrixTool::addSecondDerivatives(std::list<std::pair<int,int>,double >&)
  {
    return;
  }

  //________________________________________________________________________
  void MatrixTool::addFirstDerivative(int irow, double firstderiv)
  {
    (*m_bigvector)[irow] += firstderiv;
  }

  //________________________________________________________________________
  void MatrixTool::addSecondDerivative(int irow, int icol, double secondderiv)
  {
    (*m_bigmatrix)[irow][icol] += secondderiv;
  }

  //________________________________________________________________________
  void MatrixTool::printGlobalSolution(std::ostream & os, const CLHEP::HepSymMatrix * cov) const
  {
    const AlignModuleList * alignModules = m_alignModuleTool->alignModules1D();

    std::vector<int>::iterator it;

    AlignModuleList::const_iterator imod     = alignModules->begin();
    AlignModuleList::const_iterator imod_end = alignModules->end();
    for( ; imod!=imod_end; ++imod) {
      AlignModule * module = *imod;

      DataVector<AlignPar> * alignPars = m_alignModuleTool->getAlignPars(module);
      int thisNDoF = alignPars->size();

      // fill local covariance matrix
      CLHEP::HepSymMatrix * covsub = 0;
      if(cov && module->nHits() >= m_minNumHits && module->nTracks() >= m_minNumTrks) {
        covsub = new CLHEP::HepSymMatrix(thisNDoF,0);
        for (int i=0;i<thisNDoF;++i) {
          int ipar = alignPars->at(i)->index();
          double sigma_i = alignPars->at(i)->sigma();

          std::vector<int>::const_iterator itActive = std::find(m_activeIndices.begin(),m_activeIndices.end(),ipar);
          if( itActive == m_activeIndices.end() )
            continue;
          int iActive = std::distance(m_activeIndices.begin(),itActive);

          for (int j=0;j<=i;++j) {
            int jpar = alignPars->at(j)->index();
            double sigma_j = alignPars->at(j)->sigma();

            std::vector<int>::const_iterator jtActive = std::find(m_activeIndices.begin(),m_activeIndices.end(),jpar);
            if( jtActive == m_activeIndices.end() )
              continue;
            int jActive = std::distance(m_activeIndices.begin(),jtActive);

            (*covsub)[i][j] = (*cov)[iActive][jActive] * sigma_i * sigma_j;
          }
        }
      }

      printModuleSolution(os,module,covsub);

      delete covsub;
    }
    os << "--------------------------------------------------------------------------------" << std::endl;
  }

  //________________________________________________________________________
  void MatrixTool::printGlobalSolution(std::ostream & os, const TMatrixDSym * cov0) const
  {
    CLHEP::HepSymMatrix * cov = 0;
    if(cov0) {
      int nsize = cov0->GetNrows();
      cov = new CLHEP::HepSymMatrix(nsize,0);

      for(int i=0; i<nsize; i++)
        for(int j=0; j<=i; j++)
          (*cov)[i][j] = (*cov0)[i][j];
    }

    printGlobalSolution(os,cov);

    delete cov;
  }
/**
  namespace {
    class RestoreIOSFlags 
    {
    public:
      RestoreIOSFlags (std::ostream &os) 
        : m_os(&os), 
          m_flags(m_os->flags()),
          m_precision(m_os->precision())
      {}
      ~RestoreIOSFlags() {
        m_os->flags(m_flags);
        m_os->precision(m_precision);
      }
    private:
      std::ostream *m_os;
      std::ios_base::fmtflags m_flags;
      std::streamsize  m_precision;
    };
  }
**/
  //________________________________________________________________________
  void MatrixTool::printModuleSolution(std::ostream & os, const AlignModule * module, const CLHEP::HepSymMatrix * cov) const
  {
    boost::io::ios_all_saver  ias( os ); //save the stream state    

    os << "--------------------------------------------------------------------------------" << std::endl;
    os << "Alignment parameters for module: " << module->name() << std::endl;
    os << "Number of tracks passing: " << module->nTracks() << std::endl;
    if(m_minNumHits>0 && module->nHits()<m_minNumHits) {
      os << "Number of hits too small: "<<module->nHits()<<" < "<<m_minNumHits<<"  Skipping the module"<<std::endl;
      return;
    }
    if(m_minNumTrks>0 && module->nTracks()<m_minNumTrks) {
      os << "Number of tracks too small: "<<module->nTracks()<<" < "<<m_minNumTrks<<"  Skipping the module"<<std::endl;
      return;
    }
    os << "Number of hits seen:      " << module->nHits() << std::endl;
    os << "Number of tracks seen:    " << module->nTracks() << std::endl;

    DataVector<AlignPar> * alignPars = m_alignModuleTool->getAlignPars(module);
    int thisNDoF = alignPars->size();

    if(alignPars->size()==0)
      os << "No active parameters" << std::endl;
    else
    {
      //RestoreIOSFlags restore_flags(os);

      os.unsetf(std::ios_base::floatfield);
      os << std::setiosflags(std::ios_base::left) << std::setprecision(5);

      // output alignment parameters and errors
      DataVector<AlignPar>::const_iterator ipar     = alignPars->begin();
      DataVector<AlignPar>::const_iterator ipar_end = alignPars->end();
      for ( ; ipar != ipar_end; ++ipar) {
        AlignPar * par = *ipar;
        os << std::setw(10) << par->dumpType()
           << std::setw(12) << par->par() << " +/- " << std::setw(12) << par->err()
           << std::endl;
      }

      if(cov) {
        // calculate local correlation matrix
        CLHEP::HepSymMatrix corrsub(thisNDoF,0);
        for(int irow=0; irow<thisNDoF; ++irow)
          for(int icol=0; icol<=irow; ++icol)
            corrsub[irow][icol] = (*cov)[irow][icol] / sqrt((*cov)[irow][irow] * (*cov)[icol][icol]);
        os << "Local correlation matrix: " << corrsub << std::flush;
      }
    }
    ias.restore(); //restore the stream state
  }

  //________________________________________________________________________
  int MatrixTool::fillVecMods()
  {
    return 0;
  }

  //________________________________________________________________________
  int MatrixTool::solveLapack( )
  {
    ATH_MSG_INFO("solving Global using Lapack");
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"**************  solving using Global method  ****************"<<std::endl;
      *m_logStream<<"**************         using LAPACK          ****************"<<std::endl;
      *m_logStream<<"*************************************************************"<<std::endl;
    }

    // get rescaled first and second derivatives
    AlSymMat* aBetterMat = new AlSymMat(m_aNDoF);
    AlVec*    aBetterVec = new AlVec(m_aNDoF);
    for (int iActive=0;iActive<m_aNDoF;iActive++) {
      int i = m_activeIndices[iActive];
      (*aBetterVec)[iActive] = (*m_bigvector)[i];
      for (int jActive=0;jActive<m_aNDoF;jActive++) {
        int j = m_activeIndices[jActive];
        (*aBetterMat)[iActive][jActive] = (*m_bigmatrix)[i][j];
      }
    }

    // normalize bigmatrix and bigvector
    if(m_scaleMatrix) {
      if(m_scale<=0.)
        ATH_MSG_WARNING("Scaling requested but scale not set. Not scaling matrix and vector.");
      else {
        (*aBetterVec) *= 1./m_scale;
        (*aBetterMat) *= 1./m_scale;
      }
    }

    // Print() method is not yet implemented for AlVec and AlSymMat
//    ATH_MSG_DEBUG("First derivatives: "<<aBetterVec->Print());
//    ATH_MSG_DEBUG("Second derivatives: "<<aBetterMat->Print());

    ATH_MSG_DEBUG("Now Solving alignment using lapack diagonalization routine dspev...");

    if (m_calDet) {
      const double tol = 1.e-20;
      // compute final determinant
      double determ = (*aBetterMat).determinant();
      ATH_MSG_INFO("Determinant: " << determ);
      if (fabs(determ) < tol)
        ATH_MSG_WARNING("Matrix is singular!");
    }

    // store the original matrix for checks
    AlSymMat * d2Chi2 = 0;
    if (m_calculateFullCovariance)
      d2Chi2 = new AlSymMat(*aBetterMat);

    clock_t starttime = clock();

    // declare transition matrix + vector to store eigenvalues
    AlMat z(m_aNDoF,m_aNDoF);
    AlVec w(m_aNDoF); // vector to store the eigenvalues
    ATH_MSG_DEBUG("MatrixTool::after z/w allocation");

    char jobz = 'V';
    int info = (*aBetterMat).diagonalize(jobz,w,z);
    ATH_MSG_DEBUG(" info: " << info);
    ATH_MSG_INFO("MatrixTool::after diagonalization");

    // stop time calculation
    clock_t stoptime = clock();
    double time_diag = (stoptime-starttime)/double(CLOCKS_PER_SEC);
    ATH_MSG_INFO(" - time spent diagonalizing the matrix: "<<time_diag<<" s");

    double time_solve = 0.;
    if (info==0) {
      starttime = clock();
      postSolvingLapack(aBetterVec,d2Chi2,w,z,m_aNDoF);
      stoptime = clock();
      time_solve = (stoptime-starttime)/double(CLOCKS_PER_SEC);
      ATH_MSG_INFO(" - time spent solving the system: "<<time_solve<<" s");
      if(m_logStream) {
        *m_logStream<<"time spent for diagonalization: "<<time_diag<<" s"<<std::endl;
        *m_logStream<<"time spent for post-solving: "<<time_solve<<" s"<<std::endl;
      }
    }
    else {
      ATH_MSG_ERROR("Problem in diagonalization. Solving skipped.");
      if(m_logStream)
        *m_logStream<<"time spent for diagonalization: "<<time_diag<<" s"<<std::endl;
    }

    if(m_logStream) {
      *m_logStream<<"total time spent in solve: "<<time_diag+time_solve<<" s"<<std::endl;
      *m_logStream<<std::endl;
    }

    delete d2Chi2;
    delete aBetterMat;
    delete aBetterVec;

    // need to do this since success return value from Lapack is 0
    // and from solveLapack() it is 1
    if (info==0)
      info = 1;

    return info;
  }

  //________________________________________________________________________
  StatusCode MatrixTool::spuriousRemoval()
  {

    // copied from SiGlobalChi2Algs
    ATH_MSG_DEBUG("in spuriousRemoval");

    // compute determinant before resizing
    if (m_calDet) {
      const double tol = 1.e-20;
      double determ = m_bigmatrix->determinant();
      ATH_MSG_INFO("Determinant: " << determ);
      if (std::fabs(determ) < tol)
        ATH_MSG_WARNING("Matrix is singular!");
    }

    // fill vector with modules that need to be removed
    int fillvecmods=fillVecMods();
    if (fillvecmods==0) {

      //ATH_MSG_INFO(" No resize needed (NhitsCut = "
      //     << m_hitscut << ")");

      if (msgLvl(MSG::DEBUG)) {
        //m_bigmatrix->Write("bigmatrix.txt", false, m_wSqMatrix, m_scale,
        //           MatVersion);
        //m_bigvector->Write("bigvector.txt", false, m_scale, m_modcodemap,
        //           VecVersion, m_alignProcessLevel, m_fitParam);
      }

      return StatusCode::SUCCESS;
    }
    else if (fillvecmods==2)
      return StatusCode::FAILURE;

    /* this is a bit difficult to implement for now....

    // remove matrix/vector elements
    int cont=0;
    ModuleIndexMap::iterator itcode;
    ATH_MSG_INFO("Eliminating module...");



    for (std::vector<int>::const_iterator it=m_dropmods.begin();
    it!= m_dropmods.end(); ++it) {

    itcode = m_modcodemap.find((*it));

    if (itcode == m_modcodemap.end()) {
    ATH_MSG_WARNING("Could not find module " << *it << " in map.");
    return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(" - Removing mcode: " << (itcode->second)
    << " (old index: " << (*it) << " -> new index: " << (*it)-cont << ")");

    m_bigmatrix->RemoveModule((*it)-cont);
    m_bigvector->RemoveModule((*it)-cont);
    cont++;
    }
    ATH_MSG_INFO("Modules removed from the Matrix and from the Vector Successfully!");
    ATH_MSG_DEBUG("(DoF: " << nDoF << ")");

    // resizing...
    ATH_MSG_INFO("Resizing the bigvector in memory...");
    m_bigvector->reSize(nDoF-6*m_dropmods.size());
    ATH_MSG_INFO("Resizing the bigmatrix in memory...");
    m_bigmatrix->reSize(nDoF-6*m_dropmods.size());

    ATH_MSG_INFO(m_dropmods.size() << " modules eliminated from the matrix, i.e, "
    << 6*m_dropmods.size() << " DoFs");
    ATH_MSG_INFO(nDoF/6 - m_dropmods.size() << " modules to align (" << nDoF-6*m_dropmods.size() << " DoFs)");
    ATH_MSG_INFO("New bigmatrix size is: " << m_bigmatrix->size());
    ATH_MSG_INFO("New bigvector size is: " << (*m_bigvector).size());

    NDoF = nDoF-6*(m_dropmods.size());

    // resize (update) nDoF variable
    nDoF = NDoF;

    // Resizing vectors to store results
    m_alignPar->reSize(nDoF);
    m_alignSqErr->reSize(nDoF);

    // Fill the mapping module map and updating m_modcodemap
    UpdateModMap();
    */

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void MatrixTool::postSolvingLapack(AlVec * dChi2, AlSymMat * d2Chi2, AlVec &w, AlMat &z, int size)
  {
    ATH_MSG_DEBUG("in postSolvinglapack()");

    if( z.ncol() != size) {
      msg(MSG::ERROR)<<"Eigenvector matrix has incorrect size : "<<z.ncol()<<" != "<<size<<endreq;
      return;
    }

    if( (int)m_activeIndices.size() != size) {
      msg(MSG::ERROR)<<"Number of active parameters is incorrect : "<<m_activeIndices.size()<<" != "<<size<<endreq;
      return;
    }

    // Compute bigvector in diagonal basis (Vb = Ut * bigvector)
    AlVec D(size);
    D = z*(*dChi2);

    if (m_writeEigenMat) {

      ATH_MSG_INFO("writing the eigenvectors in a matrix: "<< z.nrow() << "x" << z.ncol());

      // Set Path for the z matrix (eigenvector matrix)
      z.SetPathBin(m_pathbin+m_prefixName);
      z.SetPathTxt(m_pathtxt+m_prefixName);

      ATH_MSG_INFO("writing the eigenvector matrix: "<< m_scalaMatName);
      ATH_MSG_DEBUG("matrix will be in: "<< m_pathbin+m_prefixName+m_scalaMatName);

      StatusCode sc = z.Write("eigenvectors.bin",true); // write the eigenvector matrix

      if (sc!=StatusCode::SUCCESS)
        msg(MSG::ERROR)<<"Problem writing eigenvector matrix"<<endreq;

      // Set Path for the w matrix (eigenvalues matrix - diagonal bigmatrix)
      w.SetPathBin(m_pathbin+m_prefixName);
      w.SetPathTxt(m_pathtxt+m_prefixName);

      ATH_MSG_INFO("writing the eigenvectors in a vector: "<< w.size());
      ATH_MSG_INFO("writing the eigenvalues vector (diagonal bigmatrix): "<< m_scalaVecName);
      ATH_MSG_DEBUG("vector will be in: "<< m_pathbin+m_prefixName+m_scalaVecName);

      sc = w.WriteEigenvalueVec("eigenvalues.bin",true); // write the eigenvalues vecor

      if (sc!=StatusCode::SUCCESS)
        msg(MSG::ERROR)<<"Problem writing eigenvector matrix"<<endreq;

      if (m_writeEigenMatTxt) {
        sc = z.Write("eigenvectors.txt", false);
        if (sc!=StatusCode::SUCCESS)
          msg(MSG::ERROR)<<"Problem writing eigenvector matrix to text file"<<endreq;
        sc = w.WriteEigenvalueVec("eigenvalues.txt", false);
        if (sc!=StatusCode::SUCCESS)
          msg(MSG::ERROR)<<"Problem writing eigenvalue vector to text file"<<endreq;
      }

    }

    // Set eigenvalue thresholds
    const double eigenvalue_threshold = 1e-19;

    // weak mode removal
    if (m_modcut == -1) {

      ATH_MSG_INFO(" Starting the automatic Weak Mode Removal method");

      // create a pull vector for the alignment corrections in diagonal basis (db_pulls)
      //int nDoF=m_alignModuleTool->nAlignParameters();
      AlVec* m_Align_db = new AlVec(size);
      AlVec* m_Align_error_db = new AlVec(size);
      AlVec* m_AlignPull = new AlVec(size);
      ATH_MSG_DEBUG("AlignPull vector size is: "<< (*m_AlignPull).size());

      m_modcut = 0;
      bool wm_stop = false;

      // -----------------------------------------------------------------------
      // First pass: removing weak modes because of large pull
      // compute alignment pulls for corrections in diagonal basis (db)
      for(int i=0; i<size; i++) {

        (*m_Align_db)[i] = (-D[i]/w[i]);
        (*m_Align_error_db)[i] = sqrt(1.0/w[i]/m_scale);

        if (w[i]<eigenvalue_threshold) {
          ATH_MSG_INFO("  + EigenMode " << i
                         << " removed as eigenvalue lower than the threshold " << eigenvalue_threshold
                         << ": " << w[i]);
          (*m_AlignPull)[i] = 0.0;
          m_modcut++;
        }
        else
          (*m_AlignPull)[i] = (*m_Align_db)[i] / (*m_Align_error_db)[i];

        ATH_MSG_DEBUG(i << ". AlignPar: " << (*m_Align_db)[i] << " +- " << (*m_Align_error_db)[i]
                        << " (pull: " << (*m_AlignPull)[i]  << ") ; w[i]: " << w[i]);
      }
      ATH_MSG_INFO(" +++ Weak Mode removal ++ stop after mode "<< m_modcut << " (First pass)");
      // -----------------------------------------------------------------------

      // -----------------------------------------------------------------------
      // Second pass
      // if the error is greater than the correction -> cut this mode
      for(int i=m_modcut; (i<size && !wm_stop); i++) {

        // if the error is greater than the correction -> cut this mode
        if (fabs((*m_AlignPull)[i])<m_pullcut) {
          ATH_MSG_INFO("  + EigenMode " << i
                         << " removed as pull is lower than " << m_pullcut << ": "
                         << (*m_AlignPull)[i]);
          m_modcut++;
        }
        else
          wm_stop = true;

      }
      ATH_MSG_INFO(" +++ Weak Mode removal ++ stop after mode "<< m_modcut << " (Second pass)");
      // -----------------------------------------------------------------------

      wm_stop = false;

      // ----------------------------------------------------------------------
      // Third pass
      // Check if the next eigenvalues is far away. If it is two orders of
      // magnitude bigger remove also this mode and allow the search
      for(int i=m_modcut; (i<size && !wm_stop); i++) {

        // if the next eigenvalues is far away -> cut this mode
        if (m_eigenvalueStep*w[i]<w[i+1]) {
          ATH_MSG_INFO("  + EigenMode " << i
                         << " removed as diff between eigenvalues, " << w[i] << " and " << w[i+1]
                         << ", is greater than " << m_eigenvalueStep);
          m_modcut++;
        }
        else
          wm_stop = true;

      }
      ATH_MSG_INFO(" +++ Weak Mode removal ++ stop after mode "<< m_modcut << " (Third pass)");
      // -----------------------------------------------------------------------

      wm_stop = false;

      // -----------------------------------------------------------------------
      // Fourth pass
      // Check if the next eigenvalues is far away. If it is two orders of
      // magnitude bigger remove also this mode and allow the search
      for(int i=m_modcut; (i<size && !wm_stop); i++) {

        // if the next eigenvalues is far away -> cut this mode
        if ( fabs((*m_Align_db)[i]) > m_Align_db_step*fabs((*m_Align_db)[i+1]) ) {
          ATH_MSG_INFO("  + EigenMode " << i
                         << " removed as diff between corrections, " << w[i] << " and " << w[i+1]
                         << ", is greater than "
                         << m_Align_db_step);
          m_modcut++;
        }
        else
          wm_stop = true;

      }
      ATH_MSG_INFO(" +++ Weak Mode removal ++ stop after mode "<< m_modcut << " (Fourth pass)");
      // -----------------------------------------------------------------------------------------------------

      // Free memory and clear the pointer to
      // prevent using invalid memory reference
      delete m_Align_db;
      delete m_Align_error_db;
      delete m_AlignPull;
      m_Align_db = 0;
      m_Align_error_db = 0;
      m_AlignPull = 0;

    } // end of if(m_modcut == -1)

    // Save some stuff to debug purposes
    /*
      if (m_storeDia) {
      std::string path = m_pathtxt+m_prefixName+"align_dia";
      std::fstream orthogon(path.c_str(), std::ios::out);
      orthogon.setf(std::ios::fixed);
      orthogon.setf(std::ios::showpoint);
      orthogon.precision(6);

      orthogon << std::setw(10)
      << "--------------------------------------------------------------------------------"
      << std::endl;
      orthogon << std::setw(10) << " ModeCut = " << m_modcut << std::endl;
      orthogon << std::setw(10)
      << "--------------------------------------------------------------------------------"
      << std::endl;

      orthogon << std::setw(10) << "mode"
      << std::setw(20) << "eigenmode"
      << std::setw(20) << "eigenmode error"
      << std::setw(25) << "eigenvalue"
      << std::endl;

      for( int m=0; m<size; m++) {

      // mode
      orthogon << std::setw(10) << m;

      // eigenmode (db)
      if( w[m]>1.0e-15) orthogon << std::setw(20) << -D[m]/w[m];
      else orthogon << std::setw(20) << 0.0;

      // error eigenmode (error_db)
      if( w[m]>1.0e-15) orthogon << std::setw(20) << sqrt(1.0/w[m]/m_scale);
      else orthogon << std::setw(20) << 0.0;

      // eigenvalues
      orthogon << std::setw(25) << w[m] << std::endl;
      }
      orthogon.close();
      } // end store align_dia.txt
    */

    AlVec delta(size);
    AlVec deltafull(size);
    AlVec errSq(size);

    // full covariance matrix
    CLHEP::HepSymMatrix * cov = 0;
    if(m_calculateFullCovariance)
      // Warning ! The matrix can be huge!
      // This can lead to memory problems
      cov = new CLHEP::HepSymMatrix(size,0);

    if(m_logStream)
      *m_logStream<<"/------ The Eigenvalue Spectrum -------"<<std::endl;

    for (int i=0;i<size;i++) {
      AlVec thisdelta(size);
      for(int j=0;j<size;j++)
        thisdelta[j] = z[i][j] * (-D[i]/w[i]);
      deltafull += thisdelta;

      ATH_MSG_DEBUG("eigenvalue "<<w[i]);
      if( i<m_modcut ) {
        ATH_MSG_INFO("skipping eigenvalue "<<w[i]<<" , modcut is "<<m_modcut);
        if(m_logStream)
           *m_logStream<<"| skipping eigenvalue "<<w[i]<<std::endl;
      }
      else if( w[i] < m_eigenvaluethreshold ) {
        ATH_MSG_INFO("skipping eigenvalue "<<w[i]<<" , cut is "<<m_eigenvaluethreshold);
        if(m_logStream)
           *m_logStream<<"| skipping eigenvalue "<<w[i]<<std::endl;
      }
      else {
        if(m_logStream)
          *m_logStream<<"| "<<w[i]<<std::endl;

        delta += thisdelta;
        for(int j=0;j<size;j++) {
          errSq[j] += z[i][j] * z[i][j] / w[i];
          if(m_calculateFullCovariance) {
            for(int k=0;k<=j;k++)
              (*cov)[j][k] += z[i][j] * z[i][k] / w[i];
          }
        }
      }
    }

    if(m_logStream)
      *m_logStream<<"\\----- End of Eigenvalue Spectrum -----"<<std::endl;

    ATH_MSG_DEBUG("Alignment constants:");

    DataVector<AlignPar> * alignParList = m_alignModuleTool->alignParList1D();

    // compute alignment corrections (translations in mm and rotations in rad) and their variances
    for(int i=0; i<size; i++) {

      double param = delta[i];
      double err = sqrt(2.*std::fabs(errSq[i]));

      int idof = m_activeIndices[i];
      AlignPar * alignPar=(*alignParList)[idof];

      // undo the sigma scaling
      double sigma = alignPar->sigma();

      param *= sigma;
      err *= sigma;

      // undo normalization scaling of error
      if(m_scaleMatrix && m_scale>0.)
        err /= sqrt(m_scale);

      ATH_MSG_DEBUG(i <<" : "<< param << " +/- "<< err);
      ATH_MSG_DEBUG("cov("<<i<<")="<<errSq[i]<<", sigma: "<<sigma);
      ATH_MSG_DEBUG("init par: "<<alignPar->initPar());
      alignPar->setPar(param, err);
      ATH_MSG_DEBUG("set param to "<<param<<" for alignPar "<<alignPar);
      ATH_MSG_DEBUG(*(*alignParList)[idof]);
    }

    if(m_logStream) {
      printGlobalSolution(*m_logStream, cov);

      // norm of first derivative
      double norm1st = dChi2->norm();
      if(m_scaleMatrix && m_scale>0.) // undo normalization scaling
        norm1st *= m_scale;
      *m_logStream<<"norm of first derivative :            "<<norm1st<<std::endl;

      if(d2Chi2) {
        // distance to solution
        double dist = ( (*d2Chi2) * deltafull + (*dChi2) ).norm();
        if(m_scaleMatrix && m_scale>0.) // undo normalization scaling
          dist *= m_scale;
        *m_logStream<<"distance to solution :                "<<dist<<std::endl;

        // calculate chi2 of the alignment change
        double chi2 = delta * (*d2Chi2) * delta * .5;
        if(m_scaleMatrix && m_scale>0.) // undo normalization scaling
          chi2 *= m_scale;
        *m_logStream<<"delta(chi2) of the alignment change : "<<chi2<<" / "<<size<<std::endl;
      }
    }

    delete cov;
  }

  //________________________________________________________________________
  int MatrixTool::solveSparseEigen()
  {
    ATH_MSG_INFO("solving Global using SparseEigen");
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"**************  solving using Global method  ****************"<<std::endl;
      *m_logStream<<"**************      using SparseEigen        ****************"<<std::endl;
      *m_logStream<<"*************************************************************"<<std::endl;
    }

    // start measuring time
    clock_t starttime = clock();

    DataVector<AlignPar> * alignParList = m_alignModuleTool->alignParList1D();

    AlSpaMat * ABetterMat = 0;
    bool isCopy = false;
    if ( dynamic_cast<AlSymMat*>(m_bigmatrix) ) {
      ATH_MSG_INFO("Converting Matrix Format for fast solving");
      ABetterMat = new AlSpaMat(*(dynamic_cast<AlSymMat*>(m_bigmatrix)));
      isCopy = true;
    }
    else if ( dynamic_cast<AlSpaMat*>(m_bigmatrix) ) {
      ATH_MSG_INFO("Matrix format native to the fast solving");
      ABetterMat = (dynamic_cast<AlSpaMat*>(m_bigmatrix));
    }
    else {
      ATH_MSG_ERROR("Cannot cast to neither AlSymMat nor AlSpaMat");
      return 0;
    }

    ATH_MSG_DEBUG("checking active indices");

    // use const matrix when checking for non-zero elements to avoid
    // filling of the whole matrix
    const AlSpaMat * chkMatrix = ABetterMat;

    AlSpaMat * aBetterMat = new AlSpaMat(m_aNDoF);
    AlVec    * aBetterVec = new AlVec(m_aNDoF);

      
    for (int iActive=0;iActive<m_aNDoF;iActive++) {
      int i = m_activeIndices[iActive];
      (*aBetterVec)[iActive] = (*m_bigvector)[i];
      for (int jActive=0;jActive<m_aNDoF;jActive++) {
        int j = m_activeIndices[jActive];
        // only fill if non-zero !!!
        if ( (*chkMatrix)[iActive][jActive] != 0. )
          (*aBetterMat)[iActive][jActive]=(*ABetterMat)[i][j];
      }
    }

    // store original vector for cross-checks
    AlVec origVec(*aBetterVec);

    ATH_MSG_DEBUG("running the solving");

    // solve
    int info = (*aBetterMat).SolveWithEigen(*aBetterVec);

    if(info == 0) {
      ATH_MSG_INFO("SolveWithEigen solving OK");
      if(m_logStream)
        *m_logStream<<"SolveWithEigen solving OK."<<std::endl;
    }
    else {
      msg(MSG::ERROR)<<"SolveWithEigen error code (0 if OK) = "<<info<<endreq;
      if(m_logStream)
        *m_logStream<<"SolveWithEigen error code (0 if OK) = "<<info<<std::endl;
    }

    if( isCopy )
      delete ABetterMat;
    ABetterMat = 0;

    // stop measuring time
    clock_t stoptime = clock();
    double totaltime = (stoptime-starttime)/double(CLOCKS_PER_SEC);
    ATH_MSG_INFO("Time spent in SolveWithEigen: "<<totaltime<<" s");

    ATH_MSG_DEBUG("Alignment constants:");
    // compute alignment corrections (translations in mm and rotations in rad)
    // for solveSparseEigen variances are not calculated
    for(int i=0; i<m_aNDoF; i++) {

      double param = -(*aBetterVec)[i];
      double err = 0.;

      int idof = m_activeIndices[i];
      AlignPar * alignPar=(*alignParList)[idof];

      // undo the sigma scaling
      double sigma = alignPar->sigma();
      param *= sigma;

      ATH_MSG_DEBUG(i <<" : "<< param << " +/- "<< err);
      ATH_MSG_DEBUG("sigma: "<<sigma);
      ATH_MSG_DEBUG("init par: "<<alignPar->initPar());
      alignPar->setPar(param, err);
      ATH_MSG_DEBUG("set param to "<<param<<" for alignPar "<<alignPar);
      ATH_MSG_DEBUG(*(*alignParList)[idof]);
    }

    if(m_logStream) {
      CLHEP::HepSymMatrix * cov = 0;
      printGlobalSolution(*m_logStream, cov);

      // norm of first derivative
      *m_logStream<<"norm of first derivative :            "<<origVec.norm()<<std::endl;

      // distance to solution
      double dist = ( (*aBetterMat) * (*aBetterVec) - origVec ).norm();
      *m_logStream<<"distance to solution :                "<<dist<<std::endl;

      // calculate chi2 of the alignment change
      double chi2 = (*aBetterVec) * (*aBetterMat) * (*aBetterVec) * .5;
      *m_logStream<<"delta(chi2) of the alignment change : "<<chi2<<" / "<<m_aNDoF<<std::endl;

      // time spent here
      *m_logStream<<"time spent in solve :                 "<<totaltime<<" s"<<std::endl;
    }

    delete aBetterMat;
    delete aBetterVec;

    // need to do this since success return value from Lapack is 0
    // and from SolveWithEigen() it is 1
    if (info==0)
      info = 1;

    return info;
  }

  //________________________________________________________________________
  void MatrixTool::writeHitmap()
  {
    ATH_MSG_INFO("writing the hitmap to file");

    const AlignModuleList * moduleList = m_alignModuleTool->alignModules1D();
    int nModules = moduleList->size();

    AlMat hitmap(nModules,2);
    AlignModuleList::const_iterator imod     = moduleList->begin();
    AlignModuleList::const_iterator imod_end = moduleList->end();
    int index(0);
    for(; imod != imod_end; ++imod) {
      AlignModule * module = *imod;
      hitmap[index][0] = module->nHits();
      hitmap[index][1] = module->nTracks();
      index++;
    }

    // Set Path for the hitmap matrix
    hitmap.SetPathBin(m_pathbin+m_prefixName);
    hitmap.SetPathTxt(m_pathtxt+m_prefixName);

    StatusCode sc = hitmap.Write("hitmap.bin",true); // write the hitmap matrix

    if (sc!=StatusCode::SUCCESS)
      ATH_MSG_ERROR("Problem writing hitmap matrix");

    if (m_writeHitmapTxt) {
      sc = hitmap.Write("hitmap.txt", false, 0);
      if (sc!=StatusCode::SUCCESS)
        ATH_MSG_ERROR("Problem writing hitmap matrix to text file");
    }

    ATH_MSG_DEBUG("hitmap written to: "<< m_pathbin+m_prefixName <<"hitmap.bin (.txt)");
  }

  //________________________________________________________________________
  void MatrixTool::readHitmaps()
  {
    ATH_MSG_INFO("read hitmaps from files");

    const AlignModuleList * moduleList = m_alignModuleTool->alignModules1D();
    int nModules = moduleList->size();

    AlMat hitmap(nModules,2);
    int nFiles = (int)m_inputHitmapFiles.size();
    for(int imap=0;imap<nFiles; imap++) {
      AlMat nextmap(nModules,2);

      ATH_MSG_INFO("Reading hitmap "<<imap<<" from file "<<m_inputHitmapFiles[imap]);

      if(nextmap.ReadScalaPack(m_inputHitmapFiles[imap]).isFailure()) {
        ATH_MSG_WARNING("Problem reading hitmap from \'"<<m_inputHitmapFiles[imap]<<"\'. Skipping.");
        continue;
      }

      if(nextmap.nrow()!=nModules || nextmap.ncol()!=2) {
        ATH_MSG_WARNING("Matrix in file \'"<<m_inputHitmapFiles[imap]<<"\' has wrong size ("
            <<nextmap.nrow()<<" x "<<nextmap.ncol()<<"), should be ("<<nModules<<" x 2). Skipping.");
        continue;
      }

      hitmap += nextmap;
    }

    AlignModuleList::const_iterator imod     = moduleList->begin();
    AlignModuleList::const_iterator imod_end = moduleList->end();
    int index = 0;
    int totalhits = 0;
    for(; imod != imod_end; ++imod) {
      AlignModule * module = *imod;
      //if ((int)hitmap[index][0]!=(int)module->identify().get_identifier32().get_compact()) ATH_MSG_ERROR("bad module identifier");
      //module->setIdentifier((Identifier)hitmap[index][0]);
      module->setNHits((int)hitmap[index][0]);
      module->setNTracks((int)hitmap[index][1]);
      totalhits += (int)hitmap[index][0];
      index++;
    }

    m_nHits = totalhits;
    m_nTracks = 0;
    m_nMeasurements = 0;

    ATH_MSG_INFO("Hitmap accumulated from "<<nFiles<<" files with total of "<<totalhits<<" hits.");
  }

} // end of namespace

