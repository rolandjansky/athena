// Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_DECISIONALG_H
#define ATHENASERVICES_DECISIONALG_H

// STL include files
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>

// Required for inheritance
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"
#include "GaudiKernel/IIoComponent.h"

// forward declarations
class IClassIDSvc;
class OutputStreamSequencerSvc;
class StoreGateSvc;
class IDecisionAlgTool;
class IAthenaOutputTool;
class IItemListSvc;

namespace SG {
   class DataProxy;
   class IFolder;
   class FolderItem;
}

/** @class DecisionAlg
   * @brief Call through interface to FilteredAlgorithm
   * 
   * @author srinir@bnl.gov
   * $Id: DecisionAlg.h,v 1.12 2008-12-15 19:14:58 binet Exp $
   */
class DecisionAlg : public FilteredAlgorithm
{
public:
   
   /// Standard algorithm Constructor
   DecisionAlg(const std::string& name, ISvcLocator* pSvcLocator); 
   /// Standard Destructor
   virtual ~DecisionAlg();

};

#endif // ATHENASERVICES_OUTPUTSTREAM_H
