/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

#include "TrkAlignEvent/AlignTrack.h"

namespace Trk {

  //________________________________________________________________________
  AlignTrack::AlignTrack(const Track& associatedTrack) 
    : Track(associatedTrack)
    , m_originalTrack(0)
    , m_type(Unknown)
    , m_nAlignTSOSMeas(0)
    , m_alignTSOSCollection(0)
    , m_localErrorMat(0)
    , m_localErrorMatInv(0)
    , m_derivativeMatrix(0)
    , m_fullCovarianceMatrix(0)
    , m_derivatives(0)
    , m_derivativeErr(0)
    , m_actualSecondDerivatives(0)
    , m_residuals(0)
    , m_weights(0)
    , m_weightsFirstDeriv(0)
    , m_chi2(0.)
    , m_chi2dof(0.)
    , m_trackAlignParamQuality(new double[6])
    , m_trackWithoutScattering(0)
  {
    for (int i=0;i<6;i++) m_trackAlignParamQuality[i]=0.;
    m_vtx=0;
    m_refitD0=true;
    m_refitZ0=true;
    m_refitPhi=true;
    m_refitTheta=true;
    m_refitQovP=true;
  }

  //________________________________________________________________________
  AlignTrack::AlignTrack(const Track& associatedTrack, 
                         AlignTSOSCollection* alignTSOSCollection)
    : Track(associatedTrack)
    , m_originalTrack(0)
    , m_type(Unknown)
    , m_nAlignTSOSMeas(0)
    , m_localErrorMat(0)
    , m_localErrorMatInv(0)
    , m_derivativeMatrix(0)
    , m_fullCovarianceMatrix(0)
    , m_derivatives(0)
    , m_derivativeErr(0)
    , m_actualSecondDerivatives(0)
    , m_residuals(0)
    , m_weights(0)
    , m_weightsFirstDeriv(0)
    , m_chi2(0.)
    , m_chi2dof(0.)
    , m_trackAlignParamQuality(new double[6])
    , m_trackWithoutScattering(0)
  {
    for (int i=0;i<6;i++) m_trackAlignParamQuality[i]=0.;
    setAlignTSOSCollection(alignTSOSCollection);
    m_vtx=0;
    m_refitD0=true;
    m_refitZ0=true;
    m_refitPhi=true;
    m_refitTheta=true;
    m_refitQovP=true;
  }

  //________________________________________________________________________
  AlignTrack::AlignTrack(const AlignTrack& atrack)
    : Track(atrack)
    , m_originalTrack(atrack.m_originalTrack)
    , m_type(atrack.m_type)
    , m_nAlignTSOSMeas(atrack.m_nAlignTSOSMeas)
    , m_localErrorMat(atrack.m_localErrorMat ?
        new Amg::SymMatrixX(*(atrack.m_localErrorMat)) : 0)
    , m_localErrorMatInv(atrack.m_localErrorMatInv ?
        new Amg::SymMatrixX(*(atrack.m_localErrorMatInv)) : 0)
    , m_derivativeMatrix(atrack.m_derivativeMatrix ?
        new Amg::MatrixX(*(atrack.m_derivativeMatrix)) : 0)
    , m_fullCovarianceMatrix(atrack.m_fullCovarianceMatrix ?
        new Amg::SymMatrixX(*(atrack.m_fullCovarianceMatrix)) : 0)
    , m_derivatives(atrack.m_derivatives ?
        new std::vector<AlignModuleDerivatives>(*(atrack.m_derivatives)) : 0)
    , m_derivativeErr(atrack.m_derivativeErr ?
        new std::vector<AlignModuleDerivatives>(*(atrack.m_derivativeErr)) : 0)
    , m_actualSecondDerivatives(atrack.m_actualSecondDerivatives ?
        new std::vector<std::pair<const AlignModule*,std::vector<double> > > (*(atrack.m_actualSecondDerivatives)) : 0)
    , m_residuals(atrack.m_residuals ?
        new Amg::VectorX(*(atrack.m_residuals)) : 0)
    , m_weights(atrack.m_weights ?
        new Amg::SymMatrixX(*(atrack.m_weights)) : 0)
    , m_weightsFirstDeriv(atrack.m_weightsFirstDeriv ?
        new Amg::SymMatrixX(*(atrack.m_weightsFirstDeriv)) : 0)
    , m_chi2(atrack.m_chi2)
    , m_chi2dof(atrack.m_chi2dof)
    , m_trackAlignParamQuality(new double[6])
    , m_trackWithoutScattering(atrack.m_trackWithoutScattering ?
  new Trk::Track(*(atrack.m_trackWithoutScattering)) : 0)
  {
    for (int i=0;i<6;i++)
      m_trackAlignParamQuality[i] = atrack.m_trackAlignParamQuality[i];

    // copy AlignTSOSCollection
    if(atrack.m_alignTSOSCollection!=0) {
      AlignTSOSCollection * aTSOScoll = new AlignTSOSCollection;
      aTSOScoll->reserve(atrack.m_alignTSOSCollection->size());
      AlignTSOSIt itAtsos_end = atrack.m_alignTSOSCollection->end();
      for(AlignTSOSIt itAtsos = atrack.m_alignTSOSCollection->begin(); itAtsos!=itAtsos_end; ++itAtsos) {
        assert(*itAtsos!=0); // check that is defined.
        AlignTSOS * atsos = new AlignTSOS(**itAtsos);
        aTSOScoll->push_back(atsos);
      }
      m_alignTSOSCollection=aTSOScoll;
    }
    else
      m_alignTSOSCollection=0;

    findPerigee();
    m_vtx=0;
    m_refitD0=true;
    m_refitZ0=true;
    m_refitPhi=true;
    m_refitTheta=true;
    m_refitQovP=true;
  }

