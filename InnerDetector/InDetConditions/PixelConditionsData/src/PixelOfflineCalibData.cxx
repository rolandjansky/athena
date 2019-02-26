/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOfflineCalibData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelConditionsData/PixelOfflineCalibData.h"

namespace PixelCalib{

  // PixelCalibData::~PixelCalibData(){}

std::vector<float> PixelOfflineCalibData::GetConstants() const { 

  // format version of parametrizations (note it is an increasing NEGATIVE number)
  int v1 = m_clustererrordata->getVersion(); 
  int v2 = m_clusterontrackerrordata->getVersion(); 
  int v3 = m_chargeinterpolationparameters->getVersion(); 

  int n1 = m_clustererrordata->getNumberOfBarrelBins(); 
  int n2 = m_clustererrordata->getNumberOfEndcapBins();
  // get the bins of the cluster on track error data parametrization 
  const std::vector<float> &csx = m_clusterontrackerrordata->getClusterSizeBinsX();
  const std::vector<float> &csy = m_clusterontrackerrordata->getClusterSizeBinsY();
  const std::vector<float> &eta = m_clusterontrackerrordata->getEtaBins();
  const std::vector<float> &alpha = m_clusterontrackerrordata->getIncidenceAngleBins();
  //IBL
  const std::vector<float> &etaibl = m_clusterontrackerrordata->getEtaIBLBins();
  const std::vector<float> &alphaibl = m_clusterontrackerrordata->getIncidenceAngleIBLBins();
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
  const std::vector<float> &csx2 = m_chargeinterpolationparameters->getClusterSizeXBins();
  const std::vector<float> &csy2 = m_chargeinterpolationparameters->getClusterSizeYBins();
  const std::vector<float> &eta2 = m_chargeinterpolationparameters->getEtaBins();
  const std::vector<float> &alpha2 = m_chargeinterpolationparameters->getAngleBins();
  // IBL
  const std::vector<float> &etaibl2 =  m_chargeinterpolationparameters->getIBLEtaBins() ;
  const std::vector<float> &alphaibl2 = m_chargeinterpolationparameters->getIBLAngleBins() ;
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

  int offset = 13;
  if(v1<-1||v2<-1||v3<-1) { // including IBL
    offset +=8;
    datasize +=2*n1 + netaibl+1 + nalphaibl+1 + n3c +n3d + netaibl2+1 + nalphaibl2+1 + n6 + n7;
  }

  std::vector<float> constants;
  constants.resize(datasize+offset);

  constants.push_back(v1);
  constants.push_back(v2);
  constants.push_back(v3); 
  constants.push_back(n1);
  constants.push_back(n2);
  constants.push_back(ncsx);
  constants.push_back(ncsy);
  constants.push_back(neta);
  constants.push_back(nalpha);
  constants.push_back(ncsx2);
  constants.push_back(ncsy2);
  constants.push_back(neta2);
  constants.push_back(nalpha2);
  if(offset>13){
    constants.push_back(ncsx_ibl);
    constants.push_back(ncsx_ibl); 
    constants.push_back(netaibl);
    constants.push_back(nalphaibl);
    constants.push_back(ncsx2_ibl);
    constants.push_back(ncsy2_ibl); 
    constants.push_back(netaibl2);
    constants.push_back(nalphaibl2);
  }

  // set bins of cluster error on track parametrization (uppermost value implicit)
  for(int i=0; i<ncsx; i++){ constants.push_back(csx[i]); }
  for(int i=0; i<ncsy; i++){ constants.push_back(csy[i]); }
  if(netaibl>0){
    for(int i=0; i<netaibl+1; i++){ constants.push_back(etaibl[i]); }
  }
  if(nalphaibl>0){
    for(int i=0; i<nalphaibl+1; i++){ constants.push_back(alphaibl[i]); }
  }
  for(int i=0; i<neta; i++){ constants.push_back(eta[i]); }
  for(int i=0; i<nalpha; i++){ constants.push_back(alpha[i]); }
  // set bins of charge interpolation parametrization (uppermost value stored)
  for(int i=0; i<ncsx2+1; i++){ constants.push_back(csx2[i]); }
  for(int i=0; i<ncsy2+1; i++){ constants.push_back(csy2[i]); }
  if(netaibl2>0){
    for(int i=0; i<netaibl2+1; i++){ constants.push_back(etaibl2[i]); }
  }
  if(nalphaibl2>0){
    for(int i=0; i<nalphaibl2+1; i++){ constants.push_back(alphaibl2[i]); }
  }
  for(int i=0; i<neta2+1; i++){ constants.push_back(eta2[i]); }
  for(int i=0; i<nalpha2+1; i++){ constants.push_back(alpha2[i]); }
  // parametrization values
  for(int i=0; i<n1; i++){ constants.push_back(m_clustererrordata->getPixelBarrelPhiError(i)); }
  for(int i=0; i<n1; i++){ constants.push_back(m_clustererrordata->getPixelBarrelEtaError(i)); }
  for(int i=0; i<n2; i++){ constants.push_back(m_clustererrordata->getPixelEndcapPhiError(i)); }
  for(int i=0; i<n2; i++){ constants.push_back(m_clustererrordata->getPixelEndcapRError(i)); }
  // IBL
  if(v1<-1){
    for(int i=0; i<n1; i++){ constants.push_back(m_clustererrordata->getPixelIBLPhiError(i)); }
    for(int i=0; i<n1; i++){ constants.push_back(m_clustererrordata->getPixelIBLEtaError(i)); }
  }
  //IBL
  if(n3c>0){ 
    for(int i=0; i<n3c; i++){ constants.push_back(m_clusterontrackerrordata->getPixelIBLPhiError(i)); }
  }
  for(int i=0; i<n3a; i++){ constants.push_back(m_clusterontrackerrordata->getPixelBarrelPhiError(i)); }
  if(n3d>0){
    for(int i=0; i<n3d; i++){ constants.push_back(m_clusterontrackerrordata->getPixelIBLEtaError(i)); }
  }
  for(int i=0; i<n3b; i++){ constants.push_back(m_clusterontrackerrordata->getPixelBarrelEtaError(i)); }
  for(int i=0; i<n6+n4; i++){ constants.push_back(m_chargeinterpolationparameters->getDeltaX(i)); }
  for(int i=0; i<n7+n5; i++){ constants.push_back(m_chargeinterpolationparameters->getDeltaY(i)); }
  return constants;
}

void PixelOfflineCalibData::Dump(){
  m_clustererrordata->Print("PixelClusterDump.txt");
  m_clusterontrackerrordata->Print("PixelClusterOnTrackDump.txt");
  m_chargeinterpolationparameters->Print("PixelChargeInterpolationParametersDump.txt");
}

void PixelOfflineCalibData::SetConstants(const std::vector<float> &constants){ 

  if(constants.at(0) > 0){    // old format
    m_clustererrordata->setVersion(0);
    m_clusterontrackerrordata->setVersion(0);
    m_chargeinterpolationparameters->setVersion(0);

    const int n1 = int(constants.at(0)); // number of PixelClusterError barrel bins (60)
    const int n2 = int(constants.at(1)); // number of PixelClusterError endcap bins (12)
    const int ncsx = int(constants.at(2)); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy = int(constants.at(3)); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta = int(constants.at(4)); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha = int(constants.at(5)); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    const int n4 = int(constants.at(6)); // number of charge interpolation constants (x direction)
    const int n5 = int(constants.at(7)); // number of charge interpolation constants (y direction)
    int ierr = 1; // error code
    int offset = 8;
    m_clusterontrackerrordata->setParameters(ncsx,ncsy,neta,nalpha,offset,constants);
    offset += ncsx+ncsy+neta+nalpha;
    for(int i=0; i<n1; i++){
      ierr *= m_clustererrordata->setPixelBarrelPhiError(i,constants.at(i+offset));
      ierr *= m_clustererrordata->setPixelBarrelEtaError(i,constants.at(i+n1+offset));
    }
    offset += 2*n1;
    for(int i=0; i<n2; i++){
      ierr *= m_clustererrordata->setPixelEndcapPhiError(i,constants.at(offset+i));
      ierr *= m_clustererrordata->setPixelEndcapRError(i,constants.at(offset+n2+i));
    }
    offset += 2*n2;
    int n3a = ncsx*nalpha; // number of barrel phi bins
    int n3b = ncsx*ncsy*neta; // number of barrel eta bins
    for(int i=0; i<n3a; i++){
      m_clusterontrackerrordata->setPixelBarrelPhiError(i,constants.at(offset+i));
    }
    offset += n3a;
    for(int i=0; i<n3b; i++){
      m_clusterontrackerrordata->setPixelBarrelEtaError(i,constants.at(offset+i));
    }
    offset += n3b;
    for(int i=0; i<n4; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaX(i,constants.at(offset+i));
    }
    offset += n4;
    for(int i=0; i<n5; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaY(i,constants.at(offset+i));
    }
    m_chargeinterpolationparameters->setVersion(-1);
    if(ierr == 0) std::cout << "something went wrong!" << std::endl;
    return;

  }
  else{ 

    const int v1 = int(constants.at(0)); // format version number for PixelClusterError
    const int v2 = int(constants.at(1)); // format version number for PixelClusterOnTrackError
    const int v3 = int(constants.at(2)); // format version number for PixelChargeInterpolation
    m_clustererrordata->setVersion(v1);
    m_clusterontrackerrordata->setVersion(v2);
    m_chargeinterpolationparameters->setVersion(v3);

    const int n1 = int(constants.at(3)); // number of PixelClusterError barrel bins (60)
    const int n2 = int(constants.at(4)); // number of PixelClusterError endcap bins (12)
    const int ncsx = int(constants.at(5)); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy = int(constants.at(6)); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta = int(constants.at(7)); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha = int(constants.at(8)); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    const int ncsx2 = int(constants.at(9)); // number of PixelClusterOnTrackError cluster size bins (x direction)
    const int ncsy2 = int(constants.at(10)); // number of PixelClusterOnTrackError cluster size bins (y direction)
    const int neta2 = int(constants.at(11)); // number of PixelClusterOnTrackError eta bins (x direction)
    const int nalpha2 = int(constants.at(12)); // number of PixelClusterOnTrackError incidence angle bins (y direction)
    int ierr = 1; // error code
    int offset = 13;
    if(v1<-1||v2<-1||v3<-1)offset +=8; 
    const int ncsx_ibl = offset>13 ? int(constants.at(13)) : 0;
    const int ncsy_ibl = offset>13 ? int(constants.at(14)) : 0;
    const int netaibl = offset>13 ? int(constants.at(15)) : 0;
    const int nalphaibl = offset>13 ? int(constants.at(16)) : 0;
    const int ncsx2_ibl = offset>13 ? int(constants.at(17)) : 0;
    const int ncsy2_ibl = offset>13 ? int(constants.at(18)) : 0;
    const int netaibl2 = offset>13 ? int(constants.at(19)) : 0;
    const int nalphaibl2 = offset>13 ? int(constants.at(20)) : 0;
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
      ierr *= m_clustererrordata->setPixelBarrelPhiError(i,constants.at(i+offset));
      ierr *= m_clustererrordata->setPixelBarrelEtaError(i,constants.at(i+n1+offset));
    }
    offset += 2*n1;
    for(int i=0; i<n2; i++){
      ierr *= m_clustererrordata->setPixelEndcapPhiError(i,constants.at(offset+i));
      ierr *= m_clustererrordata->setPixelEndcapRError(i,constants.at(offset+n2+i));
    }
    offset += 2*n2;
    if(v1<-1){
      for(int i=0; i<n1; i++){
        ierr *= m_clustererrordata->setPixelIBLPhiError(i,constants.at(i+offset));
        ierr *= m_clustererrordata->setPixelIBLEtaError(i,constants.at(i+n1+offset));
      }
      offset +=2*n1;
    }
    int n3a = ncsx*nalpha; // number of barrel phi bins
    int n3b = ncsx*ncsy*neta; // number of barrel eta bins
    int n3c = ncsx_ibl*nalphaibl;
    int n3d = ncsy_ibl*netaibl;
    if(n3c>0){
      for(int i=0; i<n3c; i++){
        m_clusterontrackerrordata->setPixelIBLPhiError(i,constants.at(offset+i));
      }
      offset += n3c;
    }
    for(int i=0; i<n3a; i++){
      m_clusterontrackerrordata->setPixelBarrelPhiError(i,constants.at(offset+i));
    }
    offset += n3a;

    if(n3d>0){
      for(int i=0; i<n3d; i++){
        m_clusterontrackerrordata->setPixelIBLEtaError(i,constants.at(offset+i));
      }
      offset += n3d;
    }

    for(int i=0; i<n3b; i++){
      m_clusterontrackerrordata->setPixelBarrelEtaError(i,constants.at(offset+i));
    }
    offset += n3b;
    const int n4 = 6*ncsx2*nalpha2; // number of charge interpolation constants (x direction)
    const int n5 = 6*ncsy2*neta2; // number of charge interpolation constants (y direction)
    const int n6 = ncsx2_ibl*nalphaibl2;
    const int n7 = ncsy2_ibl*netaibl2;

    for(int i=0; i<n6+n4; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaX(i,constants.at(offset+i));
    }
    offset += n6+n4;
    for(int i=0; i<n7+n5; i++){
      ierr *= m_chargeinterpolationparameters->setDeltaY(i,constants.at(offset+i));
    }
    offset += n7+n5;

    if(ierr == 0) std::cout << "something went wrong!" << std::endl;
    return;
  }
}

} // end of namespace 
