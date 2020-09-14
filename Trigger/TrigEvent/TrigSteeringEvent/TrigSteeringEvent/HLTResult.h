/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
// emacs -*- c++ -*-
/**
 * \file HLTResult.h
 * \author gianluca.comune@cern.ch simon.george@cern.ch andreas.hoecker@cern.ch
 */

#ifndef TRIGSTEERINGEVENT_HLTResult_H
#define TRIGSTEERINGEVENT_HLTResult_H

#include <vector>
#include <map>
#include <string>
#include <set>
#include <stdint.h>
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/GenericResult.h"

#ifdef XAOD_STANDALONE
#include "xAODCore/ClassID_traits.h"
#else
#include "AthenaKernel/IClassIDSvc.h"
#endif

#include "xAODCore/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"

/****************************************************************************************
 * (June 2006; comments on documentation to: Andreas.Hoecker@cern.ch)
 * Oct 2006;  Tomasz.Bold@cern.ch removed most of internal variables, general restructure
 *            removed most of the methods
 * HLTResult implements a GenericResult which is communcated between PSC and HLT.
 *
 ****************************************************************************************/
namespace HLT {

  class HLTExtraData;
  
  /**
   * @brief HLT::HLTResult is sumarising result of trigger decision evaluation (online/offline)
   * It contains basically 3 parts:
   *
   * -# header word which are holding very basic ingormation line if event was accepted or error occurred
   * -# serialized trigger chains info
   * -# serialized navigation structure (eventually together with objects)
   */

  class HLTResult : public GenericResult {

  public:
    friend void swap(HLTResult& lhs, HLTResult& rhs);
  
    HLTResult(); //<! Default ctor
    HLTResult(const HLTResult&); //!< copy constructor
    HLTResult(HLTResult&& rhs); //!< move ctor
    HLTResult& operator=(HLTResult rhs); //!< unified assignement operator
    ~HLTResult(); //<! Dtor

    /**
     * @copydoc GenericResult::accepted()
     */
    virtual inline bool accepted() const { return isAccepted(); }

    /**
     * @copydoc GenericResult::error()
     */
    virtual bool error() const ;

    /**
     * @copydoc GenericResult::error_bits()
     */
    virtual uint32_t error_bits() const ;

    /**
     * @copydoc GenericResult::serialize(std::vector<uint32_t>& output, unsigned int mod_id)
     */
    bool serialize(std::vector<uint32_t>& output, const unsigned int mod_id);

    /**
     * @copydoc GenericResult::serialize(std::vector<uint32_t>& output)
     */
    bool serialize( std::vector<uint32_t>& output );

    /**
     * @copydoc GenericResult::serialize( uint32_t*& output, int& data_size, const int int max_size = -1, const unsigned int mod_id = 0)  
     */
    bool serialize( uint32_t*& output, int& data_size, const int max_size = -1, const unsigned int mod_id = 0 );

    /**
     * GenericResult::deserialize( const std::vector<uint32_t>& source )
     */
    bool deserialize( const std::vector<uint32_t>& source );

    /**
     * GenericResult::deserialize(  uint32_t* source, const int data_size )
     */
    bool deserialize(  uint32_t* source, const int data_size );

    /*
     * @brief ---- Trigger masks ----
     * here fill mask of all chains according to chain counter
     * best would be to send names of all existing signatures,
     * their active/inactive flags, and pass-through/prescale information
     * (need to enumerate chains in configuration)
     * corresponding functions should contain:
     *    trigger-mask for each step
     *    add chain/retrieve chain/serialize/deserialize
     */

    /**
     * @brief HLTResult version (set accessor is private)
     */
    uint32_t getHLTResultClassVersion() const { return m_HLTResultClassVersion; }

    /**
     * @brief gets event number (Lvl1-id)
     */
    inline uint32_t   getLvl1Id() const { return m_headerResult[IndEventNumber]; }

    /**
     * @brief sets event number (Lvl1-id)
     */
    inline void       setLvl1Id( uint32_t id ) { m_headerResult[IndEventNumber] = id; }

    /**
     * @brief gets HLT decision
     */
    inline bool      isAccepted() const { return (m_headerResult[IndHLTDecision] == 1 ? true : false); }

    /**
     * @brief sets HLT decision
     */
    inline void      setAccepted( bool acc ) { acc ? (m_headerResult[IndHLTDecision] = 1) : (m_headerResult[IndHLTDecision] = 0); }

