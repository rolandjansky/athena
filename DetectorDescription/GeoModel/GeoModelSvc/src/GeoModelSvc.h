/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEOMODELSVC_H
#define GEOMODELSVC_GEOMODELSVC_H

#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfoMgt/ITagInfoMgr.h"
#include <fstream>

class ISvcLocator;

template <class TYPE> class SvcFactory;

class GeoModelSvc : public AthService, virtual public IGeoModelSvc,
                    virtual public ITagInfoMgr::Listener
{
public:

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    // Query the interfaces.
    //   Input: riid, Requested interface ID
    //          ppvInterface, Pointer to requested interface
    //   Return: StatusCode indicating SUCCESS or FAILURE.
    // N.B. Don't forget to release the interface after use!!!
    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;

    /// Callback from TagInfoMgr on TagInfo change
    virtual void       tagInfoUpdated() override final;

    StatusCode compareTags();

    virtual const IGeoModelTool* getTool(std::string toolName) const override;

    virtual StatusCode clear() override;

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

    virtual const std::string & atlasVersion()         const override {return m_AtlasVersion; }
    virtual const std::string & inDetVersionOverride() const override {return m_InDetVersionOverride ;}
    virtual const std::string & pixelVersionOverride() const override {return m_PixelVersionOverride ;}
    virtual const std::string & SCT_VersionOverride()  const override {return m_SCT_VersionOverride  ;}
    virtual const std::string & TRT_VersionOverride()  const override {return m_TRT_VersionOverride  ;}
    virtual const std::string & LAr_VersionOverride()  const override {return m_LAr_VersionOverride  ;}
    virtual const std::string & tileVersionOverride()  const override {return m_TileVersionOverride  ;}
    virtual const std::string & muonVersionOverride()  const override {return m_MuonVersionOverride  ;}
    virtual const std::string & caloVersionOverride()  const override {return m_CaloVersionOverride  ;}
    virtual const std::string & magFieldVersionOverride()     const override {return m_MagFieldVersionOverride  ;}
    virtual const std::string & cavernInfraVersionOverride()  const override {return m_CavernInfraVersionOverride  ;}
    virtual const std::string & forwardDetectorsVersionOverride()  const override {return m_ForwardDetectorsVersionOverride  ;}

    virtual const std::string & inDetVersion()         const override {return m_geoDbTagSvc->inDetVersion(); }
    virtual const std::string & pixelVersion()         const override {return m_geoDbTagSvc->pixelVersion(); }
    virtual const std::string & SCT_Version()          const override {return m_geoDbTagSvc->SCT_Version(); }
    virtual const std::string & TRT_Version()          const override {return m_geoDbTagSvc->TRT_Version(); }
    virtual const std::string & LAr_Version()          const override {return m_geoDbTagSvc->LAr_Version(); }
    virtual const std::string & tileVersion()          const override {return m_geoDbTagSvc->tileVersion(); }
    virtual const std::string & muonVersion()          const override {return m_geoDbTagSvc->muonVersion(); }
    virtual const std::string & caloVersion()          const override {return m_geoDbTagSvc->caloVersion(); }
    virtual const std::string & magFieldVersion()      const override {return m_geoDbTagSvc->magFieldVersion(); }
    virtual const std::string & cavernInfraVersion()   const override {return m_geoDbTagSvc->cavernInfraVersion(); }
    virtual const std::string & forwardDetectorsVersion()   const override {return m_geoDbTagSvc->forwardDetectorsVersion(); }

    virtual GeoModel::GeoConfig geoConfig() const override {return m_geoDbTagSvc->geoConfig();}

    StatusCode geoInit ();
    StatusCode fillTagInfo() const;
};

#endif // GEOMODELSVC_GEOMODELSVC_H

