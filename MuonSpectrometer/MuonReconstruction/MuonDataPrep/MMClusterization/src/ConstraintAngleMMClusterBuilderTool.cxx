/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ConstraintAngleMMClusterBuilderTool.h"

#include <numeric>
#include <cmath>
#include <algorithm>
#include <memory>

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TF1.h"
#include "TGraphErrors.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/IFunction.h"


namespace{
    //class to provide fit function to minuit
    class MyFCN : public ROOT::Math::IBaseFunctionMultiDim {
        public:
            MyFCN(double a = 30*Gaudi::Units::degree, double b=2.5) : m_alpha(a), m_intercept(b) {}

            Double_t DoEval (const Double_t* x) const {
                Double_t l=0;
                //error handling according to  https://root.cern.ch/doc/master/classTGraph.html#a61269bcd47a57296f0f1d57ceff8feeb
                for(uint i_strip=0;i_strip<m_x.size();i_strip++){
                    double inverseNegATan = -1/std::atan(x[0]);
                    double tmp = inverseNegATan * m_x.at(i_strip) + x[1] - m_y[i_strip];
                    tmp *= tmp; // residual squared
                    tmp /= m_errY[i_strip]*m_errY[i_strip] + m_errX[i_strip]*m_errX[i_strip]*inverseNegATan*inverseNegATan;
                    l += tmp;
                }
                l += 0.5*(x[0]-m_alpha0)*(x[0]-m_alpha0) / (m_sigmaAlpha*m_sigmaAlpha);
                return l;
            }
            
            virtual double Up() const {return 0.5;}

            ROOT::Math::IBaseFunctionMultiDim* Clone() const
            {
                return new  MyFCN(m_alpha,m_intercept);
            }
            unsigned int NDim() const {return 2;}
            void setAlpha0(double a0){m_alpha0=a0;}
            void setAlpha0Degree(double a0){m_alpha0=a0*Gaudi::Units::degree;}
            void setSigmaAlphaDegree(double sa){m_sigmaAlpha=sa*Gaudi::Units::degree;}
            void setSigmaAlpha(double sa){m_sigmaAlpha=sa;}

            void addPoint(double x,double y,double sx, double sy){
                m_x.push_back(x);
                m_errX.push_back(sx);
                m_y.push_back(y);
                m_errY.push_back(sy);
            }

        private:
            std::vector<double> m_x,m_y,m_errX,m_errY;
            double m_alpha,m_intercept;
            double m_alpha0=0;
            double m_sigmaAlpha = 90*Gaudi::Units::degree; 
            
    };


}


Muon::ConstraintAngleMMClusterBuilderTool::ConstraintAngleMMClusterBuilderTool(const std::string& t,
                    const std::string& n,const IInterface* p):
                    AthAlgTool(t,n,p)
{
    declareInterface<IMMClusterBuilderTool>(this);
    declareProperty("writeStripProperties", m_writeStripProperties = true ); // true  for debugging; needs to become false for large productions
    declareProperty("nSigmaSelection",m_nSigmaSelection = 3);
    declareProperty("sTheta",m_sigmaTheta = 3);
    declareProperty("fitAngleCut",m_fitAngleCut = 10);  //degree 
}

StatusCode Muon::ConstraintAngleMMClusterBuilderTool::initialize(){
    ATH_CHECK(m_idHelperSvc.retrieve()); 
    return StatusCode::SUCCESS;
}


StatusCode Muon::ConstraintAngleMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds,
                                                     std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec) const
{
    ATH_MSG_DEBUG("Running constraint angle MM cluster builder with: "<< MMprds.size() <<" MMprds");
    std::vector<std::vector<Muon::MMPrepData>> prdsPerLayer(8,std::vector<Muon::MMPrepData>(0));
    StatusCode sc = sortHitsToLayer(MMprds,prdsPerLayer);
    if(sc.isFailure()) return sc;
    for(const auto& prdsOfLayer:prdsPerLayer){
        if(prdsOfLayer.size()<2) continue;
        std::vector<std::vector<uint>> idxClusters; // index of strips in cluster
        std::vector<double> clusterTheta;
        sc=scanLayer(prdsOfLayer,idxClusters,clusterTheta);
        if(sc.isFailure()) continue;
        for(uint i_cluster=0;i_cluster<idxClusters.size();i_cluster++){
            if(idxClusters.at(i_cluster).size()<2) continue;
            sc=fitCluster(prdsOfLayer,idxClusters.at(i_cluster),clusterTheta.at(i_cluster),clustersVec);
            if(sc.isFailure()) continue;
        }

        

    }

    
    return StatusCode::SUCCESS;
}


