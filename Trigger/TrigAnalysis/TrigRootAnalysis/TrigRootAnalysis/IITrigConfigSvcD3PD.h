// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IITrigConfigSvcD3PD.h 502294 2012-05-24 16:11:19Z krasznaa $
#ifndef TRIGROOTANALYSIS_IITRIGCONFIGSVCD3PD_H
#define TRIGROOTANALYSIS_IITRIGCONFIGSVCD3PD_H

// STL include(s):
#include <string>
#include <vector>

// ROOT include(s):
#include <Rtypes.h>

namespace D3PD {

   /**
    *  @short Pure virtual interface for the D3PD-based trigger configuration service
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502294 $
    * $Date: 2012-05-24 18:11:19 +0200 (Thu, 24 May 2012) $
    */
   class IITrigConfigSvcD3PD {

   public:
      /// Virtual destructor to make vtable happy
      virtual ~IITrigConfigSvcD3PD() {}

      /// Check if a given LVL1 item / HLT chain is configured
      virtual ::Bool_t IsConfigured( const std::string& name ) const = 0;

      /// Get the CTP ID of a LVL1 item
      virtual ::Int_t GetCTPId( const std::string& name ) const = 0;
      /// Get the chain ID of an HLT chain
      virtual ::Int_t GetChainId( const std::string& name ) const = 0;
      /// Get the prescale of a trigger (LVL1 or HLT)
      virtual ::Float_t GetPrescale( const std::string& name ) const = 0;
      /// Get the rerun prescale value of an HLT chain
      virtual ::Float_t GetRerunPrescale( const std::string& name ) const = 0;
      /// Get the passthrough value of an HLT chain
      virtual ::Float_t GetPassthrough( const std::string& name ) const = 0;
      /// Get the lower chain name of an HLT chain
      virtual const std::string& GetLowerChainName( const std::string& name ) const = 0;

      /// Get the item name corrsponding to a given CTP ID
      virtual const std::string& GetNameFromCTPId( ::Int_t ctpid ) const = 0;
      /// Get the LVL2 chain name corrsponding to a given chain ID
      virtual const std::string& GetL2NameFromChainId( ::Int_t chainid ) const = 0;
      /// Get the EF chain name corrsponding to a given chain ID
      virtual const std::string& GetEFNameFromChainId( ::Int_t chainid ) const = 0;

      /// Get all the configured LVL1 item names
      virtual const std::vector< std::string >& GetLVL1Items() const = 0;
      /// Get all the configured HLT chain names
      virtual const std::vector< std::string >& GetHLTChains() const = 0;

      /// Print a summary of all the available configurations
      virtual void PrintSummary() const = 0;
      /// Print details about the currently loaded configuration
      virtual void PrintConfig() const = 0;

   }; // class IITrigConfigSvcD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_IITRIGCONFIGSVCD3PD_H
