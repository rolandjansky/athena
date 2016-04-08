/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkSurfaces/Surface.h"

#include <iostream>

namespace Trk {

  //________________________________________________________________________
  AlignTSOS::AlignTSOS(const TrackStateOnSurface& tsos, 
                       const AlignModule * module, 
                       const RIO_OnTrack * rio,
                       TrackState::MeasurementType measType)
    : TrackStateOnSurface(tsos)
    , m_module(module)
    , m_rio(rio)
    , m_crio(0)
    , m_unbiasedTrackPars(0)
    , m_derivatives(0)
    , m_isvalid(true)
    , m_measType(measType)
    , m_measDir(Trk::x)
    , m_alphaStrip(0)
    , m_derivativesVtx(0)  
  {
    if(!module)
      m_isvalid = false;

    const TrkDetElementBase * detelement(0);
    detelement = tsos.measurementOnTrack()->associatedSurface().associatedDetectorElement();
    
    m_detelements.push_back(detelement);
  }

  //________________________________________________________________________
  AlignTSOS::AlignTSOS(const TrackStateOnSurface& tsos, 
                       const AlignModule* module, 
                       const CompetingRIOsOnTrack* crio,
                       TrackState::MeasurementType measType)           
    : TrackStateOnSurface(tsos)
    , m_module(module)
    , m_rio(0)
    , m_crio(crio)
    , m_unbiasedTrackPars(0)
    , m_derivatives(0)
    , m_isvalid(true)
    , m_measType(measType)
    , m_measDir(Trk::x)
    , m_alphaStrip(0)
    , m_derivativesVtx(0) 
  {
    if(!module)
      m_isvalid = false;

    for (unsigned int irio = 0; irio < crio->numberOfContainedROTs(); irio++) {
      const Trk::RIO_OnTrack& rio = crio->rioOnTrack(irio);
      m_detelements.push_back(rio.associatedSurface().associatedDetectorElement());
    }
  }

  //________________________________________________________________________
  AlignTSOS::AlignTSOS(const TrackStateOnSurface& tsos, 
                       TrackState::MeasurementType measType)
    : TrackStateOnSurface(tsos)
    , m_module(0)
    , m_rio(0)
    , m_crio(0)
    , m_unbiasedTrackPars(0)
    , m_derivatives(0)
    , m_isvalid(false)
    , m_measType(measType)
    , m_measDir(Trk::x)
    , m_alphaStrip(0)
    , m_derivativesVtx(0)  
  {
    m_detelements.push_back(0);
  }

  //________________________________________________________________________
  AlignTSOS::~AlignTSOS()
  {
    delete m_unbiasedTrackPars;
    delete m_derivatives;
  }

  //________________________________________________________________________
  AlignTSOS::AlignTSOS(const AlignTSOS& atsos)
    : TrackStateOnSurface(atsos)
    , m_module(atsos.m_module)
    , m_rio(atsos.m_rio)
    , m_crio(atsos.m_crio)
    , m_unbiasedTrackPars(atsos.m_unbiasedTrackPars ? atsos.m_unbiasedTrackPars->clone() : 0)
    , m_residuals(atsos.m_residuals)
    , m_detelements(atsos.m_detelements)
    , m_derivatives(atsos.m_derivatives ? new std::vector<Amg::VectorX>(*(atsos.m_derivatives)) : 0)
    , m_isvalid(atsos.m_isvalid)
    , m_measType(atsos.m_measType)
    , m_measDir(atsos.m_measDir)
    , m_alphaStrip(atsos.m_alphaStrip)
    , m_derivativesVtx(0)
  {
  }

