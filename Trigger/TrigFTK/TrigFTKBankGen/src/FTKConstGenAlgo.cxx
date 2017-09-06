/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKBankGenAlgo.h"
#include "TrigFTKBankGen/FTKConstGenAlgo.h"
#include "TrigFTKBankGen/FTKPattKDTree.h"
#include "TrigFTKBankGen/atlparslice_root.h"

#include "TrigFTKSim/FTKSetup.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TROOT.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompLU.h>
#define MTX_TOLERANCE 3e-16

geo_constants *geo_const,*gcorth;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTKConstGenAlgo::FTKConstGenAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_merging(false),
  m_genconst(false),
  m_subregion(false),
  m_extract_1stStage(false),
  m_fpath(0x0),
  m_outfilename(""),//output file name used for all process
  m_ITkMode(false),
  m_allregion(false),//do allregion or not
  m_region(0),// use if not using all regions
  m_nsubregion(0),
  m_nbank(64),// number of total regions
  m_pmap(0x0),//12L pmap used for extract 1st stage
  m_pmap_8L(0x0),//8L pmap used for extract 1st stage
  m_pmap_path(),//12L pmap path used for extract 1st stage
  m_pmap_8L_path()//8L pmap path used for extract 1st stage
{
  declareProperty("file_path",m_fpath,"array of merging files");
  declareProperty("merged_file_path",m_cfpath,"merged file");
  declareProperty("gcon_file_path",m_gfpath,"gcon file");
  declareProperty("sector_file_path",m_sfpath,"sector file");
  declareProperty("LayerChange_path",m_extractpath,"extract1stStage");
  declareProperty("pmap_path",m_pmap_path);
  declareProperty("pmap_8L_path",m_pmap_8L_path);
  declareProperty("merging",m_merging);
  declareProperty("genconst",m_genconst);
  declareProperty("subregion",m_subregion);
  declareProperty("eightLayer",m_extract_1stStage);
  declareProperty("ITkMode",m_ITkMode);
  declareProperty("allregion",m_allregion);
  declareProperty("region",m_region);
  declareProperty("nsubregion",m_nsubregion);  
  declareProperty("nbank",m_nbank);

  declareProperty("OutputFileName",m_outfilename);
}


FTKConstGenAlgo::~FTKConstGenAlgo()
{
  if (m_pmap) delete m_pmap;
  if (m_pmap_8L) delete m_pmap_8L;
}

StatusCode FTKConstGenAlgo::initialize(){

  // toggle ITk geometry (placeholder with no effect as of Jan. 2016)
  FTKSetup::getFTKSetup().setITkMode( m_ITkMode );
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKConstGenAlgo::initialize()" << endmsg;
  
  // merge
  if(true==m_merging){
    log << MSG::INFO << "This job will be  merging " <<m_fpath.size() << " files  "  <<endmsg;
    merge();
  }
  
  // sector/constant calcuration from matrix
  if(true==m_genconst){
    log << MSG::INFO << "This job will be accumulating fit constant "   <<endmsg;
    constantgen();
  }
  
  // 12L->8L reduction for matrix file
  if(true==m_extract_1stStage){
    log << MSG::INFO << "This job will be making 8Layer From 12Layer  "   <<endmsg;

    // Look for the main plane-map (12L)
    log << MSG::INFO << "Read the logical layer definitions" << endmsg;
    if (m_pmap_path.empty()) {
      log << MSG::FATAL << "Main plane map definition missing" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
      if (!(*m_pmap)) {
	log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
	return StatusCode::FAILURE;
      }
    }
    // Look for the main plane-map (8L)
    if (m_pmap_8L_path.empty()) {
      log << MSG::FATAL << "Main plane map definition missing" << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      m_pmap_8L = new FTKPlaneMap(m_pmap_8L_path.c_str());
      if (!(*m_pmap_8L)) {
	log << MSG::FATAL << "Error using plane map: " << m_pmap_8L_path << endmsg;
	return StatusCode::FAILURE;
      }
    }
    
    doRelation = false;
    nplane = m_pmap->getNPlanes();
    nplane8 = m_pmap_8L->getNPlanes();

    // These 6 counters will evolve in each step of map building
    // to generalize to 1st/2nd stage extrapolation.
    endcap_inversion = new bool[12];

    plane_index_1st_stage = 0;
    plane_index_2nd_stage = 0;
    coord_index_1st_stage = 0;
    coord_index_2nd_stage = 0;
    index_missing_plane = 0;
    index_missing_coord = 0;

    doInvert = false;

    // select 8L from 12L, and check do endcap inversion or not
    for(int nlayer=0; nlayer<m_pmap->getNPlanes(); nlayer++){// loop Layer for 2nd stage

      // dimension of this layer (2 for pixel, 1 for SCT). Reading from .pmap file.
      const int ndimension = m_pmap->getPlane(plane_index_2nd_stage,0).getNDimension();
      
      // if doRelation is false, the layer is not used for 1st stage
      doRelation = false;
      if (plane_index_1st_stage >= nplane8) {
	doRelation = true;
      } else if (m_pmap_8L->getPlane(plane_index_1st_stage, 0).getLayer() !=
		 m_pmap   ->getPlane(plane_index_2nd_stage, 0).getLayer()
		 ) {
	doRelation = true;
      }
      
      if(doRelation){
	
	// move the indexes
	plane_index_2nd_stage += 1;
	coord_index_2nd_stage += ndimension;
	index_missing_plane += 1;
	index_missing_coord += ndimension;
	
      }else{

      // do Invert in this plane?
      doInvert = m_pmap->getPlane(plane_index_2nd_stage,0).getNSte() != m_pmap->getPlane(plane_index_2nd_stage,1).getNSte();
      if (m_pmap->getPlane(plane_index_2nd_stage,0).getNDimension()!=1) doInvert = false;
      
      endcap_inversion[index_missing_plane] = doInvert;
	
      // keep values for later use
      vec_plane_index_1st_stage.push_back(plane_index_1st_stage);
      vec_plane_index_2nd_stage.push_back(plane_index_2nd_stage);
      vec_coord_index_1st_stage.push_back(coord_index_1st_stage);
      vec_coord_index_2nd_stage.push_back(coord_index_2nd_stage);
      vec_ndimension.push_back(ndimension);
      vec_doInvert.push_back(doInvert);

      // move the indexes
      plane_index_1st_stage += 1;
      coord_index_1st_stage += ndimension;
      plane_index_2nd_stage += 1;
      coord_index_2nd_stage += ndimension;

      }//doRelation

    }// loop for 2nd stage layer

    extract_1stStage();
  }

  // make subregion, currently not used
  if(true==m_subregion){
    log << MSG::INFO << "This job will be making subregions from sectors and constants "   <<endmsg;
    make_subregion();
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKConstGenAlgo::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKConstGenAlgo::execute() start" << endmsg;
 

  return StatusCode::SUCCESS;
}

StatusCode FTKConstGenAlgo::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endmsg;
    

  return StatusCode::SUCCESS;
}


// HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// HH the main part of each functions start from here HH
// HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH

