/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************
 Class def for MuonGeoModel DblQ00/WLBI
 *******************************************************/

 //  author: S Spagnolo
 // entered: 07/28/04
 // comment: LONG BEAM

#ifndef DBLQ00_WLBI_H
#define DBLQ00_WLBI_H

#include "RDBAccessSvc/IRDBQuery.h"
#include <memory>
namespace MuonGM
{
class DblQ00Wlbi {

public:
    DblQ00Wlbi(){};
    ~DblQ00Wlbi();
    DblQ00Wlbi(std::unique_ptr<IRDBQuery>&&);

    struct WLBI {
        int version; // VERSION
        int jsta; // INDEX
        int num; // NUMBER OF OBJECTS
        float height; // HEIGHT
        float thickness; // WALL THICKNESS
        float lowerThickness; // 
        float yShift; // 
    };

    const WLBI* data() const { return m_d; };
    unsigned int size() const { return m_nObj; };
    const char* getName() const { return "WLBI"; };
    const char* getDirName() const { return "DblQ00"; };
    const char* getObjName() const { return "WLBI"; };

private:
    WLBI* m_d;
    unsigned int m_nObj; // > 1 if array; 0 if error in retrieve.
    DblQ00Wlbi & operator=(const DblQ00Wlbi &right);
    DblQ00Wlbi(const DblQ00Wlbi&);
};
} // end of MuonGM namespace

#endif // DBLQ00_WLBI_H

