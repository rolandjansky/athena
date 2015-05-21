///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHVScaleRetriever.h 
// Header file for class LArHVScaleRetriever
// Author: P-A Delsart delsart at in2p3.fr
/////////////////////////////////////////////////////////////////// 
#ifndef LARTOOLS_LARHVSCALERETRIEVER_H
#define LARTOOLS_LARHVSCALERETRIEVER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// LArElecCalib includes
#include "LArElecCalib/ILArHVCorrTool.h"
#include "LArElecCalib/ILArHVScaleCorr.h"

#include "GaudiKernel/ToolHandle.h"

// Forward declaration


/////////////////////////////////////////////////////////////////// 
/// \class LArHVScaleRetriever
/// \brief Single tool to retrieve LAr HV correction factor from MC or data input.
///
/// Retrieving LAr HV correction factors is done differently for MC and data input files.
/// This tool is a simple wrapper around the 2 corresponding methods.
///
/// It implements the ILArHVCorrTool interface and provide a IsMC property which should
/// be enough to configure it to read data or MC input.
///
/// Further configuration can be done through 2 properties :
///   - "LArHVCorrTool" : a ILArHVCorrTool instance (retrived by default from "LArHVCorrTool")
///   - KeyHVScaleCorr : a string used to retrieve the DataHandle<ILArHVScaleCorr> (default : "LArHVScaleCorr"
///
/////////////////////////////////////////////////////////////////// 

class LArHVScaleRetriever
  : virtual public ::ILArHVCorrTool,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  LArHVScaleRetriever( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~LArHVScaleRetriever(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();


  // implement ILArHVCorrTool interface 
  virtual float Scale(const HWIdentifier& id)  const  ;
  
  virtual float Scale(const Identifier& id)  const  ;
  
  
  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) ;
  
  virtual StatusCode record() ;

  bool updateOnLastCallback() const;

 private: 

  /// Default constructor: 
  LArHVScaleRetriever();


  ToolHandle<ILArHVCorrTool> m_ilarhvcorrtool;
  const DataHandle<ILArHVScaleCorr> m_ilarhvscalecorr;
  std::string m_keyHVScaleCorr;
  bool m_isMC;
    

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !LARTOOLS_LARHVSCALERETRIEVER_H
