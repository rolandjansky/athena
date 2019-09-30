/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "UTPCMMClusterBuilderTool.h"
#include "GaudiKernel/MsgStream.h"

namespace{

double median(std::vector<double> vec){
    std::sort(vec.begin(),vec.end());
    int N=vec.size();
    if (N%2 == 0){return (vec[N/2-1]+vec[N/2])/2;}
    else{return vec[N/2];}
}
template <class T>
std::string printVec(std::vector<T> vec){
    std::ostringstream outstream;
    std::copy(vec.begin(),vec.end(),std::ostream_iterator<T>(outstream,", "));
    return outstream.str();
}


bool sortTracks(std::tuple<int,double> &a, std::tuple<int,double> &b){
   if(std::get<0>(a)==std::get<0>(b)){
       if(std::get<1>(a)<std::get<1>(b)){
           return true;
       }
   }
   else if(std::get<0>(a)>std::get<0>(b)){
       return true;
   }
    return false;
}
}


Muon::UTPCMMClusterBuilderTool::UTPCMMClusterBuilderTool(const std::string& t,
                       const std::string& n,
                       const IInterface* p)
    :
    AthAlgTool(t,n,p)
{
    declareInterface<IMMClusterBuilderTool>(this);
    declareProperty("HoughAlphaMin",m_alphaMin=-90); //
    declareProperty("HoughAlphaMax",m_alphaMax=90);
    declareProperty("HoughAlphaResolution",m_alphaResolution=1.0);
    declareProperty("HoughDMax",m_dMax=0);
    declareProperty("HoughDMin",m_dMin=0);
    declareProperty("HoughDResolution",m_dResolution=0.125); 
    declareProperty("HoughMinCounts",m_houghMinCounts=3);
    declareProperty("uTPCTimeOffset",m_timeOffset=0);
    declareProperty("uTPCDHalf",m_dHalf=2.5);
    declareProperty("vDrift",m_vDrift=0.048);

}


StatusCode Muon::UTPCMMClusterBuilderTool::initialize(){
    ATH_MSG_INFO("Set vDrift to "<< m_vDrift <<" um/ns");
    ATH_CHECK( m_muonIdHelperTool.retrieve() );  
    return StatusCode::SUCCESS;
}


