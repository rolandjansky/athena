/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_KEYTRANS_P1_H
#define DETDESCRCOND_KEYTRANS_P1_H
//
// DetCondKeyTrans_p1.h - persistent class for DetCondKeyTrans
//
// RD Schaffer, started 23/02/12
//

#include <string>
#include <vector>

class DetCondKeyTrans_p1 {
public:
    DetCondKeyTrans_p1() {}

    std::vector<std::string>  m_keys;
    std::vector<double>       m_trans;
};


#endif // DETDESCRCOND_KEYTRANS_P1_H
