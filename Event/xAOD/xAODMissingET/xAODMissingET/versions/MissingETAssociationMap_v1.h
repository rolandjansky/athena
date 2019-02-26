// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETASSOCIATIONMAP_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETASSOCIATIONMAP_V1_H

#include "AthContainers/DataVector.h"

#include "xAODJet/Jet.h"
#include "xAODMissingET/versions/MissingETAssociation_v1.h"

#include <string>
#include <vector>
#include <map>

#include <iostream>

namespace xAOD
{

  // Forward declarations
	class MissingETAssociation_v1;
	class MissingETAssociationHelper;

  class MissingETAssociationMap_v1 : public DataVector<MissingETAssociation_v1>
  {
  public:

    /*! @brief Default constructor */
    MissingETAssociationMap_v1(SG::OwnershipPolicy own=SG::OWN_ELEMENTS, SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    /*! @brief Constructor from iterators */
    MissingETAssociationMap_v1(MissingETAssociationMap_v1::iterator first, MissingETAssociationMap_v1::iterator last,
			     SG::OwnershipPolicy own=SG::OWN_ELEMENTS, SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    /*! @brief Base class destructor */
    virtual ~MissingETAssociationMap_v1();

    /*! @name Find methods */
    /*!@{*/
    /*! @brief Find association from jet pointer
     *
     *  @return Const iterator referencing a non-modifiable xAOD::MissingETAssociation_v1 object linking to the given jet. If the jet is not in the association map,
     *          an end const iterator is returned.
     *
     *  @param[in] pJet pointer to the xAOD::Jet object searched for in the association map.   
     */
    const_iterator find(const Jet* pJet)   const;
    /*! @brief Find association from jet pointer
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETAssociation_v1 object linking to the given jet. If the reference jet is not in the association map,
     *          an end iterator is returned.
     *
     *  @param[in] pJet pointer to the xAOD::Jet object searched for in the association map.   
     */
    iterator find(const Jet* pJet);
    size_t   findIndex(const Jet* pJet) const;
    //
    /*! @brief Find association from jet constituent
     *
     *  @return Const iterator referencing a modifiable xAOD::MissingETAssociation_v1 object linking to the jet containing this constituent. If the parent jet is not in the association map,
     *          an end const iterator is returned.
     *
     *  @param[in]  pointer to the xAOD::IParticle object searched for in the association map.   
     */
    const_iterator findByJetConst(const IParticle* pConst) const;
    /*! @brief Find association from jet constituent
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETAssociation_v1 object linking to the jet containing this constituent. If the parent jet is not in the association map,
     *          an end iterator is returned.
     *
     *  @param[in]  pointer to the xAOD::IParticle object searched for in the association map.   
     */
    iterator       findByJetConst(const IParticle* pConst);
    /*! @brief Find association from jet constituent
     *
     *  @return Const iterator referencing a modifiable xAOD::MissingETAssociation_v1 object linking to the jet containing this constituent. If the parent jet is not in the association map,
     *          an end const iterator is returned.
     *
     *  @param[in]  ElementLink to the xAOD::IParticle object searched for in the association map.   
     */
    const_iterator findByJetConst(ElementLink<IParticleContainer> constLink) const;
    /*! @brief Find association from jet constituent
     *
     *  @return Iterator referencing a modifiable xAOD::MissingETAssociation_v1 object linking to the jet containing this constituent. If the parent jet is not in the association map,
     *          an end iterator is returned.
     *
     *  @param[in]  ElementLink to the xAOD::IParticle object searched for in the association map.   
     */
    iterator       findByJetConst(ElementLink<IParticleContainer> constLink);
    size_t findIndexByJetConst(const IParticle* pConst) const;
    size_t findIndexByJetConst(ElementLink<IParticleContainer> pConstLink) const;
    /*! @brief Get an association for miscellaneous objects not associated to jets
     *
     *  @return View container of non-overlapping constituents, which the user should delete
     */
    const MissingETAssociation_v1* getMiscAssociation() const;
    MissingETAssociation_v1* getMiscAssociation();
    /*!@}*/

    /*! @name Jet constituent and overlap removal methods */
    /*!@{*/
    /*! @brief Set constituent map for a jet
     *
     *  @return Boolean stating whether or not jet is linked to an association in the map.
     *
     *  @param[in]  Pointer to the jet
     */
    bool setJetConstituents(const Jet* pJet);
    /*! @brief Set constituent map for a jet
     *
     *  @return Boolean stating whether or not jet is linked to an association in the map.
     *
     *  @param[in]  Pointer to the jet
     *  @param[in]  Non-modifiable vector of ElementLink to the jet constituents
     */
    bool setJetConstituents(const std::vector<ElementLink<IParticleContainer> >& constLinks, const Jet* pJet);
    /*! @brief Set constituent map for a jet
     *
     *  @return Boolean stating whether or not jet is linked to an association in the map.
     *
     *  @param[in]  Index of the jet's association in the map
     *  @param[in]  Non-modifiable vector of ElementLink to the jet constituents
     */
    bool setJetConstituents(const std::vector<ElementLink<IParticleContainer> >& constLinks, size_t jetIndex);
    /*! @brief Do overlap-finding in all associations
     *
     *  @return Boolean stating whether any overlaps were identified
     *
     */
    bool identifyOverlaps();
    void clearOverlaps();
    /*! @brief Extract a container of constituents that are not in jets
     *
     *  @return View container of non-overlapping constituents, which the user should delete
     *
     *  @param[in]  Non-modifiable IParticleContainer holding the constituents to be tested
     *  @param[in]  MissingETBase::UsageHandler::Policy stating which types of constituents to check
     */
    const IParticleContainer* getUniqueSignals(const IParticleContainer* signals,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::TrackCluster) const;
    /*! @brief Extract a container of constituents that do not overlap physics objects
     *
     *  @return View container of non-overlapping constituents, which the user should delete
     *
     *  @param[in]  Non-modifiable IParticleContainer holding the constituents to be tested
     *  @param[in]  MissingETBase::UsageHandler::Policy stating which types of constituents to check
     */
    const IParticleContainer* getOverlapRemovedSignals(const MissingETAssociationHelper* helper,const IParticleContainer* signals,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::TrackCluster) const;
    /*!@}*/

  protected:

    void f_setJetConstMap(std::map<ElementLink<IParticleContainer>, size_t> map);
    void f_clearJetConstMap();
    std::map<ElementLink<IParticleContainer>, size_t> m_jetConstLinks;  /*!< @brief Map jet constituents to the jet index */

    /*! @name Internal find methods for constant access */
    /*!@{*/
    /*! @brief Find contribution by reference jet pointer
     *
     *  This method finds a xAOD::MissingETAssociation_v1 object linked to the specified xAOD::Jet object. It is invoked internally by find(const Jet*).
     *  It uses an internal cache to determine if this jet has been found in the latest invocation of the find method, and returns without further action in this case.
     *  If a different jet is searched for, it initiates a linear search and updates the internal cache with the new pointer (will be NULL if referenced jet 
     *  not found) and the datawords storing the const_iterator and the iterator referencing the non-modifiable and modifiable xAOD::MissingETAssociation_v1 object in the
     *  association map, respectively (both iterators will be set to the corresponding end iterator if the jet is not in the association map). 
     *
     *  @return Valid const iterator referencing the MissingETAssociation_v1 object linking ot the requested jet. If this jet is not in the list, 
     *          MissingETAssociationMap_v1::end() is returned.
     * 
     *  @param[in] pJet pointer to non-modifiable Jet object to be found in the association map.  
     */
    const_iterator f_findConst(const Jet* pJet) const;
    iterator f_find(const Jet* pJet);
    /*!@}*/


  }; // class xAOD::MissingETAssociationMap_v1
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::MissingETAssociationMap_v1, DataVector<xAOD::MissingETAssociation_v1>);

#endif