StatusCode Muon::UTPCMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, std::vector<Muon::MMPrepData*>& clustersVect){
    std::vector<std::vector<Muon::MMPrepData>> MMprdsPerLayer(8,std::vector<Muon::MMPrepData>(0));
    for (const auto& MMprd:MMprds){
        Identifier id = MMprd.identify();
        int layer = 4*(m_muonIdHelperTool->mmIdHelper().multilayer(id)-1)+(m_muonIdHelperTool->mmIdHelper().gasGap(id)-1);
        ATH_MSG_DEBUG("Sorting PRD into layer layer: "<< layer<< " gas_gap "<<m_muonIdHelperTool->mmIdHelper().gasGap(id) <<"multilayer "<<m_muonIdHelperTool->mmIdHelper().multilayer(id));        
        MMprdsPerLayer.at(layer).push_back(MMprd);
    }


    for(const auto& MMprdsOfLayer:MMprdsPerLayer){
        std::vector<float> stripsTime,stripsLocalPosX;
        std::vector<Identifier> strips_id;
        std::vector<int> flag=std::vector<int>(MMprdsOfLayer.size(),0); // vector of 0s and ones to indicate if strip was already used
        for( auto MMprd: MMprdsOfLayer){
            Identifier id_prd=MMprd.identify();
            strips_id.push_back(id_prd);
            stripsLocalPosX.push_back(MMprd.localPosition().x());

            //determine time of flight from strip position
            double distToIp=MMprd.globalPosition().norm();
            double tof=distToIp/299.792; // divide by speed of light in mm/ns
            //determine angle to IP for debug reasons
            double globalR=TMath::Sqrt(pow(MMprd.globalPosition().x(),2)+pow(MMprd.globalPosition().y(),2));
            double Tan=globalR/MMprd.globalPosition().z();
            double angleToIp=TMath::ATan(Tan)/m_toRad;

            stripsTime.push_back(MMprd.time()-tof); // use time minus tof as actual time
            ATH_MSG_DEBUG("Hit channel: "<< m_muonIdHelperTool->mmIdHelper().channel(id_prd) <<" time "<< MMprd.time() << " localPosX "<< MMprd.localPosition().x() << " tof "<<tof <<" angleToIp " << angleToIp<<" gas_gap "<< m_muonIdHelperTool->mmIdHelper().gasGap(id_prd) << " multiplet " << m_muonIdHelperTool->mmIdHelper().multilayer(id_prd) << " stationname " <<m_muonIdHelperTool->mmIdHelper().stationName(id_prd)  << " stationPhi " <<m_muonIdHelperTool->mmIdHelper().stationPhi(id_prd) << " stationEta "<<m_muonIdHelperTool->mmIdHelper().stationEta(id_prd));
        }
        if(MMprdsOfLayer.size()<3) continue;

        while(true){
            ATH_MSG_DEBUG("while true"); 
            std::vector<int> idx_goodStrips;
            StatusCode sc=runHoughTrafo(flag,stripsLocalPosX,stripsTime,idx_goodStrips);

            ATH_MSG_DEBUG("Hough done"); 
            if(sc.isFailure()) break;
            if(idx_goodStrips.size()<3) break; // should be already catched in runHough but for safety once more here
            float localClusterPosition=-9999;
            float finalFitAngle,finalFitChiSqProb;
            sc=finalFit(stripsLocalPosX,stripsTime,idx_goodStrips,localClusterPosition,finalFitAngle,finalFitChiSqProb);
            ATH_MSG_DEBUG("final fit done"); 
            
            if(sc.isFailure()) break;
            ATH_MSG_DEBUG("Did final fit successfull");
            std::vector<Identifier> stripsOfCluster;
            //int firstIdx=-1;
            ATH_MSG_DEBUG("Found good Strips: "<< idx_goodStrips.size());


            for(int idx:idx_goodStrips){
                ATH_MSG_DEBUG("Setting good strip: "<<idx<< " size of strips flag: "<<flag.size());
                flag.at(idx)=1;
                ATH_MSG_DEBUG("Set Good strips");
                stripsOfCluster.push_back(strips_id.at(idx));
            }
            Amg::MatrixX* covN = new Amg::MatrixX(1,1);
            covN->setIdentity();
            ATH_MSG_DEBUG("Did set covN Matrix");
            int idx = idx_goodStrips[0];
            ATH_MSG_DEBUG("idx_goodStrips[0]: "<<idx<<"MMPRDs size: "<< MMprdsOfLayer.at(idx) << " size: " <<MMprdsOfLayer.size());
            Amg::Vector2D localClusterPositionV(localClusterPosition,MMprdsOfLayer.at(idx).localPosition().y()); // y position is the same for all strips
            ATH_MSG_DEBUG("Did set local position");
            MMPrepData* prdN=new MMPrepData(MMprdsOfLayer.at(idx).identify(),MMprdsOfLayer.at(idx).collectionHash(),
                                 localClusterPositionV,stripsOfCluster,covN,MMprdsOfLayer.at(idx).detectorElement());
            ATH_MSG_DEBUG("Did create new prd");
            
            ATH_MSG_DEBUG("Setting prd angle: "<< finalFitAngle <<" chi2 Prob: "<<finalFitChiSqProb);

            prdN->setMicroTPC(finalFitAngle,finalFitChiSqProb);
            ATH_MSG_DEBUG("Reading back prd angle: "<< prdN->angle() <<" chi2 Prob: "<<prdN->chisqProb());
            clustersVect.push_back(prdN);
            ATH_MSG_DEBUG("pushedBack  prdN");
            int leftOverStrips=0;
            for(auto f:flag){if(f==0) leftOverStrips++;}
            if(leftOverStrips<3) break;
        }
            ATH_MSG_DEBUG("End of get clusters layer"); 

    }
    ATH_MSG_DEBUG("End of get clusters"); 
    return StatusCode::SUCCESS;
}


StatusCode Muon::UTPCMMClusterBuilderTool::runHoughTrafo(std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time,std::vector<int>& idx_selected){
    ATH_MSG_DEBUG("beginning of runHoughTrafo() with: " << xpos.size() <<" hits" );
    float meanX = std::accumulate(xpos.begin(),xpos.end(),0.0)/xpos.size();
    ATH_MSG_DEBUG("Got mean Element"<< meanX);
    ATH_MSG_DEBUG("Max Element at: " << *std::max_element(xpos.begin(),xpos.end()));
    float maxX =  *std::max_element(xpos.begin(),xpos.end());
    float minX =  *std::min_element(xpos.begin(),xpos.end());    
    std::unique_ptr<TH2D> h_hough=0;
    StatusCode sc = houghInitCummulator(h_hough,maxX,minX,meanX);

    if(sc.isFailure()) return sc;
    sc = fillHoughTrafo(h_hough,flag,xpos,time,meanX);
    ATH_MSG_DEBUG("filled Hough");
    if(sc.isFailure()) return sc;

    std::vector<std::tuple<double,double>> houghPeaks;
    sc = findAlphaMax(h_hough,houghPeaks);
    if(sc.isFailure()) return sc;
    ATH_MSG_DEBUG("Found HoughPeaks");
    
    std::tuple<double,double,double,double> track;
    sc = selectTrack(houghPeaks,xpos,time,meanX,flag,idx_selected);
    ATH_MSG_DEBUG("Selected track");
    if(sc.isFailure()) return sc;
    return StatusCode::SUCCESS;   
}

