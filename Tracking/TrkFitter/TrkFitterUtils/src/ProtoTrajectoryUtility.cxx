/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProtoTrajectoryUtility, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkFitterUtils/ProtoTrajectoryUtility.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSegment/Segment.h"
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkFitterUtils/DNA_MaterialEffects.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkSurfaces/TrapezoidBounds.h" // identify SCT endcap / TGC strip
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkFitterUtils/ProtoMaterialEffects.h"

Trk::ProtoTrajectoryUtility::ProtoTrajectoryUtility() :
  m_idHelper(nullptr)
{
}

Trk::ProtoTrajectoryUtility::ProtoTrajectoryUtility(const AtlasDetectorID*& id) :
  m_idHelper(id)
{
}

Trk::Trajectory::iterator
Trk::ProtoTrajectoryUtility::firstFittableState(Trk::Trajectory& T) const
{
  Trk::Trajectory::iterator it=T.begin();
  while ( (it->isOutlier() || !it->measurement()) && it!=(T.end()-1)) ++it;
  return it;
}

Trk::Trajectory::iterator
Trk::ProtoTrajectoryUtility::lastFittableState(Trk::Trajectory& T) const
{
  Trk::Trajectory::iterator it=T.end()-1;
  while ((it->isOutlier() || !it->measurement()) && it!=T.begin()) --it;
  return it;
}

Trk::Trajectory::iterator
Trk::ProtoTrajectoryUtility::previousFittableState(const Trk::Trajectory& T,
                                                   Trk::Trajectory::iterator givenState) const
{
  Trk::Trajectory& T2 = const_cast<Trk::Trajectory&>(T); // forces STL code to compile

  if (givenState == T2.begin()) return T2.end(); // signals error condition
  --givenState;                                // can mod it because it is passed by value
  while (givenState->isOutlier() && givenState!=T2.begin()) --givenState;
  if (!givenState->isOutlier()) {
    return givenState;
  } else { 
    return T2.end(); // signals error condition
  }
}

// for speed reasons implement the above with reverse_iterator.
// reverse_iterator.base()-1 would give the correct normal iterator.
Trk::Trajectory::reverse_iterator
Trk::ProtoTrajectoryUtility::previousFittableState(const Trk::Trajectory& T,
                                                   Trk::Trajectory::reverse_iterator givenState) const
{
  // force STL. Otherwise T.end() is a const_iterator, doesn't compile.
  // T2 is not modified.
  Trk::Trajectory& T2 = const_cast<Trk::Trajectory&>(T);

  // STL sucks... givenState == T.rbegin() fails to compile
  if (givenState == T2.rbegin()) return T2.rend(); // signals error condition
  --givenState;
  while (givenState->isOutlier() && givenState!=T2.rbegin()) --givenState;
  if (!givenState->isOutlier()) {
    return givenState;
  } else { 
    return T2.rend(); // signals error condition
  }
}

Trk::Trajectory::iterator
Trk::ProtoTrajectoryUtility::nextFittableState(const Trk::Trajectory& T,
                                               Trk::Trajectory::iterator givenState) const
{
  Trk::Trajectory& T2 = const_cast<Trk::Trajectory&>(T); // forces STL code to compile

  if (++givenState == T2.end()) return T2.end(); // signal error condition
  while (givenState->isOutlier() && (givenState != (T2.end()-1)) ) ++givenState;
  if (!givenState->isOutlier()) {
    return givenState;
  } else { 
    return T2.end();               // signals error condition
  }
}

Trk::Trajectory::reverse_iterator
Trk::ProtoTrajectoryUtility::nextFittableState(const Trk::Trajectory& T,
                                               Trk::Trajectory::reverse_iterator givenState) const
{
  Trk::Trajectory& T2 = const_cast<Trk::Trajectory&>(T); // forces STL code to compile
  if (++givenState == T2.rend()) return T2.rend(); // signal error condition
  while (givenState->isOutlier() && (givenState != (T2.rend()-1)) ) ++givenState;
  if (!givenState->isOutlier()) {
    return givenState;
  } else { 
    return T2.rend();               // signals error condition
  }
}

