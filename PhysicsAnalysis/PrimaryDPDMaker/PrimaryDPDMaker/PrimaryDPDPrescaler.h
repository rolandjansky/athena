/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef PRIMARYDPD_PRESCALER_H
#define PRIMARYDPD_PRESCALER_H

#include <string>
#include <vector>

#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODEventInfo/EventInfo.h"


class PrimaryDPDPrescaler : public AthReentrantAlgorithm {
public: // Constructor and Destructor
  /// Standard Service Constructor
  PrimaryDPDPrescaler(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~PrimaryDPDPrescaler()= default;
  
public:
  /// Gaudi Service Interface method implementations:
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const  override;
  StatusCode finalize()override;
  
  // Private data members
private: 
  /// handle to the @c IAthRNGSvc we want to test
    ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service"};   
  
    Gaudi::Property<double> m_prescale{this , "Prescale", 1.};
    mutable std::atomic<unsigned int> m_accepted{0};
    mutable std::atomic<unsigned int> m_vetoed{0};
    unsigned int m_rollover{0};

    /// Keep these two properties for now in order not to break the current configuration
    
     /// List of algorithms names to combine with logical OR
    StringArrayProperty      m_theAcceptAlgNames;

    /// List of algorithms names to combine with logical AND
    StringArrayProperty      m_theRequireAlgNames;
      

};


#endif
