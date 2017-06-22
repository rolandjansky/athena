/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TChain.h"
#include <iostream>
#include "TGraph.h"

void run_geo();

void run_geo()
{
  
 //how to run in root 6 on lxplus:
 //.x init_geo.C+
 //.x run_geo.C
 
 // Warning: cell lookup in the FCal is not working yet!
 CaloGeometryFromFile* geo=new CaloGeometryFromFile();
 geo->SetDoGraphs(1);
 geo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root","ATLAS-GEO-20-00-01");
 geo->LoadFCalGeometryFromFiles("FCal1-electrodes.sorted.HV.09Nov2007.dat","FCal2-electrodes.sorted.HV.April2011.dat","FCal3-electrodes.sorted.HV.09Nov2007.dat");
  //CaloGeometry::m_debug_identity=3179554531063103488;
	//geo->Validate();
 
 
  const CaloGeoDetDescrElement* cell;
  cell=geo->getDDE(2,0.24,0.24); //This is not working yet for the FCal!!!
  //cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
  
  unsigned long long cellid64(3179554531063103488);
  Identifier cellid(cellid64);
  cell=geo->getDDE(cellid); //This is working also for the FCal
  
  cout<<"Found cell id="<<cell->identify()<<" sample="<<cell->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<endl;
  
  /*TCanvas* canvas = new TCanvas("Calo_layout","Calo layout");
  TH2D* hcalolayout=new TH2D("hcalolayout","hcalolayout",50,-7000,7000,50,0,4000);
  hcalolayout->Draw();
  hcalolayout->SetStats(0);
  
  for(int i=0;i<24;++i) {
    double eta=0.2;
    double mineta,maxeta;
    geo->minmaxeta(i,eta,mineta,maxeta);
    cout<<geo->SamplingName(i)<<" : mineta="<<mineta<<" maxeta="<<maxeta<<endl;
    if(mineta<eta && maxeta>eta) {
      double avgeta=eta;
      cout<<"  r="<<geo->rent(i,avgeta)<<" -> "<<geo->rmid(i,avgeta)<<" -> "<<geo->rext(i,avgeta)<<endl;
    }  
    geo->GetGraph(i)->Draw("Psame");
  }
  
  geo->DrawGeoForPhi0();
  canvas->SaveAs("Calorimeter.png");*/
  
  TCanvas* canvas = new TCanvas("FCal1_xy","FCal1_xy");
  geo->DrawFCalGraph(21,1);
	canvas->SaveAs("FCal1Geometry.png");
	TCanvas* canvas2 = new TCanvas("FCal2_xy","FCal2_xy");
  geo->DrawFCalGraph(22,1);
	canvas2->SaveAs("FCal2Geometry.png");
	TCanvas* canvas3 = new TCanvas("FCal3_xy","FCal3_xy");
  geo->DrawFCalGraph(23,1);
	canvas3->SaveAs("FCal3Geometry.png");
  
  
  
  
  vector<ifstream*> electrodes(3);
  
  electrodes[0]=new ifstream("FCal1-electrodes.sorted.HV.09Nov2007.dat");
  electrodes[1]=new ifstream("FCal2-electrodes.sorted.HV.April2011.dat");
  electrodes[2]=new ifstream("FCal3-electrodes.sorted.HV.09Nov2007.dat");
  
  
  
  int	thisTubeId;
  int    thisTubeI;
	int    thisTubeJ;
	//int    thisTubeID;
	int    thisTubeMod;
	double thisTubeX;
	double thisTubeY;
	TString tubeName;
	
	int second_column;
	string seventh_column;
	string eight_column;
	int ninth_column;
	
	FCAL_ChannelMap *cmap = new FCAL_ChannelMap(0);
	
	
	
	
	int i;
	for(int imodule=1;imodule<=3;imodule++){
		
		i=0;
		//while(i<50){
		while(1){
		
		  //cout << electrodes[imodule-1]->eof() << endl;
			(*electrodes[imodule-1]) >> tubeName;
			if(electrodes[imodule-1]->eof())break;
			(*electrodes[imodule-1]) >> thisTubeId; // ?????
			(*electrodes[imodule-1]) >> thisTubeI;
			(*electrodes[imodule-1]) >> thisTubeJ;
			(*electrodes[imodule-1]) >> thisTubeX;
			(*electrodes[imodule-1]) >> thisTubeY;
			(*electrodes[imodule-1]) >> seventh_column;
			(*electrodes[imodule-1]) >> eight_column;
			(*electrodes[imodule-1]) >> ninth_column;
			
			tubeName.ReplaceAll("'","");
			string tubeNamestring=tubeName.Data();
			
			std::istringstream tileStream1(std::string(tubeNamestring,1,1));
		  std::istringstream tileStream2(std::string(tubeNamestring,3,2));
		  std::istringstream tileStream3(std::string(tubeNamestring,6,3));
		  int a1=0,a2=0,a3=0;
		  if (tileStream1) tileStream1 >> a1;
		  if (tileStream2) tileStream2 >> a2;
		  if (tileStream3) tileStream3 >> a3;
			
			unsigned int tileName= (a3 << 16) + a2;
			stringstream s;
			
			
			cmap->add_tube(tubeNamestring, imodule, thisTubeId, thisTubeI,thisTubeJ, thisTubeX, thisTubeY,seventh_column);
			
			
			
			//cout << "FCal electrodes: " << tubeName << " " << second_column << " " << thisTubeI << " " << thisTubeJ << " " << thisTubeX << " " << thisTubeY << " " << seventh_column << " " << eight_column << " " << ninth_column << endl;
			//cout << tileStream1.str() << " " << tileStream2.str() << " " << tileStream3.str() << endl;
			//cout << a1 << " " << a2 << " " << a3 << " " << tileName << endl;
			i++;
		}
	}
	cmap->finish(); // Creates maps
	 
	cmap->print_tubemap(1);
  cmap->print_tubemap(2);
  cmap->print_tubemap(3);
  
  int eta_index,phi_index;
  Long64_t eta_index64,phi_index64;
  double x=130;
  double y=180;
  
  
  const CaloGeoDetDescrElement* mcell=0;
  const CaloGeoDetDescrElement* mcell2=0;
  
  cout << endl;
  cout << "Looking for tile corresponding to [x,y] = [" <<  x << "," << y << "]" << endl;
  
  for(int imap=1;imap<=3;imap++){
  
		cout << "Looking for tile in module " << imap << endl;
	  if(!cmap->getTileID(imap,x,y,eta_index,phi_index)){
			cout << "Not found" << endl;continue;
		}
	  cout << "Tile found" << endl;
	 
	 cout << "Tile Id " << (eta_index << 16) + phi_index << endl;
		cout << "eta index: " << eta_index << endl;
		cout << "phi index: " << phi_index << endl;
		cout << "Tile position: [x,y] = [" <<  cmap->x(imap,eta_index,phi_index) << "," << cmap->y(imap,eta_index,phi_index) << "]" << endl;
		mcell=geo->getFCalDDE(imap+20,x,y,-1);
		cout << "Tile position for calogeometry: [x,y] = [" <<  mcell->x() << "," << mcell->y() << "]" << endl;
		
		Identifier identifier= mcell->identify();
		/*eta_index64=eta_index;
		phi_index64=phi_index;
		if (imap==2) eta_index64+=100;
		if (imap==3) eta_index64+=200;
		cout << identifier << " " << (eta_index64 << 16) + phi_index64 << endl;
		
		
		
		mcell2=geo->getDDE((eta_index64 << 16) + phi_index64);
		cout << "Tile position for calogeometry using identifier: [x,y] = [" <<  mcell2->x() << "," << mcell2->y() << "]" << endl;*/
	
	
	
	}
	
 
}

 
