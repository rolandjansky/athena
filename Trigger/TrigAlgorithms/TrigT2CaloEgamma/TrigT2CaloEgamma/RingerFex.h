/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigCaloRinger/RingerFex.h
 * $Author: damazio $
 * $Revision: 351944 $
 * $Date$
 * 
 */

#ifndef RINGERFEX_H
#define RINGERFEX_H

#include <string>
#include "xAODTrigRinger/TrigRingerRings.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "TrigT2CaloEgamma/T2CaloEgamma.h"

class MsgStream;
class TrigEMCluster; // I dont know if put the xAOD prefix here!

typedef double Feature;
typedef std::vector<Feature> Pattern;

/**
 * This is the Ringer's tool. It creates the rings which are put into
 * RingerRings and then looked by a multi-variable hypothesis making
 * algorithm or regular LVL2 decision is taken by TrigL2CaloHypo.
 * The number, sampling and widths of the rings is fully
 * configurable.
 */
class RingerFex : public IAlgToolCalo {

protected:

  // Helper class used to build ringsets
  class RingSet {
    
  public: //interface
   
    /**
     * Builds a new RingSet, starting from a ring definition. The RingSet
     * starts empty.
     *
     */
    RingSet (unsigned int max, unsigned int maxCells, double etasz, double phisz, const std::vector<CaloSampling::CaloSample> &detectors);
    
    /**
     * to create an empty RingSet is possible, but watch out, you can't do
     * much with it...
     */
    RingSet ();

    /**
     * Virtualises the destructor
     */
    virtual ~RingSet();
    
    /**
     * Access the configuration for this object
     */
    inline const double& eta_size(void) const { return m_etasz; }
    inline const double& phi_size(void) const { return m_phisz; }
    inline size_t max(void) const { return m_max; }
    inline size_t maxCells(void) const { return m_maxCells; }
    const std::vector<CaloSampling::CaloSample>& detectors(void) const
      { return m_detector; }
    
    /**
     * Adds some set of cells to this RingSet.
     *
     * @param c The cells to add
     * @param eta_center Where, in eta, I should center my rings
     * @param phi_center Where, in phi, I should center my rings
     */
    void add (const std::vector<const CaloCell *>& c,
	      const double& eta_center, const double& phi_center);
    
    /**
     * Returns the (current) ring values.
     */
    inline Pattern& pattern (void) { return m_val; }
    
    /**
     * Resets all current values
     */
    inline void reset (void) { for (unsigned int i = 0; i < m_val.size(); ++i) m_val[i] = 0; }
    
  private: //representation

    double m_etasz; ///< the width of rings, in eta
    double m_phisz; ///< the width of rings, in phi
    size_t m_max; ///< the amount of rings to produce
    size_t m_maxCells; ///< the amount of cells to gather
    std::vector<CaloSampling::CaloSample> m_detector; ///< I'm good for those
    
    Pattern m_val; ///< my current values
    
    mutable float m_cachedOverEtasize; ///< cached value of 1/m_config.eta_size() for optimizations
    mutable float m_cachedOverPhisize; ///< cached value of 1/m_config.phi_size() for optimizations
    
  };

  // Start of central methods
public: 

  using IAlgToolCalo::execute;

