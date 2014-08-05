/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void TestPhotonEfficiencySFTool(Int_t isConv=0) 
{
  gROOT->ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C+");

  PhotonEfficiencySFTool testSFTool = PhotonEfficiencySFTool("../share/PhotonSFSyst2011.root"); 

  double sf     = 0.;  
  double efferr = 0.;  
  double isoerr = 0.;
  double fragerr = 0.;

  const int neta = 6;
  const int npt = 15;

  double sfptbins[]={15,20,25,30,35,40,45,50,60,80,100,125,150,175,250,300};  
  double sfetabins[]={0.00, 0.6, 1.37, 1.52, 1.81, 2.37,2.47};  
 
  for(int i=0;i<npt;i++){ 
    for(int j=0;j<neta;j++){ 
 
      sf      = testSFTool.GetSFFFMC(sfetabins[j],sfptbins[i]*1000.,isConv); 
      efferr  = testSFTool.GetEffSyst(sfetabins[j],sfptbins[i]*1000.,isConv);
      isoerr  = testSFTool.GetIsoSyst(sfetabins[j],sfptbins[i]*1000.,isConv);
      fragerr = testSFTool.GetFragSyst(sfetabins[j],sfptbins[i]*1000.,isConv);
      toterr  = testSFTool.GetTotSyst(sfetabins[j],sfptbins[i]*1000.,isConv);


      printf("Eta [%3.2f,%3.2f] - PT [%4.1f,%4.1f] --> SF = %f +/- ( %f, %f, %f, %f) \n", 
	     sfetabins[j],  sfetabins[j+1], sfptbins[i], sfptbins[i+1], 
	     sf, efferr,isoerr,fragerr,toterr); 
 
    } 
  } 

}
