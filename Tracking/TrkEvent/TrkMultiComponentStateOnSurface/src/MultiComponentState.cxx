/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        MultiComponentState.cxx  -  description
                        ---------------------------------------
begin                : Sunday 8th May 2005
author               : atkinson, amorley ,anastopoulos
email                : Anthony.Morley@cern.ch
decription           : Implementation code for MultiComponentState class
*********************************************************************************/

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

Trk::MultiComponentState::~MultiComponentState()
{
  /*
   * Cleanup the const TrackParticle* ptr
   */
  Trk::MultiComponentState::const_iterator component = this->begin();
  Trk::MultiComponentState::const_iterator end = this->end();
  for (; component != end; ++component) {
    delete component->first;
  }
  this->clear();
}

Trk::MultiComponentState*
Trk::MultiComponentState::clone() const
{
  auto clonedState = std::make_unique<Trk::MultiComponentState>();
  clonedState->reserve(this->size());
  Trk::MultiComponentState::const_iterator component = this->begin();
  for (; component != this->end(); ++component) {
    const Trk::TrackParameters* clonedParameters = component->first->clone();
    clonedState->emplace_back(clonedParameters, component->second);
  }
  return clonedState.release();
}

Trk::MultiComponentState*
Trk::MultiComponentState::cloneWithScaledError(double errorScaleLocX,
                                               double errorScaleLocY,
                                               double errorScalePhi,
                                               double errorScaleTheta,
                                               double errorScaleQoverP) const
{
  auto stateWithScaledErrors = std::make_unique<Trk::MultiComponentState>();
  stateWithScaledErrors->reserve(this->size());
  Trk::MultiComponentState::const_iterator component = this->begin();
  for (; component != this->end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first;
    const AmgSymMatrix(5)* originalMatrix = trackParameters->covariance();
    if (!originalMatrix) {
      return this->clone();
    }

    auto covarianceMatrix = std::make_unique<AmgSymMatrix(5)>();
    int size = covarianceMatrix->rows();
    if (size == 5) {
      (*covarianceMatrix)(0, 0) = (*originalMatrix)(0, 0) * (errorScaleLocX * errorScaleLocX);
      (*covarianceMatrix)(1, 1) = (*originalMatrix)(1, 1) * (errorScaleLocY * errorScaleLocY);
      (*covarianceMatrix)(2, 2) = (*originalMatrix)(2, 2) * (errorScalePhi * errorScalePhi);
      (*covarianceMatrix)(3, 3) = (*originalMatrix)(3, 3) * (errorScaleTheta * errorScaleTheta);
      (*covarianceMatrix)(4, 4) = (*originalMatrix)(4, 4) * (errorScaleQoverP * errorScaleQoverP);

      covarianceMatrix->fillSymmetric(0, 1, (*originalMatrix)(0, 1) * (errorScaleLocX * errorScaleLocY));
      covarianceMatrix->fillSymmetric(0, 2, (*originalMatrix)(0, 2) * (errorScaleLocX * errorScalePhi));
      covarianceMatrix->fillSymmetric(0, 3, (*originalMatrix)(0, 3) * (errorScaleLocX * errorScaleTheta));
      covarianceMatrix->fillSymmetric(0, 4, (*originalMatrix)(0, 4) * (errorScaleLocX * errorScaleQoverP));

      covarianceMatrix->fillSymmetric(1, 2, (*originalMatrix)(1, 2) * (errorScaleLocY * errorScalePhi));
      covarianceMatrix->fillSymmetric(1, 3, (*originalMatrix)(1, 3) * (errorScaleLocY * errorScaleTheta));
      covarianceMatrix->fillSymmetric(1, 4, (*originalMatrix)(1, 4) * (errorScaleLocY * errorScaleQoverP));

      covarianceMatrix->fillSymmetric(2, 3, (*originalMatrix)(2, 3) * (errorScalePhi * errorScaleTheta));
      covarianceMatrix->fillSymmetric(2, 4, (*originalMatrix)(2, 4) * (errorScalePhi * errorScaleQoverP));

      covarianceMatrix->fillSymmetric(3, 4, (*originalMatrix)(3, 4) * (errorScaleTheta * errorScaleQoverP));

    } else {
      return this->clone();
    }
    const AmgVector(5) par = trackParameters->parameters();
    const TrackParameters* newTrackParameters = trackParameters->associatedSurface().createTrackParameters(
      par[Trk::loc1], par[Trk::loc2], par[Trk::phi], par[Trk::theta], par[Trk::qOverP], covarianceMatrix.release());
    // Push back new component
    stateWithScaledErrors->emplace_back(newTrackParameters, component->second);
  }
  return stateWithScaledErrors.release();
}

