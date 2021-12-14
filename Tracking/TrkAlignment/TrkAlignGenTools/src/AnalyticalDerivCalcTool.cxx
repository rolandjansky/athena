/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/AlignResidualType.h"

#include "TrkAlignGenTools/AnalyticalDerivCalcTool.h"

#include "TrkAlgebraUtils/AlSymMat.h"

#include <string>

namespace Trk {

  //________________________________________________________________________
  AnalyticalDerivCalcTool::AnalyticalDerivCalcTool(const std::string & type,
                                                   const std::string & name,
                                                   const IInterface  * parent)
    : AthAlgTool(type,name,parent)
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_idHelper(nullptr)
    , m_measTypeIdHelper(nullptr)
    , m_firstEvent(true)
    , m_residualType(HitOnly)
    , m_residualTypeSet(false)
    , m_storeDerivatives(false)
  {
    declareInterface<IDerivCalcTool>(this);

    declareProperty("AlignModuleTool",   m_alignModuleTool);

    declareProperty("UseLocalSetting", m_useLocalSetting=false, "use local setup for the covariance matrix of the track");

    // Use constant errors for each sub-detector such that it's equivalent
    // to minimize residual distance instead of minizing residual pull.
    // This is only applied if m_useLocalSetting==true
    declareProperty("UseIntrinsicPixelError", m_useIntrinsicPixelErrors, "use intrinsic errors for Pixel");
    declareProperty("UseIntrinsicSCTError",   m_useIntrinsicSCTErrors,   "use intrinsic errors for SCT");
    declareProperty("UseIntrinsicTRTError",   m_useIntrinsicTRTErrors,   "use intrinsic errors for TRT");

    declareProperty("StoreDerivatives", m_storeDerivatives, "store derivatives dr/da on AlignTSOS to be filled into ntuple");

    m_logStream = nullptr;
  }

  //________________________________________________________________________
  AnalyticalDerivCalcTool::~AnalyticalDerivCalcTool()
  {}

