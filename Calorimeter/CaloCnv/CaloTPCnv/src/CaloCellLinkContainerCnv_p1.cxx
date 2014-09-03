/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloCellLinkContainerCnv_p1.h"

#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloTPCnv/CaloCellLinkContainer_p1.h"
     
void CaloCellLinkContainerCnv_p1::persToTrans(const CaloCellLinkContainer_p1* pers, CaloCellLinkContainer* trans, MsgStream &log) {
  trans->resize(pers->m_links.size());
  std::vector<CaloCellLinkContainer_p1::CellLink>::const_iterator it=pers->m_links.begin();
  std::vector<CaloCellLinkContainer_p1::CellLink>::const_iterator it_e=pers->m_links.end();
  CaloCellLinkContainer::size_type i=0;
  for(;it!=it_e;++it,++i) {
    CaloCellLink* lnk=new CaloCellLink;
    trans->at(i)=lnk;
    m_converter.persToTrans(&(*it),lnk,log);
  }
}


void CaloCellLinkContainerCnv_p1::transToPers(const CaloCellLinkContainer* trans, CaloCellLinkContainer_p1* pers, MsgStream &log) {
  pers->m_links.resize(trans->size());
  CaloCellLinkContainer::const_iterator it=trans->begin();
  CaloCellLinkContainer::const_iterator it_e=trans->end();
  std::vector<CaloCellLinkContainer_p1::CellLink>::size_type i=0;
  for(;it!=it_e;++it,++i) {
    //const CaloCellLink* lnk=(*it);
    //const Navigable<CaloCellContainer,double>* nav=(const Navigable<CaloCellContainer,double>*)lnk;
    //m_converter.transToPers(nav,&pers->m_links[i],log);
    m_converter.transToPers((*it),&pers->m_links[i],log);
  }
}


void CaloCellLinkContainerCnv_p1::persToTransUntyped(const void* pers,
                                                     void* trans,
                                                     MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloCellLinkContainer_p1*> (pers),
               reinterpret_cast<CaloCellLinkContainer*> (trans),
               log);
}


void CaloCellLinkContainerCnv_p1::transToPersUntyped(const void* trans,
                                                     void* pers,
                                                     MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloCellLinkContainer*> (trans),
               reinterpret_cast<CaloCellLinkContainer_p1*> (pers),
               log);
}


const std::type_info& CaloCellLinkContainerCnv_p1::transientTInfo() const
{
  return typeid (CaloCellLinkContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloCellLinkContainerCnv_p1::persistentTInfo() const
{
  return typeid (CaloCellLinkContainer_p1);
}


