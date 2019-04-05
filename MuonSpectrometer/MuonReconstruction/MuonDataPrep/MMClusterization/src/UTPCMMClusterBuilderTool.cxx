/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "UTPCMMClusterBuilderTool.h"

Muon::UTPCMMClusterBuilderTool::UTPCMMClusterBuilderTool(const std::string& t,
                       const std::string& n,
                       const IInterface* p)
    :
    AthAlgTool(t,n,p),
    m_muonMgr(nullptr),
    m_mmIdHelper(nullptr)
{
    declareInterface<IMMClusterBuilderTool>(this);
    declareProperty("HoughAlphaMin",m_alphaMin=0); //
    declareProperty("HoughAlphaMax",m_alphaMax=90);
    declareProperty("HoughAlphaResolution",m_alphaResolution=.5);
    declareProperty("HoughDMax",m_dMax=0);
    declareProperty("HoughDMin",m_dMin=0);
    declareProperty("HoughDResolution",m_dResolution=1.0);
    declareProperty("HoughMinCounts",m_houghMinCounts=3);
    declareProperty("uTPCTimeOffset",m_timeOffset=0);
    declareProperty("uTPCDHalf",m_dHalf=2.5);
    declareProperty("vDrift",m_vDrift=0.048);
}

Muon::UTPCMMClusterBuilderTool::~UTPCMMClusterBuilderTool()
{

}

StatusCode Muon::UTPCMMClusterBuilderTool::initialize(){
    StoreGateSvc* detStore=0;
    StatusCode sc = serviceLocator()->service("DetectorStore",detStore);
    if(sc.isSuccess()){
        sc = detStore->retrieve(m_muonMgr);
        if(sc.isFailure()){
            ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
            return sc;
        }
    }
    else{
        ATH_MSG_ERROR("DetectorStore not found");
        return sc;
    }

    m_mmIdHelper = m_muonMgr->mmIdHelper();
    //ATH_CHECK(book(TH1F("test","test",100,0,100)));

    return StatusCode::SUCCESS;
}


StatusCode Muon::UTPCMMClusterBuilderTool::finalize(){
    return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, std::vector<Muon::MMPrepData*>& clustersVect){
    //ATH_MSG_FATAL("get cluster in uTPC");

    //if(MMprds.size()<=3) return StatusCode::FAILURE;

    std::vector<std::vector<Muon::MMPrepData>> MMprdsPerLayer(8,std::vector<Muon::MMPrepData>(0));
    for (auto MMprd:MMprds){
        Identifier id = MMprd.identify();
        int layer = 4*(m_mmIdHelper->multilayer(id)-1)+(m_mmIdHelper->gasGap(id)-1);
        MMprdsPerLayer.at(layer).push_back(MMprd);
    }


    for(auto MMprdsOfLayer:MMprdsPerLayer){
        std::vector<float> stripsTime,stripsLocalPosX;
        std::vector<Identifier> strips_id;
        std::vector<int> flag=std::vector<int>(MMprdsOfLayer.size(),0); // vector of 0s and ones to indicate if strip was already used
        for( auto MMprd: MMprdsOfLayer){
            Identifier id_prd=MMprd.identify();
            strips_id.push_back(id_prd);
            stripsLocalPosX.push_back(MMprd.localPosition().x());
            stripsTime.push_back(MMprd.time());

            ATH_MSG_VERBOSE("Hit channel: "<< m_mmIdHelper->channel(id_prd) <<" time "<< MMprd.time() <<" gas_gap "<< m_mmIdHelper->gasGap(id_prd) << " multiplet " << m_mmIdHelper->multilayer(id_prd) << " stationname " <<m_mmIdHelper->stationName(id_prd)  << " stationPhi " <<m_mmIdHelper->stationPhi(id_prd) << " stationEta "<<m_mmIdHelper->stationEta(id_prd));


        }
        while(true){    
            std::vector<int> idx_goodStrips;
            StatusCode sc=runHoughTrafo(flag,stripsLocalPosX,stripsTime,idx_goodStrips);
            if(sc.isFailure()) break;
            if(idx_goodStrips.size()<3) break; // should be already catched in runHough but for safety once more here
            float localClusterPosition=-9999;
            sc=finalFit(stripsLocalPosX,stripsTime,idx_goodStrips,localClusterPosition);
            if(sc.isFailure()) break;
            ATH_MSG_VERBOSE("Did final fit");
            std::vector<Identifier> stripsOfCluster;
            int firstIdx=-1;
            ATH_MSG_DEBUG("Found good Strips: "<< idx_goodStrips.size());



            for(int idx:idx_goodStrips){
                flag.at(idx)=1;
                stripsOfCluster.push_back(strips_id.at(idx));
            }
            Amg::MatrixX* covN = new Amg::MatrixX(1,1);
            covN->setIdentity();
            int idx = idx_goodStrips[0];
            ATH_MSG_VERBOSE("Idx: "<<idx);
            Amg::Vector2D localClusterPositionV(localClusterPosition,MMprdsOfLayer.at(idx).localPosition().y()); // y position is the same for all strips

            ATH_MSG_VERBOSE("Did set covN Matrix");
            MMPrepData* prdN=new MMPrepData(MMprdsOfLayer.at(idx).identify(),MMprdsOfLayer.at(idx).collectionHash(),
                                 localClusterPositionV,stripsOfCluster,covN,MMprdsOfLayer.at(idx).detectorElement());
            clustersVect.push_back(prdN);
            ATH_MSG_VERBOSE("pushedBack  prdN");
            int leftOverStrips=0;
            for(f:flag){if(f==0) leftOverStrips++;}
            if(leftOverStrips<3) return; 
        }
    }
    
    
    
    //if(clustersVect)


    return StatusCode::SUCCESS;

}


