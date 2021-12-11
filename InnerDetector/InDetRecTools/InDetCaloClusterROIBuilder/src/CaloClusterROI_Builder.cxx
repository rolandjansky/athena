/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterROI_Builder.h"

#include "CaloDetDescr/CaloDepthTool.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

#include "TrkCaloClusterROI/CaloClusterROI.h"

InDet::CaloClusterROI_Builder::CaloClusterROI_Builder(const std::string& t,
                                                      const std::string& n,
                                                      const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<ICaloClusterROI_Builder>(this);
}

InDet::CaloClusterROI_Builder::~CaloClusterROI_Builder() {}

StatusCode
InDet::CaloClusterROI_Builder::initialize()
{

  ATH_CHECK(m_calosurf.retrieve());
  ATH_CHECK(m_caloMgrKey.initialize());

  return StatusCode::SUCCESS;
}

Trk::CaloClusterROI*
InDet::CaloClusterROI_Builder::buildClusterROI(
  const xAOD::CaloCluster* cluster) const
{

  ATH_MSG_DEBUG("Building Trk::CaloCluster_OnTrack");

  if (!cluster)
    return nullptr;

  const Trk::Surface* surface = getCaloSurface(cluster);

  if (!surface)
    return nullptr;

  const Trk::LocalParameters* lp = getClusterLocalParameters(cluster, surface);

  if (!lp) {
    delete surface;
    return nullptr;
  }

  double energy = cluster->e();
  double widthPhi = 0.1;
  double widthEta = 0.1;

  // do we want to make energy be EM energy only?
  if (m_EMEnergyOnly) {
    static const SG::AuxElement::ConstAccessor<float> acc("EMFraction");
    double emFrac(0.);
    if (acc.isAvailable(*cluster)) {
      emFrac = acc(*cluster);
    } else if (!cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,
                                        emFrac)) {
      ATH_MSG_ERROR("EM energy requested, but No EM fraction momement stored");
      delete surface;
      delete lp;
      return nullptr;
    }
    energy *= emFrac;
  }

  Trk::CaloClusterROI* ccROI =
    new Trk::CaloClusterROI(lp, *surface, energy, widthPhi, widthEta);
  delete surface;

  if (ccROI) {
    ATH_MSG_DEBUG("Successful build of Trk::CaloClusterROI");
  }

  return ccROI;
}

const Trk::Surface*
InDet::CaloClusterROI_Builder::getCaloSurface(
  const xAOD::CaloCluster* cluster) const
{

  const Trk::Surface* destinationSurface = nullptr;

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{ m_caloMgrKey };
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;

  // Determine if we want to extrapolate to the barrel or endcap.  If in the
  // crack choose the detector with largest amount of energy in the second
  // sampling layer
  if (xAOD::EgammaHelpers::isBarrel(cluster)) {
    destinationSurface = m_calosurf->CreateUserSurface(
      CaloCell_ID::EMB2, 0., cluster->eta(), caloDDMgr);
  } else {
    destinationSurface = m_calosurf->CreateUserSurface(
      CaloCell_ID::EME2, 0., cluster->eta(), caloDDMgr);
  }
  return destinationSurface;
}

const Trk::LocalParameters*
InDet::CaloClusterROI_Builder::getClusterLocalParameters(
  const xAOD::CaloCluster* cluster,
  const Trk::Surface* surf) const
{

  Amg::Vector3D surfRefPoint = surf->globalReferencePoint();

  double eta = cluster->eta();
  double theta = 2 * atan(exp(-eta)); //  -log(tan(theta/2));
  double tantheta = tan(theta);
  double phi = cluster->phi();

  Trk::LocalParameters* newLocalParameters(nullptr);

  if (xAOD::EgammaHelpers::isBarrel(cluster)) {
    // Two corindate in a cyclinder are
    // Trk::locRPhi = 0 (ie phi)
    // Trk::locZ    = 1(ie z)
    double r = surfRefPoint.perp();
    double z = tantheta == 0 ? 0. : r / tantheta;
    Trk::DefinedParameter locRPhi(r * phi, Trk::locRPhi);
    Trk::DefinedParameter locZ(z, Trk::locZ);
    std::vector<Trk::DefinedParameter> defPar;
    defPar.push_back(locRPhi);
    defPar.push_back(locZ);
    newLocalParameters = new Trk::LocalParameters(defPar);
  } else {
    // Local paramters of a disk are
    // Trk::locR   = 0
    // Trk::locPhi = 1
    double z = surfRefPoint.z();
    double r = z * tantheta;
    Trk::DefinedParameter locR(r, Trk::locR);
    Trk::DefinedParameter locPhi(phi, Trk::locPhi);
    std::vector<Trk::DefinedParameter> defPar;
    defPar.push_back(locR);
    defPar.push_back(locPhi);
    newLocalParameters = new Trk::LocalParameters(defPar);
  }

  return newLocalParameters;
}

