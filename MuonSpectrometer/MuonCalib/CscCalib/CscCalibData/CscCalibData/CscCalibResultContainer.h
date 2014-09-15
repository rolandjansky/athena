/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBRESULTCONTAINER_H
#define CSCCALIBDATA_CSCCALIBRESULTCONTAINER_H

/******************************************************************
 Name:  CscCalibResultContainer.h
 Package: MuonSpectrometer/Muoncalib/CscCalib/CscCalibData
 Author: Caleb Parnell-Lampen <lampen@physics.arizona.edu>
         University of Arizona, April 30, 2008

class to hold all calibration run results in storegate
*******************************************************************/

#include "CscCalibData/CscCalibResultCollection.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class CscCalibResultContainer : public DataVector<CscCalibResultCollection> 
{
    private: 
        std::string m_calibType;

    public:

        /** Default constructor */
        CscCalibResultContainer() 
            : DataVector<CscCalibResultCollection>(), m_calibType("")
            { };

        /** Creates an empty container ready for filling. */
        CscCalibResultContainer(std::string calibType) 
            : DataVector<CscCalibResultCollection>(),m_calibType(calibType)
            { };

        /** the identify of this collection */
        std::string calibType() const;

};

CLASS_DEF(CscCalibResultContainer,1213324673 , 0)

#endif


