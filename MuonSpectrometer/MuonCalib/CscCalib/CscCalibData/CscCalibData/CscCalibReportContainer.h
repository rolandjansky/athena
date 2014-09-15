/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBREPORTCONTAINER_H
#define CSCCALIBDATA_CSCCALIBREPORTCONTAINER_H

/******************************************************************
 Name:  CscCalibReportContainer.h
 Package: MuonSpectrometer/Muoncalib/CscCalib/CscCalibData
 Author: Caleb Parnell-Lampen <lampen@physics.arizona.edu>
         University of Arizona, April 30, 2008

class to hold details about the process of the calibration, such as
fit plots. Its a datavector of CscCalibReportBase. There is actually
a good chance we'll only ever use one CscCalibReportBase, but making
it a datavector gives us good future flexibility.
*******************************************************************/

#include "CscCalibData/CscCalibReportBase.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class CscCalibReportContainer : public DataVector<CscCalibReportBase> 
{
    private: 
        std::string m_calibType;

    public:

        /** Default constructor */
        CscCalibReportContainer() 
            : DataVector<CscCalibReportBase>(), m_calibType("")
            { };

        /** Creates an empty container ready for filling. */
        CscCalibReportContainer(std::string calibType) 
            : DataVector<CscCalibReportBase>(),m_calibType(calibType)
            { };

        /** the identify of this collection */
        std::string calibType() const;

};

CLASS_DEF(CscCalibReportContainer,1300481702, 0)

#endif


