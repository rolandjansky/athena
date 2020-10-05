/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ProjectionMMClusterBuilderTool.h"

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <algorithm>

namespace {
  // Parametrization of the strip error after the projection
  constexpr double stripErrorSlope = 0.2;
  constexpr double stripErrorIntercept = 0.15;
}

Muon::ProjectionMMClusterBuilderTool::ProjectionMMClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p) {
  declareInterface<IMMClusterBuilderTool>(this);
  declareProperty("writeStripProperties", m_writeStripProperties = true ); // true  for debugging; needs to become false for large productions
  declareProperty("tmin", m_tmin=0.0);
  declareProperty("tmax", m_tmax=5.0);
  declareProperty("tOffset", m_tOffset=0);
  declareProperty("corr_p0",m_p0=2.48741e-01);
  declareProperty("corr_p1",m_p1=-4.54356e-01);
  declareProperty("corr_p2",m_p2=3.77574e-01);
  declareProperty("t0",m_t0=100);
  declareProperty("minClusterSize",m_minClusterSize=2);
}

StatusCode Muon::ProjectionMMClusterBuilderTool::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode Muon::ProjectionMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, 
							 std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const 
{

std::vector<std::vector<Muon::MMPrepData>> prdsPerLayer(8,std::vector<Muon::MMPrepData>(0));

ATH_MSG_DEBUG("Running ProjectionClusterBuilderTool with: "<< MMprds.size() <<" MMprds");

// sorting hits by gas gap
for(const auto& prd : MMprds){
    Identifier id = prd.identify();
    int layer = 4*(m_idHelperSvc->mmIdHelper().multilayer(id)-1) + (m_idHelperSvc->mmIdHelper().gasGap(id)-1);
    prdsPerLayer.at(layer).push_back(prd);
}

ATH_MSG_DEBUG("sorted hist");
//sort MMPrds by channel
for(unsigned int i_layer=0;i_layer<prdsPerLayer.size();i_layer++){
           std::sort(prdsPerLayer.at(i_layer).begin(),prdsPerLayer.at(i_layer).end(),
                    [this](const MMPrepData &a,const MMPrepData &b){return this->m_idHelperSvc->mmIdHelper().channel(a.identify())<this->m_idHelperSvc->mmIdHelper().channel(b.identify());});
           }

for(const auto& prdsOfLayer : prdsPerLayer){
    ATH_MSG_DEBUG("NPrds: "<<prdsOfLayer.size());
    if(prdsOfLayer.size()<2) continue;
    for(const auto& prd:prdsOfLayer){
      Identifier id_prd = prd.identify();
      //determine angle to IP for debug reasons
      double Tan=prd.globalPosition().perp() / prd.globalPosition().z();
      double angleToIp=std::atan(Tan)/Gaudi::Units::degree;
      ATH_MSG_DEBUG("Hit channel: "<< m_idHelperSvc->mmIdHelper().channel(id_prd) <<" time "<< prd.time() << " drift dist " << prd.driftDist()<< " localPosX "<< prd.localPosition().x() <<" angleToIp " << angleToIp<<" gas_gap "<< m_idHelperSvc->mmIdHelper().gasGap(id_prd) << " multiplet " << m_idHelperSvc->mmIdHelper().multilayer(id_prd) << " stationname " <<m_idHelperSvc->mmIdHelper().stationName(id_prd)  << " stationPhi " <<m_idHelperSvc->mmIdHelper().stationPhi(id_prd) << " stationEta "<<m_idHelperSvc->mmIdHelper().stationEta(id_prd));
    }

    std::vector<double> v_posxc,v_cor;
    StatusCode sc=calculateCorrection(prdsOfLayer,v_posxc,v_cor);
    ATH_MSG_DEBUG("calculated correction");
    ATH_MSG_DEBUG("v_posxc: "<<v_posxc);
    ATH_MSG_DEBUG("v_cor: "<<v_cor);

    if(sc.isFailure()) return sc;
    std::vector<int> flag(prdsOfLayer.size(),0); // flag for already used strips (1) or unused strips(0)

    while(true){
      ATH_MSG_DEBUG("while true");
      std::vector<int> idx_selected; // flag for selected strips
      double xmean,xerr,qtot;
      sc = doFineScan(flag,v_posxc,v_cor,idx_selected);
      
      ATH_MSG_DEBUG("flag"<<flag);
      ATH_MSG_DEBUG("idx_selected"<<idx_selected);
      ATH_MSG_DEBUG("did fine scan and selected strips: "<< idx_selected.size() << " Status code: " << sc);

      if(sc.isFailure()){ATH_MSG_DEBUG("Fine scan failed"); break;} //returns FAILURE if less then three strips in a cluster
      sc = doPositionCalculation(v_posxc,v_cor,idx_selected,xmean,xerr,qtot,prdsOfLayer);
      ATH_MSG_DEBUG("calculated positions "<<sc);
      if(sc.isFailure()) break;
      sc = writeNewPrd(clustersVect,xmean,xerr,qtot,idx_selected,prdsOfLayer);
      ATH_MSG_DEBUG("Size of clusters vec: "<< clustersVect.size());
      if(sc.isFailure()) break;  

      ATH_MSG_DEBUG("Remaining good strips "<< flag.size()-std::accumulate(flag.begin(),flag.end(),0));
      if(flag.size()-std::accumulate(flag.begin(),flag.end(),0)<m_minClusterSize){
        ATH_MSG_DEBUG("Remaining good strips break: "<< flag.size()-std::accumulate(flag.begin(),flag.end(),0));
        break; // break if less then three strips are left for clustering
      }
      ATH_MSG_DEBUG("End of while loop");
    }
    ATH_MSG_DEBUG("After while true");

}
ATH_MSG_DEBUG("Found N prds: "<<clustersVect.size());
return StatusCode::SUCCESS;
}


