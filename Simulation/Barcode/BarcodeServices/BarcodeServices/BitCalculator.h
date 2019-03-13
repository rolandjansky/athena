/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __BARCODESERVICES_BITCALCULATOR_H__
#define __BARCODESERVICES_BITCALCULATOR_H__

#include "TObject.h"
#include <vector>
#include <string>

namespace Barcode {

  class BitCalculator : public TObject {

   public:
    BitCalculator();
    virtual ~BitCalculator() = default;

    int GetParentID(const int& barcode) const;
    int GetPIDofParent(const int& barcode) const;
    int GetBCID(const int& barcode) const;
    int GetHS(const int& barcode) const;
    int GetSimulator(const int& barcode) const;

    void SetParentID(int& barcode, const int& parentID) const;
    void SetPIDofParent(int& barcode, const int& parentPID) const;
    void SetBCID(int& barcode, const int& bcid) const;
    void SetHS(int& barcode, const int& hs) const;
    void SetSimulator(int& barcode, const int& simulator) const;

    inline void SetHS(int& barcode, const bool& hs) const { return SetHS(barcode,(int)hs); }

    void PrintBits(const int& barcode) const;
    void Summary(const int& barcode) const;

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

#endif //> __BARCODESERVICES_BITCALCULATOR_H__