  //________________________________________________________________________
  AlignTrack& AlignTrack::operator= (const AlignTrack& atrack)
  {
    if (this!=&atrack) {

      // assign Track content
      Track::operator=(atrack);

      // now fill the extra stuff
      m_originalTrack  = atrack.m_originalTrack;
      m_type           = atrack.m_type;
      m_nAlignTSOSMeas = atrack.m_nAlignTSOSMeas;
      m_chi2           = atrack.m_chi2;
      m_chi2dof        = atrack.m_chi2dof;
      m_vtx            = atrack.m_vtx;
      m_refitD0        = atrack.m_refitD0;
      m_refitZ0        = atrack.m_refitZ0;
      m_refitPhi       = atrack.m_refitPhi;
      m_refitTheta     = atrack.m_refitTheta;
      m_refitQovP      = atrack.m_refitQovP;

      for (int i=0;i<6;i++)
        m_trackAlignParamQuality[i] = atrack.m_trackAlignParamQuality[i];

      // copy AlignTSOSCollection
      delete m_alignTSOSCollection;
      if(atrack.m_alignTSOSCollection!=0) {
        AlignTSOSCollection * aTSOScoll = new AlignTSOSCollection;
        aTSOScoll->reserve(atrack.m_alignTSOSCollection->size());
        int count(0);
        AlignTSOSIt itAtsos_end = atrack.m_alignTSOSCollection->end();
        for(AlignTSOSIt itAtsos = atrack.m_alignTSOSCollection->begin(); itAtsos!=itAtsos_end; ++itAtsos) {
          ++count;
          assert(*itAtsos!=0); // check that is defined.
          AlignTSOS * atsos = new AlignTSOS(**itAtsos);
          aTSOScoll->push_back(atsos);
        }
        m_alignTSOSCollection=aTSOScoll;
      }
      else
        m_alignTSOSCollection=0;

      findPerigee();

      // fill matrices, vectors, etc.
      delete m_derivatives;
      m_derivatives = atrack.m_derivatives ?
          new std::vector<AlignModuleDerivatives>(*(atrack.m_derivatives)) : 0;
      delete m_derivativeErr;
      m_derivativeErr = atrack.m_derivativeErr ?
          new std::vector<AlignModuleDerivatives>(*(atrack.m_derivativeErr)) : 0;
      delete m_actualSecondDerivatives;
      m_actualSecondDerivatives = atrack.m_actualSecondDerivatives ?
          new std::vector<std::pair<const AlignModule*,std::vector<double> > > (*(atrack.m_actualSecondDerivatives)) : 0;
      delete m_localErrorMat;
      m_localErrorMat = atrack.m_localErrorMat ?
          new Amg::SymMatrixX(*(atrack.m_localErrorMat)) : 0;
      delete m_localErrorMatInv;
      m_localErrorMatInv = atrack.m_localErrorMatInv ?
          new Amg::SymMatrixX(*(atrack.m_localErrorMatInv)) : 0;
      delete m_derivativeMatrix;
      m_derivativeMatrix = atrack.m_derivativeMatrix ?
          new Amg::MatrixX(*(atrack.m_derivativeMatrix)) : 0;
      delete m_fullCovarianceMatrix;
      m_fullCovarianceMatrix = atrack.m_fullCovarianceMatrix ?
          new Amg::SymMatrixX(*(atrack.m_fullCovarianceMatrix)) : 0;
      delete m_residuals;
      m_residuals = atrack.m_residuals ?
          new Amg::VectorX(*(atrack.m_residuals)) : 0;
      delete m_weights;
      m_weights = atrack.m_weights ?
          new Amg::SymMatrixX(*(atrack.m_weights)) : 0;
      delete m_weightsFirstDeriv;
      m_weightsFirstDeriv = atrack.m_weightsFirstDeriv ?
          new Amg::SymMatrixX(*(atrack.m_weightsFirstDeriv)) : 0;
    }

    return *this;
  }

