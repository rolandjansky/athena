/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/DoubleArray.h"

#include "TArrayD.h"

//==========================
//======= DoubleArray =========
//==========================

DoubleArray::DoubleArray(int n):TArrayD(n)
{
//  m_array=new TArrayD(n);
}

DoubleArray::DoubleArray()
{
//  m_array=new TArrayD();
}

DoubleArray::~DoubleArray()
{
//   if( m_array ) {
//      delete m_array;
//   }
}

