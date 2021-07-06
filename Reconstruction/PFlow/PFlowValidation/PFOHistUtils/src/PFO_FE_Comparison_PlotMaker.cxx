/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Author: M. T. Anthony

#include "PFOHistUtils/PFO_FE_Comparison_PlotMaker.h"
#include <math.h>
// some debug libraries for cluster moment floating point checks
#include <bitset>
#include <climits>
namespace PFO {
  PFO_FE_Comparison_PlotMaker::PFO_FE_Comparison_PlotMaker(PlotBase* pParent,std::string sDir, std::string sPFOContainerName, std::string sFEContainerName, bool isNeutral): PlotBase(pParent,sDir), asg::AsgMessaging("PFO_FE_Comparison_PlotMaker"), m_sPFOContainerName(sPFOContainerName), m_sFEContainerName(sFEContainerName), m_isNeutral(isNeutral) {
    // init all hists as nullptrs
    m_PFO_FE_ptratio=nullptr;
    m_PFO_FE_delta_pt=nullptr;
    m_PFO_FE_delta_R=nullptr;
    m_PFO_FE_delta_eta=nullptr;
    m_PFO_FE_delta_phi=nullptr;
    m_PFO_FE_dE=nullptr;


    // comparison of cluster observables - for want of a better solution, pick ratio as a comparison. Should be ~1 for both
    m_PFO_FE_SECOND_R_RelComparison = nullptr;
    m_PFO_FE_CENTER_LAMBDA_RelComparison = nullptr;
    m_PFO_FE_ISOLATION_RelComparison = nullptr;
    m_PFO_FE_ENG_BAD_CELLS_RelComparison = nullptr;
    m_PFO_FE_N_BAD_CELLS_RelComparison = nullptr;
    m_PFO_FE_BADLARQ_FRAC_RelComparison = nullptr;
    m_PFO_FE_ENG_POS_RelComparison = nullptr;
    m_PFO_FE_AVG_LAR_Q_RelComparison = nullptr;
    m_PFO_FE_AVG_TILE_Q_RelComparison = nullptr;
    m_PFO_FE_EM_PROBABILITY_RelComparison = nullptr;
    m_PFO_FE_SECOND_LAMBDA_RelComparison = nullptr;

    m_PFO_FE_TIMING_RelComparison=nullptr;

    //comparison of charged PFO/FE specific variables
    m_PFO_FE_isInDenseEnvironment_RelComparison=nullptr;
    m_PFO_FE_tracksExpectedEnergyDeposit=nullptr;    






  } // end of constructor
  
