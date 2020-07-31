/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  CaloClusterCorrectionCommon.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date December, 2004
 * @brief Code common to cluster corrections.
 */

#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "CaloClusterCorrection/interpolate.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include <cmath>
#include <cassert>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"

using xAOD::CaloCluster;
using CaloClusterCorr::interpolate;


namespace {


/**
 * @brief Test if a region is in the barrel or endcap.
 * @param region A region code, as defined in the header.
 *
 * If @c region is one of the sampling-specific codes, then return
 * either @c true or @c false, depending on whether the sampling
 * is for the barrel or the endcap.  Otherwise, we abort.
 */
bool barrel_p (int region)
{
  switch (region) {
  case CaloClusterCorrectionCommon::EMB1:
  case CaloClusterCorrectionCommon::EMB2:
    return true;
  case CaloClusterCorrectionCommon::EME1:
  case CaloClusterCorrectionCommon::EME2:
    return false;
  default:
    abort();
  }
}


/**
 * @brief Return the sampling for a region.
 * @param region A region code, as defined in the header.
 *
 * If @c region is one of the sampling-specific codes, then return
 * either the sampling number, as a number from 0--3.
 * Otherwise, we abort.
 */
int sampling (int region)
{
  switch (region) {
  case CaloClusterCorrectionCommon::EMB1:
  case CaloClusterCorrectionCommon::EME1:
    return 1;
  case CaloClusterCorrectionCommon::EMB2:
  case CaloClusterCorrectionCommon::EME2:
    return 2;
  default:
    abort();
  }
}


} // anonymous namespace


namespace CaloClusterCorr {


class DDHelper
{
public:
  /// Constructor.
  DDHelper (const CaloDetDescrManager* dd_man);


  /// Find the detector descriptor element for a given position,
  /// correcting for DD edge bugs.
  const CaloDetDescrElement*
  find_dd_elt (int region,
               const xAOD::CaloCluster* cluster,
               float eta,
               float phi) const;


private:
  /// Find the detector descriptor element for a given position.
  const CaloDetDescrElement* find_dd_elt1 (int region,
                                           const CaloCluster* cluster,
                                           float eta,
                                           float phi) const;


  /// Work around innermost strip problem.
  const CaloDetDescrElement*
  dd_inner_strip_fixup (int region,
                        float eta,
                        float phi) const;


  const CaloDetDescrElement* dd_try_gap (int region,
                                         const CaloCluster* cluster,
                                         float eta,
                                         float phi) const;


  /// Construct dummy DDEs used to work around innermost strip problem.
  void make_dummy_elts();


  /// The detector descriptor manager.
  const CaloDetDescrManager* m_dd_man;


