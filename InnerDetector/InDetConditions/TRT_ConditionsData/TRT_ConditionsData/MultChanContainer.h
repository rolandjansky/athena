/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file MultChanContainer.h 
 @brief CondMultChanCollection for storing TRT conditions
 data. CondMultChanCollections store objects identified by a channel
 number, allowing a separate validity interval for each channel.  In
 the TRT we use a different channel for each layer or wheel. The
 channel identifier is defined as

    channel = 1 + becindex + 4 x layer_or_wheel-index

 The objects stored in the CondMultChanCollection are
 TRTCond::NestedContainers of level
 'TRTCond::ExpandedIdentifier::LAYERWHEEL'.

 There is a separate channel (0) for storing a DETECTOR level default
 value. This value must also be stored as a NestedContainer which is
 very inefficient, but we do not have much choice. (The container
 will be empty except for its top-level default value.)

 @author Wouter Hulsbergen <wouter.hulsbergen@cern.ch>
*/
// $Id: MultChanContainer.h,v 1.8 2009-03-13 10:14:59 dwhittin Exp $

#ifndef TRTCONDITIONSDATA_MULTCHANNELCONTAINERDEFAULT_H
#define TRTCONDITIONSDATA_MULTCHANNELCONTAINERDEFAULT_H

#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "TRT_ConditionsData/NestedContainer.h"

#include <cxxabi.h>

namespace TRTCond
{

  /** @class MultChanContainer 
       Class for accessing NestedContainers via channels in a CondMultChanCollection */
  template <class DaughterContainer>
  class MultChanContainer : public CondMultChanCollection<DaughterContainer>
  {
  public:
    /** constructor. cannot do anything because that would make CondMultChanCollection upset. */
    MultChanContainer() {}

    /** assignment operator. not trivial. */
    MultChanContainer& operator=(const MultChanContainer& rhs) ;
    
    /** calculate the channel for a given TRT identifier */
    size_t channelId( const ExpandedIdentifier& x ) const ;

    /** calculate layer or wheel index from a channel id */
    size_t layerwheelindex( size_t channelid ) const ;

    /** calculate the barrel-ec index from a channel id. (note that this returns 0-3, so not the barrel-ec-ID.) */
    size_t barrelecindex( size_t channelid ) const ;
    
    /** find a layercontainer from an identifier. create it if it does not exist */
    DaughterContainer* findContainer(const ExpandedIdentifier& id)  ;
    DaughterContainer* findContainer(size_t chanid) ;
    
    /** get a layercontainer from a channel id. uses cache for fast access. */
    const DaughterContainer* getContainer(size_t chanid) const ;
    
    /** set a value */
    void set( const ExpandedIdentifier& id, const typename DaughterContainer::value_type& t) ;
    
    /** get a value */
    const typename DaughterContainer::value_type& get( const ExpandedIdentifier& id ) const ;

    /** resolve the nesting level */
    const typename DaughterContainer::value_type& get( const ExpandedIdentifier& id , size_t& resolvelevel ) const ;
    
    /** get a value with the corresponding container. the container is needed to unpack the stored class. */
    typedef std::pair< const DaughterContainer*, const typename DaughterContainer::value_type*> ContainerWithValue ;
    ContainerWithValue getWithContainer( const ExpandedIdentifier& id ) const ;
    
    /** clear all layercontainers */
    void clear() ;
    
    /** return the memory allocated by the layercontainers. It does not add the footprint of the CondMultChanCollection itself. */
    size_t footprint() const ;
    
    /** crunch the layercontainers. */
    void crunch() ;

    /** dump to standard output */
    void print() const ;
    
    /**  total number of valid calibration objects */
    size_t numObjects() const ;
    
    /** get a flat vector with all values. used for storing constants in a txtfile. */
    void getall( typename DaughterContainer::FlatContainer& entries ) const ;
    
    /// now need various forwarding calls for CondMultChanCollection functionality
   