void FTKConstGenAlgo::merge()
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO <<" m_path " << m_fpath.size() << endmsg;
  m_nfile=m_fpath.size();

  //
  // variables of matrix before merge
  //

  float nevent = 0;
  int npatterns=0;
  int ndim;
  int ndim2;
  int *sectorID=NULL;
  int *hashID=NULL;
  double *vec=NULL;
  double tmpC;
  double tmpD;
  double tmpPhi;
  double tmpCoto;
  double tmpZ;
  float nhit;
  double *tmpxC=NULL;
  double *tmpxD=NULL;
  double *tmpxPhi=NULL;
  double *tmpxCoto=NULL;
  double *tmpxZ=NULL;
  double *tmpcovx=NULL;
  ULong64_t m_merged_monval[100]={0};
  bool monFlag=false;

  std::vector<short >* tmp_intc = new std::vector<short >;
  std::vector<short >* tmp_intphi = new std::vector<short >;
  std::vector<short >* tmp_intd0 = new std::vector<short >;
  std::vector<short >* tmp_intz0 = new std::vector<short >;
  std::vector<short >* tmp_inteta = new std::vector<short >;
  
  matrix Mtmp;
  
  double tmp_cmax;
  double tmp_cmin;
  int tmp_cslices;
  
  double tmp_phimax;
  double tmp_phimin;
  int tmp_phislices;
  
  double tmp_d0max;
  double tmp_d0min;
  int tmp_d0slices;
  
  double tmp_z0max;
  double tmp_z0min;
  int tmp_z0slices;
  
  double tmp_etamax;
  double tmp_etamin;
  int tmp_etaslices;

  //
  // read information from matrix file
  //

  /* obtain the first valid file to extract some general information
   * used to intialize the merge structures
   */
  string fpath;
  TFile *file=NULL;
  int check_file=0;

  while(check_file<m_nfile){ // loop over the files, ends at the first valid file
    fpath=m_fpath[check_file];
    file = TFile::Open(fpath.c_str());
    if(!file){
      log << MSG::WARNING << fpath.c_str() << " is not found " << endmsg;
    }else{
      if(file->GetNkeys()!=0) break;
    }
    check_file++;
  } // end loop over the files

  /* extract the TTree for a reference region, all the values
   * used in this block are independent
   */
  TTree *tree = (TTree*)file->Get(Form("am%d",m_region));
  TTree *slice_tree = (TTree*)file->Get("slice");
  if(gROOT->FindObject("montree")){
    monFlag=true;
  }

  // extract logical layers related info: total dimenstion and number of logical layers
  tree->SetBranchAddress("ndim",&ndim);
  tree->SetBranchAddress("nplanes",&nplane);
  tree->SetBranchAddress("ndim2",&ndim2);
  tree->GetEntry(0); 
  
  slice_tree->SetBranchAddress("c_max",&tmp_cmax);
  slice_tree->SetBranchAddress("c_min", &tmp_cmin);
  slice_tree->SetBranchAddress("c_slices", &tmp_cslices);
  slice_tree->SetBranchAddress("phi_max", &tmp_phimax);
  slice_tree->SetBranchAddress("phi_min", &tmp_phimin);
  slice_tree->SetBranchAddress("phi_slices", &tmp_phislices);
  slice_tree->SetBranchAddress("d0_max", &tmp_d0max);
  slice_tree->SetBranchAddress("d0_min", &tmp_d0min);
  slice_tree->SetBranchAddress("d0_slices", &tmp_d0slices);
  slice_tree->SetBranchAddress("z0_max", &tmp_z0max);
  slice_tree->SetBranchAddress("z0_min", &tmp_z0min);
  slice_tree->SetBranchAddress("z0_slices", &tmp_z0slices);
  slice_tree->SetBranchAddress("eta_max", &tmp_etamax);
  slice_tree->SetBranchAddress("eta_min", &tmp_etamin);
  slice_tree->SetBranchAddress("eta_slices", &tmp_etaslices);
  slice_tree->GetEntry(0); 
  
  sectorID = (int *) calloc(nplane+1,sizeof(int));
  hashID = (int *) calloc(nplane+1,sizeof(int));
  vec = (double *) calloc(ndim,sizeof(double));  
  tmpxC = (double *) calloc(ndim,sizeof(double));  
  tmpxD = (double *) calloc(ndim,sizeof(double));
  tmpxPhi = (double *) calloc(ndim,sizeof(double));  
  tmpxCoto = (double *) calloc(ndim,sizeof(double));  
  tmpxZ = (double *) calloc(ndim,sizeof(double));  
  tmpcovx = (double *) calloc(ndim2,sizeof(double));  

  // close the temporary file
  file->Close();

  // setup the boundaries for the merge, it can be a single region or a range
  // i.e. use all region or one region

  int b_int;
  int b_end;

  int nbanks;
  int bank_order;
  if(true==m_allregion){
    b_int=0;
    b_end=m_nbank;
  }else{
    b_int=m_region; 
    b_end=m_region+1; 
  }

  nbanks = b_end - b_int;
  initPattTree(nbanks, nplane+1,ndim);

  for(int i=0;i<m_nfile;i++){ // loop over the input files
    const string &fpath = m_fpath[i]; 
    log << MSG::INFO <<" m_file " << i << endmsg;
    
    TFile *mfile = TFile::Open(fpath.c_str());
    gROOT->cd();

    // check if the file is valid, if not move to the next
    if(!mfile){
      log << MSG::WARNING << fpath.c_str() << " is not found " << endmsg;
      mfile->Close();
      continue;
    }
    else if(mfile->GetNkeys()==0){
      log << MSG::WARNING << " This file has not keys " << i << endmsg;
      mfile->Close();
      continue;
    }

    for(int b=b_int;b<b_end;b++) { // loop over the towers

      // get the tree for the specific bank
      TTree *am_tree = (TTree*)mfile->Get(Form("am%d",b));

      // extract the number of events and connect the branches
      nevent =am_tree->GetEntries();
      am_tree->SetBranchAddress("sectorID",sectorID);
      am_tree->SetBranchAddress("hashID",hashID);
      am_tree->SetBranchAddress("tmpC",&tmpC);
      am_tree->SetBranchAddress("tmpD",&tmpD);
      am_tree->SetBranchAddress("tmpPhi",&tmpPhi);
      am_tree->SetBranchAddress("tmpCoto",&tmpCoto);
      am_tree->SetBranchAddress("tmpZ",&tmpZ);
      am_tree->SetBranchAddress("nhit",&nhit);
      am_tree->SetBranchAddress("Vec",vec);
      am_tree->SetBranchAddress("tmpxC", tmpxC);
      am_tree->SetBranchAddress("tmpxD", tmpxD);
      am_tree->SetBranchAddress("tmpxPhi", tmpxPhi);
      am_tree->SetBranchAddress("tmpxCoto", tmpxCoto);
      am_tree->SetBranchAddress("tmpxZ", tmpxZ);
      am_tree->SetBranchAddress("tmpcovx", tmpcovx);

      am_tree->SetBranchAddress("tmpintc", &tmp_intc);
      am_tree->SetBranchAddress("tmpintphi", &tmp_intphi);
      am_tree->SetBranchAddress("tmpintd0", &tmp_intd0);
      am_tree->SetBranchAddress("tmpintz0", &tmp_intz0);
      am_tree->SetBranchAddress("tmpinteta", &tmp_inteta);
      
      // check the number of sectors analyzed
      nsector = nsector+nevent;
      
      for(int j=0;j<nevent;j++){ // event loop
        // load the current entry
        am_tree->GetEntry(j);
        Mtmp.C=tmpC, Mtmp.D=tmpD, Mtmp.Phi=tmpPhi;
        Mtmp.Coto=tmpCoto, Mtmp.Z=tmpZ,Mtmp.nhit=nhit;

        sectorID[nplane] = 0;//for sector, we set it to zero.
        hashID[nplane] = 0;//for sector, we set it to zero.

        bank_order = b - b_int;
        addPattReturnCode=addKDPattern(bank_order,false,sectorID,hashID,1,Mtmp,vec,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
                                       tmp_intc[0],tmp_intphi[0],tmp_intd0[0],tmp_intz0[0],tmp_inteta[0]);

	// new pattern found
        if(1==addPattReturnCode){
          ++npatterns;
        }
      } // end event loop

      // close current file
      delete am_tree;
      am_tree=NULL;
    } // end bank loop

    if(monFlag){
      TTree *m_montree = (TTree *)mfile->Get("montree");
      ULong64_t m_monval[100]={0};
      m_montree->SetBranchAddress("monval", m_monval);
      m_montree->GetEntry(0);
      for(int ibin=0;ibin<100;ibin++){
        m_merged_monval[ibin] += m_monval[ibin];
      }// for ibin                                                                                                                                                                                     
      delete m_montree;
      m_montree=NULL;
    }

    // close current file
    mfile->Close();
    delete mfile;
    mfile=NULL;
  } // end loop over the files
  
  free(sectorID);
  free(hashID);
  free(vec);
  free(tmpxC);
  free(tmpxD);
  free(tmpxPhi);
  free(tmpxCoto);
  free(tmpxZ); 
  free(tmpcovx);
  
  ////////////////////////////////////////////////////////////
  ////// create new file merged some files
  ///////////////////////////////////////////////////////////
  
  //
  // variables for merged matrix file
  //

  int *mergeSec=NULL;
  int *mergehashID=NULL;
  double *mergevec=NULL;
  double mergeC;
  double mergeD;
  double mergePhi;
  double mergeCoto;
  double mergeZ;
  float mergenhit;
  
  double *mergexC=NULL;
  double *mergexD=NULL;
  double *mergexPhi=NULL;
  double *mergexCoto=NULL;
  double *mergexZ=NULL;
  double *mergecovx=NULL;

  TTree *m_tree=NULL;
  TTree *m_mergeslice=NULL;
  TTree *m_merged_montree=0;
  if(monFlag){
    m_merged_montree = new TTree("montree", "monitor tree");
    m_merged_montree->Branch("monval", m_merged_monval, "m_monval[100]/l");
    m_merged_montree->Fill();
  }

  m_mergeslice=new TTree("slice", "slice para");
  
  mergeSec = (int *) calloc(nplane,sizeof(int));  
  mergehashID = (int *) calloc(nplane,sizeof(int));  
  mergevec = (double *) calloc(ndim,sizeof(double));  
  mergexC = (double *) calloc(ndim,sizeof(double));  
  mergexD = (double *) calloc(ndim,sizeof(double));
  mergexPhi = (double *) calloc(ndim,sizeof(double));  
  mergexCoto = (double *) calloc(ndim,sizeof(double));  
  mergexZ = (double *) calloc(ndim,sizeof(double));  
  mergecovx = (double *) calloc(ndim2,sizeof(double));  
  
  // define output file
  if (m_outfilename.size()==0) {
    // if the output file is not specified the name is built according a specific conention
    char filename[40];
    if(true==m_allregion){
      sprintf(filename,"matrix_%dL_%dDim_regall.root",nplane,ndim);
    }else{
      sprintf(filename,"matrix_%dL_%dDim_reg%d.root",nplane,ndim,m_region);
    }
    m_outfilename = filename;
  }

  // open the output file for the merged matrixes
  file = TFile::Open(m_outfilename.c_str(),"recreate");

  char name[5];
  char mtitle[20];

  m_mergeslice->Branch("c_max",&tmp_cmax);
  m_mergeslice->Branch("phi_max",&tmp_phimax);
  m_mergeslice->Branch("d0_max",&tmp_d0max);
  m_mergeslice->Branch("z0_max",&tmp_z0max);
  m_mergeslice->Branch("eta_max",&tmp_etamax);
  
  m_mergeslice->Branch("c_min",&tmp_cmin);
  m_mergeslice->Branch("phi_min",&tmp_phimin);
  m_mergeslice->Branch("d0_min",&tmp_d0min);
  m_mergeslice->Branch("z0_min",&tmp_z0min);
  m_mergeslice->Branch("eta_min",&tmp_etamin);
  
  m_mergeslice->Branch("c_slices",&tmp_cslices);
  m_mergeslice->Branch("phi_slices",&tmp_phislices);
  m_mergeslice->Branch("d0_slices",&tmp_d0slices);
  m_mergeslice->Branch("z0_slices",&tmp_z0slices); 
  m_mergeslice->Branch("eta_slices",&tmp_etaslices);    
  m_mergeslice->Fill();
  m_mergeslice->Write();

  if(monFlag){
    m_merged_montree->Write();
  }

  gROOT->cd();

  //
  // define branch name of merged matrix file, and fill
  //

  for(int b=b_int;b<b_end;b++){
    sprintf(name,"am%d",b);
    sprintf(mtitle,"Ambank %d para",b);
        
    m_tree = new TTree(name, mtitle);
    
    m_tree->Branch("ndim",&ndim,"ndim/I");
    m_tree->Branch("ndim2",&ndim2,"ndim2/I");
    m_tree->Branch("Vec", mergevec,"Vec[ndim]/D");
    m_tree->Branch("nplanes",&nplane,"nplanes/I");
    m_tree->Branch("sectorID", mergeSec,"sectorID[nplanes]/I");
    m_tree->Branch("hashID", mergehashID,"hashID[nplanes]/I");
    m_tree->Branch("nhit", &mergenhit,"nhit/F");
    m_tree->Branch("tmpC", &mergeC,"tmpC/D");
    m_tree->Branch("tmpD", &mergeD,"tmpD/D");
    m_tree->Branch("tmpPhi", &mergePhi,"tmpPhi/D");
    m_tree->Branch("tmpCoto", &mergeCoto,"tmpCoto/D");
    m_tree->Branch("tmpZ", &mergeZ,"tmpZ/D");
    m_tree->Branch("tmpxC", mergexC,"tmpxC[ndim]/D");
    m_tree->Branch("tmpxD", mergexD,"tmpxD[ndim]/D");
    m_tree->Branch("tmpxPhi", mergexPhi,"tmpxPhi[ndim]/D");
    m_tree->Branch("tmpxCoto", mergexCoto,"tmpxCoto[ndim]/D");
    m_tree->Branch("tmpxZ", mergexZ,"tmpxZ[ndim]/D");
    m_tree->Branch("tmpcovx", mergecovx,"tmpcovx[ndim2]/D");
                     
    m_mergeintc= new std::vector<short>;
    m_mergeintphi= new std::vector<short>;
    m_mergeintd0= new std::vector<short>;
    m_mergeintz0= new std::vector<short>;
    m_mergeinteta= new std::vector<short>;

    m_tree->Branch("tmpintc", &m_mergeintc);
    m_tree->Branch("tmpintphi", &m_mergeintphi);
    m_tree->Branch("tmpintd0", &m_mergeintd0);
    m_tree->Branch("tmpintz0", &m_mergeintz0);
    m_tree->Branch("tmpinteta", &m_mergeinteta);

    bank_order = b - b_int;

    sortMPatterns(bank_order);
    
    for(int i=0; i<getNPatterns(bank_order);i++){
      m_mergeintc->clear();
      m_mergeintphi->clear();
      m_mergeintd0->clear();
      m_mergeintz0->clear();
      m_mergeinteta->clear();	

      log << MSG::INFO <<"sector ";
      for(int k=0;k<nplane;k++){
	mergeSec[k]=getPatternSS(bank_order,i,k);
	mergehashID[k]=getPatthashID(bank_order,i,k);
	log <<getPatternSS(bank_order,i,k)<<" ";//tmp out
      }
      log <<" "<<getPattnhit(bank_order,i) <<endmsg;//tmp out
      mergenhit=getPattnhit(bank_order,i);
      mergeC=getPattC(bank_order,i);
      mergeD=getPattD(bank_order,i);
      mergePhi=getPattPhi(bank_order,i);
      mergeCoto=getPattCoto(bank_order,i);
      mergeZ=getPattZ(bank_order,i);
      for(int k=0;k<ndim;k++){
	mergevec[k]=getPatthitc(bank_order,i,k);
	mergexC[k]=getPattxC(bank_order,i,k);
	mergexD[k]=getPattxD(bank_order,i,k);
	mergexPhi[k]=getPattxPhi(bank_order,i,k);
	mergexCoto[k]=getPattxCoto(bank_order,i,k);
	mergexZ[k]=getPattxZ(bank_order,i,k);
	for(int l=0;l<ndim;l++){
	  mergecovx[k*ndim+l]=getPattcovx(bank_order,i,k*ndim+l);
	  log << MSG::DEBUG <<" i= "  << k*ndim+l <<"bank= "<< mergecovx[k*ndim+l] <<endmsg;
	}
	log <<getPatthitc(bank_order,i,k)<<" ";
      }

      log <<"  "<<endmsg;
      
      for(int m=0;m<mergenhit;m++){
	m_mergeintc->push_back(getPattintc(bank_order,i,m));
	m_mergeintphi->push_back(getPattintphi(bank_order,i,m));
	m_mergeintd0->push_back(getPattintd0(bank_order,i,m));
	m_mergeintz0->push_back(getPattintz0(bank_order,i,m));
	m_mergeinteta->push_back(getPattinteta(bank_order,i,m));
	      
      }
      
      m_tree->Fill();
    }

    file->cd();
    m_tree->Write();
    gROOT->cd();

    delete m_tree;
    m_tree=NULL;
  }

  free(mergeSec);
  free(mergehashID);
  free(mergevec);
  free(mergexC);
  free(mergexD);
  free(mergexPhi);
  free(mergexCoto);
  free(mergexZ);
  free(mergecovx);

  file->Close();
  delete file;
  file=NULL;
}

