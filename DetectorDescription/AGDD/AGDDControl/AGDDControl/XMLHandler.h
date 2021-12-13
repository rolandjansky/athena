/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLHandler_H
#define XMLHandler_H

#include <string>
#include <vector>
#include <xercesc/dom/DOM.hpp>

class XMLHandlerStore;
class AGDDController;

class XMLHandler {
public:
	XMLHandler(const std::string& n, AGDDController& c);
  	virtual ~XMLHandler() {}
	std::string GetName() const {return m_name;}
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t)=0;
        virtual void Handle(AGDDController& c,
                            xercesc::DOMNode *t) 
	{
		ElementHandle(c, t);
	}
	void StopLoop(bool);
	bool IsLoopToBeStopped() const;
protected:
	std::string m_name;
	bool m_stopLoop;

	bool isAttribute(const xercesc::DOMNode* t,
                         const std::string&) const;

	std::string getAttribute(const xercesc::DOMNode* t,
                                 const std::string&, bool&) const;
        std::string getAttributeAsString(AGDDController& c,
                                         const xercesc::DOMNode* t,
                                         const std::string&) const;
        double getAttributeAsDouble(AGDDController& c,
                                    const xercesc::DOMNode* t,
                                    const std::string&) const;
        int getAttributeAsInt(AGDDController& c,
                              const xercesc::DOMNode* t,
                              const std::string&) const;
        std::vector<double> getAttributeAsVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&) const;
	std::vector<int> getAttributeAsIntVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&) const;
        std::string getAttributeAsString(AGDDController& c,
                                         const xercesc::DOMNode* t,
                                         const std::string&, bool&) const;
	double getAttributeAsDouble(AGDDController& c,
                                    const xercesc::DOMNode* t,
                                    const std::string&, bool&) const;
	int getAttributeAsInt(AGDDController& c,
                              const xercesc::DOMNode* t,
                              const std::string&, bool&) const;
	std::vector<double> getAttributeAsVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&, bool&) const;
        std::vector<int> getAttributeAsIntVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&, bool&) const;
	std::string getAttributeAsString(AGDDController& c,
                                         const xercesc::DOMNode* t,
                                         const std::string&, const std::string&) const;
	double getAttributeAsDouble(AGDDController& c,
                                    const xercesc::DOMNode* t,
                                    const std::string&, const double) const;
	int getAttributeAsInt(AGDDController& c,
                              const xercesc::DOMNode* t,
                              const std::string&, const int) const;
	std::vector<double> getAttributeAsVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&, const std::vector<double>&) const;
        std::vector<int> getAttributeAsIntVector(AGDDController& c,
                                                 const xercesc::DOMNode* t,
                                                 const std::string&, const std::vector<int>&) const;
	
private:
	void RegisterToStore(AGDDController& c);
};

#endif
