// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_MISSINGETCOMPOSITION_H
#define XAODMISSINGET_MISSINGETCOMPOSITION_H

#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETComponent.h"
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODMissingET/MissingETAssociation.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAssociationHelper.h"
#include "xAODJet/Jet.h"

namespace xAOD 
{
  struct MissingETComposition
  {
    /*! @name Adding data to the composition map
     *  @anchor metcomp_insertion 
     */
    /*!@{*/
    /*! @brief Adding a @link xAOD::MissingET MissingET @endlink object to the map
     *
     *  This function should be called for a given @link xAOD::MissingET MissingET @endlink object before MissingETComposition::insert is invoked for the same object. Note that
     *  the @ref metcomp_insertion_rules "the rules" for adding a MET object do not allow to add the same object more than once to a given 
     *  @link xAOD::MissingETComponentMap MissingETComponentMap @endlink.
     * 
     *  @return @c true if the object is not already in the map, else @c false.
     *
     *  @param[in] pMap pointer to modifiable @link MissingETComponentMap MissingETComponentMap @endlink object (the map to which the @link MissingET MissingET @endlink 
     *             object added.
     *  @param[in] pMET pointer to non-modifiable @link MissingET MissingET @endlink object.
     *  @param[in] sw   bitmask indicating contribution status (optional, default is MissingETBase::Status::clearedStatus())
     */
    static bool add(MissingETComponentMap* pMap,const MissingET* pMET,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    /*! @brief Insert contributing signal or physics object by pointer, with optional kinematic weight object.
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pMET   pointer to non-modifiable MET object
     *  @param[in] pPart  generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] weight kinematic weights of the contribution (optional, for default see 
     *  @link MissingETComponent::Weight::Weight() MissingETComponent::Weight::Weight() @endlink constructor)
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,MissingETBase::Types::weight_t weight=MissingETBase::Types::weight_t());
    /*! @brief Insert MET object and contributing object by pointers, with list of signals and optional kinematic weight object.
     *
     *  @copydetails insert(MissingETComponentMap*,const MissingET*,const IParticle*,MissingETBase::Types::weight_t)
     *
     *  @param[in] signalList reference to non-modifiable list of (base type) pointers to signals associated with the given physics or signal object
     *  @param[in] p          signal type indicator (optional, default is MissingETBase:UsageHandler::OnlyCluster)
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,
		       MissingETBase::Types::weight_t weight=MissingETBase::Types::weight_t(),
		       MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster);
    /*! @brief Insert MET object and contributing object by pointers, with individual weights
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pMET   pointer to non-modifiable MET object
     *  @param[in] pPart  generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] wpx    kinematic weight @f$ w_{x} @f$
     *  @param[in] wpy    kinematic weight @f$ w_{y} @f$
     *  @param[in] wet    kinematic weight @f$ w_{\rm T} @f$
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,double wpx,double wpy,double wet);
    /*! @brief Insert MET object and contributing object by pointers, with list of signals and individual weight components
     *
     *  @copydetails insert(MissingETComponentMap*,const MissingET*,const IParticle*,double,double,double)
     *
     *  @param[in] signalList reference to non-modifiable list of (base type) pointers to signals associated with the given physics or signal object
     *  @param[in] p          signal type indicator (optional, default is MissingETBase:UsageHandler::OnlyCluster)
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,double wpx,double wpy,double wet,
		       MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster);

    /*! @name Find a contributing particle */
    /*!@{*/
    /*! @brief Find non-modifiable contribution for a given MET object
     *
     *  @return Const iterator pointing to a non-modifiable  @link MissingETComponent MissingETComponent @endlink object
     *          linked to given object (particle) if found, else return  end (const) iterator. The first occurance is returned,
     *          independent of the status of the contribution. 
     *
     *  @param[in] pmetObj pointer to valid @link MissingET MissingET @endlink object.
     *
     *  @note This method performs a linear search with at most @c N iterations (@c N is the number of contributions in the composition map).
     */
    static MissingETComponentMap::const_iterator find(const MissingETComponentMap* pMap,const MissingET* pmetObj);
    static MissingETComponentMap::iterator       find(MissingETComponentMap* pMap,const MissingET* pmetObj);
    static MissingETComponentMap::const_iterator find(const MissingETComponentMap* pMap,const std::string& metName);
    static MissingETComponentMap::iterator       find(MissingETComponentMap* pMap,const std::string& metName);
    static MissingETComponentMap::const_iterator find(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src);
    static MissingETComponentMap::iterator       find(MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src);
    static size_t findIndex(const MissingETComponentMap* pMap,const MissingET* pmetObj);
    static size_t findIndex(const MissingETComponentMap* pMap,const std::string& metName);
    static size_t findIndex(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src);


    /*! @brief Find non-modifiable contribution for given particle
     *
     *  @return Const iterator pointing to non-modifiable 
     *  @link MissingETComponent MissingETComponent @endlink object linked to given object (particle) with a given status if 
     *          found, else return end (const) iterator. The first occurance with a given status is returned.
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static MissingETComponentMap::const_iterator find(const MissingETComponentMap* pMap,const IParticle* pPart);
    /*! @brief Find modifiable contribution for given particle
     *
     *  @return Iterator pointing to non-modifiable 
     *  @link MissingETComponent MissingETComponent @endlink object linked to given object (particle) if found, else return
     *          end iterator. The first occurance is returned, independent of the status of the contribution. 
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static MissingETComponentMap::iterator find(MissingETComponentMap* pMap,const IParticle* pPart);
    /*!@}*/

    /*! @name Access contribution */
    /*!@{*/
    /*! @brief Const iterator access to beginning of composition map 
     *
     *  @return Const iterator referencing the first entry in @link MissingETComponentMap MissingETComponentMap @endlink. It the map is empty
     *          MissingETComposition::end(const MissingETComponentMap*) is returned.
     *
     *  @param[in] pMap pointer to non-modifiable composition map.
     *
     *  @warning Handing a NULL pointer to this function will lead to a program termination (crash). 
     */
    static MissingETComponentMap::const_iterator begin(const MissingETComponentMap* pMap);
    /*! @brief Iterator access to beginning of composition map 
     *
     *  @return Iterator referencing the first entry in @link MissingETComponentMap MissingETComponentMap @endlink. It the map is empty
     *          MissingETComposition::end(const MissingETComponentMap*) is returned.
     *
     *  @param[in] pMap pointer to non-modifiable composition map.
     *
     *  @warning Handing a NULL pointer to this function will lead to a program termination (crash). 
     */
    static MissingETComponentMap::iterator begin(MissingETComponentMap* pMap);
    static MissingETComponentMap::const_iterator end(const MissingETComponentMap* pMap);
    static MissingETComponentMap::iterator end(MissingETComponentMap* pMap);
    /*! @brief Access non-modifiable contribution object
     * 
     *  @return Pointer to non-modifiable 
     *  @link MissingETComponent MissingETComponent @endlink object if
     *          contribution of referenced object (particle) found, else 
     *          NULL.
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static const MissingETComponent* getComponent(const MissingETComponentMap* pMap,const IParticle* pPart);
    static const MissingETComponent* getComponent(const MissingETComponentMap* pMap,const MissingET* pmetObj);
    static const MissingETComponent* getComponent(const MissingETComponentMap* pMap,const std::string& metName);
    static const MissingETComponent* getComponent(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src);
    /*! @brief Access non-modifiable contribution object
     * 
     *  @return Pointer to modifiable 
     *  @link MissingETComponent MissingETComponent @endlink object if contribution of referenced object (particle) found, else NULL.
     */
    static MissingETComponent* getComponent(MissingETComponentMap* pMap,const IParticle* pPart);
    static MissingETComponent* getComponent(MissingETComponentMap* pMap,const MissingET* pmetObj);
    static MissingETComponent* getComponent(MissingETComponentMap* pMap,const std::string& metName);
    static MissingETComponent* getComponent(MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t src);
    /*!@}*/

    /*! @name Access contribution parameters and linked objects */
    /*!@{*/
    static MissingETBase::Types::weight_t getWeight(const MissingETComponentMap* pMap,const IParticle* pPart);
    static MissingETBase::Types::weight_t getWeight(MissingETComponentMap::const_iterator fCont,const IParticle* pPart);
    static MissingETBase::Types::bitmask_t getStatusWord(const MissingETComponentMap* pMap,const MissingET* pmetObj);
    static MissingETBase::Types::bitmask_t getStatusWord(MissingETComponentMap::const_iterator fCont);
    static const MissingET* getMissingET(const MissingETComponentMap* pMap,const IParticle* pPart);
    static const MissingET* getMissingET(MissingETComponentMap::const_iterator fCont);
    static const MissingET* getMissingET(const MissingETComponentMap* pMap,const std::string& name);
    static const MissingET* getMissingET(const MissingETComponentMap* pMap,MissingETBase::Types::bitmask_t sw);
    /*@}*/

    /*! @brief Fill a given MissingET object 
     *
     *  This method fills a referenced MissingET object with kinematics from a specific object type. This method is useful if contributions 
     *  to a given MET term are of one object type only (basically true for all hard MET terms). For the soft MET term, which may be composed of
     *  contributions from tracks and clusters, one could invoke this method twice (once for each object type) for the same MissingET object.
     *  The method does not reset the MissingET object, it strictly adds the contributions from objects of the requested type.
     *
     *  @return @c true if anything added to the referenced @link MissingET MissingET @endlink  object, else @c false. 
     *
     *  @tparam OBJTYPE xAOD::Type::ObjectType requested to contribute to the MET object.
     *
     *  @param[in] pMET pointer to modifiable MET object to be updated.
     *  @param[in] status statusword filter to be applied as a contribution filter (optional, default is MissingETBase::Status::clearedStatus())
     */
    template<int OBJTYPE>
    static bool fillMissingET(const MissingETComponentMap* pMap,MissingET* pMET,
                              MissingETBase::Types::bitmask_t /*status*/=MissingETBase::Status::clearedStatus())
    {
      if ( pMap == 0 || pMap->empty() ) return false;
      //
      double sumet(pMET->sumet());
      MissingETComponentMap::const_iterator fComp(pMap->begin());
      MissingETComponentMap::const_iterator lComp(pMap->end());
      for ( ; fComp != lComp; ++fComp )	
	{ 
	  const MissingETComponent* pCont = *fComp;
	  if ( pCont != 0 )
	    {
	      // get objects from contribution
	      MissingETBase::Types::weight_vector_t kinePars;
	      MissingETBase::Types::object_vector_t objs = pCont->objects(kinePars);
	      for ( size_t iObj(0); iObj<objs.size(); ++iObj )
		{ 
		  if ( objs[iObj]->type() == OBJTYPE ) 
		    { // this is really stupid!!!!
		      pMET->add(objs.at(iObj)->p4().Px()*kinePars.at(iObj).wpx(),
				objs.at(iObj)->p4().Py()*kinePars.at(iObj).wpy(),
				objs.at(iObj)->pt()*kinePars.at(iObj).wet());
		    } // requested type
		} // loop on contributing objects
	    } // contribution valid
	} // loop on all contributions
      return pMET->sumet() != sumet;
    } // fillMissingET

    /*! @name Adding data to the association map
     *  @anchor metassoc_insertion 
     */
    /*!@{*/
    /*! @brief Adding a @link xAOD::Jet Jet @endlink object to the map
     *
     *  This function should be called for a given @link xAOD::Jet Jet @endlink object before MissingETComposition::insert is invoked for the same object. Note that
     *  the @ref metassoc_insertion_rules "the rules" for adding a MET object do not allow to add the same object more than once to a given 
     *  @link xAOD::MissingETAssociationMap MissingETAssociationMap @endlink.
     * 
     *  @return @c true if the object is not already in the map, else @c false.
     *
     *  @param[in] pMap      pointer to modifiable @link MissingETAssociationMap MissingETAssociationMap @endlink object (the map to which the @link Jet Jet @endlink 
     *                       object added.
     *  @param[in] pJet      pointer to non-modifiable @link Jet Jet @endlink object.
     *  @param[in] jetconst  vector of elementlinks to jet constituents.
     */
    static bool add(MissingETAssociationMap* pMap,const Jet* pJet,
		    const std::vector<ElementLink<IParticleContainer> >& jetconst=std::vector<ElementLink<IParticleContainer> >(),
		    const MissingETBase::Types::constvec_t& trkvec=MissingETBase::Types::constvec_t());
    /*! @brief Adding a @link xAOD::Jet Jet @endlink object to the map
     *
     *  This function should be called for a given @link xAOD::Jet Jet @endlink object before MissingETComposition::insert is invoked for the same object. Note that
     *  the @ref metassoc_insertion_rules "the rules" for adding a MET object do not allow to add the same object more than once to a given 
     *  @link xAOD::MissingETAssociationMap MissingETAssociationMap @endlink.
     * 
     *  @return @c true if the object is not already in the map, else @c false.
     *
     *  @param[in] pMap      pointer to modifiable @link MissingETAssociationMap MissingETAssociationMap @endlink object (the map to which the @link Jet Jet @endlink 
     *                       object added.
     *  @param[in] pJet      pointer to non-modifiable @link Jet Jet @endlink object.
     *  @param[in] jettracks vector of pointers to tracks associated to jet.
     */
    static bool add(MissingETAssociationMap* pMap,const Jet* pJet,
		    const std::vector<const IParticle*>& jetracks=std::vector<const IParticle*>());
    /*! @brief Add an association to hold objects not associated to any jet
     *
     *  In some cases, e.g. muons, it's useful to have a dummy association, so that overlap removal
     *  can be handled for objects not associated to a jet.
     * 
     *  @return @c true if the object is not already in the map, else @c false.
     *
     *  @param[in] pMap      pointer to modifiable @link MissingETAssociationMap MissingETAssociationMap @endlink object (the map to which the @link Jet Jet @endlink 
     *                       object added.
     */
    static bool addMiscAssociation(MissingETAssociationMap* pMap);
    /*! @brief Insert contributing object to jet referenced by index,
        with constVecs for track and calo constituents
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pJet      non-modifiable pointer to the reference jet
     *  @param[in] pPart     generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] constList reference to non-modifiable list of (base type) pointers to signals associated with the given physics object
     */
    static bool insert(MissingETAssociationMap* pMap,size_t jetIndex,const IParticle* pPart,const std::vector<const IParticle*>& constlist);
    /*! @brief Insert contributing object to jet referenced by pointer,
        with constVecs for track and calo constituents
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pJet      non-modifiable pointer to the reference jet
     *  @param[in] pPart     generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] constList reference to non-modifiable list of (base type) pointers to signals associated with the given physics object
     */
    static bool insert(MissingETAssociationMap* pMap,const Jet* pJet,const IParticle* pPart,const std::vector<const IParticle*>& constlist);
    /*! @brief Insert contributing object, finding the appropriate association automatically
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pPart     generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] constList reference to non-modifiable list of (base type) pointers to signals associated with the given physics object
     */
    static bool insert(MissingETAssociationMap* pMap,const IParticle* pPart,const std::vector<const IParticle*>& constlist,
		       std::map<const IParticle*,MissingETBase::Types::constvec_t> pOverride=std::map<const IParticle*,MissingETBase::Types::constvec_t>());

    static bool setJetConstSum(MissingETAssociationMap* metMap,const Jet* jet,const std::vector<const IParticle*>& altConsts,
			       std::map<const IParticle*,MissingETBase::Types::constvec_t> pOverride);
    /*! @brief Insert contributing object into miscellaneous association
     * 
     *  @return @c true if insertion successful according to the rules lined out @ref metcontrib_insert_rules "here"
     * 
     *  @param[in] pPart     generic pointer to non-modifiable physics or signal object contributing to the MET object
     *  @param[in] constList reference to non-modifiable list of (base type) pointers to signals associated with the given physics object
     */
    static bool insertMisc(MissingETAssociationMap* pMap,const IParticle* pPart,const std::vector<const IParticle*>& constlist);
    /*!@}*/

    /*! @name Find a contributing particle */
    /*!@{*/
    /*! @brief Find non-modifiable contribution for a given MET object
     *
     *  @return Const iterator pointing to a non-modifiable  @link MissingETAssociation MissingETAssociation @endlink object
     *          linked to given object (particle) if found, else return  end (const) iterator. The first occurance is returned,
     *          independent of the status of the contribution. 
     *
     *  @param[in] pJet pointer to valid @link Jet Jet @endlink object.
     *
     *  @note This method performs a linear search with at most @c N iterations (@c N is the number of contributions in the association map).
     */
    static MissingETAssociationMap::const_iterator find(const MissingETAssociationMap* pMap,const Jet* pJet);
    static MissingETAssociationMap::iterator       find(MissingETAssociationMap* pMap,const Jet* pJet);

    /*! @brief Find non-modifiable contribution for given particle
     *
     *  @return Const iterator pointing to non-modifiable 
     *  @link MissingETAssociation MissingETAssociation @endlink object linked to given object (particle) with a given status if 
     *          found, else return end (const) iterator. The first occurance with a given status is returned.
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static MissingETAssociationMap::const_iterator find(const MissingETAssociationMap* pMap,const IParticle* pPart);
    /*! @brief Find modifiable contribution for given particle
     *
     *  @return Iterator pointing to non-modifiable 
     *  @link MissingETAssociation MissingETAssociation @endlink object linked to given object (particle) if found, else return
     *          end iterator. The first occurance is returned, independent of the status of the contribution. 
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static MissingETAssociationMap::iterator find(MissingETAssociationMap* pMap,const IParticle* pPart);
    /*!@}*/

    /*! @name Access contribution */
    /*!@{*/
    /*! @brief Access non-modifiable contribution object
     * 
     *  @return Pointer to non-modifiable 
     *  @link MissingETAssociation MissingETAssociation @endlink object if
     *          contribution of referenced object (particle) found, else 
     *          NULL.
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static std::vector<const MissingETAssociation*> getAssociations(const MissingETAssociationMap* pMap,const IParticle* pPart);
    static MissingETBase::Types::constvec_t getConstVec(const MissingETAssociationMap* pMap,const IParticle* pPart,MissingETBase::UsageHandler::Policy p);
    static bool objSelected(const MissingETAssociationHelper& helper,const IParticle* obj);
    static bool selectIfNoOverlaps(MissingETAssociationHelper& helper,const IParticle* obj,MissingETBase::UsageHandler::Policy p);
    static const MissingETAssociation* getAssociation(const MissingETAssociationMap* pMap,const Jet* pJet);
    //    static const MissingETAssociation* getMiscAssociation(const MissingETAssociationMap* pMap);
    /*! @brief Access non-modifiable contribution object
     * 
     *  @return Pointer to modifiable 
     *  @link MissingETAssociation MissingETAssociation @endlink object if contribution of referenced object (particle) found, else NULL.
     */
    static std::vector<MissingETAssociation*> getAssociations(MissingETAssociationMap* pMap,const IParticle* pPart);
    static MissingETAssociation* getAssociation(MissingETAssociationMap* pMap,const Jet* pJet);
    //    static MissingETAssociation* getMiscAssociation(MissingETAssociationMap* pMap);
    /*!@}*/

    /*! @name Access contribution parameters and linked objects */
    /*!@{*/
    // Leave these for now -- decide how to treat calo vs track vs jet vs signal
    // static MissingETBase::Types::constvec_t getConstVec(const MissingETAssociationMap* pMap,const IParticle* pPart);
    // static MissingETBase::Types::constvec_t getConstVec(MissingETAssociationMap::const_iterator fCont,const IParticle* pPart);
    static const Jet* getRefJet(const MissingETAssociationMap* pMap,const IParticle* pPart);
    static const Jet* getRefJet(MissingETAssociationMap::const_iterator fCont);
    /*@}*/

  }; // MissingETComposition
} // namespace xAOD

/*! @struct xAOD::MissingETComposition
 * 
 *  @brief Collection of functions managing the MET composition map and association map
 *
 *  <b>Preliminaries</b>
 * 
 *  This struct does not contain any private data, and does not support any storable data. It collects functions interacting with the data objects providing the composition of 
 *  a given missing transverse momentum (MET) reconstruction configuration. One set of these data objects includes @link MissingETComponent MissingETComponent @endlink for the description
 *  of the contributions to a given MET, and @link MissingETComponentMap MissingETComponentMap @endlink, which collects all 
 *  contributions to a full and consistent MET and thus describes its configuration in terms of contributions from the all physics and signal objects in the event. 
 *  The other set consists of  @link MissingETAssociation MissingETAssociation @endlink that stores objects matched to a reconstructed jet,
 *  and @link MissingETAssociationMap MissingETAssociationMap @endlink, which encodes all overlaps between objects in a physics event.
 *
 *  <b>Introduction</b>
 *
 *  The principal data objects describing the composition of a given reconstructed MET are (1) the @link MissingETComponent MissingETComponent @endlink object and (2)
 *  the @link MissingETComponentMap MissingETComponentMap @endlink object. The first one describes the contributions to a given MET term, as documented 
 *  @link MissingETComponent here @endlink, and the second object is a collection of MET contibutions describing the composition of the fully reconstructed MET. 
 *
 *  To permit the rebuilding of MET more easily without recourse to the constituent collections, we define (1) the @link MissingETAssociation MissingETAssociation @endlink object and (2)
 *  the @link MissingETAssociationMap MissingETAssociationMap @endlink object. The first one describes the objects matched to a reconstructed jet, as documented 
 *  @link MissingETAssociation here @endlink, and the second object is a collection of associations, holding all overlap removal decisions for objects in the event.
 * 
 *  The interaction with these data objects and the rules applied to these interactions are provided by the functions collected in the MissingETComposition structure.  
 * 
 *  @anchor metcom_insertion_rules
 *  <b>Insertion rules</b>
 *  (@ref metcomp_insertion "link to code documentation")
 *
 *  Two different levels of
 * 
 *  <b>Example: add data to the composition map</b>
 * 
 *  Each tool reconstructing a MET term is expected to add a @link MissingET MissingET @endlink object to a composition map. The sequence in the tool's execute method 
 *  should look like this:
 *
 *   @code
 *   StatusCode METElectronTool::executeTool(xAOD::MissingET* pMET,xAOD::MissingETComponentMap* pMap) {
 *      const xAOD::EgammaContainer pCont = 0; 
 *      if ( this->evtStore()->retrieve(pCont,"ElectronContainer").isFailure() ) 
 *        { ATH_MSG_ERROR("Cannot allocate ElectronContainer"); return StatusCode::FAILURE; }
 * 
 *      pMET->setName("METElectron");                        // set name 
 *      pMET->setSource(MissingETBase::Source::electron());  // set source indicator
 *      if ( !MissingETComposition::add(pMap,pMET,MissingETBase::Status::electron()) )
 *        { ATH_MSG_WARNING("MissingET object <" << pMET->name() << " already in composition map"); return StatusCode::SUCCESS; }
 *
 *      xAOD::EgammaContainer::const_iterator fEle(pCont->begin());
 *      xAOD::EgammaContainer::const_iterator lEle(pCont->end());
 *      MissingETBase::MissingETBase::Types::object_vector_t clusterList;
 *
 *      for ( ; fEle != lEle; ++fEle ) {
 *        if ( this->filter(*fEle) ) {                                       // select electrons
 *          clusterList.clear();                                             // accepted electron: clear cluster list
 *          this->extractClusters(*fEle,clusterList);                        // accepted electron: extract list of clusters associated with the electron      
 *          if ( !pMap->checkOverlap(clusterList) ) {                        // accepted electron: check for signal overlap
 *            pMet.add((*fEle)->p4().Px(),(*fEle)->p4().Py(),(*fEle)->pt()); // accepted electron/no overlap: add to MET object  
 *            MissingETComposition::insert(pMap,pMET,*fEle,clusterList);     // accepted electron/no overlap: add to composition map
 *          } // no signal overlap
 *        } // accepted electron
 *      } // loop on all electrons
 *   
 *      return StatusCode::SUCCESS;
 *    }
 *  @endcode
 * 
 *  In this example MissingETComposition::add(MissingETComponentMap*,const MissingET*,MissingETBase::Types::bitmask_t sw) and 
 *  MissingETComposition::insert(MissingETComponentMap*,const MissingET*,const MissingETBase::Types::weight_t&) are used.
 */

