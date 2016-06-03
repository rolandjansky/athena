/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexWeightCalculators/NNVertexWeightCalculator.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "FuncMeanRMS.h" 
#include <TH1.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TMatrixDEigen.h>
#include <TVectorT.h>
#include <vector>
#include <TString.h>
#include <string>

#include "TrkNeuralNetworkUtils/TTrainedNetwork.h"
#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"
 
namespace Trk{
  
  //usual tool related methods
  StatusCode NNVertexWeightCalculator::initialize()
  {
    StatusCode sc = AthAlgTool::initialize();
    
    //initializing the AlgTool itself
    if(sc.isFailure())
    {
      msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endreq;
      return StatusCode::FAILURE;
    }
    (TString)m_directoryName;
    
    sc = m_networkToHistoTool.retrieve();
    if (sc.isFailure())
    {
      msg(MSG::FATAL) << " Could not retrieve " << m_networkToHistoTool  << ". Aborting..." << endreq;
      return sc;
    } 
    else msg(MSG::INFO) << " Retrieved: " << m_networkToHistoTool << ". " << endreq;


    if (!service("THistSvc",m_iTHistSvc , true).isSuccess()) 
    {
      msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
      return StatusCode::FAILURE;
    }
    
    TString LayerInfo = "LayersInfo";
    sc = m_iTHistSvc->regHist(std::string((const char*)(m_directoryName+LayerInfo)));
    if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
      return StatusCode::FAILURE;
    }
    
    m_NNhistos.push_back(0);
    
    sc = m_iTHistSvc->getHist(std::string((const char*)(m_directoryName+LayerInfo)),m_NNhistos[0]);
    
    if (sc.isFailure())
    {
      msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
      return StatusCode::FAILURE;
    }


    for (int i = 0; i < m_NLayers - 1; i++)
    {
      TString weightName("Layer");
      weightName +=i;
      weightName +="_weights";
      
      TString thresholdName("Layer");
      thresholdName +=i;
      thresholdName +="_thresholds";
      
      sc = m_iTHistSvc->regHist(std::string((const char*)(m_directoryName+weightName)));
      if (sc.isFailure())
      {
        msg(MSG::ERROR) << "Unable to locate THistSvc" << endreq;
        return StatusCode::FAILURE;
      }
      m_NNhistos.push_back(0);
      sc = m_iTHistSvc->getHist(std::string((const char*)(m_directoryName+weightName)),m_NNhistos[2*i + 1]);
      
      sc = m_iTHistSvc->regHist(std::string((const char*)(m_directoryName+thresholdName)));
      if (sc.isFailure())
      {
        msg(MSG::ERROR)<< "Unable to locate THistSvc" << endreq;
        return StatusCode::FAILURE;
      }
      m_NNhistos.push_back(0);
      sc = m_iTHistSvc->getHist(std::string((const char*)(m_directoryName+thresholdName)),m_NNhistos[2*i + 2]);
    }
    