StatusCode Muon::ConstraintAngleMMClusterBuilderTool::sortHitsToLayer(const std::vector<Muon::MMPrepData>& MMprds, std::vector<std::vector<Muon::MMPrepData>>& prdsPerLayer )
const{
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
    return StatusCode::SUCCESS;
}

StatusCode Muon::ConstraintAngleMMClusterBuilderTool::scanLayer(const std::vector<Muon::MMPrepData> &mmPrdsPerLayer,std::vector<std::vector<uint>> &idxClusters, std::vector<double> &clusterTheta)
const{
    //get gas gap for later correction of the sign of the slope
    int gasGap = m_idHelperSvc->mmIdHelper().gasGap(mmPrdsPerLayer.at(0).identify());

    ATH_MSG_DEBUG("Scanning gas gap "<< gasGap);
    for (const auto &prd:mmPrdsPerLayer){
        ATH_MSG_DEBUG("Hit channel "<< m_idHelperSvc->mmIdHelper().channel(prd.identify())
                       << " local positionX " << prd.localPosition().x()
                       << " time " << prd.time()
                       << " angle to IP" << std::atan(prd.globalPosition().perp()/std::abs(prd.globalPosition().z())) << " "<< std::atan(prd.globalPosition().perp()/std::abs(prd.globalPosition().z()))/Gaudi::Units::degree 
                       << " stationEta " <<m_idHelperSvc->mmIdHelper().stationEta(prd.identify()) 
                       << " stationPhi "<< m_idHelperSvc->mmIdHelper().stationEta(prd.identify())
                       << " multilayer " <<m_idHelperSvc->mmIdHelper().multilayer(prd.identify()));
    }

    //simple grouping of strips using the fact that the strips where ordered by channel   
    idxClusters.push_back(std::vector<uint>{0}); // assumes that mmPrdsPerLayer always has at least one strip
    int nMaxMissingStrips = 3;
    for(uint i_strip=1; i_strip<mmPrdsPerLayer.size();i_strip++){
        if(std::abs(m_idHelperSvc->mmIdHelper().channel(mmPrdsPerLayer.at(i_strip).identify()) -
                m_idHelperSvc->mmIdHelper().channel(mmPrdsPerLayer.at(i_strip-1).identify()))<nMaxMissingStrips){
            idxClusters.back().push_back(i_strip);
        } else{
            idxClusters.push_back(std::vector<uint>{i_strip});
        }
    }
    ATH_MSG_DEBUG("Found "<< idxClusters.size() <<" clusters");
    for(const auto& idxCluster:idxClusters){
        ATH_MSG_DEBUG("cluster: "<< idxCluster);
    }

    //determine mean IP pointing theta and mean position per cluster
    clusterTheta = std::vector<double>(idxClusters.size(),0);
    std::vector<double> clusterMeanX(idxClusters.size(),0);
    for(uint i_cluster=0; i_cluster<idxClusters.size();i_cluster++){
        for(const auto& idx:idxClusters.at(i_cluster)){
            double stripPointingAlpha = std::atan(mmPrdsPerLayer.at(idx).globalPosition().perp()/std::abs(mmPrdsPerLayer.at(idx).globalPosition().z()));
            clusterTheta.at(i_cluster) += stripPointingAlpha;
            clusterMeanX.at(i_cluster) += mmPrdsPerLayer.at(idx).localPosition().x();
        }
        clusterTheta.at(i_cluster) /= idxClusters.at(i_cluster).size();
        clusterMeanX.at(i_cluster) /= idxClusters.at(i_cluster).size();
    }
    for(const auto &theta:clusterTheta){
        ATH_MSG_DEBUG("cluster theta "<< theta << " "<<theta/Gaudi::Units::degree);
    }
        ATH_MSG_DEBUG(" cluster meanX "<< clusterMeanX); 
    
    //scan through clusters with a fixed angle and find optimal 
    double sTheta = m_sigmaTheta * Gaudi::Units::degree; 
    std::unique_ptr<TF1> fLower=std::unique_ptr<TF1>(new TF1("fLower","[0]+[1]*x"));
    std::unique_ptr<TF1> fUpper=std::unique_ptr<TF1>(new TF1("fUpper","[0]+[1]*x"));
    for(uint i_cluster = 0; i_cluster<idxClusters.size(); i_cluster++){
       fLower->SetParameter(1,-1 / std::atan(clusterTheta.at(i_cluster)-sTheta));
       fUpper->SetParameter(1,-1 / std::atan(clusterTheta.at(i_cluster)+sTheta));
       int sign = 1;
       if(gasGap%2 == 0) sign *= -1;
       double idealIntercept = 2.5; // since mean x is substracted, the mean cluster position should be zero at half the drift gap
       double interceptWindow = 2;
       double interceptSteps = 0.1;
       
       std::vector<uint> idxBest;
       for(double probeIntercept = idealIntercept - interceptWindow; probeIntercept <= idealIntercept + interceptWindow; probeIntercept += interceptSteps){
           ATH_MSG_VERBOSE("probeIntercept "<< probeIntercept );
           fLower->SetParameter(0,probeIntercept);
           fUpper->SetParameter(0,probeIntercept);
           std::vector<uint> goodStrips;

           for(const auto &idx:idxClusters.at(i_cluster)){
                double x = sign * (mmPrdsPerLayer.at(idx).localPosition().x() - clusterMeanX.at(i_cluster)); 
                double y = mmPrdsPerLayer.at(idx).driftDist();
                double sy = std::sqrt(mmPrdsPerLayer.at(idx).localCovariance()(1,1));
                ATH_MSG_VERBOSE("selection: x "<< x <<" y "<< y << " fUpper(x) " << fUpper->Eval(x) <<" fLower(x) "<< fLower->Eval(x) << " ylower " << y-m_nSigmaSelection*sy << "yupper" << y+m_nSigmaSelection*sy);
                //select points which are within x sigma of the road
                if(x<=0) ATH_MSG_VERBOSE("x<=0: "<< fUpper->Eval(x)-y<<" "<< y-fLower->Eval(x) << " x*sigma"<< m_nSigmaSelection*sy);
                if(x>0) ATH_MSG_VERBOSE("x>0: "<< y-fUpper->Eval(x) <<" "<< fLower->Eval(x) -y << " x*sigma "<< m_nSigmaSelection*sy);
                if((x<=0 && fUpper->Eval(x)-y<m_nSigmaSelection*sy /*negativ diff means point is above line*/ && y-fLower->Eval(x)<m_nSigmaSelection*sy ) ||
                      (x>0 && y-fUpper->Eval(x)<m_nSigmaSelection*sy && fLower->Eval(x)-y<m_nSigmaSelection*sy)){        
                    ATH_MSG_DEBUG("Selected Point "<<idx);
                    goodStrips.push_back(idx);
                }
           } 
            if(goodStrips.size()>idxBest.size()){
                idxBest = goodStrips;
            
            if(goodStrips.size() == idxClusters.at(i_cluster).size()) break; // stop pattern reco when all strip where selected
           }
       } // end of probe intercept
        idxClusters.at(i_cluster) = idxBest;
        ATH_MSG_DEBUG(" cluster  after filtering "<< idxClusters.at(i_cluster));
    } // end of i_cluster
    for(int i_cluster = idxClusters.size()-1; i_cluster>=0; i_cluster--){
        if(idxClusters.at(i_cluster).size()<2){ // delete to small clusters after pattern reco
            idxClusters.erase(idxClusters.begin()+i_cluster);
        }
    }
    
    return StatusCode::SUCCESS;
} // end of scan layer