  //________________________________________________________________________
  AlignTrack::~AlignTrack()
  {
    delete m_alignTSOSCollection;     m_alignTSOSCollection=0;
    delete m_residuals;               m_residuals=0;
    delete m_weights;                 m_weights=0;
    delete m_weightsFirstDeriv;       m_weightsFirstDeriv=0;

    delete m_derivatives;             m_derivatives=0;
    delete m_derivativeErr;           m_derivativeErr=0;
    delete m_actualSecondDerivatives; m_actualSecondDerivatives=0;

    delete m_localErrorMat;           m_localErrorMat=0;
    delete m_localErrorMatInv;        m_localErrorMatInv=0;

    delete m_fullCovarianceMatrix;    m_fullCovarianceMatrix=0;
    delete m_derivativeMatrix;        m_derivativeMatrix=0;

    delete [] m_trackAlignParamQuality;

    delete m_trackWithoutScattering;
  }

  //________________________________________________________________________
  bool AlignTrack::isSLTrack(const Track* track) 
  {
    const Perigee* pp = track->perigeeParameters();
    if( pp ){
      
      
      const AmgSymMatrix(5)* covMatrix = pp->covariance();
      if( covMatrix ){
        // sum covariance terms of momentum, use it to determine whether fit was SL fit
        double momCov = 0.;
        for( int i=0;i<5;++i )
          momCov += fabs( (*covMatrix)(4,i));
        for( int i=0;i<4;++i )
          momCov += fabs( (*covMatrix)(i,4));
        if( momCov < 1e-10 )
          return true;
      }
    }
    return false;
  }

  //________________________________________________________________________
  bool AlignTrack::isSLTrack() const
  {
    return isSLTrack(this);
  }

  //________________________________________________________________________
  void AlignTrack::dumpTrackInfo(const Track& track, MsgStream& msg) 
  {
    for (const TrackStateOnSurface* tsos : *track.trackStateOnSurfaces())
      msg<<*tsos;
  }

