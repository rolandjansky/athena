///////////////////////////////////////////////////////////////////
// XMLReaderSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include <boost/filesystem.hpp>
#include "TMath.h"

InDet::XMLReaderSvc::XMLReaderSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name, svc),
  m_dictionaryFileName("IDTest.xml"),
  m_xml_materials("Materials.xml"),
  m_xml_pixmodules("PixelModules.xml"),
  m_xml_pixStaves("PixelStaves.xml"),
  m_xml_pixBarrelLayers("PixelBarrelLayers.xml"),
  m_xml_pixEndcapLayers("PixelEndcapLayers.xml"),
  m_xml_sctmodules("SCTModules.xml"),
  m_xml_sctStaves("SCTStaves.xml"),
  m_xml_sctBarrelLayers("SCTBarrelLayers.xml"),
  m_xml_sctEndcapLayers("SCTEndcapLayers.xml"),
  m_doPix(true),
  m_doSCT(true),
  m_isGMX(false),
  m_createDict(true),
  m_readXMLfromDB(false)
{
  declareProperty("dictionaryFileName",    m_dictionaryFileName);
  declareProperty("XML_Materials",         m_xml_materials);
  declareProperty("XML_PixelModules",      m_xml_pixmodules);
  declareProperty("XML_PixelStaves",       m_xml_pixStaves);
  declareProperty("XML_PixelBarrelLayers", m_xml_pixBarrelLayers);
  declareProperty("XML_PixelEndcapLayers", m_xml_pixEndcapLayers);
  declareProperty("XML_SCTModules",        m_xml_sctmodules);
  declareProperty("XML_SCTStaves",         m_xml_sctStaves);
  declareProperty("XML_SCTBarrelLayers",   m_xml_sctBarrelLayers);
  declareProperty("XML_SCTEndcapLayers",   m_xml_sctEndcapLayers);
  declareProperty("doPix",                 m_doPix);
  declareProperty("doSCT",                 m_doSCT);
  declareProperty("isGMX",                 m_isGMX);
  declareProperty("createXMLDictionary",   m_createDict);
  declareProperty("readXMLfromDB",         m_readXMLfromDB);
}

InDet::XMLReaderSvc::~XMLReaderSvc()
{
}

// Query the interface
StatusCode InDet::XMLReaderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_XMLReaderSvc == riid )
    *ppvInterface = (XMLReaderSvc *) this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode InDet::XMLReaderSvc::initialize()
{
  ATH_MSG_INFO("InDet::XMLReaderSvc::initialize()  - flag read from DB "<<m_readXMLfromDB);
  if(!InitializeXML()) {
    ATH_MSG_ERROR("Could not initialize XML helper.");
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("XML helper initialized");

  // WARNING: read components BEFORE materials
  //ATH_MSG_INFO("Reading Material templates");
  //parseFile(m_xml_materials.c_str(),"Materials","Component"); 
  //parseFile(m_xml_materials.c_str(),"Materials","Material");
  
  if(m_doPix) {
    // WARNING: read front-end chips BEFORE modules
    ATH_MSG_INFO("Reading pixel FrontEndChip templates");
    parseFile(m_xml_pixmodules.c_str(),"PixelModules","FrontEndChip");
    ATH_MSG_INFO("Reading pixel Module templates");
    parseFile(m_xml_pixmodules.c_str(),"PixelModules","Module");
    ATH_MSG_INFO("Reading Pixel Stave templates");
    parseFile(m_xml_pixStaves.c_str(),"PixelStaves","PixelStave");
    ATH_MSG_INFO("Reading Pixel Barrel Layer templates");
    parseFile(m_xml_pixBarrelLayers.c_str(),"PixelBarrelLayers","PixelBarrelLayer");
    ATH_MSG_INFO("Reading Pixel Endcap Layer templates");
    parseFile(m_xml_pixEndcapLayers.c_str(),"PixelEndcapLayers","PixelEndcapRing");
    parseFile(m_xml_pixEndcapLayers.c_str(),"PixelEndcapLayers","PixelEndcapDisc");
  }
  if(m_doSCT and not m_isGMX) {
    // WARNING: read front-end chips BEFORE modules
    ATH_MSG_INFO("Reading SCT FrontEndChip templates");
    parseFile(m_xml_sctmodules.c_str(),"SCTModules","FrontEndChip");
    ATH_MSG_INFO("Reading SCT Module templates");
    parseFile(m_xml_sctmodules.c_str(),"SCTModules","Module");
    ATH_MSG_INFO("Reading SCT Stave templates");
    parseFile(m_xml_sctStaves.c_str(),"SCTStaves","SCTStave");
    ATH_MSG_INFO("Reading SCT Barrel Layer templates");
    parseFile(m_xml_sctBarrelLayers.c_str(),"SCTBarrelLayers","SCTBarrelLayer");
    ATH_MSG_INFO("Reading SCT Endcap Layer templates");
    parseFile(m_xml_sctEndcapLayers.c_str(),"SCTEndcapLayers","SCTEndcapRing");
    parseFile(m_xml_sctEndcapLayers.c_str(),"SCTEndcapLayers","SCTEndcapDisc");
  }

  if(!TerminateXML()) {
    ATH_MSG_ERROR("Could not terminate XML helper");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("XML helper terminated");
  }

  // Dump XML dictionary
  if(m_createDict) {
    writeDictionary(m_dictionaryFileName);    
  }
  
  return StatusCode::SUCCESS;
}



StatusCode InDet::XMLReaderSvc::finalize()
{
  return StatusCode::SUCCESS;
}

void InDet::XMLReaderSvc::parseNode(std::string section, DOMNode *node)
{
 
  if(section.find("Material")              != std::string::npos) parseMaterialXML(node);
  else if(section.find("Component")        != std::string::npos) parseComponentXML(node);
  else if(section.find("FrontEndChip")     != std::string::npos) parseChipXML(node);
  else if(section.find("Module")           != std::string::npos) parseModuleXML(node);
  else if(section.find("PixelStave")       != std::string::npos) parseStaveXML(node,m_tmp_pixStave);
  else if(section.find("PixelBarrelLayer") != std::string::npos) parseBarrelLayerXML(node,m_tmp_pixBarrelLayer);
  else if(section.find("PixelEndcapRing" ) != std::string::npos) parseEndcapXML(node,m_tmp_pixEndcapLayer);
  else if(section.find("PixelEndcapDisc" ) != std::string::npos) parseEndcapXML(node,m_tmp_pixEndcapLayer);
  else if(section.find("SCTStave")         != std::string::npos) parseStaveXML(node,m_tmp_sctStave);
  else if(section.find("SCTBarrelLayer")   != std::string::npos) parseBarrelLayerXML(node,m_tmp_sctBarrelLayer);
  else if(section.find("SCTEndcapRing")    != std::string::npos) parseEndcapXML(node,m_tmp_sctEndcapLayer);
  else if(section.find("SCTEndcapDisc")    != std::string::npos) parseEndcapXML(node,m_tmp_sctEndcapLayer);
    return;
}


void InDet::XMLReaderSvc::parseMaterialXML(DOMNode* node)
{
  ATH_MSG_DEBUG("Parse material XML info");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_type        = transcode("MaterialType");
  XMLCh* TAG_thickness   = transcode("Thickness");
  XMLCh* TAG_component   = transcode("Component");
  XMLCh* TAG_cfrac       = transcode("Fraction");
  XMLCh* TAG_cname       = transcode("Name");

  MaterialTmp *material = new MaterialTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

   // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
    
    // Look for relevant module parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_type))              material->name  = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_thickness))    material->thickness = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_component)) {
      DOMNodeList* cmpList = currentElement->getChildNodes();
      const XMLSize_t cmpCount = cmpList->getLength();
      for( XMLSize_t xx = 0; xx < cmpCount; ++xx ) {
	DOMNode* cmpNode = cmpList->item(xx);
	if(cmpNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element
	// Found node which is an Element. Re-cast node as element
	DOMElement* cmpElement = dynamic_cast< xercesc::DOMElement* >( cmpNode );
	if( XMLString::equals(cmpElement->getTagName(),TAG_cname))      material->components.push_back(getString(cmpNode));
	else if( XMLString::equals(cmpElement->getTagName(),TAG_cfrac)) material->fractions.push_back(atof(getString(cmpNode)));
      }
    } // end component loop
  } // end material loop

  if((material->name).size()>0) {

    // compute X0, L0, A, Z and density of material based on components properties
    int ncomponents = material->components.size();
    double X0inv = 0.;
    double L0inv = 0.;
    double A = 0.;
    double Z = 0.;
    double rho = 0.;
    
    if(ncomponents==0) {
      ATH_MSG_WARNING("Material " << material->name << " contains 0 components --> Skip");
    }else{ 
      for(int ic =0; ic < ncomponents;ic++){
	ComponentTmp* comp = getComponentTemplate(material->components.at(ic));
	if(comp==0) continue;
	double fraction = material->fractions.at(ic);
	X0inv += fraction/comp->X0;
	L0inv += fraction/comp->L0;
	A += fraction*comp->A;
	Z += fraction*comp->Z;
	rho += fraction*comp->density;

	// note: approximation for X0 computation when data is missing
	// X0 = 716.4/[Z(Z+1).ln(287/sqrt(Z))] g.cm-3
	// Then divide by density to get X0 in cm

      }

      material->X0 = 1./X0inv;
      material->L0 = 1./L0inv;
      material->A = A;
      material->Z = Z;
      material->density = rho;
      
      // Register material template
      m_tmp_material.push_back(material);    
      //material->Print();

    } 
  }

}