int Trk::ProtoTrajectoryUtility::numberOfMeasurements(const Trk::Trajectory& T, int i) const
{
  if ( (i<=0) || (i> (int)T.size()) ) i = (int)T.size();
  int number=0;
  Trk::Trajectory::const_iterator it=T.begin();
  for (int k=0; k<=i; ++k, ++it) if (!it->isOutlier()) ++number;
  return number;
}

int Trk::ProtoTrajectoryUtility::rankedNumberOfMeasurements(const Trk::Trajectory& T) const
{
  int number=0;
  for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++)
    if (!it->isOutlier() && it->measurement()) {
      if ( dynamic_cast<const Trk::TrapezoidBounds*>
           (&it->measurement()->associatedSurface().bounds()) ) ++number;
      else
        number += it->measurement()->localCovariance().cols();
    }
  return number;
}

int Trk::ProtoTrajectoryUtility::numberOfOutliers(const Trk::Trajectory& T, int i) const
{
  if ( (i<=0) || (i> (int)T.size()) ) i = (int)T.size();
  int number=0;
  Trk::Trajectory::const_iterator it=T.begin();
  for (int k=0; k<i; ++k, ++it) {if (it->isOutlier()) ++number;}
  return number;
}

int Trk::ProtoTrajectoryUtility::numberOfNewOutliers(const Trk::Trajectory& T, int i) const
{
  if ( (i<=0) || (i> (int)T.size()) ) i = (int)T.size();
  int number=0;
  Trk::Trajectory::const_iterator it=T.begin();
  for (int k=0; k<i; ++k, ++it) {if (it->isNewOutlier()) ++number;}
  return number;
}

int Trk::ProtoTrajectoryUtility::numberOfSpecificStates
(const Trk::Trajectory& T,
 const Trk::TrackState::MeasurementType& mtype,
 const Trk::TrackState::TrackStateType& ttype,
 const int& iterationNumber) const
{
  using namespace Trk::TrackState;
  int number=0;
  if (ttype == AnyState)
    for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++) {
      if ( mtype==it->measurementType() ) ++number;
    }
  else if (ttype == Fittable)
    for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++) {
      if (it->isOutlier()) continue;
      if (mtype==unidentified ||  mtype==it->measurementType() ) ++number;
    }
  else 
    for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++) {
      if (!it->isNewOutlier()) continue;
      if ( (iterationNumber==0 
            || iterationNumber==it->iterationShowingThisOutlier())
           &&
           (mtype==unidentified   ||  mtype==it->measurementType() )
           &&
           (ttype==GeneralOutlier ||  ttype==it->trackStateType() ) ) ++number;
    }
  return number;
}

bool Trk::ProtoTrajectoryUtility::trajectoryHasMefot(const Trk::Trajectory& T) const
{
  Trk::Trajectory::const_iterator it=T.begin();
  while (it!=T.end()) {
    if (!it->isOutlier() && it->dnaMaterialEffects()) {
      if (it->smoothedTrackParameters()->parameters()[Trk::qOverP]==0.0) {
        std::cout << "ProtoTrajectoryUtility: WARNING: Presence of material effects for straight "
                  << "track is unphysical." << std::endl; 
      } else return true;
    }
    ++it;
  } // end while
  return false;
}

