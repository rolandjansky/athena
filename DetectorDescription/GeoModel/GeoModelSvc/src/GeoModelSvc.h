/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEOMODELSVC_H
#define GEOMODELSVC_GEOMODELSVC_H

#include "GeoModelKernel/Query.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfoMgt/ITagInfoMgr.h"

class ISvcLocator;

template <class TYPE> class SvcFactory;

class GeoModelSvc : public AthService, virtual public IGeoModelSvc {

public:

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    virtual StatusCode align        (IOVSVC_CALLBACK_ARGS);
    virtual StatusCode compareTags  (IOVSVC_CALLBACK_ARGS);

    virtual const IGeoModelTool* getTool(std::string toolName) const;

    virtual StatusCode clear();

    friend class SvcFactory<GeoModelSvc>;

    // Standard Constructor
    GeoModelSvc(const std::string& name, ISvcLocator* svc);

    // Standard Destructor
    virtual ~GeoModelSvc();


protected:

    //     StatusCode append( IGeoModelTool* pddTool, 
    // 		       std::vector<IGeoModelTool*>* theTools ); 
    //     StatusCode decodeNames( StringArrayProperty& theNames, 
    //        						std::vector<IGeoModelTool*>* theTools ); 
	
private:

    ToolHandleArray< IGeoModelTool > m_detectorTools; // Detector Tools

    ISvcLocator*        m_pSvcLocator;

    ServiceHandle<IToolSvc>     m_toolSvc;     
    ServiceHandle<StoreGateSvc> m_detStore;   
    ServiceHandle<ITagInfoMgr>  m_tagInfoMgr;   
    ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc;

    std::string           m_AtlasVersion;

    std::string           m_InDetVersionOverride;
    std::string           m_PixelVersionOverride;
    std::string           m_SCT_VersionOverride;
    std::string           m_TRT_VersionOverride;
    std::string           m_LAr_VersionOverride;
    std::string           m_TileVersionOverride;
    std::string           m_MuonVersionOverride;
    std::string           m_CaloVersionOverride;
    std::string           m_MagFieldVersionOverride;
    std::string           m_CavernInfraVersionOverride;
    std::string           m_ForwardDetectorsVersionOverride;

    bool          m_printMaterials;               // Print the contents of the Material Manager at the end of geoInit
    bool          m_callBackON;                   // Register callback for Detector Tools
    bool          m_ignoreTagDifference;          // Keep going if TagInfo and property tags are different
                                                  // when geometry configured manually 
    bool          m_useTagInfo;                   // Flag for TagInfo usage
    bool          m_useCaloAlign;                 // Flag for using alignments for Calo
    bool          m_statisticsToFile;             // Flag for generating GeoModelStatistics file in the run directory

    int           m_supportedGeometry;            // Supported geometry flag is set in jobOpt and is equal to major release version
    bool          m_ignoreTagSupport;             // If true then don't check SUPPORT flag for ATLAS tag

    const std::string & atlasVersion()         const {return m_AtlasVersion; }
    const std::string & inDetVersionOverride() const {return m_InDetVersionOverride ;}
    const std::string & pixelVersionOverride() const {return m_PixelVersionOverride ;}
    const std::string & SCT_VersionOverride()  const {return m_SCT_VersionOverride  ;}
    const std::string & TRT_VersionOverride()  const {return m_TRT_VersionOverride  ;}
    const std::string & LAr_VersionOverride()  const {return m_LAr_VersionOverride  ;}
    const std::string & tileVersionOverride()  const {return m_TileVersionOverride  ;}
    const std::string & muonVersionOverride()  const {return m_MuonVersionOverride  ;}
    const std::string & caloVersionOverride()  const {return m_CaloVersionOverride  ;}
    const std::string & magFieldVersionOverride()     const {return m_MagFieldVersionOverride  ;}
    const std::string & cavernInfraVersionOverride()  const {return m_CavernInfraVersionOverride  ;}
    const std::string & forwardDetectorsVersionOverride()  const {return m_ForwardDetectorsVersionOverride  ;}

    const std::string & inDetVersion()         const {return m_geoDbTagSvc->inDetVersion(); }
    const std::string & pixelVersion()         const {return m_geoDbTagSvc->pixelVersion(); }
    const std::string & SCT_Version()          const {return m_geoDbTagSvc->SCT_Version(); }
    const std::string & TRT_Version()          const {return m_geoDbTagSvc->TRT_Version(); }
    const std::string & LAr_Version()          const {return m_geoDbTagSvc->LAr_Version(); }
    const std::string & tileVersion()          const {return m_geoDbTagSvc->tileVersion(); }
    const std::string & muonVersion()          const {return m_geoDbTagSvc->muonVersion(); }
    const std::string & caloVersion()          const {return m_geoDbTagSvc->caloVersion(); }
    const std::string & magFieldVersion()      const {return m_geoDbTagSvc->magFieldVersion(); }
    const std::string & cavernInfraVersion()   const {return m_geoDbTagSvc->cavernInfraVersion(); }
    const std::string & forwardDetectorsVersion()   const {return m_geoDbTagSvc->forwardDetectorsVersion(); }

    GeoModel::GeoConfig geoConfig() const {return m_geoDbTagSvc->geoConfig();}

    StatusCode geoInit ();
    StatusCode fillTagInfo() const;
};

#endif // GEOMODELSVC_GEOMODELSVC_H
