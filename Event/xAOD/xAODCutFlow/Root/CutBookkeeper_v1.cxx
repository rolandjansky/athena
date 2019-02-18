/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Implementation file for class CutBookkeeper_v1
// Author: Karsten Koeneke, May 2014 <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// Local include(s):
//#include "xAODCutFlow/versions/CutBookkeeperContainer_v1.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "AthLinks/ElementLink.h"

// STL includes
#include <stdexcept>
#include <sstream>



// STL include(s):
#if __cplusplus < 201100
#   include <tr1/functional>
namespace HASH_NS = std::tr1;
#else
#   include <functional>
namespace HASH_NS = std;
#endif // C++

namespace {

   unsigned int UtilsHash( const std::string& name ) {
      // The helper object:
      static const HASH_NS::hash< std::string > helper;
      // Let the helper do the work:
      return helper( name );
   }

} // private namespace

namespace xAOD {

  //typedef ElementLink< CutBookkeeperContainer_v1>  CutBookkeeperLink_t;
  typedef ElementLink< CutBookkeeperContainer >    CutBookkeeperLink_t;
  typedef std::vector< xAOD::CutBookkeeperLink_t > CutBookkeeperLinks_t;


  // ======================================================================
  // All the static accessors for the auxiliary store
  // ======================================================================

  static const SG::AuxElement::Accessor< unsigned int >               nameIdentifierAcc( "nameIdentifier" );
  static const SG::AuxElement::Accessor< unsigned int >               uniqueIdentifierAcc( "uniqueIdentifier" );
  static const SG::AuxElement::Accessor< std::string >                nameAcc( "name" );
  static const SG::AuxElement::Accessor< std::string >                descriptionAcc( "description" );
  static const SG::AuxElement::Accessor< unsigned int >               cutLogicAcc( "cutLogic" );
  static const SG::AuxElement::Accessor< int >                        isTopFilterAcc( "isTopFilter" );
  static const SG::AuxElement::Accessor< int >                        cycleAcc( "cycle" );
  static const SG::AuxElement::Accessor< std::string >                inputStreamAcc( "inputStream" );
  static const SG::AuxElement::Accessor< std::vector< std::string > > outputStreamsAcc( "outputStreams" );
  static const SG::AuxElement::Accessor< xAOD::CutBookkeeperLink_t >  parentLinkAcc( "parentLink" );
  static const SG::AuxElement::Accessor< xAOD::CutBookkeeperLinks_t > childrenLinksAcc( "childrenLinks" );
  static const SG::AuxElement::Accessor< xAOD::CutBookkeeperLinks_t > othersLinksAcc( "othersLinks" );
  static const SG::AuxElement::Accessor< xAOD::CutBookkeeperLinks_t > siblingsLinksAcc( "siblingsLinks" );
  static const SG::AuxElement::Accessor< uint64_t >                   nAcceptedEventsAcc( "nAcceptedEvents" );
  static const SG::AuxElement::Accessor< double >                     sumOfEventWeightsAcc( "sumOfEventWeights" );
  static const SG::AuxElement::Accessor< double >                     sumOfEventWeightsSquaredAcc( "sumOfEventWeightsSquared" );


  // ======================================================================
  // Constructors and Destructor
  // ======================================================================

  CutBookkeeper_v1::CutBookkeeper_v1()
  : SG::AuxElement()
    // m_cycle(-1),
    // m_parentIndex(-1),
    // m_childrenIndices()
  {
    // Create a private store all the time for this object.
    // This is OK since it only happens in initialize that we create new objects
    // of this type or when merging files.
    this->makePrivateStore();

    // Set default data members
    nameIdentifierAcc(*this)           = 0;
    uniqueIdentifierAcc(*this)         = 0;
    nameAcc(*this)                     = std::string("");
    descriptionAcc(*this)              = std::string("");
    cutLogicAcc(*this)                 = 0;
    isTopFilterAcc(*this)              = false;
    cycleAcc(*this)                    = -1;
    inputStreamAcc(*this)              = std::string("");
    outputStreamsAcc(*this)            = std::vector< std::string >();
    parentLinkAcc(*this)               = xAOD::CutBookkeeperLink_t();
    childrenLinksAcc(*this)            = xAOD::CutBookkeeperLinks_t();
    othersLinksAcc(*this)              = xAOD::CutBookkeeperLinks_t();
    siblingsLinksAcc(*this)            = xAOD::CutBookkeeperLinks_t();
    nAcceptedEventsAcc(*this)          = 0;
    sumOfEventWeightsAcc(*this)        = 0.0;
    sumOfEventWeightsSquaredAcc(*this) = 0.0;
  }



