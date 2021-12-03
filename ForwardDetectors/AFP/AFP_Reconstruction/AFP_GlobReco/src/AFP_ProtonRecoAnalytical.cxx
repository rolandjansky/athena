/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AfpAnalysisTools include(s):
#include "AFP_GlobReco/AFP_ProtonRecoAnalytical.h"



AFP_ProtonRecoAnalytical::AFP_ProtonRecoAnalytical (const std::string &type, const std::string &name, const IInterface *parent)
 : base_class(type, name, parent)
{
  ATH_MSG_DEBUG("in AFP_ProtonRecoAnalytical constructor");
}


StatusCode AFP_ProtonRecoAnalytical::configInfo () const 
{
  ATH_MSG_INFO("----- AFP_ProtonRecoAnalytical -----");

  ATH_MSG_INFO("\tNear AFP position [m]: " << m_detectorPositionNear );
  ATH_MSG_INFO("\tFar AFP position  [m]: " << m_detectorPositionFar );
  ATH_MSG_INFO("\tSingle station reconstruction: " << m_allowSingleStationReco );
  ATH_MSG_INFO("\tCuts:\n");
  ATH_MSG_INFO("\t\ttrackDistance [mm]: " << m_trackDistance );
  
  ATH_MSG_INFO("\tparametrizationFileName = " << m_parametrizationFileName);
  ATH_MSG_INFO("\tparametrizationPosition = " << m_parametrizationPosition);
  ATH_MSG_INFO("\tparametrizationEnergy   = " << m_parametrizationEnergy);

  return StatusCode::SUCCESS;
}


StatusCode AFP_ProtonRecoAnalytical::initialize () 
{

  CHECK( m_trackContainerKey.initialize() );
  
  if(m_detectorPositions.empty())
  {
    if(m_side==0)
    {
      m_detectorPositionNear=205.217;
      m_detectorPositionFar=217.302;
    }
    else if(m_side==1)
    {
      m_detectorPositionNear=205.824;
      m_detectorPositionFar=217.909;
    }
    else
    {
      ATH_MSG_ERROR("unknown side id "<<m_side<<", allowed values are 0 (for A) and 1 (for C)");
      return StatusCode::FAILURE;
    }
  }
  else if(m_detectorPositions.size()==2)
  {
    if(m_detectorPositions[0]<m_detectorPositions[1])
    {
      m_detectorPositionNear=m_detectorPositions[0];
      m_detectorPositionFar =m_detectorPositions[1];
    }
    else
    {
      m_detectorPositionNear=m_detectorPositions[1];
      m_detectorPositionFar =m_detectorPositions[0];
    }
  }
  else
  {
    ATH_MSG_ERROR("there are "<<m_detectorPositions.size()<<" entries for m_detectorPositions, we have only 2 detectors on each side");
    return StatusCode::FAILURE;
  }
  
  
  if(m_parametrizationFileName=="")
  {
  	m_parametrizationFileName = (m_side) ? "param_mad_b1_def.txt" : "param_mad_b2_def.txt";
  }
  
  const std::string parametrization = PathResolver::find_file(m_parametrizationFileName, "DATAPATH", PathResolver::RecursiveSearch);

  m_parametrization = std::make_unique<AFP::Parameterization>(parametrization);
  m_parametrizationPosition = m_parametrization->parametrizationPosition();
  m_parametrizationEnergy = m_parametrization->energy();
  m_distanceBetweenStations = m_detectorPositionFar - m_detectorPositionNear;

  return StatusCode::SUCCESS;
}

