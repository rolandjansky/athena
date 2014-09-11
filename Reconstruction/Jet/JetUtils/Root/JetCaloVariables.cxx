/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// #include "GaudiKernel/MsgStream.h"
// #include "GaudiKernel/Algorithm.h"
// #include "GaudiKernel/ListItem.h"
// #include "GaudiKernel/IToolSvc.h"
// #include "GaudiKernel/PropertyMgr.h"
// #include "GaudiKernel/IMessageSvc.h"
// #include "GaudiKernel/StatusCode.h"
// #include "StoreGate/StoreGate.h"

// #include "PathResolver/PathResolver.h"

// #include "EventKernel/SignalStateHelper.h"
// #include "JetEvent/Jet.h"

// //#include "JetUtils/JetCaloHelper.h"

// #include "JetUtils/JetCaloVariables.h"

// #include "TAxis.h"
// #include "TFile.h"
// #include "TH1.h"
// #include "TH2.h"
// #include <cmath>
// #include <functional>


// /////////////////////////////////////////////////////////////////////////////////////
// JetCaloVariables::JetCaloVariables(const std::string& m_JetContainerName){
//   cosmicPdf_EMFraction.clear();
//   dijetPdf_EMFraction.clear();
//   cosmicPdf_RconeANDRatio.clear();
//   dijetPdf_RconeANDRatio.clear();
//   ptbin_pdf.clear();
//   filePDF=NULL;
//   JetContainerName=m_JetContainerName; 
// }

// JetCaloVariables::~JetCaloVariables(){
//   for(unsigned int i=0; i<cosmicPdf_EMFraction.size(); i++){
//     if(cosmicPdf_EMFraction[i]!=NULL)    delete cosmicPdf_EMFraction[i];
//     if(dijetPdf_EMFraction[i]!=NULL)     delete dijetPdf_EMFraction[i];
//     if(cosmicPdf_RconeANDRatio[i]!=NULL) delete cosmicPdf_RconeANDRatio[i];
//     if(dijetPdf_RconeANDRatio[i]!=NULL)  delete dijetPdf_RconeANDRatio[i];
//   }
//   if(filePDF!=NULL) delete filePDF;
// }


// StatusCode JetCaloVariables::getpdf(){
//   Athena::IMessageSvcHolder ims;   
//   MsgStream mLog(ims, "JetCaloVariables");
//   //////////////////////////////////////////////////////////////////////////////
//   ptbin_pdf.clear();
//   ptbin_pdf.push_back(40);
//   ptbin_pdf.push_back(80);
//   ptbin_pdf.push_back(160);
//   //////////////////////////////////////////////////////////////////////////////
//   cosmicPdf_EMFraction.clear();
//   dijetPdf_EMFraction.clear();
//   cosmicPdf_RconeANDRatio.clear();
//   dijetPdf_RconeANDRatio.clear();

//   // Find the path to inputFile
//   std::string fname = PathResolver::find_file("cos_LLR_pdf.root", "DATAPATH");
//   if(fname==""){
//     mLog << MSG::ERROR << "Could not find input file " << fname << endreq;
//     return StatusCode::FAILURE;
//   }

//   filePDF = new TFile(fname.c_str());
//   mLog << MSG::INFO << "The input file is " << fname.c_str() << endreq;
//   if(filePDF->IsOpen()){
//      if(JetContainerName.compare("Cone4H1TopoJets")==0){
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin0_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin1_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin2_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin3_s_pdf_more"));

//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin0_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin1_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin2_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin3_s_pdf_more"));

//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin0_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin1_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin2_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("cone4topo_JetEMf_bin3_b_pdf_more"));

//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin0_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin1_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin2_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("cone4topo_Rcone_Ratio_bin3_b_pdf_more"));
//      }else{ 
//        // we temporary use the pdf obtained with AntiKt4H1TopoJets for all other kinds of jets.//
//        // These pdf should be updated later.                                                   //
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin0_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin1_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin2_s_pdf_more"));
//        cosmicPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin3_s_pdf_more"));