  //________________________________________________________________________
  AlignTSOS& AlignTSOS::operator=(const AlignTSOS& atsos)
  {
    if(this != &atsos) {

      // assign TrackStateOnSurface contents
      TrackStateOnSurface::operator=(atsos);

      // copy the rest
      m_module = atsos.m_module;
      m_rio = atsos.m_rio;
      m_crio = atsos.m_crio;

      m_residuals.clear();
      m_residuals = atsos.m_residuals;

      m_detelements.clear();
      m_detelements = atsos.m_detelements;

      m_isvalid = atsos.m_isvalid;
      m_measType = atsos.m_measType;
      m_measDir = atsos.m_measDir;
      m_alphaStrip = atsos.m_alphaStrip;

      delete m_unbiasedTrackPars;
      m_unbiasedTrackPars = atsos.m_unbiasedTrackPars ? atsos.m_unbiasedTrackPars->clone() : 0;

      delete m_derivatives;
      m_derivatives = atsos.m_derivatives ? new std::vector<Amg::VectorX>(*(atsos.m_derivatives)) : 0;
    }

    return *this;
  }

  //________________________________________________________________________
  Identifier AlignTSOS::identify() const 
  {
    Identifier id(0);
    const MeasurementBase* mesb = this->measurementOnTrack();
    const RIO_OnTrack* rio = dynamic_cast<const RIO_OnTrack*>(mesb);
    if (rio) id=rio->identify();
    return id;
  }

  //________________________________________________________________________
  const RIO_OnTrack* AlignTSOS::rio() const {
    if (m_rio) return m_rio; 
    else if (m_crio) return &(m_crio->rioOnTrack(0));
    else return 0;
  }
  
  //________________________________________________________________________
  MsgStream& operator << ( MsgStream& sl, const AlignTSOS& atsos)
  { 
    //sl <<"\t"<<atsos.dumpType()
    // << std::setw(11) << std::showpos<< std::scientific
    // <<"\tres[0]="<<atsos.residual(0)<<", err[0]="<<atsos.residualErrorSq(0)
    // <<", dchi2="<<std::noshowpos
    // <<atsos.residual(0)*atsos.residual(0)/atsos.residualErrorSq(0)<<endreq;
    sl <<"\t"<<atsos.dumpType()<<" ("<<atsos.dumpMeasType()<<", direction "<<atsos.dumpMeasDir()<<")"<<endreq;
    sl << "          -->  ";

    int ires(0);
    std::vector<Residual>::const_iterator itRes     = atsos.firstResidual();
    std::vector<Residual>::const_iterator itRes_end = atsos.lastResidual();
    for( ; itRes != itRes_end ; ++itRes, ++ires) {
      double resnorm = itRes->residualNorm();
      if(ires){ 
        std::ios_base::fmtflags f(sl.stream().flags());  
        sl << endreq << "          -->  ";
        sl << ires << ": res="
           << std::setw(11)     << std::showpos << std::scientific
           << itRes->residual() <<"  res/err="<<resnorm
           << ", dchi2="<<std::noshowpos<<resnorm*resnorm
	   << resetiosflags(std::ios::floatfield);
        sl.flags(f);  
      }
    }
    return sl;
  }

  //________________________________________________________________________
  std::string AlignTSOS::dumpMeasType() const
  {
    switch(m_measType) {
      case Trk::TrackState::unidentified: return "none";
      case Trk::TrackState::Pixel:        return "Pixel";
      case Trk::TrackState::SCT:          return "SCT";
      case Trk::TrackState::TRT:          return "TRT";
      case Trk::TrackState::MDT:          return "MDT";
      case Trk::TrackState::CSC:          return "CSC";
      case Trk::TrackState::RPC:          return "RPC";
      case Trk::TrackState::TGC:          return "TGC";
      case Trk::TrackState::Pseudo:       return "Pseudo";
      case Trk::TrackState::Vertex:       return "Vertex";
      case Trk::TrackState::Segment:      return "Segment";
      case Trk::TrackState::SpacePoint:   return "SpacePoint";
      default: return "UNDEFINED";
    }
  }

  //________________________________________________________________________
  std::string AlignTSOS::dumpMeasDir() const
  {
    switch(m_measDir) {
      case Trk::x: return "X";
      case Trk::y: return "Y";
      case Trk::z: return "Z";
      default: return "?";
    }
  }

} // end namespace