  CutBookkeeper_v1::~CutBookkeeper_v1() {
  }



  // ======================================================================
  // Get and set the whole payload object (which contains all counters) in one go
  // ======================================================================

  CutBookkeeper_v1::Payload CutBookkeeper_v1::payload() const {
    CutBookkeeper_v1::Payload payload;
    payload.nAcceptedEvents             = this->nAcceptedEvents();
    payload.sumOfEventWeights           = this->sumOfEventWeights();
    payload.sumOfEventWeightsSquared    = this->sumOfEventWeightsSquared();
    return payload;
  }


  void CutBookkeeper_v1::setPayload( const CutBookkeeper_v1::Payload& payload ) {
    this->setNAcceptedEvents( payload.nAcceptedEvents );
    this->setSumOfEventWeights( payload.sumOfEventWeights );
    this->setSumOfEventWeightsSquared( payload.sumOfEventWeightsSquared );
  }



  // ======================================================================
  // Setters for the describing properties
  // ======================================================================

  unsigned int CutBookkeeper_v1::nameIdentifier() const {
    return nameIdentifierAcc(*this);
  }

  void CutBookkeeper_v1::setNameIdentifier( unsigned int nameIdentifier ) {
    nameIdentifierAcc( *this ) = nameIdentifier;
    return;
  }



  unsigned int CutBookkeeper_v1::uniqueIdentifier() const {
    return uniqueIdentifierAcc(*this);
  }

  void CutBookkeeper_v1::setUniqueIdentifier( unsigned int uniqueIdentifier ) {
    uniqueIdentifierAcc( *this ) = uniqueIdentifier;
    return;
  }




  const std::string& CutBookkeeper_v1::name() const {
    return nameAcc(*this);
  }


  void CutBookkeeper_v1::setName(const std::string& name) {
    nameAcc( *this ) = name;
    this->setNameIdentifier( ::UtilsHash( name ) );
    std::ostringstream str;
    str << name << inputStream() << cycle();
    this->setUniqueIdentifier( ::UtilsHash( str.str() ) );
    return;
  }



  const std::string& CutBookkeeper_v1::description() const {
    return descriptionAcc(*this);
  }


  void CutBookkeeper_v1::setDescription(const std::string& description) {
    descriptionAcc( *this ) = description;
    return;
  }



  bool CutBookkeeper_v1::isTopFilter() const {
    return static_cast<bool>(isTopFilterAcc(*this));
  }

  void CutBookkeeper_v1::setTopFilter( bool val ) {
    isTopFilterAcc( *this ) = static_cast<int>(val);
    return;
  }



  CutBookkeeper_v1::CutLogic CutBookkeeper_v1::cutLogic() const {
    return static_cast<CutBookkeeper_v1::CutLogic>(cutLogicAcc(*this));
  }

  void CutBookkeeper_v1::setCutLogic( CutBookkeeper_v1::CutLogic val ) {
    cutLogicAcc( *this ) = static_cast<unsigned int>(val);
  }



  int CutBookkeeper_v1::cycle() const {
    return cycleAcc(*this);
  }

  void CutBookkeeper_v1::setCycle(int cycle) {
    static const SG::AuxElement::Accessor< int > cycleAcc( "cycle" );
    cycleAcc(*this) = cycle;
    std::ostringstream str;
    str << name() << inputStream() << cycle;
    this->setUniqueIdentifier( ::UtilsHash( str.str() ) );
    return;
  }



  const std::string& CutBookkeeper_v1::inputStream() const {
    return inputStreamAcc(*this);
  }