  private:
    /** channel identifier for the container with the 'default' value */
    static const size_t m_defaultschannelid = 0 ;
    /** cached table for fast access from channel to layercontainer */
    mutable std::vector<const DaughterContainer*> m_channelmap ;
    /** dummy value to return when DaughterContainer is empty */
    const typename DaughterContainer::value_type& dummyVal() const;
  } ;

 
  ///////// 'inline' functions /////////////

  // Dummy Value for empty DaughterContainer
  /** This is a somewhat crazy workaround to handle returning a default
   *  value for a templated class */
  template <class DaughterContainer>
  inline const typename DaughterContainer::value_type&
  MultChanContainer<DaughterContainer>::dummyVal() const
  {
    static typename DaughterContainer::value_type dummyVal;
    return dummyVal;
  }
  
  template <class DaughterContainer>
  MultChanContainer<DaughterContainer>& 
  MultChanContainer<DaughterContainer>::operator=(const MultChanContainer<DaughterContainer>& rhs) 
  {
    clear() ;
    typename CondMultChanCollection<DaughterContainer>::chan_const_iterator rhschanit = rhs.chan_begin() ;
    typename CondMultChanCollection<DaughterContainer>::const_iterator rhsit = rhs.begin() ; 
    for(  ;  rhsit !=  rhs.end(); ++rhsit, ++ rhschanit) {
      DaughterContainer* lhscontainer = findContainer( *rhschanit ) ;
      *lhscontainer = **rhsit ;
    }
    return *this ;
  }
  

  template <class DaughterContainer>
  inline size_t
  MultChanContainer<DaughterContainer>::channelId( const ExpandedIdentifier& x) const
  {
    return x.level() >=ExpandedIdentifier::LAYERWHEEL ?
      1 + x.index(ExpandedIdentifier::BARRELEC) + 4 * x.index(ExpandedIdentifier::LAYERWHEEL) :
      m_defaultschannelid ;
  }


  template <class DaughterContainer>
  inline size_t
  MultChanContainer<DaughterContainer>::layerwheelindex( size_t channelid ) const 
  {
    return channelid!=m_defaultschannelid ? (channelid-1)/4 : 99 ;
  }


  template <class DaughterContainer>
  inline size_t
  MultChanContainer<DaughterContainer>::barrelecindex( size_t channelid ) const 
  {
    return channelid!=m_defaultschannelid ? (channelid-1)%4 : 99 ;
  }

  
  template <class DaughterContainer>
  inline DaughterContainer* MultChanContainer<DaughterContainer>::findContainer(const ExpandedIdentifier& id)  
  {
    // on the first call, we insert a container for the default. the default is always the first container.
    if(  id.level()==ExpandedIdentifier::BARRELEC ) {
      std::cout << "Sorry: TRTCond::MultiChannelContainer cannot store containers at BARREL_EC granularity" << std::endl ;
    }
    return findContainer( channelId( id ) ) ;
  }


  template <class DaughterContainer>
  inline DaughterContainer* MultChanContainer<DaughterContainer>::findContainer(size_t chanid)
  {
    // on the first call, we insert a container for the default. the default is always the first container.
    if(this->size()==0) {
      this->push_back( new DaughterContainer() ) ;
      this->add(m_defaultschannelid);
    }
    typename CondMultChanCollection<DaughterContainer>::chan_const_iterator chanit = 
      std::find( this->chan_begin(), this->chan_end(), chanid ) ;
    size_t chanindex = std::distance(this->chan_begin(),chanit) ;
    if(chanit==this->chan_end()) {
      // add a new container
      this->push_back( new DaughterContainer() ) ;
      this->add(chanid);
      chanindex = this->size()-1 ; //std::distance(chan_begin(),chan_end())-1 ;
      // make sure to clear the cache
      m_channelmap.clear() ;
    }
    return this->operator[]( chanindex ) ;
  }


