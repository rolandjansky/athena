/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXSTOREGATEEXAMPLE_LINKOBJ_H
#define ATHEXSTOREGATEEXAMPLE_LINKOBJ_H

#include <vector>

#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "MyDataObj.h"
#include "MyElement.h"

//sample data class for the Read/Write example
//it just wraps an int. Notice that is does not inherit from Gaudi DataObject

namespace SGTutorial {

  class LinkObj {
 
  public: 
 
    LinkObj() {}
    ~LinkObj(){}

    // Set links to either an object or a collection

    // Link to a data object
    void setObjLink(MyDataObj&); 

    // Link to an element of a collection
    void setContLink(std::vector<MyElement>&,
		     const MyElement&);

    // Link to an element of a collection using an index
    void setContLink(std::vector<MyElement>&,
		     int);

    // Access linked object/collection
    const MyDataObj& myLinkedObj();
    const MyElement& myLinkedElement();

    template <class OST>
    friend OST& operator<<(OST& ost, LinkObj& rhs);

  private:
    DataLink<MyDataObj> m_objLink;
    ElementLink< std::vector<MyElement> > m_contLink;
  
  };
#include <iostream>
  template <class OST>
  inline OST& operator<<(OST& ost, LinkObj& rhs) {
    ost << "LinkObj: link to MyDataObj -> " << rhs.myLinkedObj() 
	<< "\n         link to MyElement[" << rhs.m_contLink.index() << "] -> " 
	<< *rhs.m_contLink;
    return ost;
  }

}  //end ns SGTutorial
//using the macros below we can assign an identifier (and a version) 
//to the type LinkObj
//This is required and checked at compile time when you try to record/retrieve
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF(SGTutorial::LinkObj, 9909, 1)
//class version is not currently used

#endif