void InDet::XMLReaderSvc::parseComponentXML(DOMNode* node)
{
  ATH_MSG_DEBUG("Parse component XML info");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name        = transcode("Name");
  XMLCh* TAG_x0          = transcode("X0");
  XMLCh* TAG_l0          = transcode("L0");
  XMLCh* TAG_A           = transcode("A");
  XMLCh* TAG_Z           = transcode("Z");
  XMLCh* TAG_density     = transcode("Density");

  ComponentTmp *component = new ComponentTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

   // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
    
    // Look for relevant module parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))          component->name    = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_l0))       component->L0      = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_x0))       component->X0      = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_A))        component->A       = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_Z))        component->Z       = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_density))  component->density = atof(getString(currentNode));
  } // end component loop

  // Register material template
  if((component->name).size()>0) {
    m_tmp_component.push_back(component);    
    //component->Print();
  } 
}



void InDet::XMLReaderSvc::parseChipXML(DOMNode* node)
{
  ATH_MSG_DEBUG("Parse chips XML info");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name      = transcode("name");
  XMLCh* TAG_length    = transcode("chipLength");
  XMLCh* TAG_width     = transcode("chipWidth");
  XMLCh* TAG_pitch_eta = transcode("pitchEta");
  XMLCh* TAG_pitch_phi = transcode("pitchPhi");
  XMLCh* TAG_rows      = transcode("rows");
  XMLCh* TAG_columns   = transcode("columns");
  XMLCh* TAG_edges     = transcode("Edges");
  XMLCh* TAG_ewide     = transcode("wide");
  XMLCh* TAG_enarrow   = transcode("narrow");
  XMLCh* TAG_elength   = transcode("inlength");

  ChipTmp *chip = new ChipTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant chip parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))           chip->name     = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))    chip->length   = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_width))     chip->width    = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_pitch_eta)) chip->pitchEta = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_pitch_phi)) chip->pitchPhi = atof(getString(currentNode));    
    else if( XMLString::equals(currentElement->getTagName(), TAG_rows))      chip->rows     = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_columns))   chip->columns  = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_edges)){
      DOMNodeList* edgesList = currentElement->getChildNodes();
      const XMLSize_t edgesCount = edgesList->getLength();
      for( XMLSize_t xx = 0; xx < edgesCount; ++xx ) {
	DOMNode* edgeNode = edgesList->item(xx);
	if(edgeNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element
	// Found node which is an Element. Re-cast node as element
	DOMElement* edgeElement = dynamic_cast< xercesc::DOMElement* >( edgeNode );
	if( XMLString::equals(edgeElement->getTagName(), TAG_ewide))        chip->edgew   = atof(getString(edgeNode));
	else if( XMLString::equals(edgeElement->getTagName(), TAG_enarrow)) chip->edgen   = atof(getString(edgeNode));
	else if( XMLString::equals(edgeElement->getTagName(), TAG_elength)) chip->edgel   = atof(getString(edgeNode));
      }// End of loop on edges node elements
    }
  } // End of loop on chip node elements

  // Register chip template
  if((chip->name).size()>0) {
    m_tmp_chip.push_back(chip);
    if(msgLvl(MSG::DEBUG))
      chip->Print();
  }
}

void InDet::XMLReaderSvc::parseModuleXML(DOMNode* node)
{
  ATH_MSG_DEBUG("Parse modules XML info");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name      = transcode("moduleName");
  XMLCh* TAG_chip      = transcode("chipName");
  XMLCh* TAG_length    = transcode("lengthInChips");
  XMLCh* TAG_widthmax  = transcode("widthMaxInChips");
  XMLCh* TAG_widthmin  = transcode("widthMinInChips");
  XMLCh* TAG_sensthickness = transcode("sensorThickness");
  XMLCh* TAG_chipthickness = transcode("chipThickness");
  XMLCh* TAG_hybdthickness = transcode("hybridThickness");

  ModuleTmp *module = new ModuleTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant module parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))            module->name           = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_chip))       module->chip_type      = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))     module->lengthChips    = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_widthmax))   module->widthMaxChips  = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_widthmin))   module->widthMinChips  = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_sensthickness)){
      module->thickness_sensor  = atof(getString(currentNode));
      module->thickness  += atof(getString(currentNode));
    }
    else if( XMLString::equals(currentElement->getTagName(), TAG_chipthickness))  module->thickness  += atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_hybdthickness))  module->thickness  += atof(getString(currentNode));
  } // End of loop on module node elements

  if(module->chip_type.size()==0) return;
  ChipTmp* chip = getChipTemplate(module->chip_type);
  module->pitchEta = chip->pitchEta;
  module->pitchPhi = chip->pitchPhi;

  // Register module template
  if((module->name).size()>0) {
    computeModuleSize(module);
    m_tmp_module.push_back(module);
    if(msgLvl(MSG::DEBUG))
      module->Print();
  }
 
}

