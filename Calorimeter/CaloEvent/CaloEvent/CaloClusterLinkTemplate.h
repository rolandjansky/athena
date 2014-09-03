/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOCLUSTERLINKTEMPLATE_H
#define CALOEVENT_CALOCLUSTERLINKTEMPLATE_H

#include "DataModel/ElementLink.h"

#include <algorithm>
#include <iostream>
//#include <typeinfo>


/** @class CaloClusterLinkTemplate
 *  @brief template class for CaloClusterLink     
 */

template<class CONT,
	 class DOBJ=typename CONT::base_value_type,
	 class ELEM=ElementLink<CONT> >
struct CaloClusterLinkTemplate
{
  typedef ELEM  link_type;
  
  static bool setLink(CONT* pCont, DOBJ* pDat, ELEM& rElm)
  {
    //  std::cout << "[LinkHandler] invoked for container type \042"
    //      << std::type_id(pCont).name() << "\042 and data object type \042"
    //      << std::type_id(pDat).name() << std::endl;
    bool checkOut(pCont != 0);
    // check input
    if ( checkOut )
      {
        // data object has valid pointer, set only storable
        if ( pDat != 0 )
          {
            pCont->push_back(pDat);
            checkOut = rElm.setStorableObject(*pCont);

	  }
        // no data object set, create complete EL
        else
          {
            DOBJ* pNewDat = new DOBJ();
            pCont->push_back(pNewDat);
            checkOut = rElm.toContainedElement(*pCont,pNewDat);
	  }
      }
    return checkOut;

  }
};
#endif