void Trk::ProtoTrajectoryUtility::clearFitResultsAfterOutlier(Trk::Trajectory& T,
                                                              Trk::FitQuality*& oldFitQuality,
                                                              int firststate) const
{
  int c=1;
  for (Trk::Trajectory::iterator it=T.begin(); it!=T.end(); ++it, ++c) {
    // MBase pointer: never touch during outlier iterations.

    if (it->dnaMaterialEffects()) {
      // clear all dna after outlier and backward-dna also before outlier
      if ( ! it->dnaMaterialEffects()->foundByForwardFilter()
           || (c>=firststate) ) delete it->checkoutDNA_MaterialEffects();
    }
    if (it->smoothedTrackParameters()) delete it->checkoutSmoothedPar();
    if (c>=firststate) {
      if (it->forwardTrackParameters()) delete it->checkoutForwardPar();
      if (it->parametersDifference())   delete it->checkoutParametersDifference();
      if (it->parametersCovariance())   delete it->checkoutParametersCovariance();
    }
    if (it->fitQuality()) delete it->checkoutFitQuality();
    if (c>=firststate) it->setForwardStateFitQuality(0.0, 0.0);
    it->backwardStateChiSquared(0.0);
  }
  if (oldFitQuality) {
    delete oldFitQuality;
    oldFitQuality=nullptr;
  }
}

void Trk::ProtoTrajectoryUtility::clearFitResultsAndReference(Trk::Trajectory& T,
                                                              Trk::FitQuality*& oldFitQuality,
                                                              int firststate) const
{
  int c=1;
  for (Trk::Trajectory::iterator it=T.begin(); it!=T.end(); ++it, ++c) {
    // MBase pointer: never touch during outlier iterations.

    if (it->dnaMaterialEffects()) {
      // clear all dna after outlier and backward-dna also before outlier
      if ( ! it->dnaMaterialEffects()->foundByForwardFilter()
           || (c>=firststate) ) delete it->checkoutDNA_MaterialEffects();
    }
    if (c>=firststate) {
      if (it->forwardTrackParameters()) delete it->checkoutForwardPar();
      if (it->parametersDifference())   delete it->checkoutParametersDifference();
      if (it->parametersCovariance())   delete it->checkoutParametersCovariance();
      if (it->referenceParameters())    delete it->checkoutReferenceParameters();
      if (it->jacobian())               delete it->checkoutTransportJacobian();
    }
    if (it->smoothedTrackParameters()) delete it->checkoutSmoothedPar();
    if (it->fitQuality()) delete it->checkoutFitQuality();
    if (c>=firststate) it->setForwardStateFitQuality(0.0, 0.0);
    it->backwardStateChiSquared(0.0);
  }
  if (oldFitQuality) {
    delete oldFitQuality;
    oldFitQuality=nullptr;
  }
}


void Trk::ProtoTrajectoryUtility::defineMeasurementsExceptThis(Trk::Trajectory& T,
                                                               int outlierState) const
{
  for (Trk::Trajectory::iterator it=T.begin(); it!=T.end(); it++) {
    if (it->isNewOutlier() && it->positionOnTrajectory()!=outlierState) it->isOutlier(false);
    if (it->positionOnTrajectory()==outlierState) it->isOutlier(Trk::TrackState::PredictedOutlier,1);
  }
}

Trk::FitQuality
Trk::ProtoTrajectoryUtility::forwardFilterQuality(const Trk::Trajectory& T,
                                                  Trk::TrackState::MeasurementType select) const
{
  double chisquared=0.0;
  double ndof = 0.0;
  for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++) {
    if (!it->isOutlier()
        && (select == Trk::TrackState::unidentified
            || select == it->measurementType())) {
      if (it->forwardStateChiSquared() >= 0.0) chisquared += it->forwardStateChiSquared();
      ndof       += it->forwardStateNumberDoF();
    }
  }
  int    ndof_int = ndof>5 ? (int)(ndof-5.0) : 0;
  return FitQuality(chisquared,ndof_int);
}


void Trk::ProtoTrajectoryUtility::identifyMeasurement(Trk::ProtoTrackStateOnSurface& ps) const
{
  if (m_idHelper==nullptr) return;
  Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
  Trk::TrackState::MeasurementType measType = helper.defineType(ps.measurement());
  ps.setMeasurementType(measType, Trk::TrackState::CalibrationNotKnown);
}

void Trk::ProtoTrajectoryUtility::identifyMeasurements(Trk::Trajectory& T) const
{
  if (m_idHelper==nullptr) return;
  Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
  for( Trk::Trajectory::iterator it = T.begin(); it!=T.end(); it++) {
    Trk::TrackState::MeasurementType measType = helper.defineType(it->measurement());
    it->setMeasurementType(measType, Trk::TrackState::CalibrationNotKnown);
  }
}

