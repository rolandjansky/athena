/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
   gStyle->SetTitleX(0.13);
   gStyle->SetTitleY(0.98);
   gStyle->SetStatX(0.90);
   gStyle->SetStatY(0.95);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.10);
   
   
   TCanvas *c1 = new TCanvas("c1","MuonHits_MDT",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   


   c1->Divide(2,3);   
   c1->cd(1);
   TFile tmp4("./MuonHitsVal_1200_Rpp_50GeV.aan.root", "READ");     
   MDTStName4 = (TTree *)tmp4->Get("/MDTHitsValidation/MDTTestHits");
   MDTStName4->SetLineColor(6);   
   MDTStName4->Draw("m_Validation_MDT_StationName");
   TFile tmp1("./MuonHitsVal_1150_R_50GeV.aan.root", "READ");   
   MDTStName1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTStName1->Draw("m_Validation_MDT_StationName","","same");     
   TFile tmp2("./MuonHitsVal_1200_R_50GeV.aan.root", "READ");     
   MDTStName2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTStName2->SetLineColor(2);   
   MDTStName2->Draw("m_Validation_MDT_StationName","","same");
   TFile tmp3("./MuonHitsVal_1201_R_50GeV.aan.root", "READ");     
   MDTStName3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTStName3->SetLineColor(4);   
   MDTStName3->Draw("m_Validation_MDT_StationName","","same");
//    TFile tmp4("./MuonHitsVal_1200_Rpp_50GeV.aan.root", "READ");     
//    MDTStName4 = (TTree *)tmp4->Get("/MDTHitsValidation/MDTTestHits");
//    MDTStName4->SetLineColor(6);   
//    MDTStName4->Draw("m_Validation_MDT_StationName","","same");
   TFile tmp5("./MuonHitsVal_1201_Rpp_50GeV.aan.root", "READ");     
   MDTStName5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTStName5->SetLineColor(8);   
   MDTStName5->Draw("m_Validation_MDT_StationName","","same");


   c1->cd(2);  
   MDTStEta1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTStEta1->Draw("m_Validation_MDT_StationEta");        
   MDTStEta2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTStEta2->SetLineColor(2);	
   MDTStEta2->Draw("m_Validation_MDT_StationEta","","same");     
   MDTStEta3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTStEta3->SetLineColor(4);	
   MDTStEta3->Draw("m_Validation_MDT_StationEta","","same");  
   MDTStEta4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTStEta4->SetLineColor(6);	
   MDTStEta4->Draw("m_Validation_MDT_StationEta","","same");  
   MDTStEta5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTStEta5->SetLineColor(8);	
   MDTStEta5->Draw("m_Validation_MDT_StationEta","","same");  

   
   c1->cd(3);   
   MDTStPhi1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTStPhi1->Draw("m_Validation_MDT_StationPhi");        
   MDTStPhi2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTStPhi2->SetLineColor(2);	
   MDTStPhi2->Draw("m_Validation_MDT_StationPhi","","same");     
   MDTStPhi3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTStPhi3->SetLineColor(4);	
   MDTStPhi3->Draw("m_Validation_MDT_StationPhi","","same");  
   MDTStPhi4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTStPhi4->SetLineColor(6);	
   MDTStPhi4->Draw("m_Validation_MDT_StationPhi","","same");  
   MDTStPhi5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTStPhi5->SetLineColor(8);	
   MDTStPhi5->Draw("m_Validation_MDT_StationPhi","","same");    
   

   c1->cd(4);   
   MDTIDml4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDml4->SetLineColor(6);	
   MDTIDml4->Draw("m_Validation_MDT_IDMultiLayer");
   MDTIDml1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDml1->Draw("m_Validation_MDT_IDMultiLayer","","same");        
   MDTIDml2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDml2->SetLineColor(2);	
   MDTIDml2->Draw("m_Validation_MDT_IDMultiLayer","","same");     
   MDTIDml3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDml3->SetLineColor(4);	
   MDTIDml3->Draw("m_Validation_MDT_IDMultiLayer","","same");  
//    MDTIDml4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
//    MDTIDml4->SetLineColor(6);	
//    MDTIDml4->Draw("m_Validation_MDT_IDMultiLayer","","same");  
   MDTIDml5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDml5->SetLineColor(8);	
   MDTIDml5->Draw("m_Validation_MDT_IDMultiLayer","","same");     

   
   c1->cd(5);   
   MDTIDl4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDl4->SetLineColor(6);      
   MDTIDl4->Draw("m_Validation_MDT_IDLayer");
   MDTIDl1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDl1->Draw("m_Validation_MDT_IDLayer","","same");        
   MDTIDl2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDl2->SetLineColor(2);      
   MDTIDl2->Draw("m_Validation_MDT_IDLayer","","same");     
   MDTIDl3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDl3->SetLineColor(4);      
   MDTIDl3->Draw("m_Validation_MDT_IDLayer","","same");  
//    MDTIDl4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
//    MDTIDl4->SetLineColor(6);      
//    MDTIDl4->Draw("m_Validation_MDT_IDLayer","","same");  
   MDTIDl5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDl5->SetLineColor(8);      
   MDTIDl5->Draw("m_Validation_MDT_IDLayer","","same");   

   
   c1->cd(6);   
   MDTIDt1 = (TTree *)tmp1->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDt1->Draw("m_Validation_MDT_IDTube");	 
   MDTIDt2 = (TTree *)tmp2->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDt2->SetLineColor(2);      
   MDTIDt2->Draw("m_Validation_MDT_IDTube","","same");	
   MDTIDt3 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDt3->SetLineColor(4);      
   MDTIDt3->Draw("m_Validation_MDT_IDTube","","same");  
   MDTIDt4 = (TTree *)tmp3->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDt4->SetLineColor(6);      
   MDTIDt4->Draw("m_Validation_MDT_IDTube","","same");  
   MDTIDt5 = (TTree *)tmp5->Get("/MDTHitsValidation/MDTTestHits");
   MDTIDt5->SetLineColor(8);      
   MDTIDt5->Draw("m_Validation_MDT_IDTube","","same");      




   TCanvas *c2 = new TCanvas("c2","MuonHits_RPC",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   


   c2->Divide(2,4);
   c2->cd(1);
   TFile tmp1("./MuonHitsVal_1150_R_50GeV.aan.root", "READ");   
   RPCStName1 = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCStName1->Draw("m_Validation_RPC_StationName");     
   TFile tmp2("./MuonHitsVal_1200_R_50GeV.aan.root", "READ");     
   RPCStName2 = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCStName2->SetLineColor(2);   
   RPCStName2->Draw("m_Validation_RPC_StationName","","same");
   TFile tmp3("./MuonHitsVal_1201_R_50GeV.aan.root", "READ");     
   RPCStName3 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCStName3->SetLineColor(4);   
   RPCStName3->Draw("m_Validation_RPC_StationName","","same");
   TFile tmp4("./MuonHitsVal_1200_Rpp_50GeV.aan.root", "READ");     
   RPCStName4 = (TTree *)tmp4->Get("/RPCHitsValidation/RPCTestHits");
   RPCStName4->SetLineColor(6);   
   RPCStName4->Draw("m_Validation_RPC_StationName","","same");
   TFile tmp5("./MuonHitsVal_1201_Rpp_50GeV.aan.root", "READ");     
   RPCStName5 = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCStName5->SetLineColor(8);   
   RPCStName5->Draw("m_Validation_RPC_StationName","","same");


   c2->cd(2);  
   RPCStEta1 = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCStEta1->Draw("m_Validation_RPC_StationEta");        
   RPCStEta2 = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCStEta2->SetLineColor(2);	
   RPCStEta2->Draw("m_Validation_RPC_StationEta","","same");     
   RPCStEta3 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCStEta3->SetLineColor(4);	
   RPCStEta3->Draw("m_Validation_RPC_StationEta","","same");  
   RPCStEta4 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCStEta4->SetLineColor(6);	
   RPCStEta4->Draw("m_Validation_RPC_StationEta","","same");  
   RPCStEta5 = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCStEta5->SetLineColor(8);	
   RPCStEta5->Draw("m_Validation_RPC_StationEta","","same");  


   c2->cd(3);   
   RPCStPhi1 = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCStPhi1->Draw("m_Validation_RPC_StationPhi");        
   RPCStPhi2 = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCStPhi2->SetLineColor(2);	
   RPCStPhi2->Draw("m_Validation_RPC_StationPhi","","same");     
   RPCStPhi3 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCStPhi3->SetLineColor(4);	
   RPCStPhi3->Draw("m_Validation_RPC_StationPhi","","same");  
   RPCStPhi4 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCStPhi4->SetLineColor(6);	
   RPCStPhi4->Draw("m_Validation_RPC_StationPhi","","same");  
   RPCStPhi5 = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCStPhi5->SetLineColor(8);	
   RPCStPhi5->Draw("m_Validation_RPC_StationPhi","","same");    


   c2->cd(4);   
   RPCGG1 = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCGG1->Draw("m_Validation_RPC_DoubletGasGap");        
   RPCGG2 = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCGG2->SetLineColor(2);   
   RPCGG2->Draw("m_Validation_RPC_DoubletGasGap","","same");     
   RPCGG3 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCGG3->SetLineColor(4);   
   RPCGG3->Draw("m_Validation_RPC_DoubletGasGap","","same");  
   RPCGG4 = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCGG4->SetLineColor(6);   
   RPCGG4->Draw("m_Validation_RPC_DoubletGasGap","","same");  
   RPCGG5 = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCGG5->SetLineColor(8);   
   RPCGG5->Draw("m_Validation_RPC_DoubletGasGap","","same");     


   c2->cd(5);   
   RPCdbphi = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbphi->Draw("m_Validation_RPC_DoubletPhi");	    
   RPCdbphi = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbphi->SetLineColor(2);      
   RPCdbphi->Draw("m_Validation_RPC_DoubletPhi","","same");     
   RPCdbphi = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbphi->SetLineColor(4);      
   RPCdbphi->Draw("m_Validation_RPC_DoubletPhi","","same");  
   RPCdbphi = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbphi->SetLineColor(6);      
   RPCdbphi->Draw("m_Validation_RPC_DoubletPhi","","same");  
   RPCdbphi = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbphi->SetLineColor(8);      
   RPCdbphi->Draw("m_Validation_RPC_DoubletPhi","","same");   


   c2->cd(6);   
   RPCdbZ = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbZ->Draw("m_Validation_RPC_DoubletZ");     
   RPCdbZ = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbZ->SetLineColor(2);      
   RPCdbZ->Draw("m_Validation_RPC_DoubletZ","","same");  
   RPCdbZ = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbZ->SetLineColor(4);      
   RPCdbZ->Draw("m_Validation_RPC_DoubletZ","","same");  
   RPCdbZ = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbZ->SetLineColor(6);      
   RPCdbZ->Draw("m_Validation_RPC_DoubletZ","","same");  
   RPCdbZ = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbZ->SetLineColor(8);      
   RPCdbZ->Draw("m_Validation_RPC_DoubletZ","","same");	

   c2->cd(7);   
   RPCdbR = (TTree *)tmp1->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbR->Draw("m_Validation_RPC_DoubletR");	 
   RPCdbR = (TTree *)tmp2->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbR->SetLineColor(2);	
   RPCdbR->Draw("m_Validation_RPC_DoubletR","","same");  
   RPCdbR = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbR->SetLineColor(4);	
   RPCdbR->Draw("m_Validation_RPC_DoubletR","","same");  
   RPCdbR = (TTree *)tmp3->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbR->SetLineColor(6);	
   RPCdbR->Draw("m_Validation_RPC_DoubletR","","same");  
   RPCdbR = (TTree *)tmp5->Get("/RPCHitsValidation/RPCTestHits");
   RPCdbR->SetLineColor(8);	
   RPCdbR->Draw("m_Validation_RPC_DoubletR","","same"); 



   TCanvas *c3 = new TCanvas("c3","MuonHits_CSC",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   c3->Divide(2,2);
   c3->cd(1);
   TFile tmp3("./MuonHitsVal_1201_R_50GeV.aan.root", "READ");     
   CSCStName3 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCStName3->SetLineColor(4);   
   CSCStName3->Draw("m_Validation_CSC_StationName");
   TFile tmp1("./MuonHitsVal_1150_R_50GeV.aan.root", "READ");   
   CSCStName1 = (TTree *)tmp1->Get("/CSCHitsValidation/CSCTestHits");
   CSCStName1->Draw("m_Validation_CSC_StationName","","same");     
   TFile tmp2("./MuonHitsVal_1200_R_50GeV.aan.root", "READ");     
   CSCStName2 = (TTree *)tmp2->Get("/CSCHitsValidation/CSCTestHits");
   CSCStName2->SetLineColor(2);   
   CSCStName2->Draw("m_Validation_CSC_StationName","","same");
//    TFile tmp3("./MuonHitsVal_1201_R_50GeV.aan.root", "READ");     
//    CSCStName3 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
//    CSCStName3->SetLineColor(4);   
//    CSCStName3->Draw("m_Validation_CSC_StationName","","same");
   TFile tmp4("./MuonHitsVal_1200_Rpp_50GeV.aan.root", "READ");     
   CSCStName4 = (TTree *)tmp4->Get("/CSCHitsValidation/CSCTestHits");
   CSCStName4->SetLineColor(6);   
   CSCStName4->Draw("m_Validation_CSC_StationName","","same");
   TFile tmp5("./MuonHitsVal_1201_Rpp_50GeV.aan.root", "READ");     
   CSCStName5 = (TTree *)tmp5->Get("/CSCHitsValidation/CSCTestHits");
   CSCStName5->SetLineColor(8);   
   CSCStName5->Draw("m_Validation_CSC_StationName","","same");


   c3->cd(2);  
   CSCStEta1 = (TTree *)tmp1->Get("/CSCHitsValidation/CSCTestHits");
   CSCStEta1->Draw("m_Validation_CSC_StationEta");        
   CSCStEta2 = (TTree *)tmp2->Get("/CSCHitsValidation/CSCTestHits");
   CSCStEta2->SetLineColor(2);	
   CSCStEta2->Draw("m_Validation_CSC_StationEta","","same");     
   CSCStEta3 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCStEta3->SetLineColor(4);	
   CSCStEta3->Draw("m_Validation_CSC_StationEta","","same");  
   CSCStEta4 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCStEta4->SetLineColor(6);	
   CSCStEta4->Draw("m_Validation_CSC_StationEta","","same");  
   CSCStEta5 = (TTree *)tmp5->Get("/CSCHitsValidation/CSCTestHits");
   CSCStEta5->SetLineColor(8);	
   CSCStEta5->Draw("m_Validation_CSC_StationEta","","same");  


   c3->cd(3);   
   CSCStPhi1 = (TTree *)tmp1->Get("/CSCHitsValidation/CSCTestHits");
   CSCStPhi1->Draw("m_Validation_CSC_StationPhi");        
   CSCStPhi2 = (TTree *)tmp2->Get("/CSCHitsValidation/CSCTestHits");
   CSCStPhi2->SetLineColor(2);	
   CSCStPhi2->Draw("m_Validation_CSC_StationPhi","","same");     
   CSCStPhi3 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCStPhi3->SetLineColor(4);	
   CSCStPhi3->Draw("m_Validation_CSC_StationPhi","","same");  
   CSCStPhi4 = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCStPhi4->SetLineColor(6);	
   CSCStPhi4->Draw("m_Validation_CSC_StationPhi","","same");  
   CSCStPhi5 = (TTree *)tmp5->Get("/CSCHitsValidation/CSCTestHits");
   CSCStPhi5->SetLineColor(8);	
   CSCStPhi5->Draw("m_Validation_CSC_StationPhi","","same");    

 
   c3->cd(4);   
   CSCww = (TTree *)tmp1->Get("/CSCHitsValidation/CSCTestHits");
   CSCww->Draw("m_Validation_CSC_WireLayer");	    
   CSCww = (TTree *)tmp2->Get("/CSCHitsValidation/CSCTestHits");
   CSCww->SetLineColor(2);  
   CSCww->Draw("m_Validation_CSC_WireLayer","","same");     
   CSCww = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCww->SetLineColor(4);  
   CSCww->Draw("m_Validation_CSC_WireLayer","","same");  
   CSCww = (TTree *)tmp3->Get("/CSCHitsValidation/CSCTestHits");
   CSCww->SetLineColor(6);  
   CSCww->Draw("m_Validation_CSC_WireLayer","","same");  
   CSCww = (TTree *)tmp5->Get("/CSCHitsValidation/CSCTestHits");
   CSCww->SetLineColor(8);  
   CSCww->Draw("m_Validation_CSC_WireLayer","","same");    


   
   TCanvas *c4 = new TCanvas("c4","MuonHits_TGC",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   c4->Divide(2,2);
   c4->cd(1);
   TFile tmp1("./MuonHitsVal_1150_R_50GeV.aan.root", "READ");   
   TGCStName1 = (TTree *)tmp1->Get("/TGCHitsValidation/TGCTestHits");
   TGCStName1->Draw("m_Validation_TGC_StName");     
   TFile tmp2("./MuonHitsVal_1200_R_50GeV.aan.root", "READ");     
   TGCStName2 = (TTree *)tmp2->Get("/TGCHitsValidation/TGCTestHits");
   TGCStName2->SetLineColor(2);   
   TGCStName2->Draw("m_Validation_TGC_StName","","same");
   TFile tmp3("./MuonHitsVal_1201_R_50GeV.aan.root", "READ");     
   TGCStName3 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCStName3->SetLineColor(4);   
   TGCStName3->Draw("m_Validation_TGC_StName","","same");
   TFile tmp4("./MuonHitsVal_1200_Rpp_50GeV.aan.root", "READ");     
   TGCStName4 = (TTree *)tmp4->Get("/TGCHitsValidation/TGCTestHits");
   TGCStName4->SetLineColor(6);   
   TGCStName4->Draw("m_Validation_TGC_StName","","same");
   TFile tmp5("./MuonHitsVal_1201_Rpp_50GeV.aan.root", "READ");     
   TGCStName5 = (TTree *)tmp5->Get("/TGCHitsValidation/TGCTestHits");
   TGCStName5->SetLineColor(8);   
   TGCStName5->Draw("m_Validation_TGC_StName","","same");


   c4->cd(2);  
   TGCStEta1 = (TTree *)tmp1->Get("/TGCHitsValidation/TGCTestHits");
   TGCStEta1->Draw("m_Validation_TGC_StationEta");        
   TGCStEta2 = (TTree *)tmp2->Get("/TGCHitsValidation/TGCTestHits");
   TGCStEta2->SetLineColor(2);	
   TGCStEta2->Draw("m_Validation_TGC_StationEta","","same");     
   TGCStEta3 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCStEta3->SetLineColor(4);	
   TGCStEta3->Draw("m_Validation_TGC_StationEta","","same");  
   TGCStEta4 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCStEta4->SetLineColor(6);	
   TGCStEta4->Draw("m_Validation_TGC_StationEta","","same");  
   TGCStEta5 = (TTree *)tmp5->Get("/TGCHitsValidation/TGCTestHits");
   TGCStEta5->SetLineColor(8);	
   TGCStEta5->Draw("m_Validation_TGC_StationEta","","same");  


   c4->cd(3);   
   TGCStPhi1 = (TTree *)tmp1->Get("/TGCHitsValidation/TGCTestHits");
   TGCStPhi1->Draw("m_Validation_TGC_StationPhi");        
   TGCStPhi2 = (TTree *)tmp2->Get("/TGCHitsValidation/TGCTestHits");
   TGCStPhi2->SetLineColor(2);	
   TGCStPhi2->Draw("m_Validation_TGC_StationPhi","","same");     
   TGCStPhi3 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCStPhi3->SetLineColor(4);	
   TGCStPhi3->Draw("m_Validation_TGC_StationPhi","","same");  
   TGCStPhi4 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCStPhi4->SetLineColor(6);	
   TGCStPhi4->Draw("m_Validation_TGC_StationPhi","","same");  
   TGCStPhi5 = (TTree *)tmp5->Get("/TGCHitsValidation/TGCTestHits");
   TGCStPhi5->SetLineColor(8);	
   TGCStPhi5->Draw("m_Validation_TGC_StationPhi","","same");    


   c4->cd(4);     
   TGCGG4 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCGG4->SetLineColor(6);
   TGCGG4->Draw("m_Validation_TGC_GasGap");   
   TGCGG1 = (TTree *)tmp1->Get("/TGCHitsValidation/TGCTestHits");
   TGCGG1->Draw("m_Validation_TGC_GasGap","","same");        
   TGCGG2 = (TTree *)tmp2->Get("/TGCHitsValidation/TGCTestHits");
   TGCGG2->SetLineColor(2);   
   TGCGG2->Draw("m_Validation_TGC_GasGap","","same");     
   TGCGG3 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
   TGCGG3->SetLineColor(4);   
   TGCGG3->Draw("m_Validation_TGC_GasGap","","same");  
//    TGCGG4 = (TTree *)tmp3->Get("/TGCHitsValidation/TGCTestHits");
//    TGCGG4->SetLineColor(6);   
//    TGCGG4->Draw("m_Validation_TGC_GasGap","","same");  
   TGCGG5 = (TTree *)tmp5->Get("/TGCHitsValidation/TGCTestHits");
   TGCGG5->SetLineColor(8);   
   TGCGG5->Draw("m_Validation_TGC_GasGap","","same");     





}

