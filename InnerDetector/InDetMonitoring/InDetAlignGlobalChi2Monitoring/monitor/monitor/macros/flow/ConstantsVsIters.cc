/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ConstantsVsIters() {
  
  //if(!PlotNtuple) {
  cout << " Modules Global Position" << endl;
  //cout << path<< endl;
  
  if (IterCounter==0) {
    ntupleConstantsVsIter = new TNtuple("ntupleConstantsVsIter","ntupleConstantsVsIter",
					"Iter:Mcode:Layer:Tx:Ty:Tz:Rx:Ry:Rz:PTx:PTy:PTz:PRx:PRy:PRz");
  }
  // ntupleConstantsVsIter->Scan();

  
  //variables comunes dels dos files
  int MCode;
  int Sector,Ring,Side, Layer;
  //Perfect Position
  float Tx,Ty,Tz,Rx,Ry,Rz;
  //Position
  float PTx,PTy,PTz,PRx,PRy,PRz;
  float tmp;
  
  //return;
  
  sprintf(namePath,"%sIter%d/Iter%d_GlobalPositions.txt",path,IterCounter,IterCounter);
  cout<<namePath<<endl;
  sprintf(nameIter,"Iter%d",IterCounter);
  
  //Per a llegir els files:
  //sprintf(nameFile,"fileGlobalPositionsPerfect%d",IterCounter);

  //fitxers de text
  ifstream fileGlobalPositionsPerfect("macros/flow/reference/GlobalPosition.txt");
  ifstream fileGlobalPositions(namePath);
  //ifstream nameFile(namePath);

  if (fileGlobalPositionsPerfect.is_open() && fileGlobalPositions.is_open()){
        while (!fileGlobalPositionsPerfect.eof() )
      {



	fileGlobalPositions >> MCode >> Layer >> Sector >> Ring >> Side >> Tx >> Ty >> Tz >> Rx >> Ry >> Rz >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;
	//cout << "GlobalPos: " << MCode << "  "<< Layer <<" "<< Sector << "   " << Ring <<"   " << Side <<"   " << Tx <<"   "<< Ty <<"   "<< Tz<<"   "<< Rx <<"   " << Ry <<"   " << Rz << endl;
	
	fileGlobalPositionsPerfect >> MCode >> Layer >> Sector >> Ring >> Side >> PTx >>PTy >> PTz >> PRx >> PRy >> PRz >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;
	//cout << "GlobalPosPerf: " << MCode << "  "<< Sector << "   " << Ring <<"   " << Side <<"   " << PTx <<"   "<< PTy <<"   "<< PTz<<"   "<< PRx <<"   " <<PRy <<"   " << PRz << endl;
	
	// cout<<IterCounter<<endl;
	ntupleConstantsVsIter->Fill(IterCounter,MCode,Layer,Tx,Ty,Tz,Rx,Ry,Rz,PTx,PTy,PTz,PRx,PRy,PRz);
	//cout<<MCode<<endl;
      }
	//fileGlobalPositions.close();
	//fileGlobalPositionsPerfect.close();
  }
  
  //ntupleConstantsVsIter->Fill(IterCounter,MCode,Layer,Tx,Ty,Tz,Rx,Ry,Rz,PTx,PTy,PTz,PRx,PRy,PRz);

  ntupleConstantsVsIter->Scan("Iter:Layer:(Tx-PTx)");
  
  return;
  
  //Definim el canvas per als PIXELS
  cPIXPositionVsIter = new TCanvas("cPIXPositionVsIter"," Pixels Positions Vs Iter",CanvasSizeX4[0],CanvasSizeX4[1]);
  cPIXPositionVsIter->Divide(3,3);
  
  //TGraph *hGraph;
  TGraph *hGraphECA[3][3];
  TGraph *hGraphBa[3][3];
  TGraph *hGraphECC[3][3];
  
  for (int i=0; i<9; i++){
    cPIXPositionVsIter->cd(i);
  }
  
  //PIXEL EndCapA
  for (int i=0;i<3; i++){
    sprintf(namePIXECA,"Mcode>=1004000 && Mcode<=1216051 && Layer==%d",i);
    cPIXPositionVsIter->cd(i+1);
    ntupleConstantsVsIter->Draw("(PTx-Tx):Iter",namePIXECA,"goff");
    hGraphECA[0][i] = new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
				 ntupleConstantsVsIter->GetV2(),
				 ntupleConstantsVsIter->GetV1());
    
    hGraphECA[0][i]->GetXaxis()->SetTitle("Iter");
    hGraphECA[0][i]->GetYaxis()->SetTitle("DifTx");
    hGraphECA[0][i]->SetMarkerStyle(4);
    hGraphECA[0][i]->SetMarkerSize(0.4);
    //hGraph->SetMarkerColor(ColorForSCTBarrel[i]);
    hGraphECA[0][i]->SetLineWidth(1);
    //hGraph->SetLineColor(ColorForSCTBarrel[i]);
    hGraphECA[0][i]->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
    hGraphECA[0][i]->GetXaxis()->SetNdivisions(IterCounter);
    //hGraph->GetYaxis()->SetRangeUser(-50.0,50.0);
    hGraphECA[0][i]->SetTitle("EndCap A: Tx Positions");
    hGraphECA[0][i]->Draw("ALP");
    //hGraph->Draw("LP");    
    
  }
  return;

  for (int i=0;i<3; i++){
    sprintf(namePIXBarrel,"Mcode>=1004000 && Mcode<=1216051 && Layer==%d",i);
    cPIXPositionVsIter->cd(1+3);
    //ntupleConstantsVsIter->Draw("(PTx-Tx):Iter","Mcode>=1004000 && Mcode<=1216051 && Layer==0","goff");
    
    ntupleConstantsVsIter->Draw("(PTx-Tx):Iter",namePIXBarrel,"goff");
    hGrapBa[0][i] = new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
				ntupleConstantsVsIter->GetV2(),
				ntupleConstantsVsIter->GetV1());
    
       
    
    hGraphBa[0][i]->GetXaxis()->SetTitle("Iter");
    hGraphBa[0][i]->GetYaxis()->SetTitle("DifTx");
    hGraphBa[0][i]->SetMarkerStyle(4);
    hGraphBa[0][i]->SetMarkerSize(0.4);
    //hGraph[0][i]->SetMarkerColor(ColorForSCTBarrel[i]);
    hGraphBa[0][i]->SetLineWidth(1);
    //hGraph[0][i]->SetLineColor(ColorForSCTBarrel[i]);
    hGraphBa[0][i]->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
    hGraphBa[0][i]->GetXaxis()->SetNdivisions(IterCounter);
    //hGraph[1][i]->GetYaxis()->SetRangeUser(-50.0,50.0);
    hGraphBa[0][i]->SetTitle("PIXEL Barrel: Tx Positions ");
    hGraphBa[0][i]->Draw("ALP");
    //hGraph[0][i]->Draw("LP");    
  }
  // return;
  for (int i=0;i<3; i++){
    sprintf(namePIXECC,"(PTx-Tx):Iter","Mcode>=1004000 && Mcode<=1216051 && Layer==%",i);
    cPIXPositionVsIter->cd(1+6);
    //ntupleConstantsVsIter->Draw("(PTx-Tx):Iter","Mcode>=1004000 && Mcode<=1216051 && Layer==0","goff");
    ntupleConstantsVsIter->Draw("(PTx-Tx):Iter",namePIXECC,"goff");
    TGraph *hGraphECC= new TGraph(ntupleConstantsVsIter->GetSelectedRows(), 
				ntupleConstantsVsIter->GetV2(),
				ntupleConstantsVsIter->GetV1());
    
    
    
    hGraphECC->GetXaxis()->SetTitle("Iter");
    hGraphECC->GetYaxis()->SetTitle("DifTx");
    hGraphECC->SetMarkerStyle(4);
    hGraphECC->SetMarkerSize(0.4);
    //hGraph->SetMarkerColor(ColorForSCTBarrel[i]);
    hGraphECC->SetLineWidth(1);
    //hGraph->SetLineColor(ColorForSCTBarrel[i]);
    hGraphECC->GetXaxis()->SetRangeUser(0.0,(float) IterCounter-1);
    hGraphECC->GetXaxis()->SetNdivisions(IterCounter);
    //hGraph->GetYaxis()->SetRangeUser(-50.0,50.0);
    hGraphECC->SetTitle("PIXELS EndCapC: TX Positions");
    hGraphECC->Draw("ALP");
    //hGraph->Draw("LP");    
  }


}
  
