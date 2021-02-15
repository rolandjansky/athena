/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "Navigation/Navigable.h"

#include "FourMom/P4EEtaPhiM.h"

#include "CaloEvent/CaloRecoStatus.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloEnergyCluster.h"

#include <iostream>

CaloEnergyCluster::CaloEnergyCluster() 
  : Navigable<CaloCellContainer,double>()
  , P4EEtaPhiM(0.,0.,0.,0.)
  , m_status(CaloRecoStatus())
{ }

CaloEnergyCluster::~CaloEnergyCluster()
{ }

void
CaloEnergyCluster::addCell(const CaloCellContainer* theContainer,
			   const CaloCell* /*const_cell_ptr*/ theCell,
			   double weight)
{
  // cell already in collection
  if ( this->contains(theCell) )
    {
      // update weight
      double newWeight = weight + this->getCellWeight(theCell);
      // reweight the cell
      this->reweightCell(theCell,newWeight);
    }
  else
    {
      // from Navigable base
      this->insertElement(theContainer,theCell,weight);
      // update kinematics
      this->updateKine(theCell,weight);
    }
}

void
CaloEnergyCluster::addCell(const CaloCellContainer* theContainer,
			   index_type theIndex,
			   double weight)
{
  if ((int)theIndex < 0) abort();

  // cell already in collection
  if ( this->contains(theContainer,theIndex) )
    {
      //      try 
      //	{
      //	  //      std::cout << "Replace CaloCell @" 
      //	  //		<< (theContainer->operator[])(theIndex)
      //	  //		<< " index " << theIndex
      //	  //		<< " with energy "
      //	  //		<< ((theContainer->operator[])(theIndex))->e()
      //	  //		<< std::endl;
      //	  // update weight
      double newWeight = 
	weight + this->getCellWeight(theContainer,theIndex);
      // change weight
      this->reweightCell(theContainer,theIndex,newWeight);
      //	} catch(...) {
      //	std::cout << "     Object [" 
      //		  << (typeid(*this)).name()
      //		  << "] failed to insert cell @"
      //		  << (theContainer->operator[])(theIndex)
      //		  << " in container @"
      //		  << theContainer
      //		  << " with index <"
      //		  << theIndex
      //		  << ">" << std::endl;
      //	this->dumpStore();
      //      }
    }
  else
    {
      // from Navigable base
      //      try
      //	{
	  this->insertElement(theContainer,theIndex,weight);
	  //	  std::cout << "Insert CaloCell @" 
	  //		<< (theContainer->operator[])(theIndex)
	  //		<< " index " << theIndex
	  //		<< " with energy "
	  //		<< ((theContainer->operator[])(theIndex))->e()
	  //		<< std::endl;
	  // update kinematics
	  this->updateKine((theContainer->operator[])(theIndex),weight);
	  //	} catch(...) {
	  //	std::cout << "     Object [" 
	  //		  << (typeid(*this)).name()
	  //		  << "] failed to insert cell @"
	  //		  << (theContainer->operator[])(theIndex)
	  //		  << " in container @"
	  //		  << theContainer
	  //		  << " with index <"
	  //		  << theIndex
	  //	  << ">" << std::endl;
	  //	this->dumpStore();
    }
  //  this->dumpStore();
}

void
CaloEnergyCluster::addUniqueCell(const CaloCellContainer* theContainer,
                                 index_type theIndex,
                                 double weight,
                                 size_t size_hint)
{
  if ((int)theIndex < 0) abort();
  this->insertElement(theContainer,theIndex,weight,size_hint);
  this->updateKine((theContainer->operator[])(theIndex),weight);
}

void
CaloEnergyCluster::addUniqueCellNoKine(const CaloCellContainer* theContainer,
                                 index_type theIndex,
                                 double weight,
                                 size_t size_hint)
{
  if ((int)theIndex < 0) abort();
  this->insertElement(theContainer,theIndex,weight,size_hint);
}


