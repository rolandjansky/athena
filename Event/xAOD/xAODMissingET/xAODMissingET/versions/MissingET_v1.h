// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGET_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGET_V1_H

#include "AthContainers/AuxElement.h"

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
    MissingET_v1(double mpx,double mpy,double sumet,const std::string& name="MissingET",
		 MissingETBase::Types::bitmask_t src=MissingETBase::Source::unknown()); /*!< @brief Constructor with initial kinematics */
    MissingET_v1(const MissingET_v1& met);                                              /*!< @brief Copy constructor */
    ~MissingET_v1();                                                                    /*!< @brief Implementation class destructor */
    /*!@}*/

    /*! @name Kinematic accessors */
    /*!@{*/
    double mpx()    const; /*!< @brief Returns @f$ p_{x}^{\rm miss} @f$ */
    double mpy()    const; /*!< @brief Returns @f$ p_{y}^{\rm miss} @f$ */
    double met()   const; /*!< @brief Returns @f$ E_{\rm T}^{\rm miss} = \sqrt{(p_{x}^{\rm miss})^{2} + (p_{y}^{\rm miss})^{2}} @f$ */
    double phi()   const; /*!< @brief Returns @f$ \phi(E_{\rm T}^{\rm miss}) @f$ */
    double sumet() const; /*!< @brief Returns */
    /*!@}*/

    /*! @name Kinematic setters */
    /*@{*/
    void setMpx(double value);    /*!< @brief Set the @f$ p_{x}^{\rm miss} @f$ component */
    void setMpy(double value);    /*!< @brief Set the @f$ p_{y}^{\rm miss} @f$ component */
    void setSumet(double value); /*!< @brief Set @f$ \Sigma E_{\rm T} @f$ */
    /*@}*/

    /*! @brief Identifier getters */
    /*!@{*/
    const std::string& name() const;                /*!< @brief MET object name */
    MissingETBase::Types::bitmask_t source() const; /*!< @brief MET object source tag */
    const std::size_t& nameHash() const;            /*!< @brief Return hash associated to the MET object name */
    /*!@}*/

    /*! @name Identifier setters */
    /*!@{*/
    void setName(const std::string& name);               /*!< @brief Set the name of the MET object */
    void setSource(MissingETBase::Types::bitmask_t src); /*!< @brief Set the source of the MET object */
    /*!@}*/

    /*! @name Manipulating the kinematic data */
    /*!@{*/
    void add(const IParticle* particle);                 /*!< @brief Add particle kinematics to MET */
    void add(const IParticle* particle,double scale);    /*!< @brief Add scaled particle kinematics to MET */
    void add(double px,double py,double pt);             /*!< @brief Add kinematic variables to MET */
    MissingET_v1& operator=(const MissingET_v1& met); /*!< @brief Add particle kinematics to MET */
    MissingET_v1& operator+=(const IParticle* particle); /*!< @brief Add particle kinematics to MET */
    MissingET_v1& operator-=(const IParticle* part);     /*!< @brief Remove particle kinematics to MET */
    /*!@}*/

    /*! @name MET object algebra */
    /*!@{*/
    MissingET_v1& operator+=(const MissingET_v1& met); /*!< @brief Add another MET object */
    MissingET_v1& operator-=(const MissingET_v1& met); /*!< @brief Subtract another MET object */
    MissingET_v1& operator*=(double scale);            /*!< @brief Applying a scale factor */
    MissingET_v1& operator/=(double scale);            /*!< @brief Dividing by a scale factor */
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
    double&                           f_mpx();     /*!< @brief Returns reference to @f$ p_{x} @f$ store */
    double&                           f_mpy();     /*!< @brief Returns reference to @f$ p_{y} @f$ store */
    double&                           f_sumet();  /*!< @brief Returns reference to @f$ \Sigma E_{\rm T} @f$ store */
    std::string&                      f_name();   /*!< @brief Returns reference to MET object name store */
    MissingETBase::Types::bitmask_t&  f_source(); /*!< @brief Returns reference to MET object source store */
    /*!@}*/

  private:
    mutable std::pair< std::string , std::size_t> m_nameHashPair;/*!< @brief Stores the hash function for the current f_name string */
  };
}

/*! @name Global namespace operators
 *  @file MissingET_v1
 */
/*!@{*/
xAOD::MissingET_v1 operator+(const xAOD::MissingET_v1& met0,const xAOD::MissingET_v1& met1); /*!< @brief Create new MET object from sum of two MissingET_v1 objects */
xAOD::MissingET_v1 operator-(const xAOD::MissingET_v1& met0,const xAOD::MissingET_v1& met1); /*!< @brief Create new MET object from difference between two MissingET_v1 objects */
xAOD::MissingET_v1 operator*(const xAOD::MissingET_v1& met,double scale);                    /*!< @brief Create new MET object from source with scaled (weighted) kinematics */
xAOD::MissingET_v1 operator*(double scale,const xAOD::MissingET_v1& met);                    /*!< @brief Create new MET object from source with scaled (weighted) kinematics */
xAOD::MissingET_v1 operator/(const xAOD::MissingET_v1& met,double scale);                    /*!< @brief Create new MET object from source with scaled kinematics */
/*!@}*/

#include "xAODMissingET/versions/MissingET_v1.icc"

#endif