StatusCode Muon::ProjectionMMClusterBuilderTool::calculateCorrection(
  const std::vector<Muon::MMPrepData> &prdsOfLayer,std::vector<double>& v_posxc,std::vector<double>& v_cor)const {
    for(const auto& prd : prdsOfLayer){
      double posx = prd.localPosition().x();
      double tm = prd.driftDist();
      if(tm<m_tmin) tm=m_tmin;
      if(tm>m_tmax) tm=m_tmax;
      double cor=(tm-m_tmin-m_tOffset)*(1.0*prd.globalPosition().perp()/std::fabs(prd.globalPosition().z()));
      cor*=(m_idHelperSvc->mmIdHelper().gasGap(prd.identify())%2==1? 1:-1);//correct for the local drift time directions. Gas gap 2 and 4 have "negativ" time, therefore correction is applied negative
      
      ATH_MSG_DEBUG("globalR: "<< prd.globalPosition().perp() << " drift dist: " << tm-m_tmin-m_tOffset << "globalZ: " << prd.globalPosition().z() << " R/Z: "<< 1.0*prd.globalPosition().perp()/std::fabs(prd.globalPosition().z()));

      v_cor.push_back(cor); // 
      v_posxc.push_back(posx + cor);
    }
    return StatusCode::SUCCESS;
}

StatusCode Muon::ProjectionMMClusterBuilderTool::doFineScan(std::vector<int>& flag,const std::vector<double>& v_posxc,
  const std::vector<double>& v_cor, std::vector<int>& idx_selected)const {
    double xmin = *std::min_element(v_posxc.begin(),v_posxc.end());
    double xmax = *std::max_element(v_posxc.begin(),v_posxc.end());
    double stepSize = 0.1;
    double nSteps = (xmax-xmin)/stepSize;

    uint maxGoodStrips=0;
    double bestPos=-99999;
    for(int i_step=0;i_step<nSteps;i_step++){
      double testPos=xmin+stepSize*i_step;
      uint nGoodStrips = 0;
      for(size_t i_strip=0; i_strip<v_posxc.size(); i_strip++){
        if(flag.at(i_strip)==1) continue;
        double sigma=getSigma(v_cor.at(i_strip));
        if(std::fabs(testPos-v_posxc.at(i_strip)) < 3*sigma) nGoodStrips++;
      }
      if(nGoodStrips>maxGoodStrips){
        maxGoodStrips=nGoodStrips;
        bestPos=testPos;
        ATH_MSG_DEBUG("new best pos: "<< bestPos<<" nGoodStrips: "<<nGoodStrips);
      }
    }
    ATH_MSG_DEBUG("maxGoodStrips: "<< maxGoodStrips<<" best Pos: "<< bestPos);
    if(maxGoodStrips<m_minClusterSize) return StatusCode::FAILURE;
    for (size_t i_strip=0;i_strip<v_posxc.size();i_strip++){
      if(flag.at(i_strip)==1) continue;
      double sigma=getSigma(v_cor.at(i_strip));
      if(std::fabs(bestPos-v_posxc.at(i_strip)) < 3*sigma){
        idx_selected.push_back(i_strip);
        flag.at(i_strip)=1;
      }
    }
    ATH_MSG_DEBUG("idx_selected.size()"<< idx_selected.size());
    return StatusCode::SUCCESS;   
}

