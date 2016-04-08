/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_TRKALIGNDBTOOL_H
#define TRKALIGNGENTOOLS_TRKALIGNDBTOOL_H

/** @file TrkAlignDBTool.h
    @class TrkAlignDBTool

    @brief AlgTool used to read in from alignment database and to write 
    parameters to database.
    
    @author Robert Harrington <roberth@bu.edu>
    @date 1/5/08
*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"

namespace Trk {
  
  class TrkAlignDBTool : virtual ITrkAlignDBTool, public AthAlgTool {
    
  public:

    /** Constructor */
    TrkAlignDBTool(const std::string& type, const std::string& name,
	       const IInterface* parent);
    
    /** Virtual destructor */
    virtual ~TrkAlignDBTool();
    
    /** initialize */
    StatusCode initialize();
    
    /** initialize */
    StatusCode finalize();

    /** writes alignment paramters to ASCII file */
    void writeAlignParToAsciiFile() const {}
    
  private:
       
  }; // end of class

} // end of namespace


#endif // TRKALIGNGENTOOLS_TRKALIGNDBTOOL_H
