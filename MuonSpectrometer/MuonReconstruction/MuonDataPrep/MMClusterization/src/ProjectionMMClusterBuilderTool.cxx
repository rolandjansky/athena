/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "ProjectionMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "GaudiKernel/MsgStream.h" 

namespace{
template <class T>
std::string printVec(std::vector<T> vec){
    std::ostringstream outstream;
    std::copy(vec.begin(),vec.end(),std::ostream_iterator<T>(outstream,", "));
    return outstream.str();
}


}





Muon::ProjectionMMClusterBuilderTool::ProjectionMMClusterBuilderTool(const std::string& t,
							     const std::string& n,
							     const IInterface*  p )
  :  
  AthAlgTool(t,n,p),
  m_muonMgr(nullptr),
  m_mmIdHelper(nullptr)
{
  declareInterface<IMMClusterBuilderTool>(this);
  declareProperty("vDrift",m_vDrift = 0.048);
  declareProperty("tmin", m_tmin=30);
  declareProperty("tmax", m_tmax=130);
  declareProperty("corr_p0",m_p0=2.48741e-01);
  declareProperty("corr_p1",m_p1=-4.54356e-01);
  declareProperty("corr_p2",m_p2=3.77574e-01);
}



StatusCode Muon::ProjectionMMClusterBuilderTool::initialize()
{
  StatusCode sc = detStore()->retrieve(m_muonMgr);
  if (sc.isFailure()){
    ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
    return sc;
  }
  m_mmIdHelper = m_muonMgr->mmIdHelper();
  ATH_MSG_INFO("Set vDrift to "<< m_vDrift <<" um/ns");
  return StatusCode::SUCCESS;
}


