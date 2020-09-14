/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCLUSTERNAVIGABLE_H 
#define CALOEVENT_CALOCLUSTERNAVIGABLE_H 

/** 
   @class CaloClusterNavigable
   @brief Navigable part of CaloCluster, use CaloCellLink

   @author Peter Loch
*/

#include "AthLinks/ElementLink.h"

#include "Navigation/INavigable.h"

#include "CaloEvent/CaloClusterLinkTemplate.h"

#include "CaloEvent/CaloCell.h" 
#include "CaloEvent/CaloCellContainer.h"

#include "CaloEvent/CaloCellLink.h"
#include "CaloEvent/CaloCellLinkContainer.h"

#include <algorithm>

#include "GeoPrimitives/GeoPrimitives.h"
#include "CxxUtils/checker_macros.h"


class CaloClusterNavigable : virtual public INavigable
{
 public:

  /** \brief default constructor */ 
  CaloClusterNavigable();

  /** \brief constructor */ 
  CaloClusterNavigable(CaloCellLinkContainer* pLinkCont,
		    CaloCellLink*          pLink);

  /** \brief constructor */ 
  CaloClusterNavigable(CaloCellLink* pLink);

  /** \brief destructor */ 
  virtual ~CaloClusterNavigable();

  /** \brief external cell iterator type */
  typedef CaloCellLink::cell_iterator                   object_iter;

  /**  \brief add element to the Navigable: use pointer */
  void putElement(const CaloCellContainer* pContainer,
		  const CaloCell*          pCell,
		  const double&            weight=double(1.));
  /**  \brief add element to the Navigable: use element index (direct access!)*/
  void putElement(const CaloCellContainer* pContainer,
		  size_t&                  iCell,
		  const double&            weight=double(1.));
  /** insert element without checking if already in store: use element pointer */
  void insertElement(const CaloCellContainer* pContainer,
		     const CaloCell*          pCell,
		     const double&            weight=double(1.));
  /**  \brief insert element without checking if already in store: use index */
  void insertElement(const CaloCellContainer* pContainer,
		     size_t&                  iCell,
		     const double&            weight=double(1.));
  /** \brief same, with a size hint. */ 
  void insertElement(const CaloCellContainer* pContainer,
		     size_t                   iCell,
		     const double             weight,
                     size_t                   size_hint);
  /** \brief Insert element via ElementLink. */ 
  void insertElement(const ElementLink<CaloCellContainer>& el,
		     const double             weight,
                     size_t                   size_hint = 0);

  /** \brief re-weight (overwrite old weight) */
  void reweight(const CaloCell* pCell,const double& weight=double(1.));

  /** \brief re-weight (overwrite old weight) */
  void reweight(const CaloCellContainer* pContainer, size_t& iCell,
		const double& weight=double(1.));

  /** \brief re-weight (overwrite old weight) */
  void reweight(object_iter& cellIter, const double& weight=double(1.));


  /** \brief remove object from the Navigable */
  bool remove(const CaloCell* pCell);

  /** \brief remove object from the Navigable */
  bool remove(const CaloCellContainer* pContainer,size_t& iCell);

  /** \brief remove all objects from the Navigable */
  bool removeAll();

  /** \brief check if constituent is already there  */
  bool contains(const CaloCell* pCell) const;

  /** \brief check if constituent is already there  */
  bool contains(const CaloCellContainer* pContainer, size_t& iCell) const;

  // find constituent
  //  object_iter find(const CaloCell* pCell) const;
  //  object_iter find(const CaloCellContainer* pContainer, size_t& iCell) const;

  ////////////////////////////
  // Access to Constituents //
  ////////////////////////////

  /** \brief begin iterator for public object access */
  virtual object_iter begin()  const;    
  /** \brief end iterator for public object access */
  virtual object_iter end()    const;
  /** \brief size of object access */
  virtual unsigned int nCells()  const;

  /** \brief public container access: relational parameter retrieval */
  double getParameter(const CaloCell* pCell) const;
  /** \brief public container access: relational parameter retrieval */
  double getParameter(const CaloCellContainer* pContainer,size_t& iCell) const;
  /** \brief public container access: relational parameter retrieval */
  double getParameter(object_iter& cellIter) const;

  /** \brief public container access: retrieve Container for given object pointer */
  const CaloCellContainer* getContainer(const CaloCell* pCell) const;
  /** \brief public container access: retrieve Container for given object iterator */
  const CaloCellContainer* getContainer(object_iter cellIter)   const;