StatusCode Muon::ConstraintAngleMMClusterBuilderTool::fitCluster(const std::vector<Muon::MMPrepData> &prdPerLayer, const std::vector<uint>& idxCluster,const double &clusterTheta,std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVec)
const{
    std::vector<std::vector<double>> pts;
    std::unique_ptr<TGraphErrors> fitGraph = std::unique_ptr<TGraphErrors>(new TGraphErrors());
    std::unique_ptr<TF1> ffit = std::unique_ptr<TF1>(new TF1("ffit","-1/atan([0])*x + [1]"));
    ffit->SetParameters(clusterTheta,2.5);
    double meanX = 0;
    for(auto const &idx:idxCluster){
        meanX += prdPerLayer.at(idx).localPosition().x();
    }
    meanX /= idxCluster.size();
    
    MyFCN fitFunc(clusterTheta,2.5);
    fitFunc.setAlpha0(clusterTheta);
    fitFunc.setSigmaAlpha(m_sigmaTheta);

    //defining vectors for final prd
    std::vector<Identifier> stripsOfCluster;
    std::vector<int> stripsOfClusterCharges;
    std::vector<short int> stripsOfClusterTimes;
    std::vector<uint16_t> stripsOfClusterChannels;
    std::vector<float> stripDriftDists;
    std::vector<Amg::MatrixX> stripDriftDistErrors;

    uint nStrips = idxCluster.size();

    stripsOfCluster.reserve(nStrips);

    if (m_writeStripProperties) {
        stripsOfClusterCharges.reserve(nStrips);
        stripsOfClusterTimes.reserve(nStrips);
        stripsOfClusterChannels.reserve(nStrips);
    }
    stripDriftDists.reserve(nStrips);
    stripDriftDistErrors.reserve(nStrips);

    
    for(uint i_strip=0; i_strip < idxCluster.size(); i_strip++){
        double x = prdPerLayer.at(idxCluster.at(i_strip)).localPosition().x()-meanX; 
        double y = prdPerLayer.at(idxCluster.at(i_strip)).driftDist();
        double xerror = std::sqrt(prdPerLayer.at(idxCluster.at(i_strip)).localCovariance()(0,0));
        double yerror = std::sqrt(prdPerLayer.at(idxCluster.at(i_strip)).localCovariance()(1,1));
        fitGraph->SetPoint(i_strip,x,y);
        fitGraph->SetPointError(i_strip, xerror, yerror);
        fitFunc.addPoint(x,y,xerror,yerror);

        stripsOfCluster.push_back(prdPerLayer.at(idxCluster.at(i_strip)).identify());
        if (m_writeStripProperties) {
            stripsOfClusterCharges.push_back(prdPerLayer.at(idxCluster.at(i_strip)).charge());
            stripsOfClusterTimes.push_back(prdPerLayer.at(idxCluster.at(i_strip)).time());
            stripsOfClusterChannels.push_back(m_idHelperSvc->mmIdHelper().channel(prdPerLayer.at(idxCluster.at(i_strip)).identify()));
        }
        stripDriftDists.push_back(prdPerLayer.at(i_strip).driftDist());
        stripDriftDistErrors.push_back(prdPerLayer.at(i_strip).localCovariance());
    }
    if(std::abs(ffit->GetParameter(0)-clusterTheta)> m_fitAngleCut * Gaudi::Units::degree){return StatusCode::FAILURE;}; // very loose cut for now

    fitGraph->Fit("ffit","","",-20,20);




    ROOT::Minuit2::Minuit2Minimizer minuit;
    minuit.SetFunction(fitFunc);
    minuit.SetVariable(0,"alpha",clusterTheta,0.05*Gaudi::Units::degree);
    minuit.SetVariable(1,"intercept",2.5,0.1);
    minuit.Minimize();
    const double* fitResults = minuit.X();
    double clusterPos = (2.5-fitResults[1])*std::atan(fitResults[0])*(-1);
    clusterPos += meanX;
    ATH_MSG_DEBUG("Constraint fit result: alpha: "<< fitResults[0]/Gaudi::Units::degree <<" intercept "<< fitResults[1]);

    float driftDist=0;


    Amg::MatrixX* covN = new Amg::MatrixX(1,1);
    covN->coeffRef(0,0)=0.3*0.3;
    Amg::Vector2D localClusterPositionV(clusterPos,prdPerLayer.at(idxCluster.at(0)).localPosition().y()); // y position is the same for all strips

    std::unique_ptr<MMPrepData> prdN=std::make_unique<MMPrepData>(prdPerLayer.at(idxCluster.at(0)).identify(),
		prdPerLayer.at(idxCluster.at(0)).collectionHash(),
		localClusterPositionV,stripsOfCluster,
		covN,prdPerLayer.at(0).detectorElement(),
		(short int)0,
		std::accumulate(stripsOfClusterCharges.begin(),
		  stripsOfClusterCharges.end(),0),
		driftDist,
		stripsOfClusterChannels,stripsOfClusterTimes,stripsOfClusterCharges);


     prdN->setDriftDist(stripDriftDists, stripDriftDistErrors);
     prdN->setAuthor(Muon::MMPrepData::Author::ConstraintuTPCClusterBuilder);
     ATH_MSG_DEBUG("Did create new prd");

     ATH_MSG_DEBUG("Setting prd angle: "<< fitResults[0] <<" chi2 Prob: "<<0);

     prdN->setMicroTPC(fitResults[0],0);
     ATH_MSG_DEBUG("Reading back prd angle: "<< prdN->angle() <<" chi2 Prob: "<<prdN->chisqProb());
     clustersVec.push_back(std::move(prdN));
     ATH_MSG_DEBUG("pushedBack  prdN");







    return StatusCode::SUCCESS;

}
