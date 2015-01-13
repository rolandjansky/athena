// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfigSvcAthD3PD.h 637174 2014-12-20 00:26:47Z ssnyder $
#ifndef TRIGROOTANALYSIS_TRIGCONFIGSVCATHD3PD_H
#define TRIGROOTANALYSIS_TRIGCONFIGSVCATHD3PD_H

// STL include(s):
#include <vector>
#include <string>

// Gaudi/Athena include(s):
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/ReadHandle.h"

// Local include(s):
#include "../TrigRootAnalysis/ITrigConfigSvcD3PD.h"
#include "../TrigRootAnalysis/TrigConfigSvcD3PD.h"

namespace D3PD {

   /**
    *  @short Athena implementation for the IITrigConfigSvcD3PD interface
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 637174 $
    * $Date: 2014-12-20 01:26:47 +0100 (Sat, 20 Dec 2014) $
    */
   class TrigConfigSvcAthD3PD : public virtual AthService,
                                public virtual ITrigConfigSvcD3PD,
                                public virtual IIncidentListener {

   public:
      /// Gaudi Service constructor
      TrigConfigSvcAthD3PD( const std::string& name, ISvcLocator* svcloc );

      /// Function querying the interface of the object
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvInterface );

      /// Function initializing the service
      virtual StatusCode initialize();

      /// Function called when a registered incident happens
      virtual void handle( const Incident& inc );

      /// Check if a given LVL1 item / HLT chain is configured
      virtual ::Bool_t IsConfigured( const std::string& name ) const;

      /// Get the CTP ID of a LVL1 item
      virtual ::Int_t GetCTPId( const std::string& name ) const;
      /// Get the chain ID of an HLT chain
      virtual ::Int_t GetChainId( const std::string& name ) const;
      /// Get the prescale of a trigger (LVL1 or HLT)
      virtual ::Float_t GetPrescale( const std::string& name ) const;
      /// Get the rerun prescale value of an HLT chain
      virtual ::Float_t GetRerunPrescale( const std::string& name ) const;
      /// Get the passthrough value of an HLT chain
      virtual ::Float_t GetPassthrough( const std::string& name ) const;
      /// Get the lower chain name of an HLT chain
      virtual const std::string& GetLowerChainName( const std::string& name ) const;

      /// Get the item name corrsponding to a given CTP ID
      virtual const std::string& GetNameFromCTPId( ::Int_t ctpid ) const;
      /// Get the LVL2 chain name corrsponding to a given chain ID
      virtual const std::string& GetL2NameFromChainId( ::Int_t chainid ) const;
      /// Get the EF chain name corrsponding to a given chain ID
      virtual const std::string& GetEFNameFromChainId( ::Int_t chainid ) const;

      /// Get all the configured LVL1 item names
      virtual const std::vector< std::string >& GetLVL1Items() const;
      /// Get all the configured HLT chain names
      virtual const std::vector< std::string >& GetHLTChains() const;

      /// Print a summary of all the available configurations
      virtual void PrintSummary() const;
      /// Print details about the currently loaded configuration
      virtual void PrintConfig() const;

   private:
      /// The object doing the havy lifting
      TrigConfigSvcD3PD m_svc;

      /// The current event's SMK
      SG::ReadHandle< unsigned int > m_smk;
      /// The current event's L1PSK
      SG::ReadHandle< unsigned int > m_l1psk;
      /// The current event's HLTPSK
      SG::ReadHandle< unsigned int > m_hltpsk;

      /// Name of the event tree in the job
      std::string m_tupleName;
      /// Names of the input files in the job
      std::vector< std::string > m_files;

   }; // class TrigConfigSvcAthD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_TRIGCONFIGSVCATHD3PD_H
