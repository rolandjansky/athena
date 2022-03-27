/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LinkObj.h"
#include "Tutorial_ClassDEF.h"

using namespace SGTutorial;
using std::vector;

//sample data class for the Link example

  // Set links to an object
void LinkObj::setObjLink(MyDataObj&)  //dobj
{
  // Link to a data object
  // set it to refer to its target
} 
  // Set links to an element of a collection
void LinkObj::setContLink(vector<MyElement>&,    //coll
			  const MyElement&)      //cobj
{
  // Link to an element of a collection
  // set it to refer to its target

  // Here we pass the element to the DataLink and it figures out the
  // index... which locates the element in the collection.
}

void LinkObj::setContLink(vector<MyElement>&,  //coll
			  int)                 //index
{
  // Link to an element of a collection
  // set it to refer to its target

  // Faster method: we pass the index directly

}
  // Access linked object/collection
const MyDataObj& LinkObj::myLinkedObj()
{
  static const MyDataObj replaceMe;
  return replaceMe;
}

const MyElement& LinkObj::myLinkedElement() 
{
  static const MyElement replaceMe;
  return replaceMe;
  
}

