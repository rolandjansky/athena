/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAGREMAPPINTOOL_H
#define TAGREMAPPINTOOL_H

/** @file TagRemappingTool.h
 *  @brief This file contains the class definition for the TagRemappingTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaSelectorTool.h"

#include <string>

class StoreGateSvc;
namespace pool {
   class ICollection;
   class ICollectionCursor;
}

/** @class TagRemappingTool
 *  @brief This class provides a ISelectorTool to remap TAGs to down stream references.
 **/
class TagRemappingTool : public AthAlgTool, virtual public IAthenaSelectorTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   TagRemappingTool(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~TagRemappingTool();

public:
   /// IAthenaSelectorTool Interface method implementations:
   virtual StatusCode initialize();
   virtual StatusCode postInitialize();
   virtual StatusCode preNext();
   virtual StatusCode postNext();
   virtual StatusCode preFinalize();
   virtual StatusCode finalize();

private: // data
   ServiceHandle<StoreGateSvc> m_eventStore;
   pool::ICollection* m_poolColl;
   pool::ICollectionCursor* m_poolCursor;
   std::string m_attrListKey;
   std::string m_refName;

private: // properties
   StringProperty m_targetColl;
};

#endif
