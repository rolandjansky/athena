/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __BARCODE_BITCALCULATOR_H__
#define __BARCODE_BITCALCULATOR_H__

#include "TObject.h"
#include <vector>
#include <string>

namespace Barcode {

  class BitCalculator : public TObject {

   public:
    BitCalculator();
    virtual ~BitCalculator();
    
    int GetParentID(const int& barcode);
    int GetPIDofParent(const int& barcode);
    int GetBCID(const int& barcode);
    int GetHS(const int& barcode);
    int GetSimulator(const int& barcode); 
    
    void SetParentID(int& barcode, const int& parentID);
    void SetPIDofParent(int& barcode, const int& parentPID);
    void SetBCID(int& barcode, const int& bcid);
    void SetHS(int& barcode, const int& hs);
    void SetSimulator(int& barcode, const int& simulator);
    
    inline void SetHS(int& barcode, const bool& hs) { return SetHS(barcode,(int)hs); }

    void PrintBits(const int& barcode);
    void Summary(const int& barcode);

   private:

    const int m_nbitsparent;
    const int m_nbitspid;
    const int m_nbitshs;
    const int m_nbitssimulator;
    const int m_nbitsbcid;
    
    int m_parentOne;
    int m_pidOne;
    int m_hsOne;
    int m_simulatorOne;
    int m_bcidOne;
    
    ClassDef(BitCalculator,1)
  };
}

#endif //> __BARCODE_BITCALCULATOR_H__

