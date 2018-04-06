/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTEDMCREATOR_H
#define TRIGOUTPUTHANDLING_HLTEDMCREATOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// OutputHandling includes
#include "TrigOutputHandling/IHLTOutputTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

/**
 * @class Tool capable of creating collections missing (early rejection) after HLT processing
 * @brief It declares output handles for all possible collections produced by the trigger
 *
 *  
 * to add more classes follow this steps:
 * 1) add EDM pkg in CMakeLists
 * 2) add includes in this file
 * 3) add DEF_*_KEY here
 * 4) initialise it 
 * 5) add create in the createOutput
 * 6) test 
 **/

class HLTEDMCreator: public extends<AthAlgTool, IHLTOutputTool>  { 
 public: 
  HLTEDMCreator( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  virtual ~HLTEDMCreator(){}
  virtual StatusCode createOutput() const override;
  virtual StatusCode initialize() override;


 private: 

  HLTEDMCreator();

#define DEF_KEY(__TYPE) \
  SG::WriteHandleKeyArray<__TYPE> m_##__TYPE{ this, #__TYPE, {}, "Required collections of "#__TYPE}

#define DEF_XAOD_KEY(__TYPE) \
  SG::WriteHandleKeyArray<xAOD::__TYPE> m_##__TYPE{ this, #__TYPE, {}, "Required collections of xAOD::"#__TYPE}

  DEF_KEY( TrigRoiDescriptorCollection );

  DEF_XAOD_KEY( TrigCompositeContainer );
  DEF_XAOD_KEY( TrigEMClusterContainer );
  DEF_XAOD_KEY( TrigCaloClusterContainer );
  DEF_XAOD_KEY( TrigElectronContainer );
  DEF_XAOD_KEY( TrigPhotonContainer );
  DEF_XAOD_KEY( TrackParticleContainer );


#undef DEF_KEY
#undef DEF_XAOD_KEY

  /**
   * Warns if collections do nto adhere to naming convention i.e. key starting from HLT_
   * and inits them
   **/
  template<typename T>
  StatusCode initAndCheckHandles( SG::WriteHandleKeyArray<T>& handles );


  /**
   * @brief Checks if input handles of the same names as these in @arg handles are present, if not create output
   * Second argument is functional capable of collection creation & calling apropropriate record on the handle passed 
   * as first argument.
   * Customisation of xAOD and non-xAOD happens through the generator argument.
   **/
  template<typename T, typename G>
  StatusCode createIfMissing( const SG::WriteHandleKeyArray<T>& handles, G generator ) const;

}; 


#endif //> !TRIGOUTPUTHANDLING_HLTEDMCREATOR_H
