/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/XMLHandler.h"
#include "AGDD2Geo/XMLHandlerStore.h"
#include "AGDD2Geo/IAGDD2GeoSvc.h"
#include "AGDD2Geo/XercesParser.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include <vector>

DOMNode* XMLHandler::currentElement=0;
bool XMLHandler::printFlag=false;

XMLHandler::XMLHandler(std::string n):name(n) 
{
//    std::cout<< " creating new handler "<<n<<std::endl;
	stopLoop=false;
	RegisterToStore();
}

MsgStream XMLHandler::msgLog()
{
	ISvcLocator* svcLocator=Gaudi::svcLocator();
   	StatusCode sc;
   	const IAGDD2GeoSvc* geoSvc=0;
   	sc = svcLocator->service ("AGDD2GeoSvc",geoSvc);
	if (sc.isFailure()) {}
	MsgStream log= geoSvc->GetMsgStream();
	return log;
}

void XMLHandler::RegisterToStore() 
{
	XMLHandlerStore::GetHandlerStore()->RegisterHandler(this);
}

void XMLHandler::StopLoop(bool v)
{
	stopLoop=v;
}
bool XMLHandler::IsLoopToBeStopped()
{
	return stopLoop;
}
bool XMLHandler::isAttribute(const std::string name) const
{
	bool res;
	std::string temp=getAttribute(name,res);
	return res;
}
std::string XMLHandler::getAttribute(const std::string name, bool& isPresent) const
{
	std::string retValue="";
	isPresent=false;
	if (currentElement->hasAttributes()) {
		DOMNamedNodeMap *pAttributes = currentElement->getAttributes();
		DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->getNamedItem(XMLString::transcode(name.c_str()));
		if (pAttributeNode) {

  		  char* val=XMLString::transcode(pAttributeNode->getValue());
		  if (val) {
			
			isPresent=true;
			retValue=val;
			XMLString::release(&val);
			return retValue;
		  }
		}
		else return retValue;
			
	}
	return retValue;
}
std::string XMLHandler::getAttributeAsString(const std::string name, bool& isPresent) const
{
	std::string temp=getAttribute(name,isPresent);
	return temp;
}
double XMLHandler::getAttributeAsDouble(const std::string name, bool& isPresent) const
{
	double res=0.;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		res=XercesParser::Evaluator().Eval(temp.c_str());
	}
	return res;
}
int XMLHandler::getAttributeAsInt(const std::string name, bool& isPresent) const
{
	int res=0;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		res=XercesParser::Evaluator().Eval(temp.c_str());
	}
	return res;
}
std::vector<double> XMLHandler::getAttributeAsVector(const std::string name, bool& isPresent) const
{
	std::vector<double> vect;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		std::vector<std::string> v=XercesParser::Evaluator().tokenize(";",temp);
		for (unsigned int i=0;i<v.size();i++)
		{
			vect.push_back(XercesParser::Evaluator().Eval(v[i].c_str()));
		}
	}
	return vect;
}

std::string XMLHandler::getAttributeAsString(const std::string name, const std::string def) const
{
	bool isPresent;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) return temp;
	else return def;
}
double XMLHandler::getAttributeAsDouble(const std::string name, const double def) const
{
	bool isPresent;
	double res=0.;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		res=XercesParser::Evaluator().Eval(temp.c_str());
		return res;
	}
	return def;
}
int XMLHandler::getAttributeAsInt(const std::string name, const int def) const
{
	bool isPresent;
	int res=0;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		res=XercesParser::Evaluator().Eval(temp.c_str());
		return res;
	}
	return def;
}
std::vector<double> XMLHandler::getAttributeAsVector(const std::string name, const std::vector<double> def) const
{
	bool isPresent;
	std::vector<double> vect;
	std::string temp=getAttribute(name,isPresent);
	if (isPresent) 
	{
		std::vector<std::string> v=XercesParser::Evaluator().tokenize(";",temp);
		for (unsigned int i=0;i<v.size();i++)
		{
			vect.push_back(XercesParser::Evaluator().Eval(v[i].c_str()));
		}
		return vect;
	}
	return def;
}