StatusCode Muon::UTPCMMClusterBuilderTool::runHoughTrafo(std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time,std::vector<int>& idx_selected){

    float meanX = std::accumulate(xpos.begin(),xpos.end(),0.0)/xpos.size();
    float maxX =  *std::max_element(xpos.begin(),xpos.end());
    float minX =  *std::min_element(xpos.begin(),xpos.end());    
    TH2F *h_hough=0;
    TH1F *h_houghFine=0;
    StatusCode sc = houghInitCummulator(h_hough,h_houghFine,maxX,minX,meanX);

    ATH_MSG_VERBOSE("h_hough "<< h_hough <<" h_houghFine "<<h_houghFine);
    if(sc.isFailure()) return sc;
    sc = fillHoughTrafo(h_hough,flag,xpos,time,meanX);
    ATH_MSG_VERBOSE("filled Hough");
    if(sc.isFailure()) return sc;
    float amean,dmean,dRMS;
    sc=findMaxAlpha(h_hough,amean);
    ATH_MSG_DEBUG("found Alpha Max "<<amean);
    if(sc.isFailure()) return sc;
    sc=doFineScan(h_houghFine,flag,xpos,time,amean,meanX,dmean,dRMS);
    ATH_MSG_VERBOSE("did fine scan");
    if(sc.isFailure()) return sc;
    float slope,intercept,interceptRMS;
    sc=transformParameters(amean,dmean,dRMS,slope,intercept,interceptRMS);

    ATH_MSG_VERBOSE("transformed Parameter");
    if(sc.isFailure()) return sc;
    sc=selectPoints(flag,xpos,time,slope,intercept,interceptRMS,meanX,idx_selected);
    if(sc.isFailure()) return sc;
    h_hough->Delete();
    h_houghFine->Delete();
    return StatusCode::SUCCESS;
    
}