StatusCode AFP_ProtonRecoAnalytical::doProtonReco(std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer, const EventContext& ctx) const 
{
  
  SG::ReadHandle<xAOD::AFPTrackContainer> trackContainer( m_trackContainerKey, ctx );
  if(!trackContainer.isValid())
  {
    // this is allowed, there might be no AFP data in the input
    return StatusCode::SUCCESS;
  }
  
  const double trackDistanceRadiusSq = m_trackDistance*m_trackDistance;

  // Select tracks in near station
  std::vector<const xAOD::AFPTrack*> trackNearContainer;
  const int nearId = m_side + 1;
  std::copy_if(trackContainer->begin(), trackContainer->end(), std::back_inserter(trackNearContainer),
      [&nearId](auto track) { return track->stationID() == nearId; });

  // Select tracks in far station
  std::vector<const xAOD::AFPTrack*> trackFarContainer;
  const int farId = 3 * m_side;
  std::copy_if(trackContainer->begin(), trackContainer->end(), std::back_inserter(trackFarContainer),
      [&farId](auto track) { return track->stationID() == farId; });

  ATH_MSG_DEBUG("trackNearContainer size: " << trackNearContainer.size()<<", side "<<m_side);
  ATH_MSG_DEBUG("trackFarContainer  size: " << trackFarContainer.size()<<", side "<<m_side);

  // Loop over both containers
  for (const xAOD::AFPTrack* trackFar : trackFarContainer) {
    bool foundMatchingTrack = false;

    for (const xAOD::AFPTrack* trackNear : trackNearContainer) {
      // Apply cuts
      const double dx = trackFar->xLocal() - trackNear->xLocal();
      const double dy = trackFar->yLocal() - trackNear->yLocal();
      const double r2 = dx*dx + dy*dy;

      if (r2 > trackDistanceRadiusSq) {
        ATH_MSG_DEBUG(
            "Tracks too far away from each other (xNear, yNear; xFar, yFar; distance) [mm]: "
            << trackNear->xLocal() << ", " << trackNear->yLocal() << "; "
            << trackFar->xLocal()  << ", " << trackFar->yLocal()  << "; " << r2);

        continue;
      }

      // Reconstruct proton and add it to the container
      xAOD::AFPProton * proton = reco(trackNear, trackFar, outputContainer);

      if (!proton)
        continue;

      foundMatchingTrack = true;

      // Create link to tracks
      linkTracksToProton(trackNear, trackContainer, proton);
      linkTracksToProton(trackFar, trackContainer, proton);
    }

    // Reconstuct proton using only FAR station if
    // no matching track on NEAR station was found
    if (m_allowSingleStationReco and !foundMatchingTrack) {
      // Apply cuts
      // none

      xAOD::AFPProton * proton = reco(trackFar, outputContainer);

      if (!proton)
        continue;

      linkTracksToProton(trackFar, trackContainer, proton);
    }
  }
  
  return StatusCode::SUCCESS;
}