    // /*! @brief Object factories for a MET contribution object 
    //  *
    //  *  These functions can be helpful to generate @link xAOD::MissingETComponent MissingETComponent @endlink objects. They invoke the respective constructor
    //  *  with the given list of arguments. 
    //  *
    //  *  @warning These functions expose the  @link xAOD::MissingETComponent MissingETComponent @endlink object to clients, which may not be desirable. 
    //  *  Non-experts should probably not use them. 
    //  *
    //  */
    // struct Contribution
    // {
    //   /*! @name Object factories for contribution object
    //    *  @anchor contrib_factory 
    //    */
    //   /*!@{*/
    //   /*! @brief Default construction
    //    *
    //    *  Instantiates an empty @link xAOD::MissingETComponent MissingETComponent @endlink object with default parameters given in 
    //    *  @link xAOD::MissingETComponent::MissingETComponent() MissingETComponent::MissingETComponent() @endlink.
    //    *
    //    *  @return Pointer to valid but empty @link xAOD::MissingETComponent MissingETComponent @endlink object.
    //    */
    //   static MissingETComponent* generate();
    //   /*! @brief Construct with a given MET object 
    //    *
    //    *  Instantiates an incomplete (not fully loaded) @link xAOD::MissingETComponent MissingETComponent @endlink object with a link 
    //    *  to a @link xAOD::MissingET MissingET @endlink object and otherwise default data.
    //    *
    //    *  @return Pointer to valid but incomplete @link xAOD::MissingETComponent MissingETComponent @endlink object.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *
    //    *  @note If the given pointer to the @link xAOD::MissingET MissingET @endlink is NULL, the returned contribution object
    //    *        is empty. This is the same behaviour as implemented in the xAOD::MissingETComposition::generate() function.
    //    *        If the reference to the container in the MET object is invalid, 
    //    *        @link xAOD::MissingETComponent::updateMETLink() MissingETComponent::updateMETlink() @endlink
    //    *        must be called after the @link xAOD::MissingET MissingET @endlink object has been inserted into a storable 
    //    *        @link xAOD::MissingETContainer MissingETContainer @endlink. Only then is the EL complete and valid. 
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj);
    //   /*! @brief Construct with a given MET object and a first contributing object
    //    *
    //    *  Instantiates a complete (loaded with one contributing object) @link xAOD::MissingETComponent MissingETComponent @endlink 
    //    *  object with a link to a @link xAOD::MissingET MissingET @endlink object and otherwise default data.
    //    *
    //    *  @return Pointer to valid and loaded @link xAOD::MissingETComponent MissingETComponent @endlink object. Returns NULL if problems 
    //    *          with the generation occur.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *  @param[in] pPart   pointer to non-modifiable contributing object.
    //    *  @param[in] weight  reference to non-modifiable kinematic weight object MissingETComposition::Weight (optional, 
    //    *                     default is given by Weight::Weight()).
    //    *  @param[in] sw      status of contribution (optional, default is MissingETBase::Status::defaultAccept).
    //    *
    //    *  @note If the reference to the container in the MET object is invalid, 
    //    *        xAOD::MissingETComposition::Contribution::updateMETLink() must be called after the @link MissingET MissingET @endlink object has been inserted into a storable 
    //    *        @link MissingETContainer MissingETContainer @endlink. Only then is the EL complete and valid. 
    //    *
    //    *  @warning Any invalid input (expecially NULL pointers) suppresses generation of the @link MissingETComponent MissingETComponent @endlink 
    //    *           object (a NULL pointer is returned).
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj,const IParticle* pPart,const MissingETBase::Types::weight_t& weight=MissingETBase::Types::weight_t(),
    // 					     MissingETBase::Types::bitmask_t sw=MissingETBase::Status::defaultAccept);
    //   /*! @brief Construct with a given MET object and a first contributing object
    //    *
    //    *  Instantiates a complete (loaded with one contributing object) @link MissingETComponent MissingETComponent @endlink 
    //    *  object with a link to a @link MissingET MissingET @endlink  object and otherwise default data.
    //    *
    //    *  @return Pointer to valid and loaded @link MissingETComponent MissingETComponent @endlink object. Returns NULL if problem with generation.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *  @param[in] pPart   pointer to non-modifiable contributing object.
    //    *  @param[in] wpx     kinematic weight @f$ w_{x} @f$
    //    *  @param[in] wpy     kinematic weight @f$ w_{y} @f$
    //    *  @param[in] wet     kinematic weight @f$ w_{\rm T} @f$
    //    *  @param[in] sw      status of contribution (optional, default is MissingETBase::Status::defaultAccept.
    //    *
    //    *  @note If the reference to the container in the MET object is invalid, 
    //    *        Contribution::updateMETLink() must be called after the @link MissingET MissingET @endlink object has been inserted into a storable 
    //    *        @link MissingETContainer MissingETContainer @endlink. Only then is the EL complete and valid. The same
    //    *        applies to the EL to all contributing objects. 
    //    *        Invoking Contribution::updateLinks() updates all EL with the container pointers, making them valid if the container pointers are not 
    //    *        NULL. This action involves a loop over the full list of contributing objects. 
    //    *
    //    *  @warning Any invalid input (expecially NULL pointers) suppresses generation of the @link MissingETComponent MissingETComponent @endlink 
    //    *           object (a NULL pointer is returned).
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj,const IParticle* pPart,double wpx,double wpy,double wet,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());

    //   /*!@}*/
    // }; // Contribution

#endif
