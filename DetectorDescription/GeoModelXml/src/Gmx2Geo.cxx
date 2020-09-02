/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/Gmx2Geo.h"

#ifndef STANDALONE_GMX
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#else
#include <iostream>
#endif

#include <string>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "GeoModelKernel/GeoPhysVol.h"

#include "ExpressionEvaluator/Evaluator.h"

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/ElementProcessor.h"
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GmxInterface.h"
#include "GeoModelXml/createdomdocument.h"

using namespace std;
using namespace xercesc;

Gmx2Geo::Gmx2Geo(const string xmlFile, GeoPhysVol *addHere, GmxInterface &gmxInterface, unsigned int flags) {
//
//    Create the xml tree (DOMDocument)
//

// Logging: ref https://wiki.bnl.gov/dayabay/index.php?title=Logging
// Turn on logging in job-options with: MessageSvc.setDebug += {"GeoModelXml"}
#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc", "GeoModelXml");
    MsgStream log(&(*msgh), "GeoModelXml");
#endif

    DOMLSParser *parser = 0;
    DOMDocument *doc = createDOMDocument(xmlFile, parser, flags);
    if (!doc) {// Parsed badly
        XMLPlatformUtils::Terminate();
#ifndef STANDALONE_GMX
        log << MSG::FATAL << "Error in xml file " << xmlFile << ". Exiting athena." << endmsg;
#else
	std::cout<<"Error in xml file " << xmlFile << ". Exiting athena." <<std::endl;
#endif
        exit(0);
    }
//
//    Set up the CLHEP evaluator and the xml-tag processors, and store the GmxInterface:
//
    GmxUtil gmxUtil(gmxInterface); 
//
//    Process the xml tree, creating all the GeoModel items and adding to the GeoModel tree.
//
    const DOMElement *root = doc->getDocumentElement();
//
//    info message: name of detector
//
    if (flags & 0x1) {
#ifndef STANDALONE_GMX
        log << MSG::INFO << "Set up detector geometry from db-string \nwhich adds sub-detector ";
#else
	std::cout<<"Set up detector geometry from db-string \nwhich adds sub-detector ";
#endif
    }
    else {
#ifndef STANDALONE_GMX
        log << MSG::INFO << "Set up detector geometry from file " << xmlFile << " which adds sub-detector ";
#else
	std::cout<<"Set up detector geometry from db-string \nwhich adds sub-detector ";
#endif
    }
    const DOMElement *element = dynamic_cast<const DOMElement*>(root);
    const XMLCh *attribute = element->getAttribute(Translate("name"));
#ifndef STANDALONE_GMX
    log << Translate(attribute) << endmsg;
#else
    std::cout<<Translate(attribute) << std::endl;
#endif
//
//    Add all constant definitions to the evaluator, so they are ready if needed.
//
    doDefines(doc, gmxUtil.eval);
//
//    Set up all sensitive detector types (readout geometry)
//
    doReadoutGeometry(doc, gmxUtil);
//
//    Add all index names
//
    doPositionIndex(doc, gmxUtil);
//
//   The xml file ends with an addbranch element. We start there, process it, 
//   and it fills in the list of things to be added to the GeoModel graph. 
//   There is one and only one addbranch element according to the DTD.
//
    xercesc::DOMNodeList *addbranchs = doc->getElementsByTagName(Translate("addbranch"));
    const DOMElement *addbranch = dynamic_cast<const DOMElement *> (addbranchs->item(0)); 
    GeoNodeList toAdd;
    gmxUtil.processorRegistry.find("addbranch")->process(addbranch, gmxUtil, toAdd);
    for (GeoNodeList::iterator node = toAdd.begin(); node != toAdd.end(); ++node) {
        addHere->add(*node);
    }

    XMLPlatformUtils::Terminate();
}

