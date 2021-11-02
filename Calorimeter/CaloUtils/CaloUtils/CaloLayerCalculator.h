// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloLayerCalculator.h,v 1.1 2006-03-20 17:42:26 ssnyder Exp $
/**
 * @file  CaloLayerCalculator.h
 * @author scott snyder <snyder@bnl.gov>
 * @date March, 2006
 * @brief Calculate total energy, position, etc. for a given layer of
 *        a cluster.
 *
 * This tool will calculate for a given layer of an EM cluster:
 *
 *   - The total energy.
 *   - The mean @f$\eta@f$ and @f$\phi@f$ positions and widths.
 *   - The @f$\eta@f$ and @f$\phi@f$ position of the cell with
 *     the maximum energy, and the amount of energy in that cell.
 *
 * The list of cells to be used for the calculation is specified
 * by giving an iterator range; the cells within a rectangular region
 * in a specified layer are chosen.  If the list of cells is
 * from a @c Navigable, then any weights set will be used in
 * in the calculation.  The input list of cells can also
 * be specified by giving the name of a @c CaloCellContainer
 * in storegate.  Optionally, the selected cells can also be added
 * to the cluster.
 *
 * Derived from previous versions:
 * AUTHORS:  S. Rajagopalan
 * CREATED:  Nov 2000
 *           Separated from LArComputerEMLayer
 * PURPOSE:  Calculation of sampling based quantities
 * Atrecon Orig: emreco/qgcalo.age 
 *
 * Updated:  Jan 5, 2001    (HM)
 *           QA. 
 * 
 * Updated: April 11, 2002 (HMA)
 *          Moved to LArCalorimeter/LArRecUtils
 * 	    Dec. 15, 2002 (FD)
 * 	    Add initialisation in the constructor
 * 	    Add position of the hottest cell (m_imax)
 * 
 * Update : January 21, 2005 (MB)
 *          Move to Calorimeter/CaloClusterCorrection
 * 
 * Update : September 06, 2005 (DLelas)
 *          Two new methods (two fillCluster) added.
 *          These two methods, in addition to energy 
 *          and position calculations,
 *          fill cells in rectangular (SW) cluster layers.
 * 
 * Update : January 16, 2006 (DLelas)
 *          fill method was changed to meet egamma calculation
 *          requirements. Two new booleans were added to steer
 *          calculation needed by egamma (flag_celllist) and
 *          to steer cluster cell weightings (flag_cellweight)    
 */

#ifndef CALOUTILS_CALOLAYERCALCULATOR_H
#define CALOUTILS_CALOLAYERCALCULATOR_H

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloEvent/CaloClusterCellLink.h"
#include "Navigation/NavigableIterator.h"

#include "GaudiKernel/StatusCode.h"
#include <string>

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

class CaloCellContainer;
class CaloCell;

class CaloLayerCalculator
{
public:
  /** Initialization.*/
  //@{

  /**
   * @brief Default constructor.
   *
   * Sets the variables to invalid values.
   */
  CaloLayerCalculator();


  /**
   * @brief Calculate layer variables from cells in a list.
   * @param beg The starting iterator for the cell list.
   * @param end The ending iterator for the cell list.
   * @param eta The @f$\eta@f$ center of the cluster, for cell selection.
   * @param phi The @f$\phi@f$ center of the cluster, for cell selection.
   * @param deta The @f$\eta@f$ full width for cell selection.
   * @param dphi The @f$\phi@f$ full width for cell selection.
   * @param sampling The sampling for cell selection.
   * @param tofill If non-null, then selected cells will be added
   *               to this cluster.
   *
   * This takes a list of cells specified by @c beg and @c end,
   * selects those within a @c deta by @c dphi window around @c eta, @c phi
   * in layer @c sampling, and fills the layer variables from them.
   * If the iterators come from a @c Navigable, then cell weights
   * will be used in the calculation.  If @c tofill and @c cell_container
   * are non-null, then selected cells will also be added to the cluster
   * @c tofill.
   */
  template <typename Iterator>
  void fill (Iterator beg,
             Iterator end,
             double eta,
             double phi,
             double deta,
             double dphi,
             CaloSampling::CaloSample sampling,
             xAOD::CaloCluster* tofill = 0);


  /**
   * @brief Calculate layer variables from a cell container.
   * @param mgr CaloDetDescrManager 
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
  StatusCode fill (const CaloDetDescrManager& mgr,
                   const CaloCellContainer* cell_container,
                   double eta,
                   double phi,
                   double deta,
                   double dphi,
                   CaloSampling::CaloSample sampling,
                   xAOD::CaloCluster* tofill = 0);


  //@}
  /** Data access. */
  //@{

  /**
   * @brief Get the mean layer @f$\eta@f$ value.
   * @return The mean @f$\eta@f$ of the selected cells, or -999.
   */
  double etam()   const;


  /**
   * @brief Get the mean layer @f$\phi@f$ value.
   * @return The mean @f$\phi@f$ of the selected cells, or -999.
   */
  double phim()   const;


