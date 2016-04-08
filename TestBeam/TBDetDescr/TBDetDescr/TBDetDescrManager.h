/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==============================================
//  File TBDetDescrManager.h
//  keeps positions of all detectors at the testbeam
//=============================================
#ifndef TBDetDescrManager_H
#define TBDetDescrManager_H

#include "CLIDSvc/CLASS_DEF.h"

#include "TBDetDescr/TBElement.h"
class TBElementContainer;
class MsgStream;

#include <vector>

class TBDetDescrManager
{
 public:
  TBDetDescrManager();

  virtual ~TBDetDescrManager() { }

  // copy all elements from a vector to private vector
  void initialize(const std::vector<TBElement> & vec);

  // copy all elements from container to private vector
  void initialize(const TBElementContainer * pTBElementContainer);

  // copy all elements to a container
  void getAll(TBElementContainer * pTBElementContainer) const;
  
  // get one element
  const TBElement & getElement(TBElementID::TBElementID id) const;
  
  // re-initialize one element
  void setElement(const TBElement & element);

  bool is_initialized   (void)  const   {return (m_elements>0);}
  int  n_elements       (void)  const   {return m_elements; }
  int  max_size         (void)  const   {return m_TBElementVec.size(); }

  void print            (MsgStream& log)  const;
  
private:
  
  unsigned int m_elements; // number of non-empty elements
  std::vector<TBElement> m_TBElementVec; // TBElementID::TBElementID as index

 };

CLASS_DEF(TBDetDescrManager, 2890, 1)

#endif

