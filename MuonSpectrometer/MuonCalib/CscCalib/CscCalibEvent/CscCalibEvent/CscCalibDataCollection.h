/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSPECTROMETER_CSCCALIBDATACOLLECTION_H
#define MUONSPECTROMETER_CSCCALIBDATACOLLECTION_H

/******************************************************************
 Name:  CscCalibDataCollection.h
 Package: MuonSpectrometer/Muoncalib/CscCalib/CscCalibEvent
 Author: Ketevi A. Assamagan
         BNL, October 2005

 class to hold a collection of CSC pulser calibration data
*******************************************************************/

#include "CscCalibEvent/CscCalibData.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/Identifier.h"

class CscCalibDataCollection : public DataVector<CscCalibData> {

public:
 
  /** typedef  */
  typedef Identifier ID; 
  typedef CscCalibData DIGIT;
 
  /** Default constructor */
  CscCalibDataCollection() 
	: DataVector<CscCalibData>(),m_id(0)
	{ };

  /** Creates an empty container ready for filling. */
  CscCalibDataCollection(Identifier id) 
	: DataVector<CscCalibData>(),m_id(id)
	{ };

  /** the identify of this collection */
  Identifier identify() const {
    return m_id;
  }

  private:

    /** collection identifier */
    Identifier m_id; 

};

CLASS_DEF(CscCalibDataCollection, 4161, 1)

/** Class needed only for persistency */
typedef DataVector<CscCalibDataCollection> CscCalibDataCollection_vector;
CLASS_DEF( CscCalibDataCollection_vector , 1092354873 , 1 )

#endif


