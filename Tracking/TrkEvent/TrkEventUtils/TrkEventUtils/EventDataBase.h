/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _InDet_EventDataBase_H_
#define _InDet_EventDataBase_H_

#include <stdexcept>
#include <string>
#include <typeinfo>

namespace Trk {

   /** Base for a helper class to pass mutable storage to tools.
    */
   template <class T_Derived, class T_Base>
   class EventDataBase : public T_Base
   {
   protected:
      inline static T_Derived &getPrivateEventData(T_Base &virt_event_data)
      {
         T_Derived *event_data = dynamic_cast<T_Derived *>(&virt_event_data);
         if (!event_data) { throwMismtach(); }
         return *event_data;
      }

      static void throwMismtach() {
         throw std::logic_error(std::string("EventData mismatch, expecting ")
                                +typeid(T_Derived).name() + "::EventData.");
      }

   };
}
#endif
