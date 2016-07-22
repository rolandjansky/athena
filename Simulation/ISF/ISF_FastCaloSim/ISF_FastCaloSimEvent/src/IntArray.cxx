/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/IntArray.h"

#include "TArrayI.h"

//==========================
//======= IntArray =========
//==========================

using namespace std;

IntArray::IntArray(int n)
{
  m_array=new TArrayI(n);
}

IntArray::IntArray()
{
  m_array=new TArrayI();
}


//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(IntArray)