void FTKConstGenAlgo::constantgen()
{
  MsgStream log(msgSvc(), name());

  //
  // variables of matrix before merge
  //

  int ndim,tmp_ndim;
  int nplane,tmp_nplane;
  double *sum_Vec;
  int *sum_Sec;
  int *sum_Hash;
  double sum_C;
  double sum_D;
  double sum_Phi;
  double sum_Coto;
  double sum_Z;
  float sum_nhit;
  char region[5];
  int nsector;
  TTree *const_tree;
  double *sum_xC; 
  double *sum_xD;
  double *sum_xPhi;
  double *sum_xCoto;
  double *sum_xZ;
  double **sum_covx;
  double *sum_covx_i;
  double *sum_covx_v;
  double **ccopy;
  double *eigval;
  double *ccopy_v;
  double **eigvec;
  double *eigvec_v;
  int nconst=0;

  //slice file
  int nslices[5];
  double minvals[5];
  double maxvals[5];
  char s_filename[40];
  bool monFlag=false;
  
  std::vector<short >* sum_intc = new std::vector<short >;
  std::vector<short >* sum_intphi = new std::vector<short >;
  std::vector<short >* sum_intd0 = new std::vector<short >;
  std::vector<short >* sum_intz0 = new std::vector<short >;
  std::vector<short >* sum_inteta = new std::vector<short >;

  //
  // get information from input matrix file
  //
  
  sprintf(region,"am%d",m_region);
  // open the file with the matrix files after the merging
  TFile *mafile = 0x0;
  if (m_merging) {
    // if the merging was executed within this session the file was
    // created in the ::merge() routine according the specific flags
    mafile = TFile::Open(m_outfilename.c_str());
  }
  else {
    // if the merging was executed in a previous step the matrix files
    // is passed using the m_cfpath variable, controlled by JO
    mafile = TFile::Open(m_cfpath.c_str());
  }
  gROOT->cd();

  TTree *mslice_tree = (TTree*)mafile->Get("slice");
  const_tree = (TTree*)mafile->Get(region);
  nsector =const_tree->GetEntries();


  if(gROOT->FindObject("montree")){
    monFlag=true; 
  }

  TTree *m_montree=NULL;
  if(monFlag){
    TTree *m_montree_org = (TTree *)mafile->Get("montree");
    m_montree_org->SetName("montree_org");
    m_montree = m_montree_org->CloneTree();
    m_montree->SetName("montree");
    delete m_montree_org;
    m_montree_org=NULL;
  }

  const_tree->SetBranchAddress("ndim",&tmp_ndim);
  const_tree->SetBranchAddress("nplanes",&tmp_nplane);
  const_tree->GetEntry(0);
  ndim = tmp_ndim;
  nplane = tmp_nplane;
 
  sum_Sec = (int *) calloc(nplane,sizeof(int));
  sum_Hash = (int *) calloc(nplane,sizeof(int));
  sum_Vec = (double *) calloc(ndim,sizeof(double));
  sum_xC = (double *) calloc(ndim,sizeof(double));
  sum_xD = (double *) calloc(ndim,sizeof(double));
  sum_xPhi = (double *) calloc(ndim,sizeof(double));
  sum_xCoto = (double *) calloc(ndim,sizeof(double));
  sum_xZ = (double *) calloc(ndim,sizeof(double));
  sum_covx_i = (double *) calloc(ndim*ndim,sizeof(double));
  
  const_tree->SetBranchAddress("sectorID",sum_Sec);
  const_tree->SetBranchAddress("hashID",sum_Hash);
  const_tree->SetBranchAddress("tmpC",&sum_C);
  const_tree->SetBranchAddress("tmpD",&sum_D);
  const_tree->SetBranchAddress("tmpPhi",&sum_Phi);
  const_tree->SetBranchAddress("tmpCoto",&sum_Coto);
  const_tree->SetBranchAddress("tmpZ",&sum_Z);
  const_tree->SetBranchAddress("nhit",&sum_nhit);
  const_tree->SetBranchAddress("Vec",sum_Vec);
  const_tree->SetBranchAddress("tmpxC", sum_xC);
  const_tree->SetBranchAddress("tmpxD", sum_xD);
  const_tree->SetBranchAddress("tmpxPhi", sum_xPhi);
  const_tree->SetBranchAddress("tmpxCoto", sum_xCoto);
  const_tree->SetBranchAddress("tmpxZ", sum_xZ);
  const_tree->SetBranchAddress("tmpcovx", sum_covx_i);
  
  const_tree->SetBranchAddress("tmpintc", &sum_intc);
  const_tree->SetBranchAddress("tmpintphi", &sum_intphi);
  const_tree->SetBranchAddress("tmpintd0", &sum_intd0);
  const_tree->SetBranchAddress("tmpintz0", &sum_intz0);
  const_tree->SetBranchAddress("tmpinteta", &sum_inteta);
  
  mslice_tree->SetBranchAddress("c_max",&maxvals[0]);
  mslice_tree->SetBranchAddress("c_min", &minvals[0]);
  mslice_tree->SetBranchAddress("c_slices", &nslices[0]);
  mslice_tree->SetBranchAddress("d0_max", &maxvals[1]);
  mslice_tree->SetBranchAddress("d0_min", &minvals[1]);
  mslice_tree->SetBranchAddress("d0_slices", &nslices[1]);
  mslice_tree->SetBranchAddress("phi_max", &maxvals[2]);
  mslice_tree->SetBranchAddress("phi_min", &minvals[2]);
  mslice_tree->SetBranchAddress("phi_slices", &nslices[2]);
  mslice_tree->SetBranchAddress("z0_max", &maxvals[3]);
  mslice_tree->SetBranchAddress("z0_min", &minvals[3]);
  mslice_tree->SetBranchAddress("z0_slices", &nslices[3]);
  mslice_tree->SetBranchAddress("eta_max", &maxvals[4]);
  mslice_tree->SetBranchAddress("eta_min", &minvals[4]);
  mslice_tree->SetBranchAddress("eta_slices", &nslices[4]);
  
  mslice_tree->GetEntry(0); 

  eigval = (double *) calloc(ndim,sizeof(double));
  ccopy_v = (double *) calloc(ndim*ndim,sizeof(double));
  eigvec_v = (double *) calloc(ndim*ndim,sizeof(double));
  sum_covx_v = (double *) calloc(ndim*ndim,sizeof(double));  
  ccopy = (double **) malloc(ndim*sizeof(double*));
  eigvec = (double **) malloc(ndim*sizeof(double*));
  sum_covx = (double **) malloc(ndim*sizeof(double*));

  for(int i=0;i<ndim;++i){
    sum_covx[i] =(double *) calloc(ndim,sizeof(double));    
    ccopy[i] =(double *) calloc(ndim,sizeof(double));
    eigvec[i] =(double *) calloc(ndim,sizeof(double));    
    sum_covx[i] = &sum_covx_v[ndim*i];
    ccopy[i]=  &ccopy_v[ndim*i];
    eigvec[i]= &eigvec_v[ndim*i]; 
  }

  geo_const =(struct geo_constants *) malloc(nsector*sizeof(struct geo_constants));
  gcorth =(struct geo_constants *) malloc(nsector*sizeof(struct geo_constants));
    
  for(int i=0;i<nsector;i++){
    geo_const[i].Vd = (double *) calloc(ndim,sizeof(double));
    geo_const[i].Vc = (double *) calloc(ndim,sizeof(double));
    geo_const[i].Vf = (double *) calloc(ndim,sizeof(double));
    geo_const[i].Vz0 = (double *) calloc(ndim,sizeof(double));
    geo_const[i].Vo = (double *) calloc(ndim,sizeof(double));
    geo_const[i].kernel = (double **) malloc((ndim-NPARS)*sizeof(double *));
    gcorth[i].Vd = (double *) malloc(ndim*sizeof(double));
    gcorth[i].Vc = (double *) malloc(ndim*sizeof(double));
    gcorth[i].Vf = (double *) malloc(ndim*sizeof(double));
    gcorth[i].Vz0 = (double *) malloc(ndim*sizeof(double));
    gcorth[i].Vo = (double *) malloc(ndim*sizeof(double));
    gcorth[i].kernel = (double **) malloc((ndim-NPARS)*sizeof(double *));
    geo_const[i].kaverages = (double *) calloc((ndim-NPARS),sizeof(double));      
    gcorth[i].kaverages = (double *) calloc((ndim-NPARS),sizeof(double));
    for(int j=0;j<ndim-NPARS;j++){
      geo_const[i].kernel[j] = (double *) calloc(ndim,sizeof(double));
      gcorth[i].kernel[j] = (double *) calloc(ndim,sizeof(double));
	
    }
  }
            
  minvals[4]=sinh(minvals[4]);
  maxvals[4]=sinh(maxvals[4]);
  maxvals[2]= maxvals[2]*M_PI;
  minvals[2]= minvals[2]*M_PI;

  double resolutions[14] = {0.04, 0.08265625, 0.04, 0.08265625, 0.04, 0.08265625, 0.04515625, 0.04515625, 0.04515625, 0.04515625, 0.04515625, 0.04515625, 0.04515625, 0.04515625};
#ifdef DEBUG_NOISE
  float n[30];
#endif
  
  int nkernel=(ndim-NPARS)*ndim;
  int nkaverages=ndim-NPARS;
  int ndim2=ndim*ndim;
  char filename[40];
  char name[5];
  char title[20];
  double Cd,Cc,Cf,Cz0,Co;
  double *Vc,*Vd,*Vf,*Vz0,*Vo;
  double *kernel,*kaverages;
        
  Vc = (double *) calloc(ndim,sizeof(double));
  Vd = (double *) calloc(ndim,sizeof(double));
  Vf = (double *) calloc(ndim,sizeof(double));
  Vz0 = (double *) calloc(ndim,sizeof(double));
  Vo = (double *) calloc(ndim,sizeof(double));  
    
  kernel = (double *) calloc(ndim*(ndim-NPARS),sizeof(double));  
  kaverages = (double *) calloc((ndim-NPARS),sizeof(double));  
    
  ///////// root file part
    
  sprintf(filename,"const_%dL_%dDim_reg%d.root",nplane,ndim,m_region);
  cfile = new TFile(filename,"recreate");
  gROOT->cd();
  
  sprintf(name,"Am%d",m_region);
  sprintf(title,"Ambank %d const",m_region);
  m_ctree = new TTree(name, title);
       
  m_ctree->Branch("ndim",&ndim,"ndim/I");
  m_ctree->Branch("ndim2",&ndim2,"ndim2/I");
  m_ctree->Branch("nkernel",&nkernel,"nkernel/I");
  m_ctree->Branch("nkaverages",&nkaverages,"nkaverages/I");
  m_ctree->Branch("nplane",&nplane,"nplane/I");
  m_ctree->Branch("sectorID",sum_Sec,"m_sectorID[nplane]/I");
  m_ctree->Branch("hashID",sum_Hash,"m_hashID[nplane]/I");
  m_ctree->Branch("nhit",&sum_nhit,"nhit/F");
  m_ctree->Branch("Cd", &Cd,"Cd/D");
  m_ctree->Branch("Cc", &Cc,"Cc/D");
  m_ctree->Branch("Cf", &Cf,"Cf/D");
  m_ctree->Branch("Cz0", &Cz0,"Cz0/D");
  m_ctree->Branch("Co", &Co,"Co/D");
  m_ctree->Branch("Vc",Vc,"Vc[ndim]/D");
  m_ctree->Branch("Vd",Vd,"Vd[ndim]/D");
  m_ctree->Branch("Vf",Vf,"Vf[ndim]/D");
  m_ctree->Branch("Vz0",Vz0,"Vz0[ndim]/D");
  m_ctree->Branch("Vo",Vo,"Vo[ndim]/D");
  m_ctree->Branch("kernel",kernel,"kernel[nkernel]/D");
  m_ctree->Branch("kaverages",kaverages,"kaverages[nkaverages]/D");


  //
  // keep matrix information with "good" sectors to extract 8L from this matrix file
  //

  // matrix files with good sectors
  TTree *m_good_tree=NULL;
  TTree *m_good_slice=NULL;

  double good_C;
  double good_D;
  double good_Phi;
  double good_Coto;
  double good_Z;

  double *good_Vec;
  double *good_xC; 
  double *good_xD;
  double *good_xPhi;
  double *good_xCoto;
  double *good_xZ;
  double *good_covx_i;

  good_Vec = (double *) calloc(ndim,sizeof(double));
  good_xC = (double *) calloc(ndim,sizeof(double));
  good_xD = (double *) calloc(ndim,sizeof(double));
  good_xPhi = (double *) calloc(ndim,sizeof(double));
  good_xCoto = (double *) calloc(ndim,sizeof(double));
  good_xZ = (double *) calloc(ndim,sizeof(double));
  good_covx_i = (double *) calloc(ndim*ndim,sizeof(double));
  

  if (m_outfilename.size()==0) {
    // if the output file is not specified the name is built according a specific conention
    char filename[40];
    if(true==m_allregion){
      sprintf(filename,"good_matrix_%dL_%dDim_regall.root",nplane,ndim);
    }else{
      sprintf(filename,"good_matrix_%dL_%dDim_reg%d.root",nplane,ndim,m_region);
    }
    m_outfilename = filename;
  }

  // open the output file for the good matrixes
  good_file = TFile::Open(m_outfilename.c_str(),"recreate");
  gROOT->cd();

  m_good_slice=new TTree("slice", "slice para");
  
  m_good_slice->Branch("c_max",&maxvals[0]);
  m_good_slice->Branch("d0_max",&maxvals[1]);
  m_good_slice->Branch("phi_max",&maxvals[2]);
  m_good_slice->Branch("z0_max",&maxvals[3]);
  m_good_slice->Branch("eta_max",&maxvals[4]);
  
  m_good_slice->Branch("c_min",&minvals[0]);
  m_good_slice->Branch("d0_min",&minvals[1]);
  m_good_slice->Branch("phi_min",&minvals[2]);
  m_good_slice->Branch("z0_min",&minvals[3]);
  m_good_slice->Branch("eta_min",&minvals[4]);
  
  m_good_slice->Branch("c_slices",&nslices[0]);
  m_good_slice->Branch("d0_slices",&nslices[1]);
  m_good_slice->Branch("phi_slices",&nslices[2]);
  m_good_slice->Branch("z0_slices",&nslices[3]); 
  m_good_slice->Branch("eta_slices",&nslices[4]);    

  m_good_slice->Fill();
  
  good_file->cd();
  m_good_slice->Write();
  if(monFlag){
    m_montree->Write();
  }
  gROOT->cd();

  sprintf(name,"am%d",m_region);
  sprintf(title,"Ambank %d para",m_region);
  
  m_good_tree = new TTree(name, title);

  m_good_tree->Branch("ndim",&ndim,"ndim/I");
  m_good_tree->Branch("ndim2",&ndim2,"ndim2/I");
  m_good_tree->Branch("Vec", good_Vec,"Vec[ndim]/D");
  m_good_tree->Branch("nplanes",&nplane,"nplanes/I");
  m_good_tree->Branch("sectorID", sum_Sec,"sectorID[nplanes]/I");
  m_good_tree->Branch("hashID", sum_Hash,"hashID[nplanes]/I");
  m_good_tree->Branch("nhit", &sum_nhit,"nhit/F");
  m_good_tree->Branch("tmpC", &good_C,"tmpC/D");
  m_good_tree->Branch("tmpD", &good_D,"tmpD/D");
  m_good_tree->Branch("tmpPhi", &good_Phi,"tmpPhi/D");
  m_good_tree->Branch("tmpCoto", &good_Coto,"tmpCoto/D");
  m_good_tree->Branch("tmpZ", &good_Z,"tmpZ/D");
  m_good_tree->Branch("tmpxC", good_xC,"tmpxC[ndim]/D");
  m_good_tree->Branch("tmpxD", good_xD,"tmpxD[ndim]/D");
  m_good_tree->Branch("tmpxPhi", good_xPhi,"tmpxPhi[ndim]/D");
  m_good_tree->Branch("tmpxCoto", good_xCoto,"tmpxCoto[ndim]/D");
  m_good_tree->Branch("tmpxZ", good_xZ,"tmpxZ[ndim]/D");
  m_good_tree->Branch("tmpcovx", good_covx_i,"tmpcovx[ndim2]/D");
  
  m_good_tree->Branch("tmpintc", &sum_intc);
  m_good_tree->Branch("tmpintphi", &sum_intphi);
  m_good_tree->Branch("tmpintd0", &sum_intd0);
  m_good_tree->Branch("tmpintz0", &sum_intz0);
  m_good_tree->Branch("tmpinteta", &sum_inteta);
  
  //
  // calcurate constants start
  //

  for(int sector=0;sector<nsector;sector++){
      
    const_tree->GetEntry(sector);

    // keep original values before calcuration for "good" matrix
    good_C = sum_C;
    good_D = sum_D;
    good_Phi = sum_Phi;
    good_Coto = sum_Coto;
    good_Z = sum_Z;

    for(int i=0;i<ndim;++i){
      good_Vec[i]= sum_Vec[i];
      good_xC[i] = sum_xC[i];
      good_xD[i] = sum_xD[i];
      good_xPhi[i] = sum_xPhi[i];
      good_xCoto[i] = sum_xCoto[i];
      good_xZ[i] = sum_xZ[i];

      for(int j=0;j<ndim;++j){
	good_covx_i[ndim*i+j]=sum_covx_i[ndim*i+j];
      }

    }

    for(int i=0;i<ndim;++i){
      for(int j=0;j<ndim;++j){
	sum_covx[i][j]=sum_covx_i[ndim*i+j];
      }
    }

    // don't leave unitialized variables
    // fill of zero, matrix ad vector
      
    if ( sum_nhit<15/*MIN_TRACK_SEC*/) {
      //      puts("Not enough tracks!");
      nullifyConstants(sector,ndim);
      continue;
    } // end insufficient track case
      
      //////////////////////////////////////////////////////  
      ///  renormalize and diagonalize correlation matrixes and vector, 
      ///  for all sector
      //////////////////////////////////////////////////////
      
    sum_C /= sum_nhit;
    sum_D /= sum_nhit;
    sum_Phi /= sum_nhit;
    sum_Coto /= sum_nhit;
    sum_Z /= sum_nhit;  

    for (int i=0 ; i<ndim ; ++i ) {
      sum_Vec[i] /= sum_nhit;    
    }
    // traslate parameters
      
    for (int i=0;i<ndim;++i) {
      sum_xC[i] = (sum_xC[i]-sum_Vec[i]*sum_C*sum_nhit)/(sum_nhit-1);
      sum_xD[i] =(sum_xD[i]-sum_Vec[i]*sum_D*sum_nhit)/(sum_nhit-1);
      sum_xPhi[i] =(sum_xPhi[i]-sum_Vec[i]*sum_Phi*sum_nhit)/(sum_nhit-1);
      sum_xZ[i] = (sum_xZ[i]-sum_Vec[i]*sum_Z*sum_nhit)/(sum_nhit-1);
      sum_xCoto[i] =(sum_xCoto[i]-sum_Vec[i]*sum_Coto*sum_nhit)/(sum_nhit-1);
	
      // symmetrize

      for (int j=i;j<ndim;++j) {
	ccopy[j][i] = ccopy[i][j] = 
	  sum_covx[j][i] = sum_covx[i][j] =
	  (sum_covx[i][j]-sum_Vec[i]*sum_Vec[j]*sum_nhit)/(sum_nhit-1);
      }
    }

    //FlagAK - covering the case when matrix is singular (i.e., if linearity is too good due to small # of tracks)
    if(mtx_eigen(ndim,sum_covx_v,eigval,eigvec_v)==-1 || mtx_invert(ndim,ccopy_v)==-1) {
      log << MSG::INFO <<" sector " << sector << endmsg;
      puts("Failed matrix operations!");
      nullifyConstants(sector,ndim);
      continue;
    }

    // Get the "noise" contribution
    // Diagonal resolutions:
    for(int i=0;i<ndim;i++) {
      double noise_i = 0;
      for(int j=0;j<ndim;j++){
	noise_i += eigvec[i][j] * resolutions[j] * eigvec[i][j];
      }
#ifdef DEBUG_NOISE
      n[16+i] = noise_i;
      //printf("% 8f ",noise_i);
#endif
    }
       
    sigmad(geo_const[sector].Vc/*,sum_covx*/,ccopy,sum_xC,ndim);
    sigmad(geo_const[sector].Vd/*,sum_covx*/,ccopy,sum_xD,ndim);
    sigmad(geo_const[sector].Vf/*,sum_covx*/,ccopy,sum_xPhi,ndim);
    sigmad(geo_const[sector].Vz0/*,sum_covx*/,ccopy,sum_xZ,ndim);
    sigmad(geo_const[sector].Vo/*,sum_covx*/,ccopy,sum_xCoto,ndim);

    for(int i=0;i<ndim-NPARS;i++) for(int j=0;j<ndim;j++)
      geo_const[sector].kernel[i][j] = eigvec[i][j];

    for(int i=0;i<ndim-NPARS;i++)
      geo_const[sector].kaverages[i] = 0;
    
    for(int i=0;i<ndim-NPARS;i++) {
      for(int j=0;j<ndim;j++){
	geo_const[sector].kernel[i][j] = eigvec[i][j]/sqrt(eigval[i]);
      }
    }
        
    for(int i=0;i<ndim-NPARS;i++) for(int j=0;j<ndim;j++)
      geo_const[sector].kaverages[i] +=
	-geo_const[sector].kernel[i][j]*sum_Vec[j];
    
    geo_const[sector].Cd = sum_D;
    geo_const[sector].Cc = sum_C;
    geo_const[sector].Cf = sum_Phi;
    geo_const[sector].Cz0 = sum_Z;
    geo_const[sector].Co = sum_Coto;
    
    for(int i=0;i<ndim;i++)
      geo_const[sector].Cd += -geo_const[sector].Vd[i]*sum_Vec[i];
    
    for(int i=0;i<ndim;i++)
      geo_const[sector].Cc += -geo_const[sector].Vc[i]*sum_Vec[i];

    for(int i=0;i<ndim;i++)
      geo_const[sector].Cf += -geo_const[sector].Vf[i]*sum_Vec[i];

    for(int i=0;i<ndim;i++)
      geo_const[sector].Cz0 += -geo_const[sector].Vz0[i]*sum_Vec[i];

    for(int i=0;i<ndim;i++)
      geo_const[sector].Co += -geo_const[sector].Vo[i]*sum_Vec[i];

    geo_const[sector].real = sum_nhit; // valid set of contants
    
    //now orthogonalize 
     
#define gc (geo_const[sector])
#define gco (gcorth[sector])
     
    for(int j=0;j<ndim;j++) {
      gco.Vd[j] = gc.Vd[j]; 
      gco.Vc[j] = gc.Vc[j];
      gco.Vf[j] = gc.Vf[j];
      gco.Vz0[j] = gc.Vz0[j];
      gco.Vo[j] = gc.Vo[j];
    }
       
    gco.Cd = gc.Cd; gco.Cc = gc.Cc; gco.Cf = gc.Cf;
    gco.Co = gc.Co; gco.Cz0 = gc.Cz0;
    gco.real = gc.real;

    for(int i=0;i<ndim-NPARS;i++) {
      pr = proj(gc.Vd,gc.kernel[i],ndim)/proj(gc.kernel[i],gc.kernel[i],ndim);
      for(int j=0;j<ndim;j++) gco.Vd[j] += -gc.kernel[i][j]*pr;
      gco.Cd += -gc.kaverages[i]*pr;
      
      pr = proj(gc.Vc,gc.kernel[i],ndim)/proj(gc.kernel[i],gc.kernel[i],ndim);
      for(int j=0;j<ndim;j++) gco.Vc[j] += - gc.kernel[i][j]*pr;
      gco.Cc += -gc.kaverages[i]*pr;
	  
      pr = proj(gc.Vf,gc.kernel[i],ndim)/proj(gc.kernel[i],gc.kernel[i],ndim);
      for(int j=0;j<ndim;j++) gco.Vf[j] += -gc.kernel[i][j]*pr;
      gco.Cf += -gc.kaverages[i]*pr;

      pr = proj(gc.Vz0,gc.kernel[i],ndim)/proj(gc.kernel[i],gc.kernel[i],ndim);
      for(int j=0;j<ndim;j++) gco.Vz0[j] += -gc.kernel[i][j]*pr;
      gco.Cz0 += -gc.kaverages[i]*pr;

      pr = proj(gc.Vo,gc.kernel[i],ndim)/proj(gc.kernel[i],gc.kernel[i],ndim);
      for(int j=0;j<ndim;j++) gco.Vo[j] += -gc.kernel[i][j]*pr;
      gco.Co += -gc.kaverages[i]*pr;
      
        
      // Copy the kernel 
      for(int j=0;j<ndim;j++) gco.kernel[i][j] = gc.kernel[i][j];
      gco.kaverages[i] = gc.kaverages[i];
    }

    // FlagAK - final check to make sure the constants have no "nan" values
    if(failedConstants(sector,ndim)) {
      log << MSG::INFO <<" sector " << sector << endmsg;
      puts("Found nans in final writeout!");
      nullifyConstants(sector,ndim);
      continue;
    }
    else {
      
      Cc=geo_const[sector].Cc;
      Cd=geo_const[sector].Cd;
      Cf=geo_const[sector].Cf;
      Cz0=geo_const[sector].Cz0;
      Co=geo_const[sector].Co;

      for(int i=0;i<ndim;i++){
	Vc[i]=geo_const[sector].Vc[i];
	Vd[i]=geo_const[sector].Vd[i];
	Vf[i]=geo_const[sector].Vf[i];
	Vz0[i]=geo_const[sector].Vz0[i];
	Vo[i]=geo_const[sector].Vo[i];
      }
      for(int i=0;i<ndim-NPARS;i++){ 
	kaverages[i]=geo_const[sector].kaverages[i];
      }
	
      for(int i=0;i<ndim-NPARS;i++){ 
	for(int j=0;j<ndim;j++){ 
	  kernel[i*ndim+j]=geo_const[sector].kernel[i][j];
	}
      }
      m_ctree->Fill();
      nconst++;

      //fill matrix
      m_good_tree->Fill();

      // Record information on eigenvalues and noise
#ifdef DEBUG_NOISE
      n[0] = sector;
      n[1] = sum_nhit;
      n[2] = eigval[0];
      n[3] = eigval[1];
      n[4] = eigval[2];
      n[5] = eigval[3];
      n[6] = eigval[4];
      n[7] = eigval[5];
      n[8] = eigval[6];
      n[9] = eigval[7];
      n[10] = eigval[8];
      n[11] = eigval[9];
      n[12] = eigval[10];
      n[13] = eigval[11];
      n[14] = eigval[12];
      n[15] = eigval[13];
      // n[16] - n[29] recorded above for noise
#endif
    }

  }// for sector

  //
  // for good matrix
  //

  good_file->cd();
  m_good_tree->Write();
  gROOT->cd();
  
  delete m_good_tree;
  m_good_tree=NULL;

  //
  // const.root    
  //
  cfile->cd();
  m_ctree->Write();
  gROOT->cd();
  cfile->Close();
  

  ////// ASCII file part (.gcon and .patt)
  FILE *sector_file;
  FILE *sectorHW_file;
  FILE *const_file;
  char sector_filename[40];
  char sectorHW_filename[40];
  char const_filename[40];
  int nconst1=0;
    
  //slice file
  initNSlices(1,nslices,minvals,maxvals);
  initSlices(0,nconst);
       
  // slice file
    
  sprintf(const_filename,"corrgen_raw_%dL_reg%d.gcon",nplane,m_region);
  sprintf(sector_filename,"sectors_raw_%dL_reg%d.patt",nplane,m_region);
  sprintf(sectorHW_filename,"sectorsHW_raw_%dL_reg%d.patt",nplane,m_region);
  sector_file = fopen(sector_filename,"w");
  sectorHW_file = fopen(sectorHW_filename,"w");
  const_file = fopen(const_filename,"w");
    
  fprintf(const_file,"! ***           RECONSTRUCTION GEOMETRY CONSTANTS               ***\n");
  fprintf(const_file,"\n");
  fprintf(const_file,"Version 2       ! File format version number\n");
  fprintf(const_file,"\n");
  fprintf(const_file,"! ***           PHI is now in GLOBAL coordinate system          ***\n");
  fprintf(const_file," NPLANES\n");
  fprintf(const_file," %d\n",nplane);
  fprintf(const_file," NSECTORS\n");
  fprintf(const_file,"%d\n",nconst);
  fprintf(const_file," NDIM\n");
  fprintf(const_file," 2\n");
    
  fprintf(sector_file,"%d %d\n",nconst,nplane);
  fprintf(sectorHW_file,"%d %d\n",nconst,nplane);
    
  for(int sector=0;sector<nsector;sector++){
    const_tree->GetEntry(sector);
    if(geo_const[sector].Cc!=0){
	
      // sector file
      fprintf(sector_file,"%d ",nconst1);
      fprintf(sectorHW_file,"%d ",nconst1);
      for(int i=0;i<nplane;i++){
	fprintf(sector_file,"%d ",sum_Sec[i]);
	fprintf(sectorHW_file,"%d ",sum_Hash[i]);
      }
      int nhit=sum_nhit;
      fprintf(sector_file,"0 %d",nhit);
      fprintf(sector_file,"\n");
      fprintf(sectorHW_file,"0 %d",nhit);
      fprintf(sectorHW_file,"\n");
      // sector file
	
      //gcon file  
	
      fprintf(const_file,"sector\n");
      fprintf(const_file,"%d\n",nconst1);
	
      fprintf(const_file," Vc \n");
      for(int i=0;i<ndim;i++){
	fprintf(const_file,"%e\n",geo_const[sector].Vc[i]);
      }
	
      fprintf(const_file," Vd \n");
      for(int i=0;i<ndim;i++){
	fprintf(const_file,"%e\n",geo_const[sector].Vd[i]);
      }

      fprintf(const_file," Vf \n");
      for(int i=0;i<ndim;i++){
	fprintf(const_file,"%e\n",geo_const[sector].Vf[i]);
      }

      fprintf(const_file," Vz0 \n");
      for(int i=0;i<ndim;i++){
	fprintf(const_file,"%e\n",geo_const[sector].Vz0[i]);
      }

      fprintf(const_file," Vo \n");
      for(int i=0;i<ndim;i++){
	fprintf(const_file,"%e\n",geo_const[sector].Vo[i]);
      }
	
      fprintf(const_file,"kaverages\n");
      for(int i=0;i<ndim-NPARS;i++){ 
	fprintf(const_file,"%e\n",geo_const[sector].kaverages[i]);
      }
	
      fprintf(const_file,"kernel\n");
      for(int i=0;i<ndim-NPARS;i++){ 
	for(int j=0;j<ndim;j++){ 
	  fprintf(const_file,"%e\n",geo_const[sector].kernel[i][j]);
	}
      }
	
      fprintf(const_file,"Cc\n");
      fprintf(const_file,"%e\n",geo_const[sector].Cc);
	
      fprintf(const_file,"Cd\n");
      fprintf(const_file,"%e\n",geo_const[sector].Cd);

      fprintf(const_file,"Cf\n");
      fprintf(const_file,"%e\n",geo_const[sector].Cf);
	
      fprintf(const_file,"Cz0\n");
      fprintf(const_file,"%e\n",geo_const[sector].Cz0);

      fprintf(const_file,"Co\n");
      fprintf(const_file,"%e\n",geo_const[sector].Co);
      //gcon file
	
      for(int i=0;i<nhit;i++){
	addSectorToSlice(0,nconst1,sum_intc[0][i],sum_intd0[0][i],sum_intphi[0][i],sum_intz0[0][i],sum_inteta[0][i]);
      }
      nconst1++;
    }//Cc!=0
  }//sector loop
    
  mafile->Close();
  fclose(sector_file);
  fclose(sectorHW_file);
  fclose(const_file);

  sprintf(s_filename,"slices_%dL_reg%d.root",nplane,m_region);
  saveSlices(0,s_filename);
  //////ASCII file part
  for (int i=0;i<nsector;++i) {
      
    for (int j=0;j<ndim-NPARS;++j) {
      free(geo_const[i].kernel[j]);
      free(gcorth[i].kernel[j]);
    }
    // free arrays of array
    free(geo_const[i].kernel);
    free(geo_const[i].Vc);
    free(geo_const[i].Vd);
    free(geo_const[i].Vf);
    free(geo_const[i].Vz0);
    free(geo_const[i].Vo);
      
    free(gcorth[i].kernel);
    free(gcorth[i].Vc);
    free(gcorth[i].Vd);
    free(gcorth[i].Vf);
    free(gcorth[i].Vz0);    
    free(gcorth[i].Vo);
      
    free(geo_const[i].kaverages);
    free(gcorth[i].kaverages);
      
  }

}//const gen


