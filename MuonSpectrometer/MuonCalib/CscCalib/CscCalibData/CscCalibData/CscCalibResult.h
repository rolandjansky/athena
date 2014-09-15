/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBRESULT_H
#define CSCCALIBDATA_CSCCALIBRESULT_H

/**************************************************************************
Package: MuonSpectrometer/MuonCalib/CscCalib/CscCalibData
Name: CscCalibData.h
Author: Caleb Parnell-Lampen
Date & Place: April 30, 2008, University of Arizona

Class to represent CSC calibration results
 ****************************************************************************/


class CscCalibResult 
{
    private:

        /* hash it of the the strip */
        int m_hashId;

        /* strip calibration constants */
        float m_value, m_error, m_chi2;
        int m_ndf;

    public:

        /* default constructor */
        CscCalibResult() { }

        /* full constructor */
        CscCalibResult(int hashId, float value, float error = -9999, float chi2 = -9999, int ndf = -9999) :
            m_hashId(hashId), m_value(value), m_error(error), m_chi2(chi2), m_ndf(ndf) {}

        /* destructor */
        virtual ~CscCalibResult() {}
        

        /* set the calibration constants */
        void setValue (const float value);
        void setError (const float error);
        void setChi2 (const float chi2) ;
        void setNdf (const int ndf); 
        void setHashId(const int hashId);

        /* get the identifier hash */
        int hashId() const; 

        /* get the calibration constants */
        float value() const;
        float error() const;
        float chi2() const; 
        int ndf() const;   
};

#endif