void InDet::XMLReaderSvc::parseStaveXML(DOMNode* node, std::vector< InDet::StaveTmp *>& vtmp)
{
  ATH_MSG_DEBUG("Parse staves XML info");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name        = transcode("Name");
  XMLCh* TAG_type        = transcode("Type");
  XMLCh* TAG_layer       = transcode("Layer");
  XMLCh* TAG_b_modn      = transcode("NBarrelModules");
  XMLCh* TAG_b_type      = transcode("BarrelModuleType");
  XMLCh* TAG_b_gap       = transcode("BarrelModuleGap");
  XMLCh* TAG_b_tilt      = transcode("BarrelModuleTilt");
  XMLCh* TAG_b_angle     = transcode("BarrelModuleAngle");
  XMLCh* TAG_b_sameAngle = transcode("BarrelModuleSameAngle");
  XMLCh* TAG_b_stereoI   = transcode("BarrelModuleStereoAngleInner");
  XMLCh* TAG_b_stereoO   = transcode("BarrelModuleStereoAngleOuter");
  XMLCh* TAG_b_stereoSep = transcode("BarrelModuleStereoSeparation");
  XMLCh* TAG_b_rshift    = transcode("BarrelModuleRShift");
  XMLCh* TAG_trans_tilt  = transcode("TransitionModuleTilt");
  XMLCh* TAG_trans_angle = transcode("TransitionModuleAngle");
  XMLCh* TAG_trans_type  = transcode("TransitionModuleType");
  XMLCh* TAG_trans_pos   = transcode("TransitionPosOnStavePlane");
  XMLCh* TAG_trans_gap   = transcode("TransitionGap");
  XMLCh* TAG_alp_pos     = transcode("MountainPosOnStavePlane");
  XMLCh* TAG_alp_tilt    = transcode("MountainModuleTilt");
  XMLCh* TAG_alp_angle   = transcode("MountainModuleAngle");
  XMLCh* TAG_alp_type    = transcode("MountainModuleType");
  XMLCh* TAG_alp_rshift  = transcode("MountainModuleRShift");
  XMLCh* TAG_alp_radialTilt    = transcode("MountainModuleRadialTilt");
  XMLCh* TAG_material    = transcode("StaveMaterial");
  XMLCh* TAG_length      = transcode("StaveSupportLength");
  XMLCh* TAG_doublesided = transcode("DoubleSided");

  StaveTmp *stave = new StaveTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant stave parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))             stave->name               = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_type))        stave->type               = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_layer))       stave->layer              = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_modn))      stave->b_modn             = int(2*atof(getString(currentNode)));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_type))      stave->b_type             = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_gap))       stave->b_gap              = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_tilt))      stave->b_tilt             = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_angle))     stave->b_angle            = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_sameAngle)) stave->b_sameAngle        = getBoolean(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoI))   stave->b_stereoI          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoO))   stave->b_stereoO          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoSep)) stave->b_stereoSep        = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_rshift))    stave->b_rshift           = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_tilt))  stave->trans_tilt         = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_angle)) stave->trans_angle        = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_type))  stave->trans_type         = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_pos))   stave->trans_pos          = getVectorDouble(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_gap))   stave->trans_gap          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_tilt))    stave->alp_tilt           = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_angle))   stave->alp_angle          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_type))    stave->alp_type           = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_pos))     stave->alp_pos            = getVectorDouble(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_rshift))  stave->alp_rshift         = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_radialTilt))  stave->alp_radialTilt = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))      stave->support_halflength = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_material))    stave->support_material   = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_doublesided)) stave->double_sided       = getBoolean(currentNode);
  } // End of loop on stave node elements

  // Register stave template
  if((stave->name).size()>0 && stave->layer>=0) {
    vtmp.push_back(stave);
    if(msgLvl(MSG::DEBUG))
      stave->Print();
  }
}

void InDet::XMLReaderSvc::parseBarrelLayerXML(DOMNode* node, std::vector< InDet::BarrelLayerTmp *>& vtmp)
{

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_layername      = transcode("Name");
  XMLCh* TAG_layerindex     = transcode("Number");
  XMLCh* TAG_layerradius    = transcode("LayerRadius");
  XMLCh* TAG_nstaves        = transcode("NStaves");
  XMLCh* TAG_stavetype      = transcode("StaveType");
  XMLCh* TAG_stavetilt      = transcode("StaveTilt");
  XMLCh* TAG_stavezoffset   = transcode("StaveZOffset");
  XMLCh* TAG_phioffset      = transcode("LayerPhiOfModuleZero");

  BarrelLayerTmp *layer = new BarrelLayerTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant layer parameters
    if (XMLString::equals(currentElement->getTagName(),TAG_layername))         layer->name          = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerindex))   layer->index         = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerradius))  layer->radius        = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_nstaves))      layer->stave_n       = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavetype))    layer->stave_type    = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavetilt))    layer->stave_tilt    = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavezoffset)) layer->stave_zoffset = atof(getString(currentNode))/2.0;
    else if (XMLString::equals(currentElement->getTagName(),TAG_phioffset))    layer->phioffset     = atof(getString(currentNode));
  } // End of loop on layer node elements

  // Register layer template 
  if((layer->name).size()>0 && layer->index>=0) {
    unsigned int min_size = layer->index+1;
    if(vtmp.size()<min_size) vtmp.resize(min_size,0);
    vtmp.at(layer->index) = layer;
    if(msgLvl(MSG::DEBUG))
      layer->Print();
  }
}