/** Extract a reduce set of constants from sectors built using
 * more modules
 */
void FTKConstGenAlgo::extract_1stStage()
{
  MsgStream log(msgSvc(), name());

  // retrieve the original file
  TFile *Lfile = TFile::Open(m_extractpath.c_str(),"");
  // retrieve the TTree for the slices and the information on the sectors
  TTree *tree = (TTree*)Lfile->Get(Form("am%d",m_region));
  TTree *slice_tree = (TTree*) Lfile->Get("slice");

  for(int idx=0; idx<(int)vec_plane_index_2nd_stage.size(); idx++){
    log << MSG::INFO <<"2nd stage layer :" << vec_plane_index_2nd_stage[idx] << ", 1st stage layer: "<< vec_plane_index_1st_stage[idx] << ", doInvert: " << vec_doInvert[idx] << ", coord 2nd stage: " << vec_coord_index_2nd_stage[idx] << ", coord 1st stage: " << vec_coord_index_1st_stage[idx] << endmsg;
  }

  // prepare the pointers to retrieve the data from the TTrees
  int nevent;
  int npatterns=0;
  int ndim;
  int nth_dim  = 0;
  int nth_dim2 = 0;
  int ndim2;
  int *sectorID;
  int *hashID;
  double *vec; 
  double tmpC;
  double tmpD;
  double tmpPhi;
  double tmpCoto;
  double tmpZ;
  float nhit;
  double *tmpxC;
  double *tmpxD;
  double *tmpxPhi;
  double *tmpxCoto;
  double *tmpxZ;
  double *tmpcovx;

  int ndim8 = 11;
  int ndim2_8 = 11*11;
  int nth_dim8 = 0;
  int nth_dim8_2 = 0;
  int idx_cov8 = 0;
  int *sectorID8;
  int *hashID8;
  double *vec8; 
  double *tmpxC8;
  double *tmpxD8;
  double *tmpxPhi8;
  double *tmpxCoto8;
  double *tmpxZ8;
  double *tmpcovx8;

  std::vector<short >* tmp_intc = new std::vector<short >;
  std::vector<short >* tmp_intphi = new std::vector<short >;
  std::vector<short >* tmp_intd0 = new std::vector<short >;
  std::vector<short >* tmp_intz0 = new std::vector<short >;
  std::vector<short >* tmp_inteta = new std::vector<short >;
  
  matrix Mtmp;
  
  double tmp_cmax;
  double tmp_cmin;
  int tmp_cslices;
  
  double tmp_phimax;
  double tmp_phimin;
  int tmp_phislices;
  
  double tmp_d0max;
  double tmp_d0min;
  int tmp_d0slices;
  
  double tmp_z0max;
  double tmp_z0min;
  int tmp_z0slices;
  
  double tmp_etamax;
  double tmp_etamin;
  int tmp_etaslices;
  
  //
  // read matrix file
  //

  tree->SetBranchAddress("ndim",&ndim);
  tree->SetBranchAddress("nplanes",&nplane);
  tree->SetBranchAddress("ndim2",&ndim2);
  tree->GetEntry(0); 
  
  slice_tree->SetBranchAddress("c_max",&tmp_cmax);
  slice_tree->SetBranchAddress("c_min", &tmp_cmin);
  slice_tree->SetBranchAddress("c_slices", &tmp_cslices);
  slice_tree->SetBranchAddress("phi_max", &tmp_phimax);
  slice_tree->SetBranchAddress("phi_min", &tmp_phimin);
  slice_tree->SetBranchAddress("phi_slices", &tmp_phislices);
  slice_tree->SetBranchAddress("d0_max", &tmp_d0max);
  slice_tree->SetBranchAddress("d0_min", &tmp_d0min);
  slice_tree->SetBranchAddress("d0_slices", &tmp_d0slices);
  slice_tree->SetBranchAddress("z0_max", &tmp_z0max);
  slice_tree->SetBranchAddress("z0_min", &tmp_z0min);
  slice_tree->SetBranchAddress("z0_slices", &tmp_z0slices);
  slice_tree->SetBranchAddress("eta_max", &tmp_etamax);
  slice_tree->SetBranchAddress("eta_min", &tmp_etamin);
  slice_tree->SetBranchAddress("eta_slices", &tmp_etaslices);
  slice_tree->GetEntry(0); 
  
  sectorID = (int *) calloc(nplane+1,sizeof(int));  
  hashID = (int *) calloc(nplane+1,sizeof(int));  
  vec = (double *) calloc(ndim,sizeof(double));  
  tmpxC = (double *) calloc(ndim,sizeof(double));  
  tmpxD = (double *) calloc(ndim,sizeof(double));
  tmpxPhi = (double *) calloc(ndim,sizeof(double));  
  tmpxCoto = (double *) calloc(ndim,sizeof(double));  
  tmpxZ = (double *) calloc(ndim,sizeof(double));  
  tmpcovx = (double *) calloc(ndim2,sizeof(double));  

  sectorID8 = (int *) calloc(nplane8+1,sizeof(int));  
  hashID8 = (int *) calloc(nplane8+1,sizeof(int));  
  vec8 = (double *) calloc(ndim8,sizeof(double));  
  tmpxC8 = (double *) calloc(ndim8,sizeof(double));  
  tmpxD8 = (double *) calloc(ndim8,sizeof(double));
  tmpxPhi8 = (double *) calloc(ndim8,sizeof(double));  
  tmpxCoto8 = (double *) calloc(ndim8,sizeof(double));  
  tmpxZ8 = (double *) calloc(ndim8,sizeof(double));  
  tmpcovx8 = (double *) calloc(ndim8*ndim8,sizeof(double));  

  // setup the boundaries for the merge, it can be a single region or a range
  int b_int;
  int b_end;
  
  int bank_order;
  if(true==m_allregion){
    b_int=0;
    b_end=m_nbank;
  }else{
    b_int=m_region; 
    b_end=m_region+1; 
  }

  initPattTree(m_nbank,nplane8+1,ndim8);  //(nregions, nplane+1, ndim)

  for(int b=b_int;b<b_end;b++) { // loop over the banks

    TTree *am_tree = (TTree*)Lfile->Get(Form("am%d",b));

    nevent =am_tree->GetEntries();
    am_tree->SetBranchAddress("sectorID",sectorID);
    am_tree->SetBranchAddress("hashID",hashID);
    am_tree->SetBranchAddress("tmpC",&tmpC);
    am_tree->SetBranchAddress("tmpD",&tmpD);
    am_tree->SetBranchAddress("tmpPhi",&tmpPhi);
    am_tree->SetBranchAddress("tmpCoto",&tmpCoto);
    am_tree->SetBranchAddress("tmpZ",&tmpZ);
    am_tree->SetBranchAddress("nhit",&nhit);
    am_tree->SetBranchAddress("Vec",vec);
    am_tree->SetBranchAddress("tmpxC", tmpxC);
    am_tree->SetBranchAddress("tmpxD", tmpxD);
    am_tree->SetBranchAddress("tmpxPhi", tmpxPhi);
    am_tree->SetBranchAddress("tmpxCoto", tmpxCoto);
    am_tree->SetBranchAddress("tmpxZ", tmpxZ);
    am_tree->SetBranchAddress("tmpcovx", tmpcovx);
  
    am_tree->SetBranchAddress("tmpintc", &tmp_intc);
    am_tree->SetBranchAddress("tmpintphi", &tmp_intphi);
    am_tree->SetBranchAddress("tmpintd0", &tmp_intd0);
    am_tree->SetBranchAddress("tmpintz0", &tmp_intz0);
    am_tree->SetBranchAddress("tmpinteta", &tmp_inteta);
      
    log << MSG::INFO <<  " region: " << b << ", nevent: " << nevent <<endmsg;

    //
    // extract 8L from 12L
    //

    for(int j=0;j<nevent;j++){ // event loop

      am_tree->GetEntry(j);
      Mtmp.C=tmpC, Mtmp.D=tmpD, Mtmp.Phi=tmpPhi;
      Mtmp.Coto=tmpCoto, Mtmp.Z=tmpZ,Mtmp.nhit=nhit;

      // check if the module belongs to endcap (for endcap inversion.).
      bool isEndcap[12]={0};

      for(int plane_idx_2nd_stage = 0;plane_idx_2nd_stage<nplane;plane_idx_2nd_stage++){
      	if(sectorID[plane_idx_2nd_stage]%1000>20) isEndcap[plane_idx_2nd_stage]=true;
      }

      // main function extract 1st stage (8L) from 2nd stage (12L)
      for(int idx_layer=0; idx_layer<(int)vec_plane_index_2nd_stage.size(); idx_layer++){
	
	if(vec_doInvert[idx_layer] && isEndcap[vec_plane_index_2nd_stage[idx_layer]]){// if endcap and SCT layer

	  // stereo or axial
	  if(vec_plane_index_2nd_stage[idx_layer] % 2 == 0) inversion = 1;
	  else inversion = -1;

	}else{// if no inversion
	  inversion = 0;
	}// endcap inversion

	sectorID8[idx_layer] = sectorID[vec_plane_index_2nd_stage[idx_layer] + inversion];
	hashID8[idx_layer] = hashID[vec_plane_index_2nd_stage[idx_layer] + inversion];
	
	for(int ndm=0; ndm<vec_ndimension[idx_layer]; ndm++){// pixel:2, SCT:1

	  nth_dim  = vec_coord_index_2nd_stage[idx_layer] + ndm;
	  nth_dim8 = vec_coord_index_1st_stage[idx_layer] + ndm;

	  vec8[nth_dim8] = vec[nth_dim + inversion];
	  tmpxC8[nth_dim8] = tmpxC[nth_dim + inversion];
	  tmpxD8[nth_dim8] = tmpxD[nth_dim + inversion];
	  tmpxPhi8[nth_dim8] = tmpxPhi[nth_dim + inversion];
	  tmpxCoto8[nth_dim8] = tmpxCoto[nth_dim + inversion];
	  tmpxZ8[nth_dim8] = tmpxZ[nth_dim + inversion];

	  // tmpcovx culcuration
	  for(int idx2_layer=0; idx2_layer<(int)vec_plane_index_2nd_stage.size(); idx2_layer++){
	    for(int ndm2=0; ndm2<vec_ndimension[idx2_layer]; ndm2++){
	      
	      if(vec_doInvert[idx2_layer] && isEndcap[vec_plane_index_2nd_stage[idx2_layer]]){//if endcap and SCT layer
		
		// stereo or axial
		if(vec_plane_index_2nd_stage[idx2_layer] % 2 == 0) inversion2 = 1;
		else inversion2 = -1;
		
	      }else{// if no inversion
		inversion2 = 0;
	      }// endcap inversion

	      nth_dim2   = vec_coord_index_2nd_stage[idx2_layer] + ndm2;
	      nth_dim8_2 = vec_coord_index_1st_stage[idx2_layer] + ndm2;

	      idx_cov8 = (nth_dim8) * ndim8 + nth_dim8_2;//11*x + y
	      
	      if(nth_dim8>nth_dim8_2){//make triangle
		tmpcovx8[idx_cov8] = 0;
	      }else{
		tmpcovx8[idx_cov8] = tmpcovx[(nth_dim + inversion) * ndim + nth_dim2 + inversion2];
	      }// nth_dim8 < 7

	    }// dimension loop 2
	  }// layer loop 2
	  // tmpcovx culc end

	}// dimension loop

      }// layer loop

      sectorID8[8] = 0;//for sector, we set it to zero
      hashID8[8] = 0;//for sector, we set it to zero
      bank_order = b - b_int;
      
      addPattReturnCode=addKDPattern(bank_order,false,sectorID8,hashID8,1,Mtmp,vec8,tmpxC8,tmpxD8,tmpxPhi8,tmpxCoto8,tmpxZ8,tmpcovx8,
				     tmp_intc[0],tmp_intphi[0],tmp_intd0[0],tmp_intz0[0],tmp_inteta[0]);

      if(1==addPattReturnCode){
	++npatterns;
      }

    }// end of event loop
  }// end of bank loop
    
  Lfile->Close();
    
  free(sectorID);
  free(hashID);
  free(vec);
  free(tmpxC);
  free(tmpxD);
  free(tmpxPhi);
  free(tmpxCoto);
  free(tmpxZ); 
  free(tmpcovx);
  
  //
  // create new file of 8L extracted from 12L
  //

  ndim2_8=ndim8*ndim8;
  int *extract_Sec;
  int *extract_hash;
  double *extract_vec; 
  double extract_C;
  double extract_D;
  double extract_Phi;
  double extract_Coto;
  double extract_Z;
  float extract_nhit;
  
  double *extract_xC=NULL;
  double *extract_xD=NULL;
  double *extract_xPhi=NULL;
  double *extract_xCoto=NULL;
  double *extract_xZ=NULL;
  double *extract_covx=NULL;

  TTree *m_tree=NULL;
  TTree *m_extract_slice=NULL;

  m_extract_slice=new TTree("slice", "slice para");
  
  extract_Sec = (int *) calloc(nplane8,sizeof(int));  
  extract_hash = (int *) calloc(nplane8,sizeof(int));  
  extract_vec = (double *) calloc(ndim8,sizeof(double));  
  extract_xC = (double *) calloc(ndim8,sizeof(double));  
  extract_xD = (double *) calloc(ndim8,sizeof(double));
  extract_xPhi = (double *) calloc(ndim8,sizeof(double));  
  extract_xCoto = (double *) calloc(ndim8,sizeof(double));  
  extract_xZ = (double *) calloc(ndim8,sizeof(double));  
  extract_covx = (double *) calloc(ndim8*ndim8,sizeof(double));  

  // define output file
  if (m_outfilename.size()==0) {
    char filename[40];
    if(true==m_allregion){
      sprintf(filename,"matrix_%dL_%dDim_regall.root",nplane8,ndim8);
    }else{
      sprintf(filename,"matrix_%dL_%dDim_reg%d.root",nplane8,ndim8,m_region);
    }
    Lfile = new TFile(filename,"recreate");
  }
  else  {
    Lfile = TFile::Open(m_outfilename.c_str(),"recreate");
  }

  char name[5];
  char mtitle[20];

  m_extract_slice->Branch("c_max",&tmp_cmax);
  m_extract_slice->Branch("phi_max",&tmp_phimax);
  m_extract_slice->Branch("d0_max",&tmp_d0max);
  m_extract_slice->Branch("z0_max",&tmp_z0max);
  m_extract_slice->Branch("eta_max",&tmp_etamax);
  
  m_extract_slice->Branch("c_min",&tmp_cmin);
  m_extract_slice->Branch("phi_min",&tmp_phimin);
  m_extract_slice->Branch("d0_min",&tmp_d0min);
  m_extract_slice->Branch("z0_min",&tmp_z0min);
  m_extract_slice->Branch("eta_min",&tmp_etamin);
  
  m_extract_slice->Branch("c_slices",&tmp_cslices);
  m_extract_slice->Branch("phi_slices",&tmp_phislices);
  m_extract_slice->Branch("d0_slices",&tmp_d0slices);
  m_extract_slice->Branch("z0_slices",&tmp_z0slices); 
  m_extract_slice->Branch("eta_slices",&tmp_etaslices);    
  m_extract_slice->Fill();
  m_extract_slice->Write();

  //
  // define 8L matrix branch, sort by ntracks, and fill
  //

  for(int b=b_int;b<b_end;b++){
    sprintf(name,"am%d",b);
    sprintf(mtitle,"Ambank %d para",b);
        
    m_tree = new TTree(name, mtitle);
    
    m_tree->Branch("ndim",&ndim8,"ndim8/I");
    m_tree->Branch("ndim2",&ndim2_8,"ndim2_8/I");
    m_tree->Branch("Vec", extract_vec,"Vec[ndim8]/D");
    m_tree->Branch("nplanes",&nplane8,"nplanes/I");
    m_tree->Branch("sectorID", extract_Sec, "sectorID[nplanes]/I");
    m_tree->Branch("hashID", extract_hash, "hashID[nplanes]/I");
    m_tree->Branch("nhit", &extract_nhit,"nhit/F");
    m_tree->Branch("tmpC", &extract_C,"tmpC/D");
    m_tree->Branch("tmpD", &extract_D,"tmpD/D");
    m_tree->Branch("tmpPhi", &extract_Phi,"tmpPhi/D");
    m_tree->Branch("tmpCoto", &extract_Coto,"tmpCoto/D");
    m_tree->Branch("tmpZ", &extract_Z,"tmpZ/D");
    m_tree->Branch("tmpxC", extract_xC,"tmpxC[ndim8]/D");
    m_tree->Branch("tmpxD", extract_xD,"tmpxD[ndim8]/D");
    m_tree->Branch("tmpxPhi", extract_xPhi,"tmpxPhi[ndim8]/D");
    m_tree->Branch("tmpxCoto", extract_xCoto,"tmpxCoto[ndim8]/D");
    m_tree->Branch("tmpxZ", extract_xZ,"tmpxZ[ndim8]/D");
    m_tree->Branch("tmpcovx", extract_covx,"tmpcovx[ndim2_8]/D");
                     
    m_extract_intc= new std::vector<short>;
    m_extract_intphi= new std::vector<short>;
    m_extract_intd0= new std::vector<short>;
    m_extract_intz0= new std::vector<short>;
    m_extract_inteta= new std::vector<short>;

    m_tree->Branch("tmpintc", &m_extract_intc);
    m_tree->Branch("tmpintphi", &m_extract_intphi);
    m_tree->Branch("tmpintd0", &m_extract_intd0);
    m_tree->Branch("tmpintz0", &m_extract_intz0);
    m_tree->Branch("tmpinteta", &m_extract_inteta);

    bank_order = b - b_int;

    sortMPatterns(bank_order);
    
    log << MSG::INFO <<"nsector "<< getNPatterns(bank_order) <<endmsg;
    for(int i=0; i<getNPatterns(bank_order);i++){
      m_extract_intc->clear();
      m_extract_intphi->clear();
      m_extract_intd0->clear();
      m_extract_intz0->clear();
      m_extract_inteta->clear();	
      
      log << MSG::INFO <<"sector ";
      for(int k=0;k<nplane8;k++){
	extract_Sec[k]=getPatternSS(bank_order,i,k);
	extract_hash[k]=getPatthashID(bank_order,i,k);
	log <<getPatternSS(bank_order,i,k)<<" ";
      }
      log <<" "<<getPattnhit(bank_order,i) <<endmsg;
      extract_nhit=getPattnhit(bank_order,i);
      extract_C=getPattC(bank_order,i);
      extract_D=getPattD(bank_order,i);
      extract_Phi=getPattPhi(bank_order,i);
      extract_Coto=getPattCoto(bank_order,i);
      extract_Z=getPattZ(bank_order,i);
      for(int k=0;k<ndim8;k++){
	extract_vec[k]=getPatthitc(bank_order,i,k);
	extract_xC[k]=getPattxC(bank_order,i,k);
	extract_xD[k]=getPattxD(bank_order,i,k);
	extract_xPhi[k]=getPattxPhi(bank_order,i,k);
	extract_xCoto[k]=getPattxCoto(bank_order,i,k);
	extract_xZ[k]=getPattxZ(bank_order,i,k);
	for(int l=0;l<ndim8;l++){
	  extract_covx[k*ndim8+l]=getPattcovx(bank_order,i,k*ndim8+l);
	  log << MSG::DEBUG <<" i= "  << k*ndim8+l <<"bank= "<< extract_covx[k*ndim8+l] <<endmsg;
	}
	log <<getPatthitc(bank_order,i,k)<<" ";
      }
      
      log <<"  "<<endmsg;
      
      for(int m=0;m<extract_nhit;m++){
	m_extract_intc->push_back(getPattintc(bank_order,i,m));
	m_extract_intphi->push_back(getPattintphi(bank_order,i,m));
	m_extract_intd0->push_back(getPattintd0(bank_order,i,m));
	m_extract_intz0->push_back(getPattintz0(bank_order,i,m));
	m_extract_inteta->push_back(getPattinteta(bank_order,i,m));
      }

      // fill the output TTree after the reduce information has been informed
      m_tree->Fill();
    }
    // save the result in the output TTree
    Lfile->cd();
    m_tree->Write();
    gROOT->cd();

    delete m_tree;
    m_tree=NULL;
  }// loop over regions

  free(extract_Sec);
  free(extract_hash);
  free(extract_vec);
  free(extract_xC);
  free(extract_xD);
  free(extract_xPhi);
  free(extract_xCoto);
  free(extract_xZ);
  free(extract_covx);
}

