/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELRAWDATABYTESTREAMCNV_PIXELRODERRORCOLLECTION_H
#define PIXELRAWDATABYTESTREAMCNV_PIXELRODERRORCOLLECTION_H

/*
This class is implemented to store the error flag information decoded from the bytestream in Storegate.
*/



//#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"


class PixelRODErrorCollection {

 private:
  unsigned int ROD_statusword1;
  unsigned int ROD_statusword2;
  unsigned int source_identifier;
  unsigned int num_statuswords;
  
 public:

  PixelRODErrorCollection(){
    num_statuswords=0;
  }

  ~PixelRODErrorCollection(){}

  const unsigned int get_ROD_statusword1(){
    return ROD_statusword1;
  }

  const unsigned int get_ROD_statusword2(){
    return ROD_statusword2;
  }

  const unsigned int get_source_identifier(){
    return source_identifier;
  }

  const unsigned int get_num_statuswords(){
    return num_statuswords;
  }

  void set_ROD_statusword1(unsigned int word){
    ROD_statusword1 = word;
  }

  void set_ROD_statusword2(unsigned int word){
    ROD_statusword2 = word;
  }

  void set_source_identifier(unsigned int identifier){
    source_identifier = identifier;
  }

  void set_num_statuswords(unsigned int num){
    num_statuswords = num;
  }

};

CLASS_DEF(PixelRODErrorCollection, 1088063584, 1)

#endif
