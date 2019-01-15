/* emacs: this is -*- c++ -*- */
/**
 **
 **   @file         IReadCards.h  
 **
 **                 fortran callable interface routines to the 
 **                 ReadCards parameter file   
 **                   
 ** 
 **   @author       M.Sutton  
 **
 **   @date         Fri Aug 12 17:40:56 CEST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/


#ifndef READCARDS_IREADCARDS_H
#define READCARDS_IREADCARDS_H


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



#endif  /* READCARDS_IREADCARDS_H */