void FTKConstGenAlgo::make_subregion()
{
  MsgStream log(msgSvc(), name());

  char **sector_filename;
  char **gcon_filename;
  FILE *sector_file;
  FILE *gcon_file;
  
  sector_filename = (char **) malloc(m_nsubregion*sizeof(char*));
  gcon_filename = (char **) malloc(m_nsubregion*sizeof(char*));
  for(int i=0;i<m_nsubregion;i++){
    sector_filename[i] = (char *) calloc(50,sizeof(char));
    gcon_filename[i] = (char *) calloc(50,sizeof(char));
  }

  // sector file

  for(unsigned int i=0;i<m_sfpath.size();i++){
    int nlayer;
    int nsector;
    int **sector;
    int *ntrack;
    int brank;
    int sectorID;
    
    const string &sfpath = m_sfpath[i];
    ifstream slice_file(sfpath.c_str());
    
    slice_file >>nsector;
    log << MSG::INFO <<" nsector =  " << nsector << endmsg;
    
    slice_file >>nlayer;
    log << MSG::INFO <<" nlayer =  " << nlayer << endmsg;
    sector = (int **) malloc(nsector*sizeof(int*));
    ntrack=(int *) calloc(nsector,sizeof(int));
    
    for(int id=0;id<nsector;id++){
      sector[id] =(int *) calloc(nlayer,sizeof(int));
    }
    
    for(int id=0;id<nsector;id++){
      slice_file >> sectorID;
      if(id!=sectorID){log << MSG::FATAL <<" This file is bad format " << sfpath.c_str() << endmsg;}else{
	for(int layer=0;layer<nlayer;layer++){
	  slice_file >> sector[id][layer];
	}
	slice_file >>brank;
	slice_file>>ntrack[id];
      }
      
    }
    
    for(int sub=0;sub<m_nsubregion;sub++){
      int sub_nsector;
      sub_nsector=nsector/m_nsubregion;
      if(sub<nsector%m_nsubregion){
	sub_nsector=sub_nsector+1;
      }
      
      sprintf(sector_filename[sub],"sectors_raw_%dL_%dM_reg%d_sub%d.patt",nlayer,m_nsubregion,i,sub);
      sector_file = fopen(sector_filename[sub],"w");
      fprintf(sector_file,"%d %d\n",sub_nsector,nlayer);
      
      for(int id=0;id<sub_nsector;id++){
	fprintf(sector_file,"%d ",id);
	for(int layer=0;layer<nlayer;layer++){
	  fprintf(sector_file,"%d ", sector[id*m_nsubregion+sub][layer]);
	}
	fprintf(sector_file,"0 %d",ntrack[id*m_nsubregion+sub]);
	fprintf(sector_file,"\n");
      }
      
    }//for subregion
  }// for region
  
  //sector file 
  //gcon file

  for(unsigned int i=0;i<m_gfpath.size();i++){
    int nlayer;
    int nsector;
    int ndim=0,NDIM;
    int *sectorID;
    string tmp_e; 
    string identifier;
    geo_constants *tmp_const;
    const string &gfpath = m_gfpath[i];
    ifstream slice_file(gfpath.c_str());
    
    string init_line[10];
    for(int j=0;j<6;j++){
      getline(slice_file, init_line[j]);
    }
    slice_file>>nlayer;
    getline(slice_file, init_line[6]);
    getline(slice_file, init_line[6]);
    slice_file>>nsector;    
    getline(slice_file, init_line[7]);
    getline(slice_file, init_line[7]);
    slice_file>>NDIM;
    getline(slice_file, init_line[8]);

    if(nlayer==11 || nlayer==8) ndim=nlayer+3;//w/o IBL or 1st stage
    else if(nlayer==12) ndim=nlayer+4;//IBL

    tmp_const =(struct geo_constants *) malloc(nsector*sizeof(struct geo_constants));
    sectorID=(int *) calloc(nsector,sizeof(int));
    for(int j=0;j<nsector;j++){
      tmp_const[j].Vd = (double *) calloc(ndim,sizeof(double));
      tmp_const[j].Vc = (double *) calloc(ndim,sizeof(double));
      tmp_const[j].Vf = (double *) calloc(ndim,sizeof(double));
      tmp_const[j].Vz0 = (double *) calloc(ndim,sizeof(double));
      tmp_const[j].Vo = (double *) calloc(ndim,sizeof(double));
      tmp_const[j].kaverages = (double *) calloc((ndim-NPARS),sizeof(double));      
      tmp_const[j].kernel = (double **) malloc((ndim-NPARS)*sizeof(double *));
      for(int k=0;k<ndim-NPARS;k++){
	tmp_const[j].kernel[k] = (double *) calloc(ndim,sizeof(double));
      }
    }
    
    int id=0;
    while(1){
      if(nsector==id){
	break;
      }
      getline(slice_file,identifier);
      if("sector"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e );
	is>>sectorID[id];
      }
      
      if(" Vc "==identifier){
	for(int l=0;l<ndim;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].Vc[l];
	}
      }
      if(" Vd "==identifier){
	for(int l=0;l<ndim;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].Vd[l];
	}
      }
      if(" Vf "==identifier){
	for(int l=0;l<ndim;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].Vf[l];
	}
      }
      if(" Vz0 "==identifier){
	for(int l=0;l<ndim;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].Vz0[l];
	}
      }

      if(" Vo "==identifier){
	for(int l=0;l<ndim;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].Vo[l];
	}
      }
      
      if("kaverages"==identifier){
	for(int l=0;l<ndim-NPARS;l++){
	  getline(slice_file,tmp_e);
	  istringstream is( tmp_e ); 
	  is>>tmp_const[id].kaverages[l];
	}
      }
      
      if("kernel"==identifier){
	for(int l=0;l<ndim-NPARS;l++){
	  for(int m=0;m<ndim;m++){
	    getline(slice_file,tmp_e);
	    istringstream is( tmp_e ); 
	    is>>tmp_const[id].kernel[l][m];
	  }
	}
      }
      
      if("Cc"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e ); 
	is>>tmp_const[id].Cc;
      }

      if("Cd"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e ); 
	is>>tmp_const[id].Cd;
      }
      
      if("Cf"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e ); 
	is>>tmp_const[id].Cf;
      }

      if("Cz0"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e ); 
	is>>tmp_const[id].Cz0;
      }
      
      if("Co"==identifier){
	getline(slice_file,tmp_e);
	istringstream is( tmp_e ); 
	is>>tmp_const[id].Co;
	id++;
      }
    }//read ASCII file 
    
    for(int sub=0;sub<m_nsubregion;sub++){
      int sub_nsector;
      sub_nsector=nsector/m_nsubregion;
      if(sub<nsector%m_nsubregion){
	sub_nsector=sub_nsector+1;
      }
      sprintf(gcon_filename[sub],"corrgen_raw_%dL_%dM_reg%d_sub%d.gcon",nlayer,m_nsubregion,i,sub);
      gcon_file = fopen(gcon_filename[sub],"w");
      for(int j=0;j<6;j++){
	fprintf(gcon_file,init_line[j].c_str());
	fprintf(gcon_file,"\n");
      }
      fprintf(gcon_file," %d\n",nlayer);
      fprintf(gcon_file,init_line[6].c_str());
      fprintf(gcon_file,"\n");
      fprintf(gcon_file,"%d\n",sub_nsector);
      fprintf(gcon_file,init_line[7].c_str());
      fprintf(gcon_file,"\n");
      fprintf(gcon_file," %d\n",2);
      fprintf(gcon_file,init_line[8].c_str());
      for(int id=0;id<sub_nsector;id++){
	fprintf(gcon_file,"sector\n");
	fprintf(gcon_file,"%d\n",id);
	fprintf(gcon_file," Vc \n");
	for(int k=0;k<ndim;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Vc[k]);
	}
	fprintf(gcon_file," Vd \n");
	for(int k=0;k<ndim;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Vd[k]);
	}
	fprintf(gcon_file," Vf \n");
	for(int k=0;k<ndim;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Vf[k]);
	}
	fprintf(gcon_file," Vz0 \n");
	for(int k=0;k<ndim;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Vz0[k]);
	}
	fprintf(gcon_file," Vo \n");
	for(int k=0;k<ndim;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Vo[k]);
	}
	fprintf(gcon_file,"kaverages\n");
	for(int k=0;k<ndim-NPARS;k++){
	  fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].kaverages[k]);
	}
	fprintf(gcon_file,"kernel\n");
	for(int k=0;k<ndim-NPARS;k++){
	  for(int m=0;m<ndim;m++){
	    fprintf(gcon_file,"%e \n",tmp_const[id*m_nsubregion+sub].kernel[k][m]);
	  }
	}
	fprintf(gcon_file,"Cc\n");
	fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Cc);
	fprintf(gcon_file,"Cd\n");
	fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Cd);
	fprintf(gcon_file,"Cf\n");
	fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Cf);
	fprintf(gcon_file,"Cz0\n");
	fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Cz0);
	fprintf(gcon_file,"Co\n");
	fprintf(gcon_file,"%e\n",tmp_const[id*m_nsubregion+sub].Co);
      }
            
      
    }//for subregion
  }// for region
  //gcon file


}

