/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERMOMENT_H
#define CALOCLUSTERMOMENT_H

/**
 * @class CaloClusterMoment
 * @version \$Id: CaloClusterMoment.h,v 1.8 2009-01-27 08:16:05 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 28-February-2005
 * @brief defines enums and data types for different moments of @c CaloCluster
 *
 * typically a cluster moment of a certain degree \f$n\f$ over a
 * variable \f$x\f$ defined for each @c CaloCell member of a @c CaloCluster
 * is defined as:
 *
 * \f[ \langle x^n\rangle = \frac{1}{E_{\rm norm}} \times \!\!\!\sum\limits_{\{{\rm cell}|E_{\rm cell}>0\}}{\!\!\!E_{\rm cell}\, x^n},\f]
 * with
 * \f[ E_{\rm norm} = \!\!\!\sum\limits_{\{{\rm cell}|E_{\rm cell}>0\}}{\!\!\!E_{\rm cell}}. \f]
 *
 * Note that only cells with positive energy are used in this definition.
 * Common variables to calculate first and second moments of are
 * \f$\phi\f$, \f$\eta\f$, and radial and longitudanal distances from
 * the shower axis and the shower center, respectively.
 */

class CaloClusterMoment {

public:
  /** @brief enums to identify different moments
   *
   *  Each enum corresponds to a different characteristic moment of a
   *  @c CaloCluster. The enums are loosly grouped by their type - i.e.
   *  first moment over an angle, second moment, angular deviations etc.
   */
  enum MomentType {
    FIRST_PHI         = 101, /**< First Moment in \f$\phi\f$.*/      
    FIRST_ETA         = 102, /**< First Moment in \f$\eta\f$.*/            
    SECOND_R          = 201, /**< Second Moment in \f$r\f$.*/                  
    SECOND_LAMBDA     = 202, /**< Second Moment in \f$\lambda\f$.*/                  
    DELTA_PHI         = 301, /**< Angular shower axis deviation (\f$\phi\f$) from IP-to-Center. */
    DELTA_THETA       = 302, /**< Angular shower axis deviation (\f$\theta\f$) from IP-to-Center. */
    DELTA_ALPHA       = 303, /**< Angular shower axis deviation from IP-to-Center. */
    CENTER_X          = 401, /**< Cluster Centroid (\f$x\f$). */
    CENTER_Y          = 402, /**< Cluster Centroid (\f$y\f$). */
    CENTER_Z          = 403, /**< Cluster Centroid (\f$z\f$). */
    CENTER_MAG        = 404, /**< Cluster Centroid (\f$\sqrt(x^2+y^2+z^2)\f$). */
    CENTER_LAMBDA     = 501, /**< Shower depth at Cluster Centroid. */
    LATERAL           = 601, /**< Normalized lateral moment. */
    LONGITUDINAL      = 602, /**< Normalized longitudinal moment. */
    ENG_FRAC_EM       = 701, /**< Energy fraction in EM calorimeters. */
    ENG_FRAC_MAX      = 702, /**< Energy fraction of hottest cell. */
    ENG_FRAC_CORE     = 703, /**< Energy fraction of the sum of the hottest cells in each sampling. */
    FIRST_ENG_DENS    = 804, /**< First Moment in E/V. */
    SECOND_ENG_DENS   = 805, /**< Second Moment in E/V. */
    ISOLATION         = 806, /**< energy weighted fraction of non-clustered perimeter cells. */

