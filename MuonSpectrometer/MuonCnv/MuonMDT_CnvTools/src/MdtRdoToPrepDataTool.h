/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMdtRdoToPrepDataTool_H
#define MUONMdtRdoToPrepDataTool_H

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
    virtual ~MdtRdoToPrepDataTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;
      
  protected:
    virtual SetupMdtPrepDataContainerStatus setupMdtPrepDataContainer() override;
  }; 
} // end of namespace

#endif 