int Gmx2Geo::doDefines(DOMDocument *doc, Evaluator &eval) {
//
//    Add all defines to the CLHEP::Evaluator
//
//
//    It is better to store the evaluated number, not the expression 
//      1: it avoids the parenthesis problem sum = a + b; 2 * sum != 2(a + b)
//      2: it finds undefined constants sooner (helps debugging)
//      3: anyway, everything is a constant so we might as well
//    ...This means we need everything already defined before it is introduced
//    ...Which means we must do matrices and vectors (which are only numbers) before vars.
//    in case the user uses a vector-element when defining a var element
//

const DOMElement *element;
//
//-------------------------------------------------------------------------------------------
//
// Turn var printout on and off with 
    
#ifndef STANDALONE_GMX
    ServiceHandle<IMessageSvc> msgh("MessageSvc","GmxUtil");
    MsgStream log(&(*msgh), "GmxUtil");
    log << MSG::DEBUG << "\n\nGmx2Geo GmxUtil matrix, vector and var values:\n";
    log << MSG::DEBUG <<     "==============================================\n\n";
#else
    std::cout << "\n\nGmx2Geo GmxUtil matrix, vector and var values:\n";
    std::cout <<     "==============================================\n\n";
#endif

    DOMNodeList *defines = doc->getElementsByTagName(Translate("defines"));
    int nDefines = defines->getLength();
    for (int i = 0; i < nDefines; ++i) {
        element = dynamic_cast<DOMElement *>(defines->item(i));
        DOMNodeList *vars;
        DOMElement *var;
        int nVars;
//
//   Vectors: a list of variables. Names subscripted by _k; values white-space separated numbers.
//

#ifndef STANDALONE_GMX
        log << "\n\n    Vectors\n    =======\n\n";
#else
	std::cout<<"\n\n    Vectors\n    =======\n\n";
#endif

        vars = element->getElementsByTagName(Translate("vector"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = Translate(var->getAttribute(Translate("name")));
            char *val = Translate(var->getAttribute(Translate("value")));
            string fullname(name);
            fullname += '_';
            istringstream list(val);
            double dble;
            int k = 0;
            do  {
                list >> dble;
#ifndef STANDALONE_GMX
                log << fullname + to_string(k) << " = " << dble << endl; 
#else
		std::cout<<fullname + to_string(k) << " = " << dble << std::endl;
#endif
                eval.setVariable((fullname + to_string(k++)).c_str(), dble);
            } while(list.good());
            XMLString::release(&name);
            XMLString::release(&val);
        }
//
//   Matrices: a matrix of variables. Names subscripted by _j_k; values: white-space separated numbers.
//
#ifndef STANDALONE_GMX
        log << "\n\n    Matrices\n    ========\n\n";
#else
	std::cout<<"\n\n    Matrices\n    ========\n\n";
#endif

        vars = element->getElementsByTagName(Translate("matrix"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = Translate(var->getAttribute(Translate("name")));
            char *val = Translate(var->getAttribute(Translate("value")));
            int coldim;
            istringstream(Translate(var->getAttribute(Translate("coldim")))) >> coldim;
            string fullname(name);
            fullname += '_';
            istringstream list(val);
            double dble;
            int k = 0;
            do  {
                for (int col = 0; col < coldim; ++col) {
                        list >> dble;
#ifndef STANDALONE_GMX
                        log << fullname + to_string(k) + '_' + to_string(col) << " = " << dble << endl; 
#else
			std::cout<<fullname + to_string(k) + '_' + to_string(col) << " = " << dble << std::endl; 
#endif
                        eval.setVariable((fullname + to_string(k) + '_' + to_string(col)).c_str(), dble);
                        if (!list.good()) break;
                }
                k++;
            } while(list.good());
            XMLString::release(&name);
            XMLString::release(&val);
        }
//
//    Vars: single variable
//
#ifndef STANDALONE_GMX
        log << "\n\n    Single variables\n    ================\n\n";
#else
	std::cout<<"\n\n    Single variables\n    ================\n\n";
#endif

        vars = element->getElementsByTagName(Translate("var"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = Translate(var->getAttribute(Translate("name")));
            char *val = Translate(var->getAttribute(Translate("value")));
            double evaluated = eval.evaluate(val);
            if (eval.status() != Evaluator::OK) {
#ifndef STANDALONE_GMX
                log << MSG::FATAL << "GeoModelXml Error processing CLHEP Evaluator expression. Error name " <<
                eval.error_name() << endl << "Message: ";
                eval.print_error();
                log << val << endl;
                log << string(eval.error_position(), '-') << '^' << '\n';
                log << "Exiting program." << endmsg;
#else
		std::cout<< "GeoModelXml Error processing CLHEP Evaluator expression. Error name " <<
                eval.error_name() << std::endl << "Message: ";
                eval.print_error();
		std::cout << val << std::endl;
                std::cout << string(eval.error_position(), '-') << '^' << '\n';
                std::cout << "Exiting program." << std::endl;
#endif
                exit(999); // Should do better...
            }
            eval.setVariable(name, evaluated);
#ifndef STANDALONE_GMX
            log << name << "\t\t" << val << " = " << setprecision(10) << evaluated << endl;
#else
	    std::cout<<"\t\t" << val << " = " << setprecision(10) << evaluated << std::endl;
#endif
            XMLString::release(&name);
            XMLString::release(&val);
        }
    }
#ifndef STANDALONE_GMX
    log << endmsg;
#else
    std::cout<<std::endl;
#endif
    return 1;
}

int Gmx2Geo::doPositionIndex(xercesc::DOMDocument *doc, GmxUtil &gmxUtil) {
    DOMNodeList *posIndex = doc->getElementsByTagName(Translate("positionindex"));
    int nPosIndex = posIndex->getLength(); // Probably limited to 0 or 1 by DTD
    for (int i = 0; i < nPosIndex; ++i) {
        const DOMElement *element = dynamic_cast<DOMElement *>(posIndex->item(i));
        DOMNodeList *addindexs = element->getElementsByTagName(Translate("addindex"));
        int nIndexs = addindexs->getLength();
        for (int j = 0; j < nIndexs; ++j) {
            DOMElement *addindex = dynamic_cast<DOMElement*>(addindexs->item(j));
            string name = string(Translate(addindex->getAttribute(Translate("name"))));
            gmxUtil.positionIndex.addIndex(name);
        }
    }
    return 1;
}

int Gmx2Geo::doReadoutGeometry(xercesc::DOMDocument *doc, GmxUtil &gmxUtil) {
//
//    We keep three parameter lists (each is a map of (parameter name, parameter value)), one at each
//    level (readoutgeometry, sensorclass, sensortype). Currently just created new, copying previous one into
//    it. In this way the sensortype parameter list has all parameters in it. Not efficient, but the list is always short?
//
char *name2release;
string tagName;
//
//    Loop over all readoutgeometry elements
//
    DOMNodeList *rgs = doc->getElementsByTagName(Translate("readoutgeometry"));
    int nRG = rgs->getLength(); 
    for (int i = 0; i < nRG; ++i) {
        map<string, string> rgParams; // New empty list
        const DOMElement *rg = dynamic_cast<DOMElement *>(rgs->item(i));
//
//    Loop over readoutgeometry children
//
        for (DOMNode *rgChild = rg->getFirstChild(); rgChild != 0; rgChild = rgChild->getNextSibling()) {
            if (rgChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
            name2release = Translate(rgChild->getNodeName());
            tagName = name2release;
            XMLString::release(&name2release);
            if (tagName == "param") {
                addParam(rgChild, rgParams);
            }
            else if (tagName == "sensorclass") {
                map<string, string> scParams(rgParams); // Initialised with all previous params
                const DOMElement *sensorClass = dynamic_cast<DOMElement *>(rgChild);
                name2release = Translate(sensorClass->getAttribute(Translate("name")));
                string clas(name2release); // class is reserved word
                XMLString::release(&name2release);
//
//    Loop over sensorclass children
//
                for (DOMNode *scChild = sensorClass->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
                    if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
                    name2release = Translate(scChild->getNodeName());
                    tagName = name2release;
                    XMLString::release(&name2release);
                    if (tagName == "param") {
                        addParam(scChild, scParams);
                    }
                    else if (tagName == "sensortype") {
                        map<string, string> stParams(scParams); // Initialised with all previous params
                        const DOMElement *sensorType = dynamic_cast<DOMElement *>(scChild);
                        name2release = Translate(sensorType->getAttribute(Translate("name")));
                        string name(name2release);
                        XMLString::release(&name2release);
//
//    Loop over sensortype parameters
//
                        for (DOMNode *stChild = scChild->getFirstChild(); stChild != 0; stChild = stChild->getNextSibling()) {
                            if (stChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
                            addParam(stChild, stParams);
                        }
//
//    Call the user's call back routine to add this sensor type with its specific parameters
//
                        gmxUtil.gmxInterface()->addSensorType(clas, name, stParams);
                    }
                }
            }
        }
    }
    return 1;
}

void Gmx2Geo::addParam(DOMNode *node, map<string, string> &params) {
    const DOMElement *param = dynamic_cast<DOMElement *>(node);
    char *name2release = Translate(param->getAttribute(Translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
    name2release = Translate(param->getAttribute(Translate("value")));
    string value(name2release);
    XMLString::release(&name2release);
    params[name] = value;
}
