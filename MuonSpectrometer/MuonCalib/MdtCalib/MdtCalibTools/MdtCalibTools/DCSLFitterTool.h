/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DCSLFitterTool.h
//   Header file for class DCSLFitterTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_DCSLFITTERTOOL_H
#define MUONCALIB_DCSLFITTERTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibITools/IMdtSegmentFitterTool.h"


namespace MuonCalib {

  class IMdtSegmentFitter;

/**@class DCSLFitterTool
   Tool creates an instance of a IMdtSegmentFitter implementation.
   
   @author Niels.Van.Eldik@cern.ch, 
*/
  class DCSLFitterTool : public AthAlgTool, virtual public IMdtSegmentFitterTool {
  
  public:
    /**AlgTool constructor */
    DCSLFitterTool(const std::string&,const std::string&,const IInterface*);

    /**Virtual destructor*/
    virtual ~DCSLFitterTool(){};
    
    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();
 
    /** Interface method: return the implementation pointer */
    IMdtSegmentFitter* getImp() { return m_implementation; }
   
  private:
    /** Pointer to the concrete implementation */
    IMdtSegmentFitter* m_implementation;
  
 
  };

}

#endif //
