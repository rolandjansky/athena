// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloEMFrac.h,v 1.2 2007-10-26 01:47:22 ssnyder Exp $
/**
 * @file CaloConditions/CaloEMFrac.h
 * @brief Hold binned EM fraction data.
 */

#ifndef CALOCONDITIONS_CALOEMFRAC_H
#define CALOCONDITIONS_CALOEMFRAC_H


#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>


/**
 * @brief Hold binned EM fraction data.
 *
 * This class holds data on the average EM fraction of EM clusters.
 * These data are binned along four axes:
 *  - The (absolute value of the) cluster eta.
 *  - The (log10 of the) cluster energy.
 *  - The (log10 of the) cluster energy density.
 *    (In later versions, this is normalized by the cluster energy.)
 *  - The (log10 of the) cluster depth.
 *
 * In each bin, we store the mean EM fraction in the bin, plus information
 * about distribution width and number of entries.
 */
class CaloEMFrac {

public:

  /**
   * @brief Per-bin data.
   */
  struct EMFracData {
    /// The mean EM fraction in the bin.
    float m_fraction;

    /// Number of entries populating the bin.
    /// ??? FIXME: Someone check that this comment is correct.
    float m_entries;

    /// Width of the distribution.
    /// ??? FIXME: Someone check that this comment is correct.
    float m_error;

    /// Constructor.
    EMFracData() : m_fraction(0),m_entries(0),m_error(0) {};
  };


  /**
   * @brief Default Constructor.  (Only intended for use by persistency.)
   */
  CaloEMFrac();


  /**
   * @brief Constructor.
   * @param nBinEta      Number of bins in eta.
   * @param MinEta       Minimum eta for the first bin.
   * @param MaxEta       Maximum eta for the last bin.
   * @param nBinLogEClus Number of bins in log10(cluster energy).
   * @param MinLogEClus  Minimum log10(cluster energy) for the first bin.
   * @param MaxLogEClus  Maximum log10(cluster energy) for the last bin.
   * @param nBinLogEDens Number of bins in log10(cluster energy density).
   * @param MinLogEDens  Minimum log10(cluster energy density) for the first bin.
   * @param MaxLogEDens  Maximum log10(cluster energy density) for the last bin.
   * @param nBinLogDepth Number of bins in log10(cluster depth).
   * @param MinLogDepth  Minimum log10(cluster depth) for the first bin.
   * @param MaxLogDepth  Maximum log10(cluster depth) for the last bin.
   */
  CaloEMFrac(int   nBinEta, 
	     float MinEta, 
	     float MaxEta, 
	     int   nBinLogEClus,
	     float MinLogEClus,
	     float MaxLogEClus,
	     int   nBinLogEDens,
	     float MinLogEDens,
	     float MaxLogEDens,
	     int   nBinLogDepth,
	     float MinLogDepth,
	     float MaxLogDepth  
	     );
  

  /// Return the number of bins in eta.
  int getNbinsEta() const {return m_nBinEta;}
  /// Return the minimum eta for the first bin.
  float getEtaMin() const {return m_MinEta;}
  /// Return the maximum eta for the last bin.
  float getEtaMax() const {return m_MaxEta;}

  /// Return the number of bins in log10(cluster energy).
  int getNbinsLogEClus() const {return m_nBinLogEClus;}
  /// Return the minimum log10(cluster energy) for the first bin.
  float getLogEClusMin() const {return m_MinLogEClus;}
  /// Return the maximum log10(cluster energy) for the last bin.
  float getLogEClusMax() const {return m_MaxLogEClus;}

  /// Return the number of bins in log10(cluster energy density).
  int getNbinsLogEDens() const {return m_nBinLogEDens;}
  /// Return the minimum log10(cluster energy density) for the first bin.
  float getLogEDensMin() const {return m_MinLogEDens;}
  /// Return the maximum log10(cluster energy density) for the last bin.
  float getLogEDensMax() const {return m_MaxLogEDens;}

  /// Return the number of bins in log10(cluster depth).
  int getNbinsLogDepth() const {return m_nBinLogDepth;}
  /// Return the minimum log10(cluster depth) for the first bin.
  float getLogDepthMin() const {return m_MinLogDepth;}
  /// Return the maximum log10(cluster depth) for the last bin.
  float getLogDepthMax() const {return m_MaxLogDepth;}


  /**
   * @brief Return EM fraction data for one bin.
   * @param eta Cluster eta.
   * @param logEClus log10(cluster energy)
   * @param logEDens log10(cluster energy density) (possibly normalized)
   * @param logDepth log10(cluster depth)
   * @returns Pointer to the bin data, or 0 if out of range.
   *
   *  The pointer may be invalidated by a subsequent setEMFracData call.
   */
  const struct EMFracData * getEMFracData(float eta,
                                          float logEClus,
                                          float logEDens,
                                          float logDepth) const;


  /**
   * @brief Set EM fraction data for one bin.
   * @param eta Cluster eta.
   * @param logEClus log10(cluster energy)
   * @param logEDens log10(cluster energy density) (possibly normalized)
   * @param logDepth log10(cluster depth)
   * @param theData The bin data.
   *
   * This will overwrite any earlier data for the same bin.
   * If the coordinates are out of range, this is a no-op.
   */
  void setEMFracData(float eta,
                     float logEClus,
                     float logEDens,
                     float logDepth,
                     const struct EMFracData & theData);


private:

  /// Number of eta bins.
  int m_nBinEta; 
  /// Number of log10(cluster energy) bins.
  int m_nBinLogEClus;
  /// Number of log10(cluster energy density) bins.
  int m_nBinLogEDens;
  /// Number of log10(cluster depth) bins.
  int m_nBinLogDepth;

  /// Minimum eta for the first bin.
  float m_MinEta; 
  /// Minimum log10(cluster energy) for the first bin.
  float m_MinLogEClus;
  /// Minimum log10(cluster energy density) for the first bin.
  float m_MinLogEDens;
  /// Minimum log10(cluster depth) for the first bin.
  float m_MinLogDepth;

  /// Maximum eta for the last bin.
  float m_MaxEta; 
  /// Maximum log10(cluster energy) for the last bin.
  float m_MaxLogEClus;
  /// Maximum log10(cluster energy density) for the last bin.
  float m_MaxLogEDens;
  /// Maximum log10(cluster depth) for the last bin.
  float m_MaxLogDepth;

  /// The table.
  /// Binned in the following order, least-rapidly varying first:
  ///    eta, logEClus, logEDens, logDepth.
  std::vector<struct EMFracData> m_data;


  /**
   * @brief Find the eta/logEClus bin number.
   * @param eta Cluster eta.
   * @param logEClus log10(cluster energy)
   * @returns Bin number in the eta/logEClus subspace, or -1 if out of range.
   */
  int getEtaLogEClusBin(float eta, float logEClus) const;


  /**
   * @brief Find a bin number.
   * @param eta Cluster eta.
   * @param logEClus log10(cluster energy)
   * @param logEDens log10(cluster energy density) (possibly normalized)
   * @param logDepth log10(cluster depth)
   * @returns The bin number, or -1 if out of range.
   */
  int getBin(float eta, float logEClus, float logEDens, float logDepth) const;


};//end class


CLASS_DEF(CaloEMFrac,42479507,1)


#endif
