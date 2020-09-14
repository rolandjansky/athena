///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCLUSTERCONTAINER_H
#define CALOEVENT_CALOCLUSTERCONTAINER_H

/********************************************************************

NAME:     CaloClusterContainer.h
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma
CREATED:  May 2001

PURPOSE:  Container for CaloCluster objects
          Baseclass: ObjectVector<CaloCluster> 
	 
********************************************************************/

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloCluster.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "CxxUtils/checker_macros.h"


DATAVECTOR_VIRTBASES1 (CaloCluster, INavigable4Momentum);

class CaloClusterContainer : public DataVector<CaloCluster> 
{

 public:
  // constructor 
  /*! \brief Main constructor */
  CaloClusterContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) ;

  /*! \brief Default constructor */
  //  CaloClusterContainer();

  /*! \brief Optional constructor */
  CaloClusterContainer(const CaloTowerSeg& towerSeg); 

  /*! \brief Optional constructor */
  CaloClusterContainer(const std::string& RoiAuthor); 


  /*! \brief Optional constructor */
  CaloClusterContainer(const CaloTowerSeg& towerSeg, const std::string& RoiAuthor); 

  /*! \brief Default destructor */
  virtual ~CaloClusterContainer();

  /*! \brief Set tower segmentation into CaloClusterContainer */
  virtual void setTowerSeg(const CaloTowerSeg& towerSeg);

  /*! \brief Retrieve tower segmentation */
  virtual const CaloTowerSeg& getTowerSeg() const;

  /*! \brief Set ROI ID and author into CaloClusterContainer */
  virtual void setROIAuthor(const std::string& RoiAuthor);

  /*! \brief Retrieve ROI ID and author  */
  virtual const std::string& getROIAuthor() const;

  /*! \brief Print size (probably obsolete)
   *
   *  \attention Trivial implementation missing use of message service is
   *             probably obsolete.
   */
  void print();


 private:

  CaloTowerSeg m_towerSeg;

  /* ROI ID and author algorithm for trigger use */
  std::string m_ROIauthor;

};

CLASS_DEF(CaloClusterContainer, 2806, 1)
SG_BASE(CaloClusterContainer, DataVector<CaloCluster> );

/*! \class CaloClusterContainer
 *
 *  \brief Storable container for \a CaloCluster
 *  
 *  \author  unknown (send mail to loch@physics.arizona.edu)
 *  \date    April 1, 2005
 *  \version 0
 */

inline void CaloClusterContainer::setTowerSeg(const CaloTowerSeg& towerSeg)
{ m_towerSeg = towerSeg; }

inline const CaloTowerSeg& CaloClusterContainer::getTowerSeg() const
{ return m_towerSeg; }

inline void CaloClusterContainer::setROIAuthor(const std::string& RoiAuthor)
{ m_ROIauthor = RoiAuthor; }

inline const std::string& CaloClusterContainer::getROIAuthor() const
{ return m_ROIauthor; }
#endif
