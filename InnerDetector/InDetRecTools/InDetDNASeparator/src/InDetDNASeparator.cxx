/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDNASeparator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetDNASeparator/InDetDNASeparator.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "GaudiKernel/SystemOfUnits.h"
//#include "TrkParameters/MeasuredTrackParameters.h"
// #include "TrkFitterUtils/ProtoTrajectoryUtility.h"


//================ Constructor ===============================================
const double InDet::InDetDNASeparator::s_sigmaNoDna = -100.0;

InDet::InDetDNASeparator::InDetDNASeparator(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_kutoff(4.50),
  m_floorseparation_drr(4.00*Gaudi::Units::mm),
  // Initialization of ID SCT helper
  m_SCT_Tool(nullptr),
  // Initialization of ID Pixel helper
  m_pixelTool(nullptr)
{
  declareInterface<ITrackBreakpointAnalyser>(this);

  //  template for property declaration
  //declareProperty("PropertyName", m_propertyName);
  declareProperty("kutoff", m_kutoff);
}

//================ Destructor =================================================

InDet::InDetDNASeparator::~InDetDNASeparator()
{}


//================ Initialisation =================================================

StatusCode InDet::InDetDNASeparator::initialize()
{
 
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_SCT_Tool, "SCT_ID");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR ("Could not get SCT_ID helper !");
      return StatusCode::FAILURE;
    }

  sc = detStore()->retrieve(m_pixelTool, "PixelID");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR ("Could not get PixelID helper !");
      return StatusCode::FAILURE;
    }

  //  m_utility = new ProtoTrajectoryUtility(m_pixelTool);
  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::InDetDNASeparator::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  //  delete m_utility;
  return sc;
}


//============================================================================================

Trk::TrackBreakpointType InDet::InDetDNASeparator::confirmBreakpoint(const Trk::Trajectory& trajectory) const
{ 
  double sigmax = breakpointSignificance(trajectory);

  if(sigmax == s_sigmaNoDna ) return (Trk::NoBreakpoint); // no brem objects found

  if(sigmax > m_kutoff) return (Trk::DnaBremPointUseful); // significant brem
	 
  return (Trk::DnaBremPointNotUseful); // sigma calculated but not significant
}
// 
//   Returns  1 if DNA never kicked in during the fit, hence DNA = kf
//
//   Returns  3 if DNA kicked in, but momentum did not change => REFIT without DNA
//
//   Returns  2 if DNA kicked in and changed momentum => DNA fit is good
//

double InDet::InDetDNASeparator::breakpointSignificance
(const Trk::Trajectory& trajectory) const
{ 

  /* prepare trajectory information in layers or 'floor levels', averaging
     over module front and backsides and overlaps. Information per floor
     level is held in the MeanTrackParsAtFloor helper class. */

  std::vector<Trk::MeanTrackParsAtFloor> newPars; newPars.reserve(10);
  Trk::MeanTrackParsAtFloor meanPars;
  double rxy_old = 0.0;
  Trk::Trajectory::const_reverse_iterator rvit  = trajectory.rbegin();
  Trk::Trajectory::const_reverse_iterator innermostTrtHit = trajectory.rend();
  for ( ; rvit != trajectory.rend() ; ++rvit) {
    if (!rvit->measurement() || !rvit->smoothedTrackParameters()
        || rvit->isOutlier() ) continue;
    Identifier id=rvit->measurement()->associatedSurface().associatedDetectorElementIdentifier();
    if (m_pixelTool->is_trt(id)) innermostTrtHit = rvit;
    if (m_pixelTool->is_pixel(id) || m_SCT_Tool->is_sct(id)) {

      // insert one (innermost) TRT hit to allow analysing brem at last SCT layer
      if (rxy_old == 0.0 && (innermostTrtHit != trajectory.rend()) ) {
        meanPars.addTrackParsToFloor(*(innermostTrtHit->smoothedTrackParameters()),
                                     innermostTrtHit->dnaMaterialEffects(), 0);
        rxy_old = innermostTrtHit->smoothedTrackParameters()->position().perp();
      }
      // define new floor if there was previous one and distance to current large enough
      double current_dr = fabs(rxy_old-rvit->smoothedTrackParameters()->position().perp());
      if (rxy_old > 0.0 && (current_dr > m_floorseparation_drr)) {
        newPars.push_back(meanPars);
        meanPars = Trk::MeanTrackParsAtFloor();
      }
      // create-new-floor or add-to-average of existing floor
      meanPars.addTrackParsToFloor(*(rvit->smoothedTrackParameters()),
                                   rvit->dnaMaterialEffects(), 0);
      rxy_old = rvit->smoothedTrackParameters()->position().perp();
    }
  }
  newPars.push_back(meanPars);

  // calculate the significance of the brem objects found on track

  return calculateSignificance(newPars).at(0);
}

