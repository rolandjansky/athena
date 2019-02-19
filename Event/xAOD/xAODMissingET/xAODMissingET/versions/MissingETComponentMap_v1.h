// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETCOMPONENTMAP_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETCOMPONENTMAP_V1_H

#include "AthContainers/DataVector.h"

#include "xAODBase/ObjectType.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/versions/MissingETComponent_v1.h"

#include <string>
#include <vector>
#include <map>

namespace xAOD
{
  class MissingETComponentMap_v1 : public DataVector<MissingETComponent_v1>
  {
  public:

    /*! @brief Default constructor */
    MissingETComponentMap_v1(SG::OwnershipPolicy own=SG::OWN_ELEMENTS, SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    /*! @brief Constructor from iterators */
    MissingETComponentMap_v1(MissingETComponentMap_v1::iterator first, MissingETComponentMap_v1::iterator last,
			     SG::OwnershipPolicy own=SG::OWN_ELEMENTS, SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    /*! @brief Base class destructor */
    virtual ~MissingETComponentMap_v1();

    /*! @name Find methods */
    /*!@{*/
    /*! @brief Find MET object referenced by pointer
     *
     *  @return Const iterator referencing a non-modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end const iterator is returned.
     *
     *  @param[in] pMET pointer to the xAOD::MissingET object searched for in the composition map.   
     */
    const_iterator find(const MissingET* pMET)   const;
    /*! @brief Find MET object referenced by name
     *
     *  @return Const iterator referencing a non-modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end const iterator is returned.
     *
     *  @param[in] name reference to non-modifiable string storing the name of the xAOD::MissingET object searched for.  
     *
     *  @note It is generally expected that the name of the xAOD::MissingET object is unique for this composition. If not, the iterator referencing the first  
     *        xAOD::MissingETComponent_v1 object containing a link to a xAOD::MissingET object with the requested name is returned.   
     */
    const_iterator find(const std::string& name) const;
    /*! @brief Find MET object referenced by source indicator
     *
     *  @return Const iterator referencing a non-modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end const iterator is returned.
     *
     *  @param[in] src bit pattern with the requested source indicator.
     *
     *  @note It is generally expected that the source of the xAOD::MissingET object is unique for this composition. If not, the iterator referencing the first  
     *        xAOD::MissingETComponent_v1 object containing a link to a xAOD::MissingET object with the requested source is returned.   
     */
    const_iterator find(MissingETBase::Types::bitmask_t src)           const;
    /*! @brief Find MET object referenced by pointer
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end iterator is returned.
     *
     *  @param[in] pMET pointer to the xAOD::MissingET object searched for in the composition map.   
     */
    iterator find(const MissingET* pMET);
    /*! @brief Find MET object referenced by name
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end iterator is returned.
     *
     *  @param[in] name reference to non-modifiable string storing the name of the xAOD::MissingET object searched for.  
     *
     *  @note It is generally expected that the name of the xAOD::MissingET object is unique for this composition. If not, the iterator referencing the first  
     *        xAOD::MissingETComponent_v1 object containing a link to a xAOD::MissingET object with the requested name is returned.   
     */
    iterator find(const std::string& name);
    /*! @brief Find MET object referenced by source indicator
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETComponent_v1 object linking to the given MET object. If the MET object is not in the ccomposition map,
     *          an end iterator is returned.
     *
     *  @param[in] src bit pattern with the requested source indicator.
     *
     *  @note It is generally expected that the source of the xAOD::MissingET object is unique for this composition. If not, the iterator referencing the first  
     *        xAOD::MissingETComponent_v1 object containing a link to a xAOD::MissingET object with the requested source is returned.   
     */
    iterator find(MissingETBase::Types::bitmask_t src);
    size_t findIndex(const MissingET* pMET) const;
    size_t findIndex(const std::string& name) const;
    size_t findIndex(MissingETBase::Types::bitmask_t src) const;
    /*! @brief Retrieve MissingET object by name
     *
     *  @return Valid pointer to MissingET typed object representing a MET term. If the requested object cannot be found,
     *          NULL is returned. 
     *
     *  @param[in] name reference to non-modifiable dataword holding the name of the requested MET object. 
     */
    const MissingET* retrieveMissingET(const std::string& name) const;
    /*! @brief Retrieve MissingET object by source indicator
     *
     *  @return Valid pointer to MissingET typed object representing a MET term. If the requested object cannot be found,
     *          NULL is returned. 
     *
     *  @param[in] src   bit pattern defining the source of the requested MET object.
     *  @param[in] excl  flag controlling the bit pattern test - if @c false, the pointer to the first MET object found with a source indicator 
     *                   containing the requested bit pattern is returned; if @c true, the first MET object found with exactly the requested 
     *                   bit pattern as a source indicator is returned. 
     */
    const MissingET* retrieveMissingET(MissingETBase::Types::bitmask_t src,bool excl=false) const;
    /*! @brief Retrieve MissingET object by source indicator and status word
     *
     *  @copydetails retrieveMissingET(MissingETBase::Types::bitmask_t)
     *
     *  @param[in] sw    bit pattern defining the status of the MET contribution linking to the requested MET object.
     *
     *  @note While the MissingET object name should be unique within a given MET composition, MET objects can have the same source with a different
     *        contribution status. This method supports a query for a MET object with a given source/status combination.
     */
    const MissingET* retrieveMissingET(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw,bool excl=false) const;
    /*!@}*/

  public:

    /*! @name Client interactions with local cache */
    /*!@{*/
    /*! @brief Check if a given object is already used in MET
     *
     *  @return @c true if the referenced signal object is already used.
     * 
     *  @param[in] pPart generic (base class) pointer to signal object (xAOD::CaloCluster or xAOD::TrackParticle).
     *  @param[in] p     tags signal type to be checked to usage (optional, default is MissingETBase::UsageHandler::OnlyCluster).   
     */
    bool checkUsage(const IParticle* pPart,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const;
    /*! @brief Check a list of objects for prior usage in MET reconstruction
     *
     *  @return @c true if any of the objects in the list has already been used in another MET contribution. The iput object list contains 
     *          pointer references to objects which have not yet been used.
     *
     *  @param[inout] sig reference to modifiable list of objects.
     *  @param[in]    p   tags signal type to be checked to usage (optional, default is MissingETBase::UsageHandler::OnlyCluster).   
     *
     *  @note The list of signals on input can contain various signal (object) types. The modified list contains only references to objects 
     *        with types determined by the MissingETBase::UsageHandler::Policy. By default, those are clusters. 
     *  @note Usage is always measured with respect to the type given by MissingETBase::UsageHandler::Policy. For example, if the input signal
     *        list has mixed object types, its content on return contains only references to objects of the type determined by the MissingETBase::UsageHandler::Policy. 
     *        By default, those are clusters. This modification of the input list does not indicate that any clusters in this list have already been used. This
     *        is done by the return value.         
     */
    bool checkUsage(MissingETBase::Types::object_vector_t& sig,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const; 
    //
    bool addObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,
		    MissingETBase::UsageHandler::Policy p = MissingETBase::UsageHandler::OnlyCluster);
    bool setClusters(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList);
    bool setTracks(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t&  signalList);
    bool setObjects(const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& objectList);
    size_t getComponentIndex(const IParticle* pPart) const;
    size_t getObjectIndex(const IParticle* pPart) const;
    /*!@}*/

    /* override base DataVector methods that can remove elements
     * in order to invalidate the cache
     */
    void resize(size_type sz);
    void pop_back();
    void clear();
    void sort();
    template <class COMPARE> void sort(COMPARE comp);
    void clear (SG::OwnershipPolicy ownPolicy);
    void clear (SG::OwnershipPolicy ownPolicy,SG::IndexTrackingPolicy trackIndices);
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);

  protected:

    /*! @name Internal types for local cache handling */
    /*!@{*/
    typedef size_t                              signal_key_t;     /*!< @brief Key type for signals (clusters, tracks) is type of index in container */
    typedef const IParticle*                    particle_key_t;   /*!< @brief Key type for physics objects is the base pointer type of these objects */
    typedef MissingETBase::Types::indexedlink_t indexedlink_t;      /*!< @brief Type for the description of the link locations
								   *
								   *   This is a two-component type which allows holding the index of the xAOD::MissingETComponent_v1 object a physics or
								   *   signal object is linked from, and the index of this linked object in the contributing object list.
								   */
    typedef std::vector<indexedlink_t>             signal_vector_t; /*!< @brief Indexed list type for signal object link locations
								   *
								   *   In case of a signal (xAOD::CaloCluster, xAOD::TrackParticle) object look-up, the index of these objects can be 
								   *   directly used to check if they have already been used in the actual MET composition (random access in this index
								   *   parallel store - with respect to the actual xAOD::CaloClusterContainer and the xAOD::TrackParticleContainer).   
								   */
    typedef std::map<particle_key_t,indexedlink_t> particle_map_t;  /*!< @brief Map type for physics object
								   *
								   *   In case of physics objects, a map look-up is implemented as the lack of a common container for these objects
								   *   does not allow using the index as a unique key. The object pointer is used instead, and the look-up is provided
								   *   by a standard map (binary search). Typically, the number of entries in this map is rather small (much less than 
								   *   100) so that this storage technology should not introduce performance problems.  
								   */
    /*!@}*/

    /*! @name Internal data for local cache */
    /*!@{*/
    signal_vector_t m_clusterLinks;  /*!< @brief Store for linked cluuster signal information lookup
                                      *   @copydetails signal_vector_t
                                      */
    signal_vector_t m_trackLinks;    /*!< @brief Store for linked track signal information lookup
                                      *   @copydetails signal_vector_t
                                      */
    particle_map_t  m_particleLinks; /*!< @brief Store for linked physics object information lookup
                                      *   @copydetails particle_map_t
                                      */
    /*!@}*/

    /*! @name Internal functions for local cache handling  */
    /*!@{*/
    template<int N> size_t f_getIndex(const IParticle* pPart) const
    {
      // check on cluster signal
      if ( pPart->type() == Type::CaloCluster ) 
	{ size_t idx(pPart->index()); return idx < m_clusterLinks.size() ? m_clusterLinks.at(idx).get<N>() : MissingETBase::Numerical::invalidIndex(); }
      // check on track signal
      if ( pPart->type() == Type::TrackParticle )
	{ size_t idx(pPart->index()); return idx < m_trackLinks.size() ? m_trackLinks.at(idx).get<N>() : MissingETBase::Numerical::invalidIndex(); }
      // 
      particle_map_t::const_iterator fPart(m_particleLinks.find(pPart));
      return fPart != m_particleLinks.end() ? (fPart->second).get<N>() : MissingETBase::Numerical::invalidIndex();
    }
    /*!@}*/

    /*! @name Internal object lookup cache handlers */
    /*!@{*/
    /*! @brief Check signal object usage
     *
     *  @return @c true if any of the signal objects in the inout list is already marked as used. In addition, all used objects are removed from the input (signal) object list.
     *          If none of the objects in this list is already used, @c false is returned.
     *
     *  @param[inout] sig reference to modifiable list of signal objects to be checked for usage in MET reconstruction.
     *  @param[in]    signalLinks reference to modifiable list of signals already used (means linked to a MET contribution).
     *
     *  @note This method does not add yet unused signal  objects to the signal link list, as this decision is controlled by the specific MET reconstruction tool.   
     */
    bool f_checkObjectUsage(MissingETBase::Types::object_vector_t& sig, const signal_vector_t& signalLinks) const ;
    /*! @brief Check physics object usage
     *
     *  @return @c true if any of the physics objects in the input list is already marked as used. In addition, all used objects are removed from the input (physics) object list.
     *          If none of the objects in this list is already used, @c false is returned.
     *
     *  @param[inout] objects reference to modifiable list of physics objects to be checked for usage in MET reconstruction.
     *  @param[in]    physicsLinks reference to modifiable map of link descriptors for physics objects already used (means linked to a MET contribution).
     *
     *  @note This method does not add yet unused physics objects to the signal link list, as this decision is controlled by the specific MET reconstruction tool.   
     */
    bool f_checkObjectUsage(MissingETBase::Types::object_vector_t& objects, const particle_map_t& physicsLinks) const;
    /*! @brief Fill the lookup cache from the composition map data content */
    void f_fillLookupCache();
    /*! @brief Clear the lookup cache */
    void f_clearLookupCache();
    /*! @brief Set up the signal object lookup */
    void f_setupLookupCache(signal_vector_t& signalLinks,size_t reserve);
    /*! @brief Signal type extractor
     *
     *  @return @c true if input object list contains requested signal type, else @c false. The input object list is filtered on return, meaning it only contains 
     *          objects of the requested type. A return of @c false thus indicates an empty list.   
     *
     *  @tparam SIGNAL signal type of extracted objects.
     * 
     *  @param[inout] sig reference to modifiable list of signal objects to be checked for usage in MET reconstruction.
     */
    template<int OBJTYPE>
    bool f_extractSignal(MissingETBase::Types::object_vector_t& sig) const
    {
      MissingETBase::Types::object_vector_t::iterator fSign(sig.begin());
      while ( fSign != sig.end() ) { if ( (*fSign)->type() != OBJTYPE ) { fSign = sig.erase(fSign); } else { ++fSign; } }
      return !sig.empty(); 
    }

    const MissingET* f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src)              const;
    const MissingET* f_retrieveMissingETExcl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const;
    const MissingET* f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src)              const;
    const MissingET* f_retrieveMissingETIncl(MissingETBase::Types::bitmask_t src,MissingETBase::Types::bitmask_t sw) const;

  private:

    static const size_t m_clusterLinkReserve =10000;
    static const size_t m_trackLinkReserve =10000; 

  }; // class xAOD::MissingETComponentMap_v1
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::MissingETComponentMap_v1, DataVector<xAOD::MissingETComponent_v1>);

#include "xAODMissingET/versions/MissingETComponentMap_v1.icc"
#endif