//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin0_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin1_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin2_s_pdf_more"));
//        cosmicPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin3_s_pdf_more"));

//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin0_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin1_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin2_b_pdf_more"));
//        dijetPdf_EMFraction.push_back((TH1D*)filePDF->Get("antikt4topo_JetEMf_bin3_b_pdf_more"));

//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin0_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin1_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin2_b_pdf_more"));
//        dijetPdf_RconeANDRatio.push_back((TH2D*)filePDF->Get("antikt4topo_Rcone_Ratio_bin3_b_pdf_more"));
//      }
//   }else{
//     mLog << MSG::ERROR << "Could not open file " << fname << endreq;
//     return StatusCode::FAILURE;
//   }
//   /////////////////////////////////////////////////////////////////////////
//   //for(int i=0; i<int(cosmicPdf_EMFraction.size()); i++)
//   //mLog << MSG::INFO <<cosmicPdf_EMFraction[i]->GetName()<<":"<<cosmicPdf_EMFraction[i]->GetSumOfWeights()<<endreq;

//   //for(int i=0; i<int(cosmicPdf_RconeANDRatio.size()); i++)
//   //mLog << MSG::INFO <<cosmicPdf_RconeANDRatio[i]->GetName()<<":"<<cosmicPdf_RconeANDRatio[i]->GetSumOfWeights()<<endreq;

//   //for(int i=0; i<int(dijetPdf_EMFraction.size()); i++)
//   //mLog << MSG::INFO <<dijetPdf_EMFraction[i]->GetName()<<":"<<dijetPdf_EMFraction[i]->GetSumOfWeights()<<endreq;

//   //for(int i=0; i<int(dijetPdf_RconeANDRatio.size()); i++)
//   //mLog << MSG::INFO <<dijetPdf_RconeANDRatio[i]->GetName()<<":"<<dijetPdf_RconeANDRatio[i]->GetSumOfWeights()<<endreq;

//   return StatusCode::SUCCESS;
// }


// double JetCaloVariables::compute_RCone(const Jet* jet){
//   // double sumE_cells=0;
//   // double rcone=0;


//   // SignalStateHelper emscale_h(jet, P4SignalState::JETEMSCALE);//Set the jet to EMSCALE

//   // double em_eta=jet->eta();
//   // double em_phi=jet->phi();
//   // emscale_h.resetSignalState(); //Set it back to the original state

//   // NavigationToken<CaloCell,double> cellToken;
//   // jet->fillToken(cellToken,double(1.));
//   // if ( cellToken.size() > 0 ){
//   //   NavigationToken<CaloCell,double>::const_iterator fCell = cellToken.begin();
//   //   NavigationToken<CaloCell,double>::const_iterator lCell = cellToken.end();
//   //   for( ; fCell != lCell; fCell++){
//   //      if( (*fCell)->e()>0 ){
//   //        sumE_cells+=(*fCell)->e();
//   //        double deta = ((*fCell)->eta()-em_eta);
//   //        double dphi = fabs((*fCell)->phi()-em_phi);
//   //        if(dphi>3.1415)dphi=2*3.1415-dphi;
//   //        double dr = sqrt(deta*deta+dphi*dphi);
//   //        rcone+=dr*fabs((*fCell)->e());
//   //      }
//   //   }
//   //   if(sumE_cells!=0) rcone=rcone/sumE_cells;
//   // }
//   // return rcone;
//   return 0;
// }


