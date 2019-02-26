// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODCUTFLOW_VERSIONS_CUTBOOKKEEPER_V1_H
#define XAODCUTFLOW_VERSIONS_CUTBOOKKEEPER_V1_H

// STL includes
#include <string>
#include <iosfwd>
#include <stdint.h>
#include <vector>

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "CxxUtils/checker_macros.h"
namespace xAOD {

  /// Description of the class that is used to keep track of event counts.
  ///
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class CutBookkeeper_v1 : public SG::AuxElement {

  public:

    /// Enumeration of the logic of a cut
    enum CutLogic {
      UNKNOWN            = 0,
      ALLEVENTSPROCESSED = 1,
      ALLEVENTSWRITTEN   = 2,
      OTHER              = 3,
      ACCEPT             = 4,
      REQUIRE            = 5,
      VETO               = 6,
    };


    /// Add struct for payload and implement operator *,/....*=,...
    struct Payload {
    public:
      Payload operator+( const Payload& rhs ) {
        Payload result;
        result.nAcceptedEvents          = this->nAcceptedEvents + rhs.nAcceptedEvents;
        result.sumOfEventWeights        = this->sumOfEventWeights + rhs.sumOfEventWeights;
        result.sumOfEventWeightsSquared = this->sumOfEventWeightsSquared + rhs.sumOfEventWeightsSquared;
        return result;
      }

      Payload& operator+=( const Payload& rhs ) {
        this->nAcceptedEvents          += rhs.nAcceptedEvents;
        this->sumOfEventWeights        += rhs.sumOfEventWeights;
        this->sumOfEventWeightsSquared += rhs.sumOfEventWeightsSquared;
        return *this;
      }

    public:
      uint64_t nAcceptedEvents;
      double   sumOfEventWeights;
      double   sumOfEventWeightsSquared;
    };



    /// Constructor
    CutBookkeeper_v1();



    ///  Destructor
    ~CutBookkeeper_v1();



    /// Equality operator
    bool operator==( const CutBookkeeper_v1& rhs ) {
      return this->isEqualTo( &rhs );
    }


    /// @name Getters and setters for the (sort of) 'static' information of the CutBookkeeper
    /// @{

    /// Get the nameIdentifier for this CutBookkeeper
    unsigned int nameIdentifier() const;
    /// Get the uniqueIdentifier for this CutBookkeeper
    unsigned int uniqueIdentifier() const;

  private:
    /// Set the nameIdentifier for this CutBookkeeper
    void setNameIdentifier( unsigned int hash );
    /// Set the uniqueIdentifier for this CutBookkeeper
    void setUniqueIdentifier( unsigned int hash );

  public:
    /// Get the name of this CutBookkeeper
    const std::string& name() const;
    /// Set the name of this CutBookkeeper
    void setName( const std::string& name );

    /// Get the description of this CutBookkeeper
    const std::string& description() const;
    /// Set the description of this CutBookkeeper
    void setDescription( const std::string &description );


    /// Check if this CutBookkeeper was a top filter
    bool isTopFilter() const;
    /// Set the top-filter property of this CutBookkeeper
    void setTopFilter( bool isTopFilter );


    /// Get the cut logic of this CutBookkeeper, e.g., ACCEPT, REQUIRE, VETO
    CutLogic cutLogic() const;
    /// Set the cut logic of this CutBookkeeper, e.g., ACCEPT, REQUIRE, VETO
    void setCutLogic( CutLogic logic );

    /// Get the skimming cycle that this CutBookkeeper was running in
    int cycle() const;
    /// Set the skimming cycle that this CutBookkeeper is running in
    void setCycle( int cycle );

    /// Get the name of the input-file stream object that was seen by this CutBookkeeper
    const std::string& inputStream() const;
    /// Set the name of the current input-file stream object for this CutBookkeeper
    void setInputStream( const std::string& inputstream );

    /// Get the names of the output-file stream objects that were seen by this CutBookkeeper
    const std::vector<std::string>& outputStreams() const;
    /// Check if the given output stream name is known to this CutBookkeeper
    bool hasOutputStream( const std::string& outputstream ) const;
    /// Add the name of an output-file stream object for this CutBookkeeper
    void addOutputStream( const std::string& outputstream );
    /// Add the name of an output-file stream object for this CutBookkeeper
    void addOutputStreamForAllUsed ATLAS_NOT_THREAD_SAFE ( const std::string& outputstream );
    /// Set the names of the output-file stream objects for this CutBookkeeper
    void setOutputStreams( const std::vector<std::string>& outputstreams );

    /// @}


    /// @name Getters and setters for the payload information, i.e., event coutners and weights
    /// @{

    /// Get the whole payload object (which contains all counters) in one go.
    /// Return it by value as it only contains three numbers.
    Payload payload() const;
    /// Set the whole payload object (which contains all counters) in one go
    void setPayload( const Payload& payload );

    /// Get the number of accepted events that this CutBookkeeper has seen
    uint64_t nAcceptedEvents() const;
    /// Set the number of accepted events for this CutBookkeeper
    void setNAcceptedEvents( uint64_t nEvents );
    /// Add seen events to the number of accepted events that this CutBookkeeper has seen
    void addNAcceptedEvents( uint64_t nEvents );

    /// Get the sum-of-event-weights that this CutBookkeeper has seen
    double sumOfEventWeights() const;
    /// Set the sum-of-event-weights that this CutBookkeeper has seen
    void setSumOfEventWeights( double nWeightedEvents );
    /// Add more sum-of-event-weights that this CutBookkeeper has seen
    void addSumOfEventWeights( double nWeightedEvents );

    /// Get the sum-of-(event-weights-squared) that this CutBookkeeper has seen
    double sumOfEventWeightsSquared() const;
    /// Set the sum-of-(event-weights-squared) that this CutBookkeeper has seen
    void setSumOfEventWeightsSquared( double nWeightedEventsSquared );
    /// Add more sum-of-(event-weights-squared) that this CutBookkeeper has seen
    void addSumOfEventWeightsSquared( double nWeightedEventsSquared );

    /// @}


    /// Test for the equality of this CutBookkeeper with another one
    bool isEqualTo( const CutBookkeeper_v1 *eb ) const;



    /// @name Handling of the child-parent relationship between different CutBookkeepers
    /// @{

    /// Check if there is a parent CutBookkeeper of this CutBookkeeper
    bool hasParent() const;
    /// Get the parent CutBookkeeper
    const xAOD::CutBookkeeper_v1* parent() const;
    /// Set the parent CutBookkeeper of this CutBookkeeper
    void setParent( const CutBookkeeper_v1* parentEB );
    /// Get the number of children CutBookkeepers of this CutBookkeeper
    std::size_t nChildren() const;
    /// Test if a given child already exists
    bool hasChild( const xAOD::CutBookkeeper_v1* testCBK ) const;
    /// Get the child at position i
    const xAOD::CutBookkeeper_v1* child( std::size_t i ) const;
    /// Set all children of this CutBookkeeper in one go
    void setChildren( const std::vector< CutBookkeeper_v1* >& childrenEB );
    /// Add multiple children in one go to this CutBookkeeper
    void addChildren( const std::vector< CutBookkeeper_v1* >& childrenEB );
    /// Add one child to this CutBookkeeper
    void addChild( CutBookkeeper_v1* childEB );
    /// Create a new CutBookkeeper (using the given name and description)
    /// and add this new CutBookkeeper to this one as a child
    CutBookkeeper_v1* addNewChild( const std::string& name,
                                     const std::string& description );

    /// @}



    /// @name Handling of the usage of other CutBookkeepers by this one
    /// @{

    /// Check if this CutBookkeeper has used others
    std::size_t nUsedOthers() const;
    /// Check if this CutBookkeeper has used others
    bool hasUsedOther( const xAOD::CutBookkeeper_v1* testCBK ) const;
    /// Get the usedOther at position i
    const xAOD::CutBookkeeper_v1* usedOther( std::size_t i ) const;
    /// Set all CutBookkeeper that are used by this one in one go
    void setUsedOthers( const std::vector< CutBookkeeper_v1* >& usedOthers );
    /// Add multiple children in one go to this CutBookkeeper
    void addUsedOthers( const std::vector< CutBookkeeper_v1* >& usedOthers );
    /// Add one other CutBookkeeper that is used by this one
    void addUsedOther( CutBookkeeper_v1* usedOtherEB );

    /// @}



    /// @name Handling of the siblings of this CutBookkeepers, i.e., the cuts
    /// that happen at the same cut stage as this one, but in another (control) region
    /// @{

    /// Check if this CutBookkeeper has siblings
    std::size_t nSiblings() const;
    /// Test if the provided CutBookkeeper is already a sibling
    bool hasSibling( const xAOD::CutBookkeeper_v1* testCBK ) const;
    /// Get the sibling number i
    const xAOD::CutBookkeeper_v1* sibling( std::size_t i ) const;
    /// Set all CutBookkeeper that are siblings to this one in one go
    void setSiblings( const std::vector< CutBookkeeper_v1* >& siblings );
    /// Add multiple siblings in one go to this CutBookkeeper
    void addSiblings( const std::vector< CutBookkeeper_v1* >& siblings );
    /// Add one sibling CutBookkeeper to this one
    void addSibling( CutBookkeeper_v1* siblingsEB );

    /// @}


    /// Function preparing the object to be persistified
    void toPersistent();


  };


} // namespace xAOD


#endif //> XAODCUTFLOW_VERSIONS_CUTBOOKKEEPER_V1_H