int FTKConstGenAlgo::mtx_eigen(int dim, double *mtx_v, double *eigval_v, double *eigvecs_v) {
  TMatrixD mtx(dim,dim,mtx_v);

  // Catching singular matrix
  TMatrixD chk_mtx(dim,dim,mtx_v);
  TDecompLU dc(chk_mtx);
  int status = dc.InvertLU(chk_mtx,MTX_TOLERANCE);
  if(!status) return -1;
  // Catching singular matrix
  
  TVectorD eigvals(dim);
  TMatrixD eigvecs(dim,dim);
  
  // calculate eigvecs and aigvals
  eigvecs = mtx.EigenVectors(eigvals);
    
  // copy into the vectors in the right mode: autovetors per columun
  double *tmpv = eigvals.GetMatrixArray();
  //  double *tmpm = eigvecs.GetMatrixArray();

  for (int i=0;i<dim;++i){
    for (int j=0;j<dim;++j) {
      eigvecs_v[i*dim+j] = eigvecs[j][dim-1-i];
    }
  }

  for (int i=0;i<dim;++i) {
    eigval_v[i] = tmpv[dim-1-i];
  }
  return 0;
}


int FTKConstGenAlgo::mtx_invert(int dim, double *mtx_v) {
  TMatrixD mtx(dim,dim,mtx_v);
  
  // Catching singular matrix
  TMatrixD chk_mtx(dim,dim,mtx_v);
  TDecompLU dc(chk_mtx);
  int status = dc.InvertLU(chk_mtx,MTX_TOLERANCE);
  if(!status) return -1;
  // Catching singular matrix

  //mtx.Print();
  mtx.Invert();

  double *tmpm = mtx.GetMatrixArray();
  for (int i=0;i<dim;++i) for (int j=0;j<dim;++j) {
    mtx_v[i*dim+j] =tmpm[i*dim+j];
  }

  return 0;
}

