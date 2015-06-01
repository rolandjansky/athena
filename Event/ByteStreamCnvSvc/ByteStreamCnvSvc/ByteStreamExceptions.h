/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVC_BYTESTREAMEXCEPTIONS_H
#define BYTESTREAMCNVSVC_BYTESTREAMEXCEPTIONS_H

/** @file ByteExceptions.h
 *  $Id: ByteStreamInputSvc.h,v 1.51 2009-03-03 16:03:22 gemmeren Exp $
 **/
 
// EXCEPTIONS 
namespace ByteStreamExceptions 
{
   class fileAccessError
   {
      virtual const char* what() const throw() {
         return "Problem accessing file";
      }
   };  
   class readError
   { 
      virtual const char* what() const throw() {
         return "Problem during DataReader getData";
      }
   }; 
   class badFragment
   {
      virtual const char* what() const throw() {
         return "Unable to build RawEvent, fragment does not match known formats.";
      }
   }; 
   class badFragmentData
   {
      virtual const char* what() const throw() {
         return "RawEvent does not pass validation";
      }
   }; 
} 
#endif
