/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARG3ESCALE_TDR_H
#define LARCELLREC_LARG3ESCALE_TDR_H

/********************************************************************

NAME:     LArG3Escale_TDR.cxx
PACKAGE:  offline/LArCalorimeter/LArCellRec

AUTHORS:  H. Ma
CREATED:  Nov. 15, 1999

PURPOSE:  Energy Scale Factor for G3 digits (TDR).

          The original version is adapted from the atrecon routines:
	  qrcaleb, qrcalee, in offline/atutil

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Jan. 7, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

********************************************************************/

#include "CaloIdentifier/LArID.h"
#include "CaloUtils/CaloCellCorrection.h"
class CaloCell;

class LArG3Escale_TDR : public CaloCellCorrection
{

 public:

    LArG3Escale_TDR(const std::string& type, const std::string& name, 
		    const IInterface* parent);

    ~LArG3Escale_TDR();
    virtual StatusCode initialize(); 

    void MakeCorrection(CaloCell* theCell);    

 private: 

    double scaleb(double eta);
    double scalee(double eta);

    static const double s_gap0;
    static const double s_etacr;
    static const double s_ecdg_scale;
    static const double s_sfgin[4];
    static const double s_sfgout[4];

    float m_OverallScale;
    bool  m_FromHit;//MATTHIEU 

    const LArEM_ID* m_emID;
    const LArHEC_ID* m_hecID;
    const LArFCAL_ID* m_fcalID;

};

#endif       // of LARCELLREC_LARG3ESCALE_TDR