  /// Collection of dummy elements.
  std::vector<std::unique_ptr<const CaloDetDescrElement> > m_dummy_elts;
};


/**
 * @brief Constructor.
 * @param dd_man The detector descriptor manager.
 */
DDHelper::DDHelper (const CaloDetDescrManager* dd_man)
  : m_dd_man (dd_man)
{
  make_dummy_elts();
}


/**
 * @brief Find the detector descriptor element for a given position,
 *        correcting for DD edge bugs.
 * @param region A region code, as defined in the header.
 * @param cluster The cluster being corrected.
 * @param eta    The @f$\eta@f$ coordinate to find.
 * @param phi    The @f$\phi@f$ coordinate to find.
 *
 * Looks up the DD element containing @c eta, @c phi in the region
 * specified by @c region.  Returns 0 if there's no such cell.
 *
 * Sometimes when you look up a position near the edge of a cell,
 * DD can erroneously return an adjacent cell.
 * This routine attempts to work around this bug.
 * After we get an element, we test to see if it in fact contains
 * the position requested.  If not, we shift the request by half
 * a cell and try again.
 */
const CaloDetDescrElement*
DDHelper::find_dd_elt (int region,
                       const CaloCluster* cluster,
                       float eta,
                       float phi) const
{
  const CaloDetDescrElement* elt = 0;
  float eta_offs = 0;
  float phi_offs = 0;
  int n = 0;
  int good = 0;
  
  while (good != 2) {
    elt = find_dd_elt1 (region, cluster,
                        eta + eta_offs, CaloPhiRange::fix (phi + phi_offs));

    if (!elt) {
      elt = dd_inner_strip_fixup (region, eta, phi);
      if (elt) return elt;
      elt = dd_try_gap (region, cluster, eta, phi);
      return elt;
    }

    // Don't do this more than twice.
    // Originally, we were aborting if we couldn't find a good element
    // after two passes.  However, it turns out that there are some
    // small gaps between the @f$\eta@f$ ranges of adjacent cells, so if
    // we demanded that the @f$\eta@f$ we provide be within the
    // @f$\eta@f$ range of the element we return, we wouldn't succeed.
    // Downstream code will just have to Deal With It.
    if (++n >= 2)
      return elt;

    float deta = elt->deta();
    float dphi = elt->dphi();

    good = 0;

    if (eta > elt->eta() + deta/2)
      eta_offs += deta/2;
    else if (eta < elt->eta() - deta/2)
      eta_offs -= deta/2;
    else
      ++good;

    // Assume that cells don't wrap around the phi boundary...
    if (phi > elt->phi() + dphi/2)
      phi_offs += dphi/2;
    else if (phi < elt->phi() - dphi/2)
      phi_offs -= dphi/2;
    else
      ++good;

    if (good != 2 && n == 1) {
      elt = dd_inner_strip_fixup (region, eta, phi);
      if (elt) break;
    }
  }

  return elt;
}


/**
 * @brief Find the detector descriptor element for a given position.
 * @param region A region code, as defined in the header.
 * @param cluster The cluster being corrected.
 * @param eta    The @f$\eta@f$ coordinate to find.
 * @param phi    The @f$\phi@f$ coordinate to find.
 *
 * Looks up the DD element containing @c eta, @c phi in the region
 * specified by @c region.  Returns 0 if there's no such cell.
 */
const CaloDetDescrElement*
DDHelper::find_dd_elt1 (int region,
                        const CaloCluster* cluster,
                        float eta,
                        float phi) const
{
  const CaloDetDescrElement* elt = 0;

  // Decode the region.
  switch (region) {
  case CaloClusterCorrectionCommon::EMB1:
  case CaloClusterCorrectionCommon::EMB2:
  case CaloClusterCorrectionCommon::EME1:
  case CaloClusterCorrectionCommon::EME2:
    // Simple case, it's a specific sampling.
    elt = m_dd_man->get_element
      (CaloCell_ID::LAREM, sampling (region), barrel_p (region), eta, phi);
    break;

  case CaloClusterCorrectionCommon::COMBINED2:
  case CaloClusterCorrectionCommon::CLUSTER:
    // We're combining both the barrel and endcap.
    // Look for elements in both.
    // If we actually get both, make the decision by choosing
    // the one with the most energy in sampling 2.
    {
      const CaloDetDescrElement* elt_b = m_dd_man->get_element
        (CaloCell_ID::LAREM, 2,  true, eta, phi);
      const CaloDetDescrElement* elt_e = m_dd_man->get_element
        (CaloCell_ID::LAREM, 2, false, eta, phi);

      if (elt_b == 0)
        elt = elt_e;
      else if (elt_e == 0)
        elt = elt_b;
      else if (cluster->eSample (CaloSampling::EMB2) >
               cluster->eSample (CaloSampling::EME2))
        elt = elt_b;
      else
        elt = elt_e;      
    }
    break;
  default:
    abort();
  }

  return elt;
}


/**
 * @brief Work around innermost strip problem.
 * @param region A region code, as defined in the header.
 * @param dd_man Detector descriptor manager.
 * @param eta    The @f$\eta@f$ coordinate to find.
 * @param phi    The @f$\phi@f$ coordinate to find.
 * @param dummy_elts Vector of dummy elements for the innermost strip.
 *
 * The innermost strip in sampling 1 in the barrel is disconnected.
 * However, CaloDetDescr doesn't really know about that.
 * If you ask for a cell in that region, it will instead return
 * you a cell for some completely different calorimeter region!
 * We call this in the case that we're unable to find a good
 * det descr element.  Here we check to see if we're actually
 * asking for a location in this innermost strip.  If so, then we
 * cons up a new dummy element and return that.
 */
const CaloDetDescrElement*
DDHelper::dd_inner_strip_fixup (int region,
                                float eta,
                                float phi) const
{
  if (region == CaloClusterCorrectionCommon::EMB1 && fabs(eta) < 0.1) {
    const CaloDetDescriptor* descr =
      m_dd_man->get_descriptor (CaloCell_ID::LAREM,
                                1, true, eta, phi);
    if (!descr) return 0;
    int ieta = descr->eta_channel (eta);
    if (ieta == 0) {
      // If we get here, then we're looking at one of the problematic cells.
      int iphi = descr->phi_channel (phi);
      if (iphi < 0) return 0;
      unsigned int index = iphi;
      if (eta < 0)
        index += descr->n_phi();
      if (m_dummy_elts.size() <= index)
        return 0;
      return m_dummy_elts[index].get();
    }
  }

  return 0;
}


const CaloDetDescrElement*
DDHelper::dd_try_gap (int region,
                      const CaloCluster* cluster,
                      float eta,
                      float phi) const
{
  const CaloDetDescrElement* elt1 = find_dd_elt1 (region, cluster,
                                                  eta + 1e-4, phi);
  if (!elt1) return 0;
  const CaloDetDescrElement* elt2 = find_dd_elt1 (region, cluster,
                                                  eta - 1e-4, phi);
  if (!elt2) return 0;
  if (eta > 0)
    return elt2;
  return elt1;
}


/**
 * @brief Construct dummy DDEs used to work around innermost strip problem.
 */
void DDHelper::make_dummy_elts()
{
  const CaloDetDescriptor* descr = m_dd_man->get_descriptor (CaloCell_ID::LAREM,
                                                             1, true, 0.05, 0);
  if (descr) {
    int nphi = descr->n_phi();
    m_dummy_elts.resize (nphi*2);
    for (int etasgn = 1; etasgn >= -1; etasgn -= 2) {
      for (int iphi = 0; iphi < nphi; iphi++) {
        // Make a new dummy cell.
        // First, try to find the adjacent strip.  Punt if we can't
        // find _that_!
        const CaloCell_ID* cellid_mgr = m_dd_man->getCaloCell_ID();
        Identifier cellId2 = cellid_mgr->cell_id (descr->identify(),
                                                  1, iphi);
        IdentifierHash cellIdHash2 = cellid_mgr->calo_cell_hash (cellId2);
        // Verify that we don't have another nonexistent cell!
        if (cellid_mgr->cell_id (cellIdHash2) != cellId2)
          continue;
        const CaloDetDescrElement* elt2 = m_dd_man->get_element (cellIdHash2);
        if (!elt2) continue;

        auto elt = std::make_unique<DummyDetDescrElement>
          (descr->subcalo_hash(),
           0,
           0,
           descr);

        // Copy geometry from the adjacent cell, shifting eta.
        elt->set_cylindric_size (elt2->deta(),
                                 elt2->dphi(),
                                 elt2->dr());
        elt->set_cylindric (elt2->eta() - etasgn * elt2->deta(),
                            elt2->phi(),
                            elt2->r());
        elt->set_cylindric_raw (elt2->eta_raw() - etasgn * elt2->deta(),
                                elt2->phi_raw(),
                                elt2->r_raw());

        int index = iphi;
        if (etasgn < 0) index += nphi;
        m_dummy_elts[index] = std::move(elt);
      }
    }
  }
}


} // namespace CaloClusterCorr


