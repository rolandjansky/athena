/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SelectEvent.h
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SelectEvent_H
#define DERIVATIONFRAMEWORK_SelectEvent_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "BSelectionParser.h"

/** THE event selection tool
 */
namespace DerivationFramework {

  class SelectEvent : public AthAlgTool, public ISkimmingTool {
    public: 
      SelectEvent(const std::string& t, const std::string& n, const IInterface* p);

      /** inirialization and finalization
       */
      StatusCode initialize();
      StatusCode finalize();
      
      /** @brief: event selection
       *  The event is selected if it contains desired objects 
       */
      virtual bool eventPassesFilter() const;
      
    private:
      
      /** job options
       */
      std::string m_selectionExpression;  //!< expression to determine the final event selection
      
      /** private attributes
       */
      BSelectionParser* m_expressionParser;
  }; 
}

#endif // DERIVATIONFRAMEWORK_SelectEvent_H
