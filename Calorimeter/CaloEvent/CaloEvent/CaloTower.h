/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  CALOEVENT_CALOTOWER_H
#define  CALOEVENT_CALOTOWER_H

/********************************************************************

NAME:     CaloTower.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Rajagopalan
CREATED:  Oct 30, 2000

PURPOSE:  This is a Contained data object, containing information 
          about a Calorimeter Tower.  It contains a list of CaloCell
	  that make up the tower.  The Tower segmentation should be in	
	  the tower container. 

Update:   Jan 10 2001 HMA
          ordor of static/virtual/const 

Update:   Feb 5, 2001 HMA
	  Made it to be a concrete class.  
	  LAr and Tile should both instantiate the CaloTower for tower
	  objects. 

Update:   Jan 24, 2003 P Loch
          Keep the weights associated with CaloCells. 
          Implement getWeight() access method.
          Navigation model update.

Update:   Feb 17, 2004 P Loch
          Complete overhaul: now inherits from CaloEnergyCluster, a 
          navigable 4-vector. Most interfaces changed to adopt navigation.

********************************************************************/

#include "Navigation/Navigable.h"

#include "EventKernel/INavigable4Momentum.h"

#include "CaloEvent/CaloEnergyCluster.h"

//#include "CaloEvent/CaloCompositeKineBase.h"
//#include "CaloEvent/CaloCompositeCellBase.h"
#include "CaloEvent/CaloCellContainer.h"

class CaloTower : public CaloEnergyCluster
		  // class CaloTower : 
		  // public CaloCompositeKineBase,
		  //  public CaloCompositeCellBase< Navigable<CaloCellContainer,double> >,
		  //  virtual public INavigable4Momentum
{

public:

  //////////////////
  // Public Types //
  //////////////////

  /*! \brief Iterator on \a CaloCell s */
  typedef CaloEnergyCluster::cell_iterator cell_iterator;
  //  typedef 
  //  CaloCompositeCellBase< Navigable<CaloCellContainer,double> >::cell_iterator
  //  cell_iterator;

  ////////////////////////////////
  // Constructors & Destructors //
  ////////////////////////////////

  /*! \brief Default constructor
   *
   *  Instantiates an empty tower without signal and location.
   */
  CaloTower(); 

  /*! \brief Alternate constructor
   *
   *  \param eta  input central \f$ \eta \f$ of tower
   *  \param phi  input central \f$ \varphi \f$ of tower
   *
   *  Instantiates an empty tower without signal at a fixed location.
   */
  CaloTower(double eta, double phi); 

  /*! \brief Destructor */
  virtual ~CaloTower() override;

  ///////////////////////
  // Kinematics Access //
  ///////////////////////

  /*! \brief Basic signal getter */
  virtual double getBasicEnergy() const override;

  /*! \brief Basic signal setter */
  virtual void setBasicEnergy(double theEnergy) override;


  // Redeclare these methods as final, so they can be inlined.

  /** get energy data member */
  virtual double e() const override final
  { return CaloEnergyCluster::e(); }
  /** get energy data member */
  virtual double energy() const override final
  { return CaloEnergyCluster::energy(); }
  /** get eta data member */
  virtual double eta() const override final
  { return CaloEnergyCluster::eta(); }
  /** get phi data member */
  virtual double phi() const override final
  { return CaloEnergyCluster::phi(); }
  /** get mass data member */
  virtual double m() const override final
  { return CaloEnergyCluster::m(); }

  // global in CaloEnergyCluster base class!

  ////////////////////////////
  // Constituent Management //
  ////////////////////////////

  // all in CaloEnergyCluster base class!

 protected:

  /*! Update tower kinematics when cell is added
   *
   *  \param theCell   input pointer to a \a CaloCell
   *  \param weight    input signal weight of the cell in the tower
   *
   * The cell signal is added to the tower signal. The tower directions are
   * unchanged.  
   */
  virtual void updateKine(const CaloCell* theCell, double weight) override;

 private:

};


inline
CaloTower::CaloTower( ) 
{
  this->setEta(0);
  this->setPhi(0);
}


inline
CaloTower::CaloTower(double eta, double phi) 
{
  this->setEta(eta);
  this->setPhi(phi);
}


/*! \class CaloTower
 *
 *  \brief Data class for calorimeter cell towers
 *
 *  \author S. Rajagopalan <srinir@bnl.gov>
 *  \author H. Ma <hma@bnl.gov>
 *  \author P. Loch <loch@physics.arizona.edu>
 *  \date April 1, 2005
 *  \version 2
 *
 *  \a CaloTower stores a list of cells which are at least partially in the
 *  same \f$ ( \eta, \varphi ) \f$ bin. These bins are defined externally in
 *  the \a CaloTowerContainer . All public behaviour other than the constructor
 *  is implemented in the \a CaloEnergyCluster base class.
 */
#endif // CALOTOWER_H







