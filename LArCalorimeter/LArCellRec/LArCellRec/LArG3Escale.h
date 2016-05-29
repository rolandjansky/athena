/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARG3ESCALE_H
#define LARCELLREC_LARG3ESCALE_H

/** 
@class LArG3Escale
@brief Energy Scale Factor for G3 digits.

NAME:     LArG3Escale.cxx
PACKAGE:  offline/LArCalorimeter/LArCellRec

AUTHORS:  H. Ma
CREATED:  Nov. 15, 1999

PURPOSE:  Energy Scale Factor for G3 digits.

          The original version is adapted from the atrecon routines:
	  qrcaleb, qrcalee, in offline/atutil

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Jan. 7, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

********************************************************************/

#include "CaloIdentifier/LArID.h"
#include "CaloUtils/CaloCellCorrection.h"
class CaloDetDescrElement ;


class CaloCell;

class LArG3Escale : public CaloCellCorrection
{

 public:

    LArG3Escale(const std::string& type, const std::string& name, 
		const IInterface* parent);

    virtual ~LArG3Escale();
    virtual StatusCode initialize(); 

    void MakeCorrection(CaloCell* theCell);    
    double correction(const CaloDetDescrElement* theCaloDDE) ;    

 private: 
 
    double scalee(double eta);
    double GetWgt(int table, double eta);
    double LArScale(int bar_ec, double abseta);

    static const double s_gap0;
    static const double s_ecdg_scale;
    static const double s_sfgin[4];
    static const double s_emscale_bar[62]; 
    static const double s_emscale_end[46]; 
    static const int s_nbinbar;
    static const int s_nbinend;
    static const double s_etamaxbar;
    static const double s_etaminbar;
    static const double s_etamaxend;
    static const double s_etaminend;
    static const double s_etamax;
    static const double s_etamin;
    static const int s_numEtaBins = 100;
    static const double m_table_eta[s_numEtaBins+1];
    static const double m_table_p[4][s_numEtaBins];  

    bool  m_Weights;    
    float m_OverallScale;    

    // Identifiers
    const LArEM_ID*        m_emID;
    const LArHEC_ID*       m_hecID;
    const LArFCAL_ID*      m_fcalID;

    int m_WorkMode;
};

#endif       // of LARCELLREC_LARG3ESCALE