  //________________________________________________________________________
  StatusCode AnalyticalDerivCalcTool::initialize()
  {
    if (m_alignModuleTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_alignModuleTool << endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved " << m_alignModuleTool);

    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endmsg;
      return StatusCode::FAILURE;
    }
    m_measTypeIdHelper = new MeasurementTypeID(m_idHelper);

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode AnalyticalDerivCalcTool::finalize()
  {
    ATH_MSG_DEBUG("in AnalyticalDerivCalcTool::finalize()");
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool AnalyticalDerivCalcTool::setDerivatives(AlignTrack * alignTrack)
  {
    if(!m_residualTypeSet)
      checkResidualType(alignTrack);

    // create table of modules for checking whether the module
    // is hit by the track or not
    int nModules = m_alignModuleTool->alignModules1D()->size();
    std::vector<bool> hitModules(nModules,false);

    // loop over AlignTSOSCollection, 
    // find modules that are in the AlignModuleList,
    std::vector<const AlignModule *> alignModules;    
    AlignTSOSCollection::const_iterator atsosItr = alignTrack->firstAtsos();
    for (; atsosItr != alignTrack->lastAtsos(); ++atsosItr) {      
      const AlignModule * module=(*atsosItr)->module();
      if (module)
        ATH_MSG_DEBUG("have ATSOS for module "<<module->identify());
      else
        ATH_MSG_DEBUG("no module!");

      if (!(*atsosItr)->isValid() || !module)
        continue;

      // if the module is not yet in the list for this track, add it
      if(!hitModules[module->identifyHash()]) {
        hitModules[module->identifyHash()] = true;
        alignModules.push_back(module);
      }
    }

    // Determine derivatives from shifting these modules
    std::vector<AlignModuleDerivatives> * derivatives = new std::vector<AlignModuleDerivatives>;
    std::vector<const AlignModule *>::const_iterator moduleIt = alignModules.begin();
    for ( ; moduleIt!=alignModules.end(); ++moduleIt) {
      std::vector<Amg::VectorX> deriv_vec = getDerivatives(alignTrack,*moduleIt);
      derivatives->push_back(make_pair(*moduleIt,deriv_vec));
    }

    // alignTrack takes care of deleting the derivatives
    ATH_MSG_DEBUG("setting matrix derivatives");
    alignTrack->setDerivatives(derivatives);

    ATH_MSG_DEBUG("returning from setDerivatives");

    return true;
  }
    
  //________________________________________________________________________
  bool AnalyticalDerivCalcTool::setResidualCovMatrix(AlignTrack * alignTrack) const
  {
    if (m_firstEvent) {
      if(m_logStream) {
        *m_logStream<<"*************************************************************"<<std::endl;
        *m_logStream<<"*************************************************************"<<std::endl;
        *m_logStream<<"***                                                     *****"<<std::endl;

        if(m_useLocalSetting)
          *m_logStream<<"***           Running full LOCAL Chi2 method            *****"<<std::endl;
        else
          *m_logStream<<"***           Running full GLOBAL Chi2 method           *****"<<std::endl;

        *m_logStream<<"***            using analytical derivatives             *****"<<std::endl;

        if(m_useLocalSetting) {
          std::string resType = "HitOnly ";
          if(m_residualType==Unbiased)
            resType = "Unbiased";
          *m_logStream<<"***            with residual type: "<<resType<<"             *****"<<std::endl;
        }

        *m_logStream<<"***                                                     *****"<<std::endl;
        *m_logStream<<"*************************************************************"<<std::endl;
      }
      m_firstEvent = false;
    }

    // get inverse local error matrix of the track
    const Amg::MatrixX * Vinv = alignTrack->localErrorMatrixInv();
    if(!checkValidity(*Vinv)) {
      ATH_MSG_WARNING("Inverse local error matrix is invalid, skipping the track");
      return false;
    }
    ATH_MSG_DEBUG("V inverse (diagonal only):");
    if (msgLvl(MSG::DEBUG)) {
      for (int i=0;i<Vinv->rows();i++) msg()<<(*Vinv)(i,i)<<" ";
      msg()<<endmsg;
    } 

    // ========================
    // setup local chi2 method
    // ========================
    if (m_useLocalSetting) {
      ATH_MSG_DEBUG("setting Residual covariance matrix for local method");

      // for local method we only need the Vinv
      const Amg::MatrixX * Vinv = alignTrack->localErrorMatrixInv();
      Amg::MatrixX * W = new Amg::MatrixX(*Vinv);

      // if we want to ignore the real errors of measurements for a particular subretector
      // (we should add something similar for Muon detectors)
      if(m_useIntrinsicPixelErrors || m_useIntrinsicSCTErrors || m_useIntrinsicTRTErrors) {

        if(m_useIntrinsicPixelErrors)
          ATH_MSG_DEBUG("Ignoring measured errors for Pixel clusters, using intrinsic errors");
        if(m_useIntrinsicSCTErrors)
          ATH_MSG_DEBUG("Ignoring measured errors for SCT clusters, using intrinsic errors");
        if(m_useIntrinsicTRTErrors)
          ATH_MSG_DEBUG("Ignoring measured errors for TRT clusters, using intrinsic errors");

        int index(0);
        AlignTSOSCollection::const_iterator itAtsos     = alignTrack->firstAtsos();
        AlignTSOSCollection::const_iterator itAtsos_end = alignTrack->lastAtsos();
        for ( ; itAtsos != itAtsos_end; ++itAtsos) {
          const AlignTSOS * atsos = *itAtsos;
          if (!atsos->isValid())
            continue;

          if(m_useIntrinsicPixelErrors && atsos->measType() == Trk::TrackState::Pixel) {
            (*W)(index,index) = 1./(0.05*0.05/12);
            (*W)(index+1,index+1) = 1./(0.4*0.4/12);
          }
          else if(m_useIntrinsicSCTErrors && atsos->measType() == Trk::TrackState::SCT)
            (*W)(index,index) = 1./(0.1*0.1/12);
          else if(m_useIntrinsicTRTErrors && atsos->measType() == Trk::TrackState::TRT)
            (*W)(index,index) = 1./(0.5*0.5/12);

          index += atsos->nResDim();
        }
      }

      ATH_MSG_DEBUG("setting local weight matrix W ( "<<W->rows()<<" x "<<W->cols()<<" ) (diagonal only):");
      if (msgLvl(MSG::DEBUG)) {
        for (int i=0;i<W->rows();i++) msg()<<(*W)(i,i)<<" ";
        msg()<<endmsg;
      } 
      alignTrack->setWeightMatrix(W);

      Amg::MatrixX * W1st = new Amg::MatrixX(*W);
      alignTrack->setWeightMatrixFirstDeriv(W1st);

      return true;
    }

    // ========================
    // setup global chi2 method
    // ========================

    // get the ingredients for calculating R = V - Q    
    const Amg::MatrixX * V = alignTrack->localErrorMatrix();
    ATH_MSG_DEBUG("V ( "<<V->rows()<<" x "<<V->cols()<<" ) (diagonal only):");
    if (msgLvl(MSG::DEBUG)) {
      for (int i=0;i<V->rows();i++) msg()<<(*V)(i,i)<<" ";
      msg()<<endmsg;
    } 

    const int outputdim = alignTrack->nAlignTSOSMeas();

    Amg::MatrixX Q(outputdim, outputdim); //symmetric matrix
    if (!getTrkParamCovMatrix(alignTrack, Q))
      return false;
    ATH_MSG_DEBUG("Q ( "<<Q.rows()<<" x "<<Q.cols()<<" )");
    ATH_MSG_DEBUG("Q: "<<Q);

    // calculate R
    const Amg::MatrixX R = (*V) - Q;
    if (!checkValidity(R)) {
      ATH_MSG_WARNING("Matrix R = V - HCH is invalid, skipping the track");
      return false;
    }
    ATH_MSG_DEBUG("R ( "<<R.rows()<<" x "<<R.cols()<<" )");
    ATH_MSG_DEBUG("R: "<<R);

    // calculate weight matrix and set it
    Amg::MatrixX * W = new Amg::MatrixX(outputdim,outputdim); //symmetric matrix
    *W = ((*Vinv) * R * (*Vinv));
    
    if (!checkValidity(*W)) {
      ATH_MSG_DEBUG("Weight matrix is invalid, skipping the track");
      delete W;
      return false;
    }
    ATH_MSG_DEBUG("setting weight: "<<(*W));
    alignTrack->setWeightMatrix(W);

    // for 1st derivatives the weight matrix is just the V^-1
    // it has been chacked above so no need to do it again
    Amg::MatrixX * W1st = new Amg::MatrixX(*Vinv); //symmetric matrix
    ATH_MSG_DEBUG("setting weight for 1st derivatives (diagonal only): ");
    if (msgLvl(MSG::DEBUG)) {
      for (int i=0;i<W1st->rows();i++) msg()<<(*W1st)(i,i)<<" ";
      msg()<<endmsg;
    }
    alignTrack->setWeightMatrixFirstDeriv(W1st);

    return true;
  }

  //________________________________________________________________________
  bool AnalyticalDerivCalcTool::getTrkParamCovMatrix(const AlignTrack * alignTrack, Amg::MatrixX & Q /*symmetric matrix*/) const
  {
       
    // get derivative matrices from fitter
    const Amg::MatrixX * H0 = alignTrack->derivativeMatrix(); 
    const Amg::MatrixX * C  = alignTrack->fullCovarianceMatrix(); //symmetric matrix

    // H0 is a q0 x p matrix, 
    // C is a p x p matrix,
    // q0 = number of tsos measurements
    // p = number perigee params+2*nscat+nbrem

    if( H0==nullptr || C==nullptr) {
      ATH_MSG_ERROR("no derivative matrix or cov matrix stored on AlignTrack!"
                    << "This should have been done in AlignTrackPreProcessor!"
                    << H0 << " " << C );
      return false;
    }

    ATH_MSG_DEBUG("H0 ( "<<H0->rows()<<" x "<<H0->cols()<<" )");
    ATH_MSG_DEBUG("H0: "<<(*H0));
    ATH_MSG_DEBUG("C ( "<<C->rows()<<" x "<<C->cols()<<" )");
    ATH_MSG_DEBUG("C: "<<(*C));


    int    Csize(C->rows());
    Amg::MatrixX CC(*C);         // take a copy of C //symmetric matrix
    int    ierr(0);
    bool   amendC( !(alignTrack->refitD0() && alignTrack->refitZ0() &&
                     alignTrack->refitPhi() && alignTrack->refitTheta() &&
                     alignTrack->refitQovP()) );

    // amendC = true;       // FUDGE!

    if( amendC ) {
      // test with AlSymMat:
      // build AlSymMat instance from 
      AlSymMat*     CA = new AlSymMat(Csize);

      // take a copy of C:
      for( int ii=0; ii<Csize; ++ii ) {
        for( int jj=ii; jj<Csize; ++jj ) {
          CA->elemr(ii,jj) = (*C)(ii,jj);
        }
      }

      // first inversion:
      ierr = CA->invert();
      if( ierr ) {
        ATH_MSG_ERROR("First inversion of matrix CA failed with LAPACK status flag " << ierr);
        return false;
      } else {
        // disable selected track parametrs (remove from refit).
        // It is your duty to assure that corresponding constraints have been imposed!
        if( !(alignTrack->refitD0()) )    {
          for(int ii=0; ii<(Csize); ++ii) CA->elemr(0,ii)=0.0;   // should do the trick. It is a CLHEP::HepSymMatrix! 
          CA->elemr(0,0)=1.0;
        }
        if( !(alignTrack->refitZ0()) )    {
          for(int ii=0; ii<(Csize); ++ii) CA->elemr(1,ii)=0.0;   // should do the trick. It is a CLHEP::HepSymMatrix! 
          CA->elemr(1,1)=1.0;
        }
        if( !(alignTrack->refitPhi()) )   {
          for(int ii=0; ii<(Csize); ++ii) CA->elemr(2,ii)=0.0;   // should do the trick. It is a CLHEP::HepSymMatrix! 
          CA->elemr(2,2)=1.0;
        }
        if( !(alignTrack->refitTheta()) ) {
          for(int ii=0; ii<(Csize); ++ii) CA->elemr(3,ii)=0.0;   // should do the trick. It is a CLHEP::HepSymMatrix!
          CA->elemr(3,3)=1.0;
        } 
        if( !(alignTrack->refitQovP()) )  {
          for(int ii=0; ii<(Csize); ++ii) CA->elemr(4,ii)=0.0;   // should do the trick. It is a CLHEP::HepSymMatrix! 
          CA->elemr(4,4)=1.0;
        }


  // invert back:
        ierr = CA->invert();
        if( ierr ) {
          ATH_MSG_ERROR("Second inversion of matrix CA failed with LAPACK status flag " << ierr);
          return false;
        }

        // copy back to CC:
        for( int ii=0; ii<Csize; ++ii ) {
          for( int jj=ii; jj<Csize; ++jj ) {
            CC(ii,jj) = CA->elemc(ii,jj);
          }
        }

        // clear the disabled rows/collumns
        if( !(alignTrack->refitD0()) )    for(int ii=0; ii<(Csize); ++ii){ CC(0,ii)=0.0; CC(ii,0)=0.0;  };   // should do the trick. 
        if( !(alignTrack->refitZ0()) )    for(int ii=0; ii<(Csize); ++ii){ CC(1,ii)=0.0; CC(ii,1)=0.0;  };   // should do the trick. 
        if( !(alignTrack->refitPhi()) )   for(int ii=0; ii<(Csize); ++ii){ CC(2,ii)=0.0; CC(ii,2)=0.0;  };   // should do the trick.  
        if( !(alignTrack->refitTheta()) ) for(int ii=0; ii<(Csize); ++ii){ CC(3,ii)=0.0; CC(ii,3)=0.0;  };   // should do the trick. 
        if( !(alignTrack->refitQovP()) )  for(int ii=0; ii<(Csize); ++ii){ CC(4,ii)=0.0; CC(ii,4)=0.0;  };   // should do the trick.  


      }

      //garbage collection:
      delete CA;
    }



    int nMeas  = H0->rows();
    int nAtsos = alignTrack->nAlignTSOSMeas();
    ATH_MSG_DEBUG("nMeas: "<<nMeas);

    //CLHEP::HepSymMatrix HCH(nMeas,0);
    Amg::MatrixX HCH(nAtsos,nAtsos);
    HCH = CC.similarity( *H0 );

    ATH_MSG_DEBUG("HCH ( "<<HCH.rows()<<" x "<<HCH.cols()<<" )");
    ATH_MSG_DEBUG("HCH: "<<HCH);

    //
    // get indices of HCH matrix corresponding to alignTSOSs in alignTrack
    const AlignTSOSCollection * alignTSOSCollection = alignTrack->alignTSOSCollection();

    std::vector<int> matrixIndices(nMeas);

    int imeas(-1);
    for (const TrackStateOnSurface* tsos : *alignTrack->trackStateOnSurfaces()){

      ATH_MSG_DEBUG("tsos:  "<<tsos->dumpType());

      // get tsos and make sure it is a RIO_OnTrack
      if (tsos->type(TrackStateOnSurface::Outlier))
        continue;

      // RIO
      if (!tsos->type(TrackStateOnSurface::Scatterer)) {
        ATH_MSG_DEBUG("not scatterer, trying rio");

        const MeasurementBase      * mesb = tsos->measurementOnTrack();
        const RIO_OnTrack          * rio  = dynamic_cast<const RIO_OnTrack *>(mesb);
        const CompetingRIOsOnTrack * crio = dynamic_cast<const CompetingRIOsOnTrack *>(mesb);
        if (!rio && crio)
          rio=&crio->rioOnTrack(0);

        if (rio==nullptr)
          continue;

        ++imeas;
        matrixIndices[imeas]=-1;

        Identifier tsosId = rio->identify();
        ATH_MSG_DEBUG("have tsos with Id "<<tsosId);

        // get matching alignTSOS and store track index in goodMatrixIndices      
        int iameas(0);
        for (const AlignTSOS* atsos : *alignTSOSCollection) {

          if (!atsos->isValid())
            continue;

          if (atsos->type(TrackStateOnSurface::Scatterer))
            ATH_MSG_ERROR("can't use scatterers on AlignTrack yet for analytical derivatives!");

          const RIO_OnTrack * atsos_rio = atsos->rio();
          if (atsos_rio) {
//            ATH_MSG_DEBUG("tsosId / atsosId  :  "<<tsosId<<" / "<<atsos_rio->identify());

            if (atsos_rio->identify()==tsosId) {
              matrixIndices[imeas]=iameas;
              ATH_MSG_DEBUG("matrixIndices["<<imeas<<"]="<<iameas);

              // for Pixel we have two measurements
              if (atsos->nResDim()>1)
              {
                imeas++;
                iameas++;
                matrixIndices[imeas]=iameas;
                ATH_MSG_DEBUG("matrixIndices["<<imeas<<"]="<<iameas);
              }
              break;
            }
            // for Pixel we have two measurements
            else if(atsos->nResDim()>1)
              iameas++;
          }
          iameas++;
        }

        // even when the Pixel is not aligned we have to take into account
        // that it has two measurements
        if(matrixIndices[imeas]==-1 && m_measTypeIdHelper->defineType(mesb) == TrackState::Pixel) {
          imeas++;
          matrixIndices[imeas]=-1;
          ATH_MSG_DEBUG("matrixIndices["<<imeas<<"]="<<matrixIndices[imeas]);
        }

      } // end tsos==rio

    } // end loop over track tsos

    // strip elements in the HCH matrix which don't correspond
    // to AlignTSOS on alignTrack
    ATH_MSG_DEBUG("Filling the Q matrix:");
    for (int k=0;k<nMeas;k++) {

      int iameas=matrixIndices[k];
      if (iameas==-1)
        continue;

      for (int l=0;l<nMeas;l++) {
        int jameas=matrixIndices[l];
        if (jameas==-1)
          continue;

        Q(iameas,jameas) = HCH(k,l);

      }
    }

    ATH_MSG_DEBUG("before check Q ( "<<Q.rows()<<" x "<<Q.cols()<<" )");
    ATH_MSG_DEBUG("before check Q: "<<Q);

    if(!checkValidity(Q)) {
      ATH_MSG_DEBUG("Matrix Q = HCH is invalid, skipping the track");
      return false;
    }

    return true;
  }

  //________________________________________________________________________
  // is this method used used anywhere? maybe we should remove it
  bool AnalyticalDerivCalcTool::getMeasErrorMatrix(const AlignTrack* alignTrack, Amg::MatrixX& V /*Symmetric Matrix*/) const
  {
    int index(0);
    AlignTSOSCollection::const_iterator itAtsos=alignTrack->firstAtsos();
    for (; itAtsos != alignTrack->lastAtsos(); ++itAtsos) {      

      std::vector<Residual>::const_iterator itRes=(**itAtsos).firstResidual();
      for (; itRes!=(**itAtsos).lastResidual(); ++itRes,index++) {
        
        V(index,index) = itRes->errSq();
      }
    }
    return checkValidity(V);
  }
  
  //________________________________________________________________________
  bool AnalyticalDerivCalcTool::checkValidity(const Amg::MatrixX& R /*symmetric matrix*/) const
  {
    // perform some sort of sanity check. depending on how many hits
    // on the track we use, R can have zero determinant, but it
    // should definitely not be negative. for now, we'll just check
    // that all diagonal elements are positive and that all correlation
    // coefficients are within bounds

    bool Risvalid(true);
    const double epsilon=1e-10;
    for( int irow=0; irow<R.rows(); ++irow) {

      Risvalid = Risvalid && R(irow,irow)>0;
      if ( msgLvl(MSG::DEBUG) ) {
        if( !(R(irow,irow)>0) )
          msg(MSG::DEBUG) << "matrix invalid: (" << irow << "," << irow<<") = " << R(irow,irow) << endmsg;
      }
      else if (!Risvalid)
        break;

      for(int icol=0; icol<=irow; ++icol) {
        // this one must be true if everything else succeeded
        double Rcorr = R(irow,icol)/sqrt(R(irow,irow)*R(icol,icol));
        if( Rcorr+epsilon<-1 || Rcorr-epsilon>1 )
        {
          Risvalid = false;
          if (msgLvl(MSG::DEBUG))
            ATH_MSG_DEBUG("matrix corr invalid for (" << irow << "," << icol << ") Rcorr = " << Rcorr);
          else
            break;
        }
      }
    }

    if( !Risvalid ) {
      ATH_MSG_WARNING("Checked matrix is invalid.");
      ATH_MSG_WARNING("R: \n"<<R);
    }
    return Risvalid;
  }

  //________________________________________________________________________
  std::vector<Amg::VectorX> AnalyticalDerivCalcTool::getDerivatives(AlignTrack * alignTrack, const AlignModule * module)
  {
    // module-specific transforms
    Amg::Transform3D globalFrameToAlignFrame = module->globalFrameToAlignFrame();
    ATH_MSG_DEBUG("globalFrameToAlignFrame: ");
    ATH_MSG_DEBUG(globalFrameToAlignFrame(0,0)<<" "<<
                  globalFrameToAlignFrame(0,1)<<" "<<
                  globalFrameToAlignFrame(0,2));
    ATH_MSG_DEBUG(globalFrameToAlignFrame(1,0)<<" "<<
                  globalFrameToAlignFrame(1,1)<<" "<<
                  globalFrameToAlignFrame(1,2));
    ATH_MSG_DEBUG(globalFrameToAlignFrame(2,0)<<" "<<
                  globalFrameToAlignFrame(2,1)<<" "<<
                  globalFrameToAlignFrame(2,2));

    Amg::RotationMatrix3D globalToAlignFrameRotation = module->globalToAlignFrameRotation();
    ATH_MSG_DEBUG("globalToAlignFrameRotation: ");
    ATH_MSG_DEBUG(globalToAlignFrameRotation(0,0)<<" "<<
                  globalToAlignFrameRotation(0,1)<<" "<<
                  globalToAlignFrameRotation(0,2));
    ATH_MSG_DEBUG(globalToAlignFrameRotation(1,0)<<" "<<
                  globalToAlignFrameRotation(1,1)<<" "<<
                  globalToAlignFrameRotation(1,2));
    ATH_MSG_DEBUG(globalToAlignFrameRotation(2,0)<<" "<<
                  globalToAlignFrameRotation(2,1)<<" "<<
                  globalToAlignFrameRotation(2,2));

    DataVector<AlignPar> * alignPars = m_alignModuleTool->getAlignPars(module);
    const int nAlignPar = alignPars->size();

    //Create derivatives storage vector and initialise them ro zero     
    std::vector<Amg::VectorX> derivatives( nAlignPar+3 , Amg::VectorX(alignTrack->nAlignTSOSMeas()));
    for(int i(0); i<nAlignPar+3; ++i) derivatives[i].setZero();

    int imeas(0);
    AlignTSOSCollection::iterator iatsos = alignTrack->firstAtsos();
    for (; iatsos != alignTrack->lastAtsos(); ++iatsos) {
      
      AlignTSOS * alignTSOS = *iatsos;
      if (!alignTSOS->isValid() || nullptr==alignTSOS->module()) 
        continue;

      // we only calculate the derivatives if the AlignTSOS belongs to the align module
      int nResDim = alignTSOS->nResDim();
      if (alignTSOS->module() != module) {
        imeas += nResDim;
        continue;
      }

      // derivatives to be stored on the AlignTSOS
      std::vector<Amg::VectorX> * atsosDerivs = nullptr;
      std::vector<Amg::VectorX> * atsosDerVtx = nullptr;
      if (m_storeDerivatives) {
        atsosDerivs = new std::vector<Amg::VectorX>(nResDim,Amg::VectorX(nAlignPar));
        atsosDerVtx = new std::vector<Amg::VectorX>(nResDim,Amg::VectorX(3));
        ATH_MSG_DEBUG("nResDim = "<<nResDim<<"       vector size is    "<<atsosDerivs->size());
        ATH_MSG_DEBUG("nAlignPar = "<<nAlignPar<<"   CLHEP::HepVector size is "<<atsosDerivs->at(0).rows());
      }

      // Get the rotation to the frame in which the residual is
      // defined. In this frame the trkdistance is the x-coordinate.    
      
      const TrackParameters * mtp  = alignTSOS->trackParameters();
      if (!mtp || !(mtp->covariance()) ) continue;
      Amg::RotationMatrix3D localToGlobalRotation =    mtp->measurementFrame();
       

      ATH_MSG_DEBUG( "localToGlobalRotation:");
      ATH_MSG_DEBUG(localToGlobalRotation(0,0) << " " <<
                    localToGlobalRotation(0,1) << " " <<
                    localToGlobalRotation(0,2));
      ATH_MSG_DEBUG(localToGlobalRotation(1,0) << " " <<
                    localToGlobalRotation(1,1) << " " <<
                    localToGlobalRotation(1,2));
      ATH_MSG_DEBUG(localToGlobalRotation(2,0) << " " <<
                    localToGlobalRotation(2,1) << " " <<
                    localToGlobalRotation(2,2));

      if(double alphastrip=alignTSOS->alphaStrip()) {
        ATH_MSG_DEBUG( "applying fanout rotation : " << alphastrip );
      	localToGlobalRotation = localToGlobalRotation * Amg::AngleAxis3D(alphastrip, Amg::Vector3D(0.,0.,1.));
        ATH_MSG_DEBUG( "localToGlobalRotation * fanout_rotation:");
        ATH_MSG_DEBUG(localToGlobalRotation(0,0) << " " <<
                      localToGlobalRotation(0,1) << " " <<
                      localToGlobalRotation(0,2));
        ATH_MSG_DEBUG(localToGlobalRotation(1,0) << " " <<
                      localToGlobalRotation(1,1) << " " <<
                      localToGlobalRotation(1,2));
        ATH_MSG_DEBUG(localToGlobalRotation(2,0) << " " <<
                      localToGlobalRotation(2,1) << " " <<
                      localToGlobalRotation(2,2));
      }

      // get the position of the track in the alignmentframe.
      Amg::Vector3D refPos = globalFrameToAlignFrame * alignTSOS->trackParameters()->position();
      ATH_MSG_DEBUG("refPos: "<<refPos);

          
      const Amg::RotationMatrix3D R = globalToAlignFrameRotation * localToGlobalRotation;
      ATH_MSG_DEBUG("R:");
      ATH_MSG_DEBUG(R(0,0) << " " << R(0,1) << " " << R(0,2));
      ATH_MSG_DEBUG(R(1,0) << " " << R(1,1) << " " << R(1,2));
      ATH_MSG_DEBUG(R(2,0) << " " << R(2,1) << " " << R(2,2));

      // In the SCT measurement frame:
      // x --> perpendicular to strips in wafer plane
      // y --> along strips in wafer plane
      // z --> perpendicular to wafer plane

      // In the TRT measurement frame:
      // x --> perpendicular to track and straw
      // y --> along straw wire
      // z --> perpendicular to x and y (but not parallel to track!)

      // now 'correct' for the track angle in the measurement frame.
      const TrackParameters * trkpars = nullptr;
      if(m_residualType == HitOnly) {
        ATH_MSG_DEBUG("using BIASED track parameters");
        trkpars = alignTSOS->trackParameters();
      }
      else {
        ATH_MSG_DEBUG("using UNBIASED track parameters");
        trkpars = alignTSOS->unbiasedTrackPars();
      }
      
      
      Amg::Vector3D trackdir = localToGlobalRotation.inverse()  *  trkpars->momentum();
      ATH_MSG_DEBUG( "trackdir " << trackdir[0] << " " << trackdir[1] << " " << trackdir[2]);

      // for 1-dimensional measurements and Pixel-x
      ATH_MSG_DEBUG( "trackdir.z(): " << trackdir.z() );
      double cotphi_x = trackdir.x() / trackdir.z();

      // some 1D measurements are in Y direction (e.g. in CSC)
      // so we need the other angle
      if (alignTSOS->measDir() == Trk::y) 
        cotphi_x = trackdir.y() / trackdir.z();
      

      double Rxx = R(0,0) - cotphi_x * R(0,2);
      double Ryx = R(1,0) - cotphi_x * R(1,2);
      double Rzx = R(2,0) - cotphi_x * R(2,2);
      ATH_MSG_DEBUG("Rxx/Ryx/Rzx: " << Rxx << "/" << Ryx << "/" << Rzx);

      double projR[AlignModule::NTransformPar];
      projR[AlignModule::TransX] = Rxx;
      projR[AlignModule::TransY] = Ryx;
      projR[AlignModule::TransZ] = Rzx;
      projR[AlignModule::RotX] = -Ryx * refPos.z() + Rzx * refPos.y();
      projR[AlignModule::RotY] = -Rzx * refPos.x() + Rxx * refPos.z();
      projR[AlignModule::RotZ] = -Rxx * refPos.y() + Ryx * refPos.x();
      projR[AlignModule::BowX]  = 0;  
      projR[AlignModule::BowY]  = 0;    
      projR[AlignModule::BowZ]  = 0;

      /** for L16 in local stave frame 
      //Bowing Parameterised by a 2nd order polynomial   
      const double localy = refPos.y();
      // stave length in the IBL -- we will see if there a more generic way of doing this
      const double  y0y0  = 366.5*366.5;
      //  refPos.y() should be the y position along in the AlignModule Frame
      //      projR[AlignModule::BowX] = Rxx * ( localy*localy - y0y0 ) / y0y0;
      projR[AlignModule::BowX] = -Rxx * ( localy*localy ) / y0y0;   // change the bowing base
      **/
      
      /** for L11 in global frame
      //Bowing Parameterised by a 2nd order polynomial                                                                         
      const double localz = refPos.z();                                                                                         
      // stave length in the IBL -- we will see if there a more generic way of doing this                             
      const double  z0z0  = 366.5*366.5;                                                                       
      //  refPos.z() should be the z position along in the AlignModule Frame
      projR[AlignModule::BowX] = -projR[AlignModule::RotZ] / refPos.perp() * ( localz*localz - z0z0 ) / z0z0;
      **/


      /** try a generic formula:  **/
      const double localz = alignTSOS->trackParameters()->position().z(); // - globalToAlignFrameTranslation().z();  // the last term to be doublechecked!
      // stave length in the IBL -- we will see if there is a more generic way of doing this
      const double  z0z0  = 366.5*366.5;

      projR[AlignModule::BowX] = ( localz*localz - z0z0) / z0z0;    // this formula should work for both L11 ans L16, sign to be checked!


      // prepare derivatives w.r.t. the vertex position:
      Amg::Vector3D RxLoc(Rxx, Ryx, Rzx);
      Amg::Vector3D RxGlob=-1.0 * (globalToAlignFrameRotation.inverse() * RxLoc);        // to be double checked!!!
   
      for (int ipar=0; ipar<nAlignPar; ipar++) {
        const AlignPar * alignPar = (*alignPars)[ipar];
        int paramType = alignPar->paramType();
        //double sigma  = alignPar->sigma();
        ATH_MSG_DEBUG("ipar="<<ipar<<", paramType="<<paramType);
        derivatives[ipar][imeas] = projR[paramType];//*sigma;
        if (m_storeDerivatives)
          (*atsosDerivs)[0][ipar] = projR[paramType];//*sigma;
      }
      // the dr/db bit:
      for (int ipar=0; ipar<3; ipar++) {
        derivatives[nAlignPar+ipar][imeas] = RxGlob[ipar];
        if (m_storeDerivatives)   (*atsosDerVtx)[0][ipar] = RxGlob[ipar];
      }

      for (int i=0;i<nAlignPar+3;i++)
        ATH_MSG_DEBUG("derivatives["<<i<<"]["<<imeas<<"]="<<derivatives[i][imeas]);

      imeas++;

      if (nResDim>1) {
        // for Pixel the second measurement has to be corrected
        // for the second angle
        double cotphi_y = trackdir.y() / trackdir.z() ;
        double Rxy = R(0,1) - cotphi_y * R(0,2) ;
        double Ryy = R(1,1) - cotphi_y * R(1,2) ;
        double Rzy = R(2,1) - cotphi_y * R(2,2) ;
        ATH_MSG_DEBUG("Rxy/Ryy/Rzy: " << Rxy << "/" << Ryy << "/" << Rzy);

        projR[AlignModule::TransX] = Rxy;
        projR[AlignModule::TransY] = Ryy;
        projR[AlignModule::TransZ] = Rzy;
        projR[AlignModule::RotX] = -Ryy * refPos.z() + Rzy * refPos.y();
        projR[AlignModule::RotY] = -Rzy * refPos.x() + Rxy * refPos.z();
        projR[AlignModule::RotZ] = -Rxy * refPos.y() + Ryy * refPos.x();

        //Possibly could add the bowing correction  --  very weakly coupled to y residuals 
        projR[AlignModule::BowX]  = 0;  
        projR[AlignModule::BowY]  = 0; 
        projR[AlignModule::BowZ]  = 0;


        // prepare derivatives w.r.t. the vertex position:
        Amg::Vector3D RyLoc(Rxy, Ryy, Rzy);
        Amg::Vector3D RyGlob=-1.0 * (globalToAlignFrameRotation.inverse() * RyLoc);        // to be double checked!!!

        for (int ipar=0; ipar<nAlignPar; ipar++) {
          const AlignPar * alignPar = (*alignPars)[ipar];
          int paramType = alignPar->paramType();
          ATH_MSG_DEBUG("2nd dim, ipar="<<ipar<<", paramType="<<paramType);
          //double sigma=alignPar->sigma();
          derivatives[ipar][imeas] = projR[paramType];//*sigma;
          if (m_storeDerivatives)
            (*atsosDerivs)[1][ipar] = projR[paramType];//*sigma;
        }
        
        // the dr/db bit:
        for (int ipar=0; ipar<3; ipar++) {
          derivatives[nAlignPar+ipar][imeas] = RyGlob[ipar];
          if (m_storeDerivatives)   (*atsosDerVtx)[1][ipar] = RyGlob[ipar];
        }

        for (int i=0;i<nAlignPar+3;i++)
          ATH_MSG_DEBUG("2nd dim: derivatives["<<i<<"]["<<imeas<<"]="<<derivatives[i][imeas]);

        imeas++;
      }

      alignTSOS->setDerivatives(atsosDerivs);
      alignTSOS->setDerivativesVtx(atsosDerVtx);
    }
    ATH_MSG_DEBUG("returning derivatives");
    return derivatives;
  }

  //________________________________________________________________________
  void AnalyticalDerivCalcTool::checkResidualType(const AlignTrack * alignTrack)
  {
    // get first AlignTSOS of the AlignTrack
    // this assumes that for unbiased or DCA residuals the scatterers
    // and energy deposits are not included in the AlignTSOSSollection
    const AlignTSOS * atsos = *(alignTrack->firstAtsos());

    // get residual type of the first residual
    m_residualType = atsos->firstResidual()->residualType();
    ATH_MSG_DEBUG("setting residualType to "<<m_residualType);

    m_residualTypeSet = true;
  }

} // end namespace
