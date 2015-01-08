/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOUTILS_XAODCLUSTERCOMPRESSOR_H
#define CALOUTILS_XAODCLUSTERCOMPRESSOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/IxAODClusterCompressor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class xAODClusterCompressor : 
  virtual public IxAODClusterCompressor, public AthAlgTool { 
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  xAODClusterCompressor() = delete;
  /// Constructor with parameters: 
  xAODClusterCompressor( const std::string& type,
			 const std::string& name, 
			 const IInterface* parent );

  /// Destructor: 
  virtual ~xAODClusterCompressor(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  void compress(xAOD::CaloClusterContainer* clustercontainer) const;

 private: 
  //Private type:
  union floatshort_t {
    float fvalue;
    uint32_t ivalue;
  };


  //Private methods:
  float reduceFloatPrecision(const float fValue) const;
  
  //Magic numbers:
  //half of the LSB-value after cutting the lower 16 bits
  const uint32_t m_rounding=0x00008000;

  //Largest possible positive 32bit float minus the rounding 
  const uint32_t m_vmax=0x7f7f7fff;

  //List of all moments
  typedef std::array< xAOD::CaloCluster::MomentType,60> momentList_t;
  momentList_t m_allMoments;
 
  //JobO-driven flag to turn compression on/off
  bool m_isEnabled;

}; 

#endif //> !CALOUTILS_XAODCLUSTERCOMPRESSOR_H
