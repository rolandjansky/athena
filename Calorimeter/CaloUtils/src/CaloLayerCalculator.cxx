/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloLayerCalculator.cxx,v 1.1 2006-03-20 17:42:26 ssnyder Exp $
/**
 * @file  CaloLayerCalculator.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Calculate total energy, position, etc. for a given layer of
 *        a cluster.
 */

#include "CaloUtils/CaloLayerCalculator.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCellContainer.h"

/**
 * @brief Default constructor.
 *
 * Sets the variables to invalid values.
 */
CaloLayerCalculator::CaloLayerCalculator()
{
  reset();
}


/**
 * @brief Calculate layer variables from a cell container.
 * @param cell_container The cell container.
 * @param eta The @f$\eta@f$ center of the cluster, for cell selection.
 * @param phi The @f$\phi@f$ center of the cluster, for cell selection.
 * @param deta The @f$\eta@f$ full width for cell selection.
 * @param dphi The @f$\phi@f$ full width for cell selection.
 * @param sampling The sampling for cell selection.
 * @param tofill If non-null, then selected cells will be added
 *               to this cluster.
 *
 * This takes a list of cells from a cell container,
 * selects those within a @c deta by @c dphi window around @c eta, @c phi
 * in layer @c sampling, and fills the layer variables from them.
 * If @c tofill is non-null, then selected cells will also be added
 * to the cluster @c tofill.
 */
StatusCode
CaloLayerCalculator::fill (const CaloDetDescrManager& mgr,
                           const CaloCellContainer* cell_container,
                           double eta,
                           double phi,
                           double deta,
                           double dphi,
                           CaloSampling::CaloSample sampling,
                           xAOD::CaloCluster* tofill /*= 0*/)
{

  //Check if cell_container and cluster matches
  if (tofill) {
    if (tofill->getCellLinks()->getCellContainer()!=cell_container) {
      std::cout << "ERROR attempt to fill a cluster from different cell containers\n";
      return StatusCode::FAILURE;
    }
  }

  CaloCellList cell_list(cell_container); 
  cell_list.select(mgr,eta,phi,deta,dphi,sampling);

  fill (cell_list.begin(),
        cell_list.end(),
        eta, phi, deta, dphi, sampling, tofill);

  return StatusCode::SUCCESS;
}

StatusCode
CaloLayerCalculator::fill (const CaloCellContainer* cell_container,
                           double eta,
                           double phi,
                           double deta,
                           double dphi,
                           CaloSampling::CaloSample sampling,
                           xAOD::CaloCluster* tofill /*= 0*/)
{
const CaloDetDescrManager* mgr = CaloDetDescrManager::instance();
return fill(*mgr, cell_container, eta, phi, deta, dphi, sampling, tofill);
}


/// Reset output variables to invalid values.
void CaloLayerCalculator::reset()
{
  m_etam   = -999;
  m_phim   = -999;
  m_etamax = -999;
  m_phimax = -999;
  m_etarmax= -999;
  m_phirmax= -999;

  m_etas   = 0;
  m_phis   = 0;
  m_em     = 0;
  m_emax   = 0;

  // Added to cluster in the calorimeter frame
  m_etamr  = -999;
  m_phimr  = -999;

}


/// Reset output variables, but not m_em, allowing samplings to be negative.
void CaloLayerCalculator::resetOnNegativeEnergy()
{
  m_etam   = -999;
  m_phim   = -999;
  m_etamax = -999;
  m_phimax = -999;
  // Don't reset these --- that could prevent us from being able
  // to calculate the e2* variables in egammaMiddleShape if there
  // are negative energy cells just outside the cluster.
  //m_etarmax= -999;
  //m_phirmax= -999;

  m_etas   = 0;
  m_phis   = 0;
  //m_em     = 0;
  m_emax   = 0;

  // Added to cluster in the calorimeter frame
  m_etamr  = -999;
  m_phimr  = -999;
}

/// Reset output variables using seed position to fill eta,phi layer variables but not m_em, allowing samplings to be negative.
void CaloLayerCalculator::resetOnNegativeEnergy(double eta, double phi)
{
  m_etam   = eta;
  m_phim   = phi;
  m_etamax = eta;
  m_phimax = phi;
  m_etarmax= eta;
  m_phirmax= phi;

  m_etas   = 0;
  m_phis   = 0;
  //m_em     = 0;    
  m_emax   = 0;

  // Added to cluster in the calorimeter frame
  m_etamr  = eta; 
  m_phimr  = phi; 
}



/// Phi-wrapping helper.
const CaloPhiRange CaloLayerCalculator::Helper::s_range;


/**
 * @brief Initialize for doing layer variable calculation.
 * @param calc The parent @c CaloLayerCalculator instance.
 * @param eta The @f$\eta@f$ center of the cluster, for cell selection.
 * @param phi The @f$\phi@f$ center of the cluster, for cell selection.
 * @param deta The @f$\eta@f$ full width for cell selection.
 * @param dphi The @f$\phi@f$ full width for cell selection.
 * @param sampling The sampling for cell selection.
 * @param tofill If non-null, then selected cells will be added
 *               to this cluster.
 * @param cell_container If @c tofill is non-null, then this is this
 *               container from which the cells came.
 */
