/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XercesParser_H
#define XercesParser_H

#include <string>
#include "AGDDControl/ExpressionEvaluator.h"
#include "AGDDControl/IAGDDParser.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

class AGDDController;

class XercesParser: public IAGDDParser {
public:
    XercesParser();
    XercesParser(const std::string&);
    virtual ~XercesParser();
    virtual bool ParseFile(const std::string&) override;
    virtual bool ParseFileAndNavigate(AGDDController& c,
                                      const std::string&) override;
    virtual bool ParseString(const std::string&) override;
    virtual bool ParseStringAndNavigate(AGDDController& c,
                                        const std::string&) override;
    virtual bool WriteToFile(const std::string&) override;
    virtual void navigateTree(AGDDController& c) override;
    static void elementLoop();
    static void elementLoop(AGDDController& c, xercesc::DOMNode*);
    static ExpressionEvaluator& Evaluator();
    bool Initialize();
    bool Finalize();
private:
    xercesc::DOMDocument *m_doc;
    xercesc::XercesDOMParser *m_parser;
    bool m_initialized;
};

#endif