  //________________________________________________________________________
  void AlignTrack::dumpLessTrackInfo(const Track& track, MsgStream& msg) 
  {
    int ntsos(0);
    for (const TrackStateOnSurface* tsos : *track.trackStateOnSurfaces()) {
      msg<<"ntsos "<<ntsos<<":"<<", type "<<tsos->dumpType();
      //msg << " perigee center of this TSOS: "<< tsos->trackParameters()->associatedSurface()->center() << endmsg;

      if (tsos->type(TrackStateOnSurface::Perigee)) 
        msg << ", Perigee"<<endmsg;

      else if (tsos->type(TrackStateOnSurface::Outlier))
        msg << ", Outlier"<<endmsg;

      else if ( !tsos->type(TrackStateOnSurface::Scatterer) &&
                !tsos->type(TrackStateOnSurface::InertMaterial)) {
        if ( tsos->fitQualityOnSurface()!=0) 
          msg << "," << *( tsos->fitQualityOnSurface() )<<endmsg;
        else
          msg << ", no FitQuality!"<<endmsg;
      }

      else {

        const MaterialEffectsBase*     meb = tsos->materialEffectsOnTrack();
        const MaterialEffectsOnTrack* meot = dynamic_cast<const MaterialEffectsOnTrack*>(meb);

        if (meot) {
          msg<<", meot";
          if (meot->scatteringAngles()) 
            msg<<", have angles"<<endmsg;
          else
            msg<<", no angles"<<endmsg;
        }
        else if (tsos->type(TrackStateOnSurface::InertMaterial))
          msg<<", InertMaterial"<<endmsg;
        else
          msg << ", hmm... no material effects on track!"<<endmsg;
      }
      ++ntsos;
    }
  }

  //________________________________________________________________________
  void AlignTrack::dump(MsgStream& msg) 
  {
    msg<<"dumping AlignTrack"<<endmsg;
    double chi2=0.;
    int imeas(1);
    if (m_alignTSOSCollection) {
      int natsos = m_alignTSOSCollection->size();
      for (int iatsos=0;iatsos<natsos;iatsos++) {
        const AlignTSOS* atsos=(*m_alignTSOSCollection)[iatsos];
        std::vector<Residual>::const_iterator itRes     = atsos->firstResidual();
        std::vector<Residual>::const_iterator itRes_end = atsos->lastResidual();
        for ( ; itRes != itRes_end; ++itRes,++imeas) {
          double resNorm = itRes->residualNorm();
          chi2 += resNorm*resNorm;
          //msg<<"resNorm="<<resNorm
          //   <<", errorMat("<<imeas<<")="<<(*m_localErrorMat)(imeas,imeas)
          //   <<endmsg;
        }
        msg<<"iatsos "<<iatsos<<", chi2="<<chi2<<*atsos<<endmsg;
      }
    }
    msg<<"total chi2: "<<m_chi2<<endmsg;
  }

  //________________________________________________________________________
  void AlignTrack::setAlignTSOSCollection(const AlignTSOSCollection* atsosColl)
  {
    m_alignTSOSCollection=atsosColl;

    // calculate m_nAlignTSOSMeas
    int natsos = m_alignTSOSCollection->size();
    for (int iatsos=0;iatsos<natsos;iatsos++)
      m_nAlignTSOSMeas += (*m_alignTSOSCollection)[iatsos]->nResDim();

    // calculate V and Vinv    
    m_localErrorMat    = new Amg::SymMatrixX(m_nAlignTSOSMeas,m_nAlignTSOSMeas); 
    m_localErrorMat->setZero();
    m_localErrorMatInv = new Amg::SymMatrixX(m_nAlignTSOSMeas,m_nAlignTSOSMeas);
    m_localErrorMatInv->setZero();
 
    int matrixindex(0);
    AlignTSOSCollection::const_iterator itAtsos     = firstAtsos();
    AlignTSOSCollection::const_iterator itAtsos_end = lastAtsos();
    for ( ; itAtsos != itAtsos_end; ++itAtsos) {
      const AlignTSOS * atsos = *itAtsos;
      std::vector<Residual>::const_iterator itRes     = atsos->firstResidual();
      std::vector<Residual>::const_iterator itRes_end = atsos->lastResidual();
      for ( ; itRes != itRes_end; ++itRes) {
        (*m_localErrorMat)(matrixindex,matrixindex) = itRes->errSq();
        (*m_localErrorMatInv)(matrixindex,matrixindex) = 1./itRes->errSq();
        matrixindex++;
      }
    }
  }

  //________________________________________________________________________
  MsgStream& operator<< (MsgStream& sl, const AlignTrack::AlignTrackType type)
  {
    switch(type) {
      case AlignTrack::Unknown:
        return sl<<"Unknown             ";
      case AlignTrack::Original:
        return sl<<"Original            ";
      case AlignTrack::NormalRefitted:
        return sl<<"NormalRefitted      ";
      case AlignTrack::BeamspotConstrained:
        return sl<<"BeamspotConstrained ";
      case AlignTrack::VertexConstrained:
        return sl<<"VertexConstrained   ";
      default:
        return sl<<"UNDEFINED           ";
    }
  }