StatusCode Muon::UTPCMMClusterBuilderTool::houghInitCummulator(std::unique_ptr<TH2D>& h_hough,float xmax,float xmin,float xmean){
    ATH_MSG_VERBOSE("xmax: "<< xmax <<" xmin: "<< xmin <<" xmean "<< xmean <<" m_dResolution "<< m_dResolution <<" m_alphaMin "<< m_alphaMin <<" m_alphaMax: "<< m_alphaMax <<" m_toRad: "<< m_toRad <<" m_alphaResolution: "<<m_alphaResolution);

    float dmax=std::max(fabs(xmin-xmean),fabs(xmax-xmean));
    dmax=TMath::Sqrt(pow(dmax,2)+pow(6,2)); // rspace =sqrt(xmax*xmax+driftrange*driftrange) where driftrange is assumed to be 6mm
    int nbinsd = static_cast<int>(1.0*dmax*2/m_dResolution);
    int nbinsa = static_cast<int>((1.0*m_alphaMax-m_alphaMin)/m_alphaResolution);

    ATH_MSG_VERBOSE("Hough Using nBinsA "<< nbinsa <<" nBinsd "<< nbinsd<<" dmax "<< dmax);

    h_hough = std::unique_ptr<TH2D>(new TH2D("h_hough","h_hough",nbinsa,m_alphaMin*m_toRad,m_alphaMax*m_toRad,nbinsd,-dmax,dmax));

    return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::fillHoughTrafo(std::unique_ptr<TH2D>& h_hough,std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time, float meanX){
   for(size_t i_hit=0; i_hit<xpos.size(); i_hit++){
    if(flag.at(i_hit)==1) continue; //skip hits which have been already used
    float x=xpos.at(i_hit)-meanX;
    float y=time.at(i_hit)*m_vDrift;
    for(int i_alpha=1;i_alpha<h_hough->GetNbinsX();i_alpha++){
        float alpha=h_hough->GetXaxis()->GetBinCenter(i_alpha);
        float d= x*TMath::Cos(alpha) - y*TMath::Sin(alpha);
        ATH_MSG_VERBOSE("Fill Hough alpha: "<< alpha <<" d "<< d  << " x "<< x << " y "<< y);
        h_hough->Fill(alpha,d);
    }
   }
   return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::findAlphaMax(std::unique_ptr<TH2D>& h_hough, std::vector<std::tuple<double,double>> &maxPos){
    int cmax =  h_hough->GetMaximum();
    if (cmax<m_houghMinCounts){
        ATH_MSG_DEBUG("cmax "<< cmax<< "smaller then m_houghMinCounts "<<m_houghMinCounts);
        return StatusCode::FAILURE;
    }
     
    for(int i_binX=0; i_binX<=h_hough->GetNbinsX();i_binX++){
        for(int i_binY=0; i_binY<=h_hough->GetNbinsY();i_binY++){
            if(h_hough->GetBinContent(i_binX,i_binY)==cmax){
            ATH_MSG_DEBUG("Find Max Alpha: BinX "<< i_binX << " Alpha: "<< h_hough->GetXaxis()->GetBinCenter(i_binX)/m_toRad <<" BinY: "<< i_binY <<" over threshold: "<< h_hough->GetBinContent(i_binX,i_binY));
            maxPos.push_back(std::make_tuple(h_hough->GetXaxis()->GetBinCenter(i_binX),h_hough->GetYaxis()->GetBinCenter(i_binY)));
            }
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::selectTrack(std::vector<std::tuple<double,double>> &tracks,std::vector<float>& xpos, std::vector<float>& time,
                                float meanX,std::vector<int>& flag, std::vector<int> &idxGoodStrips){
    std::vector<double> chi2;
    std::vector<std::vector<int>> allGoodStrips;
    std::vector<int> ngoodStrips;
    std::vector<std::tuple<int,double>> trackQuality; // touble holding the number of good strips and the chi2 for each track
    for (auto track: tracks){
        double houghAngle=std::get<0>(track);
        double houghD=std::get<1>(track);
        double slope,intercept;
        double interceptRMS;
        double aRMS=0;
        StatusCode sc = transformParameters(houghAngle,houghD,aRMS,slope,intercept,interceptRMS);
        if(sc.isFailure()){return sc;}
        std::vector<double> dist;
        std::vector<int> indexUsedForDist,goodStrips;
        for(size_t i_hit=0;i_hit<xpos.size();i_hit++){
            if (flag.at(i_hit) != 1){
                dist.push_back(time.at(i_hit)*m_vDrift-slope*(xpos.at(i_hit)-meanX)-intercept);
                indexUsedForDist.push_back(i_hit);
            }
        }
        double med=median(dist);
        ATH_MSG_DEBUG("indexUsedForDist "<< printVec(indexUsedForDist));
        ATH_MSG_DEBUG("distVec: "<<printVec(dist)<<" median: "<< med);
        intercept-=med;
        dist.clear();
        for(size_t i_hit: indexUsedForDist){
            double d=time.at(i_hit)*m_vDrift-slope*(xpos.at(i_hit)-meanX)-intercept;
            ATH_MSG_DEBUG("dist: "<<d<<" for index "<<i_hit);
            if (fabs(d)<0.7){ 
                dist.push_back(pow(d/m_vDrift/10,2)); //determine dummy chi2
                goodStrips.push_back(i_hit);
            }
        }
        if(true){
            ATH_MSG_DEBUG("Angle estimate         ="<< std::get<0>(track)<<" "<<std::get<0>(track)/3.1415*180.);
            ATH_MSG_DEBUG("restimate              ="<< std::get<1>(track));
            ATH_MSG_DEBUG("slope estimate         ="<< slope);
            ATH_MSG_DEBUG("intercept estimate     ="<< intercept);
            ATH_MSG_DEBUG("good strips            ="<< printVec(goodStrips));
            ATH_MSG_DEBUG("n good points          ="<< goodStrips.size());
            ATH_MSG_DEBUG("chi2                   ="<< std::accumulate(dist.begin(),dist.end(),0.0)/(dist.size()-2));
        }
        if (goodStrips.size()){
            allGoodStrips.push_back(goodStrips);
            chi2.push_back(std::accumulate(dist.begin(),dist.end(),0.0)/(dist.size()-2));
            trackQuality.push_back(std::make_tuple(goodStrips.size(),std::accumulate(dist.begin(),dist.end(),0.0)/(dist.size()-2)));
        }
    }
     
    if(chi2.size()==0) return StatusCode::FAILURE;
    int trackIndex=std::min_element(trackQuality.begin(),trackQuality.end(),sortTracks)-trackQuality.begin();
    idxGoodStrips=allGoodStrips.at(trackIndex);
    return StatusCode::SUCCESS;
}







StatusCode Muon::UTPCMMClusterBuilderTool::transformParameters(double alpha, double d, double dRMS, double& slope,
    double& intercept, double& interceptRMS){
        slope=1./TMath::Tan(alpha);
        intercept=-d/TMath::Sin(alpha);
        interceptRMS=fabs(dRMS);
        ATH_MSG_VERBOSE("transformParameters alpha: "<<alpha<<" d: "<< d << " dRMS: " << dRMS << " slope: " << slope << " intercept: " << intercept);
        return StatusCode::SUCCESS;
}


StatusCode Muon::UTPCMMClusterBuilderTool::finalFit(std::vector<float>& xpos, std::vector<float>& time, std::vector<int>& idxSelected, float& x0,float &fitAngle, float &chiSqProb){
    std::unique_ptr<TGraphErrors> fitGraph=std::unique_ptr<TGraphErrors>(new TGraphErrors());
    std::unique_ptr<TF1> ffit=std::unique_ptr<TF1>(new TF1("ffit","pol1"));
    
    for(int idx:idxSelected){
        fitGraph->SetPoint(fitGraph->GetN(),xpos.at(idx),time.at(idx)*m_vDrift);
        fitGraph->SetPointError(fitGraph->GetN()-1,0.425/3.464101615 /*sqrt(12)*/,0.8*m_vDrift);
    }
    ffit->SetParameter(1,-1.7);
    int s=fitGraph->Fit("ffit","Q");
    if(s!=0) s=fitGraph->Fit("ffit","Q"); //redo fit if first one failed
    if(s!=0){
        ATH_MSG_DEBUG("Fit failed twice");
        return StatusCode::FAILURE;
    }

    x0=(m_dHalf-ffit->GetParameter(0))/ffit->GetParameter(1);
    fitAngle=TMath::ATan(-1/ffit->GetParameter(1));
    chiSqProb=ffit->GetProb();

    if(true){
        ATH_MSG_DEBUG("==========uTPC fit Summary==============");
        ATH_MSG_DEBUG("Fit slope: " << ffit->GetParameter(1));
        ATH_MSG_DEBUG("Fit intercept:" << ffit->GetParameter(0));
        ATH_MSG_DEBUG("Cluster position"<< x0);
        ATH_MSG_DEBUG("Fit angle: "<<fitAngle <<" "<<fitAngle*180./3.1415);
        ATH_MSG_DEBUG("ChisSqProb"<< chiSqProb);
        ATH_MSG_DEBUG("nStrips:"<<idxSelected.size());
    }



    return StatusCode::SUCCESS;

}
