// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETCOMPONENT_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETCOMPONENT_V1_H

#include "AthContainers/AuxElement.h"

#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODMissingET/MissingETContainer.h"

#include <vector>

namespace xAOD
{
  /*!  @brief MET component descriptor contains object links and corresponding parameters */ 
  class MissingETComponent_v1 : public SG::AuxElement
  {
  public:
    /*! @brief Kinematic weight descriptor */                                                                                                                                
    class Weight
    {
    public:
      Weight();                                 /*!< @brief Default constructor */
      Weight(double wpx,double wpy,double wet); /*!< @brief Constructor with parameters */
      // Weight(const Weight& wght);               /*!< @brief Copy constructor */
      ~Weight();                                /*!< @brief Data class destructor */
      /*! @name Accessors */
      /*!@{*/
      double wpx() const; /*!< @brief Returns @f$ w_{x} @f$ */
      double wpy() const; /*!< @brief Returns @f$ w_{y} @f$ */
      double wet() const; /*!< @brief Returns @f$ w_{\rm T} @f$ */
      /*!@}*/
      /*! @name Setters */
      /*!@{*/
      void setWpx(double wpx); /*!< @brief Set @f$ w_{x} @f$ */
      void setWpy(double wpy); /*!< @brief Set @f$ w_{y} @f$ */
      void setWet(double wet); /*!< @brief Set @f$ w_{\rm T} @f$ */
      /*!@}*/
      /*! @name Modifiers for kinematic weights */
      /*!@{*/
      double& wpx();                          /*!< @brief Returns reference to modifiable dataword storing @f$ w_{x} @f$ */
      double& wpy();                          /*!< @brief Returns reference to modifiable data word storing @f$ w_{y} @f$ */
      double& wet();                          /*!< @brief Returns reference to modifiable data word storing @f$ w_{\rm T} @f$ */
      Weight& operator+=(const Weight& wght); /*!< @brief Add another weight */
      Weight& operator-=(const Weight& wght); /*!< @brief Subtract another weight */
      Weight& operator*=(const Weight& wght); /*!< @brief Multiply two weights */
      Weight& operator*=(double scale);       /*!< @brief Scale weight by multiplication */ 
      Weight& operator/=(const Weight& wght); /*!< @brief Divide two weights */
      Weight& operator/=(double scale);       /*!< @brief Scale weight by division */
      /*!@}*/
      /*! @name Comparators */
      /*!@{*/
      bool operator==(const Weight& wght) const; /*!< @brief Equality comparator */
      bool operator!=(const Weight& wght) const; /*!< @brief Inequality operator */
      /*!@}*/
     private:
      /*! @name Cache for weights */
      /*!@{*/
      double m_wpx; /*!< @brief @f$ w_{x} @f$ */
      double m_wpy; /*!< @brief @f$ w_{y} @f$ */
      double m_wet; /*!< @brief @f$ w_{\rm T} @f$ */
      /*!@}*/
    }; // MissingETComponent_v1::Weight

