/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "UTPCMMClusterBuilderTool.h"
#include "GaudiKernel/MsgStream.h"

namespace{

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
    declareProperty("HoughAlphaMin",m_alphaMin=-90); //degree
    declareProperty("HoughAlphaMax",m_alphaMax=90); //degree
    declareProperty("HoughAlphaResolution",m_alphaResolution=1.0); // degree
    declareProperty("HoughDMax",m_dMax=0);
    declareProperty("HoughDMin",m_dMin=0);
    declareProperty("HoughExpectedDriftRange",m_driftRange=12); //mm
    declareProperty("HoughDResolution",m_dResolution=0.125); 
    declareProperty("HoughMinCounts",m_houghMinCounts=3); 
    declareProperty("timeOffset",m_timeOffset=0); //ns
    declareProperty("uTPCDHalf",m_dHalf=2.5); //mm
    declareProperty("HoughSelectionCut",m_selectionCut=1.0); //mm
    declareProperty("vDrift",m_vDrift=0.047); //mm/us
    declareProperty("longDiff",m_longDiff=0.019); //mm/mm
    declareProperty("transDiff",m_transDiff=0.036); //mm/mm
    declareProperty("ionUncertainty",m_ionUncertainty=4.0); //ns
    declareProperty("scaleXerror",m_scaleXerror=1.);
    declareProperty("scaleYerror",m_scaleYerror=1.);
    declareProperty("outerChargeRatioCut",m_outerChargeRatioCut=0.); //charge ratio cut to supress cross talk
    declareProperty("maxStripRemove",m_maxStripsCut=4); //max number of strips cut by cross talk cut

}


