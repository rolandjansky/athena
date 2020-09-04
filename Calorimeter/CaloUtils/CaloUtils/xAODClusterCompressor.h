/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOUTILS_XAODCLUSTERCOMPRESSOR_H
#define CALOUTILS_XAODCLUSTERCOMPRESSOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "CaloInterface/IxAODClusterCompressor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "CxxUtils/checker_macros.h"
#include "CxxUtils/FloatCompressor.h"

class ATLAS_CHECK_THREAD_SAFETY xAODClusterCompressor : 
  virtual public IxAODClusterCompressor, public AthService { 
  
 protected:   
  friend class SvcFactory<xAODClusterCompressor>;


  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  xAODClusterCompressor() = delete;
  /// Constructor with parameters: 
  xAODClusterCompressor(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode  initialize() override;
  static const InterfaceID& interfaceID() { return IID_IxAODClusterCompressor;}

  virtual void compress(xAOD::CaloClusterContainer* clustercontainer) const override;
  
 virtual StatusCode queryInterface( const InterfaceID& riid, 
				    void** ppvInterface ) override;

  //List of all moments
  typedef std::array< xAOD::CaloCluster::MomentType,60> momentList_t;
  momentList_t m_allMoments;

  //Use Float compressor from CxxUtils
  CxxUtils::FloatCompressor m_compressor;
 
  //JobO-driven flag to turn compression on/off
  bool m_isEnabled;

}; 

#endif //> !CALOUTILS_XAODCLUSTERCOMPRESSOR_H