    ENG_BAD_CELLS     = 807, /**< total em-scale energy of bad cells in this cluster. */
    N_BAD_CELLS       = 808, /**< number of bad cells*/ 
    N_BAD_CELLS_CORR  = 809, /**< Number of bad cells with energy density average correction applied. */
    BAD_CELLS_CORR_E  = 813, /**< Energy of bad cells with energy density average correction applied. */
    BADLARQ_FRAC      = 821, /**< Energy fraction of LAr cells with quality larger than a given cut. */
    ENG_POS           = 822, /**< Total positive Energy of this cluster. */
    SIGNIFICANCE      = 823, /**< Cluster significance. */
    CELL_SIGNIFICANCE = 824, /**< Cell significance = E/sig of the cell with the largest |E|/sig. */
    CELL_SIG_SAMPLING = 825, /**< CaloSample of the cell with the largest |E|/sig. */
    AVG_LAR_Q         = 826, /**< Sum(E_cell_LAr^2 Q_cell_LAr)/Sum(E_cell_LAr^2). */
    AVG_TILE_Q        = 827, /**< Sum(E_cell_Tile^2 Q_cell_Tile)/Sum(E_cell_Tile^2). */
    EM_PROBABILITY    = 900, /**< classification probability to be em-like. */
    HAD_WEIGHT        = 901, /**< hadronic weight (E_w/E_em). */
    OOC_WEIGHT        = 902, /**< out-of-cluster weight (E_ooc/E_w). */
    DM_WEIGHT         = 903, /**< dead-material weight (E_dm/E_ooc). */
    TILE_CONFIDENCE_LEVEL = 904, /**< Confidence Level of a tile calorimeter cluster to be noise */
    VERTEX_FRACTION = 1000, /**< Vertex fraction of this cluster wrt. primary vertex of the event. Calculated in CaloRec/CaloClusterVertexFractionMaker.cxx */
    NVERTEX_FRACTION = 1001, /**< slightly updated vertex fraction more pile up independent (similar to nJVF) */
    ENG_CALIB_TOT     = 10001, /**< Calibration Hit energy inside the cluster. */  
    ENG_CALIB_OUT_L   = 10010, /**< Attached Calibration Hit energy
				  outside clusters but inside the calorimeter
				  with loose matching (Angle < 1.0)
			       */  
    ENG_CALIB_OUT_M   = 10011, /**< Attached Calibration Hit energy
				  outside clusters but inside the
				  calorimeter with medium matching
				  (Angle < 0.5). */  
    ENG_CALIB_OUT_T   = 10012, /**< Attached Calibration Hit energy
				  outside clusters but inside the
				  calorimeter with tight matching
				  (Angle < 0.3). */  
    ENG_CALIB_DEAD_L  = 10020, /**< Attached Calibration Hit energy in
				  dead material with loose matching
				  (Angle < 1.0). */  
    ENG_CALIB_DEAD_M  = 10021, /**< Attached Calibration Hit energy in
				  dead material with medium matching
				  (Angle < 0.5). */  
    ENG_CALIB_DEAD_T  = 10022,  /**< Attached Calibration Hit energy in
				  dead material with tight matching
				  (Angle < 0.3). */  

    ENG_CALIB_EMB0         = 10030, /**< Calibration Hit energy inside the cluster barrel presampler. */
    ENG_CALIB_EME0         = 10031, /**< Calibration Hit energy inside the cluster endcap presampler. */
    ENG_CALIB_TILEG3       = 10032, /**< Calibration Hit energy inside the cluster scintillator. */
    ENG_CALIB_DEAD_TOT     = 10040, /**< Attached Calibration Hit energy in dead material. */
    ENG_CALIB_DEAD_EMB0    = 10041, /**< Attached Calibration Hit energy in dead material before EMB0, between EMB0 and EMB1 */
    ENG_CALIB_DEAD_TILE0   = 10042, /**< Attached Calibration Hit energy in dead material between EMB3 and TILE0 */
    ENG_CALIB_DEAD_TILEG3  = 10043, /**< Attached Calibration Hit energy in dead material before scintillator */
    ENG_CALIB_DEAD_EME0    = 10044, /**< Attached Calibration Hit energy in dead material before EME0, between EME0 and EME1 */
    ENG_CALIB_DEAD_HEC0    = 10045, /**< Attached Calibration Hit energy in dead material between EME3 and HEC0 */
    ENG_CALIB_DEAD_FCAL    = 10046, /**< Attached Calibration Hit energy in dead material before FCAL, between FCAL and HEC */
    ENG_CALIB_DEAD_LEAKAGE = 10047, /**< Attached Calibration Hit energy in dead material behind calorimeters */
    ENG_CALIB_DEAD_UNCLASS = 10048,  /**< Attached Calibration Hit energy in dead material in unclassified areas of the detector */

    ENG_CALIB_FRAC_EM      = 10051, /**< Calibration Hit energy inside the cluster caused by e/gamma/pi0 */  
    ENG_CALIB_FRAC_HAD     = 10052, /**< Calibration Hit energy inside the cluster caused by charged pi+ and pi-. */  
    ENG_CALIB_FRAC_REST    = 10053  /**< Calibration Hit energy inside the cluster caused by other particles. */  
};

private:

  // Friends
  
  // Data members

  /** @brief the value of the cluster moment. */
  double m_value;

public:
  
  /** 
   * @brief Constructor.
   * @param value The actual value of the moment
   */
  CaloClusterMoment(const double &value):m_value(value) { }

  /** 
   * @brief default Constructor.
   *
   * defines the default moment value
   */
  CaloClusterMoment() : m_value(0.) { }

  // Destructors

  virtual ~CaloClusterMoment() { }

  // Methods

  /** 
   * @brief returns the value of this moment
   * @return a const reference to the double holding the value of the moment
   */
  const double & getValue() const { return m_value; }

};

#endif // CALOCLUSTERMOMENT_H

