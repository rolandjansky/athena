/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   Simple error handler
//
#ifndef GEO_MODEL_XML_STRICT_ERROR_HANDLER_H
#define GEO_MODEL_XML_STRICT_ERROR_HANDLER_H

#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>

class StrictErrorHandler: public xercesc::DOMErrorHandler {

public:
    StrictErrorHandler();
    ~StrictErrorHandler();
    bool getSawErrors() const;
    //
    //  Implementation of the DOM ErrorHandler interface
    //
    bool handleError(const xercesc::DOMError &domError);

private :
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. It's used by the main code to suppress output if there are
    //      errors.
    //
    bool    m_SawErrors;
};

inline bool StrictErrorHandler::getSawErrors() const {
    return m_SawErrors;
}

#endif //STRICT_ERROR_HANDLER_H