  /**
   * @brief Get the RMS @f$\eta@f$ size.
   * @return The RMS @f$\eta@f$ of the selected cells.
   */
  double etas()   const;


  /**
   * @brief Get the RMS @f$\phi@f$ size.
   * @return The RMS @f$\phi@f$ of the selected cells.
   */
  double phis()   const;


  /**
   * @brief Get the total energy.
   * @return The total energy of the selected cells.
   */
  double em()     const;


  /**
   * @brief Get maximum @f$\eta@f$ value.
   * @return The @f$\eta@f$ center of the selected cell with the most energy,
   *         or -999.
   */
  double etamax() const;


  /**
   * @brief Get maximum @f$\phi@f$ value.
   * @return The @f$\phi@f$ center of the selected cell with the most energy,
   *         or -999.
   */
  double phimax() const;


  /**
   * @brief Get the maximum energy.
   * @return The energy of the selected cell with the most energy.
   */
  double emax()   const;

  /**
   * @brief Similar as phim, etam, phimax, etamax, but with raw quantities
   */
  double phimr() const { return m_phimr; }
  double etamr() const { return m_etamr; }
  double phirmax() const { return m_phirmax; }
  double etarmax() const { return m_etarmax; }

  //@}
private:
  /** Internals. */
  //@{

  /// Reset all variables to invalid values.
  void reset();

  /// Reset output variables, but not m_em, allowing samplings to be negative.
  void resetOnNegativeEnergy();

  /// Reset output variables, but not m_em, allowing samplings to be negative, fill layer position with original eta,phi seed position
  void resetOnNegativeEnergy(double eta, double phi);


  /// Get the weight from a cell iterator.  This is the generic
  /// version, that always returns 1.
  template <typename Iterator>
  static double get_weight (Iterator it);

  /// Get the weight from a cell iterator.  This is the version specialized
  /// for CaloClusterCellLink::const_iterator 
  double get_weight(CaloClusterCellLink::const_iterator it);

  /// Get the weight from a cell iterator.  This is the version specialized
  /// for CaloClusterCellLink::iterator 
  double get_weight(CaloClusterCellLink::iterator it);

  /// Get the weight from a cell iterator.  This is the version specialized
  /// for a @c Navigable, which returns the @c Navigable parameter.
  template <typename CONT, typename RPAR, typename COLL>
  double get_weight (NavigableIterator<CONT,RPAR,COLL> it);


  /// Layer variables.
  double   m_etam = 0.0;    // Mean @f$\eta@f$.
  double   m_phim = 0.0;    // Mean @f$\phi@f$.
  double   m_etas = 0.0;    // RMS @f$\eta@f$.
  double   m_phis = 0.0;    // RMS @f$\phi@f$.
  double   m_em = 0.0;      // Total energy.
  double   m_etamax = 0.0;  // @f$\eta@f$ center of maximum-energy cell.
  double   m_phimax = 0.0;  // @f$\phi@f$ center of maximum-energy cell.
  double   m_emax = 0.0;    // Energy of maximum-energy cell.

  // Added to cluster in the calorimeter frame
  double   m_etamr = 0.0;  // Mean @f$\eta@f$ with raw quantity
  double   m_phimr = 0.0;  // Mean @f$\phi@f$ with raw quantity
  double   m_etarmax = 0.0;  // @f$\eta_raw@f$ center of maximum-energy cell.
  double   m_phirmax = 0.0;  // @f$\phi_raw@f$ center of maximum-energy cell.


  /**
   * @brief Helper class for layer variable calculation.
   * This class is used to allow factoring out the layer variable
   * calculation itself (which we only want to write once) from
   * the loop over cells (of which there may be multiple varieties).
   * To compute the layer variables, a @c Helper instance is created;
   * it will contain the temporary variables used to accumulate the
   * results.  Then, for each cell in the list, call the @c cell
   * method of @c Helper.  At the end, the @c Helper destructor will
   * update the layer variables.
   */
  struct Helper
  {
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
    Helper (CaloLayerCalculator& calc,
            double eta,
            double phi,
            double deta,
            double dphi,
            CaloSampling::CaloSample sampling,
            xAOD::CaloCluster* tofill,
            const CaloCellContainer* cell_container);


    /**
     * @brief Accumulate results for another cell.
     * @param cell The cell to accumulate.
     * @param weight The weight for the cell.
     */
    void cell (const CaloCell* cell, double weight);


    /**
     * @brief Calculate the layer variables from the accumulated cells
     * The variables in the parent @c CaloLayerCalculator instance
     * (passed to the constructor) are updated.
     */
    ~Helper();


    /// These members hold the values passed to the constructor.
    CaloLayerCalculator& m_calc;
    double m_eta;
    double m_phi;
    double m_deta;
    double m_dphi;
    CaloSampling::CaloSample m_sampling;
    xAOD::CaloCluster* m_tofill;
    const CaloCellContainer* m_cell_container;

