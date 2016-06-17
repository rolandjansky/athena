/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/Gmx2Geo.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "GeoModelKernel/GeoPhysVol.h"

#include "CLHEP/Evaluator/Evaluator.h"

#include "GeoModelXml/translate.h"
#include "GeoModelXml/GeoNodeList.h"
#include "GeoModelXml/ElementProcessor.h"
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GmxInterface.h"

using namespace std;
using namespace xercesc;

DOMDocument *createDOMDocument(const string filename, DOMLSParser *parser, unsigned int flags = 0);

Gmx2Geo::Gmx2Geo(const string xmlFile, GeoPhysVol *addHere, GmxInterface &gmxInterface, unsigned int flags) {
//
//    Create the xml tree (DOMDocument)
//
    DOMLSParser *parser = 0;
    DOMDocument *doc = createDOMDocument(xmlFile, parser, flags);
    if (!doc) {// Parsed badly
        XMLPlatformUtils::Terminate();
        cerr << "\n\nError in xml file " << xmlFile << ". Exiting athena.\n\n";
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
        cout << "\n\nGmx2Geo: Set up detector geometry from db-string which adds sub-detector ";
    }
    else {
        cout << "\n\nGmx2Geo: Set up detector geometry from file " << xmlFile << " which adds sub-detector ";
    }
    const DOMElement *element = dynamic_cast<const DOMElement*>(root);
    const XMLCh *attribute = element->getAttribute(translate("name"));
    cout << translate(attribute) << "\n\n";
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
    xercesc::DOMNodeList *addbranchs = doc->getElementsByTagName(translate("addbranch"));
    const DOMElement *addbranch = dynamic_cast<const DOMElement *> (addbranchs->item(0)); 
    GeoNodeList toAdd;
    gmxUtil.processorRegistry.find("addbranch")->process(addbranch, gmxUtil, toAdd);
    for (GeoNodeList::iterator node = toAdd.begin(); node != toAdd.end(); ++node) {
        addHere->add(*node);
    }

    XMLPlatformUtils::Terminate();
}

