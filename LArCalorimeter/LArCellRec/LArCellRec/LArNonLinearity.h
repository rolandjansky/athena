/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARNONLINEARITY_H
#define LARCELLREC_LARNONLINEARITY_H

/**

@class     LArNonLinearity
@brief   Add non linearity in calorimeter for test at L2

PACKAGE:  offline/LArCalorimeter/LArCellRec

@author   G.Unal
CREATED:  August 2003


          (non linearity = residuals of a linear calibration
           fit, from L.Serin)


********************************************************************/

#include "CaloIdentifier/LArID.h"
#include "CaloUtils/CaloCellCorrection.h"
#include "AthenaKernel/IOVSvcDefs.h"

class CaloCell;

class LArNonLinearity : public CaloCellCorrection
{

 public:

    LArNonLinearity(const std::string& type, const std::string& name, 
		const IInterface* parent);

    ~LArNonLinearity();
    virtual StatusCode initialize(); 

    /** Callback added to handle Data-driven GeoModel initialisation
     */
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);
    
    void MakeCorrection(CaloCell* theCell);    

 private: 
 
    static const double m_etatrans;
    static const double m_etrans[6];
    static const double m_emax[6];
    static const double m_p0[6][2];
    static const double m_p1[6][2];
    static const double m_p2[6][2];
    static const double m_p3[6][2];

    // Identifiers
    const LArEM_ID*        m_emID;
    const LArHEC_ID*       m_hecID;
    const LArFCAL_ID*      m_fcalID;

};

#endif       // of LARCELLREC_LARNONLINEARITY