Trk::TrackBreakpointType InDet::InDetDNASeparator::confirmBreakpoint(const Trk::Track& track) const
{ 
  double sigmax = breakpointSignificance(track);
  if(sigmax == s_sigmaNoDna ) return (Trk::NoBreakpoint); // no brem objects found
  if(sigmax > m_kutoff) return (Trk::DnaBremPointUseful); // significant brem
  return (Trk::DnaBremPointNotUseful); // sigma calculated but not significant
}

double InDet::InDetDNASeparator::breakpointSignificance(const Trk::Track& track) const
{ 
  if (track.trackStateOnSurfaces() == NULL ||
      track.trackStateOnSurfaces()->size() < 2) return s_sigmaNoDna;

  /* prepare trajectory information in layers or 'floor levels', averaging
     over module front and backsides and overlaps. Information per floor
     level is held in the MeanTrackParsAtFloor helper class. */

  std::vector<Trk::MeanTrackParsAtFloor> newPars; newPars.reserve(10);
  Trk::MeanTrackParsAtFloor meanPars;
  double rxy_old = 0.0;
  const std::vector<const Trk::TrackStateOnSurface*>& tsos = 
    track.trackStateOnSurfaces()->stdcont();
  std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator rvit  = tsos.rbegin();
  std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator innermostTrt = tsos.rend();
  for ( ; rvit != tsos.rend() ; ++rvit) {
    if (!(*rvit)->measurementOnTrack() || !(*rvit)->trackParameters()
        || !(*rvit)->type(Trk::TrackStateOnSurface::Measurement) ) continue;
    Identifier 
      id=(*rvit)->measurementOnTrack()->associatedSurface().associatedDetectorElementIdentifier();
    if (m_pixelTool->is_trt(id)) innermostTrt = rvit;
    if (m_pixelTool->is_pixel(id) || m_SCT_Tool->is_sct(id)) {

      // insert one (innermost) TRT hit to allow analysing brem at last SCT layer
      if (rxy_old == 0.0 && (innermostTrt != tsos.rend()) ) {
        const Trk::EstimatedBremOnTrack* ebr = dynamic_cast<const Trk::EstimatedBremOnTrack*>
          ((*innermostTrt)->materialEffectsOnTrack());
        meanPars.addTrackParsToFloor(*((*innermostTrt)->trackParameters()),0,ebr);
        rxy_old = (*innermostTrt)->trackParameters()->position().perp();
      }
      // define new floor if there was previous one and distance to current large enough
      double current_dr = fabs(rxy_old-(*rvit)->trackParameters()->position().perp());
      if (rxy_old > 0.0 && (current_dr > m_floorseparation_drr)) {
        newPars.push_back(meanPars);
        meanPars = Trk::MeanTrackParsAtFloor();
      }
      // create-new-floor or add-to-average of existing floor
      const Trk::EstimatedBremOnTrack* ebr = dynamic_cast<const Trk::EstimatedBremOnTrack*>
        ((*rvit)->materialEffectsOnTrack());
      meanPars.addTrackParsToFloor(*((*rvit)->trackParameters()), 0, ebr);
      rxy_old = (*rvit)->trackParameters()->position().perp();
    }
  }
  newPars.push_back(meanPars);

  // calculate the significance of the brem objects found on track

  return calculateSignificance(newPars).at(0);
}


