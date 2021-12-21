/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkAlignEvent/AlignTrack.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "TrkAlignGenTools/AlignResidualCalculator.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkParameters/TrackParameters.h"

#include "muonEvent/CaloEnergy.h"

#include <vector>

namespace Trk {

  //______________________________________________________________
  AlignResidualCalculator::AlignResidualCalculator(const std::string& type, const std::string& name,
           const IInterface* parent)

    : AthAlgTool(type,name,parent)
    , m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
    , m_updator("Trk::KalmanUpdator/TrkKalmanUpdator")
    , m_qOverP{}
    , m_previousQOverP{}
    , m_nDoF{}
    , m_chi2ForMeasType(nullptr)
  {
    declareInterface<IAlignResidualCalculator>(this);

    declareProperty("ResidualPullCalculator",   m_pullCalculator);
    declareProperty("ResidualType",             m_resType = HitOnly);
    declareProperty("IncludeScatterers",        m_includeScatterers = false );

    declareProperty("UpdatorTool",              m_updator);
  }

  //________________________________________________________________________
  AlignResidualCalculator::~AlignResidualCalculator()
  {
    delete [] m_chi2ForMeasType;
  }

  //________________________________________________________________________
  StatusCode AlignResidualCalculator::initialize()
  {
    // get residual pull calculator
    if (m_pullCalculator.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not get " << m_pullCalculator << endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved " << m_pullCalculator);

    // get updator
    if(m_resType==Unbiased) {
      if (m_updator.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not get " << m_updator << endmsg;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Retrieved " << m_pullCalculator);
    }

    ATH_MSG_INFO("Using"<<AlignResidualType(m_resType)<<"residuals.");

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode AlignResidualCalculator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  double AlignResidualCalculator::setResiduals(AlignTSOSCollection* atsosColl,const Track* track)
  {
    bool useNewTrack = (track!=nullptr);
    return setResiduals(atsosColl->begin(), atsosColl->end(), track, useNewTrack);
  }

  //________________________________________________________________________
  double AlignResidualCalculator::setResiduals(AlignTrack* alignTrack,const Track* track)
  {
    bool useNewTrack = (track!=nullptr);
    const Track* newTrack = (useNewTrack) ? track : alignTrack;
    return setResiduals(alignTrack->firstAtsos(), alignTrack->lastAtsos(), newTrack, useNewTrack);
  }

  //________________________________________________________________________
  double AlignResidualCalculator::setResiduals(AlignTSOSCollection::iterator firstAtsos,
                                               AlignTSOSCollection::iterator lastAtsos,
                                               const Track* track, bool newTrack)
  {
    m_nDoF=0;
    m_matchedTSOS.clear();

    delete [] m_chi2ForMeasType;
    m_chi2ForMeasType = new double[TrackState::NumberOfMeasurementTypes];
    for (int i=0;i<TrackState::NumberOfMeasurementTypes;i++)
      m_chi2ForMeasType[i]=0.;

    //ATH_MSG_DEBUG("in setResiduals with newTrack="<<newTrack);

    if (!track&&newTrack) { ATH_MSG_ERROR("no track!"); return 0.; }

    int    ntsos(0);
    double chi2(0.);
    for (AlignTSOSCollection::iterator atsos=firstAtsos; atsos != lastAtsos; ++atsos,ntsos++) {

      const AlignTSOS* atsosp = *atsos;
      // get TSOS from AlignTSOS or closest TSOS if track given
      const TrackStateOnSurface* tsos =
          (newTrack) ? getMatchingTSOS(*atsos,track) : dynamic_cast<const TrackStateOnSurface*>(atsosp);

      ATH_MSG_DEBUG("ntsos "<<ntsos);

      const MaterialEffectsBase*         meb        = tsos->materialEffectsOnTrack();
      const Trk::MaterialEffectsOnTrack* meot       = dynamic_cast<const MaterialEffectsOnTrack*>(meb);
      //const ScatteringAngles*            scatterer = (meot) ? meot->scatteringAngles() : 0;

      // if scatterer, add scattering parameters
      //int nScattererDim=0;
      if (m_includeScatterers && meb && meot) {
        //nScattererDim = (scatterer) ? 2 : 1;
        accumulateScattering(tsos);
      }
      //ATH_MSG_DEBUG("scattererDim="<<nScattererDim);
      //(**atsos).setScatteringAngle(scatteringAngle);

      // set residuals for alignTSOS
      double dchi2 = setResidualsOnATSOS(*atsos,tsos);
      chi2 += dchi2;
      int imeasType=(**atsos).measType();
      m_chi2ForMeasType[imeasType] += dchi2;
      ATH_MSG_DEBUG("adding "<<dchi2<<", m_chi2ForMeasType["<<imeasType<<"]="<<m_chi2ForMeasType[imeasType]);
    }

    return chi2;///(double)m_nDoF;
  }

  //_______________________________________________________________________
  double
  AlignResidualCalculator::setResidualsOnATSOS(AlignTSOS * atsos, const TrackStateOnSurface * tsos)
  {
    // this method does the following:
    // 1. gets residuals (measurement and scattering dimensions),
    // 2. adds to AlignTSOS, and
    // 3. returns contribution to chi2 from this ATSOS

    ATH_MSG_DEBUG("in setResidualsOnATSOS");

    atsos->clearResiduals();

    double dchi2(0.);

    // scattering residual(s) and/or energy loss first
    if (m_includeScatterers) {
      ATH_MSG_DEBUG("scatterer");

      // when using unbiased residuals including scatterers doesn't make sense
      if(m_resType == Unbiased)
        ATH_MSG_WARNING("When using unbiased residuals including scatterers doesn't make sense!");

      const MaterialEffectsBase*         meb        = tsos->materialEffectsOnTrack();
      const Trk::MaterialEffectsOnTrack* meot       = dynamic_cast<const MaterialEffectsOnTrack*>(meb);
      const ScatteringAngles*            scatterer  = (meot) ? meot->scatteringAngles() : nullptr;

      int nscatparam=0;
      if (meb && meot)
        nscatparam = (scatterer) ? 2 : 1;

      for (int iparam=0;iparam<nscatparam;iparam++) {

        double errSq=0.;
        double residual(-999.);

        if (nscatparam==2) {

          const Trk::TrackParameters * tparp = tsos->trackParameters();
          if (tparp) {
            if (iparam==0) {
              residual=-scatterer->deltaPhi()* sin (tparp->parameters()[Trk::theta]);
              errSq   = scatterer->sigmaDeltaTheta();
              ATH_MSG_DEBUG("sigmaDeltaTheta="<<scatterer->sigmaDeltaTheta());
              ATH_MSG_DEBUG("sigmaDeltaPhi  ="<<scatterer->sigmaDeltaPhi());
              ATH_MSG_DEBUG("residual:"<<residual<<", errSq: "<<errSq<<", err="<<std::sqrt(errSq));
              errSq  *= errSq;
            }
            else{
              residual=-scatterer->deltaTheta();
              errSq   = scatterer->sigmaDeltaTheta();
              errSq  *= errSq;
            }
          }
          else{
            ATH_MSG_WARNING("scatterer has no TrackParameters!");
          }

          Residual res(HitOnly,Scatterer,ParamDefs(iparam),residual,errSq);
          atsos->addResidual(res);

          dchi2 += res.residualNorm()*res.residualNorm();
          m_nDoF++;
        }
        else if (!atsos->rio() && !atsos->crio()) {
          ATH_MSG_DEBUG("energy deposit");

          // energy deposit
          double E0        = 1./std::fabs(m_previousQOverP);
          double E1        = 1./std::fabs(m_qOverP);
          double energyLoss= std::fabs(meot->energyLoss()->deltaE());

          double residual = .001*(E0-E1-energyLoss);
          double errSq    = .001*meot->energyLoss()->sigmaDeltaE();

          ATH_MSG_DEBUG("E0/E1/energyLoss: "<<E0<<"/"<<E1<<"/"<<energyLoss);
          ATH_MSG_DEBUG("calorimeter residual: "<<residual/errSq);

          errSq*=errSq;

          Residual res(HitOnly,EnergyDeposit,ParamDefs(0),residual,errSq);
          atsos->addResidual(res);
          dchi2 += res.residualNorm()*res.residualNorm();
          m_nDoF++;
        }
      }
    }

    // residuals from measurement
    if (atsos->rio() || atsos->crio()) {

      int nparams = (atsos->measType()==TrackState::Pixel) ? 2 : 1;
      for (int iparam=0;iparam<nparams;iparam++) {

        double errSq=0.;
        double residual(-999.);

        if ( atsos->measType()!=TrackState::unidentified &&
            (atsos->rio()!=nullptr || atsos->crio()!=nullptr) ) {

          const MeasurementBase* mesb = tsos->measurementOnTrack();

          const TrackParameters * trackPars = tsos->trackParameters();
          const ResidualPull * resPull = nullptr;

          if ( trackPars ) {

            if (m_resType == Unbiased) {
              // Get unbiased state
              const Trk::TrackParameters * unbiasedTrackPars =
                  m_updator->removeFromState(*trackPars,
                                             mesb->localParameters(),
                                             mesb->localCovariance()).release();
              if (unbiasedTrackPars) {
                trackPars = unbiasedTrackPars;
                // AlignTSOS desctructor takes care of deleting unbiasedTrackPars
                atsos->setUnbiasedTrackPars(unbiasedTrackPars);
              }
              else
                ATH_MSG_WARNING("Could not get unbiased track parameters, use normal parameters");
            }

            ATH_MSG_DEBUG("Calling ResidualPullCalculator for residual type "
                          <<ResidualPullType(AlignResidualType(m_resType))
                          <<" (AlignResidualType "<<AlignResidualType(m_resType)
                          <<" "<<m_resType<<")");
            ATH_MSG_DEBUG("mesb->localErrorMatrix().covValue(Trk::loc1): "<<mesb->localCovariance()(Trk::loc1,Trk::loc1));
            resPull = m_pullCalculator->residualPull(mesb, trackPars,
                                                     ResidualPullType(AlignResidualType(m_resType)),
                                                     atsos->measType());
            if ( resPull ) {
              residual = (resPull->residual())[iparam];
              double pull=(resPull->pull())[iparam];
              if (pull!=0.) {
                errSq  = residual/pull;
                errSq *= errSq;
              }
              ATH_MSG_DEBUG("residual="<<residual<<", pull="<<pull);
                    ATH_MSG_DEBUG("pos: ("<<mesb->globalPosition().x()<<", "
                <<mesb->globalPosition().y()<<", "
                <<mesb->globalPosition().z()<<")");
              ATH_MSG_DEBUG("residual:"<<residual<<", errSq: "<<errSq<<", err="<<std::sqrt(errSq));
            }
          }
          delete resPull;
        }
        else {
          ATH_MSG_WARNING("Expected measurement for atsos of type "
                          << atsos->dumpType()<<", meas type "<<atsos->measType() );
        }

        Residual res(AlignResidualType(m_resType),Measurement,(ParamDefs)iparam,residual,errSq);
        atsos->addResidual(res);
        dchi2 += res.residualNorm()*res.residualNorm();
        m_nDoF++;
      }
    }
    return dchi2;
  }

  //________________________________________________________________________
  void AlignResidualCalculator::accumulateScattering(const TrackStateOnSurface* tsos)
  {

    const MaterialEffectsOnTrack* meot       = dynamic_cast<const MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
    const TrackParameters*        tpar       = tsos->trackParameters();

    m_qOverP      = tpar->charge()/tpar->pT()*sin( tpar->parameters()[Trk::theta]);

    if(!meot) return;
    const EnergyLoss*             energyLoss = meot->energyLoss();


    if (!dynamic_cast<const CaloEnergy*>(energyLoss)) {
      m_previousQOverP     = m_qOverP;
    }

     }

  //________________________________________________________________________
  const TrackStateOnSurface*
  AlignResidualCalculator::getMatchingTSOS(const AlignTSOS* atsos, const Track* track) const
  {
    const TrackStateOnSurface* tsos(nullptr);

    if (atsos->rio() || atsos->crio()) {

      for (const TrackStateOnSurface* itTsos : *track->trackStateOnSurfaces()) {
        if (itTsos->type(TrackStateOnSurface::Outlier))
          continue;

        const MeasurementBase* mesb = itTsos->measurementOnTrack();
        const RIO_OnTrack* rio = dynamic_cast<const RIO_OnTrack*>(mesb);
        const CompetingRIOsOnTrack* crio = dynamic_cast<const CompetingRIOsOnTrack*>(mesb);

        if (!rio && crio) {
          rio = &(crio->rioOnTrack(0));
        }
        if (!rio) continue;
        if (rio->identify() == atsos->rio()->identify()) {
          ATH_MSG_DEBUG("matched TSOS with identifier: "<<rio->identify());
          tsos=itTsos;
          break;
        }
      }
      ATH_MSG_DEBUG("done with measurement");
    }
    else {

      const Amg::Vector3D origPosition=atsos->trackParameters()->position();
      double distance2(1.e27);

      // loop over track and get closest TSOS
      for (const TrackStateOnSurface* itTsos : *track->trackStateOnSurfaces()) {
        if (itTsos->type(TrackStateOnSurface::Outlier))
          continue;
        if (!dynamic_cast<const MaterialEffectsOnTrack*>(itTsos->materialEffectsOnTrack())) continue;
        if (!itTsos->trackParameters()) { ATH_MSG_WARNING("no track parameters!"); continue; }
        const Amg::Vector3D newPosition=itTsos->trackParameters()->position();
        ATH_MSG_DEBUG("origPos: "<<origPosition<<", newPos: "<<newPosition);
        double newdist2=(newPosition - origPosition).mag2();
        if (newdist2<distance2) {
          distance2=newdist2;
          tsos=itTsos;
        }
      }
      ATH_MSG_DEBUG("done with scatterer");
    }
    if (!tsos) return nullptr;
    const Amg::Vector3D addPosition=tsos->trackParameters()->position();
    if (std::find(m_matchedTSOS.begin(),m_matchedTSOS.end(),tsos)==m_matchedTSOS.end()) {
      m_matchedTSOS.push_back(tsos);
      ATH_MSG_DEBUG("added tsos with pos: "<<addPosition);
    }
    else {
      ATH_MSG_WARNING("TSOS already found with position "<<addPosition<<"!");
      //return 0;
    }
    return tsos;
  }

} // end namespace