    /**
     * @brief forced-accepted (FA) event
     *        configuration the FA request(s) are set per chain and hlt level
     *        by the tigger configuration; isForcedAccepted() is set to true
     *        if a chain has been accepted owing to the FA mechanism; this is
     *        independent of whether the event would been accepted anyway by
     *        this chain where the FA has been applied
     */
    inline bool      isPassThrough() const { return m_headerResult[IndPassThrough]; }
    /**
     * @brief set passTrough flag @see isPassThrough
     */
    inline void      setPassThrough( bool fa ) { fa ? (m_headerResult[IndPassThrough] = 1) : (m_headerResult[IndPassThrough] = 0) ; }

    /**
     * @brief overall hlt status: if StatusCode::isFailure(), event should be
     *        discarded from physics stream (and, eg, send to debug stream)
     *        (StatusCode enums are: Failure=0, Success=1, Recoverable=2,
     *         see GaudiKernel/ StatusCode.h for more information)
     */
    inline ErrorCode getHLTStatus() const { return  ErrorCode(m_headerResult[IndHLTStatus]); }
    inline void      setHLTStatus( ErrorCode hltStatus ) { m_headerResult[IndHLTStatus] = hltStatus; }

    /**
     * @brief overall hlt status: if StatusCode::isFailure(), event should be
     *        discarded from physics stream (and, eg, send to debug stream)
     *        (StatusCode enums are: Failure=0, Success=1, Recoverable=2,
     *         see GaudiKernel/ StatusCode.h for more information)
     */
    inline ErrorCode getLvlConverterStatus() const { return ErrorCode(m_headerResult[IndLvlConverterStatus]); }
    inline void      setLvlConverterStatus( ErrorCode status ) { m_headerResult[IndLvlConverterStatus] = status; }

    /**
     * @brief hlt level (L2 or EF)
     */
    inline HLTLevel   getHLTLevel() const { return (HLTLevel)m_headerResult[IndHLTLevelInfo]; }
    inline void       setHLTLevel( HLTLevel hltLevel ) { m_headerResult[IndHLTLevelInfo] = hltLevel; }
    
    /**
     * @brief is the result valid
     * @return true if result was ever stuffed, this is to mark results taht were never filled in steeing
     */
    inline bool       isValid() const { return (getHLTLevel() != HLT::UNKNOWN); }

    /**
     * @brief get number of satisfied signatures
     */
    inline uint32_t   getNumOfSatisfiedSigs() const { return m_headerResult[IndNumOfSatisfiedSigs]; }

    /**
     * @brief sget number of satisfied signatures
     */
    inline void       setNumOfSatisfiedSigs( uint32_t sigs ) { m_headerResult[IndNumOfSatisfiedSigs] = sigs; }

    /**
     * @brief sets the information about the error, namely chain in which the error happened
     * and the step
     * @param chainCounter says in which chain it was (0 denotes that error wasn;t inchains processing)
     * @param step is telling in which step in the chain the error occured
     */
    inline void      setErrorCoordinates( unsigned int chainCounter=0, unsigned int step=0 ) {
      m_headerResult[IndErrorInChain] = chainCounter;  m_headerResult[IndErrorInStep] = step; };

    /**
     * @brief get error coordinates @see setErrorCoordinates( unsigned int chainCounter, unsigned int step )
     */
    inline void      getErrorCoordinates( unsigned int & chainCounter, unsigned int & step ) const {
      chainCounter = m_headerResult[IndErrorInChain]; step = m_headerResult[IndErrorInStep]; }


    /**
     * @brief is HLTResult created externally, in PSC in example
     */
    bool       isCreatedOutsideHLT() const { return (m_headerResult[IndCreatedOutsideHLT] == 0 ? false: true); } // previously "hasBeenCreated"
    void       setCreatedOutsideHLT( bool created ) { created ? (m_headerResult[IndCreatedOutsideHLT] = 1) : (m_headerResult[IndCreatedOutsideHLT] = 0) ; }

    
    /**
     * @brief sets the key of the menu which was used to trigger this event
     */
    void       setConfigSuperMasterKey(uint32_t key);

    /**
     * @brief gets the key of the menu which was used to trigger this event
     * @warning This info was introduced in version 2 of HLTresult for former data this method returns 0
     */
    uint32_t    getConfigSuperMasterKey() const;


    /**
     * @brief sets the key of the prescales config which was used to trigger this event
     */
    void       setConfigPrescalesKey(uint32_t key);

    /**
     * @brief gets the key of the prescales config which was used to trigger this event
     * @warning This info was introduced in version 2 of HLTresult for former data this method returns 0
     */
    uint32_t    getConfigPrescalesKey() const;


    /**
     * @brief is serialized HLTResult truncated
     */
    bool       isHLTResultTruncated() const { return (m_headerResult[IndHLTResultTruncated] == 0 ? false : true); }
    void       setHLTResultTruncated( bool truncated) {
      m_headerResult[IndHLTResultTruncated] = truncated ? 1 : 0;
    }

