/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloGeometryFromFile.h"
#include <TTree.h>
#include <TFile.h>
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <fstream>
#include <sstream>
#include <TGraph.h>
#include "TH2D.h"

using namespace std;

map<unsigned long long, unsigned long long> g_cellId_vs_cellHashId_map;

CaloGeometryFromFile::CaloGeometryFromFile() : CaloGeometry()
{
	ifstream textfile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/cellId_vs_cellHashId_map.txt");
	unsigned long long id, hash_id; 
	cout << "Loading cellId_vs_cellHashId_map" << endl;
	int i=0;
	string line;
	stringstream s;
	while(1){
		//getline(textfile,line);
		s.str(line);
	  textfile >> id;
		if(textfile.eof())break;
		textfile >> hash_id;
		g_cellId_vs_cellHashId_map[id]=hash_id;
		if(i%10000==0)cout << "Line: " << i << " line " << line << " id " << hex << id << " hash_id " << dec << hash_id << endl;
		i++;
	}
	cout << "Done." << endl;
	
}

CaloGeometryFromFile::~CaloGeometryFromFile()
{
}

bool CaloGeometryFromFile::LoadGeometryFromFile(TString filename,TString treename)
{
  TTree *tree;
  TFile *f = TFile::Open(filename);
  if(!f) return false;
  f->GetObject(treename,tree);
  if(!tree) return false;

  TTree* fChain = tree;
  
  CaloDetDescrElement cell;
  
  // List of branches
  TBranch        *b_identifier;   //!
  TBranch        *b_calosample;   //!
  TBranch        *b_eta;   //!
  TBranch        *b_phi;   //!
  TBranch        *b_r;   //!
  TBranch        *b_eta_raw;   //!
  TBranch        *b_phi_raw;   //!
  TBranch        *b_r_raw;   //!
  TBranch        *b_x;   //!
  TBranch        *b_y;   //!
  TBranch        *b_z;   //!
  TBranch        *b_x_raw;   //!
  TBranch        *b_y_raw;   //!
  TBranch        *b_z_raw;   //!
  TBranch        *b_deta;   //!
  TBranch        *b_dphi;   //!
  TBranch        *b_dr;   //!
  TBranch        *b_dx;   //!
  TBranch        *b_dy;   //!
  TBranch        *b_dz;   //!
  
  fChain->SetMakeClass(1);
  fChain->SetBranchAddress("identifier", &cell.m_identify, &b_identifier);
  fChain->SetBranchAddress("calosample", &cell.m_calosample, &b_calosample);
  fChain->SetBranchAddress("eta", &cell.m_eta, &b_eta);
  fChain->SetBranchAddress("phi", &cell.m_phi, &b_phi);
  fChain->SetBranchAddress("r", &cell.m_r, &b_r);
  fChain->SetBranchAddress("eta_raw", &cell.m_eta_raw, &b_eta_raw);
  fChain->SetBranchAddress("phi_raw", &cell.m_phi_raw, &b_phi_raw);
  fChain->SetBranchAddress("r_raw", &cell.m_r_raw, &b_r_raw);
  fChain->SetBranchAddress("x", &cell.m_x, &b_x);
  fChain->SetBranchAddress("y", &cell.m_y, &b_y);
  fChain->SetBranchAddress("z", &cell.m_z, &b_z);
  fChain->SetBranchAddress("x_raw", &cell.m_x_raw, &b_x_raw);
  fChain->SetBranchAddress("y_raw", &cell.m_y_raw, &b_y_raw);
  fChain->SetBranchAddress("z_raw", &cell.m_z_raw, &b_z_raw);
  fChain->SetBranchAddress("deta", &cell.m_deta, &b_deta);
  fChain->SetBranchAddress("dphi", &cell.m_dphi, &b_dphi);
  fChain->SetBranchAddress("dr", &cell.m_dr, &b_dr);
  fChain->SetBranchAddress("dx", &cell.m_dx, &b_dx);
  fChain->SetBranchAddress("dy", &cell.m_dy, &b_dy);
  fChain->SetBranchAddress("dz", &cell.m_dz, &b_dz);
  
  Long64_t nentries = fChain->GetEntriesFast();
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = fChain->LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry);
    
    if (g_cellId_vs_cellHashId_map.find(cell.m_identify)!=g_cellId_vs_cellHashId_map.end()) cell.m_hash_id=g_cellId_vs_cellHashId_map[cell.m_identify];
    else cout << endl << "ERROR: Cell id not found in the cellId_vs_cellHashId_map!!!" << endl << endl;
    

    const CaloDetDescrElement* pcell=new CaloDetDescrElement(cell);
    int sampling = pcell->getSampling();
    
    
    if(sampling >20) continue;
    this->addcell(pcell);
		
    if(jentry%25000==0) {
    //if(jentry==nentries-1) {
			cout << "Checking loading cells from file" << endl;
      cout<<jentry<<" : "<<pcell->getSampling()<<", "<<pcell->identify()<<endl;

      
      
      
      //if(jentry>5) break;
    }
  }
	//cout<<"all : "<<m_cells.size()<<endl;
  //unsigned long long max(0);
  //unsigned long long min_id=m_cells_in_sampling[0].begin()->first;
  //for(int i=0;i<21;++i) {
		////cout<<"  cells sampling "<<i<<" : "<<m_cells_in_sampling[i].size()<<" cells";
		////cout<<", "<<m_cells_in_regions[i].size()<<" lookup map(s)"<<endl;
		//for(auto it=m_cells_in_sampling[i].begin(); it!=m_cells_in_sampling[i].end();it++){
			////cout << it->second->getSampling() << " " << it->first << endl;
			//if(min_id/10 >=  it->first){
				////cout << "Warning: Identifiers are not in increasing order!!!!" << endl;
				////cout << min_id << " " << it->first << endl;
				
			//}
			//if(min_id > it->first)min_id = it->first;
		//}
	//}
	//cout << "Min id for samplings < 21: " << min_id << endl;
  delete f;
	//return true;
	bool ok = PostProcessGeometry();
	
	cout << "Result of PostProcessGeometry(): " << ok << endl;

	const CaloDetDescrElement* mcell=0;
	unsigned long long cellid64(3179554531063103488);
	Identifier cellid(cellid64);
	mcell=this->getDDE(cellid); //This is working also for the FCal
	
	std::cout << "\n \n";
	std::cout << "Testing whether CaloGeoGeometry is loaded properly" << std::endl;
	if(!mcell)std::cout << "Cell is not found" << std::endl;
	std::cout << "Identifier " << mcell->identify() <<" sampling " << mcell->getSampling() << " eta: " << mcell->eta() << " phi: " << mcell->phi() << std::endl<< std::endl;
	
	const CaloDetDescrElement* mcell2 = this->getDDE(mcell->getSampling(),mcell->eta(),mcell->phi());
	std::cout << "Identifier " << mcell2->identify() <<" sampling " << mcell2->getSampling() << " eta: " << mcell2->eta() << " phi: " << mcell2->phi() << std::endl<< std::endl;
	
  return ok;
}