void InDet::XMLReaderSvc::parseEndcapXML(DOMNode* node, std::vector< InDet::EndcapLayerTmp *>& vtmp)
{

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name            = transcode("Name");
  XMLCh* TAG_layernum        = transcode("LayerNumber");
  XMLCh* TAG_layerpos        = transcode("LayerPosition");
  XMLCh* TAG_discnum         = transcode("DiscNumber");
  XMLCh* TAG_radius          = transcode("Radius");
  XMLCh* TAG_radiustype      = transcode("RadiusType");
  XMLCh* TAG_nsectors        = transcode("NumberOfSectors");
  XMLCh* TAG_modtype         = transcode("RingModuleType");
  XMLCh* TAG_zoffset         = transcode("RingModuleZSeparation");
  XMLCh* TAG_phioffset       = transcode("RingPhiOfModuleZero");
  XMLCh* TAG_material        = transcode("RingSupportMaterial");
  XMLCh* TAG_ringpos         = transcode("RingPositions");
  XMLCh* TAG_stereoInner     = transcode("RingModuleStereoAngleInner");
  XMLCh* TAG_stereoOuter     = transcode("RingModuleStereoAngleOuter");
  XMLCh* TAG_stereoSep       = transcode("RingModuleStereoSeparation");
  XMLCh* TAG_doublesided     = transcode("DoubleSided");
  XMLCh* TAG_useDiscSurface  = transcode("UseDiscSurface");
  XMLCh* TAG_splitMode       = transcode("SplitMode");
  XMLCh* TAG_splitOffset     = transcode("SplitOffset");


  // temporary variables
  std::string name;
  int layer_index = -1; 
  int layer_pos   = -1; 
  int disc_index  = -1; 
  std::string material = "DefaultPixelRingMaterial";
  std::string rtype;
  double stereoInner = 0.;
  double stereoOuter = 0.;
  double stereoSep = 0.;
  bool double_sided = false;
  bool usediscsurf  = false;

  std::vector<std::string> tmpmodtype;
  std::vector<double> tmpradius;
  std::vector<double> tmpringpos;
  std::vector<int>    tmpnsectors;
  std::vector<std::string>  tmpsplitmode;
  std::vector<double>       tmpsplitoffset;
  std::vector<double>       tmpzoffset;
  std::vector<double>       tmpphioffset;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {

    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant layer parameters
    if (XMLString::equals(currentElement->getTagName(),TAG_name))                 name          = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_layernum))        layer_index   = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerpos))        layer_pos     = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_discnum))         disc_index    = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_material))        material      = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_radiustype))      rtype         = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoInner))     stereoInner   = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoOuter))     stereoOuter   = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoSep))       stereoSep     = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_doublesided))     double_sided  = getBoolean(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_useDiscSurface))  usediscsurf   = getBoolean(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_modtype))         tmpmodtype    = getVectorString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_radius))          tmpradius     = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_nsectors))        tmpnsectors   = getVectorInt(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_splitMode))       tmpsplitmode  = getVectorString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_splitOffset))     tmpsplitoffset= getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_zoffset))         tmpzoffset    = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_phioffset))       tmpphioffset  = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_ringpos))         tmpringpos    = getVectorDouble(currentNode);
  }

  // If different number of entries for rings fields, use the values of ring 0 everywhere
  unsigned int nrings = tmpringpos.size();
  if(tmpmodtype.size() != nrings && tmpmodtype.size()>0 ){
    std::string modtype = tmpmodtype.at(0);
    tmpmodtype.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpmodtype.push_back(modtype);
  }else if(tmpmodtype.size() == 0){
    ATH_MSG_WARNING("No module type defined for rings --> No template built");
    return;
  }

  if(tmpzoffset.size()>0){
    if(tmpzoffset.size() != nrings){
      double zoffs = tmpzoffset.at(0); 
      tmpzoffset.clear();
      for (unsigned int ir = 0 ; ir < nrings; ir++) tmpzoffset.push_back(zoffs);
    }
  }else{
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpzoffset.push_back(0.);    
  }
  
  if(tmpphioffset.size()>0){
    if(tmpphioffset.size() != nrings){
      double phioffs = tmpphioffset.at(0); 
      tmpphioffset.clear();
      for (unsigned int ir = 0 ; ir < nrings; ir++) tmpphioffset.push_back(phioffs);
    }
  }else{
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpphioffset.push_back(0.);    
  }
  
  if(tmpnsectors.size() != nrings && tmpnsectors.size()>0){
    double nsec = tmpnsectors.at(0);
    tmpnsectors.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpnsectors.push_back(nsec);
  }else if (tmpnsectors.size() == 0){
    ATH_MSG_WARNING("Number of sectors not defined for rings --> No template built");
      return;
  }
  
  if(tmpsplitmode.size() != nrings && tmpsplitmode.size()>0){
    std::string split = tmpsplitmode.at(0);
    tmpsplitmode.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) {
      tmpsplitmode.push_back(split);
    }
  } else {
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpsplitmode.push_back("None");
  }
  
  if(tmpsplitoffset.size() != nrings && tmpsplitoffset.size()>0){
    double splitoffs = tmpsplitoffset.at(0);
    tmpsplitoffset.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) {
      tmpsplitoffset.push_back(splitoffs);
    }
  } else {
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpsplitoffset.push_back(0.);
  }

  for (unsigned int ir = 0 ; ir < nrings; ir++) {
    if ((tmpsplitmode.at(ir)!= "None" and tmpsplitoffset.at(ir)==0.) or (tmpsplitmode.at(ir)== "None" and tmpsplitoffset.at(ir)!=0.)) {
      ATH_MSG_WARNING("Split offset == "<< tmpsplitoffset.at(ir) << " not compatible with split mode " << tmpsplitmode.at(ir) <<"... Moving to split mode = 'None' with split offset == 0.0");
      tmpsplitmode.at(ir)="None";
      tmpsplitoffset.at(ir)=0.;
    }
  }
  
  if(tmpradius.size() != nrings && tmpradius.size()>0){
    double rad = tmpradius.at(0);
    tmpradius.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpradius.push_back(rad);
  }else if (tmpradius.size() == 0 ){
    ATH_MSG_WARNING("Rings radii not defined--> No template built");
    return;
  }

  EndcapLayerTmp *layer = new EndcapLayerTmp;
  
  layer->name                = name;
  layer->support_material    = material; 
  layer->double_sided        = double_sided;
  layer->stereoI             = stereoInner;
  layer->stereoO             = stereoOuter;
  layer->stereoSep           = stereoSep;
  layer->useDiscSurface      = usediscsurf;
  // disc layer
  if(layer_index<0) {
    layer->ilayer = disc_index; 
    layer->layer_pos = disc_index; 
    layer->isDisc = true;  
  // ring layer
  } else {
    layer->ilayer = layer_index; 
    layer->isDisc = false;
    if(layer_pos<0) layer->layer_pos = layer_index;
    else layer->layer_pos = layer_pos;
  }

  // loop over rings to store the info in template
  for(unsigned int ir=0;ir<tmpringpos.size();ir++){

    // retrieve module info to compute inner/outer rings radii 
    ModuleTmp* moduleTmp = getModuleTemplate(tmpmodtype.at(ir));

    if(moduleTmp == 0) {
      ATH_MSG_WARNING("Module type " <<  tmpmodtype.at(ir)<< " is not defined --> Skipping ring template " << ir << " Layer " << layer_index ); return;
    }

    double modlength = moduleTmp->length;
    double modthick  = moduleTmp->thickness;

    layer->modtype.push_back(tmpmodtype.at(ir));
    layer->ringpos.push_back(tmpringpos.at(ir));
    layer->zoffset.push_back(tmpzoffset.at(ir));
    layer->phioffset.push_back(tmpphioffset.at(ir));
    layer->nsectors.push_back(tmpnsectors.at(ir));
    layer->splitMode.push_back(tmpsplitmode.at(ir));
    layer->splitOffset.push_back(tmpsplitoffset.at(ir));

    // compute ring radii
    double innerRadius = tmpradius.at(ir);
    double outerRadius = tmpradius.at(ir);
    if (rtype.compare("Inner")==0) outerRadius += modlength;
    else innerRadius -= modlength;
    layer->innerRadius.push_back(innerRadius);
    layer->outerRadius.push_back(outerRadius);
    // compute ring thickness (module thickness + zoffset (can be negative))
    double tmpthick  = modthick + std::fabs(tmpzoffset.at(ir)); 
    if(double_sided) tmpthick += modthick+2*stereoSep;
    layer->thickness.push_back(tmpthick);
  }

  // Register endcap layer template
  vtmp.push_back(layer);
  if(msgLvl(MSG::DEBUG))
    layer->Print();

  return;

}