  void CutBookkeeper_v1::setInputStream(const std::string& inputstream) {
    inputStreamAcc(*this) = inputstream;
    std::ostringstream str;
    str << name() << inputstream << cycle();
    this->setUniqueIdentifier( ::UtilsHash( str.str() ) );
    return;
  }



  const std::vector<std::string>& CutBookkeeper_v1::outputStreams() const {
    return outputStreamsAcc(*this);
  }


  bool CutBookkeeper_v1::hasOutputStream( const std::string& outputstream ) const {
    for ( std::size_t i=0; i<outputStreamsAcc(*this).size(); ++i ) {
      if ( outputstream == outputStreamsAcc(*this).at(i) ) {
        return true;
      }
    }
    return false;
  }


  void CutBookkeeper_v1::addOutputStream( const std::string& outputstream ) {
    for ( std::size_t i=0; i<outputStreamsAcc(*this).size(); ++i ) {
      if ( outputstream == outputStreamsAcc(*this).at(i) ) {
        return;
      }
    }
    outputStreamsAcc(*this).push_back(outputstream);
  }


  void CutBookkeeper_v1::addOutputStreamForAllUsed  ATLAS_NOT_THREAD_SAFE (const std::string& outputstream ) {
    // First, add the output stream name for this one
    this->addOutputStream(outputstream);

    // Get the container where this CutBookkeeper lives in.
    // Throw an exception if we cannot find it
    CutBookkeeperContainer_v1* ebCont = dynamic_cast<xAOD::CutBookkeeperContainer_v1*>(this->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for this CutBookkeeper" );
    }

    // Then, find all other CutBookkeepers that were used by this one
    // and also add the output stream name to those (recursively)
    for ( std::size_t i=0; i<this->nUsedOthers(); ++i ) {
      CutBookkeeper_v1* otherEBK = const_cast<CutBookkeeper_v1*>( this->usedOther(i) );
      otherEBK->addOutputStreamForAllUsed(outputstream);
    }
  }


  void CutBookkeeper_v1::setOutputStreams(const std::vector<std::string>& outputstreams) {
    outputStreamsAcc(*this) = outputstreams;
    return;
  }



  // ======================================================================
  // The counters
  // ======================================================================

  uint64_t CutBookkeeper_v1::nAcceptedEvents() const {
    return nAcceptedEventsAcc(*this);
  }

  void CutBookkeeper_v1::setNAcceptedEvents( uint64_t val ) {
    nAcceptedEventsAcc( *this ) = val;
    return;
  }



  void CutBookkeeper_v1::addNAcceptedEvents(uint64_t nEvents) {
    nAcceptedEventsAcc(*this) += nEvents;
    return;
  }



  double CutBookkeeper_v1::sumOfEventWeights() const {
    return sumOfEventWeightsAcc(*this);
  }

  void CutBookkeeper_v1::setSumOfEventWeights( double val ) {
    sumOfEventWeightsAcc( *this ) = val;
    return;
  }



  void CutBookkeeper_v1::addSumOfEventWeights(double sumOfEventWeights) {
    sumOfEventWeightsAcc(*this) += sumOfEventWeights;
    return;
  }



  double CutBookkeeper_v1::sumOfEventWeightsSquared() const {
    return sumOfEventWeightsSquaredAcc(*this);
  }

  void CutBookkeeper_v1::setSumOfEventWeightsSquared( double val ) {
    sumOfEventWeightsSquaredAcc( *this ) = val;
    return;
  }



  void CutBookkeeper_v1::addSumOfEventWeightsSquared(double sumOfEventWeightsSquared) {
    sumOfEventWeightsSquaredAcc(*this) += sumOfEventWeightsSquared;
    return;
  }



  // ======================================================================
  // Equality testing
  // ======================================================================
  bool CutBookkeeper_v1::isEqualTo( const CutBookkeeper_v1 *eb ) const {
    //This function is mostly a placeholder in case we decide to complexify later.
    return ( this->uniqueIdentifier() == eb->uniqueIdentifier() );
  }



  // ======================================================================
  // Parent/children handling
  // ======================================================================

