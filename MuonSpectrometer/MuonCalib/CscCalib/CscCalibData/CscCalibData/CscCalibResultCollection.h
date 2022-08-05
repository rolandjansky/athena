/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBRESULTCOLLECTION_H
#define CSCCALIBDATA_CSCCALIBRESULTCOLLECTION_H

/******************************************************************
 Name:  CscCalibResultCollection.h
 Package: MuonSpectrometer/Muoncalib/CscCalib/CscCalibData
 Author: Caleb Parnell-Lampen
         University of Arizona, April 30 2008

 class to hold a collection of CSC calibration results. Each
 CscCalibResultCollection should contain all of the results
 for a single tested parameter.
*******************************************************************/

#include "CscCalibData/CscCalibResult.h"
#include "AthContainers/DataVector.h"
#include <string>

class CscCalibResultCollection : public DataVector<CscCalibResult> 
{
    private:
        std::string m_parName; 

    public:
        /** Default constructor */
        CscCalibResultCollection() 
            : DataVector<CscCalibResult>(),m_parName("")
            { };

        /** Creates an empty container ready for filling. */
        CscCalibResultCollection(const std::string& parName) 
            : DataVector<CscCalibResult>(),m_parName(parName)
            { };

        /** the identify of this collection */
        std::string parName() const ;
};

#endif