CaloLayerCalculator::Helper::Helper (CaloLayerCalculator& calc,
                                     double eta,
                                     double phi,
                                     double deta,
                                     double dphi,
                                     CaloSampling::CaloSample sampling,
                                     xAOD::CaloCluster* tofill,
                                     const CaloCellContainer*
                                     cell_container)
  : m_calc (calc),
    m_eta (eta),
    m_phi (CaloPhiRange::fix (phi)),
    m_deta (deta),
    m_dphi (dphi),
    m_sampling (sampling),
    m_tofill (tofill),
    m_cell_container (cell_container),
    m_s00 (0),
    m_s10 (0),
    m_s20 (0),
    m_s01 (0),
    m_s02 (0),
    // Added to cluster in the calorimeter frame
    m_s10r (0),
    m_s01r (0)
{
  m_calc.m_emax   = -999999;
}


/**
 * @brief Accumulate results for another cell.
 * @param cell The cell to accumulate.
 * @param weight The weight for the cell.
 */
void
CaloLayerCalculator::Helper::cell (const CaloCell* cell, double weight)
{
  // Test to see if this cell should be selected.
  if (cell->caloDDE()->getSampling() == m_sampling) {
    double etac = cell->eta();
    double phic = cell->phi();
    double etar = cell->caloDDE()->eta_raw();
    double phir = cell->caloDDE()->phi_raw();
    // Ideally, phi is computed as a deltaphi + phi to be safe with range
    // the offset is taken to be the seed from the previous sampling
    // I take the same offset for the raw and real computations
    double dphic = CaloPhiRange::diff (phic, m_phi);
    double dphir = CaloPhiRange::diff (phir, m_phi);

    // The conditions are applied in the calorimeter frame, the biggest difference w.r.t. before... 
    if (etar >= m_eta-m_deta/2. && etar <= m_eta+m_deta/2.) {
      if (dphir >= -m_dphi/2. && dphir <= m_dphi/2.) {

        // Is this the maximum-energy cell so far?
        double ene = cell->energy() * weight;

        if (ene > m_calc.m_emax) {
          m_calc.m_etamax = etac;
          m_calc.m_phimax = phic;
          m_calc.m_emax   = ene;
	  // In the calorimeter frame
          m_calc.m_etarmax = etar;
          m_calc.m_phirmax = phir;
        }

        // Update accumulation.
        m_s00 += ene;
        m_s10 += etac*ene;
        m_s20 += ene*etac*etac;
        m_s01 += dphic*ene;
        m_s02 += ene*dphic*dphic;
	// In the calorimeter frame
	m_s10r += etar*ene;
	m_s01r += dphir*ene;

        // Add the cell to the cluster, if requested.
        if (m_tofill)
          m_tofill->addCell(m_cell_container->findIndex(cell->caloDDE()->calo_hash()), weight);
      }
    }
  }
}


/**
 * @brief Calculate the layer variables from the accumulated cells
 * The variables in the parent @c CaloLayerCalculator instance
 * (passed to the constructor) are updated.
 */
CaloLayerCalculator::Helper::~Helper()
{
  m_calc.m_em   = m_s00;

  if (m_s00 > 0.) {
    // When we computed m_s01 and m_s02 above, we had subtracted phi from each
    // angle to avoid wrapping problems.  We need to add it back in
    // when computing the mean, but not when we compute the width.
    // To be pedantic (but i got this wrong the first time, so maybe
    // someone else will also be confused):
    //  We want <phic^2>-<phic>^2.
    //    s2 = s02/s00 = <(phic-phi)^2>
    //    s1 = s01/s00 = <(phic-phi)>
    //  Expanding the square in s2 gives
    //    <phic^2>-<phic>^2 = s2 - <phic>^2 + phi*(2<phic>-phi)
    //  Expanding the square in s1^2 and substituting for <phic>^2 above gives
    //    <phic^2>-<phic>^2 = s2 - s1^2
    double s1 = m_s01/m_s00;
    m_calc.m_phim = CaloPhiRange::fix (s1 + m_phi);
    m_calc.m_phis = m_s02/m_s00 - s1*s1;

    
    m_calc.m_etam = m_s10/m_s00;
    m_calc.m_etas = m_s20/m_s00 - m_calc.m_etam*m_calc.m_etam;

    // In the calorimeter frame
    m_calc.m_etamr = m_s10r/m_s00;
    m_calc.m_phimr = CaloPhiRange::fix(m_s01r/m_s00 + m_phi);

    // Reset position variables if the calculated eta,phi is outside
    // of the window defined by deta,dphi. This usually happens
    // if the negative energy within the window nearly cancels
    // the positive energy.
    if (std::abs (m_calc.m_etamr - m_eta) > m_deta ||
        CaloPhiRange::fix (m_calc.m_phimr - m_phi) > m_dphi)
    {
      m_calc.resetOnNegativeEnergy(m_eta,m_phi);
    }
  }
  else {
    //reset();//replaced by:
    m_calc.resetOnNegativeEnergy(m_eta,m_phi);
      
    //energy of a sampling CAN be negative (especially PS and back) 
    //when there is the (gaussian) noise
    //TEMPORARY SOLUTION ( energy negative is not reset to 0 anymore, 
    //but m_etam, m_phim, m_etas, m_phis are still null !! )
    // =>  << TO BE REDONE >>   
  }
  
  if (m_calc.m_etas > 0.) m_calc.m_etas = sqrt(m_calc.m_etas);
  if (m_calc.m_phis > 0.) m_calc.m_phis = sqrt (m_calc.m_phis);
}
