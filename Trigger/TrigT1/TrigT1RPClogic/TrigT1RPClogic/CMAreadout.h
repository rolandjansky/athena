/* // -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CMAREADOUT_H
#define CMAREADOUT_H

#include "TrigT1RPClogic/RPCtrigDataObject.h"
#include "TrigT1RPClogic/CMApatterns.h"

#include "TrigT1RPChardware/Matrix.h"
#include "TrigT1RPChardware/MatrixReadOut.h"


class CMAreadout : public RPCtrigDataObject
{
    private:
    CMAparameters::CMAconfiguration m_CMAconfiguration;
    
    int m_sector;
    CMAidentity m_cma_identity;
    Matrix* m_low_pt_matrix;
    Matrix* m_high_pt_matrix;
    MatrixReadOut* m_matrices_readout[2];

    public:
    CMAreadout(CMApatterns*);
    CMAreadout(const CMAreadout&);
    ~CMAreadout();

    CMAreadout operator=(const CMAreadout&);

    bool operator==(const CMAreadout&) const;
    bool operator!=(const CMAreadout&) const;
    bool operator< (const CMAreadout&) const;

    CMAparameters::CMAconfiguration CMAconfiguration(void) const 
                                                  {return m_CMAconfiguration;}
    int sector(void)             const {return m_sector;}
    const CMAidentity& id(void)  const {return m_cma_identity;}
    const Matrix* low_pt_matrix(void)  const {return m_low_pt_matrix;}
    const Matrix* high_pt_matrix(void) const {return m_high_pt_matrix;}
    MatrixReadOut** give_matrix_readout(void);
};

#endif
