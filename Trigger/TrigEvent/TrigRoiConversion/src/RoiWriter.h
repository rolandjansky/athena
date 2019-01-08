// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    RoiWriter.h        
//
//                   
//  
//
//   $Id: RoiWriter.h 781778 2016-11-01 23:42:26Z sutt $
//
#ifndef TRIGROICONVERSION_ROIWRITER_H
#define TRIGROICONVERSION_ROIWRITER_H

// System include(s):
#include <string>

// Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM include(s):
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

/// Algorithm creating xAOD::RoiDescriptorStore objects
///
/// This algorithm creates xAOD::RoiDescriptorStore objects during AOD writing.
///
/// @author M. Sutton
///
/// $Revision: 781778 $
/// $Date: 2016-11-02 00:42:26 +0100 (Wed, 02 Nov 2016) $
///
class RoiWriter : public AthAlgorithm {

public:
  /// Algorithm constructor
  RoiWriter( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Execute the conversion
  StatusCode execute();
  
private:
   /// Force the overwrite of the xAOD container(s)
   bool m_forceOverwrite;

}; // class RoiWriter

#endif // TRIGROICONVERSION_ROIWRITER_H
