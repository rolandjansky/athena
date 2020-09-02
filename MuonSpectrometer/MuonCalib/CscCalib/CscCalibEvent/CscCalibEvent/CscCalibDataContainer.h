/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBEVENT_CSCCALIBDATACONTAINER_H
#define CSCCALIBEVENT_CSCCALIBDATACONTAINER_H 

/****************************************************************
 Name: CscCalibDataContainer.h
 Package: MuonSpectrometer/Muoncalib/CscCalib/CscCalibEvent
 Author: Ketevi A. Assamagan, October 2005
 Class to hold collections of CSC pulser calibration data
****************************************************************/

#include <vector>
#include <string>

#include "CscCalibEvent/CscCalibDataCollection.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "EventContainers/IdentifiableContainer.h" 

class CscCalibDataContainer : public IdentifiableContainer<CscCalibDataCollection> {
 public:  
    
    /** constructor */
    CscCalibDataContainer(int maxHash) ;

    /** destructor */ 
    virtual ~CscCalibDataContainer()=default; 

    /** type definitions */
    typedef CscCalibDataCollection::size_type size_type ; 
    typedef IdentifiableContainer<CscCalibDataCollection> MyBase; 

    /** Insert a Calibration data */
    void push_back( CscCalibData* calibData);

    /** content size */
    size_type calibData_size() const ; 

    /** IdentifiableContainer is still a DataObject Put CLID here. */ 
    static const CLID& classID();

    /** return class ID */
    virtual const CLID& clID() const {
      return classID(); 
    }

    /** for the default constructor without parameters */
    //void setHashFunc (const CscIdHelper* helper, int offset) ; Don't think this is needed anymore. EJWM

 private:

};

CLASS_DEF(CscCalibDataContainer,4162,0) 

/** Output stream for printing */
std::ostream& operator<<(std::ostream& lhs, const CscCalibDataContainer& rhs);

#endif



