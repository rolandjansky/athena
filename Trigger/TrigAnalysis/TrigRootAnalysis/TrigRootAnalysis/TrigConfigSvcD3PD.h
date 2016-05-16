// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfigSvcD3PD.h 726663 2016-02-28 01:22:15Z ssnyder $
#ifndef TRIGROOTANALYSIS_TRIGCONFIGSVCD3PD_H
#define TRIGROOTANALYSIS_TRIGCONFIGSVCD3PD_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

#include <stdint.h>

// ROOT include(s):
#include <TNamed.h>

// Local include(s):
#include "IITrigConfigSvcD3PD.h"

// Forward declaration(s):
class TTree;
class TBranch;

namespace D3PD {

   /**
    *  @short Class acting as a trigger configuration service on D3PDs
    *
    *         This class can be used to ease reading the information out of the
    *         trigger configuration metadata trees of D3PDs.
    *
    *         The user is required to get hold of the trigger configuration
    *         tree himself/herself, but after that this class takes care of loading
    *         the correct entries from the TTree.
    *
    * @author Attila Krasznahrokay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 726663 $
    * $Date: 2016-02-28 02:22:15 +0100 (Sun, 28 Feb 2016) $
    */
   class TrigConfigSvcD3PD : public ::TNamed,
                             public IITrigConfigSvcD3PD {

      /// A convenience definition for an internal type
      typedef std::pair< ::Int_t, std::pair< ::Int_t, ::Int_t > > DBKeys_t;

   public:
      /// Constructor with a pointer to the D3PD trigger configuration tree
      TrigConfigSvcD3PD( ::TTree* confTree = 0 );

      /// Set the object to use a new configuration tree
      ::Bool_t SetConfigTree( ::TTree* confTree );

      /// Load the configuration belonging to a DB entry
      ::Bool_t Load( ::Int_t SMK, ::Int_t L1PSK, ::Int_t HLTPSK,
                     ::Bool_t silent = kFALSE );
      /// Check if a given configuration is available
      ::Bool_t IsAvailable( ::Int_t SMK, ::Int_t L1PSK, ::Int_t HLTPSK ) const;

      /// Check if a given LVL1 item / HLT chain is configured
      ::Bool_t IsConfigured( const std::string& name ) const;

      /// Get the CTP ID of a LVL1 item
      ::Int_t GetCTPId( const std::string& name ) const;
      /// Get the chain ID of an HLT chain
      ::Int_t GetChainId( const std::string& name ) const;
      /// Get the prescale of a trigger (LVL1 or HLT)
      ::Float_t GetPrescale( const std::string& name ) const;
      /// Get the rerun prescale value of an HLT chain
      ::Float_t GetRerunPrescale( const std::string& name ) const;
      /// Get the passthrough value of an HLT chain
      ::Float_t GetPassthrough( const std::string& name ) const;
      /// Get the lower chain name of an HLT chain
      const std::string& GetLowerChainName( const std::string& name ) const;

      /// Get the item name corrsponding to a given CTP ID
      const std::string& GetNameFromCTPId( ::Int_t ctpid ) const;
      /// Get the LVL2 chain name corrsponding to a given chain ID
      const std::string& GetL2NameFromChainId( ::Int_t chainid ) const;
      /// Get the EF chain name corrsponding to a given chain ID
      const std::string& GetEFNameFromChainId( ::Int_t chainid ) const;

      /// Get all the configured LVL1 item names
      const std::vector< std::string >& GetLVL1Items() const;
      /// Get all the configured HLT chain names
      const std::vector< std::string >& GetHLTChains() const;

      /// Print a summary of all the available configurations
      void PrintSummary() const;
      /// Print details about the currently loaded configuration
      void PrintConfig() const;

      // Begin [TrigMonConf]
      // Get the D3PD index for a given chain counter
      UInt_t GetChainIndexFromCounter(UInt_t counter) const;
      /// Map an OutputTE from a Signature to its Sequence
      UInt_t GetSequenceIndex( UInt_t outputTE ) const;
      /// Get a sequences name from its index
      const std::string& GetSequenceNameFromIndex( unsigned int index ) const;
      /// Get an algorithms name from its parent sequence and its position in said sequence
      const std::string& GetAlgNameFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const;
      /// Get an algorithms name hash from its parent sequence and its position in said sequence
      UInt_t GetAlgNameIDFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const;
      /// Get an algorithms class name from its parent sequence and its poisition in said sequence
      const std::string& GetAlgClassNameFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const;
      /// Get an algorithms class name hash from its parent sequence and its poisition in said sequence
      UInt_t GetAlgClassNameIDFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const;
      /// Use full flat config information to populate helper maps
      void GenerateSignatureSequenceMap();

      // Chain
      UInt_t      GetChainN() const;
      UInt_t      GetChainLevel(UInt_t c) const;
      UInt_t      GetChainCounter(UInt_t c) const;
      std::string GetChainName(UInt_t c) const;
      UInt_t      GetChainEBHypoNameSize(UInt_t c) const;
      std::string GetChainEBHypoName(UInt_t c, UInt_t h) const;
      UInt_t      GetChainGroupNameSize(UInt_t c) const;
      std::string GetChainGroupName(UInt_t c, UInt_t g) const;
      UInt_t      GetChainStreamNameSize(UInt_t _c) const;
      std::string GetChainStreamName(UInt_t _c, UInt_t _g) const;
      // Chain->Sig
      UInt_t      GetSigN(UInt_t c) const;
      UInt_t      GetSigCounter(UInt_t c, UInt_t s) const;
      UInt_t      GetSigLogic(UInt_t c, UInt_t s) const;
      std::string GetSigLabel(UInt_t c, UInt_t s) const;
      UInt_t      GetSigNOutputTE(UInt_t c, UInt_t s) const;
      UInt_t      GetSigOutputTE(UInt_t c, UInt_t s, UInt_t t) const;

      // Seq
      UInt_t       GetSeqN() const;
      UInt_t       GetSeqID(UInt_t s) const;
      UInt_t       GetSeqIndex(UInt_t s) const;
      std::string  GetSeqName(UInt_t s) const;
      UInt_t       GetSeqNInputTEs(UInt_t s) const;
      UInt_t       GetSeqInputTE(UInt_t s, UInt_t t) const;
      // Seq->Alg
      UInt_t      GetAlgN(UInt_t s) const;
      UInt_t      GetAlgIndex(UInt_t s, UInt_t a) const;
      UInt_t      GetAlgPosition(UInt_t s, UInt_t a) const;
      UInt_t      GetAlgNameID(UInt_t s, UInt_t a) const;
      std::string GetAlgName(UInt_t s, UInt_t a) const;
      UInt_t      GetAlgTypeID(UInt_t s, UInt_t a) const;
      std::string GetAlgTypeName(UInt_t s, UInt_t a) const;

      UInt_t      GetMetaStringN() const;
      std::string GetMetaStringKey(UInt_t m) const;
      std::string GetMetaStringVal(UInt_t m) const;
      // End [TrigMonConf]

   private:
      /// Initialize the object from the specified TTree
      ::Bool_t Initialize( TTree* tree );
      /// Reset the object
      void Reset();
      /// Extract the configured LVL1 item and HLT chain names
      ::Bool_t UpdateTriggerNames();
      /// Create the key object describing a configuration
      static DBKeys_t MakeDBKeys( ::Int_t SMK, ::Int_t L1PSK, ::Int_t HLTPSK );

      /// All LVL1 item -> CTP key associations
      std::map< DBKeys_t, std::map< std::string, int > > m_lvl1NameMap;
      /// All LVL1 item -> Prescale value associations
      std::map< DBKeys_t, std::map< std::string, float > > m_lvl1PrescaleMap;

      /// All HLT chain -> Chain ID associations
      std::map< DBKeys_t, std::map< std::string, int > > m_hltNameMap;
      /// All HLT chain -> Prescale value associations
      std::map< DBKeys_t, std::map< std::string, float > > m_hltPrescaleMap;
      /// All HLT chain -> Rerun prescale value associations
      std::map< DBKeys_t, std::map< std::string, float > > m_hltRerunPrescaleMap;
      /// All HLT chain -> Passthrough value associations
      std::map< DBKeys_t, std::map< std::string, float > > m_hltPassthroughMap;

      /// All HLT chain -> Seeding chain / item associations
      std::map< DBKeys_t, std::map< std::string, std::string > > m_lowerChainNameMap;

      /// [TrigMonConfig] Chain Counter -> D3PD location map
      std::map< DBKeys_t, std::map< unsigned int, unsigned int > > m_chainCounterMap;

      /// [TrigMonConfig] All Sig Output TE -> index of Seq with the corresponding Input TE
      std::map< DBKeys_t, std::map< unsigned int, unsigned int > > m_TEIndexMap;

      /// [TrigMonConfig] Sequence Index -> Sequence Name
      std::map< DBKeys_t, std::map< unsigned int , std::string > > m_hltSequenceNameMap;

      /// [TrigMonConfig] pair<SequenceID, Alg Position> -> Algorithm Name
      std::map< DBKeys_t, std::map< std::pair< int, int > , std::string > > m_hltAlgorithmNameMap;

      /// [TrigMonConfig] pair<SequenceID, Alg Position> -> Algorithm Name Hash
      std::map< DBKeys_t, std::map< std::pair< int, int > , unsigned int > > m_hltAlgorithmNameIDMap;

      /// [TrigMonConfig] pair<SequenceID, Alg Position> -> Algorithm Class Name
      std::map< DBKeys_t, std::map< std::pair< int, int > , std::string > > m_hltAlgorithmTypeMap;

      /// [TrigMonConfig] pair<SequenceID, Alg Position> -> Algorithm Class Name Hash
      std::map< DBKeys_t, std::map< std::pair< int, int > , unsigned int > > m_hltAlgorithmTypeIDMap;

      ::Bool_t m_initialized; ///< Internal status flag
      DBKeys_t m_key; ///< Current configuration key

      /// Reverse of m_lvl1NameMap
      std::map< DBKeys_t, std::map< int, std::string > > m_lvl1IDMap;
      /// Reverse of m_hltNameMap (for LVL2 chains)
      std::map< DBKeys_t, std::map< int, std::string > > m_lvl2IDMap;
      /// Reverse of m_hltNameMap (for EF chains)
      std::map< DBKeys_t, std::map< int, std::string > > m_efIDMap;

      /// Internal cache of the LVL1 item names
      std::map< DBKeys_t, std::vector< std::string > > m_confLVL1Items;
      /// Internal cache of the HLT chain names
      std::map< DBKeys_t, std::vector< std::string > > m_confHLTChains;

      std::string m_dummy; ///< Dummy string needed for technical reasons
      std::vector< std::string > m_dummyVec; ///< Dummy vector

      /// [TrigMonConfig]
      // variables for CHAIN
      std::map< DBKeys_t, uint16_t >                                  m_chainN;
      std::map< DBKeys_t, std::vector< uint32_t > >                   m_chainID;
      std::map< DBKeys_t, std::vector< uint16_t > >                   m_chainCounter;
      std::map< DBKeys_t, std::vector< std::string > >                m_chainName;
      std::map< DBKeys_t, std::vector< uint32_t > >                   m_chainLowerID;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_chainLowerIDs;
      std::map< DBKeys_t, std::vector< std::string > >                m_chainLowerName;
      std::map< DBKeys_t, std::vector< uint16_t > >                   m_chainLowerCounter;
      std::map< DBKeys_t, std::vector< uint8_t > >                    m_chainLevel;
      std::map< DBKeys_t, std::vector< float > >                      m_chainPrescale;
      std::map< DBKeys_t, std::vector< float > >                      m_chainPassthrough;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_chainStreamNames;
      std::map< DBKeys_t, std::vector< std::vector< float > > >       m_chainStreamPrescales;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_chainGroupNames;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_chainEBHypoNames;
      // variables for CHAIN -> SIGNATURE
      std::map< DBKeys_t, std::vector< uint16_t > >                   m_chainSigN;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_chainSigCounter;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_chainSigLogic;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_chainSigLabel;
      std::map< DBKeys_t, std::vector< std::vector< uint16_t > > >    m_chainSigOutputTEn;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_chainSigOutputTEIndex;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_chainSigOutputTEs;
      // variables for SEQUENCE
      std::map< DBKeys_t, uint16_t >                                  m_seqN;
      std::map< DBKeys_t, std::vector< uint32_t > >                   m_seqID;
      std::map< DBKeys_t, std::vector< uint16_t > >                   m_seqIndex;
      std::map< DBKeys_t, std::vector< std::string > >                m_seqName;
      std::map< DBKeys_t, std::vector< uint32_t > >                   m_seqTopoTEID;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_seqInputTEs;
      // variables for SEQUENCE -> ALG
      std::map< DBKeys_t, std::vector< uint16_t > >                   m_seqAlgN;
      std::map< DBKeys_t, std::vector< std::vector< uint16_t > > >    m_seqAlgIndex;
      std::map< DBKeys_t, std::vector< std::vector< uint8_t > > >     m_seqAlgPosition;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_seqAlgNameID;
      std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >    m_seqAlgTypeID;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_seqAlgName;
      std::map< DBKeys_t, std::vector< std::vector< std::string > > > m_seqAlgTypeName;
      // variables for META
      std::map< DBKeys_t, std::vector< std::string > >                m_metaStringKey;
      std::map< DBKeys_t, std::vector< std::string > >                m_metaStringVal;

      ClassDef( D3PD::TrigConfigSvcD3PD, 0 )

   }; // class TrigConfigSvcD3PD

} // namespace D3PD

#endif // TRIGROOTANALYSIS_TRIGCONFIGSVCD3PD_H
