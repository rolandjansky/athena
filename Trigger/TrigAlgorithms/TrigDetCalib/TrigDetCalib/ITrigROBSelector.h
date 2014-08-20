/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: LAr Calibration
 * @Package: TrigDetCalib
 * @Class  : ITrigROBSelector
 *
 * @brief : interface for TrigROBSelector
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: ITrigROBSelector.h,v 1.1 2008-10-14 12:03:47 nberger Exp $
 **********************************************************************************/

#ifndef TrigEgammaHypo_ITrigROBSelector_H
#define TrigEgammaHypo_ITrigROBSelector_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "IRegionSelector/RegSelEnums.h"

class PartialEventBuildingInfo;
class IRoiDescriptor;

/**
     @class ITrigROBSelector
     interface for TrigROBSelector
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
*/

static const InterfaceID IID_ITrigROBSelector ( "ITrigROBSelector", 1, 0 );


class ITrigROBSelector : virtual public IAlgTool
{
  public:
    
    static const InterfaceID& interfaceID() { return IID_ITrigROBSelector; }
    
    virtual HLT::ErrorCode fillPEBInfo(PartialEventBuildingInfo& pebInfo,
                                       const IRoiDescriptor& roi, 
                                       std::vector<DETID>* dets = 0, std::vector<int>* nRobs = 0) = 0;
    virtual HLT::ErrorCode setupMonitoring() = 0;
    virtual bool reset() = 0;
};

#endif
