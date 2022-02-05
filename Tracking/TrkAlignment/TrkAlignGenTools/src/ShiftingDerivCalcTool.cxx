/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkEventPrimitives/TrackStateDefs.h"

#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkAlignInterfaces/IAlignResidualCalculator.h"
#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/AlignModuleList.h"

#include "TrkAlignGenTools/ShiftingDerivCalcTool.h"
#include "TrkAlignGenTools/AlignTrackCreator.h"

#include "xAODEventInfo/EventInfo.h"

#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TAxis.h"

using namespace std;

namespace Trk {

  //________________________________________________________________________
  ShiftingDerivCalcTool::ShiftingDerivCalcTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)

    : AthAlgTool(type,name,parent)
    , m_trackFitterTool("Trk::GlobalChi2Fitter/MCTBFitter")
    , m_SLTrackFitterTool("Trk::GlobalChi2Fitter/MCTBSLFitter")
    //,m_fitter?
    , m_residualCalculator("Trk::AlignResidualCalculator/ResidualCalculator")
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_traSize(.1)
    , m_rotSize(.1)
    , m_runOutlierRemoval(false)
    , m_particleHypothesis(Trk::muon)
    , m_particleNumber(2)
    , m_nChamberShifts{}
    , m_nIterations(0)
    , m_unshiftedResiduals(nullptr)
    , m_unshiftedResErrors(nullptr)
    //m_chi2VAlignParamVec
    //m_chi2VAlignParamXVec
    , m_tmpChi2VAlignParam(nullptr)
    , m_tmpChi2VAlignParamX(nullptr)
    , m_tmpChi2VAlignParamMeasType(nullptr)
    //m_chi2VAlignParamVecMeasType
    , m_unshiftedTrackChi2{}
    , m_unshiftedTrackChi2MeasType(new double  [TrackState::NumberOfMeasurementTypes])
    //m_trackAlignParamCut
    //m_setMinIterations
    //m_maxIter
    //m_minIter
    //m_removeScatteringBeforeRefit
    , m_ntracksProcessed(0)
    , m_ntracksPassInitScan(0)
    , m_ntracksPassSetUnshiftedRes(0)
    , m_ntracksPassDerivatives(0)
    , m_ntracksPassGetDeriv(0)
    , m_ntracksPassGetDerivSecPass(0)
    , m_ntracksPassGetDerivLastPass(0)
    , m_ntracksFailMaxIter(0)
    , m_ntracksFailTrackRefit(0)
    , m_ntracksFailAlignParamCut(0)
    , m_ntracksFailFinalAttempt(0)
    , m_secPass{}
  {
    declareInterface<IDerivCalcTool>(this);

    declareProperty("TrackFitterTool",               m_trackFitterTool);
    declareProperty("SLTrackFitterTool",             m_SLTrackFitterTool);
    declareProperty("TranslationSize",               m_traSize);
    declareProperty("RotationSize",                  m_rotSize);
    declareProperty("RunOutlierRemoval",             m_runOutlierRemoval);
    declareProperty("ParticleNumber",                m_particleNumber);
    declareProperty("doChi2VChamberShiftsMeasType",  m_doChi2VAlignParamMeasType = false);
    declareProperty("doResidualFits",                m_doFits = true);
    declareProperty("NumberOfShifts",                m_nFits=5);
    declareProperty("ResidualCalculator",            m_residualCalculator);
    declareProperty("AlignModuleTool",               m_alignModuleTool);
    declareProperty("doResidualPlots",               m_doResidualPlots=false);
    declareProperty("TrackAlignParamCut",            m_trackAlignParamCut=1e6);//.001
    declareProperty("SetMinIterations",              m_setMinIterations=false);
    declareProperty("MaxIterations",                 m_maxIter=50);
    declareProperty("MinIterations",                 m_minIter=10);

    declareProperty("RemoveScatteringBeforeRefit",   m_removeScatteringBeforeRefit=false);

    m_logStream = nullptr;

  }

  //________________________________________________________________________
  ShiftingDerivCalcTool::~ShiftingDerivCalcTool()
  {
    deleteChi2VAlignParam();
    delete [] m_unshiftedTrackChi2MeasType;
  }

  //________________________________________________________________________
  StatusCode ShiftingDerivCalcTool::initialize()
  {

    msg(MSG::DEBUG)  << "in ShiftingDerivCalcTool initialize()"<<endmsg;
    ATH_CHECK(m_trackFitterTool.retrieve());
    ATH_CHECK(m_SLTrackFitterTool.retrieve());
    ATH_CHECK(m_residualCalculator.retrieve());
    ATH_CHECK(m_alignModuleTool.retrieve());

    ParticleSwitcher particleSwitch;
    m_particleHypothesis = particleSwitch.particle[m_particleNumber];
    msg(MSG::INFO) << "ParticleNumber: "     << m_particleNumber     << endmsg;
    msg(MSG::INFO) << "ParticleHypothesis: " << m_particleHypothesis << endmsg;


    if(!m_doFits){
      m_nFits = 2;
    }

    m_nChamberShifts = m_nFits;
    m_traSize = 5.*m_traSize/(double)m_nFits;
    m_rotSize = 5.*m_rotSize/(double)m_nFits;

    msg(MSG::INFO) << "doFits: "   << m_doFits  << endmsg;
    msg(MSG::INFO) << "nFits:  "   << m_nFits  << endmsg;

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode ShiftingDerivCalcTool::finalize()
  {
    ATH_MSG_INFO("number tracks processed:                           "<<m_ntracksProcessed<<
     "\nnumber tracks passing initial scan:                "<<m_ntracksPassInitScan<<
     "\nnumber tracks passing setting unshifted residuals: "<< m_ntracksPassSetUnshiftedRes<<
     "\nnumber tracks pass getting derivatives (1st pass): "<<m_ntracksPassGetDeriv<<
     "\nnumber tracks pass getting derivatives (2nd pass): "<<m_ntracksPassGetDerivSecPass<<
     "\nnumber tracks pass getting derivatives (3rd pass): "<<m_ntracksPassGetDerivLastPass<<
     "\nnumber tracks pass setting derivatives:            "<<m_ntracksPassDerivatives);
    ATH_MSG_INFO("number tracks fail max iterations:                 "<<m_ntracksFailMaxIter<<
     "\nnumber tracks fail track refit:                    "<<m_ntracksFailTrackRefit<<
     "\nnumber tracks fail align param cut:                "<<m_ntracksFailAlignParamCut<<
     "\nnumber tracks fail final attempt:                  "<<m_ntracksFailFinalAttempt);

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool ShiftingDerivCalcTool::scanShifts(const AlignTrack* alignTrack,
           const std::vector<const AlignModule*>& alignModules)
  {
    ATH_MSG_DEBUG("in scanShifts");

    const Trk::Track* trackForRefit =
      (m_removeScatteringBeforeRefit) ? alignTrack->trackWithoutScattering():
      dynamic_cast<const Trk::Track*>(alignTrack);

    // see whether straight track or not
    m_fitter = alignTrack->isSLTrack() ?
      m_SLTrackFitterTool : m_trackFitterTool;
    ATH_MSG_DEBUG("refitting unshifted track with "<<m_fitter<<" (isSLTrack="
      <<alignTrack->isSLTrack()<<")");

    ATH_MSG_DEBUG("setting minNIterations to "<<m_nIterations);

    // refit track

    IGlobalTrackFitter::AlignmentCache alignCache;
    alignCache.m_minIterations = m_nIterations;
    const Track* refittedTrack = m_fitter->alignmentFit(alignCache, *trackForRefit,
            m_runOutlierRemoval,
            m_particleHypothesis);
    if (!refittedTrack) {
      msg(MSG::WARNING)  << "initial track refit failed" << endmsg;
      return false;
    }
    else
      ATH_MSG_DEBUG("initial track refit successful");

    m_nIterations = alignCache.m_iterationsOfLastFit;
    if (m_nIterations>m_maxIter) {
      ATH_MSG_DEBUG("exceeded maximum number of iterations");
      return false;
    }
    ATH_MSG_DEBUG("initial nIterations: "<<m_nIterations);

    // loop over AlignModules
    int imod(0);
    for (std::vector<const AlignModule*>::const_iterator moduleIt=alignModules.begin();
   moduleIt!=alignModules.end(); ++moduleIt,imod++) {

      // loop over AlignPar
      int ipar(0);
      DataVector<AlignPar>* alignPars=m_alignModuleTool->getAlignPars(*moduleIt);
      for (DataVector<AlignPar>::iterator alignParIt=alignPars->begin();
     alignParIt!=alignPars->end(); ++alignParIt,ipar++) {

  for (int ishift=0;ishift<2;ishift++) {

    double shiftsize = shiftSize(*alignParIt);
    if (ishift>0) shiftsize*=-1.;
    m_alignModuleTool->shiftModule(*moduleIt,alignTrack,(**alignParIt).paramType(),shiftsize);
    refittedTrack = (m_fitter->fit(Gaudi::Hive::currentContext(),
                                  *trackForRefit,m_runOutlierRemoval,
                                  m_particleHypothesis)).release();
    m_alignModuleTool->restoreModule(*moduleIt);
    if (!refittedTrack) {
      msg(MSG::WARNING) << "track refit failed!"<<endmsg;
      m_nIterations=0;
      return false;
    }

    int nIter=alignCache.m_iterationsOfLastFit;
    ATH_MSG_DEBUG("nIter: "<<nIter);
    if (nIter>m_maxIter) {
      ATH_MSG_DEBUG("exceeded maximum number of iterations");
      m_nIterations=0;
      return false;
    }

    if (nIter>m_nIterations) m_nIterations=nIter;
  }
      } // loop over AlignPar
    } // loop over AlignModules

    ATH_MSG_DEBUG("done with scanShifts, m_nIterations="<<m_nIterations);
    return true;
  }

  //________________________________________________________________________
  bool ShiftingDerivCalcTool::setUnshiftedResiduals(AlignTrack* alignTrack)
  {

    // see whether straight track or not
    m_fitter = alignTrack->isSLTrack() ?
      m_SLTrackFitterTool : m_trackFitterTool;
    ATH_MSG_DEBUG("refitting unshifted track with "<<m_fitter<<" (isSLTrack="
      <<alignTrack->isSLTrack()<<")");

    // refit track
    ATH_MSG_DEBUG("\nsetting min number iterations to "<<m_nIterations);
    IGlobalTrackFitter::AlignmentCache alignCache;
    alignCache.m_minIterations = m_nIterations;

    const Trk::Track* trackForRefit =
      (m_removeScatteringBeforeRefit) ? alignTrack->trackWithoutScattering():
      dynamic_cast<const Trk::Track*>(alignTrack);
    if (!trackForRefit) ATH_MSG_ERROR("no track for refit!");

    const Track* refittedTrack = m_fitter->alignmentFit( alignCache,
            *trackForRefit,
            m_runOutlierRemoval,
            m_particleHypothesis);

    if (!refittedTrack) {
      msg(MSG::WARNING)  << "initial track refit failed" << endmsg;
      return false;
    }
    else
      ATH_MSG_DEBUG("initial track refit successful");

    // dump local track chi2 for debugging
    double localChi2=m_residualCalculator->setResiduals(alignTrack,refittedTrack);
    msg()<<MSG::DEBUG<<"local Chi2(unshifted) in setChi2VAlignParam="<<localChi2<<endmsg;
    m_unshiftedTrackChi2 = localChi2;
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++) {
      ATH_MSG_DEBUG("getting chi2 for measType "<<i);
      m_unshiftedTrackChi2MeasType[i]=m_residualCalculator->chi2ForMeasType(i);
    }
    ATH_MSG_DEBUG("done");

    // create vector containing unshifted residuals and matrices containing errors
    const int NMEAS=alignTrack->nAlignTSOSMeas();

    // unshiftedResiduals owned by AlignTrack
    m_unshiftedResiduals=new Amg::VectorX(NMEAS);

    // unshiftedResErrors owned by ShiftingDerivCalcTool
    if (m_unshiftedResErrors) delete m_unshiftedResErrors;
      m_unshiftedResErrors=new Amg::VectorX(NMEAS);

    // loop over atsos and determine residuals and errors
    int imeas=0;
    AlignTSOSCollection::const_iterator atsosItr=alignTrack->firstAtsos();
    for (; atsosItr != alignTrack->lastAtsos(); ++atsosItr) {
      if (!(**atsosItr).isValid()) continue;
      for (std::vector<Residual>::const_iterator itRes=(**atsosItr).firstResidual();
        itRes!=(**atsosItr).lastResidual();++itRes,++imeas) {
          double residual = itRes->residual();
          double errSq    = itRes->errSq();
          (*m_unshiftedResiduals)[imeas]=residual;
          (*m_unshiftedResErrors)[imeas]=std::sqrt(errSq);
          //ATH_MSG_DEBUG("weight: "<<1./errSq<<", unshiftedRes["<<imeas<<"]="
          //      <<(*m_unshiftedResiduals)[imeas]
          //      <<", resNorm="<<itRes->residualNorm());
      }
    }
    if (imeas!=NMEAS) {
      msg(MSG::ERROR)<<"problem with nmeas, imeas="<<imeas<<", NMEAS="<<NMEAS<<endmsg;
      exit(3);
    }
    alignTrack->setResidualVector(m_unshiftedResiduals);

    delete refittedTrack; refittedTrack=nullptr;

    return true;
  }

//________________________________________________________________________
bool ShiftingDerivCalcTool::setDerivatives(AlignTrack* alignTrack)
{
  ATH_MSG_DEBUG("in ShiftingDerivCalcTool setDerivatives");
  m_ntracksProcessed++;

  // loop over AlignTSOSCollection,
  // find modules that are in the AlignModuleList,
  std::vector<const AlignModule*> alignModules;
  for (AlignTSOSCollection::const_iterator atsosItr=alignTrack->firstAtsos();
      atsosItr != alignTrack->lastAtsos(); ++atsosItr) {

    ATH_MSG_VERBOSE("getting module");
    const AlignModule* module=(*atsosItr)->module();
    if (module)
      ATH_MSG_VERBOSE("have ATSOS for module "<<module->identify());
    else
      ATH_MSG_VERBOSE("no module!");

    if (!(*atsosItr)->isValid() || !module) continue;
    if (find(alignModules.begin(),alignModules.end(),module) == alignModules.end())
    alignModules.push_back(module);
  }

  // find perigee of best track fit and use as starting perigee for all fits
  m_nIterations=m_minIter;
  if (m_setMinIterations && !scanShifts(alignTrack, alignModules)) {
    return false;
  };

  m_ntracksPassInitScan++;

  // set unshifted residuals (this is done in AlignTrackDresser but redone here with track refit)
  if (!setUnshiftedResiduals(alignTrack)) {
    ATH_MSG_WARNING("problem with refitting track!");
    return false;
  };

  m_ntracksPassSetUnshiftedRes++;

  // Determine derivatives from shifting these modules
  std::vector<AlignModuleDerivatives> * derivatives = new std::vector<AlignModuleDerivatives>;
  std::vector<AlignModuleDerivatives> * derivativeErr = new std::vector<AlignModuleDerivatives>;
  std::vector<std::pair<const AlignModule*, std::vector<double> > > * actualSecondDerivatives =
      new std::vector<std::pair<const AlignModule*, std::vector<double> > >;
  deleteChi2VAlignParam();
  for (std::vector<const AlignModule*>::const_iterator moduleIt=alignModules.begin();
    moduleIt!=alignModules.end(); ++moduleIt) {

    ATH_MSG_DEBUG("finding derivatives for module "<<(**moduleIt).identify());

    std::vector<Amg::VectorX> deriv_vec;
    std::vector<Amg::VectorX> derivErr_vec;
    std::vector<double> actualsecderiv_vec;

    // get alignPars and create arrays to store chi2 vs. align pars
    DataVector<AlignPar>* alignPars=m_alignModuleTool->getAlignPars(*moduleIt);
    const int nAlignPar = alignPars->size();
    m_tmpChi2VAlignParam  = new double*[nAlignPar];
    m_tmpChi2VAlignParamX = new double*[nAlignPar];
    if (m_doChi2VAlignParamMeasType) {
      m_tmpChi2VAlignParamMeasType = new double**[TrackState::NumberOfMeasurementTypes];
      for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
        m_tmpChi2VAlignParamMeasType[i] = new double*[nAlignPar];
    }


    // get derivatives and arrays of chi2 vs. align params
    bool resetIPar=false;
    std::vector<Amg::VectorX> tmpderiv_vec;
    std::vector<Amg::VectorX> tmpderivErr_vec;
    std::vector<double> tmpactualsecderiv_vec;
    m_secPass=false;

    // first attempt with normal number of fitter iterations
    bool success=getAllDerivatives(
           alignTrack, *moduleIt,
           tmpderiv_vec,tmpderivErr_vec,tmpactualsecderiv_vec,
           resetIPar);
    if (!success){
      delete derivatives;
      delete derivativeErr;
      delete actualSecondDerivatives;
      return false;
    }

    m_ntracksPassGetDeriv++;

    if (resetIPar) {
      // second attempt with increased number of fitter iterations
      m_secPass=true;
      success=getAllDerivatives(alignTrack,*moduleIt,
              tmpderiv_vec,tmpderivErr_vec,tmpactualsecderiv_vec,
              resetIPar);
    }

    if (!success){
      delete derivatives;
      delete derivativeErr;
      delete actualSecondDerivatives;
      return false;
    }

    m_ntracksPassGetDerivSecPass++;

    if (resetIPar) {
      // third and last attempt with number of fitter iterations set to maximum
      m_nIterations=m_maxIter;
      success=getAllDerivatives(alignTrack,*moduleIt,
              tmpderiv_vec,tmpderivErr_vec,tmpactualsecderiv_vec,
              resetIPar);
    }

    if (!success){
      delete derivatives;
      delete derivativeErr;
      delete actualSecondDerivatives;
      return false;
    }


    m_ntracksPassGetDerivLastPass++;

    if (success && !resetIPar) {
      for (int i=0;i<(int)tmpderiv_vec.size();i++) {
        deriv_vec.push_back(tmpderiv_vec[i]);
        derivErr_vec.push_back(tmpderivErr_vec[i]);
        actualsecderiv_vec.push_back(tmpactualsecderiv_vec[i]);
      }
    }
    else{
      delete derivatives;
      delete derivativeErr;
      delete actualSecondDerivatives;
      return false;
    }
    // set the chi2 vs. align param arrays
    ATH_MSG_DEBUG("setting chi2 vs. align param arrays");
    m_chi2VAlignParamVec.push_back(m_tmpChi2VAlignParam);
    m_chi2VAlignParamXVec.push_back(m_tmpChi2VAlignParamX);
    (**moduleIt).setChi2VAlignParamArray (m_tmpChi2VAlignParam);
    (**moduleIt).setChi2VAlignParamXArray(m_tmpChi2VAlignParamX);

    // arrays for measurement types
    if (m_doChi2VAlignParamMeasType) {
      ATH_MSG_DEBUG("pushing back for measType");
      m_chi2VAlignParamVecMeasType.push_back(m_tmpChi2VAlignParamMeasType);
      for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
        (**moduleIt).setChi2VAlignParamArrayMeasType(i,m_tmpChi2VAlignParamMeasType[i]);
    }
    ATH_MSG_DEBUG("done setting arrays");

    derivatives->push_back(make_pair(*moduleIt,deriv_vec));
    derivativeErr->push_back(make_pair(*moduleIt,derivErr_vec));
    actualSecondDerivatives->push_back(make_pair(*moduleIt,actualsecderiv_vec));
  }

  m_ntracksPassDerivatives++;

  alignTrack->setDerivatives(derivatives);
  alignTrack->setDerivativeErr(derivativeErr);
  alignTrack->setActualSecondDerivatives(actualSecondDerivatives);

  // restore unshifted residuals in AlignTSOS
  setUnshiftedResiduals(alignTrack);

  return true;
}

//________________________________________________________________________
Amg::VectorX ShiftingDerivCalcTool::getDerivatives(
              AlignTrack* alignTrack,
              int ipar, const AlignPar* alignPar,
              Amg::VectorX& derivativeErr,
              bool& resetIPar,
              double& actualSecondDeriv)
{
  const Trk::Track* trackForRefit =
    (m_removeScatteringBeforeRefit) ? alignTrack->trackWithoutScattering():
    dynamic_cast<const Trk::Track*>(alignTrack);

  ATH_MSG_DEBUG("m_nIterations: "<<m_nIterations);

  // gets derivatives of residuals w.r.t. a specific alignment parameter given by alignPar
  if (!m_fitter)
    ATH_MSG_ERROR("set m_fitter before calling getDerivatives (by calling setUnshiftedResiduals)");

  const AlignModule* module=alignPar->alignModule();

  // set derivatives for 2 shifts up and 2 shifts down
  const int NFITS = m_nFits;
  const int NMEAS = alignTrack->nAlignTSOSMeas();
  module->setNChamberShifts(m_nFits);

  ATH_MSG_DEBUG("NMEAS="<<NMEAS);
  double** residuals=new double*[NFITS];
  double** resErrors=new double*[NFITS];
  double* chi2Array =new double[NFITS];
  double* chi2ArrayX=new double[NFITS];

  if (m_doChi2VAlignParamMeasType) {
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
      m_tmpChi2VAlignParamMeasType[i][ipar] =new double[NFITS];
  }

  for (int ifit=0;ifit<NFITS;ifit++) {
    residuals[ifit]=new double[NMEAS];
    resErrors[ifit]=new double[NMEAS];
  }

  // set the values for the unshifted track
  const int unshiftedTrackIndex = m_doFits ? (m_nFits-1)/2 : 1;
  chi2Array [unshiftedTrackIndex] = m_unshiftedTrackChi2;
  ATH_MSG_DEBUG("chi2Array["<<unshiftedTrackIndex<<"]="<<chi2Array[unshiftedTrackIndex]);
  chi2ArrayX[unshiftedTrackIndex] = 0.;
  if (m_doChi2VAlignParamMeasType) {
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++) {
      m_tmpChi2VAlignParamMeasType[i][ipar][unshiftedTrackIndex]=
      m_unshiftedTrackChi2MeasType[i];
      ATH_MSG_DEBUG("chi2ArrayMeasType["<<i<<"]["<<unshiftedTrackIndex<<"]="<<m_unshiftedTrackChi2MeasType[i]);
    }
  }


  // get shift size
  double shiftsize=shiftSize(alignPar);

  IGlobalTrackFitter::AlignmentCache alignCache;


  ATH_MSG_VERBOSE("doing refits");
  for (int ifit=0;ifit<NFITS;ifit++) {

    ATH_MSG_VERBOSE("ifit="<<ifit);
    int jfit=ifit;
    if (ifit>unshiftedTrackIndex) {
      jfit=NFITS-ifit+unshiftedTrackIndex;
    }
    if (m_doFits && ifit==unshiftedTrackIndex) {
      for (int i=0;i<(int)m_unshiftedResiduals->rows();i++) {
        residuals[ifit][i]=(*m_unshiftedResiduals)[i];
        resErrors[ifit][i]=(*m_unshiftedResErrors)[i];
      }
      // change back in case it got changed on the other side of zero
      shiftsize=shiftSize(alignPar);
      continue;
    }

    // shift module and fit track
    double currentshift = 0.;
    if(m_doFits)
      currentshift = shiftsize * (double)(jfit-unshiftedTrackIndex);
    else
      currentshift = (ifit==0) ? -1.*shiftsize : shiftsize;

    ATH_MSG_DEBUG("current shift="<<currentshift<<" in getDerivatives");

    m_alignModuleTool->shiftModule(module,alignTrack,
    alignPar->paramType(),currentshift);


    ATH_MSG_VERBOSE("fitting after shift");
    const Track* refittedTrack=m_fitter->alignmentFit(alignCache,
               *trackForRefit,
               m_runOutlierRemoval,m_particleHypothesis);
    if (m_setMinIterations && alignCache.m_iterationsOfLastFit>m_nIterations) {
      m_nIterations=alignCache.m_iterationsOfLastFit;
      if (m_nIterations>m_maxIter) {
        ATH_MSG_DEBUG("exceeded max number of iterations");
        m_alignModuleTool->restoreModule(module);
        resetIPar=false;
        ATH_MSG_DEBUG("resetIPar set to false");
        delete [] residuals; delete [] resErrors;
        delete [] chi2Array; delete [] chi2ArrayX;
        ATH_MSG_DEBUG("fail max iter");
        m_ntracksFailMaxIter++;
        Amg::VectorX derivatives(1);
        return derivatives;
      }
      ATH_MSG_DEBUG("increasing m_nIterations to "<<m_nIterations<<" (not changing in fit yet)");
      resetIPar=true;
      ATH_MSG_DEBUG("resetIPar set to true");
    }

    // if resetIPar refit the rest of the tracks, but don't do anything with them until next pass
    if (resetIPar) {
      m_alignModuleTool->restoreModule(module);
      continue;
    }

    if (!refittedTrack) {
      msg(MSG::WARNING) << "track refit failed for jfit "<<jfit <<endmsg;
      delete [] residuals; delete [] resErrors;
      delete [] chi2Array; delete [] chi2ArrayX;
      m_alignModuleTool->restoreModule(module);
      if (!resetIPar || m_secPass) {
        m_ntracksFailTrackRefit++;
      }
      ATH_MSG_DEBUG("fail track refit, resetIPar "<<resetIPar<<", secPass "<<m_secPass);
      Amg::VectorX derivatives(1);
      return derivatives;
    }
    else
      ATH_MSG_VERBOSE("track refit successful");

    double chi2=refittedTrack->fitQuality()->chiSquared();

    ATH_MSG_VERBOSE("jfit = "<<jfit);
    double localChi2=m_residualCalculator->setResiduals(alignTrack,refittedTrack);
    ATH_MSG_DEBUG("localChi2/fittedChi2="<<localChi2<<"/"<<chi2);

    chi2ArrayX[jfit]= shiftsize * (double)(jfit-unshiftedTrackIndex);// / module->sigma(idof);
    chi2Array[jfit]=localChi2;
    ATH_MSG_DEBUG("chi2Array["<<jfit<<"]="<<chi2Array[jfit]);
    if (m_doChi2VAlignParamMeasType) {
      for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++) {
        m_tmpChi2VAlignParamMeasType[i][ipar][jfit]=  m_residualCalculator->chi2ForMeasType(i);
        ATH_MSG_DEBUG("chi2ArrayMeasType["<<i<<"]["<<jfit<<"]="
                      <<m_tmpChi2VAlignParamMeasType[i][ipar][jfit]);
      }
    }

    ATH_MSG_DEBUG("positions["<<jfit<<"]="<<chi2ArrayX[jfit]);

    int imeas(0);
    AlignTSOSCollection::const_iterator atsosItr=alignTrack->firstAtsos();
    for (; atsosItr != alignTrack->lastAtsos(); ++atsosItr) {
      if (!(*atsosItr)->isValid()) continue;
      for (vector<Residual>::const_iterator itRes=(**atsosItr).firstResidual();
           itRes!=(**atsosItr).lastResidual();++itRes,++imeas) {

        if (refittedTrack) {
          residuals[jfit][imeas]=itRes->residual();
          resErrors[jfit][imeas]=std::sqrt(itRes->errSq());
        }
        else {
          residuals[jfit][imeas]=resErrors[jfit][imeas]=0.;
        }
        ATH_MSG_DEBUG("residuals["<<jfit<<"]["<<imeas<<"]="<<residuals[jfit][imeas]);
        ATH_MSG_DEBUG("resErrors["<<jfit<<"]["<<imeas<<"]="<<resErrors[jfit][imeas]);
      }
    }

    delete refittedTrack; refittedTrack=nullptr;
    ATH_MSG_VERBOSE("calling restoreModule");
    m_alignModuleTool->restoreModule(module);
  } // NFITS

  int iimeas(0);
  AlignTSOSCollection::const_iterator aatsosItr=alignTrack->firstAtsos();
  for (; aatsosItr != alignTrack->lastAtsos(); ++aatsosItr) {
    if (!(*aatsosItr)->isValid()) continue;
    for (vector<Residual>::const_iterator itRes=(**aatsosItr).firstResidual();
         itRes!=(**aatsosItr).lastResidual();++itRes,++iimeas) {
      for (int ifit=0;ifit<NFITS;ifit++) {
        ATH_MSG_DEBUG("["<<ifit<<"]["<<iimeas<<"]   res="<<residuals[ifit][iimeas]<<
          ",   resErr="<<resErrors[ifit][iimeas]);
      }
    }
  }

  if (resetIPar) {
    //resetIPar=false;
    delete [] residuals; delete [] resErrors;
    delete [] chi2Array; delete [] chi2ArrayX;
    if (m_secPass) ATH_MSG_WARNING("failed second pass!");
    ATH_MSG_DEBUG("returning to reset IPar");
    Amg::VectorX derivatives;
    return derivatives;
  }

  // check chi2 vs. chamber pos to see if discontinuous
  TGraph* gr = new TGraph(m_nFits,chi2ArrayX,chi2Array);
  gr->Fit("pol2","QF");
  TF1* fit=gr->GetFunction("pol2");
  double chi2 =fit->GetChisquare()/double(m_nFits-3);
  double slope=fit->GetParameter(1);
  actualSecondDeriv=fit->GetParameter(2);
  delete gr;

  ATH_MSG_DEBUG("discontinuity check: chi2="<<chi2);
  alignTrack->setTrackAlignParamQuality(alignPar->paramType(),chi2);

  // EventInfo
  if (chi2>1.e-6 || std::fabs(slope)<1.e-10) {
    const xAOD::EventInfo* eventInfo;
    StatusCode sc=evtStore()->retrieve(eventInfo);
    if (sc.isFailure())
    ATH_MSG_ERROR("Couldn't retrieve event info");
    int run=eventInfo->runNumber();
    int evt=eventInfo->eventNumber();
    ATH_MSG_DEBUG("discontinuity check: chi2="<<chi2<<", run/evt "<<run<<"/"<<evt);
  }

  //reset in case it got changed somewhere
  shiftsize = shiftSize(alignPar);

  //-----------------------------------------//
  //--   get derivatives from residuals    --//
  //-----------------------------------------//
  ATH_MSG_VERBOSE("calculating residuals");
  Amg::VectorX derivatives(alignTrack->nAlignTSOSMeas(),0);
  ATH_MSG_DEBUG("created derivatives with "<<derivatives.rows()<<" rows");

  // if bad fit or first derivative close to zero, replace derivatives with zeros
  if (chi2>m_trackAlignParamCut) {// || std::fabs(slope)<1.e-10 ) {
    ATH_MSG_DEBUG("chi2/"<<m_nFits-3<<"="<<chi2);
    delete [] residuals; delete [] resErrors;
    delete [] chi2Array; delete [] chi2ArrayX;

    m_nIterations=alignCache.m_iterationsOfLastFit+5;
    if (m_nIterations>m_maxIter) {
      ATH_MSG_DEBUG("exceeded max number of iterations");
      resetIPar=false;
    }
    ATH_MSG_DEBUG("increasing m_nIterations to "<<m_nIterations<<" (not changing in fit yet)");
    resetIPar=true;
    ATH_MSG_INFO("fail align param cut, secPass "<<m_secPass);
    if (m_secPass) {
      m_ntracksFailAlignParamCut++;
    }
    Amg::VectorX emptyDerivatives;
    return emptyDerivatives;
  }

  int imeas(0);
  TCanvas* canv(nullptr);
  std::vector<TGraph*> vecGraphs;
  AlignTSOSCollection::const_iterator atsosItr=alignTrack->firstAtsos();
  for (; atsosItr != alignTrack->lastAtsos(); ++atsosItr) {
    if (!(*atsosItr)->isValid()) continue;
    for (int idim=0;idim<(*atsosItr)->nResDim();idim++) {

      double* gr_x    = new double[NFITS];
      double* gr_y    = new double[NFITS]; // residuals only have float precision if determined from ESD
      int ngoodfits=0;
      for (int ifit=0;ifit<NFITS;ifit++) {
        double residual=residuals[ifit][imeas];
        double resError=resErrors[ifit][imeas];
        if (residual>-999.) {
          gr_x[ngoodfits]   =chi2ArrayX[ifit];
          gr_y[ngoodfits]   =residual/resError;
          ngoodfits++;
        }
      }

      if (!m_doFits && ngoodfits==2) {
        derivatives[imeas]=(residuals[1][imeas]-residuals[0][imeas])/(2.*shiftsize)*
        resErrors[unshiftedTrackIndex][imeas];
      }
      else if (m_doFits && ngoodfits>3) {
        TGraph* gr=new TGraph(ngoodfits,gr_x,gr_y);

      if (m_doResidualPlots)
        gr->Fit("pol2","VF");
      else
        gr->Fit("pol2","QF");
      TF1* fit=gr->GetFunction("pol2");

      //double derivRatio=fit->GetParameter(2)/fit->GetParameter(1);
      ATH_MSG_DEBUG("deriv["<<imeas<<"]="<<fit->GetParameter(1)<<" +/- "<<fit->GetParError(1)
        <<", chi2="<<fit->GetChisquare());
      derivatives[imeas]=fit->GetParameter(1)*resErrors[unshiftedTrackIndex][imeas]; // first derivative at x=0
      derivativeErr[imeas]=fit->GetParError(1)*resErrors[unshiftedTrackIndex][imeas];


      // plot residuals vs. chamber position
      if (m_doResidualPlots) {
        if (!canv) canv=new TCanvas("resPlots","resPlots");
        canv->cd();
        gr->SetMarkerStyle(20);
              gr->Draw("AP");

        gr->GetXaxis()->SetTitle("shift in chamber pos. from nominal (CLHEP::mm)");
        gr->GetYaxis()->SetTitle("residual (CLHEP::mm)");

        TPaveText* pave=new TPaveText(.4,.65,.97,.92,"NDC");
        pave->SetFillColor(0);
        pave->SetBorderSize(1);
        std::stringstream measType; measType<<"meas type: ";
        if      ((*atsosItr)->measType()==TrackState::MDT) measType<<" MDT";
        else if ((*atsosItr)->measType()==TrackState::TGC) measType<<" TGC";
        else if ((*atsosItr)->measType()==TrackState::RPC) measType<<" RPC";
        else measType<<" undefined";

        pave->AddText(measType.str().c_str());

        stringstream firstderivtxt,secndderivtxt,aptxt,chi2txt;
        firstderivtxt<<fit->GetParameter(1)<<" +/- "<<fit->GetParError(1);
        secndderivtxt<<fit->GetParameter(2)<<" +/- "<<fit->GetParError(2);
        aptxt  <<"alignPar "<<alignPar->paramType()<<", RIO in "<<(*atsosItr)->identify();
        chi2txt<<"chi2="<<fit->GetChisquare();

        pave->AddText(firstderivtxt.str().c_str());
        pave->AddText(secndderivtxt.str().c_str());
        pave->AddText(aptxt.str().c_str());
        pave->AddText(chi2txt.str().c_str());
        pave->Draw();

        std::stringstream canvName;
        canvName<<"resPlots_ap"<<alignPar->paramType()<<"_measType"
        <<(*atsosItr)->measType()<<"_"<<imeas<<".eps";
        canv->Print(canvName.str().c_str());
        canv->Clear();

        delete pave;
      }
      vecGraphs.push_back(gr);
    }
    else {
      derivatives[imeas]=-999.;
      derivativeErr[imeas]=-999.;
    }

    delete [] gr_y;
    delete [] gr_x;

    ++imeas;
    }
  }

  // delete TGraphs and TCanvas
  for (int i=0;i<(int)vecGraphs.size();i++)
    delete vecGraphs[i];
  delete canv;

  delete [] residuals;
  delete [] resErrors;

  // set chi2 v alignparam
  for (int ifit=0;ifit<NFITS;ifit++) {
    m_tmpChi2VAlignParamX[ipar]=chi2ArrayX;
    m_tmpChi2VAlignParam [ipar]=chi2Array;
  }

  ATH_MSG_DEBUG("derivativeErr: "<<derivativeErr);
  return derivatives;
}

