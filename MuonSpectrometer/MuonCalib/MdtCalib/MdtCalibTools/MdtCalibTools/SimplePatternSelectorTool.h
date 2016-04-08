/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimplePatternSelectorTool.h
//   Header file for class SimplePatternSelectorTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_SIMPLEPATTERNSELECTORTOOL_H
#define MUONCALIB_SIMPLEPATTERNSELECTORTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibITools/IPatternSelectorTool.h"



namespace MuonCalib {

  class ICalibPatternSelector;

/**@class SimplePatternSelectorTool
   Creates an instance of a IGlobalPatternSelector implementation.
   
   @author Niels.Van.Eldik@cern.ch, 
*/
  class SimplePatternSelectorTool : public AthAlgTool, virtual public IPatternSelectorTool {
  
  public:
    /**AlgTool constructor */
    SimplePatternSelectorTool(const std::string&,
			      const std::string&,
			      const IInterface*);

    /**Virtual destructor*/
    virtual ~SimplePatternSelectorTool(){};
    
    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();
 
    /** Interface method: return the implementation pointer */
    virtual const ICalibPatternSelector* getImp() { return m_implementation; }
   
  private:
    /** Pointer to the concrete implementation */
    const ICalibPatternSelector* m_implementation;
  
 
  };

}

#endif //