  template <class DaughterContainer>
  inline const DaughterContainer* MultChanContainer<DaughterContainer>::getContainer(size_t chanid) const
  {
    if( m_channelmap.empty() ) {
      // update the cache

      // sanity check to test we have added a default value
      if( this->size()>0 && *(this->chan_begin()) != m_defaultschannelid )
	std::cout << "TRTCal::MultChanContainer ERROR: first channel id is not defaults channel id!" << std::endl ;
      
      // first get the max channel id
      size_t maxchanid(0) ;
      for( typename CondMultChanCollection<DaughterContainer>::chan_const_iterator chanit = this->chan_begin() ;
	   chanit != this->chan_end(); ++chanit ) if( maxchanid < *chanit ) maxchanid=*chanit ;
      
      // now allocate the map and fill it
      m_channelmap.clear() ;
      m_channelmap.resize( maxchanid + 1, 0 ) ;
      typename CondMultChanCollection<DaughterContainer>::const_iterator dauit = this->begin() ; 
      for( typename CondMultChanCollection<DaughterContainer>::chan_const_iterator chanit = this->chan_begin() ;
	   chanit != this->chan_end(); ++chanit, ++dauit ) m_channelmap[*chanit] = *dauit ;
    }
    return chanid < m_channelmap.size() ? m_channelmap[chanid] : 0 ;
  }


  template <class DaughterContainer>
  inline void MultChanContainer<DaughterContainer>::set( const ExpandedIdentifier& id, const typename DaughterContainer::value_type& t ) 
  {
    if( id.level()==ExpandedIdentifier::DETECTOR ) 
      // set the default value. don't use the id, since it will only confuse the container
      findContainer(id)->set( t ) ; 
    else if( id.level()==ExpandedIdentifier::BARRELEC ) 
      std::cout << "Sorry: TRTCond::MultiChannelContainer cannot store containers at BARREL_EC granularity" << std::endl ;
    else 
      findContainer(id)->set( id, t ) ;
  }
  

  template <class DaughterContainer>
  inline const typename DaughterContainer::value_type& 
  MultChanContainer<DaughterContainer>::get( const ExpandedIdentifier& id ) const 
  {
    if( id.level() >= ExpandedIdentifier::LAYERWHEEL ) {
      const DaughterContainer* container = getContainer( channelId(id) ) ;
      if( container ) {
	const typename DaughterContainer::value_type& rc = container->get( id ) ;
	if( DaughterContainer::trait_type::isvalid(rc) ) return rc ;
      }
    }
    // Throw error if the container is empty.
    if ( this->size() == 0 ) {
      size_t len;
      int s;
      char* tmp = abi::__cxa_demangle(typeid(this).name(),0,&len,&s);
      std::string error = std::string(tmp) + std::string(" is empty!!");
      throw std::runtime_error( error );
      return dummyVal();
    }
    return this->front()->get() ;
    //return operator[]( 0 )->set( t ) ;// m_default ;
  }

  template <class DaughterContainer>
  inline const typename DaughterContainer::value_type& 
  MultChanContainer<DaughterContainer>::get( const ExpandedIdentifier& id , size_t& resolvelevel) const 
  {
    if( id.level() >= ExpandedIdentifier::LAYERWHEEL ) {
      const DaughterContainer* container = getContainer( channelId(id) ) ;
      if( container ) {
	const typename DaughterContainer::value_type& rc = container->get( id , resolvelevel) ;
	if( DaughterContainer::trait_type::isvalid(rc) ) return rc ;
      }
    }
    // Throw error if the container is empty.
    if ( this->size() == 0 ) {
      size_t len;
      int s;
      char* tmp = abi::__cxa_demangle(typeid(this).name(),0,&len,&s);
      std::string error = std::string(tmp) + std::string(" is empty!!");
      throw std::runtime_error( error );
      return dummyVal();
    }
      return  this->front()->get(id,resolvelevel) ;
  }

