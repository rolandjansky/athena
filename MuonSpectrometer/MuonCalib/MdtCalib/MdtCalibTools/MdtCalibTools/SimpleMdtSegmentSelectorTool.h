/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleMdtSegmentSelectorTool.h
//   Header file for class SimpleMdtSegmentSelectorTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_SIMPLEMDTSEGMENTSELECTORTOOL_H
#define MUONCALIB_SIMPLEMDTSEGMENTSELECTORTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibITools/IMdtSegmentSelectorTool.h"


namespace MuonCalib {

  class IMdtSegmentSelector;

/**@class SimpleMdtSegmentSelectorTool
   Creates an instance of a IMdtSegmentSelector implementation.
   
   @author Niels.Van.Eldik@cern.ch, 
*/
  class SimpleMdtSegmentSelectorTool : public AthAlgTool, virtual public IMdtSegmentSelectorTool {
  
  public:
    /**AlgTool constructor */
    SimpleMdtSegmentSelectorTool(const std::string&,
				 const std::string&,
				 const IInterface*);

    /**Virtual destructor*/
    virtual ~SimpleMdtSegmentSelectorTool(){};
    
    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();
 
    /** Interface method: return the implementation pointer */
    virtual const IMdtSegmentSelector* getImp() { return m_implementation; }
   
  private:
    /** Pointer to the concrete implementation */
    const IMdtSegmentSelector* m_implementation;
  
    /** minimum chi2 */
    double m_chi2;

    /** minimum number of HitOnTrack */
    unsigned int m_minHits;
  };

}

#endif //