// *********************************************************************************************
std::vector<double> InDet::InDetDNASeparator::calculateSignificance
(const std::vector<Trk::MeanTrackParsAtFloor> MTPF ) const
{
  double sigmax=s_sigmaNoDna, dphmax=-9.0, rasmax=0.0, dnamax=0.0;
  double sigf,sigb,sigm,dphb,dphf,dphm,rasf,rasb,rasm,dnam=0.0;
  std::vector<double> X = std::vector<double>(4); X.at(0)=s_sigmaNoDna;
      	      
  if ( MTPF.size() <= 1) {
    ATH_MSG_DEBUG ("Number of floors = " << MTPF.size() << " is too small,"
                   << " returning 'no breakpoint'.");
    return X; // or Trk::NoBreakpoint;
  }

  double       charge = (MTPF.back().pts > 0.0) ? 1.0 : -1.0 ;
  unsigned int xfl    = MTPF.size();

  for(unsigned int ifl=0; ifl<MTPF.size(); ++ifl) {

    ATH_MSG_DEBUG ("VATO-SEPARATOR-223-3    xfl ifl nen ras phs ths pts dps dnf dnb pec pfl sec sfl rat drt thr "
                   << xfl           <<"   " << ifl+1 <<"   "
                   << MTPF[ifl].nen <<"   " << MTPF[ifl].ras <<"   "
                   << MTPF[ifl].phs <<"   " 
                   << MTPF[ifl].ths <<"   " 
                   << MTPF[ifl].pts <<"   " 
                   << MTPF[ifl].dps <<"   " 
                   << MTPF[ifl].dnf <<"   " 
                   << MTPF[ifl].dnb <<"   ");

    sigf=0.0,sigb=0.0,dphb=0.0,dphf=0.0,dphm=0.0,rasf=0.0,rasb=0.0,rasm=0.0;
    sigm=s_sigmaNoDna;
	 
    if( (MTPF[ifl].dnb > 0.0) && (ifl+1 < xfl) ) {

      // note that a negative sigb,sigf is possible when a mis-reconstruction
      // suggests an increasing momentum

      if(MTPF[ifl+1].pts!=0.0 && MTPF[ifl].pts!=0.0 && MTPF[ifl].dps!=0.0) {
        sigb = (1.0-fabs(MTPF[ifl].pts)/fabs(MTPF[ifl+1].pts))/MTPF[ifl].dps;
        dphb = charge*sin(MTPF[ifl+1].phs - MTPF[ifl].phs)
          - 0.3*(MTPF[ifl].ras-MTPF[ifl+1].ras)/fabs(MTPF[ifl].pts);
        rasb = MTPF[ifl].ras;     
        if (sigb<=s_sigmaNoDna) sigb = s_sigmaNoDna+0.1; // protect against mix with bottom value
      } else ATH_MSG_WARNING ("DNA bw activity detected, but momentum quantities are zero!");
    }
	 
    if( (MTPF[ifl].dnf > 0.0) && (ifl > 0)) {

      if(MTPF[ifl-1].pts!=0.0 && MTPF[ifl].pts!=0.0 && MTPF[ifl].dps!=0.0) {
        sigf = (1.0-fabs(MTPF[ifl-1].pts)/fabs(MTPF[ifl].pts))/MTPF[ifl].dps;        
        dphf = charge*sin(MTPF[ifl].phs-MTPF[ifl-1].phs) 
          - 0.3*(MTPF[ifl-1].ras-MTPF[ifl].ras)/fabs(MTPF[ifl-1].pts);
        rasf = MTPF[ifl].ras;
        if (sigf<=s_sigmaNoDna) sigf = s_sigmaNoDna+0.1; // protect against mix with bottom value
      } else ATH_MSG_WARNING ("DNA fw activity detected, but momentum quantities are zero!");
    }
	 
    if (MTPF[ifl].dnf  > 0.0 && MTPF[ifl].dnb == 0.0) {
      sigm=sigf;
      dphm=dphf;
      dnam=MTPF[ifl].dnf;
      rasm=rasf;
    }
	 
    if (MTPF[ifl].dnf == 0.0 && MTPF[ifl].dnb  > 0.0) {
      sigm=sigb;
	  dphm=dphb;
	  dnam=MTPF[ifl].dnb;
	  rasm=rasb;
    }
	 
    if (MTPF[ifl].dnf  > 0.0 && MTPF[ifl].dnb  > 0.0) {
      sigm=sigb+sigf;
      dphm=dphb+dphf;
      dnam=MTPF[ifl].dnf+MTPF[ifl].dnb;
      rasm=(rasb+rasf)/2.;
    }
     
    // look for biggest significance as the one to give out as discriminator
    if ( (MTPF[ifl].dnf  > 0.0 || MTPF[ifl].dnb  > 0.0) && sigm > sigmax) {
      sigmax=sigm;
      dphmax=dphm;
      dnamax=dnam;
      rasmax=rasm;
    }
  }	 

  ATH_MSG_DEBUG ("VATO-SEPARATOR-251 xfl ifl dphi1 dphi2 rat1 rat2 1-thr dps dps1 dps2 dnf dnb pec pfl sec sfl outflag"
                 << xfl    << "   " << rasmax <<"   " << dphmax <<"   "
                 << dnamax << "   " << sigmax <<"   " << m_kutoff <<"   ");
   
  // The four "final" variables: sigmax, dphmax, dnamax, rasmax   
  //  std::vector<double> test = std::vector<double>(sigmax, dphmax, dnamax, rasmax);
  X.at(0)=sigmax; X.at(1)=dphmax; X.at(2)=dnamax; X.at(3)=rasmax;
  return X;
}