  /// for retrieving t0 values, we need also the container to 'unpack' the t0
  template <class DaughterContainer>
  inline typename MultChanContainer<DaughterContainer>::ContainerWithValue
  MultChanContainer<DaughterContainer>::getWithContainer( const ExpandedIdentifier& id) const 
  {
    if( id.level() >= ExpandedIdentifier::LAYERWHEEL ) {
      const DaughterContainer* container = getContainer( channelId(id) ) ;
      if( container ) {
	const typename DaughterContainer::value_type& value = container->get( id ) ;
	if( DaughterContainer::trait_type::isvalid(value) ) return ContainerWithValue(container,&value) ;
      }
    }
    // Throw error if the container is empty.
    if ( this->size() == 0 ) {
      size_t len;
      int s;
      char* tmp = abi::__cxa_demangle(typeid(this).name(),0,&len,&s);
      std::string error = std::string(tmp) + std::string(" is empty!!");
      throw std::runtime_error( error );
      return ContainerWithValue(this->front(),&dummyVal());
    }
    return ContainerWithValue(this->front(),&(this->front()->get())) ;
  }
 

  template <class DaughterContainer>
  inline void MultChanContainer<DaughterContainer>::clear() 
  {
    for( typename CondMultChanCollection<DaughterContainer>::iterator it = this->begin() ;
	 it != this->end(); ++it) (*it)->clear() ;
  }

  
  template <class DaughterContainer>
  inline size_t MultChanContainer<DaughterContainer>::footprint() const {
    size_t total=0; //= Trait::footprint(m_default) ;
    for( typename CondMultChanCollection<DaughterContainer>::const_iterator it = this->begin() ; 
	 it != this->end(); ++it) total += (*it)->footprint() ;
    return total ;
  }

  
  template <class DaughterContainer>
  inline void MultChanContainer<DaughterContainer>::crunch() {
    for( typename CondMultChanCollection<DaughterContainer>::iterator it = this->begin() ; 
	 it != this->end(); ++it) (*it)->crunch() ;
  }


  template <class DaughterContainer>
  size_t MultChanContainer<DaughterContainer>::numObjects() const {
    size_t rc(0) ;
    for( typename CondMultChanCollection<DaughterContainer>::const_iterator it = this->begin() ; 
	 it != this->end(); ++it) rc += (*it)->numObjects() ;
    return rc ;
  }

  
  template <class DaughterContainer>
  inline void MultChanContainer<DaughterContainer>::getall( typename DaughterContainer::FlatContainer& entries ) const { 
    typename CondMultChanCollection<DaughterContainer>::chan_const_iterator chanit = this->chan_begin() ;
    typename CondMultChanCollection<DaughterContainer>::const_iterator dauit = this->begin() ; 
    for( ; dauit != this->end(); ++dauit, ++chanit) {
      // get the entries in this subcontainer
      typename DaughterContainer::FlatContainer newentries ;
      (*dauit)->getall(newentries) ;
      // update the identifiers
      if( dauit == this->begin() ) {
	// deal with the default value
	for( typename DaughterContainer::FlatContainer::iterator it = newentries.begin() ; it!= newentries.end(); ++it)
	  it->first= ExpandedIdentifier(0,0,0,0,0,ExpandedIdentifier::DETECTOR) ;
      } else {
	for( typename DaughterContainer::FlatContainer::iterator it = newentries.begin() ; it!= newentries.end(); ++it) {
	  it->first.index(ExpandedIdentifier::BARRELEC) = barrelecindex( *chanit) ;
	  it->first.index(ExpandedIdentifier::LAYERWHEEL) = layerwheelindex( *chanit) ;
	}
      }
      entries.insert( entries.end(), newentries.begin(), newentries.end() ) ;
    }
  }
  

  template <class DaughterContainer>
  inline void MultChanContainer<DaughterContainer>::print() const {
    typename CondMultChanCollection<DaughterContainer>::chan_const_iterator chanit = this->chan_begin() ;
    typename CondMultChanCollection<DaughterContainer>::const_iterator dauit = this->begin() ; 
    for( ; dauit != this->end(); ++dauit, ++chanit) {
      std::cout << "Now printing channel " << *chanit << " layer/becindex: " << layerwheelindex(*chanit) << " " << barrelecindex(*chanit) << std::endl ;
      (*dauit)->print() ;
    }
  }
  



}

#endif