StatusCode  Muon::ProjectionMMClusterBuilderTool::doPositionCalculation(std::vector<double>& v_posxc, 
  const std::vector<double>& v_cor,const std::vector<int>& idx_selected, double& xmean, double& xmeanErr,double& qtot,const std::vector<Muon::MMPrepData>& prdsOfLayer)const {
    //determine cluster charge
    qtot=0;
    for(const auto& idx:idx_selected) qtot+=prdsOfLayer.at(idx).charge();
    
    double numerator=0,denominator=0;
    for(const auto& idx:idx_selected){
      double qrat = prdsOfLayer.at(idx).charge()/qtot;
      v_posxc.at(idx) -= m_p0/(m_p1-qrat) +m_p2;
      numerator+=v_posxc.at(idx)/std::pow(getSigma(v_cor.at(idx)),2);
      denominator+=std::pow(getSigma(v_cor.at(idx)),-2);
    }
    xmean=numerator/denominator;
    xmeanErr=1./std::sqrt(denominator);
    ATH_MSG_DEBUG("do Position Calculation mean: "<< xmean <<" err: "<< xmeanErr);
    return StatusCode::SUCCESS;
  }


StatusCode Muon::ProjectionMMClusterBuilderTool::writeNewPrd(std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect,double xmean, double xerr,double qtot,const std::vector<int>& idx_selected,const std::vector<MMPrepData>& prdsOfLayer)const {
      Amg::MatrixX* covN = new Amg::MatrixX(1,1);
      covN->coeffRef(0,0)=xerr; // TODO set proper uncertainty
      ATH_MSG_VERBOSE("Did set covN Matrix");

      int idx = idx_selected.at(idx_selected.size()/2);
      ATH_MSG_VERBOSE("Idx: "<<idx);
      Amg::Vector2D localClusterPositionV(xmean,prdsOfLayer.at(idx).localPosition().y()); // y position is the same for all strips

      std::vector<Identifier> stripsOfCluster;
      std::vector<short int> stripsOfClusterDriftTime;
      std::vector<int> stripsOfClusterCharge;
      std::vector<uint16_t> stripsOfClusterStripNumber;
      std::vector<float> stripsOfClusterDriftDists;
      std::vector<Amg::MatrixX> stripsOfClusterDriftDistErrors;



      stripsOfCluster.reserve(idx_selected.size());
      if (m_writeStripProperties) {
        stripsOfClusterDriftTime.reserve(idx_selected.size());
        stripsOfClusterCharge.reserve(idx_selected.size());
        stripsOfClusterStripNumber.reserve(idx_selected.size());
      }
      stripsOfClusterDriftDists.reserve(idx_selected.size());
      stripsOfClusterDriftDistErrors.reserve(idx_selected.size());

      double meanTime=0;
      for(const auto& id_goodStrip:idx_selected){
        stripsOfCluster.push_back(prdsOfLayer.at(id_goodStrip).identify());
        if (m_writeStripProperties) {
          stripsOfClusterDriftTime.push_back(static_cast<short int>(prdsOfLayer.at(id_goodStrip).time()));
          stripsOfClusterCharge.push_back(static_cast<int>(prdsOfLayer.at(id_goodStrip).charge()));
          stripsOfClusterStripNumber.push_back(m_idHelperSvc->mmIdHelper().channel(prdsOfLayer.at(id_goodStrip).identify()));
        }
        stripsOfClusterDriftDists.push_back(prdsOfLayer.at(id_goodStrip).driftDist());
        stripsOfClusterDriftDistErrors.push_back(prdsOfLayer.at(id_goodStrip).localCovariance());


        meanTime+=prdsOfLayer.at(id_goodStrip).time()*prdsOfLayer.at(id_goodStrip).charge();
      }
      meanTime/=qtot;


      float driftDist = 0.0;

      std::unique_ptr<Muon::MMPrepData> prdN =  std::make_unique<MMPrepData>(prdsOfLayer.at(idx).identify(),prdsOfLayer.at(idx).collectionHash(),
				      localClusterPositionV,stripsOfCluster,
				      covN,prdsOfLayer.at(idx).detectorElement(),
				      (short int) int(meanTime),int(qtot), driftDist,
				      stripsOfClusterStripNumber,stripsOfClusterDriftTime,stripsOfClusterCharge);

      prdN->setAuthor(Muon::MMPrepData::Author::ProjectionClusterBuilder);

      clustersVect.push_back(std::move(prdN));
      ATH_MSG_VERBOSE("pushedBack  prdN");
      ATH_MSG_VERBOSE("pushedBack PRDs: stationEta: "<< m_idHelperSvc->mmIdHelper().stationEta(prdN->identify())
                       <<" stationPhi "<< m_idHelperSvc->mmIdHelper().stationPhi(prdN->identify()) 
                       <<" stationName "<< m_idHelperSvc->mmIdHelper().stationName(prdN->identify())
                       << " gasGap "<< m_idHelperSvc->mmIdHelper().gasGap(prdN->identify()) 
                       <<" multiplet "<<m_idHelperSvc->mmIdHelper().multilayer(prdN->identify()) 
                       <<" channel "<< m_idHelperSvc->mmIdHelper().channel(prdN->identify()));
      return StatusCode::SUCCESS;
} 


double Muon::ProjectionMMClusterBuilderTool::getSigma(double correction)const {
  return stripErrorIntercept+stripErrorSlope*std::fabs(correction);
}