StatusCode Muon::UTPCMMClusterBuilderTool::initialize(){
    ATH_CHECK( m_muonIdHelperTool.retrieve() );
    ATH_MSG_INFO("Set vDrift to "<< m_vDrift <<" um/ns");
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

    //sort MMPrds by channel
    for(unsigned int i_layer=0;i_layer<MMprdsPerLayer.size();i_layer++){
        std::sort(MMprdsPerLayer.at(i_layer).begin(),MMprdsPerLayer.at(i_layer).end(),
                  [this](const MMPrepData &a,const MMPrepData &b){return this->m_muonIdHelperTool->mmIdHelper().channel(a.identify())<this->m_muonIdHelperTool->mmIdHelper().channel(b.identify());});
    }



    for(const auto& MMprdsOfLayer:MMprdsPerLayer){
        std::vector<double> stripsTime,stripsLocalPosX;
        std::vector<Identifier> strips_id;
        std::vector<int> stripsCharge,stripsChannel;

        std::vector<int> flag=std::vector<int>(MMprdsOfLayer.size(),0); // vector of 0s and ones to indicate if strip was already used (0=unused;1=used;2=rejected as cross talk)
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

            stripsTime.push_back(MMprd.time()-tof+m_timeOffset); // use time minus tof minus vmm integration time as actual time
            stripsChannel.push_back(m_muonIdHelperTool->mmIdHelper().channel(id_prd));
            stripsCharge.push_back(MMprd.charge());
            
            ATH_MSG_DEBUG("Hit channel: "<< m_muonIdHelperTool->mmIdHelper().channel(id_prd) <<" time "<< MMprd.time()-tof+m_timeOffset << " localPosX "<< MMprd.localPosition().x() << " tof "<<tof <<" angleToIp " << angleToIp<<" gas_gap "<< m_muonIdHelperTool->mmIdHelper().gasGap(id_prd) << " multiplet " << m_muonIdHelperTool->mmIdHelper().multilayer(id_prd) << " stationname " <<m_muonIdHelperTool->mmIdHelper().stationName(id_prd)  << " stationPhi " <<m_muonIdHelperTool->mmIdHelper().stationPhi(id_prd) << " stationEta "<<m_muonIdHelperTool->mmIdHelper().stationEta(id_prd));
        }
        if(MMprdsOfLayer.size()<2) continue;

        //move hits to the origin of the coordinate system to exclude extrapolation error and to keep the size of the hough space small
        double meanX = std::accumulate(stripsLocalPosX.begin(),stripsLocalPosX.end(),0.0)/stripsLocalPosX.size();
        ATH_MSG_DEBUG("Got mean element "<<meanX );
        std::for_each(stripsLocalPosX.begin(),stripsLocalPosX.end(),[meanX](double& d){d-=meanX;});
        ATH_MSG_VERBOSE(printVec(stripsLocalPosX)); 

        while(true){
            ATH_MSG_DEBUG("while true"); 
            std::vector<int> idx_goodStrips;
            StatusCode sc=runHoughTrafo(flag,stripsLocalPosX,stripsTime,idx_goodStrips);

            ATH_MSG_DEBUG("Hough done"); 
            //if(sc.isFailure()) break;
            if(idx_goodStrips.size()<3 || sc.isFailure()){
                //break; // should be already catched in runHough but for safety once more here
                idx_goodStrips.clear();
                for(size_t i_hit=0;i_hit<MMprdsOfLayer.size();i_hit++){
                    if(flag.at(i_hit)==0) idx_goodStrips.push_back(i_hit);
                }
                ATH_MSG_DEBUG("Hough Trafo failed, try to fit everything");
            }

            //remove strips induced by crosstalk till no more strip was removed
            int nStripsCutByCrosstalk=0;
            while(!applyCrossTalkCut(idx_goodStrips,MMprdsOfLayer,flag,nStripsCutByCrosstalk).isFailure()){}

                
            double localClusterPosition=-9999;
            double sigmaLocalClusterPosition=0;
            double finalFitAngle,finalFitChiSqProb;
            sc=finalFit(stripsLocalPosX,stripsTime,idx_goodStrips,localClusterPosition, sigmaLocalClusterPosition,finalFitAngle,finalFitChiSqProb);
            localClusterPosition+=meanX;
            ATH_MSG_DEBUG("final fit done"); 
            
            if(sc.isFailure()) break;
            ATH_MSG_DEBUG("Did final fit successfull");
            std::vector<Identifier> stripsOfCluster;
            std::vector<uint16_t> stripsOfClusterChannels;
            std::vector<short int> stripsOfClusterTimes;
            std::vector<int> stripsOfClusterCharges;

            ATH_MSG_DEBUG("Found good Strips: "<< idx_goodStrips.size());


            for(int idx:idx_goodStrips){
                ATH_MSG_DEBUG("Setting good strip: "<<idx<< " size of strips flag: "<<flag.size());
                flag.at(idx)=1;
                ATH_MSG_DEBUG("Set Good strips");
                stripsOfCluster.push_back(strips_id.at(idx));
                stripsOfClusterChannels.push_back(stripsChannel.at(idx));
                stripsOfClusterTimes.push_back(stripsTime.at(idx));
                stripsOfClusterCharges.push_back(stripsCharge.at(idx));
            }
            Amg::MatrixX* covN = new Amg::MatrixX(1,1);
            covN->coeffRef(0,0)=sigmaLocalClusterPosition;
            //covN->coeffRef(0,0)=0.25;
            ATH_MSG_DEBUG("Did set covN Matrix");
            int idx = idx_goodStrips[0];
            ATH_MSG_DEBUG("idx_goodStrips[0]: "<<idx<<"MMPRDs size: "<< MMprdsOfLayer.at(idx) << " size: " <<MMprdsOfLayer.size());
            Amg::Vector2D localClusterPositionV(localClusterPosition,MMprdsOfLayer.at(idx).localPosition().y()); // y position is the same for all strips
            ATH_MSG_DEBUG("Did set local position");
            MMPrepData* prdN=new MMPrepData(MMprdsOfLayer.at(idx).identify(),MMprdsOfLayer.at(idx).collectionHash(),
                                 localClusterPositionV,stripsOfCluster,covN,MMprdsOfLayer.at(idx).detectorElement(),
                                 (short int)0,std::accumulate(stripsOfClusterCharges.begin(),stripsOfClusterCharges.end(),0),
                                 stripsOfClusterChannels,stripsOfClusterTimes,stripsOfClusterCharges);
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

StatusCode Muon::UTPCMMClusterBuilderTool::runHoughTrafo(std::vector<int>& flag,std::vector<double>& xpos, std::vector<double>& time,std::vector<int>& idx_selected){
    ATH_MSG_DEBUG("beginning of runHoughTrafo() with: " << xpos.size() <<" hits" );
    double maxX =  *std::max_element(xpos.begin(),xpos.end());
    double minX =  *std::min_element(xpos.begin(),xpos.end());    
    std::unique_ptr<TH2D> h_hough=0;
    StatusCode sc = houghInitCummulator(h_hough,maxX,minX);

    if(sc.isFailure()) return sc;
    sc = fillHoughTrafo(h_hough,flag,xpos,time);
    ATH_MSG_DEBUG("filled Hough");
    if(sc.isFailure()) return sc;

    std::vector<std::tuple<double,double>> houghPeaks;
    sc = findAlphaMax(h_hough,houghPeaks);
    if(sc.isFailure()) return sc;
    ATH_MSG_DEBUG("Found HoughPeaks");
    
    sc = selectTrack(houghPeaks,xpos,time,flag,idx_selected);
    ATH_MSG_DEBUG("Selected track");
    if(sc.isFailure()) return sc;
    return StatusCode::SUCCESS;   
}

StatusCode Muon::UTPCMMClusterBuilderTool::houghInitCummulator(std::unique_ptr<TH2D>& h_hough,double xmax,double xmin){
    ATH_MSG_VERBOSE("xmax: "<< xmax <<" xmin: "<< xmin <<" m_dResolution "<< m_dResolution <<" m_alphaMin "<< m_alphaMin <<" m_alphaMax: "<< m_alphaMax <<" m_toRad: "<< m_toRad <<" m_alphaResolution: "<<m_alphaResolution);

    double dmax=std::max(fabs(xmin),fabs(xmax));
    dmax=TMath::Sqrt(pow(dmax,2)+pow(m_driftRange,2)); // rspace =sqrt(xmax*xmax+driftrange*driftrange) where driftrange is assumed to be 6mm
    int nbinsd = static_cast<int>(1.0*dmax*2/m_dResolution);
    int nbinsa = static_cast<int>((1.0*m_alphaMax-m_alphaMin)/m_alphaResolution);

    ATH_MSG_VERBOSE("Hough Using nBinsA "<< nbinsa <<" nBinsd "<< nbinsd<<" dmax "<< dmax);

    h_hough = std::unique_ptr<TH2D>(new TH2D("h_hough","h_hough",nbinsa,m_alphaMin*m_toRad,m_alphaMax*m_toRad,nbinsd,-dmax,dmax));

    return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::fillHoughTrafo(std::unique_ptr<TH2D>& h_hough,std::vector<int>& flag,std::vector<double>& xpos, std::vector<double>& time){
   for(size_t i_hit=0; i_hit<xpos.size(); i_hit++){
    if(flag.at(i_hit)!=0) continue; //skip hits which have been already used or been rejected as cross talk
    double x=xpos.at(i_hit);
    double y=time.at(i_hit)*m_vDrift;
    for(int i_alpha=1;i_alpha<h_hough->GetNbinsX();i_alpha++){
        double alpha=h_hough->GetXaxis()->GetBinCenter(i_alpha);
        double d= x*TMath::Cos(alpha) - y*TMath::Sin(alpha);
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

StatusCode Muon::UTPCMMClusterBuilderTool::selectTrack(std::vector<std::tuple<double,double>> &tracks,std::vector<double>& xpos, std::vector<double>& time,
                                std::vector<int>& flag, std::vector<int> &idxGoodStrips){
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
                dist.push_back(time.at(i_hit)*m_vDrift-slope*xpos.at(i_hit)-intercept);
                indexUsedForDist.push_back(i_hit);
            }
        }
        ATH_MSG_VERBOSE("indexUsedForDist "<< printVec(indexUsedForDist));
        ATH_MSG_VERBOSE("distVec: "<<printVec(dist));
        dist.clear();
        for(size_t i_hit: indexUsedForDist){
            double d=time.at(i_hit)*m_vDrift-slope*xpos.at(i_hit)-intercept;
            ATH_MSG_VERBOSE("dist: "<<d<<" for index "<<i_hit);
            if (fabs(d)<m_selectionCut){  
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

StatusCode Muon::UTPCMMClusterBuilderTool::applyCrossTalkCut(std::vector<int> &idxSelected,const std::vector<MMPrepData> &MMPrdsOfLayer,std::vector<int> &flag, int &nStripsCut){
   int N=idxSelected.size();
   bool removedStrip=false;
   if (N<3 || nStripsCut>=m_maxStripsCut ) return StatusCode::FAILURE;
   //reject outer strip for fit if charge ratio to second outer strip indicates strip charge is created by crosstalk only 
   if(1.0*MMPrdsOfLayer.at(idxSelected.at(N-1)).charge()/MMPrdsOfLayer.at(idxSelected.at(N-2)).charge()<m_outerChargeRatioCut){
       flag.at(idxSelected.at(N-1))=2;
       idxSelected.erase(idxSelected.begin()+(N-1));
       removedStrip=true;
       nStripsCut++;
   }
   if(idxSelected.size()<3 || nStripsCut>=m_maxStripsCut) return StatusCode::FAILURE;

   if(1.0*MMPrdsOfLayer.at(idxSelected.at(0)).charge()/MMPrdsOfLayer.at(idxSelected.at(1)).charge()<m_outerChargeRatioCut){
       flag.at(idxSelected.at(0))=2;
       idxSelected.erase(idxSelected.begin()+0);
       removedStrip=true;
       nStripsCut++;
   }
   if(nStripsCut>=m_maxStripsCut) return StatusCode::FAILURE;
   return (!removedStrip ? StatusCode::FAILURE:StatusCode::SUCCESS); //return success if at least one strip was removed
}


StatusCode Muon::UTPCMMClusterBuilderTool::finalFit(std::vector<double>& xpos, std::vector<double>& time, std::vector<int>& idxSelected, double& x0, double &sigmaX0 ,double &fitAngle, double &chiSqProb){
    std::unique_ptr<TGraphErrors> fitGraph=std::unique_ptr<TGraphErrors>(new TGraphErrors());
    std::unique_ptr<TF1> ffit=std::unique_ptr<TF1>(new TF1("ffit","pol1"));
    
    double xmin = 5000;
    double xmax = -5000;
    std::unique_ptr<TLinearFitter> lf=std::unique_ptr<TLinearFitter>(new TLinearFitter(1,"1++x"));
    
    for(int idx:idxSelected){
        if(xmin>xpos.at(idx)) xmin = xpos.at(idx);
        else if(xmax<xpos.at(idx)) xmax = xpos.at(idx);
        lf->AddPoint(&xpos.at(idx),time.at(idx)*m_vDrift);
        fitGraph->SetPoint(fitGraph->GetN(),xpos.at(idx),time.at(idx)*m_vDrift);
        fitGraph->SetPointError(fitGraph->GetN()-1,m_scaleXerror*pow(pow(0.425/3.464101615/*sqrt(12)*/,2)+pow(m_transDiff*time.at(idx)*m_vDrift,2),0.5), m_scaleYerror*pow(pow(m_ionUncertainty*m_vDrift,2)+pow(m_longDiff*time.at(idx)*m_vDrift,2),0.5));
    }
    
    lf->Eval(); 

    ffit->SetParLimits(1,-11.5,-0.15); //5 to 81 degree
    ffit->SetParameters(lf->GetParameter(0),lf->GetParameter(1));
    TFitResultPtr s=fitGraph->Fit("ffit","SMQ","",xmin-.5,xmax+.5);


    double cov = (s->GetCovarianceMatrix())(0,1);
    ATH_MSG_DEBUG("covariance is: "<<cov);
    double fitSlope = ffit->GetParameter(1);
    double interceptCorr = m_dHalf - ffit->GetParameter(0);
    x0 = interceptCorr/fitSlope;
    sigmaX0 = (pow(ffit->GetParError(0)/interceptCorr,2)+pow(ffit->GetParError(1)/fitSlope,2) - 2./(fitSlope*interceptCorr)*cov)*pow(x0,2);
    fitAngle = TMath::ATan(-1/fitSlope);
    chiSqProb=ffit->GetProb();

    if(true){
        ATH_MSG_DEBUG("==========uTPC fit Summary==============");
        ATH_MSG_DEBUG("Fit slope: " << ffit->GetParameter(1));
        ATH_MSG_DEBUG("Fit intercept:" << ffit->GetParameter(0));
        ATH_MSG_DEBUG("Fit status: "<<s);
        ATH_MSG_DEBUG("Cluster position "<< x0 << " +- " << sigmaX0);
        ATH_MSG_DEBUG("Fit angle: "<<fitAngle <<" "<<fitAngle*180./3.1415);
        ATH_MSG_DEBUG("ChisSqProb"<< chiSqProb);
        ATH_MSG_DEBUG("nStrips:"<<idxSelected.size());
    }
    if(s!=0){
        ATH_MSG_DEBUG("Fit failed twice");
        return StatusCode::FAILURE;
    }
    if(ffit->GetParameter(1)<=-11.5 || ffit->GetParameter(1)>=-0.15) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
}