void InDet::XMLReaderSvc::computeModuleSize(InDet::ModuleTmp *module)
{

  if(module == 0) return;
  if(module->chip_type.size()==0) return;

  ChipTmp* chip = getChipTemplate(module->chip_type);
  module->length = module->lengthChips*chip->length+2*chip->edgel; 
  if(module->widthMinChips==0) module->widthMinChips = module->widthMaxChips;
  module->widthmin  = module->widthMinChips*chip->width+chip->edgew; 
  module->widthmax  = module->widthMaxChips*chip->width+chip->edgew; 

  if(chip->name.find("RD53") != std::string::npos) {
    module->length = module->lengthChips*chip->length+(module->lengthChips-1)*2.*chip->edgel;
    module->widthmax = module->widthMaxChips*chip->width+(module->widthMaxChips-1)*2.*chip->edgew;
    module->widthmin = module->widthmax;
  }
  else {
    if(module->chip_type=="SingleChip"){
      module->widthmin += chip->edgen;
      module->widthmax += chip->edgen;
    }else{
      module->widthmin += chip->edgew;
      module->widthmax += chip->edgew;
    }
  }
}

InDet::MaterialTmp* InDet::XMLReaderSvc::getMaterialTemplate(const std::string s) const
{
  // Get material template from list
  unsigned int n = m_tmp_material.size();

  for(unsigned int i=0;i<n;i++) {
    MaterialTmp *material = m_tmp_material.at(i);
    if(!material) continue;
    if(s.compare(material->name)) continue;
    return material;
  }
  ATH_MSG_WARNING("InDet Material type '" << s << "' is not defined");
  return 0;
}

InDet::ComponentTmp* InDet::XMLReaderSvc::getComponentTemplate(const std::string s) const
{
  // Get material template from list
  unsigned int n = m_tmp_component.size();

  for(unsigned int i=0;i<n;i++) {
    ComponentTmp *component = m_tmp_component.at(i);
    if(!component) continue;
    if(s.compare(component->name)) continue;
    return component;
  }
  ATH_MSG_WARNING("InDet Component type '" << s << "' is not defined");
  return 0;
}

InDet::ModuleTmp* InDet::XMLReaderSvc::getModuleTemplate(const std::string s) const
{
  // Get module template from list
  unsigned int n = m_tmp_module.size();

  for(unsigned int i=0;i<n;i++) {
    ModuleTmp *module = m_tmp_module.at(i);
    if(!module) continue;
    if(s.compare(module->name)) continue;
    return module;
  }
  ATH_MSG_WARNING("InDet Module type '" << s << "' is not defined");
  return 0;
}

InDet::ChipTmp* InDet::XMLReaderSvc::getChipTemplate(const std::string s) const
{
  // Get chip template from list
  unsigned int n = m_tmp_chip.size();
  for(unsigned int i=0;i<n;i++) {
    ChipTmp *chip = m_tmp_chip.at(i);
    if(!chip) continue;
    if(s.compare(chip->name)) continue;
    return chip;
  }

  ATH_MSG_WARNING("InDet Chip type '" << s << "' is not defined");
  return 0;
}

std::vector<InDet::StaveTmp*> InDet::XMLReaderSvc::getPixelStaveTemplate(unsigned int ilayer) const
{

  std::vector<InDet::StaveTmp*> staves;

  // Get staves from template list
  unsigned int n = m_tmp_pixStave.size();
  for(unsigned int istave=0;istave<n;istave++){
    StaveTmp *stave = m_tmp_pixStave.at(istave);
    if(stave->layer == int(ilayer)) staves.push_back(stave);
  } 
 
  if(staves.size()==0) ATH_MSG_WARNING("InDet no Pixel Stave template found for layer " << ilayer);
  
  return staves;
}

std::vector<InDet::StaveTmp*> InDet::XMLReaderSvc::getSCTStaveTemplate(unsigned int ilayer) const
{
  std::vector<InDet::StaveTmp*> staves;

  // Get staves from template list
  unsigned int n = m_tmp_sctStave.size();
  for(unsigned int istave=0;istave<n;istave++){
    StaveTmp *stave = m_tmp_sctStave.at(istave);
    if(stave->layer == int(ilayer)) staves.push_back(stave);
  } 
 
  if(staves.size()==0) ATH_MSG_WARNING("InDet no SCT Stave template found for layer " << ilayer);
  
  return staves;

}


InDet::BarrelLayerTmp* InDet::XMLReaderSvc::getPixelBarrelLayerTemplate(unsigned int ilayer) const
{
  // Get layer index from list
  unsigned int n = m_tmp_pixBarrelLayer.size();

  if(ilayer>n-1) {
    ATH_MSG_WARNING("InDet Pixel barrel template for layer " << ilayer << " is not defined");
    return 0;
  } 

  BarrelLayerTmp *layer = m_tmp_pixBarrelLayer.at(ilayer);
  return layer;
}

InDet::BarrelLayerTmp* InDet::XMLReaderSvc::getSCTBarrelLayerTemplate(unsigned int ilayer) const
{
  // Get layer index from list
  unsigned int n = m_tmp_sctBarrelLayer.size();

  if(ilayer>n-1) {
    ATH_MSG_WARNING("InDet SCT barrel template for layer " << ilayer << " is not defined");
    return 0;
  } 

  BarrelLayerTmp *layer = m_tmp_sctBarrelLayer.at(ilayer);
  return layer;
}

InDet::EndcapLayerTmp* InDet::XMLReaderSvc::getPixelEndcapLayerTemplate(unsigned int ilayer) const
{
  // Get layer index from list
  unsigned int n = m_tmp_pixEndcapLayer.size();

  if(ilayer>n-1) {
    ATH_MSG_WARNING("InDet Pixel endcap template for layer " << ilayer << " is not defined");
    return 0;
  } 

  EndcapLayerTmp *layer = m_tmp_pixEndcapLayer.at(ilayer);
  return layer;
}

InDet::EndcapLayerTmp* InDet::XMLReaderSvc::getSCTEndcapLayerTemplate(unsigned int ilayer) const
{
  // Get layer index from list
  unsigned int n = m_tmp_sctEndcapLayer.size();

  if(ilayer>n-1) {
    ATH_MSG_WARNING("InDet SCT endcap template for layer " << ilayer << " is not defined");
    return 0;
  } 

  EndcapLayerTmp *layer = m_tmp_sctEndcapLayer.at(ilayer);
  return layer;
}

void InDet::XMLReaderSvc::writeDictionary(std::string filename)
{
  
  std::ofstream file;
  // look for the directory
  std::size_t found = filename.find("/");
  if (found!=std::string::npos) {
    std::string folder = filename.substr(0, found);
    boost::filesystem::path dir(folder);
    if(!(boost::filesystem::exists(dir))) {
        ATH_MSG_INFO("Directory " << folder << " doesn't exist... Creating it!");
        if (boost::filesystem::create_directory(dir))
            ATH_MSG_INFO("Directory " << folder << " created!");
    }
  }	
  openDictFile(file,filename);
  writePixBarrelDict(file);
  writePixEndcapDict(file);

  if (m_doSCT and !m_isGMX) {
    writeSctBarrelDict(file);
    writeSctEndcapDict(file);
  }
  
  closeDictFile(file);
}

