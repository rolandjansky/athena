// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGET_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGET_V1_H

#include "AthContainers/AuxElement.h"

#include "CxxUtils/CachedValue.h"

#include "xAODBase/IParticle.h"
#include "xAODMissingET/versions/MissingETBase.h"

#include <string>
#include <functional>
#include <utility>

namespace xAOD
{
  /*! @brief Principal data object for Missing ET */
  class MissingET_v1 : public SG::AuxElement
  {
  public:

    /*! @name Constructors and destructor */
    /*!@{*/
    explicit MissingET_v1(bool createStore=false);  /*!< @brief Default constructor */
    MissingET_v1(const std::string& name,
		 MissingETBase::Types::bitmask_t src=MissingETBase::Source::unknown()); /*!< @brief Constructor with identifiers */
    MissingET_v1(const IParticle* particle, const std::string& name="MissingET",
		 MissingETBase::Types::bitmask_t src=MissingETBase::Source::unknown()); /*!< @brief Constructor with particle */
    MissingET_v1(float mpx,float mpy,float sumet,const std::string& name="MissingET",
		 MissingETBase::Types::bitmask_t src=MissingETBase::Source::unknown()); /*!< @brief Constructor with initial kinematics */
    MissingET_v1(const MissingET_v1& met);                                              /*!< @brief Copy constructor */
    ~MissingET_v1();                                                                    /*!< @brief Implementation class destructor */
    /*!@}*/

    /*! @name Kinematic accessors */
    /*!@{*/
    float mpx()    const; /*!< @brief Returns @f$ p_{x}^{\rm miss} @f$ */
    float mpy()    const; /*!< @brief Returns @f$ p_{y}^{\rm miss} @f$ */
    float met()   const; /*!< @brief Returns @f$ E_{\rm T}^{\rm miss} = \sqrt{(p_{x}^{\rm miss})^{2} + (p_{y}^{\rm miss})^{2}} @f$ */
    float phi()   const; /*!< @brief Returns @f$ \phi(E_{\rm T}^{\rm miss}) @f$ */
    float sumet() const; /*!< @brief Returns */
    /*!@}*/

    /*! @name Kinematic setters */
    /*@{*/
    void setMpx(float value);    /*!< @brief Set the @f$ p_{x}^{\rm miss} @f$ component */
    void setMpy(float value);    /*!< @brief Set the @f$ p_{y}^{\rm miss} @f$ component */
    void setSumet(float value); /*!< @brief Set @f$ \Sigma E_{\rm T} @f$ */
    /*@}*/

    /*! @brief Identifier getters */
    /*!@{*/
    const std::string& name() const;                /*!< @brief MET object name */
    MissingETBase::Types::bitmask_t source() const; /*!< @brief MET object source tag */
    std::size_t nameHash() const;                   /*!< @brief Return hash associated to the MET object name */
    /*!@}*/

    /*! @name Identifier setters */
    /*!@{*/
    void setName(const std::string& name);               /*!< @brief Set the name of the MET object */
    void setSource(MissingETBase::Types::bitmask_t src); /*!< @brief Set the source of the MET object */
    void updateHash();                                   /*!< @brief Update the hash to match the current name */
    /*!@}*/

    /*! @name Manipulating the kinematic data */
    /*!@{*/
    void add(const IParticle* particle);                 /*!< @brief Add particle kinematics to MET */
    void add(const IParticle* particle,float scale);    /*!< @brief Add scaled particle kinematics to MET */
    void add(float px,float py,float pt);             /*!< @brief Add kinematic variables to MET */
    MissingET_v1& operator=(const MissingET_v1& met); /*!< @brief Add particle kinematics to MET */
    MissingET_v1& operator+=(const IParticle* particle); /*!< @brief Add particle kinematics to MET */
    MissingET_v1& operator-=(const IParticle* part);     /*!< @brief Remove particle kinematics to MET */
    /*!@}*/

    /*! @name MET object algebra */
    /*!@{*/
    MissingET_v1& operator+=(const MissingET_v1& met); /*!< @brief Add another MET object */
    MissingET_v1& operator-=(const MissingET_v1& met); /*!< @brief Subtract another MET object */
    MissingET_v1& operator*=(float scale);            /*!< @brief Applying a scale factor */
    MissingET_v1& operator/=(float scale);            /*!< @brief Dividing by a scale factor */
    /*!@}*/

    /*! @name Stored data management */
    /*!@{*/
    void clear(); /*!< @brief Clear/reset the data store */
    /*!@}*/

    /*! @name Comparators */
    /*!@{*/
    bool operator==(const MissingET_v1& met) const; /*!< @brief Equality test. */
    bool operator!=(const MissingET_v1& met) const; /*!< @brief Inequality test. */
    /*!@}*/



  protected:
    /// Function initialising the object to work in standalone mode
    void createPrivateStore();

    /*! @name Internal data modification support */
    /*!@{*/
    float&                           f_mpx();     /*!< @brief Returns reference to @f$ p_{x} @f$ store */
    float&                           f_mpy();     /*!< @brief Returns reference to @f$ p_{y} @f$ store */
    float&                           f_sumet();  /*!< @brief Returns reference to @f$ \Sigma E_{\rm T} @f$ store */
    std::string&                      f_name();   /*!< @brief Returns reference to MET object name store */
    const std::string&                f_nameConst() const; /*!< @brief Returns const reference to the MET object name store*/
    MissingETBase::Types::bitmask_t&  f_source(); /*!< @brief Returns reference to MET object source store */
    /*!@}*/

  private:
    CxxUtils::CachedValue<std::size_t> m_nameHash; /*!< @brief Stores the hash for the current f_name string */

    static const SG::AuxElement::Accessor<float> m_acc_mpx;
    static const SG::AuxElement::Accessor<float> m_acc_mpy;
    static const SG::AuxElement::Accessor<float> m_acc_sumet;
  };
}

/*! @name Global namespace operators
 *  @file MissingET_v1
 */
/*!@{*/
xAOD::MissingET_v1 operator+(const xAOD::MissingET_v1& met0,const xAOD::MissingET_v1& met1); /*!< @brief Create new MET object from sum of two MissingET_v1 objects */
xAOD::MissingET_v1 operator-(const xAOD::MissingET_v1& met0,const xAOD::MissingET_v1& met1); /*!< @brief Create new MET object from difference between two MissingET_v1 objects */
xAOD::MissingET_v1 operator*(const xAOD::MissingET_v1& met,float scale);                    /*!< @brief Create new MET object from source with scaled (weighted) kinematics */
xAOD::MissingET_v1 operator*(float scale,const xAOD::MissingET_v1& met);                    /*!< @brief Create new MET object from source with scaled (weighted) kinematics */
xAOD::MissingET_v1 operator/(const xAOD::MissingET_v1& met,float scale);                    /*!< @brief Create new MET object from source with scaled kinematics */
/*!@}*/

#include "xAODMissingET/versions/MissingET_v1.icc"

#endif
