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

namespace xAOD 
{
  struct MissingETComposition
  {
    /*! @name Adding data to the composition map
     *  @anchor metcomp_insertion 
     */
    /*!@{*/
    /*! @brief Adding a @link xAOD::MissingET_v1 MissingET @endlink object to the map
     *
     *  This function should be called for a given @link xAOD::MissingET_v1 MissingET @endlink object before MissingETComposition::insert is invoked for the same object. Note that
     *  the @ref metcomp_insertion_rules "the rules" for adding a MET object do not allow to add the same object more than once to a given 
     *  @link xAOD::MissingETComponentMap_v1 MissingETComponentMap @endlink.
     * 
     *  @return @c true if the object is not already in the map, else @c false.
     *
     *  @param[in] pMap pointer to modifiable @link MissingETComponentMap_v1 MissingETComponentMap @endlink object (the map to which the @link MissingET_v1 MissingET @endlink 
     *             object added.
     *  @param[in] pMET pointer to non-modifiable @link MissingET_v1 MissingET @endlink object.
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
     *  @link MissingETComponent_v1::Weight::Weight() MissingETComponent::Weight::Weight() @endlink constructor)
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,MissingETBase::Types::weight_t weight=MissingETBase::Types::weight_t());
    /*! @brief Insert MET object and contributing object by pointers, with list of signals and optional kinematic weight object.
     *
     *  @copydetail insert(MissingETComponentMap*,const MissingET*,const IParticle*,MissingETBase::Types::weight_t)
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
     *  @copydetail insert(MissingETComponentMap*,const MissingET*,const IParticle*,double,double,double)
     *
     *  @param[in] signalList reference to non-modifiable list of (base type) pointers to signals associated with the given physics or signal object
     *  @param[in] p          signal type indicator (optional, default is MissingETBase:UsageHandler::OnlyCluster)
     */
    static bool insert(MissingETComponentMap* pMap,const MissingET* pMET,const IParticle* pPart,const MissingETBase::Types::object_vector_t& signalList,double wpx,double wpy,double wet,
		       MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster);
    /*! @brief Update all ElementLinks in all contributions 
     *
     *  This function triggers an update of all ElementLinks stored in the map entries. This may be necessary in case a @link MissingETComponent_v1 MissingETComponent @endlink
     *  object has been filled with POD MET, particle, and/or signal objects, thus rendering the corresponding ElementLink incomplete. If the linked to objects are 
     *  stored in a collection later, this function makes sure that all ElementLinks are updated with the collection pointer. 
     *
     *  @warning Usually MET terms are reconstructed from signal or physics objects stored in a collection. In this case this function does not need to  be
     *           used at all, as all ElemenntLinks to contributing objects are complete. On the other hand, the @link MissingET_v1 MissingET @endlink in the 
     *           contribution is usually still a POD when the composition map entry for the corresponding MET term is generated, and the ElementLink to 
     *           it requires an update later, when the @link MissingET_v1 MissingET @endlink object is stored in a @link MissingETContainer_v1 MissingEETContainer @endlink.
     *           In this case MissingETComposition::updateMETLinks(MissingETComponentMap*) should be used after the MET object is inserted in its container, as
     *           it is much faster (only scans the small number of @link MissingETComponent_v1 MissingETComponent @endlink objects in the map).  
     * 
     *  @return @c true if all links in all MET contributions in the composition map are updated correctly. In case of an unsuccessful update attempt, or 
     *          an invalid pointer to the @link MissingETComponentMap_v1 MissingETComponentMap @endlink object (NULL pointer), @c false is
     *          returned. 
     *
     *  @param[in] pMap pointer referencing a modifiable MET composition map.
     *
     *  @note This method involves a scan of all @link MissingETComponent_v1 MissingETComponent @endlink objects stored in the map, and a scan
     *        of the contributing object list in these MET contributions. This can be costly in terms off CPU usage.
     *
     */
    static bool updateLinks(MissingETComponentMap* pMap);
    /*! @brief Update all ElementLinks to MET objects only in all contributions 
     *
     *  This function triggers an update of all ElementLinks stored in the map entries. This may be necessary in case a @link MissingETComponent_v1 MissingETComponent @endlink
     *  object has been filled with POD @link MissingET_v1 MissingET @endlink, thus rendering the corresponding ElementLink incomplete. If the linked to MET objects are 
     *  stored in a collection later, this function makes sure that the ElementLinks are updated with the collection pointer. 
     *
     *  @return @c true if all links to @link MissingET_v1 MissingET @endlink objects in all MET contributions in the composition map are updated correctly. 
     *          In case of an unsuccessful update attempt, or 
     *          an invalid pointer to the @link MissingETComponentMap_v1 MissingETComponentMap @endlink object (NULL pointer), @c false is
     *          returned. 
     *
     *  @param[in] pMap pointer referencing a modifiable MET composition map.
     *
     *  @note This method involves a scan of all @link MissingETComponent_v1 MissingETComponent @endlink objects stored in the map.
     *
     */
    static bool updateMETLinks(MissingETComponentMap* pMap);
    /*!@}*/