void InDet::XMLReaderSvc::openDictFile(std::ofstream& file,std::string filename) const
{
  file.open(filename);
  file << "<!--\n\n   The InnerDetector identifier is arranged as follows\n\n\
   InnerDetector\n\
     part=Pixel\n\
       side=barrel\n\
         layer phi_module eta_module phi_index eta_index\n\
       side=endcap\n\
         disk phi_module eta_module phi_index eta_index\n\
     part=SCT\n\
       side=barrel\n\
         layer phi_module eta_module side strip\n\
       side=endcap\n\
         disk phi_module eta_module side strip\n\
     part=TRT\n\
       side=barrel\n\
         layer phi_module straw_layer straw\n\
       side=endcap\n\
         wheel phi_module straw_layer straw\n\n  -->\n\
\n\
<IdDictionary name=\"InnerDetector\" version=\"SLHC\" >\n\
  <field name=\"part\" >\n\
    <label name=\"Pixel\" value=\"1\" />\n\
    <label name=\"SCT\"   value=\"2\" />\n\
    <label name=\"TRT\"   value=\"3\" />\n\
  </field>\n\
\n\
  <field name=\"barrel_endcap\">\n\
    <label name=\"negative_endcap\" value=\"-2\" />\n\
    <label name=\"negative_barrel\" value=\"-1\" />\n\
    <label name=\"barrel\"          value=\"0\" />\n\
    <label name=\"positive_barrel\" value=\"+1\"  />\n\
    <label name=\"positive_endcap\" value=\"+2\"  />\n\
  </field>\n\
\n " << std::endl;
}

void InDet::XMLReaderSvc::closeDictFile(std::ofstream& file) const
{
  if (!m_isGMX)
    file << "</IdDictionary>" << std::endl;
  file.close();
}

void InDet::XMLReaderSvc::writeSctEndcapDict(std::ofstream& file)
{
  // CREMI SI Pixel endcap part comments
  // Loop on SCT Endcap layers

  for(unsigned int i=0; i<m_tmp_sctEndcapLayer.size();i++) {
    EndcapLayerTmp *layer = m_tmp_sctEndcapLayer.at(i);
    if(layer==0) continue;
    
    // Make one region per ring of the disc
    for(unsigned int iring=0;iring<layer->nsectors.size();iring++) {
      ModuleTmp* module = getModuleTemplate(layer->modtype[iring]);
      if(module==0) {
        ATH_MSG_WARNING("Indet SCT : no module template found for disk " << i << " and ring " << iring);
        continue;
      }      
      file << "  <region group=\"sct\" >" << std::endl;
      file << "    <range field=\"part\" value=\"SCT\" />" << std::endl;
      file << "    <range field=\"barrel_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"disk\" value=\"" << layer->ilayer << "\" />" << std::endl;
      file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" << layer->nsectors[iring]-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"eta_module\" value=\"" << iring << "\" />" << std::endl;
      file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
      int iphi = 0;
      iphi = module->widthmin/module->pitchPhi;
      file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"" << iphi << "\" />" << std::endl;
      file << "  </region>" << std::endl;
    }
  }
  file << std::endl;
}

void InDet::XMLReaderSvc::writePixEndcapDict(std::ofstream& file)
{
  // CREMI Should be one entry per ring instead of per disc
  // ilayer, iphi=isector [0-nsectors[iring]-1], ieta=iring [0-ringpos.size()-1]b

  // Loop on Pixel Endcap layers
  for(unsigned int i=0; i<m_tmp_pixEndcapLayer.size();i++) {
    EndcapLayerTmp *layer = m_tmp_pixEndcapLayer.at(i);
    if(layer==0) continue;
    ModuleTmp* module = getModuleTemplate(layer->modtype[0]);
    if(module==0) {
      ATH_MSG_WARNING("Indet PIX : no module template found for layer " << i << "");
      continue;
    }

    // Make one region per ring of the disc
    for(unsigned int iring=0;iring<layer->nsectors.size();iring++) {
      file << "  <region group=\"pixel\" >" << std::endl;
      file << "    <range field=\"part\" value=\"Pixel\" />" << std::endl;
      file << "    <range field=\"barrel_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"disk\" value=\"" << layer->ilayer << "\" />" << std::endl;
      file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" << layer->nsectors[iring]-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"eta_module\" value=\"" << iring << "\" />" << std::endl;
      int iphi = module->widthmin/module->pitchPhi;
      file << "    <range field=\"phi_index\" minvalue=\"0\" maxvalue=\"" << iphi << "\" />" << std::endl;
      int ieta = module->length/module->pitchEta;
      file << "    <range field=\"eta_index\" minvalue=\"0\" maxvalue=\"" << ieta << "\" />" << std::endl;
      file << "  </region>" << std::endl;
    }
  }
  file << std::endl;
}

void InDet::XMLReaderSvc::writePixBarrelDict(std::ofstream& file)
{
  // Loop on Pixel Barrel layers
  for(unsigned int i=0; i<m_tmp_pixBarrelLayer.size();i++) {
    BarrelLayerTmp *layer = m_tmp_pixBarrelLayer.at(i);
    if(layer==0) continue;
    std::vector<StaveTmp*> staves = getPixelStaveTemplate(layer->index);
    if(staves.size()==0) continue;

    // JL: this is dirty for now -- need to think a bit more if different number of module on each stave type
    StaveTmp* stave = staves.at(0);
    //
    ModuleTmp* b_module = 0;
    ModuleTmp* t_module = 0;
    ModuleTmp* a_module = 0;
    if(stave->b_modn >0) b_module = getModuleTemplate(stave->b_type);
    if(stave->trans_pos.size()>0) t_module = getModuleTemplate(stave->trans_type);
    if(stave->alp_pos.size()>0) a_module = getModuleTemplate(stave->alp_type);

    if(b_module==0 && t_module==0 && a_module==0) continue;
    ModuleTmp* module = b_module;
    if(module==0 && a_module !=0 ) module = a_module;
    if(module==0 && t_module !=0 ) module = t_module;  
    
    file << "  <region group=\"pixel\" >" << std::endl;
    file << "    <range field=\"part\" value=\"Pixel\" />" << std::endl;
    file << "    <range field=\"barrel_endcap\" value=\"barrel\" />" << std::endl;
    file << "    <range field=\"layer\" value=\"" << layer->index << "\" />" << std::endl;
    file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" << layer->stave_n-1 << "\" wraparound=\"TRUE\" />" << std::endl;
    int neta = 0;
    neta += stave->b_modn; // For Pixels 1 id = 1 module
    neta += 4*stave->trans_pos.size(); // 1 transition = 2 modules * 2 sides = 4 modules
    neta += 2*stave->alp_pos.size(); // 1 mountain = 1 module * 2 sides = 2 modules
    int iend   =  neta/2;
    int istart = -iend;
    file << "    <range field=\"eta_module\" values=\"";
    for(int i=istart;i<=iend;i++) {
      if(i==0 && (neta%2)==0) continue;
      file << i << " ";
    }
    file << "\" />" << std::endl;
    //file << "    <range field=\"eta_module\" minvalue=\"-" << neta << "\" maxvalue=\"" << neta << "\" />" << std::endl;
    int iphi = b_module->widthmin/b_module->pitchPhi;
    file << "    <range field=\"phi_index\" minvalue=\"0\" maxvalue=\"" << iphi << "\" />" << std::endl;
    int ieta = b_module->length/b_module->pitchEta;
    file << "    <range field=\"eta_index\" minvalue=\"0\" maxvalue=\"" << ieta << "\" />" << std::endl;
    file << "  </region>" << std::endl;
  }
  file << std::endl;
}