// FlagAK: If constants are bad (eg, contain "nan"), fill them up with zeros!
void FTKConstGenAlgo::nullifyConstants(int sector,int ndim) {
  int i;
  int j;
  //printf("Sector %d has bad constants; nullifying them!\n",sector);
  // to don't leave unitialized the variables
  // fill of zero, matrix ad vector
  gcorth[sector].Cc = geo_const[sector].Cc = .0;
  gcorth[sector].Cd = geo_const[sector].Cd = .0;
  gcorth[sector].Cf = geo_const[sector].Cf = .0;
  gcorth[sector].Cz0 = geo_const[sector].Cz0 = .0;
  gcorth[sector].Co = geo_const[sector].Co = .0;
  
  for (i=0;i<ndim;++i) {
    gcorth[sector].Vc[i] = geo_const[sector].Vc[i] = .0;
    gcorth[sector].Vd[i] = geo_const[sector].Vd[i] = .0;
    gcorth[sector].Vf[i] = geo_const[sector].Vf[i] = .0;
    gcorth[sector].Vz0[i] = geo_const[sector].Vz0[i] = .0;
    gcorth[sector].Vo[i] = geo_const[sector].Vo[i] = .0;
  }
  
  for (i=0;i<ndim-NPARS;++i) {
    geo_const[sector].kaverages[i] = .0;
    for (j=0;j<ndim;++j)
      geo_const[sector].kernel[i][j] = .0;
  }
  
  geo_const[sector].real = 0; // flag for validate constants 
  
  return;
}