    /*! @name Find a contributing particle */
    /*!@{*/
    /*! @brief Find non-modifiable contribution for a given MET object
     *
     *  @return Const iterator pointing to a non-modifiable  @link MissingETComponent_v1 MissingETComponent @endlink object
     *          linked to given object (particle) if found, else return  end (const) iterator. The first occurance is returned,
     *          independent of the status of the contribution. 
     *
     *  @param[in] pmetObj pointer to valid @link MissingET_v1 MissingET @endlink object.
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
     *  @link MissingETComponent_v1 MissingETComponent @endlink object linked to given object (particle) with a given status if 
     *          found, else return end (const) iterator. The first occurance with a given status is returned.
     *
     *  @param[in] pPart pointer to non-modifiable object possibly contributing to a given MET object.
     */
    static MissingETComponentMap::const_iterator find(const MissingETComponentMap* pMap,const IParticle* pPart);
    /*! @brief Find modifiable contribution for given particle
     *
     *  @return Iterator pointing to non-modifiable 
     *  @link MissingETComponent_v1 MissingETComponent @endlink object linked to given object (particle) if found, else return
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
     *  @return Const iterator referencing the first entry in @link MissingETComponentMap_v1 MissingETComponentMap @endlink. It the map is empty
     *          MissingETComposition::end(const MissingETComponentMap*) is returned.
     *
     *  @param[in] pMap pointer to non-modifiable composition map.
     *
     *  @warning Handing a NULL pointer to this function will lead to a program termination (crash). 
     */
    static MissingETComponentMap::const_iterator begin(const MissingETComponentMap* pMap);
    /*! @brief Iterator access to beginning of composition map 
     *
     *  @return Iterator referencing the first entry in @link MissingETComponentMap_v1 MissingETComponentMap @endlink. It the map is empty
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
     *  @link MissingETComponent_v1 MissingETComponent @endlink object if
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
     *  @link MissingETComponent_v1 MissingETComponent @endlink object if contribution of referenced object (particle) found, else NULL.
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

    /*! @brief Fill a given MissingET_v1 object 
     *
     *  This method fills a referenced MissingET_v1 object with kinematics from a specific object type. This method is useful if contributions 
     *  to a given MET term are of one object type only (basically true for all hard MET terms). For the soft MET term, which may be composed of
     *  contributions from tracks and clusters, one could invoke this method twice (once for each object type) for the same MissingET_v1 object.
     *  The method does not reset the MissingET_v1 object, it strictly adds the contributions from objects of the requested type.
     *
     *  @return @c true if anything added to the referenced @link MissingET_v1 MissingET @endlink  object, else @c false. 
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
	  const MissingETComponent_v1* pCont = *fComp;
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
  }; // MissingETComposition
} // namespace xAOD

/*! @struct xAOD::MissingETComposition
 * 
 *  @brief Collection of functions managing the MET composition map
 *
 *  <b>Preliminaries</b>
 * 
 *  This struct does not contain any private data, and does not support any storable data. It collects functions interacting with the data objects providing the composition of 
 *  a given missing transverse momentum (MET) reconstruction configuration. These data objects are @link MissingETComponent_v1 MissingETComponent @endlink for the description
 *  of the contributions to a given MET, and @link MissingETComponentMap_v1 MissingETComponentMap @endlink, which collects all 
 *  contributions to a full and consistent MET and thus describes its configuration in terms of contributions from the all physics and signal objects in the event. 
 *
 *  <b>Introduction</b>
 *
 *  The principal data objects describing the composition of a given reconstructed MET are (1) the @link MissingETComponent_v1 MissingETComponent @endlink object and (2)
 *  the @link MissingETComponentMap_v1 MissingETComponentMap @endlink object. The first one describes the contributions to a given MET term, as documented 
 *  @link MissingETComponent_v1 here @endlink, and the second object is a collection of MET contibutions describing the composition of the fully reconstructed MET. 
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
 *  Each tool reconstructing a MET term is expected to add a @link MissingET_v1 MissingET @endlink object to a composition map. The sequence in the tool's execute method 
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
    //  *  These functions can be helpful to generate @link xAOD::MissingETComponent_v1 MissingETComponent @endlink objects. They invoke the respective constructor
    //  *  with the given list of arguments. 
    //  *
    //  *  @warning These functions expose the  @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object to clients, which may not be desirable. 
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
    //    *  Instantiates an empty @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object with default parameters given in 
    //    *  @link xAOD::MissingETComponent_v1::MissingETComponent_v1() MissingETComponent::MissingETComponent() @endlink.
    //    *
    //    *  @return Pointer to valid but empty @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object.
    //    */
    //   static MissingETComponent* generate();
    //   /*! @brief Construct with a given MET object 
    //    *
    //    *  Instantiates an incomplete (not fully loaded) @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object with a link 
    //    *  to a @link xAOD::MissingET_v1 MissingET @endlink object and otherwise default data.
    //    *
    //    *  @return Pointer to valid but incomplete @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *
    //    *  @note If the given pointer to the @link xAOD::MissingET_v1 MissingET @endlink is NULL, the returned contribution object
    //    *        is empty. This is the same behaviour as implemented in the xAOD::MissingETComposition::generate() function.
    //    *        If the reference to the container in the MET object is invalid, 
    //    *        @link xAOD::MissingETComponent_v1::updateMETLink() MissingETComponent::updateMETlink() @endlink
    //    *        must be called after the @link xAOD::MissingET_v1 MissingET @endlink object has been inserted into a storable 
    //    *        @link xAOD::MissingETContainer_v1 MissingETContainer @endlink. Only then is the EL complete and valid. 
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj);
    //   /*! @brief Construct with a given MET object and a first contributing object
    //    *
    //    *  Instantiates a complete (loaded with one contributing object) @link xAOD::MissingETComponent_v1 MissingETComponent @endlink 
    //    *  object with a link to a @link xAOD::MissingET_v1 MissingET @endlink object and otherwise default data.
    //    *
    //    *  @return Pointer to valid and loaded @link xAOD::MissingETComponent_v1 MissingETComponent @endlink object. Returns NULL if problems 
    //    *          with the generation occur.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *  @param[in] pPart   pointer to non-modifiable contributing object.
    //    *  @param[in] weight  reference to non-modifiable kinematic weight object MissingETComposition::Weight (optional, 
    //    *                     default is given by Weight::Weight()).
    //    *  @param[in] sw      status of contribution (optional, default is MissingETBase::Status::defaultAccept).
    //    *
    //    *  @note If the reference to the container in the MET object is invalid, 
    //    *        xAOD::MissingETComposition::Contribution::updateMETLink() must be called after the @link MissingET_v1 MissingET @endlink object has been inserted into a storable 
    //    *        @link MissingETContainer_v1 MissingETContainer @endlink. Only then is the EL complete and valid. 
    //    *
    //    *  @warning Any invalid input (expecially NULL pointers) suppresses generation of the @link MissingETComponent_v1 MissingETComponent @endlink 
    //    *           object (a NULL pointer is returned).
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj,const IParticle* pPart,const MissingETBase::Types::weight_t& weight=MissingETBase::Types::weight_t(),
    // 					     MissingETBase::Types::bitmask_t sw=MissingETBase::Status::defaultAccept);
    //   /*! @brief Construct with a given MET object and a first contributing object
    //    *
    //    *  Instantiates a complete (loaded with one contributing object) @link MissingETComponent_v1 MissingETComponent @endlink 
    //    *  object with a link to a @link MissingET_v1 MissingET @endlink  object and otherwise default data.
    //    *
    //    *  @return Pointer to valid and loaded @link MissingETComponent_v1 MissingETComponent @endlink object. Returns NULL if problem with generation.
    //    *
    //    *  @param[in] pmetObj pointer to non-modifiable MET object.
    //    *  @param[in] pPart   pointer to non-modifiable contributing object.
    //    *  @param[in] wpx     kinematic weight @f$ w_{x} @f$
    //    *  @param[in] wpy     kinematic weight @f$ w_{y} @f$
    //    *  @param[in] wet     kinematic weight @f$ w_{\rm T} @f$
    //    *  @param[in] sw      status of contribution (optional, default is MissingETBase::Status::defaultAccept.
    //    *
    //    *  @note If the reference to the container in the MET object is invalid, 
    //    *        Contribution::updateMETLink() must be called after the @link MissingET_v1 MissingET @endlink object has been inserted into a storable 
    //    *        @link MissingETContainer_v1 MissingETContainer @endlink. Only then is the EL complete and valid. The same
    //    *        applies to the EL to all contributing objects. 
    //    *        Invoking Contribution::updateLinks() updates all EL with the container pointers, making them valid if the container pointers are not 
    //    *        NULL. This action involves a loop over the full list of contributing objects. 
    //    *
    //    *  @warning Any invalid input (expecially NULL pointers) suppresses generation of the @link MissingETComponent_v1 MissingETComponent @endlink 
    //    *           object (a NULL pointer is returned).
    //    */
    //   static MissingETComponent* generate(const MissingET* pmetObj,const IParticle* pPart,double wpx,double wpy,double wet,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());

    //   /*!@}*/
    // }; // Contribution

#endif
