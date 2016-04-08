/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include <iostream>

//using namespace xercesc;

namespace pool{

class XMLDOMErrorMessanger : public xercesc::ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
  XMLDOMErrorMessanger();
    

    ~XMLDOMErrorMessanger()
    {
    }

    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const xercesc::SAXParseException& toCatch);
    void error(const xercesc::SAXParseException& toCatch);
    void fatalError(const xercesc::SAXParseException& toCatch);
    void resetErrors();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};

inline bool XMLDOMErrorMessanger::getSawErrors() const
{
    return fSawErrors;
}
} /* ns pool */