    /**
     * @brief retrieve the sub-payloads (can be used to retrieve/write data)
     */
    const std::vector<uint32_t>& getNavigationResult() const { return m_navigationResult; }
    const std::vector<uint32_t>& getNavigationResult_DSonly() const { return m_navigationResult_DSonly; }
    const std::vector<uint32_t>& getChainResult     () const { return m_chainsResult; }
    const std::vector<uint32_t>& getExtras() const { return m_extras; }

    /**
     * @brief gets ref to the internal array holding places for safe navigation truncations
     */
    std::vector<unsigned int>& getNavigationResultCuts() { return m_navigationResultCuts; }
    const std::vector<unsigned int>& getNavigationResultCuts() const { return m_navigationResultCuts; }
     
    /**
     * @brief gets ref to the internal array holding places for safe navigation truncations
     */
    std::vector<unsigned int>& getNavigationResultCuts_DSonly() { return m_navigationResultCuts_DSonly; }
    const std::vector<unsigned int>& getNavigationResultCuts_DSonly() const { return m_navigationResultCuts_DSonly; }

    /**
     * @brief gets ref to the internal vector of pairs of CLID and collection name
     */
    std::vector< std::pair <CLID, std::string> >& getNavigationResultIDName() { return m_id_name; }
    const std::vector< std::pair <CLID, std::string> >& getNavigationResultIDName() const { return m_id_name; }
    /**
     * @brief gets ref to the internal vector of pairs of CLID and collection name
     */
    std::vector< std::pair <CLID, std::string> >& getNavigationResultIDName_DSonly() { return m_id_name_DSonly; }
    const std::vector< std::pair <CLID, std::string> >& getNavigationResultIDName_DSonly() const { return m_id_name_DSonly; }

    /**
     * @brief gets ref to the internal array holding serialized navigation
     */
    std::vector<uint32_t>& getNavigationResult() { return m_navigationResult; }

    /**
     * @brief gets ref to the internal array holding serialized navigation
     */
    std::vector<uint32_t>& getNavigationResult_DSonly() { return m_navigationResult_DSonly; }

    /**
     * @brief gets ref to the internal array holding serialized chains
     */
    std::vector<uint32_t>& getChainResult     () { return m_chainsResult; }


    /*
     * @brief gets piece of extra information
     * @warning This method should not be used. Use getExtraData() to modify the payload object.
     */
    std::vector<uint32_t>& getExtras()       { return m_extras; }

    /**
     * @brief methods to get and set the Scouting Map saved into HLTResult
     */
    std::map<unsigned int, std::set<std::pair<CLID, std::string> > >& getScoutingMap() {return  m_modID_id_name;}
    const std::map<unsigned int, std::set<std::pair<CLID, std::string> > >& getScoutingMap() const {return  m_modID_id_name;}
    
    void  setScoutingMap(std::map<unsigned int, std::set<std::pair<CLID, std::string> > > map_modid_clid_name) {m_modID_id_name = map_modid_clid_name;}  

    /*
     * @brief Return object representing the extra payload
     */
    HLTExtraData& getExtraData();
    inline const HLTExtraData& getExtraData ATLAS_NOT_THREAD_SAFE () const {
      return const_cast<HLTResult*>(this)->getExtraData();
    }

    /*
     * @brief gets size of the rawResult (in words)
     */
    unsigned int size() const;

    /*
     * @brief gets sizes of 3 main parts of the result (in words)
     */
    std::vector<unsigned int> partSizes() const;

    /**
     * @brief true if result is empty
     */
    bool isEmpty() const;

    /**
     * @brief map of satisfied signatures (needed by TriggerDecisionMaker)
     *        returns a vector of satisfied signatures for this event;
     *        it is a vector of pairs where first is signature ID,
     *        second is signature label(name).
     */
    // std::vector<std::pair<unsigned int, std::string> > getSatisfiedSigs();

    /**
     * @brief return the rob module ID vector.
     * (fills up rod_mod_ids vector given by reference)
     * @warning note that vector is not cleared
     **/
    void listOfModIDs(std::vector<unsigned int>& mod_ids) const;

    /**
     * @brief return the rob module ID vector
     */
    std::vector<unsigned int> listOfModIDs() const;
    

  private:
    friend class HLTResultCnv_p1;