  bool CutBookkeeper_v1::hasParent() const {
    return ( parentLinkAcc(*this).isValid() ? true : false );
  }


  const xAOD::CutBookkeeper_v1* CutBookkeeper_v1::parent() const {
    if ( parentLinkAcc(*this).isValid() ) {
      return *(parentLinkAcc(*this));
    }
    return 0;
  }


  void CutBookkeeper_v1::setParent( const CutBookkeeper_v1* parentEB ) {
    // Get the index of the provided CutBookkeeper in its container
    std::size_t ebIdx = parentEB->index();
    const xAOD::CutBookkeeperContainer_v1* ebCont =
      dynamic_cast<const xAOD::CutBookkeeperContainer_v1*>(parentEB->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the parent CutBookkeeper" );
    }
    parentLinkAcc(*this) = xAOD::CutBookkeeperLink_t( *ebCont, ebIdx );
    return;
  }




  std::size_t CutBookkeeper_v1::nChildren() const {
    return childrenLinksAcc(*this).size();
  }


  bool CutBookkeeper_v1::hasChild( const xAOD::CutBookkeeper_v1* testCBK ) const {
    // Search for the child in the existing chidren
    for ( std::size_t i=0; i<childrenLinksAcc(*this).size(); ++i ) {
      const xAOD::CutBookkeeperLink_t& tmpLink = childrenLinksAcc(*this).at(i);
      if ( !(tmpLink.isValid()) ) {
        throw std::runtime_error( "ERROR: Got an invalid child CutBookkeeper" );
      }
      const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
      if ( tmpBK->isEqualTo(testCBK) ) {
        return true;
      }
    }
    return false;
  }


  const xAOD::CutBookkeeper_v1* CutBookkeeper_v1::child( std::size_t i ) const {
    if ( i >= childrenLinksAcc(*this).size() ) {
      throw std::runtime_error( "ERROR: Got an index out of the range of childred CutBookkeepers" );
    }
    const xAOD::CutBookkeeperLink_t& tmpLink = childrenLinksAcc(*this).at(i);
    if ( !(tmpLink.isValid()) ) {
      throw std::runtime_error( "ERROR: Got an invalid child CutBookkeeper" );
    }
    const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
    return tmpBK;
  }


  void
  CutBookkeeper_v1::setChildren( const std::vector< CutBookkeeper_v1* >& childrenEB ) {
    childrenLinksAcc(*this).clear();
    return this->addChildren(childrenEB);
  }


  void
  CutBookkeeper_v1::addChildren( const std::vector< CutBookkeeper_v1* >& childrenEB ) {
    // Reserve the space for the children vector
    childrenLinksAcc(*this).reserve( childrenLinksAcc(*this).size() + childrenEB.size() );

    // Add all children
    for ( std::size_t i=0; i<childrenEB.size(); ++i ) {
      this->addChild( childrenEB.at(i) );
    }
    return;
  }


  void
  CutBookkeeper_v1::addChild( CutBookkeeper_v1* childEB ) {
    // See if the provided CutBookkeeper is already a child.
    // If so, just do nothing
    if ( this->hasChild(childEB) ) {
      return;
    }

    //synchronize contextual info with the parent
    childEB->setInputStream( this->inputStream() );
    childEB->setCycle( this->cycle() );
    childEB->setOutputStreams( this->outputStreams() );
    childEB->setParent( this );

    // Create the needed ElementLink
    std::size_t ebIdx = childEB->index();
    const xAOD::CutBookkeeperContainer_v1* ebCont =
      dynamic_cast<const xAOD::CutBookkeeperContainer_v1*>(childEB->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the child CutBookkeeper" );
    }

    // Finally, add the provided CutBookkeeper as a child
    childrenLinksAcc(*this).push_back( xAOD::CutBookkeeperLink_t(*ebCont, ebIdx) );

    return;
  }