  void PFO_FE_Comparison_PlotMaker::initializePlots(){
    //initialise anyway regardless of flow element/PFOs configured - since this is simple kinematics
    m_PFO_FE_ptratio= Book1D("_ptratio",m_sPFOContainerName+m_sFEContainerName+"_ptratio",40,0,2);
    m_PFO_FE_delta_pt= Book1D("_deltapt",m_sPFOContainerName+m_sFEContainerName+"_deltapt (GeV)",40,-200,200);
    m_PFO_FE_delta_R=Book1D("_deltaR",m_sPFOContainerName+m_sFEContainerName+"_deltaR",30,0,3);
    m_PFO_FE_delta_eta=Book1D("_abs_delta_eta",m_sPFOContainerName+m_sFEContainerName+"_abs_delta_eta",20,0,5); // use absolute value since we check consistency, as opposed to absolute difference
    m_PFO_FE_delta_phi=Book1D("_delta_phi",m_sPFOContainerName+m_sFEContainerName+"_delta_phi",60,-3,3);
    m_PFO_FE_dE=Book1D("_dE",m_sPFOContainerName+m_sFEContainerName+"_dE (GeV)",40,-200,200);

    if(m_isNeutral){
      //Neutral FE vs PFO plots
      // since these are based on FEs/ PFOs which share the same topocluster, expect values very close to (if not exactly 1, so bin as finely as possible in this interval) - easier to rebin using TH1 rebin in plot generation if necessary

      m_PFO_FE_SECOND_R_RelComparison=Book1D("_SECOND_R_RelComparison",m_sPFOContainerName+m_sFEContainerName+"_SECOND_R_RelComparison",300,-1,2);
      m_PFO_FE_CENTER_LAMBDA_RelComparison=Book1D("_CENTER_LAMBDA_RelComparison",m_sPFOContainerName+m_sFEContainerName+"_CENTER_LAMBDA_RelComparison",300,-1,2);
      
      m_PFO_FE_ISOLATION_RelComparison=Book1D("_ISOLATION_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_ISOLATION_RelComparison",300,-1,2);
      m_PFO_FE_ENG_BAD_CELLS_RelComparison=Book1D("_ENG_BAD_CELLS_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_ENG_BAD_CELLS_RelComparison",300,-1,2);
      m_PFO_FE_N_BAD_CELLS_RelComparison=Book1D("_N_BAD_CELLS_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_N_BAD_CELLS_RelComparison",300,-1,2);
      m_PFO_FE_BADLARQ_FRAC_RelComparison=Book1D("_BADLARQ_FRAC_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_BADLARQ_FRAC_RelComparison",300,-1,2);

      m_PFO_FE_ENG_POS_RelComparison=Book1D("_ENG_POS_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_ENG_POS_RelComparison",300,-1,2);
      m_PFO_FE_AVG_LAR_Q_RelComparison=Book1D("_AVG_LAR_Q_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_AVG_LAR_Q_RelComparison",300,-1,2);
      m_PFO_FE_AVG_TILE_Q_RelComparison=Book1D("_AVG_TILE_Q_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_AVG_TILE_Q_RelComparison",300,-1,2);
      m_PFO_FE_EM_PROBABILITY_RelComparison=Book1D("_EM_PROBABILITY_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_EM_PROBABILITY_RelComparison",300,-1,2);
      m_PFO_FE_SECOND_LAMBDA_RelComparison=Book1D("_SECOND_LAMBDA_RelComparison",m_sPFOContainerName+m_sFEContainerName + "_SECOND_LAMBDA_RelComparison",300,-1.0,2);            

      m_PFO_FE_TIMING_RelComparison=Book1D("_TIMING_RelComparison",m_sPFOContainerName+m_sFEContainerName+"_TIMING_RelComparison",400,-2,2);
      
    } // end of NFE specific block
    else{
      //charged pfo/flow element comparison using track matched PFO and FE
      //binning is chosen so that we can be sensitive to small variations around 1, since we expect the retrievals to be close if not identical. -1 lower bound kept to pick up misconfigurations/debug errors
      m_PFO_FE_isInDenseEnvironment_RelComparison=Book1D("_isInDenseEnvironment_RelComparison",m_sPFOContainerName+m_sFEContainerName+"_isInDenseEnvironment_RelComparison",30,-1,2);
      m_PFO_FE_tracksExpectedEnergyDeposit=Book1D("_tracksExpectedEnergyDeposit_RelComparison",m_sPFOContainerName+m_sFEContainerName+"_tracksExpectedEnergyDeposit_RelComparison",300,-1,2);

      

    }// end of charged flow element/pfo specific block


  }
  