  /** \brief get index of child in original container, given a pointer */ 
  bool getIndex(const CaloCell* pCell,size_t& iCell) const;
  /** \brief get index of child in original container, given an iterator */ 
  bool getIndex(object_iter cellIter,size_t& iCell) const;

  /** \brief  fill token for navigation */ 
  virtual void fillToken(INavigationToken& iToken) const;
  /** \brief  fill token for navigation */ 
  virtual void fillToken(INavigationToken& iToken,
			 const boost::any& rPar) const;

  virtual bool isCellLinkValid() const ; 

  typedef 
  CaloClusterLinkTemplate<CaloCellLinkContainer>::link_type cell_link_type;

  /*! \brief Access to underlying link. */
  const cell_link_type& cellLink() const { return m_cellLink; }

  void resetCellLink (const cell_link_type& cellLink)
  {
    m_cellLink = cellLink;
  }


protected: 

  friend class CaloClusterStoreHelper;
  friend class CaloClusterContainerCnv_p2;
  friend class CaloClusterContainerCnv_p3;
  friend class CaloClusterContainerCnv_p4;
  friend class CaloClusterContainerCnv_p5;
  friend class CaloClusterContainerCnv_p6;
  friend class CaloClusterContainerCnv_p7;
  friend class CaloClusterContainerCnvTestMakeCluster;

  CaloCellLink*        getCellLink();
  const CaloCellLink*  getCellLink() const;

  bool setCellLink(CaloCellLinkContainer* pLink);

  bool setLinkStore(CaloCellLink* pLink,bool ownStores=true);

  CaloCellLink*       getCellLinkPtr();
  const CaloCellLink* getCellLinkPtr() const;

  bool m_ownLinkStore;


private:
  /*! \brief Local pointer to cell store */
  //  CaloCellLink* m_cellLinkPointer;

  /*! \brief Link to cell links object */
  cell_link_type m_cellLink;

};

inline 
bool CaloClusterNavigable::setLinkStore(CaloCellLink* pLink,
					bool ownStores)
{
  m_cellLink.setElement(pLink);
  m_ownLinkStore = ownStores;
  //  m_cellLinkPointer = pLink;
  return *m_cellLink != 0;
}

inline 
void CaloClusterNavigable::putElement(const CaloCellContainer* pContainer,
				   const CaloCell*          pCell,
				   const double&            weight)
{
  (this->getCellLink())->putElement(pContainer,pCell,weight);
}

inline
void CaloClusterNavigable::putElement(const CaloCellContainer* pContainer,
				   size_t&                  iCell,
				   const double&            weight)
{
  (this->getCellLink())->putElement(pContainer,iCell,weight);
}

inline 
void CaloClusterNavigable::insertElement(const CaloCellContainer* pContainer,
				      const CaloCell*          pCell,
				      const double&            weight)
{
  (this->getCellLink())->insertElement(pContainer,pCell,weight);
}

inline
void CaloClusterNavigable::insertElement(const CaloCellContainer* pContainer,
				      size_t&                  iCell,
				      const double&            weight)
{
  (this->getCellLink())->insertElement(pContainer,iCell,weight);
}

inline
void CaloClusterNavigable::insertElement(const CaloCellContainer* pContainer,
				      size_t                   iCell,
                                      const double             weight,
                                      size_t                   size_hint)
{
  (this->getCellLink())->insertElement(pContainer,iCell,weight,size_hint);
}

inline
void
CaloClusterNavigable::insertElement(const ElementLink<CaloCellContainer>& el,
                                    const double             weight,
                                    size_t                   size_hint)
{
  (this->getCellLink())->insertElement(el,weight,size_hint);
}

inline
void CaloClusterNavigable::reweight(const CaloCell* pCell,const double& weight)
{
  (this->getCellLink())->reweight(pCell,weight);
}

inline
void CaloClusterNavigable::reweight(const CaloCellContainer* pContainer, 
				 size_t& iCell,
				 const double& weight)
{
  (this->getCellLink())->reweight(pContainer,iCell,weight);
}

inline
void CaloClusterNavigable::reweight(object_iter& cellIter,const double& weight)
{
  (this->getCellLink())->reweight(cellIter,weight);
}

inline
bool CaloClusterNavigable::remove(const CaloCell* pCell)
{
  return (this->getCellLink())->remove(pCell);
}

inline
bool CaloClusterNavigable::remove(const CaloCellContainer* pContainer,
			       size_t& iCell)
{
  return (this->getCellLink())->remove(pContainer,iCell);
}

