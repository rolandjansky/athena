/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



void input()
{
 
 vector<int> layer;
 layer.push_back(0);
 layer.push_back(1);
 layer.push_back(2);
 layer.push_back(3);
 layer.push_back(12);
 layer.push_back(13);
 layer.push_back(14);
 
 TFile* input=TFile::Open("PCAOutput.root");
 
 for(int l=0;l<layer.size();l++)
 {
 	
 	cout<<"*********** LAYER "<<layer[l]<<" **************"<<endl;
 	
  TTree* theTree = (TTree*)input->Get("Id");
  double data;
  if(layer[l]==0)
   theTree->SetBranchAddress( "data_0", &data );
  if(layer[l]==1)
   theTree->SetBranchAddress( "data_1", &data );
  if(layer[l]==2)
   theTree->SetBranchAddress( "data_2", &data );
  if(layer[l]==3)
   theTree->SetBranchAddress( "data_3", &data );
  if(layer[l]==12)
   theTree->SetBranchAddress( "data_12", &data );
  if(layer[l]==13)
   theTree->SetBranchAddress( "data_13", &data );
  if(layer[l]==14)
   theTree->SetBranchAddress( "data_14", &data );
 
  vector<double> vals;
  
  std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
  for(Long64_t ievt=0; ievt<theTree->GetEntries();ievt++)
  {
   
   theTree->GetEntry(ievt);
   cout<<"data "<<data<<endl;
   vals.push_back(data);
   
  }
  
  //now sort the vector vals
  //*****************************************************************
  vector<int> indices;
  for(int i=0;i<vals.size();i++)
   indices.push_back(i);
  
  //now order
  vector<double> sorted_values;
  vector<int>    sorted_indices; //this contains the indices that correspond to the names
  
  //initial
  int max=get_min(vals);
  sorted_indices.push_back(max);
  sorted_values.push_back(vals[max]);
  
  vector<int>    temp_indices;
  vector<double> temp_values;
  for(int j=0;j<vals.size();j++)
  {
 	 if(j!=max)
 	 {
 	  temp_values.push_back(vals[j]);
 	  temp_indices.push_back(j);
 	 }
  }
  
  int n=temp_indices.size();
  for(int i=0;i<n;i++)
  {
   int max=get_min(temp_values); 
   sorted_indices.push_back(temp_indices[max]);
   sorted_values.push_back(temp_values[max]);
 	 temp_indices.erase(temp_indices.begin()+max);
 	 temp_values.erase(temp_values.begin()+max);
  }
  cout<<"done sorting"<<endl;
  //**************************************
  
  cout<<"sorted vals: "<<endl;
  for(int i=0;i<sorted_values.size();i++)
   cout<<"i "<<i<<", val= "<<sorted_values[i]<<endl;
  
  //now be careful!
  TFile* output=new TFile(Form("input_%i.root",layer[l]),"RECREATE");
  TTree* tree=new TTree("tree","tree");
  
  Float_t x,y;
  tree->Branch("x",&x,"x/F");
  tree->Branch("y",&y,"y/F");
  
  Float_t e=0.0;
  
  for(int i=0;i<sorted_values.size();i++)
  {
 	 
 	 y=(Float_t)i/(Float_t)(sorted_values.size());
 	 x=(Float_t)(sorted_values[i]);
 	
 	 tree->Fill();
 	
  }
 
  output->Write();
 
 } //layer
 
}


int get_max(vector<double> values)
{
 double max=-1000.0;
 int index;
 for(int i=0;i<values.size();i++)
 {
 	if(values[i]>max)
 	{
 	 maxvalues[i];
 	 index=i;
 	}
 }
 return index;
 
}


int get_min(vector<double> values)
{
 double min=1000.0;
 int index;
 for(int i=0;i<values.size();i++)
 {
 	if(values[i]<min)
 	{
 	 min=values[i];
 	 index=i;
 	}
 }
 return index;
 
}

