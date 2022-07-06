/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_MUONDETECTORTOOL_H
#define MUONGEOMODEL_MUONDETECTORTOOL_H

#include "GeoPrimitives/GeoPrimitives.h" // need to include Amg stuff first to avoid that another Eigen implementation is included first which breaks compilation
#include "GeoModelUtilities/GeoModelTool.h"

namespace MuonGM {
    class MuonDetectorManager;
}

class MuonDetectorTool final : public GeoModelTool {

  public:
    // Standard Constructor
    MuonDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);

    // Standard Destructor
    virtual ~MuonDetectorTool() override final;

    // initialize - needed to retrieve the alignment Tool
    virtual StatusCode initialize() override;

    // build the geometry
    virtual StatusCode create() override final;
    StatusCode createFactory(MuonGM::MuonDetectorManager * & ) const;

    // Dereference tree tops and drop readout objects
    virtual StatusCode clear() override final;

  private:

    Gaudi::Property<std::string> m_layout{this, "LayoutName", "R.08" };
    Gaudi::Property<bool> m_accessCondDb{this,"UseConditionDb", true};
    Gaudi::Property<bool> m_asciiCondData{this,"UseAsciiConditionData", false};
    Gaudi::Property<int> m_amdcDb{this, "BuildFromAmdcDb", 0, "Build the geometry from AMDB file (expert only!)"};
 
    Gaudi::Property<bool> m_includeCutouts{this, "IncludeCutouts", false};
    Gaudi::Property<bool> m_includeCutoutsBog{this, "IncludeCutoutsBog", false };
    Gaudi::Property<bool> m_includeCtbBis{this, "IncludeCtbBis" , false };
      
    Gaudi::Property<bool> m_fillCache_initTime{this, "FillCacheInitTime", false};
    Gaudi::Property<bool> m_dumpMemoryBreakDown{this, "DumpMemoryBreakDown", false};
    Gaudi::Property<bool> m_enableFineClashFixing{this, "EnableFineClashFixing", false};
    Gaudi::Property<bool> m_hasCSC{this,"HasCSC", true};
    Gaudi::Property<bool> m_hasSTgc{this, "HasSTgc", true};
    Gaudi::Property<bool> m_hasMM{this, "HasMM", true};
   
    Gaudi::Property<bool> m_stationSelection{this, "StationSelection", false};
    Gaudi::Property<std::vector<std::string>> m_selectedStations{this, "SelectedStations", {}};
    Gaudi::Property<std::vector<int>> m_selectedStEta{this,"SelectedStJzz",{}};
    Gaudi::Property<std::vector<int>> m_selectedStPhi{this,"SelectedStJff",{}};
    
   
    Gaudi::Property<int> m_controlAlines{this, "ControlAlines", 111111, "allowed values are   0 -  111111"};
    Gaudi::Property<bool> m_dumpAlines{this, "DumpAlines", false};
    Gaudi::Property<std::string> m_altAsztFile{this, "AlternateASZTFile", "" };
    Gaudi::Property<int> m_minimalGeoFlag{this, "MinimalGeoFlag", 0};

    Gaudi::Property<bool> m_useCscIntAlines{this, "EnableCscInternalAlignment", false};
    Gaudi::Property<int> m_controlCscIntAlines{this, "ControlCscInternalAlines", 111111 , "allowed values are 0 - 111111"};
    Gaudi::Property<bool> m_dumpCscIntAlines{this, "DumpCscInternalAlines", false};
    Gaudi::Property<bool> m_useCscIntAlinesFromGM{this, "UseIlinesFromGM", true};
    Gaudi::Property<std::string> m_altCscIntAlinesFile{this, "AlternateCscIntAlignFile", ""};
    
    
    
    Gaudi::Property<bool> m_cachingFlag{this, "CachingFlag", true, "Turn on/off caching of ReadoutElement surfaces etc. (i.e. for MDTs)"};
    // the following two properties are for local validation, not for production
    Gaudi::Property<std::string> m_NSWABLineAsciiPath{this, "NSWABLineAsciiPath", "", "Pass A/B Lines for NSW from an Ascii file"};
    
    Gaudi::Property<bool> m_enableMdtDeformations{this, "EnableMdtDeformations", true};
    Gaudi::Property<bool> m_enableMdtAsBuiltParameters{this, "EnableMdtAsBuiltParameters", false};
    Gaudi::Property<bool> m_enableNswAsBuiltParameters{this, "EnableNswAsBuiltParameters", false};
    Gaudi::Property<std::string> m_altMdtAsBuiltFile{this, "AlternateAsBuiltParamAlignFile", "" };

    MuonGM::MuonDetectorManager* m_manager{nullptr};
};

#endif // GEOMODELEXAMPLES_TOYDETECTORTOOL_H
