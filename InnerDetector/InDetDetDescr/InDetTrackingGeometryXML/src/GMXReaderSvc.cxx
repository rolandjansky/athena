///////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometryXML/GMXReaderSvc.h"

#include "PathResolver/PathResolver.h"
#include "GeoModelXml/createdomdocument.h"
#include "GeoModelXml/translate.h"

#include <boost/filesystem.hpp>
#include "TMath.h"

InDet::GMXReaderSvc::GMXReaderSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name, svc),
  m_dictionaryFileName("MyFolder/IDTest.xml"),
  m_gmxInputName("ITkStrip.gmx"),
  m_createDict(true),
  m_addBCL(false),
  m_useDb(true),
  m_doHGTD(false),
  m_pathToGMXFile("")
{
  declareProperty("dictionaryFileName",    m_dictionaryFileName);
  declareProperty("GMX_InputName",         m_gmxInputName);
  declareProperty("createXMLDictionary",   m_createDict);
  declareProperty("addBCL",                m_addBCL);
  declareProperty("readGMXfromDB",         m_useDb);
  declareProperty("pathToGMXFile",         m_pathToGMXFile);
}

InDet::GMXReaderSvc::~GMXReaderSvc()
{
}

// Query the interface
StatusCode InDet::GMXReaderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_GMXReaderSvc == riid )
    *ppvInterface = (GMXReaderSvc *) this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode InDet::GMXReaderSvc::initialize()
{
  ATH_MSG_INFO("InDet::GMXReaderSvc::initialize()");

  // Dump XML dictionary
  if(m_createDict) {
    writeDictionary(m_dictionaryFileName);    
  }

  if(m_useDb && m_pathToGMXFile==""){
    ATH_MSG_FATAL("readGMXFromDB set without specifying pathToGMXFile! Please check your configuration!");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode InDet::GMXReaderSvc::finalize()
{
  return StatusCode::SUCCESS;
}


void InDet::GMXReaderSvc::writeDictionary(std::string filename)
{
  
  std::ofstream file;
  // look for the directory
  std::size_t found = filename.find("/");
  if (found!=std::string::npos) {
    std::string folder = filename.substr(0, found);
    boost::filesystem::path dir(folder);
    if(!(boost::filesystem::exists(dir))) {
        ATH_MSG_DEBUG("Directory " << folder << " doesn't exist... Creating it!");
        if (boost::filesystem::create_directory(dir))
            ATH_MSG_DEBUG("Directory " << folder << " created!");
    }
  }
  
  openDictFile(file,filename);
  writeGMXDictionary(file);
  closeDictFile(file);
}

void InDet::GMXReaderSvc::openDictFile(std::ofstream& file,std::string filename) const
{
  file.open(filename, ios::app);
 
  ATH_MSG_DEBUG("InDet::GMXReaderSvc::openDictFile ");
  
}

void InDet::GMXReaderSvc::closeDictFile(std::ofstream& file) const
{
  if (!m_doHGTD)
    file << "</IdDictionary>" << std::endl;
  file.close();
}


void InDet::GMXReaderSvc::getInputGMX(std::string& gmxInput) {

  ATH_MSG_DEBUG("InDet::GMXReaderSvc::getInputGMX start!!");

  gmxInput = PathResolver::find_file(m_gmxInputName, "DATAPATH");
  if (gmxInput == "") { // File not found
    std::string errMessage("GMXReaderSvc::getInputGMX : Unable to find file " + m_gmxInputName +
    " with PathResolver; check filename and DATAPATH environment variable");
    throw std::runtime_error(errMessage);
  }
  ATH_MSG_DEBUG("InDet::GMXReaderSvc::getInputGMX finish!!");

}

void InDet::GMXReaderSvc::writeGMXDictionary(std::ofstream& file) {
  
  std::string gmxInput;
  if(m_useDb) gmxInput  =  m_pathToGMXFile;
  else getInputGMX(gmxInput);
  
  ATH_MSG_DEBUG("InDet::GMXReaderSvc::writeGMXDictionary start!!");
  
  DOMLSParser *parser = 0;
  DOMDocument *doc = createDOMDocument(gmxInput, parser, 0);
  if (!doc) {// Parsed badly
    XMLPlatformUtils::Terminate();
    ATH_MSG_ERROR("\n\nError in xml file " << gmxInput << "!!!!");
  }
  
  //const DOMElement *root = doc->getDocumentElement();
  //const DOMElement *element = dynamic_cast<const DOMElement*>(root);
  //const XMLCh *attribute = element->getAttribute(translate("name"));
  //cout << translate(attribute) << "\n\n";
  
  getVariables(doc);
  
  for ( auto& brl: m_tmp_BarrelLayers ) {
    for ( unsigned int row = 0; row< brl->sensor.rows; row++) {
      file << "  <region group=\"sct\" >" << std::endl;
      file << "    <range field=\"part\" value=\"SCT\" />" << std::endl;
      file << "    <range field=\"barrel_endcap\" value=\"barrel\" />" << std::endl;
      file << "    <range field=\"layer\" value=\"" << brl->layer_disc << "\" />" << std::endl;
      file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" <<  brl->phi_modules-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      
      int neta = brl->eta_modules*2.;
      int iend   =  neta/2;
      int istart = -iend;
      file << "    <range field=\"eta_module\" values=\"";
      for(int i=istart;i<=iend;i++) {
	if(i==0 && (neta%2)==0) continue;
	file << i << " ";
      }
      file << "\" />" << std::endl;
      file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
      file << "    <range field=\"row\" value=\"" << row << "\" />" << std::endl;
      file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"" << brl->sensor.strips.at(row)-1 << "\" />" << std::endl;
      file << "  </region>" << std::endl;
      file << std::endl;
    }
  }
  
  if (m_addBCL) {
       file << "  <region group=\"sct\" >" << std::endl;
       file << "    <range field=\"part\" value=\"SCT\" />" << std::endl;
       file << "    <range field=\"barrel_endcap\" value=\"barrel\" />" << std::endl;
       file << "    <range field=\"layer\" value=\"4\" />" << std::endl;
       file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"59\" wraparound=\"TRUE\" />" << std::endl;
       file << "    <range field=\"eta_module\" values=\"-14 -13 13 14\" />" << std::endl;
       file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
       file << "    <range field=\"rows\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
       file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"1279\" />" << std::endl;
       file << "  </region>" << std::endl;
       file << std::endl;
  }
 
  for ( auto& end : m_tmp_EndcapDiscs) {
    for ( unsigned int row = 0; row< end->sensor.rows; row++) {
      file << "  <region group=\"sct\" >" << std::endl;
      file << "    <range field=\"part\" value=\"SCT\" />" << std::endl;
      file << "    <range field=\"barrel_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"disk\" value=\"" << end->layer_disc << "\" />" << std::endl;
      file << "    <range field=\"phi_module\" minvalue=\"0\" maxvalue=\"" <<  end->phi_modules-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"eta_module\" value=\"" << end->eta_modules << "\" />" << std::endl;
      file << "    <range field=\"side\" minvalue=\"0\" maxvalue=\"1\" />" << std::endl;
      file << "    <range field=\"row\" value=\"" << row << "\" />" << std::endl;
      file << "    <range field=\"strip\" minvalue=\"0\" maxvalue=\"" << end->sensor.strips.at(row)-1 << "\" />" << std::endl;
      file << "  </region>" << std::endl;
      file << std::endl;
    }      
  }
  
  ATH_MSG_DEBUG("InDet::GMXReaderSvc::writeGMXDictionary finish!!");

}

void InDet::GMXReaderSvc::getVariables(DOMDocument *doc) {
  
  //cout << "\n\nDefined constants\n=================\n\n";

  DOMNodeList *defines = doc->getElementsByTagName(translate("defines"));
  const XMLSize_t nDefines = defines->getLength();

  vector<double> Barrel_NumberOfEta;
  vector<double> Barrel_NumberOfPhi;
  double Endcap_Wheels = 0.;
  vector<double> Endcap_NumberOfPhi;
  
  
  for (XMLSize_t i = 0; i < nDefines; ++i) {
    DOMElement* element = dynamic_cast<DOMElement *>(defines->item(i));
    DOMNodeList *vars;
    DOMElement *var;
    int nVars;
//
//   Vectors: a list of variables. Names subscripted by _k; values white-space separated numbers.
//
    std::vector<std::string> elements = {"vector", "var"};
    
    for (auto& el: elements) {
      vars = element->getElementsByTagName(transcode(el.c_str()));
      nVars = vars->getLength();
      for (int j = 0; j < nVars; ++j) {
        var = dynamic_cast<DOMElement*>(vars->item(j));
        string name = translate(var->getAttribute(translate("name")));
    
      if (name == "N_StavesInCyl") 
        Barrel_NumberOfPhi = returnVector(translate(var->getAttribute(translate("value"))));
      else if (name == "B_N_SensorsPerFace") 
        Barrel_NumberOfEta = returnVector(translate(var->getAttribute(translate("value"))));
      else if (name == "N_Wheels")
	Endcap_Wheels = returnDouble(translate(var->getAttribute(translate("value"))));
      else if (name == "N_InRing")
	Endcap_NumberOfPhi = returnVector(translate(var->getAttribute(translate("value"))));	
      }
    }
  }
  ATH_MSG_DEBUG( "Size number of Phi = " << Barrel_NumberOfPhi.size() );
  for (unsigned int size = 0; size < Barrel_NumberOfPhi.size(); size++) 
    ATH_MSG_DEBUG( "element " << size << " = " << Barrel_NumberOfPhi.at(size) );
 
  ATH_MSG_DEBUG( "Size number of Eta = " << Barrel_NumberOfEta.size() );
  for (unsigned int size = 0; size < Barrel_NumberOfEta.size(); size++) 
    ATH_MSG_DEBUG( "element " << size << " = " << Barrel_NumberOfEta.at(size) );
 
  ATH_MSG_DEBUG( "Number of Wheels = " << Endcap_Wheels );
  
  ATH_MSG_DEBUG( "Size number of Phi = " << Endcap_NumberOfPhi.size() );
  for (unsigned int size = 0; size < Endcap_NumberOfPhi.size(); size++) 
    ATH_MSG_DEBUG( "element " << size << " = " << Endcap_NumberOfPhi.at(size) );
  
  
  CreateBarrelLayers(Barrel_NumberOfEta, Barrel_NumberOfPhi);
  CreateEndcapDiscs(Endcap_Wheels, Endcap_NumberOfPhi);

  // Find sensitive names first
  DOMNodeList *log = doc->getElementsByTagName(translate("logvol"));
  const XMLSize_t nLog = log->getLength();
  
  std::vector<SensorClass*> sensors;
 
  std::vector<string> sensorTypes;
  std::vector<string> sensorNames;

  
  for (XMLSize_t i = 0; i < nLog; ++i) {
    DOMElement * var = dynamic_cast<DOMElement *>(log->item(i));
    string type = translate(var->getAttribute(translate("sensitive")));
    if (type == "") continue;
    sensorTypes.push_back(type);
    string name = translate(var->getAttribute(translate("name")));
    sensorNames.push_back(name);
    
    int eta = -1;
    
    for (DOMNode *elChild = var->getFirstChild(); elChild != 0; elChild = elChild->getNextSibling()) {
      if (elChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
      string nodeName = translate(elChild->getNodeName());
      if (nodeName == "index") {
	const DOMElement *elElement = dynamic_cast<DOMElement *>(elChild);
	if ("eta_module" == std::string(translate(elElement->getAttribute(translate("ref"))))) {
	  eta = (int)returnDouble(translate(elElement->getAttribute(translate("value"))));
	  }
	}
    }    
    sensors.push_back(new SensorClass( name, type, (eta<0), eta));
  }
   
  ATH_MSG_DEBUG( "Size sensorTypes = " << sensorTypes.size() );
  for (unsigned int size = 0; size < sensorTypes.size(); size++) 
    ATH_MSG_DEBUG( "element " << size << " = " << sensorTypes.at(size) );
  
  if(msgLvl(MSG::DEBUG))
    for ( auto ss: sensors) ss->Print();
  
  DOMNodeList *rgs = doc->getElementsByTagName(translate("readoutgeometry"));
  const XMLSize_t nRgs = rgs->getLength();
  std::vector<std::vector<double>> sensorStrip;
  std::vector<std::vector<double>> sensorRows;
  
  // enter in the readoutgeometry
  for (XMLSize_t i = 0; i < nRgs; ++i) {
    DOMElement* rg = dynamic_cast<DOMElement *>(rgs->item(i));
    // loop on the children and look for the sensor classes
    for (DOMNode *rgChild = rg->getFirstChild(); rgChild != 0; rgChild = rgChild->getNextSibling()) {
      if (rgChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
      string nodeName = translate(rgChild->getNodeName());
      if (nodeName == "sensorclass") {
	std::vector<double> strip;
	std::vector<double> rows;
	// when the sensorclass is found you loop on the children to get the sensortype 
	// you have already the existing sensor type available
	const DOMElement *sensorClass = dynamic_cast<DOMElement *>(rgChild);
	for (DOMNode *scChild = sensorClass->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
	  if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	  string tagName = translate(scChild->getNodeName());
	  if (tagName == "sensortype") {
	    std::vector<double> sensorS;
	    std::vector<double> sensorR;
	    // when you find the sensortype
	    // get the number of rows and strips
	    const DOMElement *sensorType = dynamic_cast<DOMElement *>(scChild);
	    string type = translate(sensorType->getAttribute(translate("name")));
	    // you are interested only in the sensortype already defined in the sensorTypes
	    //if(std::find(sensorTypes.begin(), sensorTypes.end(), type) != sensorTypes.end()) {
            int pos = -1;
	    for (unsigned int ss = 0; ss<sensors.size(); ss++) {
	      if (sensors.at(ss)->type == type) {
		pos = ss;
		break;
	      }	      
	    }
	    if (pos>=0) {
	      ATH_MSG_DEBUG( "sensortype = " << type );
	      for (DOMNode *stChild = scChild->getFirstChild(); stChild != 0; stChild = stChild->getNextSibling()) {
		if (stChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
		const DOMElement *param = dynamic_cast<DOMElement *>(stChild);
		string name = translate(param->getAttribute(translate("name")));
		if (name == "nRows") {
		  std::vector<double> taken = returnVector(translate(param->getAttribute(translate("value"))));
		  sensorR.insert(sensorR.end(), taken.begin(), taken.end());
		  double taken_double = returnDouble(translate(param->getAttribute(translate("value"))));
		  sensors.at(pos)->rows = taken_double;
		} else if (name == "nStrips") {
		  std::vector<double> taken = returnVector(translate(param->getAttribute(translate("value"))));
		  sensorS.insert(sensorS.end(), taken.begin(), taken.end());
		  sensors.at(pos)->strips = taken;
		}
	      }
	    }
	    if (sensorR.size()<sensorS.size() and sensorR.size()>0) sensorR.resize(sensorS.size(),sensorR.at(sensorR.size()-1));	    
	    else if (sensorR.size()>sensorS.size() and sensorS.size()>0) sensorS.resize(sensorR.size(),sensorS.at(sensorS.size()-1));
	    
	    if (sensorS.size()==0) sensorS.resize(sensorR.size(),0);
	    
	    strip.insert(strip.end(), sensorS.begin(), sensorS.end());
	    rows.insert(rows.end(), sensorR.begin(), sensorR.end());

	  }
	}
	// Check on the dimensions
	//if the number of strips is not found in the sensortype
	// you can use what is specified in the mother --> the sensorclass
	//if (std::find(strip.begin(), strip.end(), 0.) != strip.end()) {
	for (auto& ss: sensors) {
	  if (ss->strips.size()>0) continue;
	  for (DOMNode *scChild = sensorClass->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
	    if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	    string tagName = translate(scChild->getNodeName());
	    const DOMElement *param = dynamic_cast<DOMElement *>(scChild);
	    if (tagName == "param") {
	      string name = translate(param->getAttribute(translate("name")));
	      if (name == "nStrips") {
		std::vector<double> taken = returnVector(translate(param->getAttribute(translate("value"))));
		if (taken.size()>0) std::replace(strip.begin(), strip.end(), 0., taken.at(0));
		ss->strips = taken;
	      }
	    }
	  }
	  
	  if (rows.size()<strip.size() and rows.size()>0) rows.resize(strip.size(),rows.at(rows.size()-1));	    
	  else if (rows.size()>strip.size() and strip.size()>0) strip.resize(rows.size(),strip.at(strip.size()-1));
	  
	  if (ss->strips.size()<ss->rows) ss->strips.resize(ss->rows,ss->strips.at(ss->strips.size()-1));
	  	  
	}
	
	sensorStrip.push_back(strip);
	sensorRows.push_back(rows);
	
	ATH_MSG_DEBUG( "(0) Size Strip = " << sensorStrip.size() );
	for (unsigned int x = 0; x < sensorStrip.size(); x++) 
	  for (unsigned int y = 0; y < sensorStrip.at(x).size(); y++) 
	    ATH_MSG_DEBUG( "(0) element (x,y) = (" << x << "," << y << ") " << sensorStrip.at(x).at(y) );
	  
	ATH_MSG_DEBUG( "(0) Size Row = " << sensorRows.size() );
	for (unsigned int x = 0; x < sensorRows.size(); x++) 
	  for (unsigned int y = 0; y < sensorRows.at(x).size(); y++) 
	   ATH_MSG_DEBUG( "(0) element (x,y) = (" << x << "," << y << ") " << sensorRows.at(x).at(y) );
       
      }	
    }
  }
  
  ATH_MSG_DEBUG( "Size Strip = " << sensorStrip.size() );
  for (unsigned int x = 0; x < sensorStrip.size(); x++) 
    for (unsigned int y = 0; y < sensorStrip.at(x).size(); y++) 
      ATH_MSG_DEBUG( "element (x,y) = (" << x << "," << y << ") " << sensorStrip.at(x).at(y) );
    
  ATH_MSG_DEBUG( "Size Row = " << sensorRows.size() );
  for (unsigned int x = 0; x < sensorRows.size(); x++) 
    for (unsigned int y = 0; y < sensorRows.at(x).size(); y++) 
      ATH_MSG_DEBUG( "element (x,y) = (" << x << "," << y << ") " << sensorRows.at(x).at(y) );

  if(msgLvl(MSG::DEBUG))    
    for ( auto& ss: sensors) ss->Print();
  
  std::map < std::string , std::vector<int> > barrel_indices;
        
  for (auto& ss: sensors) {     
    
    if (not ss->isBrl) continue;    
    
    std::string sensor = ss->name;
    std::vector<int> indices;
    
    DOMNodeList *assembly = doc->getElementsByTagName(translate("assembly"));
    const XMLSize_t nAssembly = assembly->getLength();
    
    std::string newName = "";
    bool found = false;
    bool assfound = false;
  
    for (XMLSize_t i = 0; i < nAssembly; ++i) {
      DOMElement* element = dynamic_cast<DOMElement *>(assembly->item(i));
      DOMNodeList *vars;
      DOMElement *var;
                  
      if (!found) {
	vars = element->getElementsByTagName(transcode("logvolref"));
	for (XMLSize_t j = 0; j < vars->getLength(); ++j) {
	  var = dynamic_cast<DOMElement*>(vars->item(j));
	  std::string name = translate(var->getAttribute(translate("ref")));
          ATH_MSG_DEBUG( "I have name -->  " << name << " with sensor " << sensor );
	  if (name == sensor) {
	    ATH_MSG_DEBUG( "I have found an association with sensor --> " << name << " -- " << sensor );
	    newName = translate(element->getAttribute(translate("name")));
	    found = true;
	    // restart the external loop
	    i=0;
	    break;
	  }
	}
      }

      
      assfound = false;
      
      ATH_MSG_DEBUG( " -------  Starting loop with Newname --> " << newName << "  and assfound = " << assfound );
      
      
      // loop on the children and look for the multicopy
      for (DOMNode *elChild = element->getFirstChild(); elChild != 0; elChild = elChild->getNextSibling()) {
	if (elChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	string nodeName = translate(elChild->getNodeName());
	ATH_MSG_DEBUG( "Element with name = " <<translate(element->getAttribute(translate("name"))) );
	if (nodeName == "transform") {
          ATH_MSG_DEBUG( "I have found a transform" );
	  const DOMElement *assemblyref = dynamic_cast<DOMElement *>(elChild);
	  for (DOMNode *asChild = assemblyref->getFirstChild(); asChild != 0; asChild = asChild->getNextSibling()) {
	    if (asChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	    string assName = translate(asChild->getNodeName());
	    if (assName == "assemblyref") {
	      const DOMElement *assElement = dynamic_cast<DOMElement *>(asChild);
	      ATH_MSG_DEBUG( "I have found an assemblyref" );
	      if (newName == translate(assElement->getAttribute(translate("ref")))) {
		ATH_MSG_DEBUG( "Found ref with " << newName );
		newName = translate(element->getAttribute(translate("name")));
		ATH_MSG_DEBUG( "Newname is " << newName );		
		i=0;
		assfound=true;
	      }
	    }
	    if (assfound) asChild = asChild->getNextSibling();
	  }
	} else if (nodeName == "multicopy") {
	  ATH_MSG_DEBUG( "I have found a multicopy" );
	  const DOMElement *transform = dynamic_cast<DOMElement *>(elChild);
	  for (DOMNode *scChild = transform->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
	    if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	    string tagName = translate(scChild->getNodeName());
	    if (tagName == "transform") {
              ATH_MSG_DEBUG( "I have found a transform" );
	      const DOMElement *assemblyref = dynamic_cast<DOMElement *>(scChild);
	      for (DOMNode *asChild = assemblyref->getFirstChild(); asChild != 0; asChild = asChild->getNextSibling()) {
		if (asChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
		string assName = translate(asChild->getNodeName());
		if (assName == "assemblyref") {
		  const DOMElement *assElement = dynamic_cast<DOMElement *>(asChild);
		  ATH_MSG_DEBUG( "I have found an assemblyref" );
		  if (newName == translate(assElement->getAttribute(translate("ref")))) {
		    ATH_MSG_DEBUG( "Found ref with " << newName );
		    newName = translate(element->getAttribute(translate("name")));
		    ATH_MSG_DEBUG( "Newname is " << newName );
		    i=0;
		    assfound=true;		    
		  }
		}
		if (assfound) asChild = asChild->getNextSibling();
	      }
	    }
	    if (assfound) scChild = scChild->getNextSibling();
	  }
	}
	if (assfound) elChild = elChild->getNextSibling();
      }  
      ATH_MSG_DEBUG( " -------  Exiting loop with Newname --> " << newName << "  and assfound = " << assfound );		
      
    }
    
    ATH_MSG_DEBUG( " -------  Checking last part --> " << newName << "  and assfound = " << assfound );
    
    if (!assfound) {
      DOMNodeList *logvol = doc->getElementsByTagName(translate("logvol"));
      // loop on the children and look for the multicopy   
      for (XMLSize_t j = 0; j < logvol->getLength(); ++j) {
	DOMElement * logel = dynamic_cast<DOMElement *>(logvol->item(j));
	double index = -1.;
	for (DOMNode *elChild = logel->getFirstChild(); elChild != 0; elChild = elChild->getNextSibling()) {
	  if (elChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	  string nodeName = translate(elChild->getNodeName());
	  if (index<0. and nodeName == "index") {
	    const DOMElement *elElement = dynamic_cast<DOMElement *>(elChild);
	    if ("layer_wheel" == std::string(translate(elElement->getAttribute(translate("ref"))))) {
	      index = returnDouble(translate(elElement->getAttribute(translate("value"))));
	      elChild = logel->getFirstChild();
	    }
	  } else if (nodeName == "multicopy") {
	    ATH_MSG_DEBUG( "I have found a multicopy" );
	    const DOMElement *transform = dynamic_cast<DOMElement *>(elChild);
	    for (DOMNode *scChild = transform->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
	      if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
	      string tagName = translate(scChild->getNodeName());
	      if (tagName == "transform") {
		ATH_MSG_DEBUG( "I have found a transform" );
		const DOMElement *assemblyref = dynamic_cast<DOMElement *>(scChild);
		for (DOMNode *asChild = assemblyref->getFirstChild(); asChild != 0; asChild = asChild->getNextSibling()) {
		  if (asChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
		  string assName = translate(asChild->getNodeName());
		  if (assName == "assemblyref") {
		    const DOMElement *assElement = dynamic_cast<DOMElement *>(asChild);
		    ATH_MSG_DEBUG( "I have found an assemblyref" );
		    if (newName == translate(assElement->getAttribute(translate("ref")))) {
		      ATH_MSG_DEBUG( "Printing association between sensor = " << sensor << " and index = " << index );
		      indices.push_back((int)index);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    ATH_MSG_DEBUG( "Sensor --> " << sensor << " Done!!" );
    ATH_MSG_DEBUG( "found indices = ");
    for (auto& in: indices) ATH_MSG_DEBUG( in << "  ");
    barrel_indices.insert( std::pair<std::string, std::vector<int> > (sensor, indices));
  }  
  
  LinkSensors(sensors, barrel_indices);  
  
}

std::vector<double> InDet::GMXReaderSvc::returnVector(char* val) {
  std::vector<double> v;
  istringstream list(val);
  double d;
  while (list >> d) {
    v.push_back(d);
  }
  return v;
}


double InDet::GMXReaderSvc::returnDouble(char* val) {
  istringstream list(val);
  double d;
  list >> d;
  return d;
}

void InDet::GMXReaderSvc::CreateBarrelLayers (std::vector<double> Barrel_NumberOfEta, std::vector<double> Barrel_NumberOfPhi){  
  // Check dimensions first
  // if the dimension of Barrel_NumberOfEta < Barrel_NumberOfPhi
  // then increase the size of Barrel_NumberOfEta to the Barrel_NumberOfPhi one
  // filling the remaining elements with the last element.
  
  if (Barrel_NumberOfPhi.size()>0 and Barrel_NumberOfEta.size()>0 and Barrel_NumberOfEta.size()<Barrel_NumberOfPhi.size())
   Barrel_NumberOfEta.resize(Barrel_NumberOfPhi.size(), Barrel_NumberOfEta.at(Barrel_NumberOfEta.size()-1));
    
  // Now build the layer elements  
  for (unsigned int el=0; el < Barrel_NumberOfPhi.size(); el++ ) 
    m_tmp_BarrelLayers.push_back( new LayerDiscClass( true, el, Barrel_NumberOfPhi.at(el), Barrel_NumberOfEta.at(el)));  

  if(msgLvl(MSG::DEBUG))
    for (auto& el: m_tmp_BarrelLayers)
      el->Print();  
}

void InDet::GMXReaderSvc::CreateEndcapDiscs (double Endcap_Wheels, std::vector<double> Endcap_NumberOfPhi) {
  
  for (unsigned int wheel=0; wheel < (unsigned int)Endcap_Wheels; wheel++) {
    for (unsigned int el=0; el < Endcap_NumberOfPhi.size(); el++) {
          m_tmp_EndcapDiscs.push_back( new LayerDiscClass( false, wheel, Endcap_NumberOfPhi.at(el), el));  
    }    
  }  

  if(msgLvl(MSG::DEBUG))
    for (auto& el: m_tmp_EndcapDiscs)
      el->Print();
}

void InDet::GMXReaderSvc::LinkSensors( std::vector<SensorClass*> sensors, std::map < std::string, std::vector <int> > barrel_indices ) {
 
  for ( auto& ss: sensors ) {
    if ( ss->isBrl and (barrel_indices.find(ss->name)!=barrel_indices.end()) ) {
      for ( auto& el: m_tmp_BarrelLayers ) {
	for ( auto& in: barrel_indices.at(ss->name) ) {
	 if ( el->layer_disc == in )
	  el->sensor = *ss;
	}
      }
    } else {
      for ( auto& el: m_tmp_EndcapDiscs ) {
	if ( el->eta_modules == ss->eta_module)
	  el->sensor = *ss;
      }
    }
  }
  if(msgLvl(MSG::DEBUG)){  
    for (auto& el: m_tmp_BarrelLayers) el->Print();  
    for (auto& el: m_tmp_EndcapDiscs)  el->Print();
  }
  
}