  CutBookkeeper_v1*
  CutBookkeeper_v1::addNewChild( const std::string& name,
                                   const std::string& description) {
    // Get the container where this CutBookkeeper lives in.
    // Throw an exception if we cannot find it
    CutBookkeeperContainer_v1* ebCont = dynamic_cast<xAOD::CutBookkeeperContainer_v1*>(this->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for this CutBookkeeper" );
    }

    // Create a new CutBookkeeper and add it to the same container as this one
    CutBookkeeper_v1* eb = new CutBookkeeper_v1();
    ebCont->push_back(eb);

    // Set the properties of the new CutBookkeeper
    eb->setName(name);
    eb->setDescription(description);

    addChild(eb);
    return eb;
  }





  // ======================================================================
  // Handling of the other CutBookkeepers used by this one
  // ======================================================================

  std::size_t CutBookkeeper_v1::nUsedOthers() const {
    return othersLinksAcc(*this).size();
  }


  bool CutBookkeeper_v1::hasUsedOther( const xAOD::CutBookkeeper_v1* testCBK ) const {
    // Search for the usedOther in the existing usedOthers
    for ( std::size_t i=0; i<othersLinksAcc(*this).size(); ++i ) {
      const xAOD::CutBookkeeperLink_t& tmpLink = othersLinksAcc(*this).at(i);
      if ( !(tmpLink.isValid()) ) {
        throw std::runtime_error( "ERROR: Got an invalid usedOther CutBookkeeper" );
      }
      const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
      if ( tmpBK->isEqualTo(testCBK) ) {
        return true;
      }
    }
    return false;
  }


  const xAOD::CutBookkeeper_v1* CutBookkeeper_v1::usedOther( std::size_t i ) const {
    if ( i >= othersLinksAcc(*this).size() ) {
      throw std::runtime_error( "ERROR: Got an index out of the range of usedOthers CutBookkeepers" );
    }
    const xAOD::CutBookkeeperLink_t& tmpLink = othersLinksAcc(*this).at(i);
    if ( !(tmpLink.isValid()) ) {
      throw std::runtime_error( "ERROR: Got an invalid usedOther CutBookkeeper" );
    }
    const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
    return tmpBK;
  }


  void
  CutBookkeeper_v1::setUsedOthers( const std::vector< CutBookkeeper_v1* >& usedOthers ) {
    othersLinksAcc(*this).clear();
    return this->addUsedOthers(usedOthers);
  }


  void
  CutBookkeeper_v1::addUsedOthers( const std::vector< CutBookkeeper_v1* >& usedOthers ) {
    // Reserve the space for the used-other vector
    othersLinksAcc(*this).reserve( othersLinksAcc(*this).size() + usedOthers.size() );

    // Add all used ones
    for ( std::size_t i=0; i<usedOthers.size(); ++i ) {
      this->addUsedOther( usedOthers.at(i) );
    }
    return;
  }


  void
  CutBookkeeper_v1::addUsedOther( CutBookkeeper_v1* usedOtherEB ) {
    if ( this->hasUsedOther(usedOtherEB) ) {
      return;
    }

    // I don't think I need this as the others should have already been properly registered
    // //synchronize contextual info with the used-other
    // usedOtherEB->setInputStream( m_inputstream );
    // usedOtherEB->setCycle( m_cycle );
    // usedOtherEB->setOutputStream( m_outputstream );

    // Create the needed ElementLink
    std::size_t ebIdx = usedOtherEB->index();
    const xAOD::CutBookkeeperContainer_v1* ebCont =
      dynamic_cast<const xAOD::CutBookkeeperContainer_v1*>(usedOtherEB->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the usedOthers CutBookkeeper" );
    }

    // Finally, add the provided CutBookkeeper as a usedOther
    othersLinksAcc(*this).push_back( xAOD::CutBookkeeperLink_t(*ebCont, ebIdx) );

    return;
  }




  // ======================================================================
  // Handling of the sibling CutBookkeepers
  // ======================================================================

  std::size_t CutBookkeeper_v1::nSiblings() const {
    return siblingsLinksAcc(*this).size();
  }