CaloClusterCorrectionCommon::CaloClusterCorrectionCommon (const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent)
  : CaloClusterCorrection (type, name, parent)
{
}


/**
 * @brief Destructor.
 *
 * Needs to be out-of-line due to m_ddhelper.
 */
CaloClusterCorrectionCommon::~CaloClusterCorrectionCommon()
{
}


/**
 * @brief Perform the correction.  Called by the tool.
 * @param myctx   ToolWithConstants context.
 * @param cluster The cluster to correct.
 *                It is updated in place.
 *
 * Does the following:
 *  - Checks whether the cluster is present in the given calorimeter
 *    and sampling.  If not, do nothing.
 *  - Computes quantities to pass to @c makeTheCorrection.
 *  - Calls @c makeTheCorrection.
 */
void CaloClusterCorrectionCommon::makeCorrection (const Context& myctx,
                                                  CaloCluster* cluster) const
{
  int region = m_region (myctx);

  // This causes a lot of overhead (mostly from the MsgStream ctor).
  // Comment out when not needed.
  //MsgStream log( msgSvc(), name() );
  //log << MSG::DEBUG << "Entering makeCorrection" << endmsg;
  //log << MSG::DEBUG << "e, eta, phi, etasize, phisize" << " " << cluster->e() << " " << cluster->eta() << " " << cluster->phi() 
  //  << " " <<  cluster->etasize(CaloSampling::EMB2) << " " << cluster->phisize(CaloSampling::EMB2) << endmsg;
  //log << MSG::DEBUG << "B / E  " << cluster->inBarrel() << " " << cluster->inEndcap() << endmsg;
  //log << MSG::DEBUG << "region " << region << endmsg;

  float eta;
  float phi;
  CaloSampling::CaloSample samp = CaloSampling::Unknown;

  // Find the proper @f$\eta@f$ and @f$\phi@f$ measures of the cluster.
  // Also set up the sampling code @c samp.
  switch (region) {
  case EMB1:
  case EMB2:
  case EME1:
  case EME2:
    // Return immediately if we can tell we're in the wrong region.
    if (barrel_p (region)) {
      if (!cluster->inBarrel()) return;
    }
    else {
      if (!cluster->inEndcap()) return;
    }

    switch (region) {
    case EMB1:
      samp = CaloSampling::EMB1;
      break;
    case EMB2:
      samp = CaloSampling::EMB2;
      break;
    case EME1:
      samp = CaloSampling::EME1;
      break;
    case EME2:
      samp = CaloSampling::EME2;
      break;
    }

    eta = cluster->etaSample (samp);
    phi = cluster->phiSample (samp);
    break;

  case COMBINED2:
    eta = cluster->etaBE (2);
    phi = cluster->phiBE (2);
    break;

  case CLUSTER:
    eta = cluster->eta();
    phi = cluster->phi();
    break;

  default:
    abort();
  }

  // Give up if one of them is an error flag.
  if (std::abs (phi) > 900 || std::abs (eta) > 900)
    return;

  // Sometimes unnormalized @f$\phi@f$ values still come through.
  // Make sure this is in the proper range before calling the correction.
  phi = CaloPhiRange::fix (phi);

  // Look up the DD element.
  // Give up if we can't find one.
  const CaloDetDescrElement* elt = ddhelper().find_dd_elt (region,
                                                           cluster,
                                                           eta, phi);
  if (!elt)
    return;

  // Compute the adjusted eta and phi --- the coordinates shifted
  // from the actual to the nominal coordinate system.
  float adj_eta = eta - elt->eta() + elt->eta_raw();
  float adj_phi = CaloPhiRange::fix (phi - elt->phi() + elt->phi_raw());

  // Call the actual correction.
  makeTheCorrection (myctx, cluster, elt, eta, adj_eta, phi, adj_phi, samp);
}


