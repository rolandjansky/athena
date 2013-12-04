/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BarcodeServices/TruthDressRoom.h"
#include <iostream>

ClassImp(Barcode::TruthDressRoom)


Barcode::TruthDressRoom::TruthDressRoom()
  : TObject()
{
}


Barcode::TruthDressRoom::~TruthDressRoom()
{
}


int 
Barcode::TruthDressRoom::GetID(const int& barcode, const int& /*extrabarcode*/)
{
  return barcode;
}


int 
Barcode::TruthDressRoom::GetStatus(const int& /*barcode*/, const int& /*extrabarcode*/)
{
  return 0;
}


int 
Barcode::TruthDressRoom::GetParentID(/*const int& barcode,*/ const int& extrabarcode)
{
  return m_calculator.GetParentID(extrabarcode);
}


int 
Barcode::TruthDressRoom::GetPIDofParent(/*const int& barcode,*/ const int& extrabarcode)
{
  return m_calculator.GetPIDofParent(extrabarcode);
}


int 
Barcode::TruthDressRoom::GetHS(/*const int& barcode,*/ const int& extrabarcode)
{
  return m_calculator.GetHS(extrabarcode);
}


int
Barcode::TruthDressRoom::GetSimulator(/*const int& barcode,*/ const int& extrabarcode)
{
  return m_calculator.GetSimulator(extrabarcode);
}


int 
Barcode::TruthDressRoom::GetBCID(/*const int& barcode,*/ const int& extrabarcode)
{
  return m_calculator.GetBCID(extrabarcode);
}