void
CaloEnergyCluster::addUniqueCellNoKine(const CaloCellContainer* theContainer,
                                       index_type theIndex,
                                       IProxyDict* sg,
                                       double weight,
                                       size_t size_hint)
{
  if ((int)theIndex < 0) abort();
  this->insertElement(theContainer,theIndex,sg,weight,size_hint);
}


void
CaloEnergyCluster::addUniqueCellNoKine(const ElementLink<CaloCellContainer>& theContainer,
                                       index_type theIndex,
                                       double weight,
                                       size_t size_hint)
{
  if ((int)theIndex < 0) abort();
  ElementLink<CaloCellContainer> el (theContainer, theIndex);
  this->insertElement(el,weight,size_hint);
}


void 
CaloEnergyCluster::removeCell(/*CaloCellContainer::const_reference*/
			      const CaloCell* theCell)
{
  if ( this->contains(theCell) )
    {
      // update kinematics
      this->updateKine(theCell,
		       -(this->getCellWeight(theCell)));
      // from Navigable base
      this->remove(theCell);
    }
}

void
CaloEnergyCluster::removeCell(const CaloCellContainer* theContainer,
			      index_type& theIndex)
{
  if ( this->contains(theContainer,theIndex) )
    {
      // update kinematics
      this->updateKine((*theContainer)[theIndex],
		       -(this->getCellWeight(theContainer,theIndex)));
 
      // from Navigable
      this->remove(theContainer,theIndex);
    }
}

void
CaloEnergyCluster::removeCells()
{
  this->removeAll();
}

void 
CaloEnergyCluster::reweightCell(const CaloCell* pCell, double weight)
{
  double adjWeight = weight - this->getCellWeight(pCell);
  this->reweight(pCell,weight);
  this->updateKine(pCell,adjWeight);
}

void 
CaloEnergyCluster::reweightCell(const CaloCellContainer* pContainer,
				index_type& refIndex,
				double weight)
{
  this->reweightCell((pContainer->operator[])(refIndex),weight);
  //  double adjWeight = weight - this->getCellWeight(pContainer,refIndex);
  //  this->reweight(pContainer,refIndex,weight);
  //  this->updateKine((pContainer->operator[])(refIndex),adjWeight);
}

void
CaloEnergyCluster::reweightCell(cell_iterator& cellIter,double weight)
{
  // keep this implementation separated (faster!)
  double adjWeight = weight - this->getCellWeight(cellIter);
  this->reweight(cellIter,weight);
  this->updateKine(*cellIter,adjWeight);
}

double
CaloEnergyCluster::getCellWeight(/*CaloCellContainer::const_reference*/
				 const CaloCell* theCell) const
{
  // from Navigable
  return this->contains(theCell) 
    ? this->getParameter(theCell)
    : 0.;
}

double
CaloEnergyCluster::getCellWeight(const CaloCellContainer* theContainer,
				 index_type& theIndex) const
{
  // from Navigable
  return this->contains(theContainer,theIndex)
    ? this->getParameter(theContainer,theIndex)
    : 0.;
}

double
CaloEnergyCluster::getCellWeight(cell_iterator& iterCell) const
{
  return this->getParameter(iterCell);
}

const CaloCellContainer*
CaloEnergyCluster::getCellContainer(/*CaloCellContainer::const_reference*/
				    const CaloCell*
				    theCell) const
{
  return this->getContainer(theCell);
}

bool
CaloEnergyCluster::getCellIndex(const CaloCell* theCell,
				index_type& theIndex) const
{
  // from Navigable
  return this->getIndex(theCell,theIndex);
}

unsigned int
CaloEnergyCluster::getNumberOfCells() const
{
  return this->size();
}

CaloEnergyCluster::cell_iterator CaloEnergyCluster::cell_begin() const
{
  return this->begin();
}

CaloEnergyCluster::cell_iterator CaloEnergyCluster::cell_end()   const 
{
  return this->end();
}

void CaloEnergyCluster::setRecoStatus(CaloRecoStatus::StatusIndicator 
				      recStatus)
{
  m_status = recStatus;
}

const CaloRecoStatus& CaloEnergyCluster::getRecoStatus() const
{
  return m_status;
}
