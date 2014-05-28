/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XercesParser_H
#define XercesParser_H

#include <string>
#include "AGDD2Geo/ExpressionEvaluator.h"
#include "IAGDDParser.h"

#include <xercesc/dom/DOM.hpp>

using namespace xercesc;

class XercesParser: public IAGDDParser {
public:
	XercesParser();
	XercesParser(std::string);
	bool ParseFile(std::string);
	bool ParseFileAndNavigate(std::string);
	bool ParseString(std::string);
	bool ParseStringAndNavigate(std::string);
	void navigateTree();
	static void elementLoop();
	static void elementLoop(DOMNode*);
	static ExpressionEvaluator& Evaluator();
	static DOMNode* GetCurrentElement() {return currentElement;}
	bool Initialize();
private:
	DOMDocument *doc;
protected:
	static DOMNode *currentElement;
};

#endif