// double JetCaloVariables::compute_RatioLeadingCells(const Jet* jet){
// //------------------------------------------
// // for the definition of Ratio_LeadingCells
// //------------------------------------------
//  // double ratio_leading_cells=0;
//  // double sumE_cells=0;
//  // std::vector<double> cell_energies_em; cell_energies_em.clear();
//  // std::vector<double> cell_energies_had; cell_energies_had.clear();
//  // NavigationToken<CaloCell,double> cellToken;
//  // jet->fillToken(cellToken,double(1.));
//  // if ( cellToken.size() > 0 ){
//  //    NavigationToken<CaloCell,double>::const_iterator fCell = cellToken.begin();
//  //    NavigationToken<CaloCell,double>::const_iterator lCell = cellToken.end();
//  //    for( ; fCell != lCell; fCell++){
//  //       if( (*fCell)->e()>0 ){
//  //         sumE_cells+=(*fCell)->e();
//  //         if( (*fCell)->caloDDE()->getSubCalo()==0 ){ //0=EM
//  //           cell_energies_em.push_back((*fCell)->e());
//  //         }else{
//  //           cell_energies_had.push_back((*fCell)->e());
//  //         }
//  //       }
//  //    }
//  //    std::sort(cell_energies_had.rbegin(),cell_energies_had.rend());
//  //    for(unsigned int i=0; i<cell_energies_had.size()&&i<2; i++){
//  //       ratio_leading_cells+=cell_energies_had[i];
//  //    }
//  //    std::sort(cell_energies_em.rbegin(),cell_energies_em.rend());
//  //    for(unsigned int i=0; i<cell_energies_em.size()&&i<32; i++){
//  //       ratio_leading_cells+=cell_energies_em[i];
//  //    }
//  //    if(sumE_cells!=0) ratio_leading_cells=ratio_leading_cells/sumE_cells;
//  // }
//   //  return ratio_leading_cells;
//   return 0;
// }


// double JetCaloVariables::compute_LLREmFraction(const Jet* jet){
//   // Athena::IMessageSvcHolder ims;    
//   // MsgStream mLog(ims, "JetCaloVariables");

//   // //--- to get pdf ---
//   // if(filePDF==NULL){
//   //    StatusCode sc = getpdf();
//   //    if(sc.isFailure()) mLog << MSG::WARNING << "Some problems with getpdf()! " << endreq;
//   // }

//   // //--- to get emfraction and em_pt ---
//   // double emfraction=JetCaloHelper::jetEMFraction(jet);

//   // SignalStateHelper emscale_h(jet, P4SignalState::JETEMSCALE);//Set the jet to EMSCALE
//   // double em_pt=jet->pt();
//   // double em_eta=jet->eta();
//   // emscale_h.resetSignalState(); //Set it back to the original state

//   // //mLog << MSG::DEBUG << "compute_LLREmFraction: em_pt = " <<em_pt<< endreq;

//   // if(em_pt/1000.<20. || fabs(em_eta)>2.5) return -20.;

//   // int iptbin=0;
//   // //em_pt: MeV, ptbin_pdf[iptbin] : GeV
//   // while(ptbin_pdf[iptbin]<em_pt/1000.&&iptbin<int(ptbin_pdf.size())){
//   //   iptbin++;
//   // }
//   // //mLog << MSG::INFO << "compute_LLREmFraction iptbin = " <<iptbin<< endreq;
//   // TH1* hist_cos=cosmicPdf_EMFraction[iptbin];
//   // TH1* hist_dij=dijetPdf_EMFraction[iptbin];

//   // double xmax=hist_cos->GetXaxis()->GetXmax(); 
//   // double xmin=hist_cos->GetXaxis()->GetXmin();
//   // double emfra=emfraction;
//   // if(emfra>xmax){
//   //   emfra=xmax-0.001;
//   // }else if(emfra<xmin){
//   //   emfra=xmin+0.001;
//   // }     
//   // double sig = hist_cos->Interpolate(emfra);
//   // double bkg = hist_dij->Interpolate(emfra); 

//   // if(sig!=0&&bkg!=0){
//   //   return log(sig/bkg);
//   // }else if(sig==0){
//   //   return -20;
//   // }else{
//   //   return 20; 
//   // }
//   return 0;
// }


