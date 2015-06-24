/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalManager.h
 * Implementation of inner detector global monitoring manager 
 * Based on AthenaMon.cxx
 *
 * @author Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalManager.h,v 1.17 2009-01-19 18:56:48 kastanas Exp $
 *
 ****************************************************************************/


#ifndef InDetGlobalManager_H
#define InDetGlobalManager_H

//Local includes
#include "InDetGlobalMotherMonTool.h"

//Framework
#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"


//Standard c++
#include <string>
//Predeclarations
class ISvcLocator;
class InDetGlobalMonitorToolBase;
class StatusCode;
class SCT_IdMapping;
class IIneterface;

/// Top Algorithm of the Inner Detector Global Monitoring

class InDetGlobalManager : public AthenaMonManager
{

public:

    InDetGlobalManager(const std::string &name,ISvcLocator *pSvcLocator);
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

private:
    
    ///@name storegatenames
    ///Names of objects to retrieve and feed into the monitoring AlgTools
    ///@{

    /// Name of SCT tracks container
    std::string m_SCTTracksName;

    /// Name of TRT tracks container
    std::string m_TRTTracksName;

    /// Name of pixel tracks container
    std::string m_PIXTracksName;

    /// Name of combined inner detector tracks container
    std::string m_CombinedTracksName;
    const TrackCollection *m_CombinedTracks;

    /// Name of trt drift circle container
    std::string m_TRT_DriftCircleName;
    ///@}

    bool m_doTopBottom;   

}; //end of class


#endif