  void PFO_FE_Comparison_PlotMaker::Fill(const xAOD::PFO* pfo, const xAOD::FlowElement* fe){
    TLorentzVector fe_tlv=fe->p4();  
    TLorentzVector pfo_tlv=pfo->p4EM(); // default PFO is LC scale, switch to EM scale for comparison
 
    double pfo_pt=pfo_tlv.Pt();
    double pfo_eta=pfo_tlv.Eta();
    double pfo_e=pfo_tlv.E(); // get correct scale
    
    double fe_pt=fe_tlv.Pt();
    double fe_eta=fe_tlv.Eta();
    double fe_e=fe_tlv.E();
    double ptratio=0;
    
    double deltaPt=fe_pt-pfo_pt;
    if(fe_pt>0){
      ptratio = pfo_pt/fe_pt;
    }
    double dR=fe_tlv.DeltaR(pfo_tlv);
    double abs_dEta=std::abs(fe_eta-pfo_eta); 
    double dphi=fe_tlv.DeltaPhi(pfo_tlv);
    double dE=fe_e-pfo_e;
    double ERatio=-9999999999;
    if(pfo_e!=0)
	ERatio=fe_e/pfo_e;

    if(m_isNeutral){
	// debug of scale
	double pfo_e_LC=pfo->p4().E(); // pfo LC scale energy
	double pfo_eEM=pfo->eEM(); // pfo EM scale energy
	ATH_MSG_DEBUG("PFO E(): "<<pfo_e_LC<<"PFO eEM()"<<pfo_eEM<<" FE E(): "<<fe_e);
	if(ERatio>-999999999)
	    ATH_MSG_INFO("Ratio (PFO_e/FE_e): "<<ERatio);
	if(dE!=0)
	    ATH_MSG_INFO("FE_e - PFO_e : "<<dE);
	// assume all edge cases/nullptr catches for clusters were already vetoed
	double fe_clus_e=(fe->otherObjects()).at(0)->p4().E();
	double pfo_clus_e=pfo->cluster(0)->p4().E();
		

	ATH_MSG_INFO("PFO_clus E(): "<<pfo_clus_e<<" FE_clus E(): "<<fe_clus_e);
    }
    // now fill the histos
    
    m_PFO_FE_dE->Fill(dE);
    m_PFO_FE_ptratio->Fill(ptratio);
    m_PFO_FE_delta_pt->Fill(deltaPt);
    m_PFO_FE_delta_R->Fill(dR);
    m_PFO_FE_delta_eta->Fill(abs_dEta);
    m_PFO_FE_delta_phi->Fill(dphi);

    if(m_isNeutral){
      // for the more complex observables, need the following
      // 1) Flow Element -> Retrieve cluster moments from auxdata
      // 2) PFO - > Direct function call
      // 3) Calculate Ratio (excl div0)
      // 4) Dump to plot
      float FE_moment_SECOND_R = -1.0;
      float FE_moment_CENTER_LAMBDA = -1.0;
      float FE_moment_ISOLATION = -1.0;
      float FE_moment_ENG_BAD_CELLS=-1.0;
      float FE_moment_N_BAD_CELLS = -1.0;     
      float FE_moment_BADLARQ_FRAC = -1.0;
      float FE_moment_ENG_POS = -1.0;
      float FE_moment_AVG_LAR_Q = -1.0;
      float FE_moment_AVG_TILE_Q = -1.0; 
      float FE_moment_EM_PROBABILITY = -1.0;
      float FE_moment_SECOND_LAMBDA = -1.0;
      
      float FE_TIMING=-50;

      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_SECOND_R("SECOND_R");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_CENTER_LAMBDA("CENTER_LAMBDA");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_ISOLATION("ISOLATION");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_ENG_BAD_CELLS("ENG_BAD_CELLS");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_N_BAD_CELLS("N_BAD_CELLS");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_BADLARQ_FRAC("BADLARQ_FRAC");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_ENG_POS("ENG_POS");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_AVG_LAR_Q("AVG_LAR_Q");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_AVG_TILE_Q("AVG_TILE_Q");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_EM_PROBABILITY("EM_PROBABILITY");
      static SG::AuxElement::ConstAccessor<float>acc_FE_moment_SECOND_LAMBDA("SECOND_LAMBDA");
      static SG::AuxElement::ConstAccessor<float>acc_FE_TIMING("PF_TIMING");



      if(acc_FE_moment_SECOND_R.isAvailable(*fe))
	FE_moment_SECOND_R=acc_FE_moment_SECOND_R(*fe);
     
     if(acc_FE_moment_CENTER_LAMBDA.isAvailable(*fe))
       FE_moment_CENTER_LAMBDA=acc_FE_moment_CENTER_LAMBDA(*fe);
     
     if(acc_FE_moment_ISOLATION.isAvailable(*fe))
       FE_moment_ISOLATION=acc_FE_moment_ISOLATION(*fe);
     
     if(acc_FE_moment_ENG_BAD_CELLS.isAvailable(*fe))
       FE_moment_ENG_BAD_CELLS=acc_FE_moment_ENG_BAD_CELLS(*fe);
     
     if(acc_FE_moment_N_BAD_CELLS.isAvailable(*fe))
       FE_moment_N_BAD_CELLS=acc_FE_moment_N_BAD_CELLS(*fe);
     
     if(acc_FE_moment_BADLARQ_FRAC.isAvailable(*fe)){
       FE_moment_BADLARQ_FRAC=acc_FE_moment_BADLARQ_FRAC(*fe);
       // this section is a debug of floating point compression
       ATH_MSG_DEBUG("BADLARQ_FRAC "<<FE_moment_BADLARQ_FRAC);

       union{ //using union and bitset to read the binary 
	      //representation of the floating point number (to check compression effects)
	   float input; // 
	   int output;
       } indata;
       indata.input=FE_moment_BADLARQ_FRAC;
       std::bitset<sizeof(float) * CHAR_BIT> bits(indata.output);
       ATH_MSG_DEBUG("BADLARQ_FRAC bits (binary rep) "<<bits<<"");       


     }
     if(acc_FE_moment_ENG_POS.isAvailable(*fe))
       FE_moment_ENG_POS=acc_FE_moment_ENG_POS(*fe);
     
     if(acc_FE_moment_AVG_LAR_Q.isAvailable(*fe))
       FE_moment_AVG_LAR_Q=acc_FE_moment_AVG_LAR_Q(*fe);
     
     if(acc_FE_moment_AVG_TILE_Q.isAvailable(*fe))
       FE_moment_AVG_TILE_Q=acc_FE_moment_AVG_TILE_Q(*fe);
     
     if(acc_FE_moment_EM_PROBABILITY.isAvailable(*fe))
       FE_moment_EM_PROBABILITY=acc_FE_moment_EM_PROBABILITY(*fe);
     
     if(acc_FE_moment_SECOND_LAMBDA.isAvailable(*fe))
       FE_moment_SECOND_LAMBDA=acc_FE_moment_SECOND_LAMBDA(*fe);

     if(acc_FE_TIMING.isAvailable(*fe))
       FE_TIMING=acc_FE_TIMING(*fe);
     // now do the same for PFOs by direct function call

     float pfo_moment_SECOND_R = -1.0;
     pfo->getClusterMoment(pfo_moment_SECOND_R,xAOD::CaloCluster::SECOND_R);
     float pfo_moment_CENTER_LAMBDA = -1.0;
     pfo->getClusterMoment(pfo_moment_CENTER_LAMBDA,xAOD::CaloCluster::CENTER_LAMBDA);
     float pfo_moment_ISOLATION = -1.0;
     pfo->getClusterMoment(pfo_moment_ISOLATION,xAOD::CaloCluster::ISOLATION); 
     float pfo_moment_ENG_BAD_CELLS = -1.0;
     pfo->getClusterMoment(pfo_moment_ENG_BAD_CELLS,xAOD::CaloCluster::ENG_BAD_CELLS);     
     float pfo_moment_N_BAD_CELLS = -1.0;
     pfo->getClusterMoment(pfo_moment_N_BAD_CELLS,xAOD::CaloCluster::N_BAD_CELLS);     
     float pfo_moment_BADLARQ_FRAC = -1.0;
     pfo->getClusterMoment(pfo_moment_BADLARQ_FRAC,xAOD::CaloCluster::BADLARQ_FRAC);     
     float pfo_moment_ENG_POS = -1.0;
     pfo->getClusterMoment(pfo_moment_ENG_POS,xAOD::CaloCluster::ENG_POS);
     float pfo_moment_AVG_LAR_Q = -1.0;
     pfo->getClusterMoment(pfo_moment_AVG_LAR_Q,xAOD::CaloCluster::AVG_LAR_Q);   
     float pfo_moment_AVG_TILE_Q = -1.0;
     pfo->getClusterMoment(pfo_moment_AVG_TILE_Q,xAOD::CaloCluster::AVG_TILE_Q);     
     float pfo_moment_EM_PROBABILITY = -1.0;
     pfo->getClusterMoment(pfo_moment_EM_PROBABILITY,xAOD::CaloCluster::EM_PROBABILITY);     
     float pfo_moment_SECOND_LAMBDA = -1.0;
     pfo->getClusterMoment(pfo_moment_SECOND_LAMBDA,xAOD::CaloCluster::SECOND_LAMBDA);
     float pfo_TIMING=-50;
     bool gotattr=pfo->attribute<float>(xAOD::PFODetails::eflowRec_TIMING,pfo_TIMING);
     if (!gotattr) // if not retrieved, reset to original value if it was changed 
       pfo_TIMING=-50;
       
     // step 3: calculate the ratios
     // Ratio is PFO/FE (in this order)
     // also skip Div0 errors - setting to -1 should suffice to make these debuggable
      float Ratio_PFO_FE_moment_SECOND_R = -1.0;
      if(FE_moment_SECOND_R!=0){
	Ratio_PFO_FE_moment_SECOND_R=pfo_moment_SECOND_R/FE_moment_SECOND_R;
      }
      float Ratio_PFO_FE_moment_CENTER_LAMBDA = -1.0;
      if(FE_moment_CENTER_LAMBDA!=0){
	Ratio_PFO_FE_moment_CENTER_LAMBDA=pfo_moment_CENTER_LAMBDA/FE_moment_CENTER_LAMBDA;
	ATH_MSG_DEBUG("CENTER_LAMBDA: (PFO) "<<pfo_moment_CENTER_LAMBDA<<" (FE) "<<FE_moment_CENTER_LAMBDA);	
      }
      float Ratio_PFO_FE_moment_ISOLATION = -1.0;
      if(FE_moment_ISOLATION!=0){
	Ratio_PFO_FE_moment_ISOLATION=pfo_moment_ISOLATION/FE_moment_ISOLATION;
      }
      float Ratio_PFO_FE_moment_ENG_BAD_CELLS=-1.0;
      if(FE_moment_ENG_BAD_CELLS!=0){
	Ratio_PFO_FE_moment_ENG_BAD_CELLS=pfo_moment_ENG_BAD_CELLS/FE_moment_ENG_BAD_CELLS;
      }
      float Ratio_PFO_FE_moment_N_BAD_CELLS = -1.0;     
      if(FE_moment_N_BAD_CELLS!=0){
	Ratio_PFO_FE_moment_N_BAD_CELLS=pfo_moment_N_BAD_CELLS/FE_moment_N_BAD_CELLS;
      }

      float Ratio_PFO_FE_moment_BADLARQ_FRAC = -1.0;
      if(FE_moment_BADLARQ_FRAC!=0){
	Ratio_PFO_FE_moment_BADLARQ_FRAC=pfo_moment_BADLARQ_FRAC/FE_moment_BADLARQ_FRAC;
	ATH_MSG_DEBUG("BADLARQ_FRAC: (PFO) "<<pfo_moment_BADLARQ_FRAC<<" (FE) "<<FE_moment_BADLARQ_FRAC);
      }
      float Ratio_PFO_FE_moment_ENG_POS = -1.0;
      if(FE_moment_ENG_POS!=0){
	Ratio_PFO_FE_moment_ENG_POS=pfo_moment_ENG_POS/FE_moment_ENG_POS;
      }
      float Ratio_PFO_FE_moment_AVG_LAR_Q = -1.0;
      if(FE_moment_AVG_LAR_Q){
	Ratio_PFO_FE_moment_AVG_LAR_Q=pfo_moment_AVG_LAR_Q/FE_moment_AVG_LAR_Q;
	ATH_MSG_DEBUG("AVG_LAR_Q: (PFO) "<<pfo_moment_AVG_LAR_Q<<" (FE) "<<FE_moment_AVG_LAR_Q);
      }
      float Ratio_PFO_FE_moment_AVG_TILE_Q = -1.0; 
      if(FE_moment_AVG_TILE_Q){
	Ratio_PFO_FE_moment_AVG_TILE_Q=pfo_moment_AVG_TILE_Q/FE_moment_AVG_TILE_Q;
      }
      float Ratio_PFO_FE_moment_EM_PROBABILITY = -1.0;
      if(FE_moment_EM_PROBABILITY!=0){
	Ratio_PFO_FE_moment_EM_PROBABILITY=pfo_moment_EM_PROBABILITY/FE_moment_EM_PROBABILITY;
      }
      
      float Ratio_PFO_FE_moment_SECOND_LAMBDA = -1.0;
      if(FE_moment_SECOND_LAMBDA!=0){
	Ratio_PFO_FE_moment_SECOND_LAMBDA=pfo_moment_SECOND_LAMBDA/FE_moment_SECOND_LAMBDA;
      }
      float Ratio_PFO_FE_TIMING=-2.0; // specific default value given interval of [-50,50]
      if (FE_TIMING!=0){
	Ratio_PFO_FE_TIMING=pfo_TIMING/FE_TIMING;
      }
      // Step 4: Dump


      m_PFO_FE_SECOND_R_RelComparison->Fill(Ratio_PFO_FE_moment_SECOND_R);
      m_PFO_FE_CENTER_LAMBDA_RelComparison->Fill(Ratio_PFO_FE_moment_CENTER_LAMBDA);
      m_PFO_FE_ISOLATION_RelComparison->Fill(Ratio_PFO_FE_moment_ISOLATION);
      m_PFO_FE_ENG_BAD_CELLS_RelComparison->Fill(Ratio_PFO_FE_moment_ENG_BAD_CELLS);
      m_PFO_FE_N_BAD_CELLS_RelComparison->Fill(Ratio_PFO_FE_moment_N_BAD_CELLS);
      m_PFO_FE_BADLARQ_FRAC_RelComparison->Fill(Ratio_PFO_FE_moment_BADLARQ_FRAC);
      m_PFO_FE_ENG_POS_RelComparison->Fill(Ratio_PFO_FE_moment_ENG_POS);
      m_PFO_FE_AVG_LAR_Q_RelComparison->Fill(Ratio_PFO_FE_moment_AVG_LAR_Q);
      m_PFO_FE_AVG_TILE_Q_RelComparison->Fill(Ratio_PFO_FE_moment_AVG_TILE_Q);
      m_PFO_FE_EM_PROBABILITY_RelComparison->Fill(Ratio_PFO_FE_moment_EM_PROBABILITY);
      m_PFO_FE_SECOND_LAMBDA_RelComparison->Fill(Ratio_PFO_FE_moment_SECOND_LAMBDA);      
      m_PFO_FE_TIMING_RelComparison->Fill(Ratio_PFO_FE_TIMING);
      
    } // end of NFE specific block
    else{
      // Charged Flow Element specific plots
      // for the more complex observables, need the following
      // 1) Flow Element -> Retrieve from auxdata
      // 2) PFO - > Direct function call
      // 3) Calculate Ratio (excl div0)
      // 4) Dump to plot     

      // Step 1: CFE
      static SG::AuxElement::ConstAccessor<int> acc_FE_isInDenseEnvironment("IsInDenseEnvironment");
      static SG::AuxElement::ConstAccessor<float> acc_FE_tracksExpectedEnergyDeposit("TracksExpectedEnergyDeposit");

      float FE_isInDenseEnvironment=-1.0;
      float FE_tracksExpectedEnergyDeposit=-1.0;

      if(acc_FE_isInDenseEnvironment.isAvailable(*fe))
	FE_isInDenseEnvironment=acc_FE_isInDenseEnvironment(*fe);
      if(acc_FE_tracksExpectedEnergyDeposit.isAvailable(*fe))
	FE_tracksExpectedEnergyDeposit=acc_FE_tracksExpectedEnergyDeposit(*fe);
      // Step 2: cPFO
      xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
      int pfo_isInDenseEnvironment=false; // boolean false = 0 
      bool gotattr=pfo->attribute(myAttribute_isInDenseEnvironment,pfo_isInDenseEnvironment);
      if(!gotattr) // reset to default value
	pfo_isInDenseEnvironment=0;
      float pfo_tracksExpectedEnergyDeposit=-1.0;
      xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
      gotattr = pfo->attribute(myAttribute_tracksExpectedEnergyDeposit,pfo_tracksExpectedEnergyDeposit);
      if(!gotattr) // reset to default value if it changes
	pfo_tracksExpectedEnergyDeposit=-1.0;
      
      // Step 3: Calculate Ratio
      float Ratio_PFO_FE_isInDenseEnvironment=-1.0;
      if(FE_isInDenseEnvironment!=0){
	Ratio_PFO_FE_isInDenseEnvironment=pfo_isInDenseEnvironment/FE_isInDenseEnvironment;
	ATH_MSG_DEBUG("isInDenseEnvironment: (pfo) "<<pfo_isInDenseEnvironment<<" (FE) "<<FE_isInDenseEnvironment);
      }
      float Ratio_PFO_FE_tracksExpectedEnergyDeposit=-1.0;
      if(FE_tracksExpectedEnergyDeposit!=0){
	
	Ratio_PFO_FE_tracksExpectedEnergyDeposit=pfo_tracksExpectedEnergyDeposit/FE_tracksExpectedEnergyDeposit;
	ATH_MSG_DEBUG("tracksExpectedEnergyDeposit: (pfo) "<<pfo_tracksExpectedEnergyDeposit<<" (FE) "<<FE_tracksExpectedEnergyDeposit);
      }
      // Step 4: Dump
      m_PFO_FE_isInDenseEnvironment_RelComparison->Fill(Ratio_PFO_FE_isInDenseEnvironment);
      m_PFO_FE_tracksExpectedEnergyDeposit->Fill(Ratio_PFO_FE_tracksExpectedEnergyDeposit);  
    }
  } // end of fill function
}// end of PFO namespace