    /**
     * @brief fixed bits; general trigger and status information
     */
    enum InitBits { IndHLTResultClassVersion = 0,
		    IndEventNumber,        //!< event number (from EventInfo::EventID::event_number())
		    IndHLTDecision,        //!< HLT decision (== 0 if event has been rejected at HLT)
		    IndPassThrough,        //!< has the event been forced (passed through)
		    IndHLTStatus,          //!< HLT status corresponding to ErrorCode enums
		    IndLvlConverterStatus, //!< LvlConverter status corresponding to ErrorCode enums
		    IndHLTLevelInfo,       //!< the HLT level
		    IndNumOfSatisfiedSigs, //!< number of satisfied signatures
		    IndErrorInChain,       //!< chain ID in which the error occured, in normal conditions sbould be 0
		    IndErrorInStep,        //!< step number in which error occured, in normal conditions sbould be 0
		    IndCreatedOutsideHLT,  //!< also an error identifier
		    IndHLTResultTruncated, //!< the serialize function could not fit everything into the given max data_size
		    IndConfigSuperMasterKey, //!< configuration key for the menu
		    IndConfigPrescalesKey,   //!< configuration key for prescales		    
		    IndNumOfFixedBit       //!< total number of fixed bits
    };

    using CutPairs = std::vector<std::pair<unsigned int, unsigned int>>;
    using CutPairVecs = std::pair<CutPairs, CutPairs>;

    /*
     * Serialize the regular result
     */
    bool serialize_regular(uint32_t*& output,
                           int& data_size,
                           int max_size);

    /*
     * Serialize the DS result corresponding to the given mod_id
     */
    bool serialize_DS(uint32_t*& output,
                      int& data_size,
                      int max_size,
                      unsigned int mod_id);

    /*
     * Bootstrap the serialization by serializing an initial portion of the end
     * result which is common to both regular and DS results
     */
    bool serialize_bootstrap(uint32_t*& output,
                             int& data_size,
                             bool& truncating,
                             int max_size,
                             unsigned int estimated_size);

    /*
     * Serialize the body of the regular result, that is, everything after the
     * bootstrap part
     */
    bool serialize_body_regular(uint32_t* output,
                                int& data_size,
                                unsigned int umax_size,
                                bool truncating) const;

    /*
     * Serialize the body of a DS result, that is, everything after the
     * bootstrap part
     */
    bool serialize_body_DS(uint32_t* output,
                           int& data_size,
                           unsigned int max_size,
                           unsigned int nav_size,
                           const CutPairVecs& dscuts,
                           bool truncating) const;

    /*
     * Serialize the navigation part of the regular result
     */
    bool serialize_navigation_reg(uint32_t* output,
                                  int& data_size,
                                  unsigned int umax_size,
                                  bool truncating) const;

    /*
     * Serialize the navigation part of a DS result
     */
    bool serialize_navigation_DS(uint32_t* output,
                                 int& data_size,
                                 unsigned int umax_size,
                                 unsigned int nav_size,
                                 const CutPairVecs& dscuts,
                                 bool truncating) const;

    /*
     * Update serialized extra data
     */
    void updateExtras();

    /**
     * Estimate the size this HLTResult would ocuppy once serialized.
     */
    unsigned int estimateSize() const;

    /**
     * Calculate the size of a DS result, given the size of its navigation
     */
    static unsigned int calc_total_size_DS(unsigned int ds_nav_size);

    /**
     * Find the pairs of cut points that mark the boundaries of DS collections
     * for the given module id.
     * @return A pair whose first element has the boundaries of collections
     *  found in the DS result and whose second element has the boundaries
     *  of collections found only in the regular result
     */
    CutPairVecs findDSCuts(unsigned int) const;

    /**
     * @brief split the rawResult vector into rawPartialResult's
     */
    bool unpackFromStorable(const std::vector<uint32_t>& raw);

    /**
     * @brief the version of this HLTResult class
     */
    static const uint32_t m_HLTResultClassVersion;

    /**
     * @brief the full payload, and sub-payloads
     */

    std::vector<uint32_t>             m_headerResult;      //!< storage of header words
    std::vector<uint32_t>             m_chainsResult;      //!< storege of serialized chains
    std::vector<uint32_t>             m_navigationResult;  //!< storage of navigation (serialized also)
    std::vector<uint32_t>             m_navigationResult_DSonly;  //!< storage of navigation (serialized also) for DataScouting
    std::vector<uint32_t>             m_extras;            //!< extra storeage (which can be used to store some operational infos)

    std::vector< std::pair < CLID, std::string > >     m_id_name;
    
    std::vector< std::pair < CLID, std::string > >     m_id_name_DSonly;
    
    std::map<unsigned int, std::set<std::pair<CLID, std::string> > > m_modID_id_name; // map from rob module ID to class ID and collection name

    std::vector<unsigned int> m_navigationResultCuts;

    std::vector<unsigned int> m_navigationResultCuts_DSonly;

    HLTExtraData* m_extraData;     //!< object for m_extras deserialization (on demand)

  };
  
  void swap(HLTResult& lhs, HLTResult& rhs);

} // end namespace

CLASS_DEF(HLT::HLTResult, 107385089, 1)

#endif
 