    /// Temporary variables used to accumulate the results.
    double m_s00; // energy sum.
    double m_s10; // eta-weighted energy sum.
    double m_s20; // eta^2-weighted energy sum.
    double m_s01; // phi-weighted energy sum.
    double m_s02; // phi^2-weighted energy sum.

    // Added to cluster in the calorimeter frame
    double m_s10r; // eta_raw-weighted energy sum.
    double m_s01r; // phi_raw-weighted energy sum.
  };
  friend struct Helper;
  //@}
};


/**
 * @brief Calculate layer variables from cells in a list.
 * @param beg The starting iterator for the cell list.
 * @param end The ending iterator for the cell list.
 * @param eta The @f$\eta@f$ center of the cluster, for cell selection.
 * @param phi The @f$\phi@f$ center of the cluster, for cell selection.
 * @param deta The @f$\eta@f$ full width for cell selection.
 * @param dphi The @f$\phi@f$ full width for cell selection.
 * @param sampling The sampling for cell selection.
 * @param tofill If non-null, then selected cells will be added
 *               to this cluster.
 * @param cell_container If @c tofill is non-null, then this is this
 *               container from which the cells came.
 *
 * This takes a list of cells specified by @c beg and @c end,
 * selects those within a @c deta by @c dphi window around @c eta, @c phi
 * in layer @c sampling, and fills the layer variables from them.
 * If the iterators come from a @c Navigable, then cell weights
 * will be used in the calculation.  If @c tofill and @c cell_container
 * are non-null, then selected cells will also be added to the cluster
 * @c tofill; in this case, @c cell_container should be the container
 * in storegate holding the cells.
 */
template <typename Iterator>
void CaloLayerCalculator::fill (Iterator beg,
                                Iterator end,
                                double eta,
                                double phi,
                                double deta,
                                double dphi,
                                CaloSampling::CaloSample sampling,
                                xAOD::CaloCluster* tofill /*= 0*/)
{
  reset();
  const CaloCellContainer* cell_container=0;
  if (tofill) cell_container=tofill->getCellLinks()->getCellContainer();
  if (beg != end) {
    Helper h (*this, eta, phi, deta, dphi, sampling, tofill, cell_container);
    for (; beg != end; ++beg)
      h.cell (*beg, get_weight (beg));
  }
}


/**
 * @brief Get the mean layer @f$\eta@f$ value.
 * @return The mean @f$\eta@f$ of the selected cells, or -999.
 */
inline
double CaloLayerCalculator::etam()   const
{
  return m_etam;
}


/**
 * @brief Get the mean layer @f$\phi@f$ value.
 * @return The mean @f$\phi@f$ of the selected cells, or -999.
 */
inline
double CaloLayerCalculator::phim()   const
{
  return m_phim;
}


/**
 * @brief Get the RMS @f$\eta@f$ size.
 * @return The RMS @f$\eta@f$ of the selected cells.
 */
inline
double CaloLayerCalculator::etas()   const
{
  return m_etas;
}


/**
 * @brief Get the RMS @f$\phi@f$ size.
 * @return The RMS @f$\phi@f$ of the selected cells.
 */
inline
double CaloLayerCalculator::phis()   const
{
  return m_phis;
}


/**
 * @brief Get the total energy.
 * @return The total energy of the selected cells.
 */
inline
double CaloLayerCalculator::em()     const
{
  return m_em;
}


/**
 * @brief Get maximum @f$\eta@f$ value.
 * @return The @f$\eta@f$ center of the selected cell with the most energy,
 *         or -999.
 */
inline
double CaloLayerCalculator::etamax() const
{
  return m_etamax;
}


/**
 * @brief Get maximum @f$\phi@f$ value.
 * @return The @f$\phi@f$ center of the selected cell with the most energy,
 *         or -999.
 */
inline
double CaloLayerCalculator::phimax() const
{
  return m_phimax;
}


/**
 * @brief Get the maximum energy.
 * @return The energy of the selected cell with the most energy.
 */
inline
double CaloLayerCalculator::emax()   const
{
  return m_emax;
}


/// Get the weight from a cell iterator.  This is the generic
/// version, that always returns 1.
template <typename Iterator>
inline
double CaloLayerCalculator::get_weight (Iterator /*it*/)
{
  return 1;
}


/// Get the weight from a cell iterator.  This is the version specialized
/// for a @c Navigable, which returns the @c Navigable parameter.
template <typename CONT, typename RPAR, typename COLL>
inline
double CaloLayerCalculator::get_weight (NavigableIterator<CONT,RPAR,COLL> it)
{
  return it.getParameter();
}

inline
double CaloLayerCalculator::get_weight(CaloClusterCellLink::const_iterator it)
{
  return it.weight();
}


inline
double CaloLayerCalculator::get_weight(CaloClusterCellLink::iterator it)
{
  return it.weight();
}


#endif // not CALOUTILS_CALOLAYERCALCULATOR_H

