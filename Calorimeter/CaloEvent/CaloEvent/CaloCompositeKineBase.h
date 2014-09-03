/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCOMPOSITEKINEBASE_H
#define CALOEVENT_CALOCOMPOSITEKINEBASE_H

#include "FourMom/P4EEtaPhiM.h"

#include "CaloEvent/CaloRecoStatus.h"

class CaloCompositeKineBase : public P4EEtaPhiM
{ 

 public:

  /*! \brief Destructor */
  virtual ~CaloCompositeKineBase();

  /*! \brief Return energy */
  virtual double energy() const;

  /*! \brief Return energy (nonvirtual version) */
  double energy_nonvirt() const;

  /*! \brief Set the reconstruction status */
  void setRecoStatus(const CaloRecoStatus& recStatus);
  /*! \brief Set the reconstruction status indicator (recommended) */
  void setRecoStatus(const CaloRecoStatus::StatusIndicator& recIndic);
  /*! \brief Check if a given indicator is set */
  bool checkRecoStatus(const CaloRecoStatus::StatusIndicator& 
                       recIndic) const;
  /*! \brief Remove a given indicator */
  void removeRecoStatus(const CaloRecoStatus::StatusIndicator& 
                        recIndic);
  /*! \brief Remove all indicators */ 
  void removeRecoStatus();
  /*! \brief Retrieve the reconstruction status */
  const CaloRecoStatus& getRecoStatus() const;


  /*! \brief Return uncalibrated energy */
  virtual double getBasicEnergy() const = 0; 
  /*! \brief Set uncalibrated energy
   *
   * \param theEnergy basic energy signal 
   */
  virtual void setBasicEnergy(double theEnergy) = 0;

 protected:
  /*! \brief Constructor
   *
   *  All kinematic variables are reset when constructed. 
   */
  CaloCompositeKineBase();

  /*! \brief Calorimeter reconstruction status */
  CaloRecoStatus m_status;
};

/*! \class CaloCompositeKineBase
 *  \brief base class for kinematics of composite calo object
 * 
 *  Abstract base class provides a complete interface to all kinematic
 *  quantities available for composite calorimeter reconstruction objects.
 *  This class basically extends the \a P4EEtaPhiM base class.
 */

inline double 
CaloCompositeKineBase::energy() const 
{ return this->e(); }

inline double 
CaloCompositeKineBase::energy_nonvirt() const 
{ return this->m_e; }

inline const CaloRecoStatus& 
CaloCompositeKineBase::getRecoStatus() const
{ return m_status; }

inline void 
CaloCompositeKineBase::setRecoStatus(const CaloRecoStatus& theStatus)
{ m_status = theStatus; }

inline void 
CaloCompositeKineBase::setRecoStatus(const CaloRecoStatus::StatusIndicator&
				     recIndic)
{ m_status.setStatus(recIndic); }

inline bool
CaloCompositeKineBase::checkRecoStatus(const CaloRecoStatus::StatusIndicator&
				       recIndic) const
{ return m_status.checkStatus(recIndic); }

inline void
CaloCompositeKineBase::removeRecoStatus(const CaloRecoStatus::StatusIndicator&
				       recIndic)
{ m_status.resetStatus(recIndic); }

inline void
CaloCompositeKineBase::removeRecoStatus()
{ m_status.resetStatus(); }
#endif