void FTKConstGenAlgo::sigmad(double *geoconstvect/*, double **marray*/, double **marrayinv,double *v,int ndim)
{
  //int i,k,j;
  //double *vd,*sreturn,sigmadreturn,ssd;

  int i,j;
  double *vd;

  if ((vd = (double*) calloc(ndim,sizeof(double)))==NULL) {
     //  svt_err(1,"main:error allocating vd") ;
  }
  for(i=0;i<ndim;i++)
    {
      for(j=0;j<ndim;j++)
	vd[i] += marrayinv[i][j]*v[j];
    }

  for(i = 0;i < ndim;i++) 
    geoconstvect[i] = vd[i];

  free(vd);
}

float FTKConstGenAlgo::proj(double *vec1, double *vec2, int size)
{
  float total=0;
  int i;
  for(i=0;i<size;i++)
    total+=vec1[i]*vec2[i];
  return (float)total;
}

// FlagAK: Check if constants are bad (eg, contain "nan")
int FTKConstGenAlgo::failedConstants(int sector,int ndim) {
  MsgStream log(msgSvc(), name());

  int status = 0;
  int i;
  int j;

  //check if the constants are reasonable
  //Cc
  if(TMath::IsNaN(gcorth[sector].Cc)) {
    log << MSG::DEBUG << "Bad gcorth[" << sector << "].Cc: " << gcorth[sector].Cc << endmsg;
    status++;
  }
  if(TMath::IsNaN(geo_const[sector].Cc)) {
    //  if(TMath::IsNaN(geo_const[sector].Cc) || geo_const[sector].Cc==0) {//tomoya
    log << MSG::DEBUG << "Bad geo_const[" << sector << "].Cc: " << geo_const[sector].Cc << endmsg;
    status++;
  }
  
  //Cd
  if(TMath::IsNaN(gcorth[sector].Cd)) {
    log << MSG::DEBUG << "Bad gcorth[" << sector << "].Cd: " << gcorth[sector].Cd << endmsg;
    status++;
  }
  if(TMath::IsNaN(geo_const[sector].Cd)) {
    log << MSG::DEBUG << "Bad geo_const[" << sector << "].Cd: " << geo_const[sector].Cd << endmsg;
    status++;
  }

  //Cf
  if(TMath::IsNaN(gcorth[sector].Cf)) {
    log << MSG::DEBUG << "Bad gcorth[" << sector << "].Cf: " << gcorth[sector].Cf << endmsg;
    status++;
  }
  if(TMath::IsNaN(geo_const[sector].Cf)) {
    log << MSG::DEBUG << "Bad geo_const[" << sector << "].Cf: " << geo_const[sector].Cf << endmsg;
    status++;
  }

  //Cz0
  if(TMath::IsNaN(gcorth[sector].Cz0)) {
    log << MSG::DEBUG << "Bad gcorth[" << sector << "].Cz0: " << gcorth[sector].Cz0 << endmsg;
    status++;
  }
  if(TMath::IsNaN(geo_const[sector].Cz0)) {
    log << MSG::DEBUG << "Bad geo_const[" << sector << "].Cz0: " << geo_const[sector].Cz0 << endmsg;
    status++;
  }

  //Co
  if(TMath::IsNaN(gcorth[sector].Co)) {
    log << MSG::DEBUG << "Bad gcorth[" << sector << "].Co: " << gcorth[sector].Co << endmsg;
    status++;
  }
  if(TMath::IsNaN(geo_const[sector].Co)) {
    log << MSG::DEBUG << "Bad geo_const[" << sector << "].Co: " << geo_const[sector].Co << endmsg;
    status++;
  }

  for (i=0;i<ndim;++i) {
    //Vc
    if(TMath::IsNaN(gcorth[sector].Vc[i])) {
      log << MSG::DEBUG << "Bad gcorth[" << sector << "].Vc[" << i << "]: " << gcorth[sector].Vc[i] << endmsg;
      status++;
    }
    if(TMath::IsNaN(geo_const[sector].Vc[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].Vc[" << i << "]: " << geo_const[sector].Vc[i] << endmsg;
      status++;
    }

    //Vd
    if(TMath::IsNaN(gcorth[sector].Vd[i])) {
      log << MSG::DEBUG << "Bad gcorth[" << sector << "].Vd[" << i << "]: " << gcorth[sector].Vd[i] << endmsg;
      status++;
    }
    if(TMath::IsNaN(geo_const[sector].Vd[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].Vd[" << i << "]: " << geo_const[sector].Vd[i] << endmsg;
      status++;
    }

    //Vf
    if(TMath::IsNaN(gcorth[sector].Vf[i])) {
      log << MSG::DEBUG << "Bad gcorth[" << sector << "].Vf[" << i << "]: " << gcorth[sector].Vf[i] << endmsg;
      status++;
    }
    if(TMath::IsNaN(geo_const[sector].Vf[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].Vf[" << i << "]: " << geo_const[sector].Vf[i] << endmsg;
      status++;
    }

    //Vz0
    if(TMath::IsNaN(gcorth[sector].Vz0[i])) {
      log << MSG::DEBUG << "Bad gcorth[" << sector << "].Vz0[" << i << "]: " << gcorth[sector].Vz0[i] << endmsg;
      status++;
    }
    if(TMath::IsNaN(geo_const[sector].Vz0[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].Vz0[" << i << "]: " << geo_const[sector].Vz0[i] << endmsg;
      status++;
    }

    //Vo
    if(TMath::IsNaN(gcorth[sector].Vo[i])) {
      log << MSG::DEBUG << "Bad gcorth[" << sector << "].Vo[" << i << "]: " << gcorth[sector].Vo[i] << endmsg;
      status++;
    }
    if(TMath::IsNaN(geo_const[sector].Vo[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].Vo[" << i << "]: " << geo_const[sector].Vo[i] << endmsg;
      status++;
    }

  }

  for (i=0;i<ndim-NPARS;++i) {
    if(TMath::IsNaN(geo_const[sector].kaverages[i])) {
      log << MSG::DEBUG << "Bad geo_const[" << sector << "].kaverages[" << i << "]: " << geo_const[sector].kaverages[i] << endmsg;
      status++;
    }
    for (j=0;j<ndim;++j)
      if(TMath::IsNaN(geo_const[sector].kernel[i][j])) {
	log << MSG::DEBUG << "Bad geo_const[" << sector << "].kernel[" << i << "][" << j << "]: " << geo_const[sector].kernel[i][j];
	status++;
      }
  }

  return status;
}