//________________________________________________________________________
double ShiftingDerivCalcTool::shiftSize(const AlignPar* alignPar) const {
  bool rotation =
    alignPar->paramType() == AlignModule::RotX ||
    alignPar->paramType() == AlignModule::RotY ||
    alignPar->paramType() == AlignModule::RotZ;

  double shift = rotation ? m_rotSize : m_traSize;

  //ok... this is kind of ugly.
  double sigma=alignPar->sigma();
  return shift * sigma;
}

//________________________________________________________________________
bool ShiftingDerivCalcTool::setResidualCovMatrix(AlignTrack* alignTrack) const
{
  Amg::MatrixX* pW = new Amg::MatrixX(alignTrack->nAlignTSOSMeas(),alignTrack->nAlignTSOSMeas());
  //AmgSymMatrix* pW = new AmgSymMatrix(alignTrack->nAlignTSOSMeas());
  Amg::MatrixX& W = *pW;

  if (alignTrack->localErrorMatrixInv()) {
    ATH_MSG_ERROR("Need to assign this matrix correctly: ShiftingDerivCalcTool.cxx:888");
    W = *(alignTrack->localErrorMatrixInv());
    //W.assign(*(alignTrack->localErrorMatrixInv()));
  } else{
    delete pW;
    return false;
  }
  ATH_MSG_DEBUG("W: "<<W);

  bool Wisvalid(true);
  const double epsilon=1e-10;
  for( int irow=0; irow<W.rows(); ++irow) {
    Wisvalid = Wisvalid && W(irow,irow)>0;
    if( !(W(irow,irow)>0) )
      msg(MSG::WARNING) << "matrix invalid: " << W(irow,irow) << endmsg;

    for(int icol=0; icol<=irow; ++icol) {

    // this one must be true if everything else succeeded
    double Wcorr = W(irow,icol)/sqrt(W(irow,irow)*W(icol,icol));
    bool Wcorrisvalid = Wcorr+epsilon>=-1 && Wcorr-epsilon<=1;
    Wisvalid = Wisvalid && Wcorrisvalid;
    if( !Wcorrisvalid )
      msg(MSG::WARNING) << "matrix corr invalid: " << Wcorr-1 << " " << Wcorr+1 << endmsg;
    }
  }

  if (Wisvalid)
  alignTrack->setWeightMatrix(pW);

  Amg::MatrixX* pWfirst=new Amg::MatrixX(*pW);
  alignTrack->setWeightMatrixFirstDeriv(pWfirst);
  delete pW;

  return true;
}

