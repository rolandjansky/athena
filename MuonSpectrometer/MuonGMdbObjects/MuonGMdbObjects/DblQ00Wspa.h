/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WSPA
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: RPC SPACER

#ifndef DBLQ00_WSPA_H
#define DBLQ00_WSPA_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wspa {

public:
    DblQ00Wspa(){};
    ~DblQ00Wspa();
    DblQ00Wspa(std::unique_ptr<IRDBQuery>&&);

    // data members for DblQ00/WSPA fields
    struct WSPA {
        int version; // VERSION
        int jsta; // JSTA INDEX
        int nb; // NUMBER OF DETAILS
        float x0; // X0
        float tckspa; // THICKNESS OF SPACER
    };
    
    const WSPA* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WSPA"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WSPA"; };

private:
  WSPA* m_d;
  unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
  DblQ00Wspa & operator=(const DblQ00Wspa &right);
  DblQ00Wspa(const DblQ00Wspa&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WSPA_H