// double JetCaloVariables::compute_LLRRconeANDRatio(const Jet* jet){
//   // Athena::IMessageSvcHolder ims;    
//   // MsgStream mLog(ims, "JetCaloVariables");

//   // //--- to get pdf ---
//   // if(filePDF==NULL){
//   //    StatusCode sc = getpdf();
//   //    if(sc.isFailure()) mLog << MSG::WARNING << "Some problems with getpdf()! " << endreq;
//   // }

//   // //--- to get rcone, ratio_leadingcells and em_pt ---
//   // double rcone=compute_RCone(jet);
//   // double ratio=compute_RatioLeadingCells(jet);

//   // SignalStateHelper emscale_h(jet, P4SignalState::JETEMSCALE);//Set the jet to EMSCALE
//   // double em_pt=jet->pt();
//   // double em_eta=jet->eta();
//   // emscale_h.resetSignalState(); //Set it back to the original state
//   // //mLog << MSG::DEBUG << "compute_LLRRconeANDRatio: em_pt = " <<em_pt<< endreq;

//   //   if(em_pt/1000.<20. || fabs(em_eta)>2.5) return -20.;

//   // int iptbin=0;
//   // //pt: MeV, ptbin_pdf[iptbin] : GeV
//   // while(ptbin_pdf[iptbin]<em_pt/1000.&&iptbin<int(ptbin_pdf.size())){
//   //   iptbin++;
//   // }

//   // //mLog << MSG::INFO << "compute_LLRLeading iptbin = " <<iptbin<< endreq;
//   // TH2* hist_cos=cosmicPdf_RconeANDRatio[iptbin];
//   // TH2* hist_dij=dijetPdf_RconeANDRatio[iptbin];
//   // double xmax=hist_cos->GetXaxis()->GetXmax();
//   // double ymax=hist_cos->GetYaxis()->GetXmax();
//   // double xmin=hist_cos->GetXaxis()->GetXmin();
//   // double ymin=hist_cos->GetYaxis()->GetXmin();
//   // double rconetmp=rcone;
//   // double ratiotmp=ratio;
//   // if(rconetmp>xmax){
//   //   rconetmp=xmax-0.001;
//   // }else if(rconetmp<xmin){
//   //   rconetmp=xmin+0.001;
//   // }
//   // if(ratiotmp>ymax){
//   //  ratiotmp=ymax-0.001;
//   // }else if(ratiotmp<ymin){
//   //  ratiotmp=ymin+0.001;
//   // }

//   // double sig=hist_cos->Interpolate(rconetmp,ratiotmp);
//   // double bkg=hist_dij->Interpolate(rconetmp,ratiotmp);
//   // //mLog << MSG::INFO <<"LLRLeading = "<<log(sig/bkg)<< endreq;

//   // if(sig!=0&&bkg!=0){
//   //   return log(sig/bkg);
//   // }else if(sig==0){
//   //   return -20;
//   // }else{
//   //   return 100;
//   // }
//   return 0;
// }


// double JetCaloVariables::compute_jetTime(const Jet* jet){
//  // double sumE_cells=0;
//  // double jettime=0;
//  // NavigationToken<CaloCell,double> cellToken;
//  // jet->fillToken(cellToken,double(1.));
//  // if ( cellToken.size() > 0 ){
//  //    NavigationToken<CaloCell,double>::const_iterator fCell = cellToken.begin();
//  //    NavigationToken<CaloCell,double>::const_iterator lCell = cellToken.end();
//  //    for( ; fCell != lCell; fCell++){
//  //       sumE_cells+=(*fCell)->e();
//  //       double tmptime=(*fCell)->time();
//  //       jettime+=tmptime*((*fCell)->e());
//  //    }
//  //    if(sumE_cells!=0) jettime=jettime/sumE_cells;
//  // }else{
//  //   jettime=-900.;
//  // }
//  // return jettime;
//   return 0.;
// }
