/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAKEINPUTDATAHEADER_H
#define MAKEINPUTDATAHEADER_H
/** @file MakeInputDataHeader.h
 *  @brief This file contains the class definition for the MakeInputDataHeader class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MakeInputDataHeader.h,v 1.2 2008-02-27 21:59:41 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class StoreGateSvc;

/** @classMakeInputDataHeader 
 *  @brief This class provides an algorithm to make an OutputDataHeader the InputDataHeader.
 **/
class MakeInputDataHeader : public ::AthAlgorithm {
public:
   /// Standard Algorithm Constructor
   MakeInputDataHeader(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~MakeInputDataHeader();
   /// AthAlgorithm Interface method implementations:
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();

private:
   /// StreamName, name of Stream to be made Input.
   StringProperty  m_streamName;
   /// KeepCurrentInput, keep the original InputDataHeader: default = false
   BooleanProperty m_keepInput;

   /// Pointer to the data store
   ServiceHandle<StoreGateSvc> p_SGevent;
};

#endif
