/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  StatusCode.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef __TopoCore__StatusCode__
#define __TopoCore__StatusCode__

#include <iostream>

namespace TCS {
   
   class StatusCode {
   public:
      enum code_t { FAILURE=0, RECOVERABLE=1, SUCCESS=2 };
      
      StatusCode():
         m_status(SUCCESS)
         , m_checked(false)
      {}
      
   public:
      StatusCode(code_t code):
         m_status(code)
         , m_checked(false)
      {}
      
      code_t getCode()  const { return m_status; }
      
      bool isSuccess() const {
         m_checked = true;
         return m_status == SUCCESS;
      }
      
      bool isRecoverable() const {
         m_checked = true;
         return m_status == RECOVERABLE;
      }
      
      bool isFailure() {
         m_checked = true;
         return m_status == FAILURE;
      }
      
      StatusCode operator|=(const StatusCode & sc) {
         return (sc.getCode()>m_status)?StatusCode(sc.getCode()):StatusCode(m_status); // max of this and sc (any SUCCESS is SUCCESS)
      }

      StatusCode operator&=(const StatusCode & sc) {
         return (sc.getCode()<m_status)?StatusCode(sc.getCode()):StatusCode(m_status); // min of this and sc (any FAILURE is FAILURE)
      }

   private:
      code_t       m_status;
      mutable bool m_checked;
      
   };
   
} // end of namespace TCS

inline
std::ostream& operator<< ( std::ostream& o, const TCS::StatusCode& sc ) {
   if ( TCS::StatusCode::SUCCESS == sc.getCode() ) {
      return o << "SUCCESS" ;
   } else if ( sc.isRecoverable() ) {
      return o << "RECOVERABLE";
   }
   o << "FAILURE" ;
   if ( TCS::StatusCode::FAILURE != sc.getCode() ) {
      o << "(" << sc.getCode() << ")" ;
   }
   return o;
}
      
      
      
#endif /* defined(__TopoCore__StatusCode__) */