void InDet::XMLReaderSvc::writeSctBarrelDict(std::ofstream& file)
{
  // Loop on SCT Barrel layers
  for(unsigned int i=0; i<m_tmp_sctBarrelLayer.size();i++) {
    BarrelLayerTmp *layer = m_tmp_sctBarrelLayer.at(i);
    if(layer==0) continue;
    std::vector<StaveTmp*> staves = getSCTStaveTemplate(layer->index);
    if(staves.size()==0) continue;
    // JL: this is dirty for now -- need to think a bit more 
    StaveTmp* stave = staves.at(0);
    //
    ModuleTmp* b_module = 0;
    ModuleTmp* t_module = 0;
    ModuleTmp* a_module = 0;
    if(stave->b_modn >0) b_module = getModuleTemplate(stave->b_type);
    if(stave->trans_pos.size()>0) t_module = getModuleTemplate(stave->trans_type);
    if(stave->alp_pos.size()>0) a_module = getModuleTemplate(stave->alp_type);

    if(b_module==0 && t_module==0 && a_module==0) continue;

    file << "  <region group=\"sct\" >" << std::endl;
    file << "    <range field=\"part\" value=\"SCT\" />" << std::endl;
    file << "    <range field=\"barrel_endcap\" value=\"barrel\" />" << std::endl;
    file << "    <range field=\"layer\" value=\"" << layer->index << "\" />" << std::endl;
    file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" <<  layer->stave_n-1 << "\" wraparound=\"TRUE\" />" << std::endl;
    int neta = 0;
    if(b_module) neta += stave->b_modn*b_module->lengthChips; // For SCT 1 id = 1 chip
    if(t_module) neta += 4*stave->trans_pos.size()*t_module->lengthChips; // 1 transition = 2 modules * 2 sides = 4 modules
    if(a_module) neta += 2*stave->alp_pos.size()*a_module->lengthChips; // 1 mountain = 1 module * 2 sides = 2 modules
    
    int iend   =  neta/2;
    int istart = -iend;
    file << "    <range field=\"eta_module\" values=\"";
    for(int i=istart;i<=iend;i++) {
      if(i==0 && (neta%2)==0) continue;
      file << i << " ";
    }
    file << "\" />" << std::endl;
    //file << "    <range field=\"eta_module\" minvalue=\"-" << neta << "\" maxvalue=\"" << neta << "\" />" << std::endl;
    
    file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
    int iphi = 0;
    if(b_module) { 
      iphi = b_module->widthmin/b_module->pitchPhi;
    }
    if(t_module) {
      iphi = t_module->widthmin/t_module->pitchPhi;
    }
    if(a_module) {
      iphi = a_module->widthmin/a_module->pitchPhi;    
    }
    
    file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"" << iphi << "\" />" << std::endl;
    file << "  </region>" << std::endl;
  }
  file << std::endl;
}

void InDet::XMLReaderSvc::writeSctGMXDict(std::ofstream& file) {
  
  file << "  <subregion name=\"SCT_barrel\"> " << std::endl;
  file << "    <range field=\"part\" value=\"SCT\" /> " << std::endl;
  file << "    <range field=\"barrel_endcap\" value=\"barrel\" /> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCTBarrel_layer0_phi\"> " << std::endl;
  file << "    <range field=\"layer_disk\" value=\"0\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"27\" wraparound=\"TRUE\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTBarrel_layer1_phi\"> " << std::endl;
  file << "    <range field=\"layer_disk\" value=\"1\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"39\" wraparound=\"TRUE\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTBarrel_layer2_phi\"> " << std::endl;
  file << "    <range field=\"layer_disk\" value=\"2\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"51\" wraparound=\"TRUE\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTBarrel_layer3_phi\"> " << std::endl;
  file << "    <range field=\"layer_disk\" value=\"3\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"71\" wraparound=\"TRUE\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"side\"> " << std::endl;
  file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" /> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCT_NegativeStave_eta_side\"> " << std::endl;
  file << "    <range field=\"eta_module\" minvalue=\"-14\" maxvalue=\"-1\" next_value=\"1\"/> " << std::endl; 
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCT_PositiveStave_eta_side\"> " << std::endl;
  file << "    <range field=\"eta_module\" minvalue=\"1\" maxvalue=\"14\" prev_value=\"-1\"/> " << std::endl; 
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCTBarrel_strip\"> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1279\" /> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"ShortStrip_row_strip\"> " << std::endl; 
  file << "    <range field=\"row\"   minvalue=\"0\" maxvalue=\"3\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_strip\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"MediumStrip_row_strip\"> " << std::endl;
  file << "    <range field=\"row\"   minvalue=\"0\" maxvalue=\"1\"/> " << std::endl; 
  file << "    <reference subregion=\"SCTBarrel_strip\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_NegativeBarrel0\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer0_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_NegativeStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"ShortStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_NegativeBarrel1\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer1_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_NegativeStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"ShortStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_NegativeBarrel2\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer2_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_NegativeStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"MediumStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_NegativeBarrel3\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer3_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_NegativeStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"MediumStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_PositiveBarrel0\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer0_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_PositiveStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"ShortStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_PositiveBarrel1\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer1_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_PositiveStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"ShortStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_PositiveBarrel2\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer2_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_PositiveStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"MediumStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCT_PositiveBarrel3\"> " << std::endl;  
  file << "    <reference subregion=\"SCT_barrel\"/> " << std::endl;
  file << "    <reference subregion=\"SCTBarrel_layer3_phi\"/> " << std::endl;
  file << "    <reference subregion=\"SCT_PositiveStave_eta_side\"/> " << std::endl;
  file << "    <reference subregion=\"MediumStrip_row_strip\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <!--  SCT Endcaps  --> " << std::endl;
  file << "  " << std::endl;
  file << "  <!--                                                                                 " << std::endl;
  file << "    Petal structure: each petal has 6 rings of sensors with variable numbers of rows:  " << std::endl;
  file << "  ___________________________                                                          " << std::endl;
  file << "  \\            | 2          /                                                          " << std::endl;
  file << "   \\ __________|___________/      -> last three rings have 64 sensors                  " << std::endl;
  file << "    \\          | 2        /                                                            " << std::endl;
  file << "     \\ ________|_________/                                                             " << std::endl;
  file << "      \\        | 4      /                                                              " << std::endl;
  file << "       \\ ______|_______/                                                               " << std::endl;
  file << "        \\      2      /                                                                " << std::endl;
  file << "         \\___________/     -> first three rings have 32 sensors                        " << std::endl;
  file << "          \\    4    /                                                                  " << std::endl;
  file << "           \\_______/                                                                   " << std::endl;
  file << "            \\  4  /                                                                    " << std::endl;
  file << "             \\___/                                                                     " << std::endl;
  file << "  " << std::endl;
  file << "  --> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCTEndcap_layer\"> " << std::endl;
  file << "    <range field=\"part\" value=\"SCT\"/> " << std::endl;
  file << "    <range field=\"barrel_endcap\" values=\"negative_endcap positive_endcap\"/> " << std::endl;
  file << "    <range field=\"layer\" minvalue=\"0\" maxvalue=\"5\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCTEndcap_innerRing\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_layer\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"31\" wraparound=\"TRUE\"/> " << std::endl;	
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_outerRing\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_layer\"/> " << std::endl;
  file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"63\" wraparound=\"TRUE\"/> " << std::endl;	
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring0\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_innerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"0\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring1\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_innerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"1\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring2\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_innerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"2\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring3\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_outerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"3\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring4\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_outerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"4\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  <subregion name=\"SCTEndcap_ring5\"> " << std::endl;
  file << "    <reference subregion=\"SCTEndcap_outerRing\"/> " << std::endl;
  file << "    <range field=\"eta_module\" value=\"5\"/> " << std::endl;	
  file << "    <reference subregion=\"side\"/> " << std::endl;
  file << "  </subregion> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring0_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring0\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1023\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring0_hybrid1\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring0\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"2\" maxvalue=\"3\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1151\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring1_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring1\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1279\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring1_hybrid1\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring1\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"2\" maxvalue=\"3\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1407\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring2_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring2\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1535\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring3_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring3\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"895\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring3_hybrid1\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring3\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"2\" maxvalue=\"3\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"895\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring4_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring4\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1023\"/> " << std::endl;
  file << "  </region> " << std::endl;
  file << "  " << std::endl;
  file << "  <region group=\"sct\" name=\"SCTEndcap_ring5_hybrid0\"> " << std::endl; 
  file << "    <reference subregion=\"SCTEndcap_ring5\"/> " << std::endl;
  file << "    <range field=\"row\" minvalue=\"0\" maxvalue=\"1\"/> " << std::endl;
  file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1151\"/> " << std::endl;
  file << "  </region> " << std::endl;
  
}

