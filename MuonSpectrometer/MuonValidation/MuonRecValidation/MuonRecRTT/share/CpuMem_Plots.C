{
// CpuMem_Plots.C
// 
// Make simple plots

// open ntuple file
TChain tc("CollectionTree");
tc.Add("ntuple.root");

// create histograms
gROOT->SetStyle("Plain");
gStyle->SetOptStat(111111);

// Prepare output plots

TCanvas *c1 = new TCanvas("c1","VMemory.C");
c1->Divide(1,1);
c1.cd(1);
tc.Draw("MemVSize:Event","","line");
c1.Update();
c1.SaveAs("Memory_vs_evt.gif");

//TPostScript psFile("TimeConsumption.ps",112);

// TCanvas *c1 = new TCanvas("c1","TimeRdoToPrepData0.C");
// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeCscRdoToCscPrepData");
// c1.cd(2);
// tc.Draw("TimeMdtRdoToMdtPrepData");
// c1.cd(3);
// tc.Draw("TimeRpcRdoToRpcPrepData");
// c1.cd(4);
// tc.Draw("TimeTgcRdoToTgcPrepData");
// c1.Update();
// c1.SaveAs("TimeRdoToPrepData0.gif");

// //TCanvas *c1 = new TCanvas("c1","TimeRdoToPrepData1.C");
// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeCscRdoToCscPrepData","Event>0");
// c1.cd(2);
// tc.Draw("TimeMdtRdoToMdtPrepData","Event>0");
// c1.cd(3);
// tc.Draw("TimeRpcRdoToRpcPrepData","Event>0");
// c1.cd(4);
// tc.Draw("TimeTgcRdoToTgcPrepData","Event>0");
// c1.Update();
// c1.SaveAs("TimeRdoToPrepData1.gif");

// //TCanvas *c1 = new TCanvas("c1","TimeMoore.C");
// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMooSegmentMaker");
// c1.cd(2);
// tc.Draw("TimeMuonCombiTrackBuilder");
// c1.cd(3);
// tc.Draw("TimeMuidStandAlone");
// c1.cd(4);
// tc.Draw("TimeMuidCombined");
// c1.Update();
// c1.SaveAs("TimeMooreMuid.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMuidMooreTPCreator");
// c1.cd(2);
// tc.Draw("TimeMuidExtrCnvAlg");
// c1.cd(3);
// tc.Draw("TimeMuidCnvAlg");
// c1.Update();
// c1.SaveAs("TimeMooreMuidCnv.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMboyRec");
// c1.cd(2);
// tc.Draw("TimeStacoMaster");
// c1.cd(3);
// tc.Draw("TimeMuTagMaster");
// c1.Update();
// c1.SaveAs("TimeMboyStaco.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMuGirl");
// c1.cd(2);
// tc.Draw("TimeCaloTrkMuIdAlg");
// c1.cd(3);
// tc.Draw("TimeTotal");
// c1.Update();
// c1.SaveAs("TimeMuGirlCaloAll.gif");

c1->Clear();
c1->Divide(1,1);
c1.cd(1);
tc.Draw("TimeTotal:Event","","line");
c1.Update();
c1.SaveAs("TimeTotal_vs_event.gif");

// // Timing plots but as a function of event number this time

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeCscRdoToCscPrepData:Event","","line");
// c1.cd(2);
// tc.Draw("TimeMdtRdoToMdtPrepData:Event","","line");
// c1.cd(3);
// tc.Draw("TimeRpcRdoToRpcPrepData:Event","","line");
// c1.cd(4);
// tc.Draw("TimeTgcRdoToTgcPrepData:Event","","line");
// c1.Update();
// c1.SaveAs("TimeRdoToPrepData0_vs_evt.gif");

// //TCanvas *c1 = new TCanvas("c1","TimeRdoToPrepData1.C");
// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeCscRdoToCscPrepData:Event","Event>0","line");
// c1.cd(2);
// tc.Draw("TimeMdtRdoToMdtPrepData:Event","Event>0","line");
// c1.cd(3);
// tc.Draw("TimeRpcRdoToRpcPrepData:Event","Event>0","line");
// c1.cd(4);
// tc.Draw("TimeTgcRdoToTgcPrepData:Event","Event>0","line");
// c1.Update();
// c1.SaveAs("TimeRdoToPrepData1_vs_evt.gif");

// //TCanvas *c1 = new TCanvas("c1","TimeMoore.C");
// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMooSegmentMaker:Event","","line");
// c1.cd(2);
// tc.Draw("TimeMuonCombiTrackBuilder:Event","","line");
// c1.cd(3);
// tc.Draw("TimeMuidStandAlone:Event","","line");
// c1.cd(4);
// tc.Draw("TimeMuidCombined:Event","","line");
// c1.Update();
// c1.SaveAs("TimeMooreMuid_vs_evt.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMuidMooreTPCreator:Event","","line");
// c1.cd(2);
// tc.Draw("TimeMuidExtrCnvAlg:Event","","line");
// c1.cd(3);
// tc.Draw("TimeMuidCnvAlg:Event","","line");
// c1.Update();
// c1.SaveAs("TimeMooreMuidCnv_vs_evt.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMboyRec:Event","","line");
// c1.cd(2);
// tc.Draw("TimeStacoMaster:Event","","line");
// c1.cd(3);
// tc.Draw("TimeMuTagMaster:Event","","line");
// c1.Update();
// c1.SaveAs("TimeMboyStaco_vs_evt.gif");

// c1->Clear();
// c1->Divide(2,2);
// c1.cd(1);
// tc.Draw("TimeMuGirl:Event","","line");
// c1.cd(2);
// tc.Draw("TimeCaloTrkMuIdAlg:Event","","line");
// c1.cd(3);
// tc.Draw("TimeTotal:Event","","line");
// c1.Update();
// c1.SaveAs("TimeMuGirlCaloAll_vs_evt.gif");

//psFile.Close();

}