Trk::MultiComponentState*
Trk::MultiComponentState::cloneWithScaledError(double errorScale) const
{

  auto stateWithScaledErrors = std::make_unique<Trk::MultiComponentState>();
  stateWithScaledErrors->reserve(this->size());
  Trk::MultiComponentState::const_iterator component = this->begin();
  for (; component != this->end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first;
    const AmgSymMatrix(5)* originalMatrix = trackParameters->covariance();
    if (!originalMatrix) {
      return this->clone();
    }

    auto covarianceMatrix = std::make_unique<AmgSymMatrix(5)>();

    (*covarianceMatrix)(0, 0) = (*originalMatrix)(0, 0) * errorScale;
    (*covarianceMatrix)(1, 1) = (*originalMatrix)(1, 1) * errorScale;
    (*covarianceMatrix)(2, 2) = (*originalMatrix)(2, 2) * errorScale;
    (*covarianceMatrix)(3, 3) = (*originalMatrix)(3, 3) * errorScale;
    (*covarianceMatrix)(4, 4) = (*originalMatrix)(4, 4) * errorScale;

    covarianceMatrix->fillSymmetric(0, 1, (*originalMatrix)(0, 1) * errorScale);
    covarianceMatrix->fillSymmetric(0, 2, (*originalMatrix)(0, 2) * errorScale);
    covarianceMatrix->fillSymmetric(0, 3, (*originalMatrix)(0, 3) * errorScale);
    covarianceMatrix->fillSymmetric(0, 4, (*originalMatrix)(0, 4) * errorScale);

    covarianceMatrix->fillSymmetric(1, 2, (*originalMatrix)(1, 2) * errorScale);
    covarianceMatrix->fillSymmetric(1, 3, (*originalMatrix)(1, 3) * errorScale);
    covarianceMatrix->fillSymmetric(1, 4, (*originalMatrix)(1, 4) * errorScale);

    covarianceMatrix->fillSymmetric(2, 3, (*originalMatrix)(2, 3) * errorScale);
    covarianceMatrix->fillSymmetric(2, 4, (*originalMatrix)(2, 4) * errorScale);

    covarianceMatrix->fillSymmetric(3, 4, (*originalMatrix)(3, 4) * errorScale);

    const AmgVector(5) par = trackParameters->parameters();
    const TrackParameters* newTrackParameters = trackParameters->associatedSurface().createTrackParameters(
      par[Trk::loc1], par[Trk::loc2], par[Trk::phi], par[Trk::theta], par[Trk::qOverP], covarianceMatrix.release());

    // Push back new component
    stateWithScaledErrors->emplace_back(newTrackParameters, component->second);
  }
  return stateWithScaledErrors.release();
}

bool
Trk::MultiComponentState::isMeasured() const
{
  bool isMeasured = true;
  Trk::MultiComponentState::const_iterator component = this->begin();
  Trk::MultiComponentState::const_iterator end = this->end();
  for (; component != end; ++component) {
    const AmgSymMatrix(5)* originalMatrix = component->first->covariance();
    if (!originalMatrix)
      isMeasured = false;
    break;
  }
  return isMeasured;
}

void
Trk::MultiComponentState::renormaliseState(double norm)
{

  Trk::MultiComponentState::iterator component = this->begin();
  Trk::MultiComponentState::iterator end = this->end();
  // Determine total weighting of state
  double sumWeights = 0.;
  for (; component != end; ++component) {
    sumWeights += component->second;
  }
  if (sumWeights == 0) {
    return;
  }

  double normalise = norm / sumWeights;
  component = this->begin();
  for (; component != end; ++component) {
    component->second = component->second * normalise;
  }
}

MsgStream&
Trk::MultiComponentState::dump(MsgStream& out) const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  out << "***************** Multi-Component State print *****************" << std::endl;

  out << "State address: " << this << std::endl;

  int componentCounter(1);
  int stateSize = this->size();

  for (; component != this->end(); ++component, ++componentCounter) {
    out << "Component " << componentCounter << " of " << stateSize << std::endl;
    out << "Component address: " << component->first << std::endl;
    out << "Track parameters: " << *(component->first) << std::endl;
    out << "Weight: " << component->second << std::endl;
    out << "------------------------------------------------" << std::endl;
  }

  out << "********************** End of state print *********************" << std::endl;

  return out;
}

std::ostream&
Trk::MultiComponentState::dump(std::ostream& out) const
{

  Trk::MultiComponentState::const_iterator component = this->begin();

  out << "***************** Multi-Component State print *****************" << std::endl;

  out << "State address: " << this << std::endl;

  int componentCounter(1);
  int stateSize = this->size();

  for (; component != this->end(); ++component, ++componentCounter) {
    out << "Component " << componentCounter << " of " << stateSize << std::endl;
    out << "Component address: " << component->first << std::endl;
    out << "Track parameters: " << *(component->first) << std::endl;
    out << "Weight: " << component->second << std::endl;
    out << "------------------------------------------------" << std::endl;
  }

  out << "********************** End of state print *********************" << std::endl;

  return out;
}

MsgStream&
Trk::operator<<(MsgStream& stream, const Trk::MultiComponentState& multiComponentState)
{
  return multiComponentState.dump(stream);
}

std::ostream&
Trk::operator<<(std::ostream& stream, const MultiComponentState& multiComponentState)
{
  return multiComponentState.dump(stream);
}