xAOD::AFPProton * AFP_ProtonRecoAnalytical::createProton(const Momentum& momentum, const Measurement& my_measAFP, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const 
{

  // Return nullptr if any of momentum components is not a number
  if ( std::any_of(begin(momentum), end(momentum), [](auto& el) { return !isfinite(el); }) )
    return nullptr;

  const auto [px, py, pz] = momentum;

  auto * proton = outputContainer->push_back(std::make_unique<xAOD::AFPProton>());
  
  // Set proton properties
  constexpr double protonMass = 0.938; // in GeV
  
  proton->setPxPyPzE(px, py, pz, sqrt(px*px + py*py + pz*pz + protonMass*protonMass));
  proton->setChi2(chi2(px, py, pz, my_measAFP));
  proton->setSide(m_side);
  proton->setMethodID(0);
  
  ATH_MSG_DEBUG("Reconstructed proton (px, py, pz): " << proton->px() << ", " << proton->py() << ", " << proton->pz()<<", chi2 "<<proton->chi2()<<", side "<<proton->side());

  return proton;
}


void AFP_ProtonRecoAnalytical::linkTracksToProton
  (const xAOD::AFPTrack* track, SG::ReadHandle<xAOD::AFPTrackContainer>& trackContainer, xAOD::AFPProton * proton) const {

  ElementLink<xAOD::AFPTrackContainer> trackLink;

  trackLink.toContainedElement(*trackContainer, track);
  proton->addAFPTrackLink(trackLink);
}


double AFP_ProtonRecoAnalytical::bisection (double (AFP_ProtonRecoAnalytical::*fun)(double, const Measurement&, std::vector<double>&, std::vector<double>&) const, const Measurement& my_measAFP, std::vector<double>& my_slopeCalculated, std::vector<double>& my_positionCalculated) const {

  // Alias to minimized function
  auto fn = [this, fun, my_slopeCalculated, my_positionCalculated](double x, const Measurement& m, std::vector<double>& s, std::vector<double>& p) { return (this->*fun)(x,m,s,p); };

  constexpr double tol = 1e-3; // 1 MeV
  double eMin = m_parametrizationEnergy - 2000.; // ~30% xi
  double eMax = m_parametrizationEnergy;

  if (fn(eMax, my_measAFP, my_slopeCalculated, my_positionCalculated) * fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated) >= 0) { // Cannot use bisection method

    ATH_MSG_DEBUG("Cannot use bisection method");

    // Return value closest to zero
    return (fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated) < 0) ? std::max(fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated), fn(eMax, my_measAFP, my_slopeCalculated, my_positionCalculated)) : std::min(fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated), fn(eMax, my_measAFP, my_slopeCalculated, my_positionCalculated));
  }

  ATH_MSG_VERBOSE("==== eMin\teMax\te\tfun(eMin)\tfun(eMax)\tfun(E) ====");

  double e = eMin;

  // Bisection method
  while (eMax - eMin > tol) {
    e = (eMin + eMax)/2.0;
    ATH_MSG_VERBOSE("* " << eMin << "\t" << eMax << "\t" << e << "\t" << fn(e, my_measAFP, my_slopeCalculated, my_positionCalculated) << "\t" << fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated) << "\t" << fn(eMax, my_measAFP, my_slopeCalculated, my_positionCalculated));
    if (fn(eMax, my_measAFP, my_slopeCalculated, my_positionCalculated) * fn(e, my_measAFP, my_slopeCalculated, my_positionCalculated) < 0)
      eMin = e;
    else if (fn(eMin, my_measAFP, my_slopeCalculated, my_positionCalculated) * fn(e, my_measAFP, my_slopeCalculated, my_positionCalculated) < 0)
      eMax = e;
    else {
      eMin = e;
      eMax = e;
    }
  }

  ATH_MSG_DEBUG("Bisection energy: " << e);

  return e;
}


