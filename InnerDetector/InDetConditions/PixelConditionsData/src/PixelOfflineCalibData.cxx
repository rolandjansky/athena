/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelConditionsData/PixelOfflineCalibData.h"

namespace PixelCalib{

  // PixelCalibData::~PixelCalibData(){}

float* PixelOfflineCalibData::GetConstants() const { 

  // format version of parametrizations (note it is an increasing NEGATIVE number)
  int v1 = m_clustererrordata->getVersion(); 
  int v2 = m_clusterontrackerrordata->getVersion(); 
  int v3 = m_chargeinterpolationparameters->getVersion(); 

  int n1 = m_clustererrordata->getNumberOfBarrelBins(); 
  int n2 = m_clustererrordata->getNumberOfEndcapBins();
  // get the bins of the cluster on track error data parametrization 
  const std::vector<float> csx = m_clusterontrackerrordata->getClusterSizeBinsX();
  const std::vector<float> csy = m_clusterontrackerrordata->getClusterSizeBinsY();
  const std::vector<float> eta = m_clusterontrackerrordata->getEtaBins();
  const std::vector<float> alpha = m_clusterontrackerrordata->getIncidenceAngleBins();
  //IBL
  const std::vector<float> etaibl = m_clusterontrackerrordata->getEtaIBLBins();
  const std::vector<float> alphaibl = m_clusterontrackerrordata->getIncidenceAngleIBLBins();
  //
  // number of bins 
  // Since the upper value is always implicit, numer of bins is the size of the vector
  int ncsx = csx.size();
  int ncsy = csy.size();
  int neta = eta.size();
  int nalpha = alpha.size();
  int ncsx_ibl = m_clusterontrackerrordata->getIBLcsxbins();
  int ncsy_ibl = m_clusterontrackerrordata->getIBLcsybins();
  int netaibl = m_clusterontrackerrordata->getIBLetabins();
  int nalphaibl =m_clusterontrackerrordata->getIBLphibins(); 
  // get the bins of the charge interpolation parametrization 
  const std::vector<float> csx2 = m_chargeinterpolationparameters->getClusterSizeXBins();
  const std::vector<float> csy2 = m_chargeinterpolationparameters->getClusterSizeYBins();
  const std::vector<float> eta2 = m_chargeinterpolationparameters->getEtaBins();
  const std::vector<float> alpha2 = m_chargeinterpolationparameters->getAngleBins();
  // IBL
  const std::vector<float> etaibl2 =  m_chargeinterpolationparameters->getIBLEtaBins() ;
  const std::vector<float> alphaibl2 = m_chargeinterpolationparameters->getIBLAngleBins() ;
  //
  // number of bins
  // The upper limit is not implicit, we must subtract one!
  int ncsx2 = csx2.size()-1;
  int ncsy2 = csy2.size()-1;
  int neta2 = eta2.size()-1;
  int nalpha2 = alpha2.size()-1;
  int ncsx2_ibl = m_chargeinterpolationparameters->getIBLcsxbins();
  int ncsy2_ibl = m_chargeinterpolationparameters->getIBLcsybins();
  int netaibl2 =m_chargeinterpolationparameters->getIBLetabins(); 
  int nalphaibl2 = m_chargeinterpolationparameters->getIBLphibins();

  // Compute the overall number of values to store

  int n3a = ncsx*nalpha; // ClusterErrorOnTrack - number of barrel phi bins
  int n3b = ncsx*ncsy*neta; // ClusterErrorOnTrack - number of barrel eta bins
  int n3c = ncsx_ibl*nalphaibl;
  int n3d = ncsy_ibl*netaibl;
  int n4 = 6*ncsx2*nalpha2; // ChargeInterpolation - number of barrel phi bins
  int n5 = 6*ncsy2*neta2; // ChargeInterpolation - number of barrel eta bins
  int n6 = ncsx2_ibl*nalphaibl2;
  int n7 = ncsy2_ibl*netaibl2;
  int datasize = 
    2*n1+2*n2  // number of ClusterError values
    +ncsx+ncsy+neta+nalpha // number of ClusterErrorOnTrack bin extremes values
    +n3a+n3b // number of ClusterErrorOnTrack values
    +ncsx2+1+ncsy2+1+neta2+1+nalpha2+1 // number of ChargeInterpolation bin extremes values
    +n4+n5; // number of ChargeInterpolation values
  //std::cout << "AAA bins cluster error on track: " << ncsx << " " << ncsy << " " << neta 
    //        << " " << nalpha << std::endl;
  //std::cout << "AAA bins charge interpolation: " << ncsx2 << " " << ncsy2 << " " << neta2 
    //        << " " << nalpha2 << std::endl;
  //std::cout << "AAA clusterError: " << 2*n1 << " + " << 2*n2 << " + " 
    //        << ncsx+ncsy+neta+nalpha << " bins" << std::endl;
  //std::cout << "AAA clusterErrorOnTrack: " << n3a << " + " << n3b << "bins" <<
    //           std::endl;
  //std::cout << "AAA charge interpolation: " << n4 << " + " << n5 << " + " 
    //        << ncsx2+1+ncsy2+1+neta2+1+nalpha2+1 << "bins" << std::endl;
  //std::cout << "AAA datasize is " << datasize << " : create vector with " 
    //        << datasize+13 << "floats " << std::endl; 
  int offset = 13;
  if(v1<-1||v2<-1||v3<-1) { // including IBL
    offset +=8;
    datasize +=2*n1 + netaibl+1 + nalphaibl+1 + n3c +n3d + netaibl2+1 + nalphaibl2+1 + n6 + n7;
  }
  float* constants = new float[datasize+offset]; // 13 values needed for storing the number of bins
                                             // and version numbering
  constants[0] = v1;
  constants[1] = v2;
  constants[2] = v3; 
  constants[3] = n1;
  constants[4] = n2;
  constants[5] = ncsx;
  constants[6] = ncsy;
  constants[7] = neta;
  constants[8] = nalpha;
  constants[9] = ncsx2;
  constants[10] = ncsy2;
  constants[11] = neta2;
  constants[12] = nalpha2;
  if(offset>13){
    constants[13] = ncsx_ibl;
    constants[14] = ncsx_ibl; 
    constants[15] = netaibl;
    constants[16] = nalphaibl;
    constants[17] = ncsx2_ibl;
    constants[18] = ncsy2_ibl; 
    constants[19] = netaibl2;
    constants[20] = nalphaibl2;
  }
  // set bins of cluster error on track parametrization (uppermost value implicit)
  for(int i=0; i<ncsx; i++){ constants[offset+i] = csx[i]; }
  offset += ncsx;
  for(int i=0; i<ncsy; i++){ constants[offset+i] = csy[i]; }
  offset += ncsy;
  if(netaibl>0){
    for(int i=0; i<netaibl+1; i++){ constants[offset+i] = etaibl[i]; }
    offset += netaibl+1;
  }
  if(nalphaibl>0){
    for(int i=0; i<nalphaibl+1; i++){ constants[offset+i] = alphaibl[i]; }
    offset += nalphaibl+1;
  }
  for(int i=0; i<neta; i++){ constants[offset+i] = eta[i]; }
  offset += neta;
  for(int i=0; i<nalpha; i++){ constants[offset+i] = alpha[i]; }
  offset += nalpha;
  //  std::cout << "offset is " << offset << std::endl;
  // set bins of charge interpolation parametrization (uppermost value stored)
  for(int i=0; i<ncsx2+1; i++){ constants[offset+i] = csx2[i]; }
  offset += ncsx2+1;
  for(int i=0; i<ncsy2+1; i++){ constants[offset+i] = csy2[i]; }
  offset += ncsy2+1;
  if(netaibl2>0){
    for(int i=0; i<netaibl2+1; i++){ constants[offset+i] = etaibl2[i]; }
    offset += netaibl2+1;
  }
  if(nalphaibl2>0){
    for(int i=0; i<nalphaibl2+1; i++){ constants[offset+i] = alphaibl2[i]; }
    offset += nalphaibl2+1;
  }
  for(int i=0; i<neta2+1; i++){ constants[offset+i] = eta2[i]; }
  offset += neta2+1;
  for(int i=0; i<nalpha2+1; i++){ constants[offset+i] = alpha2[i]; }
  offset += nalpha2+1;
   //std::cout << "offset is " << offset << std::endl;
  // parametrization values
  for(int i=0; i<n1; i++){
    constants[offset+i] = m_clustererrordata->getPixelBarrelPhiError(i);
    constants[offset+n1+i] = m_clustererrordata->getPixelBarrelEtaError(i);
  }
  offset += 2*n1;
  for(int i=0; i<n2; i++){
    constants[offset+i] = m_clustererrordata->getPixelEndcapPhiError(i);
    constants[offset+n2+i] = m_clustererrordata->getPixelEndcapRError(i);
  }
  offset += 2*n2;
  // IBL
  if(v1<-1){
    for(int i=0; i<n1; i++){
      constants[offset+i] = m_clustererrordata->getPixelIBLPhiError(i);
      constants[offset+n1+i] = m_clustererrordata->getPixelIBLEtaError(i);
    }
    offset += 2*n1;
  }
  //
  //std::cout << "offset is " << offset << std::endl;
  //IBL
  if(n3c>0){ 
    for(int i=0; i<n3c; i++){
      constants[offset+i] = m_clusterontrackerrordata->getPixelIBLPhiError(i);
    }
    offset += n3c;
  }
  for(int i=0; i<n3a; i++){
    constants[offset+i] = m_clusterontrackerrordata->getPixelBarrelPhiError(i);
  }
  offset += n3a;
  if(n3d>0){
    for(int i=0; i<n3d; i++){
      constants[offset+i] = m_clusterontrackerrordata->getPixelIBLEtaError(i);
    }
    offset += n3d;
  }
  for(int i=0; i<n3b; i++){
    constants[offset+i] = m_clusterontrackerrordata->getPixelBarrelEtaError(i);
  }
  offset += n3b;

  //
  //std::cout << "offset is " << offset << std::endl;

  for(int i=0; i<n6+n4; i++){
    constants[offset+i] = m_chargeinterpolationparameters->getDeltaX(i);
  }
  offset += n6+n4;
  //std::cout << "offset is " << offset << ", " << n5 << "values to go" << std::endl;
  for(int i=0; i<n7+n5; i++){
    constants[offset+i] = m_chargeinterpolationparameters->getDeltaY(i);
  }
  offset +=n7+n5;
  //
  //std::cout << "return constants" << std::endl;
  return constants;
}

void PixelOfflineCalibData::Dump(){
  m_clustererrordata->Print("PixelClusterDump.txt");
  m_clusterontrackerrordata->Print("PixelClusterOnTrackDump.txt");
  m_chargeinterpolationparameters->Print("PixelChargeInterpolationParametersDump.txt");
}

void PixelOfflineCalibData::SetConstants(const float* constants){ 

  if(constants[0] > 0){    // old format
    m_clustererrordata->setVersion(0);
    m_clusterontrackerrordata->setVersion(0);
    m_chargeinterpolationparameters->setVersion(0);

    const int n1 = int(constants[0]); // number of PixelClusterError barrel bins (60)
    const int n2 = int(constants[1]); // number of PixelClusterError endcap bins (12)
    const int ncsx = int(constants[2]); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy = int(constants[3]); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta = int(constants[4]); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha = int(constants[5]); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    const int n4 = int(constants[6]); // number of charge interpolation constants (x direction)
    const int n5 = int(constants[7]); // number of charge interpolation constants (y direction)
    int ierr = 1; // error code
    int offset = 8;
    m_clusterontrackerrordata->setParameters(ncsx,ncsy,neta,nalpha,offset,constants);
    offset += ncsx+ncsy+neta+nalpha;
    for(int i=0; i<n1; i++){
      ierr *= m_clustererrordata->setPixelBarrelPhiError(i,constants[i+offset]);
      ierr *= m_clustererrordata->setPixelBarrelEtaError(i,constants[i+n1+offset]);
    }
    offset += 2*n1;
    for(int i=0; i<n2; i++){
      ierr *= m_clustererrordata->setPixelEndcapPhiError(i,constants[offset+i]);
      ierr *= m_clustererrordata->setPixelEndcapRError(i,constants[offset+n2+i]);
    }
    offset += 2*n2;
    int n3a = ncsx*nalpha; // number of barrel phi bins
    int n3b = ncsx*ncsy*neta; // number of barrel eta bins
    for(int i=0; i<n3a; i++){
      m_clusterontrackerrordata->setPixelBarrelPhiError(i,constants[offset+i]);
    }
    offset += n3a;
    for(int i=0; i<n3b; i++){
      m_clusterontrackerrordata->setPixelBarrelEtaError(i,constants[offset+i]);
    }
    offset += n3b;
    for(int i=0; i<n4; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaX(i,constants[offset+i]);
    }
    offset += n4;
    for(int i=0; i<n5; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaY(i,constants[offset+i]);
    }
    m_chargeinterpolationparameters->setVersion(-1);
    if(ierr == 0) std::cout << "something went wrong!" << std::endl;
    return;

  }
  else{ 

    const int v1 = int(constants[0]); // format version number for PixelClusterError
    const int v2 = int(constants[1]); // format version number for PixelClusterOnTrackError
    const int v3 = int(constants[2]); // format version number for PixelChargeInterpolation
    m_clustererrordata->setVersion(v1);
    m_clusterontrackerrordata->setVersion(v2);
    m_chargeinterpolationparameters->setVersion(v3);

    const int n1 = int(constants[3]); // number of PixelClusterError barrel bins (60)
    const int n2 = int(constants[4]); // number of PixelClusterError endcap bins (12)
    const int ncsx = int(constants[5]); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy = int(constants[6]); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta = int(constants[7]); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha = int(constants[8]); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    const int ncsx2 = int(constants[9]); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy2 = int(constants[10]); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta2 = int(constants[11]); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha2 = int(constants[12]); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    int ierr = 1; // error code
    int offset = 13;
    if(v1<-1||v2<-1||v3<-1)offset +=8; 
    const int ncsx_ibl = offset>13 ? int(constants[13]) : 0;
    const int ncsy_ibl = offset>13 ? int(constants[14]) : 0;
    const int netaibl = offset>13 ? int(constants[15]) : 0;
    const int nalphaibl = offset>13 ? int(constants[16]) : 0;
    const int ncsx2_ibl = offset>13 ? int(constants[17]) : 0;
    const int ncsy2_ibl = offset>13 ? int(constants[18]) : 0;
    const int netaibl2 = offset>13 ? int(constants[19]) : 0;
    const int nalphaibl2 = offset>13 ? int(constants[20]) : 0;
    if(v2<-1){
      m_clusterontrackerrordata->setIBLcsxbins(ncsx_ibl);
      m_clusterontrackerrordata->setIBLcsybins(ncsy_ibl);
      m_clusterontrackerrordata->setIBLetabins(netaibl);
      m_clusterontrackerrordata->setIBLphibins(nalphaibl);
    }
    m_clusterontrackerrordata->setParameters(ncsx,ncsy,neta,nalpha,offset,constants);
    offset += ncsx+ncsy+neta+nalpha; // check that offset is advanced or not 
    if(v2<-1) offset +=netaibl+1+nalphaibl+1; 
    if(v3<-1){
      m_chargeinterpolationparameters->setIBLcsxbins(ncsx2_ibl);
      m_chargeinterpolationparameters->setIBLcsybins(ncsy2_ibl);
      m_chargeinterpolationparameters->setIBLetabins(netaibl2);
      m_chargeinterpolationparameters->setIBLphibins(nalphaibl2);
    }

    m_chargeinterpolationparameters->setParameters(ncsx2,ncsy2,neta2,nalpha2,offset,constants);
    offset += (ncsx2+1)+(ncsy2+1)+(neta2+1)+(nalpha2+1);
    if(v3<-1) offset += (netaibl2+1) + (nalphaibl2+1);
    for(int i=0; i<n1; i++){
      ierr *= m_clustererrordata->setPixelBarrelPhiError(i,constants[i+offset]);
      ierr *= m_clustererrordata->setPixelBarrelEtaError(i,constants[i+n1+offset]);
    }
    offset += 2*n1;
    for(int i=0; i<n2; i++){
      ierr *= m_clustererrordata->setPixelEndcapPhiError(i,constants[offset+i]);
      ierr *= m_clustererrordata->setPixelEndcapRError(i,constants[offset+n2+i]);
    }
    offset += 2*n2;
    if(v1<-1){
      for(int i=0; i<n1; i++){
	ierr *= m_clustererrordata->setPixelIBLPhiError(i,constants[i+offset]);
	ierr *= m_clustererrordata->setPixelIBLEtaError(i,constants[i+n1+offset]);
      }
      offset +=2*n1;
    }
    int n3a = ncsx*nalpha; // number of barrel phi bins
    int n3b = ncsx*ncsy*neta; // number of barrel eta bins
    int n3c = ncsx_ibl*nalphaibl;
    int n3d = ncsy_ibl*netaibl;
    if(n3c>0){
      for(int i=0; i<n3c; i++){
	m_clusterontrackerrordata->setPixelIBLPhiError(i,constants[offset+i]);
      }
      offset += n3c;
    }
    for(int i=0; i<n3a; i++){
      m_clusterontrackerrordata->setPixelBarrelPhiError(i,constants[offset+i]);
    }
    offset += n3a;

    if(n3d>0){
      for(int i=0; i<n3d; i++){
	m_clusterontrackerrordata->setPixelIBLEtaError(i,constants[offset+i]);
      }
      offset += n3d;
    }

    for(int i=0; i<n3b; i++){
      m_clusterontrackerrordata->setPixelBarrelEtaError(i,constants[offset+i]);
    }
    offset += n3b;
    const int n4 = 6*ncsx2*nalpha2; // number of charge interpolation constants (x direction)
    const int n5 = 6*ncsy2*neta2; // number of charge interpolation constants (y direction)
    const int n6 = ncsx2_ibl*nalphaibl2;
    const int n7 = ncsy2_ibl*netaibl2;

    for(int i=0; i<n6+n4; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaX(i,constants[offset+i]);
    }
    offset += n6+n4;
    for(int i=0; i<n7+n5; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaY(i,constants[offset+i]);
    }
    offset += n7+n5;
    
    if(ierr == 0) std::cout << "something went wrong!" << std::endl;
    return;
  }
}

} // end of namespace 
