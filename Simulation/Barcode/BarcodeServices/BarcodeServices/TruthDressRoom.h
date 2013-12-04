/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __BARCODE_TRUTHDRESSROOM_H__
#define __BARCODE_TRUTHDRESSROOM_H__

#include "TObject.h"
#include <vector>
#include <string>
#include "BarcodeServices/BitCalculator.h"

namespace Barcode {

  class TruthDressRoom : public TObject {

   public:
    TruthDressRoom();
    virtual ~TruthDressRoom();
    
    int GetID(const int& barcode, const int& extrabarcode);
    int GetStatus(const int& barcode, const int& extrabarcode);
    int GetParentID(/*const int& barcode,*/ const int& extrabarcode);
    int GetPIDofParent(/*const int& barcode,*/ const int& extrabarcode);
    int GetBCID(/*const int& barcode,*/ const int& extrabarcode);
    int GetHS(/*const int& barcode,*/ const int& extrabarcode);
    int GetSimulator(/*const int& barcode,*/ const int& extrabarcode); 
    
   private:

    BitCalculator m_calculator;

    ClassDef(TruthDressRoom,1)
  };
}

#endif //> __BARCODE_TRUTHDRESSROOM_H__

