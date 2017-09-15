/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

@page MuonRdoToPrepData_page Short Description: Muon RDO conversion to PrepRawData

 - The Raw Data Object (RDO) classes of the MuonSpectrometer are implemented in the package MuonRDO. The 
Reconstruction Input/Output Object (RIO) classes are in MuonDetRec/MuonPrepRawData.
  
 - CscRdoToCscPrepData: it reads in the collection of CSC RDO and converts then into reconstruction 
input/output objects, using the PrepRawData class. The CSC calibration tools and services are needed for this 
conversion RDO to PrepRawData. Eventually, a cabling service will be needed. The RIO can then be used as input 
in the reconstruction, and or the clusterization. 

 - RpcRdoToRpcPrepData: it reads in the collection of RPC RDO and converts then into reconstruction 
input/output objects, using the PrepRawData class. The RPC cabling service and the RpcRDO_Decoder are needed for this 
conversion. The RIO can then be used as input in the reconstruction, and or the clusterization. 

 - For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov
