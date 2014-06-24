/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2CALOJETCALIB_DBOBJ_H
#define T2CALOJETCALIB_DBOBJ_H

#include "CLIDSvc/CLASS_DEF.h"
#include<string>

/** @class T2CaloJetCalib_dBObj
    
    LVL 2 jet calibration dB data objects
    Contains the weights, eta range, and energy cuts

    @author Malachi Schram
 */
class T2CaloJetCalib_dBObj{

 public:
  /** Constructor */
  T2CaloJetCalib_dBObj();

  /** Destructor */
  ~T2CaloJetCalib_dBObj();

  //Define lvl2 data type typedef
  typedef std::vector<double> calib_dt;

  /*----------------------------------------------------------------*/
  /*------------------------ S E T T E R ---------------------------*/
  /*----------------------------------------------------------------*/
  /** Set the calibration weight */
  void set_calib_weights(calib_dt this_calib_dt);

  /** Retrieves the energy cuts */
  void set_ve_cuts(calib_dt this_calib_dt);

  /** Retrieves the eta range of the weights */
  void set_eta_ranges(calib_dt this_calib_dt);

  /*----------------------------------------------------------------*/
  /*------------------------ G E T T E R ---------------------------*/
  /*----------------------------------------------------------------*/
  /** Retrieves the calibration weight */
  inline calib_dt calib_weights() const{
    return m_calib_weights;
  } 

  /** Retrieves the energy cuts */
  inline calib_dt ve_cuts() const {
    return m_ve_cuts;
  }

  /** Retrieves the eta range of the weights */
  inline calib_dt eta_ranges() const {
    return m_eta_ranges;
  }

 private:
  calib_dt m_calib_weights;
  calib_dt m_ve_cuts;
  calib_dt m_eta_ranges;
  /** String variable used to id the set of weights */
  //std::string m_db_tag;
};

CLASS_DEF(T2CaloJetCalib_dBObj, 63773719, 1)

#endif

