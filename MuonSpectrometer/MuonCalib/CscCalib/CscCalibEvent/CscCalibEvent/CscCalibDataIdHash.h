/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBEVENT_CSCCALIBDATAIDHASH_H
#define CSCCALIBEVENT_CSCCALIBDATAIDHASH_H

/**************************************************************** 
  Hash function for CSC calibration data of a chamber
    - to be used in CscCalibDataContainer (IdentifiableContainer) 
 @author Ketevi A. Assamagan, BNL October 2005
 Name: CscCalibDataIdHash.h
 Package: MuonSpectrometer/MuonCalib/CscCalib/CscCalibEvent
*****************************************************************/

#include "MuonIdHelpers/CscIdHelper.h" 

class CscCalibDataIdHash  {

 public:

  typedef Identifier ID; 

  /** constructor */
  CscCalibDataIdHash ();

  /** destructor */
  virtual ~CscCalibDataIdHash();

  /** Convert ID to int */
  int operator() (const ID& id) const ;

  /** return maximum number of IDs */
  int max() const { return m_max; } 

  /** Initialize the Csc Id Helper */
  void setHelper (const CscIdHelper* helper) { m_cscHelper = helper; }

  /** get the CSC Id Helper */
  const CscIdHelper * cscIdHelper () { return m_cscHelper; }

 private:
  
  /** Identifier context */ 
  IdContext* m_context;

  /** total number of IDs */
  int m_max;

  /** CSC Id Helper */
  const CscIdHelper * m_cscHelper;

};

#endif



