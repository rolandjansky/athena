/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDINTERFACE_IMUONALIGNMENTDBTOOL_H 
#define MUONCONDINTERFACE_IMUONALIGNMENTDBTOOL_H 
 
// Includes for Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaKernel/IAddressProvider.h"
// typedefs for A/BLineMapContainer
#include "MuonAlignmentData/CorrContainer.h"
 
class MdtIdHelper;
 
//namespace MuonCalib 
// {
static const InterfaceID IID_IMuonAlignmentDbTool("IMuonAlignmentDbTool", 1, 0);

class IMuonAlignmentDbTool: virtual public IAlgTool, 
                            virtual public IAddressProvider {
   
 public:

   static const InterfaceID& interfaceID() {return IID_IMuonAlignmentDbTool;};

   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadAlignALine(IOVSVC_CALLBACK_ARGS) = 0;
   virtual StatusCode loadAlignBLine(IOVSVC_CALLBACK_ARGS) = 0;

   virtual std::string aLineFolderName() const = 0;
   virtual std::vector<std::string> abLineFolderNames() const = 0;
   virtual std::string bLineFolderName() const = 0;
   virtual const ALineMapContainer* ALineContainer() const = 0;
   virtual const BLineMapContainer* BLineContainer() const = 0;
   virtual const CscInternalAlignmentMapContainer* ILineContainer() const = 0;
   virtual const MdtAsBuiltMapContainer* AsBuiltContainer() const = 0;
   
   virtual std::string aLineDataLocation() = 0;
   virtual std::string bLineDataLocation() = 0; 
};

//}


#endif  
