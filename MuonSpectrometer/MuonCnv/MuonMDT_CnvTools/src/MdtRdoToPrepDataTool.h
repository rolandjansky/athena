/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMdtRdoToPrepDataTool_H
#define MUONMdtRdoToPrepDataTool_H

/// This code is only used in the single-thread setup   
/// As such, deactivating the check in this file        
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "MdtRdoToPrepDataToolCore.h"

namespace Muon 
{

  /** @class MdtRdoToPrepDataTool 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Edward Moyse <Edward.Moyse@cern.ch>
  */  

  class MdtRdoToPrepDataTool : virtual public MdtRdoToPrepDataToolCore
  {
  public:
    MdtRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~MdtRdoToPrepDataTool()=default;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
      
  protected:
    virtual SetupMdtPrepDataContainerStatus setupMdtPrepDataContainer() override;
  }; 
} // end of namespace

#endif 
