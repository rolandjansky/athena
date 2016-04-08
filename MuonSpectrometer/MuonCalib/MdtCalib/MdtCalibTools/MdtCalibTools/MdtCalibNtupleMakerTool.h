/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtCalibNtupleMakerTool.h
//   Header file for class MdtCalibNtupleMakerTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_MDTCALIBNTUPLEMAKERTOOL_H
#define MUONCALIB_MDTCALIBNTUPLEMAKERTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "MdtCalibITools/IMdtCalibTool.h"

#include <string>
#include <vector>

namespace MuonCalib {

  class IMdtCalibration;

  /**@class MdtCalibNtupleMakerTool
     Creates ntuple per calibration region containing segment information
   
     @author Niels.Van.Eldik@cern.ch, 
  */
    
  class MdtCalibNtupleMakerTool : public AthAlgTool, virtual public IMdtCalibTool {
  public:
    /**AlgTool constructor */
    MdtCalibNtupleMakerTool(const std::string&,const std::string&,const IInterface*);

    /** AlgTool initailize method.*/
    StatusCode initialize();

    /** AlgTool finalize method */
    StatusCode finalize();
 
    /** Interface method: return the implementation pointer */
    IMdtCalibration* getImp(std::string name);

    /** dummy methode */
    bool writeToDb( const IMdtCalibrationOutput* , std::string ) const { return true; }
    
  private:

    /** vector to hold pointers to all instances that were created */
    std::vector<IMdtCalibration*> m_instances;
  };

}

#endif // 