void Trk::ProtoTrajectoryUtility::dumpTrajectory(const Trk::Trajectory& T, const std::string& name) const
{
  std::cout << name << "'s internal trajectory:" << std::endl;
  for (Trk::Trajectory::const_iterator it=T.begin(); it!=T.end(); it++) {
    std::cout<< (it->positionOnTrajectory()>9?" T":" T0") << it->positionOnTrajectory()
             << (it->measurement()? (it->isOutlier()?(it->isNewOutlier()? " Ou ":" Ol "):" Ms "):" -- ");
    if ( it->dnaMaterialEffects() )
      std::cout << (it->dnaMaterialEffects()->foundByForwardFilter() ? "dnaF " : "dnaB ");
    else if ( it->materialEffects() )
      std::cout    << "MatL ";
    else std::cout << "--   ";
    std::cout<< (it->referenceParameters()?"R":"-") << (it->jacobian()?"J ":"- ") ;
    if (it->forwardTrackParameters()) std::cout<< "fP ";
    else if (it->parametersDifference()) std::cout << (it->parametersCovariance()?"dC ":"d- ");
    else std::cout << "-- ";
    std::cout<< (it->smoothedTrackParameters()? "sP ":"-- ")
             << (it->fitQuality()? "fQ  ":"--  ");

    // FIXME decide if this stays in or not
    if (it->backwardStateChiSquared() > 0.0)
      std::cout << "% "
                << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right )
                << std::setw(5) << /*std::setprecision(1) <<*/ it->forwardStateChiSquared() << "  "
                << std::setw(5) << /*std::setprecision(1) <<*/ it->backwardStateChiSquared() << " % ";
    //                << (it->fitQuality()? it->fitQuality()->chiSquared() : 0.0 ) << "  ";

    // identify the type of measurement
    if ( it->identify().is_valid() && m_idHelper!=nullptr) {
      std::string detIdString = m_idHelper->print_to_string(it->identify());
      int iCutStart = (detIdString.substr(0,10) == "InnerDetec" ) ? 14 : 17;
      std::cout /*<<it->identify()<<","*/ << detIdString.substr(iCutStart,85) << std::endl;
    } else if (nullptr != dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(it->measurement())) {
      const Trk::LocalParameters& P = it->measurement()->localParameters();
      std::cout << "PseudoMeasurement of dim " << P.dimension() << " containing ( ";
      if (P.contains(Trk::locX)) std::cout << "loc1 ";
      if (P.contains(Trk::locY)) std::cout << "loc2 ";
      if (P.contains(Trk::phi)) std::cout << "phi ";
      if (P.contains(Trk::theta)) std::cout << "theta ";
      if (P.contains(Trk::qOverP)) std::cout << "qOvP";
      std::cout << ")" << std::endl;
    } else if (nullptr != dynamic_cast<const Trk::Segment*>(it->measurement())) {
      const Trk::Segment* S = dynamic_cast<const Trk::Segment*>(it->measurement());
      const Trk::LocalParameters& P = it->measurement()->localParameters();
      std::cout << "Track Segment with of rank " << P.dimension() << " containing "
                << S->numberOfMeasurementBases() << " internal meas'ts."<<std::endl;
    } else if (it->materialEffects()) {
      std::cout << "Material layer thick="<<it->materialEffects()->x0()<<" with dE="
                << it->materialEffects()->deltaE()<<" +- "<<it->materialEffects()->sigmaDeltaE()
                << ", dPhi=" << it->materialEffects()->deltaPhi()<<" +- "<<it->materialEffects()->sigmaDeltaPhi()
                << ", dTheta="<< it->materialEffects()->deltaTheta()<<" +- "<<it->materialEffects()->sigmaDeltaTheta()<<std::endl;
    } else std::cout << std::endl;
  }
}