double InDet::XMLReaderSvc::getHalfPlainLength(InDet::StaveTmp *staveTmp) const
{
  if(staveTmp==0) return 0;

  // Number of modules
  int nmodplain = staveTmp->b_modn;
  int nmodtrans = 2*staveTmp->trans_pos.size();
  int NMOD = nmodtrans + nmodplain;

  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = getModuleTemplate(staveTmp->b_type);
  if(plainModuleTmp==0) return 0;
  double moduleplain_length = plainModuleTmp->length;
  double moduleplain_gap = staveTmp->b_gap;

  // the bins for transition modules are too long, it's normal
  double halfplainlength = 0.5*(NMOD*moduleplain_length + (NMOD-1)*moduleplain_gap);
  return halfplainlength;
}

double InDet::XMLReaderSvc::getZStep(InDet::StaveTmp *staveTmp) const
{
  if(staveTmp==0) return 0;

  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = getModuleTemplate(staveTmp->b_type);
  if(plainModuleTmp==0) return 0;
  double moduleplain_length = plainModuleTmp->length;
  double moduleplain_gap = staveTmp->b_gap;

  // the bins for transition modules are too long, it's normal
  double zstep = moduleplain_length + moduleplain_gap; 
  return zstep;
}

void InDet::XMLReaderSvc::computeZbounds(Amg::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &active_halflength) const
{

  std::vector<Amg::Vector3D >corners;

  // coordinates before any transform. 
  double xmod = moduleTmp->widthmax*.5;
  double ymod = moduleTmp->length*.5;
  double zmod = moduleTmp->thickness*.5;

  // top corners
  corners.push_back( Amg::Vector3D (-xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod,  zmod));

  // bottom corners
  corners.push_back( Amg::Vector3D (-xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, -ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod,  zmod));

  double vMin=9999.;
  double vMax=-9999.;

  for(unsigned int i=0; i<corners.size(); i++)
    {
      Amg::Vector3D p = trf*corners[i];
      double z = p.z();
      vMin = std::min(vMin,z);
      vMax = std::max(vMax,z);
    }

  active_halflength = std::max(std::fabs(vMin),active_halflength);
  active_halflength = std::max(std::fabs(vMax),active_halflength);

}

void InDet::XMLReaderSvc::computeRbounds(Amg::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &rMin, double &rMax) const
{

  std::vector<Amg::Vector3D >corners;

  // coordinates before any transform. 
  double xmod = moduleTmp->widthmax*.5;
  double ymod = moduleTmp->length*.5;
  double zmod = moduleTmp->thickness*.5;

  // top corners
  corners.push_back( Amg::Vector3D (-xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, ymod,  zmod));

  // bottom corners
  corners.push_back( Amg::Vector3D (-xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D (-xmod, -ymod,  zmod));
  corners.push_back( Amg::Vector3D ( xmod, -ymod,  zmod));

  // faces
  corners.push_back( Amg::Vector3D ( 0.5*xmod, ymod,  zmod));
  corners.push_back( Amg::Vector3D ( 0.5*xmod, ymod, -zmod));
  corners.push_back( Amg::Vector3D (-0.5*xmod, ymod,  zmod));
  corners.push_back( Amg::Vector3D (-0.5*xmod, ymod, -zmod));

  corners.push_back( Amg::Vector3D ( 0.5*xmod, -ymod,  zmod));
  corners.push_back( Amg::Vector3D ( 0.5*xmod, -ymod, -zmod));
  corners.push_back( Amg::Vector3D (-0.5*xmod, -ymod,  zmod));
  corners.push_back( Amg::Vector3D (-0.5*xmod, -ymod, -zmod));

  double vMin=9999.;
  double vMax=-9999.;

  for(unsigned int i=0; i<corners.size(); i++)
    {
      Amg::Vector3D p = trf*corners[i];
      double r = sqrt(p.x()*p.x()+p.y()*p.y());

      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);
    }

  rMin = std::min(rMin,vMin);
  rMax = std::max(rMax,vMax);

}

const Trk::LayerMaterialProperties* InDet::XMLReaderSvc::getHomogeneousMaterial(std::string material_type)
{

  InDet::MaterialTmp* materialTmp = getMaterialTemplate(material_type);
  double material_thickness = materialTmp->thickness;

  double X0 = materialTmp->X0;
  double L0 = materialTmp->L0;
  double A = materialTmp->A;
  double Z = materialTmp->Z;
  double rho = materialTmp->density;

  ATH_MSG_DEBUG("iFatras::ITkMaterialBuilder material '" << materialTmp->name << 
	       "': X0 = " << X0 << " L0 = " << L0 <<   " A = " << A <<   " Z = " << Z <<   " rho = " << rho );

  // --------------- material estimation ----------------------------------------------------------------
  Trk::MaterialProperties customMaterial(material_thickness, X0, L0, A, Z, rho);
  Trk::LayerMaterialProperties* material = new Trk::HomogeneousLayerMaterial(customMaterial, 1.0);         
  // --------------- material estimation ----------------------------------------------------------------
  return material;     

}

