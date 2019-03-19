///////////////////////////////////////////////////////////////////
// XMLReaderSvc.h, (c) ATLAS Detector software
// Tool to fill templates from XML files
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_XMLREADERSVC_H
#define INDETTRACKINGGEOMETRY_XMLREADERSVC_H

// Gaudi
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
// STD
#include <fstream>
#include <vector>
// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
// Trk tools library
#include <InDetTrackingGeometryXML/XMLUtils.h>
#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

using namespace xercesc;

namespace Trk {
  class LayerMaterialProperties;
}

namespace InDet {
  static const InterfaceID IID_XMLReaderSvc("InDetXMLReaderSvc", 1, 0);

  class XMLReaderSvc : public AthService, public XMLUtils {
  public:
    XMLReaderSvc(const std::string& name, ISvcLocator* svc);
    virtual ~XMLReaderSvc();

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_XMLReaderSvc; }
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // Get chip and module templates by name
    ChipTmp* getChipTemplate(const std::string s) const;
    ModuleTmp* getModuleTemplate(const std::string s) const;
    // Get material and component templates by name
    MaterialTmp* getMaterialTemplate(const std::string s) const;
    ComponentTmp* getComponentTemplate(const std::string s) const;
    // Get stave and layer templates by layer index
    std::vector<StaveTmp*> getPixelStaveTemplate(unsigned int ilayer) const;
    BarrelLayerTmp *getPixelBarrelLayerTemplate(unsigned int ilayer) const;
    EndcapLayerTmp *getPixelEndcapLayerTemplate(unsigned int ilayer) const;
    std::vector<StaveTmp*> getSCTStaveTemplate(unsigned int ilayer) const;
    BarrelLayerTmp *getSCTBarrelLayerTemplate(unsigned int ilayer) const;
    EndcapLayerTmp *getSCTEndcapLayerTemplate(unsigned int ilayer) const;

    // Layer building helpers
    inline bool doSCT() const { return m_doSCT; }
    inline bool isGMX() const { return m_isGMX; }
    inline bool doPixels() const { return m_doPix; }
    inline unsigned int nbOfPixelBarrelLayers() const { return m_tmp_pixBarrelLayer.size(); }
    inline unsigned int nbOfPixelEndcapLayers() const { return m_tmp_pixEndcapLayer.size(); }
    inline unsigned int nbOfSCTBarrelLayers() const   { return m_tmp_sctBarrelLayer.size(); }
    inline unsigned int nbOfSCTEndcapLayers() const   { return m_tmp_sctEndcapLayer.size(); }
    inline std::string slhcVersion() const { return m_xml_SLHCVersion; }
   
    // Stave building helpers
    double getHalfPlainLength(InDet::StaveTmp *staveTmp) const;
    double getZStep(InDet::StaveTmp *staveTmp) const;
    void computeZbounds(Amg::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &active_halflength) const;
    void computeRbounds(Amg::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &rMin, double &rMax) const;
    const Trk::LayerMaterialProperties* getHomogeneousMaterial(std::string material_type);

    // Function to overload to parse a node of an XML file
    virtual void parseNode(std::string section, DOMNode *node);

  private:
    // XML templates parsing
    void parseMaterialXML(DOMNode* node);
    void parseComponentXML(DOMNode* node);
    void parseChipXML(DOMNode* node);
    void parseModuleXML(DOMNode* node);
    void parseStaveXML(DOMNode* node, std::vector< StaveTmp *>& vtmp);
    void parseBarrelLayerXML(DOMNode* node, std::vector< BarrelLayerTmp *>& vtmp);
    void parseEndcapXML(DOMNode* node, std::vector< EndcapLayerTmp *>& vtmp);
    // Other processing methods
    void computeModuleSize(ModuleTmp *module);
    // ID Dict generation
    void writeDictionary(std::string filename);
    void openDictFile(std::ofstream& file,std::string filename) const;
    void closeDictFile(std::ofstream& file) const;
    void writePixBarrelDict(std::ofstream& file);
    void writePixInnerEndcapDict(std::ofstream& file);
    void writePixEndcapDict(std::ofstream& file);
    void writeSctBarrelDict(std::ofstream& file);
    void writeSctEndcapDict(std::ofstream& file);
    void writeSctGMXDict(std::ofstream& file);
    // Init stage to be done after initialization!
    void retrieveTools();

    // Data members
    std::string m_dictionaryFileName;
    std::string m_xml_materials;
    std::string m_xml_pixmodules;
    std::string m_xml_pixStaves;
    std::string m_xml_pixBarrelLayers;
    std::string m_xml_pixEndcapLayers;
    std::string m_xml_sctmodules;
    std::string m_xml_sctStaves;
    std::string m_xml_sctBarrelLayers;
    std::string m_xml_sctEndcapLayers;
    std::string m_xml_SLHCVersion;

    std::vector< MaterialTmp *>    m_tmp_material; 
    std::vector< ComponentTmp *>   m_tmp_component; 
    std::vector< ChipTmp *>        m_tmp_chip; 
    std::vector< ModuleTmp *>      m_tmp_module; 
    std::vector< StaveTmp *>       m_tmp_pixStave; 
    std::vector< BarrelLayerTmp *> m_tmp_pixBarrelLayer; 
    std::vector< EndcapLayerTmp *> m_tmp_pixEndcapLayer; 
    std::vector< StaveTmp *>       m_tmp_sctStave; 
    std::vector< BarrelLayerTmp *> m_tmp_sctBarrelLayer; 
    std::vector< EndcapLayerTmp *> m_tmp_sctEndcapLayer; 

    bool m_doPix, m_doSCT, m_isGMX, m_createDict, m_readXMLfromDB;  
  };
}

#endif //INDETTRACKINGGEOMETRY_XMLREADERSVC