StatusCode Muon::ProjectionMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, 
							 std::vector<Muon::MMPrepData*>& clustersVect)
{

std::vector<std::vector<Muon::MMPrepData>> prdsPerLayer(8,std::vector<Muon::MMPrepData>(0));

ATH_MSG_DEBUG("Running ProjectionClusterBuilderTool with: "<< MMprds.size() <<" MMprds");

// sorting hits by gas gap
for(const auto& prd : MMprds){
    Identifier id = prd.identify();
    int layer = 4*(m_mmIdHelper->multilayer(id)-1) + (m_mmIdHelper->gasGap(id)-1);
    prdsPerLayer.at(layer).push_back(prd);
}

ATH_MSG_DEBUG("sorted hist");
//sort MMPrds by channel
for(unsigned int i_layer=0;i_layer<prdsPerLayer.size();i_layer++){
           std::sort(prdsPerLayer.at(i_layer).begin(),prdsPerLayer.at(i_layer).end(),
                    [this](const MMPrepData &a,const MMPrepData &b){return this->m_mmIdHelper->channel(a.identify())<this->m_mmIdHelper->channel(b.identify());});
           }

for(const auto& prdsOfLayer : prdsPerLayer){
    ATH_MSG_DEBUG("NPrds: "<<prdsOfLayer.size());
    if(prdsOfLayer.size()<2) continue;
    for(const auto& prd:prdsOfLayer){
      Identifier id_prd = prd.identify();
      //determine time of flight from strip position
      double distToIp=prd.globalPosition().norm();
      double tof=distToIp/299.792; // divide by speed of light in mm/ns
      //determine angle to IP for debug reasons
      double globalR=TMath::Sqrt(pow(prd.globalPosition().x(),2)+pow(prd.globalPosition().y(),2));
      double Tan=globalR/prd.globalPosition().z();
      double angleToIp=TMath::ATan(Tan)/(180./M_PI);
      ATH_MSG_DEBUG("Hit channel: "<< m_mmIdHelper->channel(id_prd) <<" time "<< prd.time() << " localPosX "<< prd.localPosition().x() << " tof "<<tof <<" angleToIp " << angleToIp<<" gas_gap "<< m_mmIdHelper->gasGap(id_prd) << " multiplet " << m_mmIdHelper->multilayer(id_prd) << " stationname " <<m_mmIdHelper->stationName(id_prd)  << " stationPhi " <<m_mmIdHelper->stationPhi(id_prd) << " stationEta "<<m_mmIdHelper->stationEta(id_prd));
    }

    //vector<double> xpos,time,corr,newXpos,charge,globalR;
    std::vector<double> v_posxc,v_cor;
    StatusCode sc=calculateCorrection(prdsOfLayer,v_posxc,v_cor);
    ATH_MSG_DEBUG("calculated correction");
    ATH_MSG_DEBUG("v_posxc: "<<printVec(v_posxc));
    ATH_MSG_DEBUG("v_cor: "<<printVec(v_cor));

    if(sc.isFailure()) return sc;
    std::vector<int> flag(prdsOfLayer.size(),0); // flag for already used strips (1) or unused strips(0)

    while(true){
      ATH_MSG_DEBUG("while true");
      std::vector<int> idx_selected; // flag for selected strips
      double xmean,xerr,qtot;
      sc = doFineScan(flag,v_posxc,v_cor,idx_selected);
      
      ATH_MSG_DEBUG("flag"<<printVec(flag));
      ATH_MSG_DEBUG("idx_selected"<<printVec(idx_selected));
      ATH_MSG_DEBUG("did fine scan and selected strips: "<< idx_selected.size() << " Status code: " << sc);

      if(sc.isFailure()) break; //returns FAILURE if less then three strips in a cluster
      sc = doPositionCalculation(v_posxc,v_cor,idx_selected,xmean,xerr,qtot,prdsOfLayer);
      ATH_MSG_DEBUG("calculated positions "<<sc);
      if(sc.isFailure()) break;
      sc = writeNewPrd(clustersVect,xmean,xerr,qtot,idx_selected,prdsOfLayer);
      ATH_MSG_DEBUG("Size of clusters vec: "<< clustersVect.size());
      if(sc.isFailure()) break;  

      ATH_MSG_DEBUG("Remaining good strips "<< flag.size()-std::accumulate(flag.begin(),flag.end(),0));
      if(flag.size()-std::accumulate(flag.begin(),flag.end(),0)<3){
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
  const std::vector<Muon::MMPrepData> &prdsOfLayer,std::vector<double>& v_posxc,std::vector<double>& v_cor){
    for(const auto& prd : prdsOfLayer){
      double posx = prd.localPosition().x();
      double posz = prd.globalPosition().z();
      double tm = prd.time()-prd.globalPosition().norm()/299.792;
      if(tm<m_tmin) tm=m_tmin;
      if(tm>m_tmax) tm=m_tmax;
      double globalR=TMath::Sqrt(pow(prd.globalPosition().x(),2) + pow(prd.globalPosition().y(),2));
      double cor=m_vDrift*(tm-m_tmin-10)*(1.0*globalR/posz);
      ATH_MSG_DEBUG("globalR: "<< globalR << " Time: " << tm-m_tmin-10 << "globalZ: " << posz << " R/Z: "<<1.0*globalR/posz);
      v_cor.push_back(cor);
      v_posxc.push_back(posx + cor);
    }
    return StatusCode::SUCCESS;
}

StatusCode Muon::ProjectionMMClusterBuilderTool::doFineScan(std::vector<int>& flag,const std::vector<double>& v_posxc,
  const std::vector<double>& v_cor, std::vector<int>& idx_selected){
    double xmin = *std::min_element(v_posxc.begin(),v_posxc.end());
    double xmax = *std::max_element(v_posxc.begin(),v_posxc.end());
    double stepSize = 0.1;
    double nSteps = (xmax-xmin)/stepSize;

    int maxGoodStrips=0;
    double bestPos=-99999;
    for(int i_step=0;i_step<nSteps;i_step++){
      double testPos=xmin+stepSize*i_step;
      int nGoodStrips = 0;
      for(size_t i_strip=0; i_strip<v_posxc.size(); i_strip++){
        if(flag.at(i_strip)==1) continue;
        double sigma=getSigma(v_cor.at(i_strip));
        if(fabs(testPos-v_posxc.at(i_strip)) < 3*sigma) nGoodStrips++;
      }
      if(nGoodStrips>maxGoodStrips){
        maxGoodStrips=nGoodStrips;
        bestPos=testPos;
        ATH_MSG_DEBUG("new best pos: "<< bestPos<<" nGoodStrips: "<<nGoodStrips);
      }
    }
    ATH_MSG_DEBUG("maxGoodStrips: "<< maxGoodStrips<<" best Pos: "<< bestPos);
    if(maxGoodStrips<3) return StatusCode::FAILURE;
    for (size_t i_strip=0;i_strip<v_posxc.size();i_strip++){
      if(flag.at(i_strip)==1) continue;
      double sigma=getSigma(v_cor.at(i_strip));
      if(fabs(bestPos-v_posxc.at(i_strip)) < 3*sigma){
        idx_selected.push_back(i_strip);
        flag.at(i_strip)=1;
      }
    }
    ATH_MSG_DEBUG("idx_selected.size()"<< idx_selected.size());
    return StatusCode::SUCCESS;   
}

StatusCode  Muon::ProjectionMMClusterBuilderTool::doPositionCalculation(std::vector<double>& v_posxc, 
  const std::vector<double>& v_cor,const std::vector<int> idx_selected, double& xmean, double& xmeanErr,double& qtot,const std::vector<Muon::MMPrepData>& prdsOfLayer){
    //determine cluster charge
    qtot=0;
    for(const auto& idx:idx_selected) qtot+=prdsOfLayer.at(idx).charge();
    
    double numerator=0,denominator=0;
    for(const auto& idx:idx_selected){
      double qrat = prdsOfLayer.at(idx).charge()/qtot;
      v_posxc.at(idx) -= m_p0/(m_p1-qrat) +m_p2;
      numerator+=v_posxc.at(idx)/pow(getSigma(v_cor.at(idx)),2);
      denominator+=pow(getSigma(v_cor.at(idx)),-2);
    }
    xmean=numerator/denominator;
    xmeanErr=TMath::Sqrt(denominator);
    return StatusCode::SUCCESS;
  }


StatusCode Muon::ProjectionMMClusterBuilderTool::writeNewPrd(std::vector<Muon::MMPrepData*>& clustersVect,double xmean, double xerr,double qtot,const std::vector<int>& idx_selected,const std::vector<Muon::MMPrepData>& prdsOfLayer){
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
      double meanTime=0;
      for(const auto& id_goodStrip:idx_selected){
        stripsOfCluster.push_back(prdsOfLayer.at(id_goodStrip).identify());
        stripsOfClusterDriftTime.push_back(int(prdsOfLayer.at(id_goodStrip).time()-prdsOfLayer.at(id_goodStrip).globalPosition().norm()/299.792));
        stripsOfClusterCharge.push_back(int(prdsOfLayer.at(id_goodStrip).charge()));
        stripsOfClusterStripNumber.push_back(m_mmIdHelper->channel(prdsOfLayer.at(id_goodStrip).identify()));


        meanTime+=prdsOfLayer.at(id_goodStrip).time()*prdsOfLayer.at(id_goodStrip).charge();
      }
      meanTime/=qtot;


      float driftDist = 0.0;

      MMPrepData* prdN=new MMPrepData(prdsOfLayer.at(idx).identify(),prdsOfLayer.at(idx).collectionHash(),
				      localClusterPositionV,stripsOfCluster,
				      covN,prdsOfLayer.at(idx).detectorElement(),
				      (short int) int(meanTime),int(qtot), driftDist,
				      stripsOfClusterStripNumber,stripsOfClusterDriftTime,stripsOfClusterCharge);

      clustersVect.push_back(prdN);
      ATH_MSG_VERBOSE("pushedBack  prdN");
      return StatusCode::SUCCESS;
} 


double Muon::ProjectionMMClusterBuilderTool::getSigma(double correction){
  return 0.15+0.2*fabs(correction);
}