//________________________________________________________________________
void ShiftingDerivCalcTool::deleteChi2VAlignParam()
{
  for (int i=0;i<(int)m_chi2VAlignParamVec.size();i++) {
    delete [] m_chi2VAlignParamVec[i];  m_chi2VAlignParamVec[i]=nullptr;
    delete [] m_chi2VAlignParamXVec[i]; m_chi2VAlignParamXVec[i]=nullptr;
  }
  m_chi2VAlignParamVec.clear();
  m_chi2VAlignParamXVec.clear();

  for (int i=0;i<(int)m_chi2VAlignParamVecMeasType.size();i++) {
    delete [] m_chi2VAlignParamVecMeasType[i];  m_chi2VAlignParamVecMeasType[i]=nullptr;
  }
  m_chi2VAlignParamVecMeasType.clear();
}

//________________________________________________________________________
bool ShiftingDerivCalcTool::getAllDerivatives(
          AlignTrack* alignTrack,
          const AlignModule* alignModule,
          std::vector<Amg::VectorX>& deriv_vec,
          std::vector<Amg::VectorX>& derivErr_vec,
          std::vector<double>& actualsecderiv_vec,
          bool& resetIPar)
{
  resetIPar=false;

  deriv_vec.clear();
  derivErr_vec.clear();
  actualsecderiv_vec.clear();

  setUnshiftedResiduals(alignTrack); // this will set the min number of iterations to the new value

  int ipar(0);
  DataVector<AlignPar>* alignPars=m_alignModuleTool->getAlignPars(alignModule);
  for (DataVector<AlignPar>::iterator it=alignPars->begin(); it!=alignPars->end(); ++it,ipar++) {
    ATH_MSG_DEBUG("ipar: "<<ipar);
    Amg::VectorX derivErr(alignTrack->nAlignTSOSMeas());
    double actualSecondDeriv(0.);
    const Amg::VectorX vec=getDerivatives(alignTrack,ipar,*it,derivErr,resetIPar,actualSecondDeriv);
    ATH_MSG_DEBUG("vec size: "<<vec.rows());

    ATH_MSG_DEBUG("resetIPar="<<resetIPar);
    if (resetIPar) continue; // continue with derivatives to find max iteration

    if (vec.rows()<1) return false; // derivatives won't be set for alignTrack because it's a bad track

    deriv_vec.push_back(vec);
    derivErr_vec.push_back(derivErr);
    actualsecderiv_vec.push_back(actualSecondDeriv);

    for (int i=0;i<m_nFits;i++) {
      ATH_MSG_DEBUG("m_tmpChi2VAlignParam["<<ipar<<"]["
                <<i<<"]="<<m_tmpChi2VAlignParam[ipar][i]);
    }
  }

  return true;
}

} // end namespace