  RingerFex (const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~RingerFex() { }
  
  StatusCode initialize();
  StatusCode finalize();
  //StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster, double etamin, double etamax, double phimin, double phimax);
  StatusCode execute(xAOD::TrigEMCluster &rtrigEmCluster, const IRoiDescriptor& roi);

private: // Representation
    
  std::string m_hlt_feature;
  std::string m_feature;
  std::string m_key;

  T2CaloEgamma *m_parent;

  std::vector<double> m_detaRings;
  std::vector<double> m_dphiRings;
  std::vector<unsigned int> m_nRings;
  std::vector<unsigned int> m_layersRings;
  std::vector<unsigned int> m_nlayersRings;
  std::vector<unsigned int> m_maxCells;
  std::vector<float> m_etaBins;


  unsigned int m_maxCellsAccumulated;
  unsigned int m_maxRingsAccumulated;

  std::vector<const CaloCell *>   m_cell;     // Cells at RoI (TODO: use multimap!)

  bool             m_global_center;           // Should I use a global center at e.m. 2?
  double           m_etaSearchWindowSize;     // The window size in eta for searching the hot cell.
  double           m_phiSearchWindowSize;     // The window size in phi for searching the hot cell. 
  
  std::vector<RingSet>	m_ringsSet;	      // It will containg the raw rings generated and their configuration.
  std::vector< std::vector<const CaloCell *> > m_cell_for_br; 

  bool m_saveRoI;                             // Should I save the RoI information on the StoreGate? (necessary to dump it on the Ringer CBNT algo!)

  /**
   * This method is responsible to generating the rings, by accessing the RoI cells supplied.
   * 
   * @return An object containing the generated rings.
   */
  xAOD::TrigRingerRings *getRings();
  inline StatusCode   getCells(double etamin, double etamax, double phimin, double phimax,
		                    std::vector<RingSet>& rset, bool global_center,
		                    const double& eta, const double& phi, 
		                    const double& eta_window,
		                    const double& phi_window);
  //void         storeDebugRoI(const std::string &hltFeatureLabel, const xAOD::TrigEMCluster &rtrig);
  //void storeDebugRoI();
  
protected:
  // For timing
  TrigTimer *m_generateClusterTimer, *m_saveRoITimer; ///< For timing.
  TrigTimer *m_RingerFexExecuteTimer, *m_getRingsTimer; ///< For timing.
  TrigTimer *m_RingerRegSelTimer; ///< For timing.
  std::string m_histLabel;

public:

  const static double PI_THRESHOLD;
  const static double ENERGY_THRESHOLD; // Defines the energy threshold for divisions, in MeV's

  /**
   * Helper function to identify wrap around cases.
   * if reverse_wrap = true, then we check for reverse wrap_arround.
   */
  static bool check_wrap_around(const double phi_center, const bool reverse_wrap) {
    if (reverse_wrap) return (phi_center < (RingerFex::PI_THRESHOLD - M_PI));
    else return (phi_center > (M_PI - RingerFex::PI_THRESHOLD));
  }

  /**
   * Helper function to fix wrap around cases.
   * if reverse_wrap = true, then we fix reverse wrap_arround cases.
   */
  static double fix_wrap_around(const double phi_value, const bool reverse_wrap) {
    if (reverse_wrap) {
      if (phi_value > 0.)  return (phi_value - 2*M_PI);
    } else {
      if (phi_value < 0.) return (phi_value + 2*M_PI);
    }
    return phi_value;
  }
  
private:

  /**
   * Calculates based on the RoI input and on the center previously calculated.
   *
   * @param vecCells The vector containing cells in this RoI
   * @param rset The ring set configuration to use for creating the rings
   * @param global_center If this value is set to <code>false</code> a layer based
   * center is calculated for the ring center. Otherwise, the values given on
   * the following variables are considered.
   * @param eta The center to consider when building the rings
   * @param phi The center to consider when building the rings
   * @param eta_window The window size in eta, to use when considering peak finding
   * @param phi_window The window size in phi, to use when considering peak finding
  void build_rings(const std::vector<const CaloCell *> &vecCells,
		   std::vector<RingSet>& rset, bool global_center,
		   const double& eta, const double& phi, 
		   const double& eta_window,
		   const double& phi_window);
  **/

  /**
   * Calculates the maximum energy cell in a CaloCell collection.
   */
  void maxCell (const std::vector<const CaloCell*>& vcell,
		double& eta, double& phi, const double& eta_ref,
		const double& phi_ref, const double& eta_window,
		const double& phi_window);

 protected:

};
#endif
