/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEALGORITHMS_QUERYTAG_H
#define ATHENAPOOLEXAMPLEALGORITHMS_QUERYTAG_H

/** @file QueryTag.h
 *  @brief This file contains the class definition for the QueryTag class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: QueryTag.h,v 1.1 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaSelectorTool.h"

#include <string>

namespace AthPoolEx {

/** @class AthPoolEx::QueryTag
 *  @brief This class provides an example for reading with a ISelectorTool to veto events on AttributeList.
 **/
class QueryTag : public AthAlgTool, virtual public IAthenaSelectorTool {
public: // Constructor and Destructor
   /// Standard Service Constructor
   QueryTag(const std::string& type, const std::string& name, const IInterface* parent);
   /// Destructor
   virtual ~QueryTag();

public:
   /// IAthenaSelectorTool Interface method implementations:
   virtual StatusCode initialize();
   virtual StatusCode postInitialize();
   virtual StatusCode preNext();
   virtual StatusCode postNext();
   virtual StatusCode preFinalize();
   virtual StatusCode finalize();

private:
   std::string m_attrListKey;
};

} // end AthPoolEx namespace

#endif