inline 
bool CaloClusterNavigable::removeAll()
{
  return (this->getCellLink())->removeAll();
}

inline 
bool CaloClusterNavigable::contains(const CaloCell* pCell) const
{
  return (this->getCellLink())->contains(pCell);
}

inline
bool CaloClusterNavigable::contains(const CaloCellContainer* pContainer,
				 size_t& iCell) const
{
  return (this->getCellLink())->contains(pContainer,iCell);
}

inline
CaloClusterNavigable::object_iter CaloClusterNavigable::begin() const
{
  return (this->getCellLink())->begin();
}

inline
CaloClusterNavigable::object_iter CaloClusterNavigable::end() const
{
  return (this->getCellLink())->end();
}

inline
unsigned int CaloClusterNavigable::nCells() const
{

  const CaloCellLink* link = this->getCellLinkPtr();
  if (link) 
    return link->size();
  else
    return 0; 

}

inline
double CaloClusterNavigable::getParameter(const CaloCell* pCell) const
{
  return (this->getCellLink())->getParameter(pCell);
}

inline
double CaloClusterNavigable::getParameter(const CaloCellContainer* pContainer,
				       size_t& iCell) const
{
  return (this->getCellLink())->getParameter(pContainer,iCell);
}

inline
double CaloClusterNavigable::getParameter(object_iter& cellIter) const
{
  return (this->getCellLink())->getParameter(cellIter);
}

inline
const CaloCellContainer* 
CaloClusterNavigable::getContainer(const CaloCell* pCell) const
{
  return (this->getCellLink())->getContainer(pCell);
}

inline
const CaloCellContainer* 
CaloClusterNavigable::getContainer(object_iter cellIter) const
{
  return (this->getCellLink())->getContainer(cellIter);
}

inline
bool CaloClusterNavigable::getIndex(const CaloCell* pCell,size_t& iCell) const
{
  return (this->getCellLink())->getIndex(pCell,iCell);
}

inline
bool CaloClusterNavigable::getIndex(object_iter cellIter,size_t& iCell) const
{
  return (this->getCellLink())->getIndex(cellIter,iCell);
}

//inline
//CaloClusterNavigable::object_iter
//CaloClusterNavigable::find(const CaloCell* pCell) const
//{
// const CaloCellLink* pLink = this->getCellLink();
//  return pLink->find(pCell);
//}


//inline 
//CaloClusterNavigable::object_iter
//CaloClusterNavigable::find(const CaloCellContainer* pContainer,
//			size_t& iCell) const
//{
//  const CaloCellLink* pLink = this->getCellLink();
//  return pLink->find(pContainer,iCell);
//}

inline
void CaloClusterNavigable::fillToken(INavigationToken& iToken) const
{
  if ( this->getCellLink() )
    (this->getCellLink())->fillToken(iToken);
}

inline void 
CaloClusterNavigable::fillToken(INavigationToken& iToken,
			     const boost::any& rPar) const
{
  if ( this->getCellLink() )
    (this->getCellLink())->fillToken(iToken,rPar);
}

inline bool 
CaloClusterNavigable::setCellLink(CaloCellLinkContainer* pLinkStore)
{
  if (!pLinkStore) 
    return false;

  CaloCellLink* pLink = *m_cellLink != 0 
    ? const_cast<CaloCellLink*>(*m_cellLink) : (CaloCellLink*)0;

  m_ownLinkStore = !pLinkStore->ownElements();
  return CaloClusterLinkTemplate<CaloCellLinkContainer>::setLink(pLinkStore,
								 pLink,
								 m_cellLink);
}

//inline const CaloCellLink* 
//CaloClusterNavigable::getCellLink() const
//{
//  return m_cellLinkPointer != 0 ? m_cellLinkPointer : *m_cellLink;
//}

inline CaloCellLink*
CaloClusterNavigable::getCellLinkPtr() 
{ 
  CaloCellLink* ret;
  if (m_cellLink.isValid())
    ret = const_cast<CaloCellLink*>(*m_cellLink); 
  else
    ret = 0;
  return ret;

}

inline const CaloCellLink*
CaloClusterNavigable::getCellLinkPtr() const { 
  const CaloCellLink* ret;
  if (m_cellLink.isValid())
    ret = *m_cellLink; 
  else
    ret = 0;
  return ret;
}


inline
bool CaloClusterNavigable::isCellLinkValid() const
{
  return this->getCellLinkPtr() !=0;
}


#endif

