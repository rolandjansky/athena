/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_ALIGNTRANS_P1_H
#define DETDESCRCOND_ALIGNTRANS_P1_H
//
// AlignableTransform_p1.h - persistent class for AlignableTransform
//
// RD Schaffer, started 23/02/12
//

#include <vector>
#include <string>

class AlignableTransform_p1 {
public:
    AlignableTransform_p1() {}    

    std::string m_tag;
    std::vector<unsigned int>  m_ids;
    std::vector<double>        m_trans;
};

#endif // DETDESCRCOND_ALIGNTRANS_P1_H
