/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeEvent/EventShapeStore.h"

bool EventShapeStore::get(const EventShapes::EventShapeType& type, double& value, unsigned int index) const
{
  value=0;
  for(store_Citer it=m_store.begin(); it!=m_store.end(); ++it )
    if(it->getType()==type)
      {
         value=it->getValue(index);
         return true;
      }
  return false;
}

// bool EventShapeStore::get(const EventShapes::EventShapeType& type, 
// 			  std::vector<double>& values, unsigned int index) const
// {
//   value.clear();;
//   for(store_Citer it=m_store.begin(); it!=m_store.end(); ++it )
//     if(it->getType()==type)
//       {
// 	values.push_back(it->getValue(index));
//       }
//   return !values.empty();
// }

// bool EventShapeStore::get(const EventShapes::EventShapeType& type,
// 			  std::vector<std::vector<double> >& valueBlock)
// {
//   valueBlock.clear();
//   for(store_Citer it=m_store.begin(); it!=m_store.end(); ++it )
//     if(it->getType()==type)
//       {
// 	values.push_back(std::vector<double>());
// 	it->getValues(values.back());
//       }
//   return !values.empty();
// }


