/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XMLHandler_H
#define XMLHandler_H

#include <string>
#include <xercesc/dom/DOM.hpp>

#include "AGDDControl/ExpressionEvaluator.h"

using namespace xercesc;

class XMLHandlerStore;

class XMLHandler {
public:
	XMLHandler(std::string n);
	virtual ~XMLHandler() {}
	std::string GetName() {return name;}
	virtual void ElementHandle()=0;
	virtual void Handle(DOMNode *t) 
	{
		SetCurrentElement(t);
		ElementHandle();
	}
	void StopLoop(bool);
	bool IsLoopToBeStopped();
protected:
	std::string name;
	bool stopLoop;

	static DOMNode *currentElement;
	static void SetCurrentElement(DOMNode *t) {currentElement=t;}
	static DOMNode *GetCurrentElement() {return currentElement;}
	
	bool isAttribute(const std::string) const;

	std::string getAttribute(const std::string, bool&) const;
    std::string getAttributeAsString(const std::string) const;
    double getAttributeAsDouble(const std::string) const;
    int getAttributeAsInt(const std::string) const;
    std::vector<double> getAttributeAsVector(const std::string) const;
	std::vector<int> getAttributeAsIntVector(const std::string) const;
	std::string getAttributeAsString(const std::string, bool&) const;
	double getAttributeAsDouble(const std::string, bool&) const;
	int getAttributeAsInt(const std::string, bool&) const;
	std::vector<double> getAttributeAsVector(const std::string, bool&) const;
    std::vector<int> getAttributeAsIntVector(const std::string, bool&) const;
	std::string getAttributeAsString(const std::string, const std::string) const;
	double getAttributeAsDouble(const std::string, const double) const;
	int getAttributeAsInt(const std::string, const int) const;
	std::vector<double> getAttributeAsVector(const std::string, const std::vector<double>) const;
    std::vector<int> getAttributeAsIntVector(const std::string, const std::vector<int>) const;
	static bool printFlag;
	
	ExpressionEvaluator& Evaluator() const
	{
		static ExpressionEvaluator eval;
		return eval;	
	}
private:
	void RegisterToStore();
};

#endif