int Gmx2Geo::doDefines(DOMDocument *doc, HepTool::Evaluator &eval) {
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
    cout << "\n\nDefined constants\n=================\n\n";

    DOMNodeList *defines = doc->getElementsByTagName(translate("defines"));
    int nDefines = defines->getLength();
    for (int i = 0; i < nDefines; ++i) {
        element = dynamic_cast<DOMElement *>(defines->item(i));
        DOMNodeList *vars;
        DOMElement *var;
        int nVars;
//
//   Vectors: a list of variables. Names subscripted by _k; values white-space separated numbers.
//
        cout << "\n\n    Vectors\n    =======\n\n";

        vars = element->getElementsByTagName(translate("vector"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = translate(var->getAttribute(translate("name")));
            char *val = translate(var->getAttribute(translate("value")));
            string fullname(name);
            fullname += '_';
            istringstream list(val);
            double dble;
            int k = 0;
            do  {
                list >> dble;
                cout << fullname + to_string(k) << " = " << dble << endl; 
                eval.setVariable((fullname + to_string(k++)).c_str(), dble);
            } while(list.good());
            XMLString::release(&name);
            XMLString::release(&val);
        }
//
//   Matrices: a matrix of variables. Names subscripted by _j_k; values: white-space separated numbers.
//
        cout << "\n\n    Matrices\n    ========\n\n";

        vars = element->getElementsByTagName(translate("matrix"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = translate(var->getAttribute(translate("name")));
            char *val = translate(var->getAttribute(translate("value")));
            int coldim;
            istringstream(translate(var->getAttribute(translate("coldim")))) >> coldim;
            string fullname(name);
            fullname += '_';
            istringstream list(val);
            double dble;
            int k = 0;
            do  {
                for (int col = 0; col < coldim; ++col) {
                        list >> dble;
                        cout << fullname + to_string(k) + '_' + to_string(col) << " = " << dble << endl; 
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
        cout << "\n\n    Single variables\n    ================\n\n";

        vars = element->getElementsByTagName(translate("var"));
        nVars = vars->getLength();
        for (int j = 0; j < nVars; ++j) {
            var = dynamic_cast<DOMElement*>(vars->item(j));
            char *name = translate(var->getAttribute(translate("name")));
            char *val = translate(var->getAttribute(translate("value")));
            double evaluated = eval.evaluate(val);
            if (eval.status() != HepTool::Evaluator::OK) {
                cerr << "GeoModelXml Error processing CLHEP Evaluator expression. Error name " <<
                eval.error_name() << endl << "Message: ";
                eval.print_error();
                cerr << val << endl;
                cerr << string(eval.error_position(), '-') << '^' << '\n';
                cerr << "Exiting program.\n";
                exit(999); // Should do better...
            }
            eval.setVariable(name, evaluated);
//            eval.setVariable(name, val); // If ever you want to go back to storing expressions, 
                                           // uncomment this line and comment out the above few
            cout << name << "\t\t" << val << " = " << setprecision(10) << evaluated << endl;
            XMLString::release(&name);
            XMLString::release(&val);
        }
    }
    return 1;
}

int Gmx2Geo::doPositionIndex(xercesc_3_1::DOMDocument *doc, GmxUtil &gmxUtil) {
    DOMNodeList *posIndex = doc->getElementsByTagName(translate("positionindex"));
    int nPosIndex = posIndex->getLength(); // Probably limited to 0 or 1 by DTD
    for (int i = 0; i < nPosIndex; ++i) {
        const DOMElement *element = dynamic_cast<DOMElement *>(posIndex->item(i));
        DOMNodeList *addindexs = element->getElementsByTagName(translate("addindex"));
        int nIndexs = addindexs->getLength();
cout << "IndexProcessor: number of addindexs is " << nIndexs << endl;
        for (int j = 0; j < nIndexs; ++j) {
            DOMElement *addindex = dynamic_cast<DOMElement*>(addindexs->item(j));
            string name = string(translate(addindex->getAttribute(translate("name"))));
            gmxUtil.positionIndex.addIndex(name);
        }
    }
    return 1;
}

int Gmx2Geo::doReadoutGeometry(xercesc_3_1::DOMDocument *doc, GmxUtil &gmxUtil) {
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
    DOMNodeList *rgs = doc->getElementsByTagName(translate("readoutgeometry"));
    int nRG = rgs->getLength(); 
    for (int i = 0; i < nRG; ++i) {
        map<string, string> rgParams; // New empty list
        const DOMElement *rg = dynamic_cast<DOMElement *>(rgs->item(i));
//
//    Loop over readoutgeometry children
//
        for (DOMNode *rgChild = rg->getFirstChild(); rgChild != 0; rgChild = rgChild->getNextSibling()) {
            if (rgChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
            name2release = translate(rgChild->getNodeName());
            tagName = name2release;
            XMLString::release(&name2release);
            if (tagName == "param") {
                addParam(rgChild, rgParams);
            }
            else if (tagName == "sensorclass") {
                map<string, string> scParams(rgParams); // Initialised with all previous params
                const DOMElement *sensorClass = dynamic_cast<DOMElement *>(rgChild);
                name2release = translate(sensorClass->getAttribute(translate("name")));
                string clas(name2release); // class is reserved word
                XMLString::release(&name2release);
//
//    Loop over sensorclass children
//
                for (DOMNode *scChild = sensorClass->getFirstChild(); scChild != 0; scChild = scChild->getNextSibling()) {
                    if (scChild->getNodeType() != DOMNode::ELEMENT_NODE) continue; // Skip char data
                    name2release = translate(scChild->getNodeName());
                    tagName = name2release;
                    XMLString::release(&name2release);
                    if (tagName == "param") {
                        addParam(scChild, scParams);
                    }
                    else if (tagName == "sensortype") {
                        map<string, string> stParams(scParams); // Initialised with all previous params
                        const DOMElement *sensorType = dynamic_cast<DOMElement *>(scChild);
                        name2release = translate(sensorType->getAttribute(translate("name")));
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
    char *name2release = translate(param->getAttribute(translate("name")));
    string name(name2release);
    XMLString::release(&name2release);
    name2release = translate(param->getAttribute(translate("value")));
    string value(name2release);
    XMLString::release(&name2release);
    params[name] = value;
}