    MissingETComponent_v1(bool createStore=false);                                                                                /*!< @brief Default constructor */
    MissingETComponent_v1(const MissingET* pmetObj,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus()); /*!< @brief Construct with a link to a MissingET object */
    /*! @brief Construct with a link to a MissingET and a link to an IParticle object, and a @c Weight */
    MissingETComponent_v1(const MissingET* pmetObj,const IParticle* pPart,const Weight& wght=Weight(),MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    /*! @brief Construct with a link to a MissingET and a link to an IParticle object, and indivdual numerical weight components */
    MissingETComponent_v1(const MissingET* pmetObj,const IParticle* pPart,double wpx,double wpy,double wet,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    MissingETComponent_v1(const MissingETComponent_v1& compDescr);                                    /*!< @brief Copy constructor */
    MissingETComponent_v1(const MissingETComponent_v1& compDescr,MissingETBase::Types::bitmask_t sw); /*!< @brief Copy constructor with new status word */
    MissingETComponent_v1& operator=(const MissingETComponent_v1& compDescr);                         /*! @brief Assignment operator */
    virtual ~MissingETComponent_v1();                                                                 /*! @brief Base class destructor */

    /*! @name Set MET object
     *  @anchor contrib_set
     */
    /*!@{*/
    /*! @brief Set MET object by object pointer reference */
    bool setMET(const MissingET* pmetObj,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    /*! @brief Set MET object by container pointer and index reference */
    bool setMET(const MissingETContainer_v1* pmetCont,size_t pmedIdx,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    /*!@}*/
 
    /*! @name Adding data to the MET component description
     *  @anchor contrib_add
     */
    bool addObject(const IParticle* pPart,const Weight& wght=Weight());      /*!< @brief Add object (particle) from pointer reference with optional kinematic weight */
    bool addObject(const IParticle* pPart,double wpx,double wpy,double wet); /*!< @brief Add object (particle) from pointer reference with individual kinematic weight components */
    /*!@}*/

    /*! @name Conventional and dedicated getter and setters 
     *  @anchor contrib_convention
     */
    /*!@{*/
    const std::vector<double>& wpx() const;                                     /*!< @brief Get the vector of @f$ w_{x} @f$ weight components  */
    double wpx(const IParticle* pPart) const;                                   /*!< @brief Get the weight component @f$ w_{x} @f$ for a given object referenced by pointer */
    double wpx(size_t pIdx) const;                                              /*!< @brief Get the weight component @f$ w_{x} @f$ for a given object referenced by index */
    //
    bool setWpx(const std::vector<double>& wcv);                                /*!< @brief Set the vector of @f$ w_{x} @f$ weight components  */
    bool setWpx(const IParticle* pPart,double wpx);                             /*!< @brief Set kinematic weight component @f$ w_{x} @f$ for a given contributing object referenced by pointer */
    bool setWpx(size_t pIdx,double wpx);                                        /*!< @brief Set kinematic weight component @f$ w_{x} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& wpy() const;                                     /*!< @brief Get the vector of @f$ w_{y} @f$ weight components  */
    double wpy(const IParticle* pPart) const;                                   /*!< @brief Get the weight component @f$ w_{y} @f$ for a given object referenced by pointer */
    double wpy(size_t pIdx) const;                                              /*!< @brief Get the weight component @f$ w_{y} @f$ for a given object referenced by index */
    //
    bool setWpy(const std::vector<double>& wpyVector);                          /*!< @brief Set the vector of @f$ w_{y} @f$ weight components  */
    bool setWpy(const IParticle* pPart,double wpy);                             /*!< @brief Set kinematic weight component @f$ w_{y} @f$ for a given contributing object referenced by pointer */
    bool setWpy(size_t pIdx,double wpy);                                        /*!< @brief Set kinematic weight component @f$ w_{y} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& wet() const;                                     /*!< @brief Get the vector of @f$ w_{\rm T} @f$ weight components  */
    double wet(const IParticle* pPart) const;                                   /*!< @brief Get the weight component @f$ w_{\rm T} @f$ for a given object referenced by pointer */
    double wet(size_t pIdx) const;                                              /*!< @brief Get the weight component @f$ w_{\rm T} @f$ for a given object referenced by index */
    //
    bool setWet(const std::vector<double>& wcv);                                /*!< @brief Set the vector of @f$ w_{\rm T} @f$ weight components  */
    bool setWet(const IParticle* pPart,double wet);                             /*!< @brief Set kinematic weight @f$ w_{\rm T} @f$ for a given contributing object referenced by pointer */
    bool setWet(size_t pIdx,double wet);                                        /*!< @brief Set kinematic weight @f$ w_{\rm T} @f$ for a given contributing object referenced by index */
    //
    bool setWeight(const IParticle* pPart,const Weight& wght=Weight());         /*!< @brief Set the kinematic weight of an object contribution referenced by pointer */
    bool setWeight(const IParticle* pPart,double wpx,double wpy,double wet);    /*!< @brief Set the kinematic weight components of an object contribution referenced by pointer */
    bool setWeight(size_t pIdx,const Weight& wght=Weight());                    /*!< @brief Set the kinematic weight of an object contribution referenced by index */
    bool setWeight(size_t pIdx,double wpx,double wpy,double wet);               /*!< @brief Set the kinematic weight components of an object contribution referenced by index */
    //
    const MissingETBase::Types::metlink_t& metLink() const;                     /*!< @brief Get the link to the MET object */
    bool setMetLink(const MissingETBase::Types::metlink_t& metLnk);             /*!< @brief Set the link to the MET object */
    const MissingETBase::Types::objlink_vector_t& objectLinks() const;          /*!< @brief Get the vector of links to the contributing physics or signal objects */
    bool setObjectLinks(const MissingETBase::Types::objlink_vector_t& objLnks); /*!< @brief Set the vector of links to the contributing physics or signal objects */
    MissingETBase::Types::bitmask_t statusWord() const;                         /*!< @brief Get the statusword */
    bool setStatusWord(MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus()); /*!< @brief Set the statusword of a MET term */
    /*!@}*/

    /*! @name Specific methods for manipulating the component description data 
     *  @anchor contrib_manip
     */
    /*!@{*/
    bool removeContrib(const IParticle* pPart);               /*!< @brief Remove a contribution referenced by an object pointer */
    bool removeContrib(size_t pIdx);                          /*!< @brief Remove a contribution referenced by an index */
    bool removeContrib();                                     /*!< @brief Remove all contributions */
    bool resetContrib(const IParticle* pPart);                /*!< @brief Reset the contribution parameters of an object referenced by a pointer */
    bool resetContrib(size_t pIdx);                           /*!< @brief Reset the contribution parameters of an object referenced by an index */
    bool resetContrib();                                      /*!< @brief Reset all contribution parameters */
    bool mergeStatusWord(MissingETBase::Types::bitmask_t sw); /*!< @brief Merge patterns into the MET term status */
    bool clearStatusWord();                                   /*!< @brief Clear statusword */
    /*!@}*/

    /*! @name Dedicated accessors for linked objects */
    /*!@{*/
    const MissingET* metObject() const;                                      /*!< @brief Access MET object */
    const MissingETContainer_v1* metObjectContainer() const;                    /*!< @brief Access MET object container holding MET object */
    size_t metObjectIndex() const;                                              /*!< @brief Access index of MET object in its container */
    std::vector<const IParticle*> objects() const;                              /*!< @brief Access contributing objects */
    std::vector<const IParticle*> objects(std::vector<Weight>& kinePars) const; /*!< @brief Access contributing objects and retrieve kinematic weights */
    std::vector<const IParticle*> objects(const std::vector<double>*& wpxPtr,    
					  const std::vector<double>*& wpyPtr,
					  const std::vector<double>*& wetPtr) const; /*!< @brief Access contributing objects and retrieve kinematic weight components */
    Weight weight(const IParticle* pPart) const;                                     /*!< @brief Get kinematic weight for a given object */
    Weight weight(size_t pIdx) const;                                                /*!< @brief Get kinematic weight for a given entry (index) in the contributing object list */
    /*!@}*/ 


    /*! @name Allocating and accessing objects in the contribution
     *  @anchor contrib_alloc
     */
    /*! @brief Find index of given object in contributing object store 
     * 
     *  The returned index is valid for the contributing object store and the parameter stores (index-parallel stores). 
     *
     *  @return A valid index @f$ i \in \{ 0 \ldots N_{\rm objects} - 1 \} @f$, where @f$ N_{\rm objects} @f$ is the total number of contributing objects to the
     *          MET contribution. If the given object is not in the store with the requested status, MissingETBase::Numerical::invalidIndex() is returned.
     *
     *
     *  This method makes use of a cache. If the pointer to the checked object is the same as the last one successfully searched for, the index is returned from 
     *  a cached data word. If the pointer is not the same, xAOD::MissingETComponent::findIndex(const IParticle*) is invoked. If this method finds the referenced
     *  object, the  cache is updated and the valid index is returned. In case the object is not found, MissingETBase::Nummerical::invalidIndex() is returned.   
     *
     *  @return Valid object index @f$ i = 0 \ldots N_{\rm objects} - 1 @f$ if object found in list, else MissingETBase::Numerical::invalidIndex().
     *
     *  @param[in] pPart  pointer to non-modifiable object
     *  @param[in] pPart pointer to non-modifiable object in the list of contributing objects
     */
    size_t findIndex(const IParticle* pPart) const;
    /*!@}*/

    /*! @name List management and information */
    /*!@{*/
    /*! @brief [Deprecated] Update all internally used ElementLink instances 
     * 
     *  @note This method scans the full list of contributing objects. It invokes
     *        MissingETComponent_v1::updateMETLink internally.
     */
    // void updateLinks();
    /*! @brief [Deprecated] Update link MissingET object only */
    // void updateMETLink();
    size_t size() const;
    /*! @brief Empty list of contributing objects indicator */
    bool empty() const;
    /*!@}*/

    /*! @name Comparators
     *
     *  @param[in] contrib reference to non-modifiable contribution object to compare to. 
     */
    /*!@{*/
    bool operator==(const MissingETComponent_v1& contrib) const; /*! @brief Equality */
    bool operator!=(const MissingETComponent_v1& contrib) const;
    /*!@}*/

  protected:
    /// Function initialising the object to work in standalone mode
    void createPrivateStore();

    /*! @name Internal non-const accessors 
     *
     *  The underlying data model uses the auxiliary store MissingETAuxComposition_v1, which only employs low level data structures and ElementLinks. The accessors
     *  forward their specific request to static accessors, which retrieve the data by internally used names. 
     */
    /*!@{*/
    MissingETBase::Types::objlink_vector_t&  f_objectLinks(); /*!< @brief Non-const link reference */
    MissingETBase::Types::metlink_t&         f_metLink();     /*!< @brief Non-const link reference  */
    std::vector<double>&                     f_wpx();           
    std::vector<double>&                     f_wpy();
    std::vector<double>&                     f_wet();
    MissingETBase::Types::bitmask_t&         f_statusWord();
    /*!@}*/

    /*! @name Helpers */
    /*!@{*/
    /*! @brief Copy the contribution data from a source
     *     *  @param[in] contrib reference to non-modifiable contribution object (the source). 
     */
    bool f_removeContrib(size_t pIdx);
    void copyData(const MissingETComponent_v1& compDescr,MissingETBase::Types::bitmask_t sw=MissingETBase::Status::clearedStatus());
    /*! @brief Generalized link manager
     *
     *  Inserts link data into ElementLink, even if incomplete. At least, the pointer of linked object will be set (except if NULL). 
     *
     *  @tparam POBJ linked object type
     *  @tparam LINK specific ElementLink type
     *
     *  @param[in] pObj     pointer to non-modifiable object to be linked
     *  @param[in] elemLink reference to modifiable ElementLink object. Initial data content of this object are overwritten.
     *
     *  @note This is the generalized implementation for e.g. MissingETComponent_v1::setMET and MissingETComponent_v1::addObject
     */
    template<class POBJ,class LINK> void f_setObject(const POBJ* pObj,LINK& elemLink);
    template<class LINK> bool f_setLink(LINK& elemLink);
    /*!@}*/
  private:
    /*! @name Cache remembering the previous search result */
    /*!@{*/
    mutable const IParticle* m_lastObjectPointer; /*!< @brief Pointer to last searched object */
    mutable size_t           m_lastObjectIndex;   /*!< @brief Index of last searched object in list */
    /*!@}*/
  }; // MissingETComponenet_v1
}

namespace MissingETBase
{ 
  namespace Types
  {
    typedef xAOD::MissingETComponent_v1::Weight weight_t;        /*!< @brief Type for kinematic weight */
    typedef std::vector<weight_t>               weight_vector_t; /*!< @brief Vector type for kinematic weight containers */
  }
}

/*! @file MissingETComponent_v1.h */
/*! @name Global namespace operators for MissingETComponent_v1::Weight */
/*!@{*/
/*! @brief Add kinematic weights */
MissingETBase::Types::weight_t operator+(const MissingETBase::Types::weight_t& w0,const MissingETBase::Types::weight_t& w1);
/*! @brief Subtract kinematic weights */
MissingETBase::Types::weight_t operator-(const MissingETBase::Types::weight_t& w0,const MissingETBase::Types::weight_t& w1);
/*! @brief Multiply kinematic weights */
MissingETBase::Types::weight_t operator*(const MissingETBase::Types::weight_t& w0,const MissingETBase::Types::weight_t& w1);
/*! @brief Divide kinematic weights */
MissingETBase::Types::weight_t operator/(const MissingETBase::Types::weight_t& w0,const MissingETBase::Types::weight_t& w1);
/*! @brief Scale kinematic weight by multiplication */
MissingETBase::Types::weight_t operator*(const MissingETBase::Types::weight_t& wght,double scale);
/*! @brief Scale kinematic weight by multiplication */
MissingETBase::Types::weight_t operator*(double scale,const MissingETBase::Types::weight_t& wght);
/*! @brief Scale kinematic weight by division */
MissingETBase::Types::weight_t operator/(const MissingETBase::Types::weight_t& wght,double scale);
/*!@}*/
#include "xAODMissingET/versions/MissingETComponent_v1.icc"
#endif