  bool CutBookkeeper_v1::hasSibling( const xAOD::CutBookkeeper_v1* testCBK ) const {
    // Search for the sibling in the existing sibings
    for ( std::size_t i=0; i<siblingsLinksAcc(*this).size(); ++i ) {
      const xAOD::CutBookkeeperLink_t& tmpLink = siblingsLinksAcc(*this).at(i);
      if ( !(tmpLink.isValid()) ) {
        throw std::runtime_error( "ERROR: Got an invalid sibling CutBookkeeper" );
      }
      const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
      if ( tmpBK->isEqualTo(testCBK) ) {
        return true;
      }
    }
    return false;
  }


  const xAOD::CutBookkeeper_v1* CutBookkeeper_v1::sibling( std::size_t i ) const {
    if ( i >= siblingsLinksAcc(*this).size() ) {
      throw std::runtime_error( "ERROR: Got an index out of the range of sibling CutBookkeepers" );
    }
    const xAOD::CutBookkeeperLink_t& tmpLink = siblingsLinksAcc(*this).at(i);
    if ( !(tmpLink.isValid()) ) {
      throw std::runtime_error( "ERROR: Got an invalid sibling CutBookkeeper" );
    }
    const xAOD::CutBookkeeper_v1* tmpBK = *tmpLink;
    return tmpBK;
  }


  void
  CutBookkeeper_v1::setSiblings( const std::vector< CutBookkeeper_v1* >& siblings ) {
    siblingsLinksAcc(*this).clear();
    return this->addSiblings(siblings);
  }


  void
  CutBookkeeper_v1::addSiblings( const std::vector< CutBookkeeper_v1* >& siblings ) {
    // Reserve the space for the siblings vector
    siblingsLinksAcc(*this).reserve( siblingsLinksAcc(*this).size() + siblings.size() );

    // Add all siblings
    for ( std::size_t i=0; i<siblings.size(); ++i ) {
      this->addSibling( siblings.at(i) );
    }
    return;
  }


  void
  CutBookkeeper_v1::addSibling( CutBookkeeper_v1* siblingEB ) {
    if ( this->hasSibling(siblingEB) ) {
      return;
    }

    // Create the needed ElementLink
    std::size_t ebIdx = siblingEB->index();
    const xAOD::CutBookkeeperContainer_v1* ebCont =
      dynamic_cast<const xAOD::CutBookkeeperContainer_v1*>(siblingEB->container());
    if ( !ebCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the sibling CutBookkeeper" );
    }

    // Finally, add the provided CutBookkeeper as a sibling
    siblingsLinksAcc(*this).push_back( xAOD::CutBookkeeperLink_t(*ebCont, ebIdx) );

    //synchronize contextual info with the sibling
    siblingEB->addSibling( this );

    return;
  }



  // ======================================================================
  // Function preparing the object to be persistified
  // ======================================================================

  void CutBookkeeper_v1::toPersistent() {
    if( parentLinkAcc.isAvailableWritable(*this) ) {
      parentLinkAcc(*this).toPersistent();
    }
    if( childrenLinksAcc.isAvailableWritable(*this) ) {
      xAOD::CutBookkeeperLinks_t::iterator itr    = childrenLinksAcc(*this).begin();
      xAOD::CutBookkeeperLinks_t::iterator itrEnd = childrenLinksAcc(*this).end();
      for( ; itr != itrEnd; ++itr ) {
        itr->toPersistent();
      }
    }
    if( othersLinksAcc.isAvailableWritable(*this) ) {
      xAOD::CutBookkeeperLinks_t::iterator itr    = othersLinksAcc(*this).begin();
      xAOD::CutBookkeeperLinks_t::iterator itrEnd = othersLinksAcc(*this).end();
      for( ; itr != itrEnd; ++itr ) {
        itr->toPersistent();
      }
    }
    if( siblingsLinksAcc.isAvailableWritable(*this) ) {
      xAOD::CutBookkeeperLinks_t::iterator itr    = siblingsLinksAcc(*this).begin();
      xAOD::CutBookkeeperLinks_t::iterator itrEnd = siblingsLinksAcc(*this).end();
      for( ; itr != itrEnd; ++itr ) {
        itr->toPersistent();
      }
    }
    return;
  }



} // namespace xAOD