xAOD::AFPProton * AFP_ProtonRecoAnalytical::reco
  (const xAOD::AFPTrack* trackNear, const xAOD::AFPTrack* trackFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const {

  
    /// AFP measurement
  const Measurement my_measAFP = Measurement(trackNear->xLocal(), trackNear->yLocal(), trackFar->xLocal(), trackFar->yLocal());
  
  std::vector<double> my_slopeCalculated = {0, 0};
  std::vector<double> my_positionCalculated = {0, 0};
  
  my_slopeCalculated.at(0) = (my_measAFP.xF - my_measAFP.xN)/m_distanceBetweenStations;
  my_slopeCalculated.at(1) = (my_measAFP.yF - my_measAFP.yN)/m_distanceBetweenStations;
  my_positionCalculated.at(0) = my_measAFP.xN + my_slopeCalculated.at(0)*(m_parametrizationPosition - m_detectorPositionNear);
  my_positionCalculated.at(1) = my_measAFP.yN + my_slopeCalculated.at(1)*(m_parametrizationPosition - m_detectorPositionNear);

  ATH_MSG_DEBUG("Reconstructing proton with bisection method...");
  ATH_MSG_DEBUG("Tracks (xNear, yNear; xFar, yFar): " << my_measAFP.xN << ", " << my_measAFP.yN << "; "
                                                      << my_measAFP.xF << ", " << my_measAFP.yF);

  const double energy = bisection(&AFP_ProtonRecoAnalytical::bothStations, my_measAFP, my_slopeCalculated, my_positionCalculated);
  const double slopeX = calculateXslope(energy,my_slopeCalculated);
  const double slopeY = calculateYslope(energy,my_slopeCalculated);

  const double px = energy * slopeX;
  const double py = energy * slopeY;
  const double pz = energy * sqrt(1. - slopeX*slopeX - slopeY*slopeY);

  return createProton({px, py, pz}, my_measAFP, outputContainer);
}


xAOD::AFPProton * AFP_ProtonRecoAnalytical::reco (const xAOD::AFPTrack* trackFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const {

  const Measurement my_measAFP = Measurement(0., 0., trackFar->xLocal(), trackFar->yLocal());

  ATH_MSG_DEBUG("Reconstructing proton with bisection method (single station)...");
  ATH_MSG_DEBUG("Tracks (xFar, yFar): " << my_measAFP.xF << ", " << my_measAFP.yF);

  std::vector<double> my_slopeCalculated = {0, 0};
  std::vector<double> my_positionCalculated = {0, 0};

  const double energy = bisection(&AFP_ProtonRecoAnalytical::singleStation, my_measAFP, my_slopeCalculated, my_positionCalculated);

  return createProton({0., 0., energy}, my_measAFP, outputContainer);
}


double AFP_ProtonRecoAnalytical::bothStations (double energy, const Measurement& /*my_measAFP*/, std::vector<double>& my_slopeCalculated, std::vector<double>& my_positionCalculated) const {

  const double xi = 1.0 - energy / m_parametrizationEnergy;

  return
    (my_positionCalculated.at(0)
     - m_parametrization->getEquation(0)->getPolynomial(0)->Eval(xi)
     - m_vertexIP.at(0)*m_parametrization->getEquation(0)->getPolynomial(1)->Eval(xi)
     - m_vertexIP.at(2)*m_parametrization->getEquation(0)->getPolynomial(3)->Eval(xi)
     )
    *(
      m_parametrization->getEquation(2)->getPolynomial(4)->Eval(xi)
      + m_vertexIP.at(2)*m_parametrization->getEquation(2)->getPolynomial(6)->Eval(xi)
      )
    -  (
        my_slopeCalculated.at(0)
        - m_parametrization->getEquation(2)->getPolynomial(0)->Eval(xi)
        - m_vertexIP.at(0)*m_parametrization->getEquation(2)->getPolynomial(1)->Eval(xi)
        - m_vertexIP.at(2)*m_parametrization->getEquation(2)->getPolynomial(3)->Eval(xi)
        )
    *(
      m_parametrization->getEquation(0)->getPolynomial(4)->Eval(xi)
      + m_vertexIP.at(2)*m_parametrization->getEquation(0)->getPolynomial(6)->Eval(xi)
      );
}


double AFP_ProtonRecoAnalytical::singleStation (double energy, const Measurement& my_measAFP, std::vector<double>& /*my_slopeCalculated*/, std::vector<double>& /*my_positionCalculated*/) const {

  const double xi    = 1.0 - energy / m_parametrizationEnergy;
  const double xNear = my_measAFP.xF - m_distanceBetweenStations * m_parametrization->sx(m_vertexIP.at(0), m_vertexIP.at(1), m_vertexIP.at(2), 0, 0, energy);

  const double Ax = m_parametrization->getEquation(0)->getPolynomial(0)->Eval(xi);
  const double Bx = m_parametrization->getEquation(0)->getPolynomial(1)->Eval(xi);
  const double Dx = m_parametrization->getEquation(0)->getPolynomial(3)->Eval(xi);

  return xNear - Ax - m_vertexIP.at(0)*Bx - m_vertexIP.at(2)*Dx;
}


double AFP_ProtonRecoAnalytical::calculateSlope (double energy, int XorY, std::vector<double>& my_slopeCalculated) const { // 0 - X, 1 - Y

  const double xi = 1.0 - energy / m_parametrizationEnergy;

  const double Ax = m_parametrization->getEquation(2 + XorY)->getPolynomial(0)->Eval(xi);
  const double Bx = m_parametrization->getEquation(2 + XorY)->getPolynomial(4 + XorY)->Eval(xi);
  const double Cx = m_parametrization->getEquation(2 + XorY)->getPolynomial(1 + XorY)->Eval(xi);
  const double Dx = m_parametrization->getEquation(2 + XorY)->getPolynomial(6 + XorY)->Eval(xi);
  const double Ex = m_parametrization->getEquation(2 + XorY)->getPolynomial(3)->Eval(xi);

  const double Fx = my_slopeCalculated.at(0 + XorY) - Ax - Cx*m_vertexIP.at(0 + XorY) - Ex*m_vertexIP.at(2);
  const double Gx = Bx + m_vertexIP.at(2)*Dx;
  const double slp = (Gx == 0) ? 0. : Fx/Gx;

  return slp;
}


double AFP_ProtonRecoAnalytical::calculateXslope (double energy, std::vector<double>& my_slopeCalculated) const {

  return calculateSlope(energy, 0, my_slopeCalculated);
}


double AFP_ProtonRecoAnalytical::calculateYslope (double energy, std::vector<double>& my_slopeCalculated) const {

  return calculateSlope(energy, 1, my_slopeCalculated);
}


double AFP_ProtonRecoAnalytical::chi2 (double px, double py, double pz, const Measurement& my_measAFP) const {

  const double energy = sqrt(px*px + py*py + pz*pz);
  const double sx = px/energy;
  const double sy = py/energy;

  const double xNear = m_parametrization->x(m_vertexIP.at(0), m_vertexIP.at(1), m_vertexIP.at(2), sx, sy, energy);
  const double yNear = m_parametrization->y(m_vertexIP.at(0), m_vertexIP.at(1), m_vertexIP.at(2), sx, sy, energy);
  const double xFar = xNear + m_distanceBetweenStations * m_parametrization->sx(m_vertexIP.at(0), m_vertexIP.at(1), m_vertexIP.at(2), sx, sy, energy);
  const double yFar = yNear + m_distanceBetweenStations * m_parametrization->sy(m_vertexIP.at(0), m_vertexIP.at(1), m_vertexIP.at(2), sx, sy, energy);

  if (m_allowSingleStationReco and my_measAFP.xN == 0 and my_measAFP.yN == 0) {
    const double dx2 = my_measAFP.xF - xFar;
    const double dy2 = my_measAFP.yF - yFar;

    const double chi2x2 = dx2 * dx2 / (m_xSigma * m_xSigma);
    const double chi2y2 = (m_parametrization->yIsUsed()) ? dy2 * dy2 / (m_ySigma * m_ySigma) : 0.;

    return chi2x2 + chi2y2;
  }

  const double dx1 = my_measAFP.xN - xNear;
  const double dx2 = my_measAFP.xF - xFar;
  const double dy1 = my_measAFP.yN - yNear;
  const double dy2 = my_measAFP.yF - yFar;

  const double chi2x1 = dx1 * dx1 / (m_xSigma * m_xSigma);
  const double chi2x2 = dx2 * dx2 / (m_xSigma * m_xSigma);
  const double chi2y1 = (m_parametrization->yIsUsed()) ? dy1 * dy1 / (m_ySigma * m_ySigma) : 0.;
  const double chi2y2 = (m_parametrization->yIsUsed()) ? dy2 * dy2 / (m_ySigma * m_ySigma) : 0.;

  return chi2x1 + chi2y1 + chi2x2 + chi2y2;
}