  //________________________________________________________________________
  std::ostream& operator<< (std::ostream& sl, const AlignTrack::AlignTrackType type)
  {
    switch(type) {
      case AlignTrack::Unknown:
        return sl<<"Unknown             ";
      case AlignTrack::Original:
        return sl<<"Original            ";
      case AlignTrack::NormalRefitted:
        return sl<<"NormalRefitted      ";
      case AlignTrack::BeamspotConstrained:
        return sl<<"BeamspotConstrained ";
      case AlignTrack::VertexConstrained:
        return sl<<"VertexConstrained   ";
      default:
        return sl<<"UNDEFINED           ";
    }
  }

  //________________________________________________________________________
  std::string dumpAlignTrackType(const AlignTrack::AlignTrackType type)
  {
    switch(type) {
      case AlignTrack::Unknown:              return "Unknown";
      case AlignTrack::Original:             return "Original";
      case AlignTrack::NormalRefitted:       return "NormalRefitted";
      case AlignTrack::BeamspotConstrained:  return "BeamspotConstrained";
      case AlignTrack::VertexConstrained:    return "VertexConstrained";
      default:                               return "UNDEFINED";
    }
  }

  //________________________________________________________________________
  const Trk::Track* AlignTrack::trackWithoutScattering() const
  {
    if (!m_trackWithoutScattering) {

      const DataVector<const Trk::TrackStateOnSurface>* states = this->trackStateOnSurfaces();
      if (!states) return 0;
      
      // loop over TSOSs
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
      
      // This is the list of new TSOS 
      DataVector<const Trk::TrackStateOnSurface>* newTrackStateOnSurfaces = 
          new DataVector<const Trk::TrackStateOnSurface>();
      newTrackStateOnSurfaces->reserve( states->size() );
      
      for (; tsit!=tsit_end ; ++tsit) {
        const Trk::MeasurementBase*     newMeas  = (*tsit)->measurementOnTrack() ? (*tsit)->measurementOnTrack()->clone() : 0;
        const Trk::TrackParameters*     newPars  = (*tsit)->trackParameters() ? (*tsit)->trackParameters()->clone() : 0;
        const Trk::FitQualityOnSurface* newFitQoS= (*tsit)->fitQualityOnSurface() ? (*tsit)->fitQualityOnSurface()->clone() : 0;
        const Trk::MaterialEffectsBase* meb      = (*tsit)->materialEffectsOnTrack() ? (*tsit)->materialEffectsOnTrack()->clone() : 0;
  
        if (meb) {
          const Trk::MaterialEffectsOnTrack* meot=dynamic_cast<const Trk::MaterialEffectsOnTrack*>(meb);
          if (meot) {
            double tinX0=meot->thicknessInX0();
            const Trk::EnergyLoss* eLoss=meot->energyLoss() ? meot->energyLoss()->clone() : 0;
            const Trk::Surface& surf=meot->associatedSurface();
            std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes> typeMaterial;
            if (eLoss) typeMaterial.set(MaterialEffectsBase::EnergyLossEffects);
            const Trk::MaterialEffectsOnTrack* newmeot=
                new Trk::MaterialEffectsOnTrack(tinX0,0,eLoss,surf,typeMaterial);
            delete meb;
            meb=newmeot;
          }
        }
  
        std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        for (int i=0;i<(int)Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes;i++) {
          if ((*tsit)->type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType(i)))
            typePattern.set(i);
        }
        const Trk::TrackStateOnSurface* newTsos= new Trk::TrackStateOnSurface( newMeas, newPars, newFitQoS, meb, typePattern);
        newTrackStateOnSurfaces->push_back(newTsos);
      }
      
      m_trackWithoutScattering = new Trk::Track( this->info(), newTrackStateOnSurfaces, 
                                                 this->fitQuality() ? 
                                                 this->fitQuality()->clone():0 );
    }
    return m_trackWithoutScattering;
  }
  
} // end namespace