namespace {


/**
 * @brief Helper for calculating one energy point.
 * @param                The index of the energy point to calculate.
 * @param builder        Helper to construct the interpolation table.
 * @param energies       The energy interpolation table.
 * @param corrtab        The correction table being built.
 * @param n_good[in|out] Counter of the number of good evaluations
 *                       we've had so far.
 */
inline
void docalc (int i,
             const CaloClusterCorrectionCommon::TableBuilder& builder,
             const CaloRec::Array<1>& energies,
             CaloRec::WritableArray<2>& corrtab,
             int& n_good)
{
  corrtab[n_good][0] = energies[i];
  bool good = false;
  corrtab[n_good][1] = builder.calculate (i, good);
  if (good)
    ++n_good;
}


} // anonymous namespace
             


/**
 * @brief Perform energy interpolation.
 * @param energy        The energy value for the interpolation.
 * @param builder       Helper to construct the interpolation table.
 * @param energies      The energy interpolation table.
 * @param energy_degree The polynomial interpolation degree for the
 *                      energy interpolation.
 *
 * Many of the corrections use the same method for doing the final
 * interpolation in energy.  We factor out this common code here.
 */
float
CaloClusterCorrectionCommon::energy_interpolation (float energy,
						   const TableBuilder& builder,
						   const CaloRec::Array<1>&
                                                     energies,
						   int energy_degree)
  const
{
  // Calculate the correction for each energy.
  unsigned int n_energies = energies.size();
  unsigned int shape[] = {n_energies, 2};
  CaloRec::WritableArrayData<2> corrtab (shape);

  // If we're outside the range of the table, we'll just be using the
  // value at the end (no extrapolation).  We only need to calculate
  // that one point in that case.
  unsigned int beg = 0;
  unsigned int end = n_energies;
  if (energy <= energies[0])
    end = 1;
  else if (energy >= energies[n_energies-1])
    beg = n_energies-1;

  // Build the table.
  int n_good = 0;
  for (unsigned int i=beg; i<end; i++)
    docalc (i, builder, energies, corrtab, n_good);

  // If we only evaluated one point, but it wasn't good, keep
  // searching until we find a good one.
  while (n_good == 0 && beg > 0) {
    --beg;
    docalc (beg, builder, energies, corrtab, n_good);
  }
  while (n_good == 0 && end < n_energies) {
    docalc (end, builder, energies, corrtab, n_good);
    ++end;
  }

  // Now interpolate in energy.
  // But if we're outside of the range of the table, just use the value
  // at the end (don't extrapolate).
  if (n_good == 0) {
    // No good energies --- return a null correction.
    return 0;
  }
  else if (n_good == 1) {
    // Only one good energy --- nothing to do but to use it.
    return corrtab[0][1];
  }
  else if (energy <= corrtab[0][0]) {
    // Off the low end of the table --- return the first value.
    return corrtab[0][1];
  }
  else if (energy >= corrtab[n_good-1][0]) {
    // Off the high end of the table --- return the last value.
    return corrtab[n_good-1][1];
  }

  // Do the interpolation.
  return interpolate (corrtab, energy, energy_degree,
                      1, CaloRec::Array<1>(), n_good);
}


const CaloClusterCorr::DDHelper& CaloClusterCorrectionCommon::ddhelper() const
{
  const CaloClusterCorr::DDHelper* ddhelper = m_ddhelper.get();
  if (!ddhelper) {
    const CaloDetDescrManager* ddman = nullptr;
    if (detStore()->retrieve (ddman, "CaloMgr").isFailure()) {
      std::abort();
    }
    auto newhelper = std::make_unique<const CaloClusterCorr::DDHelper> (ddman);
    ddhelper = m_ddhelper.set (std::move (newhelper));
  }
  return *ddhelper;
}

