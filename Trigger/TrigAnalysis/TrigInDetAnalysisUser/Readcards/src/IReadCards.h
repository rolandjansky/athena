// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**************************************************************************
 **
 **   File:         IReadCards.h  
 **
 **   Description:  fortran callable interface routines to the 
 **                 ReadCards parameter file   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Fri Aug 12 17:40:56 CEST 2005
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __IREADCARDS_H
#define __IREADCARDS_H


#ifdef __cplusplus
extern "C" {
#endif
  void initcards_(const char* );
  void deletecards_(void);
  void isdefined_(const char* , int& );
  void getstring_(const char* , char* );
  void getdvalue_(const char* , double& );
  void getfvalue_(const char* , float& );
  void getivalue_(const char* , int& );
  void getbvalue_(const char* , bool& );
  void getdvector_(const char* , double* );
  void getivector_(const char* , int* );
  void printcards_();
#ifdef __cplusplus
}
#endif



#endif  /* __IREADCARDS_H */