StatusCode Muon::UTPCMMClusterBuilderTool::houghInitCummulator(TH2F*& h_hough,TH1F*& h_houghFine,float xmax,float xmin,float xmean){
    ATH_MSG_VERBOSE("xmax: "<< xmax <<" xmin: "<< xmin <<" xmean "<< xmean <<" m_dResolution "<< m_dResolution <<" m_alphaMin "<< m_alphaMin <<" m_alphaMax: "<< m_alphaMax <<" m_toRad: "<< m_toRad <<" m_alphaResolution: "<<m_alphaResolution);

    float dmax=std::max(fabs(xmin-xmean),fabs(xmax-xmean));
    dmax=2*TMath::Sqrt(pow(dmax,2)+pow(6,2)); // rspace =sqrt(xmax*xmax+driftrange*driftrange) where driftrange is assumed to be 6mm
    int nbinsd = static_cast<int>(1.0*dmax*2/m_dResolution);
    int nbinsa = static_cast<int>((1.0*m_alphaMax-m_alphaMin)/m_alphaResolution);

    ATH_MSG_DEBUG("Hough Using nBinsA "<< nbinsa <<" nBinsd "<< nbinsd);

    h_hough = new TH2F("h_hough","h_hough",nbinsa,m_alphaMin*m_toRad,m_alphaMax*m_toRad,nbinsd,-dmax,dmax);
    h_houghFine = new TH1F("h_houghFine","h_houghFine",nbinsd*5,-dmax,dmax);

   return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::fillHoughTrafo(TH2F* h_hough,std::vector<int>& flag,std::vector<float>& xpos, std::vector<float>& time, float meanX){
   for(size_t i_hit=0; i_hit<xpos.size(); i_hit++){
    if(flag.at(i_hit)==1) continue; //skip hits which have been already used
    float x=xpos.at(i_hit)-meanX;
    float y=time.at(i_hit)*m_vDrift;
    for(int i_alpha=1;i_alpha<h_hough->GetNbinsX();i_alpha++){
        float alpha=h_hough->GetXaxis()->GetBinCenter(i_alpha);
        float d=x*TMath::Cos(alpha)+y*TMath::Sin(alpha);
        h_hough->Fill(alpha,d);
    }
   }
   return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::findMaxAlpha(TH2F* h_hough,float& amean){
    int cmax=h_hough->GetMaximum();
    ATH_MSG_DEBUG("FindMaxAlpha: cmx= "<<cmax);
    for(int i_binX=0; i_binX<=h_hough->GetNbinsX();i_binX++){
        for(int i_binY=0; i_binY<=h_hough->GetNbinsY();i_binY++){
            if(h_hough->GetBinContent(i_binX,i_binY)>=5){
            ATH_MSG_VERBOSE("Find Max Alpha: BinX "<< i_binX <<" BinY: "<< i_binY <<" over threshold: "<< h_hough->GetBinContent(i_binX,i_binY));
            }
        }
    }

    if(cmax<m_houghMinCounts) return StatusCode::FAILURE;

    for(int i_bin=1;i_bin<=h_hough->GetSize();i_bin++){
        if(h_hough->GetBinContent(i_bin)<cmax) h_hough->SetBinContent(i_bin,0);
    }
    amean=h_hough->ProjectionX()->GetMean();

    return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::doFineScan(TH1F* h_houghFine,std::vector<int>& flag ,std::vector<float>& xpos,
    std::vector<float>& time,float amean,float meanX,float& dmean, float& dRMS){
        for(size_t i_hit=0; i_hit<xpos.size(); i_hit++){
            if(flag.at(i_hit)==1) continue;
            float x=xpos.at(i_hit)-meanX;
            float y=time.at(i_hit)*m_vDrift;
            float d=x*TMath::Cos(amean)+y*TMath::Sin(amean);
            h_houghFine->Fill(d);
        }
        TF1* ffit=new TF1("ffit","gaus"); //
        ffit->SetParameters(h_houghFine->GetMaximum(),h_houghFine->GetMean(),h_houghFine->GetRMS()); 
        int s=h_houghFine->Fit("ffit");
        if (s==0) ATH_MSG_DEBUG("Fine Scan Failed");
        dmean = ffit->GetParameter(1);
        dRMS  = ffit->GetParameter(2);
        delete ffit;
        return StatusCode::SUCCESS;
}

StatusCode Muon::UTPCMMClusterBuilderTool::transformParameters(float alpha, float d, float dRMS, float& slope,
    float& intercept, float& interceptRMS){

        slope=-1./TMath::Tan(alpha);
        intercept=d/TMath::Sin(alpha);
        interceptRMS=dRMS/TMath::Sin(alpha);                                                        
    
        return StatusCode::SUCCESS;
}


StatusCode Muon::UTPCMMClusterBuilderTool::selectPoints(std::vector<int>& flag, std::vector<float>& xpos, std::vector<float>& time,
         float& slope, float& intercept, float& interceptRMS,float& xmean,std::vector<int>& idxSelected){
            for (float sigmaFactor=1;sigmaFactor<=2;sigmaFactor++){
                for(size_t i_hit=0; i_hit<xpos.size(); i_hit++){
                    if(flag.at(i_hit)==1) continue;
                    if(fabs(time.at(i_hit)*m_vDrift-(slope*(xpos.at(i_hit)-xmean)+intercept))<interceptRMS*sigmaFactor){
                    idxSelected.push_back(i_hit);
                    }
                }
                if(idxSelected.size()>2){
                    return StatusCode::SUCCESS;
                }
                idxSelected.clear();
            }
            return StatusCode::FAILURE;
}

StatusCode Muon::UTPCMMClusterBuilderTool::finalFit(std::vector<float>& xpos, std::vector<float>& time, std::vector<int>& idxSelected, float& x0){
    TGraphErrors *fitGraph=new TGraphErrors();
    TF1 *ffit=new TF1("ffit","pol1");
    
    for(int idx:idxSelected){
        fitGraph->SetPoint(fitGraph->GetN(),xpos.at(idx),time.at(idx)*m_vDrift);
        fitGraph->SetPointError(fitGraph->GetN()-1,0.425/3.464101615 /*sqrt(12)*/,10*m_vDrift);
    }
    int s=fitGraph->Fit("ffit");
    if(s!=0) s=fitGraph->Fit("ffit"); //redo fit if first one failed
    if(s!=0) return StatusCode::FAILURE;

    x0=(ffit->GetParameter(0)-m_dHalf)/ffit->GetParameter(1);

    return StatusCode::SUCCESS;

}