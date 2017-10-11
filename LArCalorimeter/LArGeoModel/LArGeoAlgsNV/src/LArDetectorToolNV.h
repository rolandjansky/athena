/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArDetectorToolNV.h
 *
 * @brief Declaration of LArDetectorToolNV class
 *
 * $Id: LArDetectorToolNV.h,v 1.8 2009-05-13 15:47:48 tsulaia Exp $
 */

#ifndef LARGEOMODEL_LARDETECTORTOOL_H
#define LARGEOMODEL_LARDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "IOVSvc/IOVSvc.h"
class LArDetectorManager;
class StoreGateSvc;

/** @class LArDetectorToolNV
    @brief LArDetectorToolNV is a standard GeoModel tool, which calls LArDetectorFactory::create(),
    stores LArDetectorManager to the Detector Store and also registers a callback function
    align() which applies misalignments on top of the 'regular' geometry.
*/
class LArDetectorToolNV : public GeoModelTool {

public:

    // Standard Constructor
    LArDetectorToolNV( const std::string& type, const std::string& name, const IInterface* parent );

    // Standard Destructor
    virtual ~LArDetectorToolNV() override final;

    // Create Method:
    virtual StatusCode create() override final;

    // Clear Method
    virtual StatusCode clear() override final;

    // Register callback function on ConDB object
    virtual StatusCode registerCallback() override final;

    // Callback function itself
    virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override final;
	
 private:

    bool m_barrelSaggingOn;
    int  m_barrelVisLimit;
    int  m_fcalVisLimit;

    bool m_buildBarrel;
    bool m_buildEndcap;

    bool m_applyAlignments;

    const LArDetectorManager *m_manager;

    std::string m_geometryConfig; // FULL, SIMU, RECO
};

#endif 
