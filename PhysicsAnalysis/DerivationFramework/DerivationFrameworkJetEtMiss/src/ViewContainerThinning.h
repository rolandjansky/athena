///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ViewContainerThinning.h 
// Header file for class ViewContainerThinning
// Author: P-A Delsart
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H
#define DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"

// DerivationFrameworkInterfaces includes
#include "DerivationFrameworkInterfaces/IThinningTool.h"

// Forward declaration

class IThinningSvc;

namespace DerivationFramework {

  class ViewContainerThinning
    :             public AthAlgTool, public IThinningTool
  { 

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
  public: 

    // Copy constructor: 

    /// Constructor with parameters: 
    ViewContainerThinning( const std::string& type,
                           const std::string& name, 
                           const IInterface* parent );

    /// Destructor: 
    virtual ~ViewContainerThinning(); 

    // Athena algtool's Hooks
    virtual StatusCode  initialize();
    virtual StatusCode  finalize();
    virtual StatusCode doThinning() const;

  protected: 

    /// Default constructor: 
    ViewContainerThinning();

    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::string m_sourceContName;
    std::string m_viewContName;
    int m_particleType;
    bool m_and;
  

    template <class T>
    StatusCode doThinningT() const {
    
      const T * sourceCont;
      const T * viewCont;
      CHECK( evtStore()->retrieve( sourceCont, m_sourceContName) );
      CHECK( evtStore()->retrieve( viewCont, m_viewContName) );
    
      std::vector<bool> masks;
      masks.assign( sourceCont->size(), false);
    
      for( const auto* part: *viewCont){ 
        masks[ part->index() ] = true;
      }
    
      IThinningSvc::Operator::Type op = m_and ? IThinningSvc::Operator::And : IThinningSvc::Operator::Or ;
      if (m_thinningSvc->filter(*sourceCont, masks, op ).isFailure()) {
        ATH_MSG_ERROR("Application of thinning service failed! ");
        return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }

  }; 

}

#endif //> !DERIVATIONFRAMEWORKJETETMISS_VIEWCONTAINERTHINNING_H