void CaloGeometryFromFile::LoadFCalGeometryFromFiles(TString filename1,TString filename2,TString filename3){
	
	vector<ifstream*> electrodes(3);
  
  electrodes[0]=new ifstream(filename1);
  electrodes[1]=new ifstream(filename2);
  electrodes[2]=new ifstream(filename3);
	
	
	int	thisTubeId;
  int    thisTubeI;
	int    thisTubeJ;
	//int    thisTubeID;
	//int    thisTubeMod;
	double thisTubeX;
	double thisTubeY;
	TString tubeName;
	
	//int second_column;
	string seventh_column;
	string eight_column;
	int ninth_column;

	
	
	
	
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
			
			//unsigned int tileName= (a3 << 16) + a2;
			stringstream s;
			
			
			m_FCal_ChannelMap.add_tube(tubeNamestring, imodule, thisTubeId, thisTubeI,thisTubeJ, thisTubeX, thisTubeY,seventh_column);
			
			
			
			//cout << "FCal electrodes: " << tubeName << " " << second_column << " " << thisTubeI << " " << thisTubeJ << " " << thisTubeX << " " << thisTubeY << " " << seventh_column << " " << eight_column << " " << ninth_column << endl;
			//cout << tileStream1.str() << " " << tileStream2.str() << " " << tileStream3.str() << endl;
			//cout << a1 << " " << a2 << " " << a3 << " " << tileName << endl;
			i++;
		}
	}
	
	
	m_FCal_ChannelMap.finish(); // Creates maps
	
	unsigned long long phi_index,eta_index;
	float x,y,dx,dy;
	long id;
	//auto it =m_cells_in_sampling[20].rbegin();
	//it--;
	//unsigned long long identify=it->first;
	//for(auto it=m_cells_in_sampling[i].begin(); it!=m_cells_in_sampling[i].end();it++){
	//	
	//}
	long mask1[]{0x34,0x34,0x35};
	long mask2[]{0x36,0x36,0x37};
	
	for(int imap=1;imap<=3;imap++){
		for(auto it=m_FCal_ChannelMap.begin(imap);it!=m_FCal_ChannelMap.end(imap);it++){
			phi_index = it->first & 0xffff;
			eta_index = it->first >> 16;
			CaloDetDescrElement *DDE =new CaloDetDescrElement; // side C
			CaloDetDescrElement *DDE2 =new CaloDetDescrElement; // side A
			x=it->second.x();
			y=it->second.y();
			m_FCal_ChannelMap.tileSize(imap, eta_index, phi_index,dx,dy);
			//if(imap==2) eta_index+=100;
			//if(imap==3) eta_index+=200;
			//cout << imap << " " << eta_index << " " << phi_index << " " <<  it->first << " " << (eta_index << 16) + phi_index << endl;
			
			DDE->m_calosample=imap+20;
			DDE->m_x=x;
			DDE->m_y=y;
			DDE->m_x_raw=x;
			DDE->m_y_raw=y;
			DDE->m_dx=dx;
			DDE->m_dy=dy;
			
			DDE2->m_calosample=imap+20;
			DDE2->m_x=x;
			DDE2->m_y=y;
			DDE2->m_x_raw=x;
			DDE2->m_y_raw=y;
			DDE2->m_dx=dx;
			DDE2->m_dy=dy;
			
			id=(mask1[imap-1]<<12) + (eta_index << 5) +2*phi_index;
			if(imap==2) id+= (8<<8);
			DDE->m_identify=(id << 44);
			//DDE->m_identify=(id << 12);
			//cout << DDE->identify() << endl;
			if (g_cellId_vs_cellHashId_map.find(DDE->m_identify)!=g_cellId_vs_cellHashId_map.end()) DDE->m_hash_id=g_cellId_vs_cellHashId_map[DDE->m_identify];
			else cout << endl << "ERROR: Cell id not found in the cellId_vs_cellHashId_map!!!" << endl << endl;
			
			addcell(DDE);
			id=(mask2[imap-1]<<12) + (eta_index << 5) +2*phi_index;
			if(imap==2) id+= (8<<8);
			DDE2->m_identify=(id << 44);
						//DDE->m_identify=(id << 12);
			if (g_cellId_vs_cellHashId_map.find(DDE2->m_identify)!=g_cellId_vs_cellHashId_map.end()) DDE2->m_hash_id=g_cellId_vs_cellHashId_map[DDE2->m_identify];
			else cout << endl << "ERROR: Cell id not found in the cellId_vs_cellHashId_map!!!" << endl << endl;
			addcell(DDE2);
			delete DDE;
			delete DDE2;
		}
	
	}
	//auto it =m_cells_in_sampling[0].begin();
	//it--;
	//unsigned long long identify=it->first;
	////cout << "min identifier from sampling < 21: " << identify << endl;
	
	//for(int i=0;i<21;++i) {
	////for(int i=21;i<MAX_SAMPLING;++i) {
        //cout<<"  cells sampling "<<i<<" : "<<m_cells_in_sampling[i].size()<<" cells";
        //cout<<", "<<m_cells_in_regions[i].size()<<" lookup map(s)"<<endl;
        //int k=0;
        //for(auto it=m_cells_in_sampling[i].begin(); it!=m_cells_in_sampling[i].end();it++){
					//if(k<10){
						//cout << "  cells sampling "<< it->second->getSampling() << " : " << it->first << endl;
						//const CaloDetDescrElement *DDE = this->getDDE(it->second->getSampling(),it->second->eta(),it->second->phi());
						//cout << "  cells sampling "<< DDE->getSampling()  << " : " << DDE->identify()  << endl;
						//DDE = this->getDDE(it->first);
						//cout << "  cells sampling "<< DDE->getSampling()  << " : " << DDE->identify() << endl << endl;
					//}
					//k++;
				//}
        
  //}
  
  
  for(int imodule=1;imodule<=3;imodule++) delete electrodes[imodule-1];
  electrodes.clear();

}

void CaloGeometryFromFile::DrawFCalGraph(int isam,int color){
	
	stringstream ss;
	ss << "FCal" << isam - 20 << endl;
	TString name = ss.str().c_str();
	
	const int size=m_cells_in_sampling[isam].size();
	double x[size];
	double y[size];
	//const CaloDetDescrElement* cell;

	int i=0;
	for(auto it=m_cells_in_sampling[isam].begin();it!=m_cells_in_sampling[isam].end();it++){
		x[i]=it->second->x();
		y[i]=it->second->y();
		i++;
	}
	// cout << size << endl;
	//TH2D* h = new TH2D("","",10,-0.5,0.5,10,-0.5,0.5);
	//h->SetStats(0);
	//h->Draw();
	TGraph* graph = new TGraph(size,x,y);
	graph->SetLineColor(color);
	graph->SetTitle(name);
	graph->SetMarkerStyle(21);
	graph->SetMarkerColor(color);
	graph->SetMarkerSize(0.5);
	graph->GetXaxis()->SetTitle("x [mm]");
	graph->GetYaxis()->SetTitle("y [mm]");
	
	graph->Draw("AP");

	
	
}	
	
		
