/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===============================================================
//     MuonCalibStreamInputSvc.h
//===============================================================
//
// Description: Interface class for MuonCalibStream Input
//              
//              The concrete class can be provide Calib event from 
//              a file, transient store, or through network. 
//---------------------------------------------------------------
#ifndef MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMINPUTSVC_H
#define MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMINPUTSVC_H


#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthService.h"

#include "MuCalDecode/CalibEvent.h"

class MuonCalibStreamInputSvc :  public AthService
{
public:

    MuonCalibStreamInputSvc(const std::string& name, ISvcLocator* svcloc);
    virtual ~MuonCalibStreamInputSvc(void);
   
    virtual StatusCode initialize();
    
    virtual const LVL2_MUON_CALIBRATION::CalibEvent* nextEvent() = 0;
    virtual const LVL2_MUON_CALIBRATION::CalibEvent* currentEvent() const = 0;
 
 protected:

};

#endif 