    msg(MSG::INFO)<<"Initialization successfull"<<endreq;
    return StatusCode::SUCCESS;
  }//end of initialize method
 
 
  StatusCode NNVertexWeightCalculator::finalize()
  {
    msg(MSG::INFO)<< "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }
 
 //class constructor implementation
  NNVertexWeightCalculator::NNVertexWeightCalculator(const std::string& t, const std::string& n, const IInterface*  p):
          AthAlgTool(t,n,p),
          m_networkToHistoTool("Trk::NeuralNetworkToHistoTool"),
          m_NN(0),
          m_NLayers(3),
          m_iTHistSvc(0),
          m_directoryName("/NNHisto/")
  {
    declareProperty("HistoFilePath",m_directoryName );
    declareProperty("NeuralNetworkToHistoTool",m_networkToHistoTool);
  
    declareInterface<IVertexWeightCalculator>(this);
  }
 
  //destructor
  NNVertexWeightCalculator::~NNVertexWeightCalculator(){}
  
double  NNVertexWeightCalculator::estimateSignalCompatibility(const xAOD::Vertex& vertex)
{
  std::vector<const Trk::TrackParameters*> tpList;
  
  for(const auto& elTrackParticle : vertex.trackParticleLinks()) {
    
    if (not elTrackParticle.isValid()) {
      ATH_MSG_WARNING("No valid link to tracks in xAOD::Vertex object. Skipping track for signal compatibility (may be serious).");
      continue;
    }
    const Trk::Perigee& perigee = (*elTrackParticle.cptr())->perigeeParameters();
    tpList.push_back(static_cast<const Trk::TrackParameters*>(&perigee));
  }   

  return mEstimateSignalCompatibility(tpList);
  
}

double NNVertexWeightCalculator::mEstimateSignalCompatibility(const std::vector<const Trk::TrackParameters*>& tpList)
{
  double    NTrks = (double)tpList.size();
     
  double    st1Moment = 0;
  double    st2Moment = 0;
  double    st3Moment = 0;
  double    st4Moment = 0;
  double    st5Moment = 0;
  double    st6Moment = 0;
  double    st7Moment = 0;
  double    st8Moment = 0;
  double    st9Moment = 0;
  double    st10Moment = 0;
  
  double    Sphericity = 0;
  
  if (tpList.size()>0)
    {
      for(const Trk::TrackParameters* perigee : tpList) {
        double p_T = std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta])/1000.;
        st1Moment += p_T;
      }
      st1Moment = st1Moment/NTrks;
      std::vector <std::vector<double> > v_allPs;
             
      for(const Trk::TrackParameters* perigee : tpList) {
        double p_T = std::fabs(1./perigee->parameters()[Trk::qOverP])*sin(perigee->parameters()[Trk::theta])/1000.;
         
        st2Moment = st2Moment + TMath::Power(p_T - st1Moment,2);
        st3Moment = st3Moment + TMath::Power(p_T - st1Moment,3);
        st4Moment = st4Moment + TMath::Power(p_T - st1Moment,4);
        st5Moment = st5Moment + TMath::Power(p_T - st1Moment,5);
        st6Moment = st6Moment + TMath::Power(p_T - st1Moment,6);
        st7Moment = st7Moment + TMath::Power(p_T - st1Moment,7);
        st8Moment = st8Moment + TMath::Power(p_T - st1Moment,8);
        st9Moment = st9Moment + TMath::Power(p_T - st1Moment,9);
        st10Moment = st10Moment + TMath::Power(p_T - st1Moment,10); 
         
        double thisPx = perigee->parameters()[Trk::qOverP]*sin(perigee->parameters()[Trk::theta])*cos(perigee->parameters()[Trk::phi])/1000.;
        double thisPy= perigee->parameters()[Trk::qOverP]*sin(perigee->parameters()[Trk::theta])*sin(perigee->parameters()[Trk::phi])/1000.;
        double thisPz= perigee->parameters()[Trk::qOverP]*cos(perigee->parameters()[Trk::theta])/1000.;
         
        std::vector <double> V_PxPyPz;
        V_PxPyPz.push_back(thisPx);
        V_PxPyPz.push_back(thisPy);
        V_PxPyPz.push_back(thisPz);
         
        v_allPs.push_back(V_PxPyPz);
        
      }
      Sphericity = sphericityFunc(v_allPs);
             
      st2Moment = st2Moment/NTrks;
      st3Moment = st3Moment/NTrks;
      st4Moment = st4Moment/NTrks;
      st5Moment = st5Moment/NTrks;
      st6Moment = st6Moment/NTrks;
      st7Moment = st7Moment/NTrks;
      st8Moment = st8Moment/NTrks;
      st9Moment = st9Moment/NTrks;
      st10Moment =st10Moment/NTrks;
       
      std::vector <double> NNinput;
      NNinput.push_back(Norm_NTrks(((int)NTrks)));
      NNinput.push_back(Norm_st1Mom(LogFunc(st1Moment)));
      NNinput.push_back(Norm_st2Mom(LogFunc(st2Moment)));
      NNinput.push_back(Norm_st3Mom(LogFunc(st3Moment)));
      NNinput.push_back(Norm_st4Mom(LogFunc(st4Moment)));   
      NNinput.push_back(Norm_st5Mom(LogFunc(st5Moment)));
      NNinput.push_back(Norm_st6Mom(LogFunc(st6Moment)));
      NNinput.push_back(Norm_st7Mom(LogFunc(st7Moment)));
      NNinput.push_back(Norm_st8Mom(LogFunc(st8Moment)));
      NNinput.push_back(Norm_st9Mom(LogFunc(st9Moment)));
      NNinput.push_back(Norm_st10Mom(LogFunc(st10Moment)));
      NNinput.push_back(Norm_Sphere((Sphericity)));
      
      m_NN = m_networkToHistoTool->fromHistoToTrainedNetwork(m_NNhistos);
       
      return m_NN->calculateOutputValues(NNinput).at(0);
    } //at least one track
  
  return 0;
}

double NNVertexWeightCalculator::sphericityFunc(std::vector <std::vector<double> > v_allPs)
{
  TMatrixD a_P(2,2);
  
  for (int a=0; a<2; a++)
  {
    for (int b=0; b<2; b++)
    {
      double SumAbsP = 0;
      double SumPaPb = 0;
      
      for (unsigned int i=0; i < v_allPs.size(); i++)
      {
        double AbsP = TMath::Abs(v_allPs.at(i).at(0)*v_allPs.at(i).at(0) + v_allPs.at(i).at(1)*v_allPs.at(i).at(1));
        SumAbsP = SumAbsP + AbsP;
        double PaPb  = v_allPs.at(i).at(a)*v_allPs.at(i).at(b);
        SumPaPb = SumPaPb + PaPb;
      }
      a_P[a][b]=SumPaPb/SumAbsP;
    }      
  }

  TVectorT<double> EigVal;
  
  a_P.EigenVectors(EigVal);
  double smallL =0;
  
  if ( EigVal[0]< EigVal[1])
      smallL = EigVal[0];
  else
      smallL = EigVal[1];
  
  return smallL*2;
}

double NNVertexWeightCalculator::LogFunc(double value)
{
  double x0 = 0.05e-3;
  double a = 20000.;//f1_strich->Eval(x0);
  double y =  -9.90349;// f1->Eval(x0);
  double y0 = y - a*x0;
  
  if (value <= -x0)
      return -TMath::Log(-value) + 2*y0;
  else if ((value > -x0)  && (value < x0))
      return y0 + a*value;
  else if (value >= x0)
      return TMath::Log(value);
  
  return 0;
 }

} //end namespace Trk
