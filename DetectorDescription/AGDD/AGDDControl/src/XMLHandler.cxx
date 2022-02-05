/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/AGDDController.h"

#include <memory>
#include <vector>

using namespace xercesc;


XMLHandler::XMLHandler(const std::string& n, AGDDController& c)
  : m_name(n)
{
//    std::cout<< " creating new handler "<<n<<std::endl;
	m_stopLoop=false;
	RegisterToStore(c);
}

void XMLHandler::RegisterToStore(AGDDController& c)
{
	c.GetHandlerStore().RegisterHandler(this);
}

void XMLHandler::StopLoop(bool v)
{
	m_stopLoop=v;
}
bool XMLHandler::IsLoopToBeStopped() const
{
	return m_stopLoop;
}
bool XMLHandler::isAttribute(const xercesc::DOMNode* t,
                             const std::string& name) const
{
	bool res;
	std::string temp=getAttribute(t, name,res);
	return res;
}
std::string XMLHandler::getAttribute(const xercesc::DOMNode* t,
                                     const std::string& name, bool& isPresent) const
{
	std::string retValue="";
	isPresent=false;
	if (t->hasAttributes()) {
		DOMNamedNodeMap *pAttributes = t->getAttributes();
		auto deleter = [&](XMLCh buf[]) { XMLString::release(&buf); };
		std::unique_ptr<XMLCh[], decltype(deleter)> ptr(XMLString::transcode(name.c_str()), deleter);
		DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->getNamedItem(ptr.get());
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
std::string XMLHandler::getAttributeAsString(AGDDController& /*c*/,
                                             const xercesc::DOMNode* t,
                                             const std::string& name) const
{
	bool isPresent;
	std::string temp=getAttribute(t, name,isPresent);
	if (!isPresent) std::abort();
	return temp;
}
double XMLHandler::getAttributeAsDouble(AGDDController& c,
                                        const xercesc::DOMNode* t,
                                        const std::string& name) const
{
        double res=0.;
        bool isPresent;
        std::string temp=getAttribute(t, name,isPresent);
	if (!isPresent) std::abort();
        res=c.Evaluator().Eval(temp.c_str());
        return res;
}
int XMLHandler::getAttributeAsInt(AGDDController& c,
                                  const xercesc::DOMNode* t,
                                  const std::string& name) const
{
        int res=0;
        bool isPresent;
        std::string temp=getAttribute(t, name,isPresent);
	if (!isPresent) std::abort();
        res=c.Evaluator().Eval(temp.c_str());
        return res;
}
std::vector<double> XMLHandler::getAttributeAsVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name) const
{
	bool isPresent;
        std::vector<double> vect;
        std::string temp=getAttribute(t, name,isPresent);
        if (!isPresent) std::abort();
        std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
        for (unsigned int i=0;i<v.size();i++)
        {
             vect.push_back(c.Evaluator().Eval(v[i].c_str()));
        }
        return vect;
}

std::vector<int> XMLHandler::getAttributeAsIntVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name) const
{
        bool isPresent;
        std::vector<int> vect;
        std::string temp=getAttribute(t, name,isPresent);
        if (!isPresent) std::abort();
        std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
        for (unsigned int i=0;i<v.size();i++)
        {
             vect.push_back(c.Evaluator().Eval(v[i].c_str()));
        }
        return vect;
}

std::string XMLHandler::getAttributeAsString(AGDDController& /*c*/,
                                             const xercesc::DOMNode* t,
                                             const std::string& name, bool& isPresent) const
{
        std::string temp=getAttribute(t, name,isPresent);
        return temp;
} 
double XMLHandler::getAttributeAsDouble(AGDDController& c,
                                        const xercesc::DOMNode* t,
                                        const std::string& name, bool& isPresent) const
{
	double res=0.;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		res=c.Evaluator().Eval(temp.c_str());
	}
	return res;
}
int XMLHandler::getAttributeAsInt(AGDDController& c,
                                  const xercesc::DOMNode* t,
                                  const std::string& name, bool& isPresent) const
{
	int res=0;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		res=c.Evaluator().Eval(temp.c_str());
	}
	return res;
}
std::vector<double> XMLHandler::getAttributeAsVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name, bool& isPresent) const
{
	std::vector<double> vect;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
		for (unsigned int i=0;i<v.size();i++)
		{
			vect.push_back(c.Evaluator().Eval(v[i].c_str()));
		}
	}
	return vect;
}

std::vector<int> XMLHandler::getAttributeAsIntVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name, bool& isPresent) const
{
        std::vector<int> vect;
        std::string temp=getAttribute(t, name,isPresent);
        if (isPresent)
        {
                std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
                for (unsigned int i=0;i<v.size();i++)
                {
                        vect.push_back(c.Evaluator().Eval(v[i].c_str()));
                }
        }
        return vect;
}

std::string XMLHandler::getAttributeAsString(AGDDController& /*c*/,
                                             const xercesc::DOMNode* t,
                                             const std::string& name, const std::string& def) const
{
	bool isPresent;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) return temp;
	else return def;
}
double XMLHandler::getAttributeAsDouble(AGDDController& c,
                                        const xercesc::DOMNode* t,
                                        const std::string& name, const double def) const
{
	bool isPresent;
	double res=0.;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		res=c.Evaluator().Eval(temp.c_str());
		return res;
	}
	return def;
}
int XMLHandler::getAttributeAsInt(AGDDController& c,
                                  const xercesc::DOMNode* t,
                                  const std::string& name, const int def) const
{
	bool isPresent;
	int res=0;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		res=c.Evaluator().Eval(temp.c_str());
		return res;
	}
	return def;
}
std::vector<double> XMLHandler::getAttributeAsVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name, const std::vector<double>& def) const
{
	bool isPresent;
	std::vector<double> vect;
	std::string temp=getAttribute(t, name,isPresent);
	if (isPresent) 
	{
		std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
		for (unsigned int i=0;i<v.size();i++)
		{
			vect.push_back(c.Evaluator().Eval(v[i].c_str()));
		}
		return vect;
	}
	return def;
}

std::vector<int> XMLHandler::getAttributeAsIntVector(AGDDController& c,
                                                     const xercesc::DOMNode* t,
                                                     const std::string& name, const std::vector<int>& def) const
{
        bool isPresent;
        std::vector<int> vect;
        std::string temp=getAttribute(t, name,isPresent);
        if (isPresent)
        {
                std::vector<std::string> v=c.Evaluator().tokenize(";",temp);
                for (unsigned int i=0;i<v.size();i++)
                {
                        vect.push_back(c.Evaluator().Eval(v[i].c_str()));
                }
                return vect;
        }
        return def;
}
