//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_P1_H
#define ATHENAPOOLTESTATHENAPOOL_PIXELRDOELEMLINKVECCNV_P1_H

#include "AthenaPoolTestData/PixelRDOElemLinkVec.h"
#include "AthenaPoolTestAthenaPool/PixelRDOElemLinkVec_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p2.h"

class MsgStream;

class PixelRDOElemLinkVecCnv_p1 {
public:
    PixelRDOElemLinkVecCnv_p1() {};
    ~PixelRDOElemLinkVecCnv_p1() {}; 


    void persToTrans(const PixelRDOElemLinkVec_p1*, PixelRDOElemLinkVec*, MsgStream &log) ;
    void transToPers(const PixelRDOElemLinkVec*, PixelRDOElemLinkVec_p1*, MsgStream &log) ;
private:

    ElementLinkCnv_p2<PixelRDOElemLinkVec::elem_type>   m_elementLinkCnv;

};


#endif
