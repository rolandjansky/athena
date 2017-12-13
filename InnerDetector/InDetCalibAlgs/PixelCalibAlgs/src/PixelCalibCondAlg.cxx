/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCalibCondAlg.h"
#include "Identifier/Identifier.h"

PixelCalibCondAlg::PixelCalibCondAlg(const std::string& name
				     , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator )
  , m_readKey("/PIXEL/PixReco")
  , m_writeKey("PixelOfflineCalibData","PixelOfflineCalibData")
  , m_condSvc("CondSvc",name)
{
  declareProperty("ReadKey",m_readKey);
  declareProperty("WriteKey",m_writeKey);
}

PixelCalibCondAlg::~PixelCalibCondAlg()
{
}

StatusCode PixelCalibCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read Handles
  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );

  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelCalibCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<PixelCalib::PixelOfflineCalibData> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  PixelCalib::PixelOfflineCalibData* writeCdo = new PixelCalib::PixelOfflineCalibData();

  SG::ReadCondHandle<DetCondCFloat> readHandle{m_readKey};
  const DetCondCFloat* readCdo{*readHandle}; 
  if(readCdo==nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }

  const float* constants = readCdo->find(Identifier(1));
  if(constants) {
    ATH_MSG_INFO("Found constants with new-style Identifier key");
    writeCdo->SetConstants(constants);
  }
  else {
    Identifier key;
    key.set_literal(1);
    const float* const2 = readCdo->find(key);
    if (const2) {
      ATH_MSG_INFO("Found constants with old-style Identifier key");
      writeCdo->SetConstants(const2);
    }
    else {
      ATH_MSG_ERROR("Could not get the constants!");
      return StatusCode::FAILURE;
    }
  }

  // Define validity of the output cond obbject and record it
  EventIDRange rangeW;
  if(!readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeHandle.record(rangeW,writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record PixelCalib::PixelOfflineCalibData " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelCalibCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

void PixelCalibCondAlg::PrintConstants(const float* constants)
{
  bool isoldformat = (constants[0] > 0);
  int offset = 13; 
  if(constants[0]<-1||constants[1]<-1||constants[2]<-1)offset +=8;
  if(!isoldformat){
    ATH_MSG_DEBUG(" version numbering: " << constants[0] << " " << constants[1] << " " << constants[2]);
    ATH_MSG_DEBUG("Number of cluster error barrel bins for each view is " << constants[3]);
    ATH_MSG_DEBUG("Number of cluster error endcap bins for each view is " << constants[4]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of x cluster size is "  << constants[5]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of y cluster size is "  << constants[6]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of pseudorapidity is "  << constants[7]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of incidence angle is " << constants[8]);
    ATH_MSG_DEBUG("Number of charge interpolation bins for x cluster size is"  << constants[9]);
    ATH_MSG_DEBUG("Number of charge interpolation bins for y cluster size is"  << constants[10]);
    ATH_MSG_DEBUG("Number of charge interpolation bins of pseudorapidity is "  << constants[11]);
    ATH_MSG_DEBUG("Number of charge interpolation bins of incidence angle is " << constants[12]);
    if(offset>13){
      ATH_MSG_DEBUG("Number of cluster on track error IBL bins for x cluster size is " << constants[13]);
      ATH_MSG_DEBUG("Number of cluster on track error IBL bins for y cluster size is " << constants[14]);
      ATH_MSG_DEBUG("Number of cluster on track error IBL bins of pseudorapidity is "  << constants[15]);
      ATH_MSG_DEBUG("Number of cluster on track error IBL bins of incidence angle is " << constants[16]);
      ATH_MSG_DEBUG("Number of charge interpolation IBL bins for x cluster size is " << constants[17]);
      ATH_MSG_DEBUG("Number of charge interpolation IBL bins for y cluster size is " << constants[18]);
      ATH_MSG_DEBUG("Number of charge interpolation IBL bins of pseudorapidity is "  << constants[19]);
      ATH_MSG_DEBUG("Number of charge interpolation IBL bins of incidence angle is " << constants[20]);
    }
  }
  else {
    ATH_MSG_DEBUG("old format constants");     
    ATH_MSG_DEBUG("Number of cluster error barrel bins for each view is " << constants[0]);
    ATH_MSG_DEBUG("Number of cluster error endcap bins for each view is " << constants[1]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of x cluster size is "  << constants[2]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of y cluster size is "  << constants[3]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of incidence angle is " << constants[4]);
    ATH_MSG_DEBUG("Number of cluster on track error barrel bins of pseudorapidity is " << constants[5]);
    ATH_MSG_DEBUG("Number of charge interpolation constants (x-view) is " << constants[6]);
    ATH_MSG_DEBUG("Number of charge interpolation constants (y-view) is " << constants[7]);
  }

  if(!isoldformat){
   int n1 = lrint(constants[3]);
   int n2 = lrint(constants[4]);
   int n3a = lrint(constants[5]);
   int n3b = lrint(constants[6]);
   int n3c = lrint(constants[7]);
   int n3d = lrint(constants[8]);
   int n4a = lrint(constants[9]);
   int n4b = lrint(constants[10]);
   int n4c = lrint(constants[11]);
   int n4d = lrint(constants[12]);
   int ncxibl = offset>13 ?  lrint(constants[13]) : 0;
   int ncyibl = offset>13 ?  lrint(constants[14]) : 0;
   int n3e = offset>13 ?  lrint(constants[15]) : 0;
   int n3f = offset>13 ?  lrint(constants[16]) : 0;
   int ncxibl2 = offset>13 ?  lrint(constants[17]) : 0;
   int ncyibl2 = offset>13 ?  lrint(constants[18]) : 0;
   int n4e = offset>13 ?  lrint(constants[19]) : 0;
   int n4f = offset>13 ?  lrint(constants[20]) : 0;
  
   const int N0 = 2*n1+2*n2;  // number of cluster error values
   const int N1a = n3a*n3d; // number of cluster on track error values (x direction)
   const int N1b = n3a*n3b*n3c; // number of cluster on track error values (y direction)
   const int N1c = n3a+n3b+n3c+n3d; // number of cluster on track bin values
   const int N2a = 6*n4a*n4d+6*n4b*n4c; // number of charge interpolation values
   const int N2b = n4a+n4b+n4c+n4d+4; // number of charge interpolation bin interval values
   int datasize = N0+N1a+N1b+N1c+N2a+N2b;
   if(offset>13) datasize += 2*n1 + n3e+1 + n3f+1 + ncyibl*n3e + ncxibl*n3f +n4e+1 + n4f+1 + ncyibl2*n4e + ncxibl2*n4f;
   ATH_MSG_VERBOSE("Number of constants is " << N0 << " + " 
		   << N1a << " + " 
		   << N1b << " + " << N1c << " + " << N2a
		   << " + " << N2b);
   if(offset>13) 
     ATH_MSG_VERBOSE(" + Number of IBL constants "<<2*n1<< " + "<<
		     (n3e + n3f+2)<<" + "<<(ncyibl*n3e + ncxibl*n3f)<<" + "<<
		     (n4e+1 + n4f+1)<<" + "<<ncyibl2*n4e + ncxibl2*n4f);
   ATH_MSG_VERBOSE(" = " << datasize);
   
   for(int ibin=0; ibin<datasize; ibin++) {
     ATH_MSG_VERBOSE("Value of costant for bin " << ibin << " equal to " << constants[ibin+offset]); 
   }
  } // if(!isoldformat)
  else {
    int n1 = lrint(constants[0]);
    int n2 = lrint(constants[1]);
    int n3a = lrint(constants[2]);
    int n3b = lrint(constants[3]);
    int n3c = lrint(constants[4]);
    int n3d = lrint(constants[5]);
    int n4 = lrint(constants[6]);
    int n5 = lrint(constants[7]);
    const int N0 = 2*n1+2*n2;  // number of cluster error values
    const int N1a = n3a*n3d; // number of cluster on track error values (x direction)
    const int N1b = n3a*n3b*n3d; // number of cluster on track error values (y direction)
    const int N1c = n3a+n3b+n3c+n3d; // number of cluster on track bin values
    const int N2 = n4+n5; // number of charge interpolation values
    const int datasize = N0+N1a+N1b+N1c+N2; 
    
    for(int ibin=0; ibin<datasize; ibin++){
      ATH_MSG_VERBOSE("Value of costant for bin " << ibin << " equal to " << constants[ibin+8]); 
    }
  }
}
