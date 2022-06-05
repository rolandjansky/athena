/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONSTEST_LAROFCTOOFC_H
#define LARCONDITIONSTEST_LAROFCTOOFC_H 1

// STL includes
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"


class LArOFCtoOFC
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  LArOFCtoOFC( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~LArOFCtoOFC(){}; 

  virtual StatusCode  initialize() override {return StatusCode::SUCCESS;};
  virtual StatusCode  execute() override {return StatusCode::SUCCESS;};
  virtual StatusCode  stop() override;

 private: 

  /// Default constructor: 
  LArOFCtoOFC();

  std::string m_inKey;
  std::string m_inShapeKey;
  std::string m_outKey;
  std::string m_outShapeKey;
  bool m_HECshift; // add 0 in front for HEC ?
  bool m_doShape; // fill also the shape ?
  unsigned m_numAdd; // How many 0 to add

}; 

#endif //> !LARCONDITIONSTEST_LArOFCtoOFC_H
