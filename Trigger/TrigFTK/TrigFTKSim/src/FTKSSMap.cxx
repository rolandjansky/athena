/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKHit.h"
#include "TrigFTKSim/FTKSplitEncoder.h"
#include "TrigFTKSim/ftkdefs.h"

// #define TRACEBACK_ON_BOUNDARY_VIOLATION
#ifdef TRACEBACK_ON_BOUNDARY_VIOLATION
#include <execinfo.h>
#endif

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <TKey.h>
#include <TTree.h>
using namespace std;

// These limits are not needed if FTK_AMBank.h uses SS_LOOKUP_MAP option
#define SS_LOOKUP_SIZE 500000 // was 360000
#define SS_MIN_SCT_PHI 15
#define SS_MIN_PIX_PHI 10
#define SS_MIN_PIX_ETA 10  // was 15

const TString FTKSSMap::s_dirnameDC  = "SSMap_DC";
const TString FTKSSMap::s_dirnameTSP = "SSMap_TSP";

FTKSSMap::FTKSSMap() :
  m_isok(false), m_rmap(0), m_ssm(0), m_fraction(0), m_bound_checks(false), m_useHWCoords(false),
  m_eta_nondcbits(0), m_nbitsssid(0), m_nbitsssid_SSB(0), m_nbitsssid_module_pixel(0),
  m_nbitsssid_module_strip(0)
{
  // nothing special to do
}


FTKSSMap::~FTKSSMap()
{
  // nothing to do
  if (m_ssm) {
    for (int ipl=0;ipl<m_rmap->getPlaneMap()->getNPlanes();++ipl) {
      const FTKPlaneMap *pmap = m_rmap->getPlaneMap();
      int nsections = pmap->getNSections(ipl);
      for (int isec=0;isec!=nsections;++isec) delete [] m_ssm[ipl][isec];
      delete [] m_ssm[ipl];
    }
    delete [] m_ssm;
  }
}


/** this constructor load a SS map file in version 0,
    if the "UnusedLayers" flag is true the maps is evaluated on the unused
    layers */
FTKSSMap::FTKSSMap(FTKRegionMap *rmap, const char *fname, 
                   bool BoundChecks)
  : m_rmap(rmap), m_ssm(0), m_fraction(0), m_bound_checks(BoundChecks),
    m_eta_nondcbits(0), m_nbitsssid(0), m_nbitsssid_SSB(0), m_nbitsssid_module_pixel(0),
    m_nbitsssid_module_strip(0)
{
  // input stream
  ifstream fin(fname);
  if (!fin) {
     std::cout<<" bad file name \""<<fname<<"\"\n";
    FTKSetup::PrintMessage(ftk::sevr,"Error opening SS file");
  }

  m_path = fname;

  // read and verify the version
  string key;
  int version;
  fin >> key >> version;
  if (key!="Version" || (version!=9 && version!=10) ) {
    FTKSetup::PrintMessage(ftk::sevr,"Version file not compatible");
  }

  // get the PMAP pointer
  const FTKPlaneMap *pmap = m_rmap->getPlaneMap();

  // get the number of planes
  int nplanes = pmap->getNPlanes();
  // first m_ssm allocation
  /** m_ssm[nplanes][nsections][nregions] */
  m_ssm = new struct ssm**[nplanes];
  if (!m_ssm) {
    FTKSetup::PrintMessage(ftk::sevr,"Impossible to allocate ssmap structure");
  }
  bool cmDebug =  FTKSetup::getFTKSetup().getVerbosity()>1;

  for (int iplane=0; iplane<nplanes; ++iplane) {
    // retrieve the number of sections in this plane
    int nsections = pmap->getNSections(iplane);
    m_ssm[iplane] = new struct ssm*[nsections];
    if (!m_ssm[iplane]) {
      FTKSetup::PrintMessage(ftk::sevr,"Error allocating ssmap internal structure");
    }

    for (int isec=0;isec<nsections;++isec) {
      // get the maximum module of the last region using -1
      int maxmod = rmap->getRegionMapItem(-1,iplane,isec).getTotMod();
      if(maxmod<72) maxmod=72; // FIXME - cy temp hardcode for split arch
      m_ssm[iplane][isec] = new struct ssm[maxmod+1];

      if (!m_ssm[iplane]) {
	    FTKSetup::PrintMessage(ftk::sevr,"Error allocating ssmap iternal strusture");
      }
      memset(m_ssm[iplane][isec],0,sizeof(struct ssm)*(maxmod+1));
    }
  }


  // loop to read all the elements
  int nrlayers = pmap->getRLayers();
  if( FTKSetup::getFTKSetup().getITkMode() ) {
    FTKSetup::PrintMessage(ftk::warn, "ITkMode currently only set up to process first 8 layers in ss map");
    nrlayers = 8;
  }
  for (int ilayer=0;ilayer<nrlayers;++ilayer) {
    int type, isEC, layer;
    int phiss, phiwidth, etass, etawidth;
    int fiss=0, fiwidth=0, cottss=0, cottwidth=0, curvss=0, curvwidth=0; // cy addition
    int ndcx(0), ndcy(0);
    string line;
    getline(fin,line);
    if (line.size()==0 || line[0] == '!') {
      ilayer -= 1;
      continue;
    }
    if(cmDebug) cout << line << endl;
    istringstream sline(line);
      
    // read the element
    sline >> type >> isEC >> layer;
      
    // cy addition to get correct variables
    if (type!=2)
      sline >> phiss >> phiwidth >> etass >> etawidth;
    else
      sline >> fiss >> fiwidth >> cottss >> cottwidth >> curvss >> curvwidth;
      
    //
    // starting in version 10 use hardware like SSIDs
    //
    if( version>=10 ) {
      m_useHWCoords = true;
    }else{
      m_useHWCoords = false;
    }

    if (!sline) {
      if (type!=1) { // cy change if scttrk or sct, etass means means nothing
        etass = -1;
        etawidth = -1;
      }
      else if (FTKSetup::getFTKSetup().getIBLMode()>=1 && type==1 && layer==0 && isEC==0){ // for ibl cy
        etass = 160;
        etawidth = 160;
      }
      else {
        etass = 144;
        etawidth = 144;
      }
    }

    // Make sure ss widths are large enough given SS_LOOKUP_SIZE
    // If we ever need to use smaller ss size, SS_LOOKUP_SIZE or bit allocation for (int)ss must be changed
    if(m_bound_checks) {
      if(type==0) {
        if(phiss<SS_MIN_SCT_PHI) 
          FTKSetup::PrintMessageFmt(ftk::sevr,"ssmap_rd: SCT SS size is too small: %d < %d\n Line: %s",phiss,SS_MIN_SCT_PHI,line.c_str());
      }
      else if(type==1) {
        if(phiss<SS_MIN_PIX_PHI) 
          FTKSetup::PrintMessageFmt(ftk::sevr,"ssmap_rd: PIX SS size (phi) is too small: %d < %d\n Line: %s",phiss,SS_MIN_PIX_PHI,line.c_str());
        if(etass<SS_MIN_PIX_ETA) 
          FTKSetup::PrintMessageFmt(ftk::sevr,"ssmap_rd: PIX SS size (eta) is too small: %d < %d\n Line: %s",etass,SS_MIN_PIX_ETA,line.c_str());
      }
    }

    int plane = pmap->getMap(type,isEC,layer).getPlane();
    if (plane==-1) continue;
    // the SS map creation for a plane is skipped if:
    // * we are creating the unused layer SS map and plane is active
    // * we are creating the active layers Ss map and plane is inactive

	if(type!=2){ // cy if not scttrk
	  /* TEMP include all sections */
	  int section = pmap->getMap(type,isEC,layer).getSection();
	  int totMod = FTKSetup::getFTKSetup().getIBLMode()>=1 ? rmap->getRegionMapItem(-1,plane,section).getTotMod()+3 : rmap->getRegionMapItem(-1,plane,section).getTotMod(); // cy ibl +3 addition
	  for(int j=0;j<=totMod;++j) {
	    m_ssm[plane][section][j].m_etass = etass; // width of pixel in eta direction
	    m_ssm[plane][section][j].m_etawidth = etawidth; // total eta module width of j's module
	    // the line below makes module width a multiple of ss width in order to distinguish ss in the module overlap region.
	    // E.g, if ss=50 pix in eta, module width (144) will be rounded up to 150, so the total offset = 150*j
	    m_ssm[plane][section][j].m_etaoffset = (etawidth-etawidth%etass+etass)*j; // total eta offset of j's module
	    // A special hack in case eta ss width is equal to eta module width (i.e. no eta partitioning in eta at all)
	    // Then we can simply use 144*j as total eta offset and not worry about overlap
	    if(etawidth==etass) m_ssm[plane][section][j].m_etaoffset=etawidth*j;
	    
	    m_ssm[plane][section][j].m_phiss = phiss; // width of pixel or sct in r-phi direction
	    m_ssm[plane][section][j].m_phiwidth = phiwidth; // total phi module width of j's module.
	    // A similar correction rounding module phi width to nearest multiple of ss width - for SCT and pixels
	    // E.g., if ss=50 strips, module width (768) will be rounded up to 800, so the total offset = 800*j
	    m_ssm[plane][section][j].m_phioffset = (phiwidth-phiwidth%phiss+phiss)*j; // total phi offset of j's module.
	    //printf("m_ssm[plane=%d][section=%d][j=%d].m_phioffset/m_phiss = %d/%d\n",plane,section,j,m_ssm[plane][section][j].m_phioffset,m_ssm[plane][section][j].m_phiss);
	    //printf("SS info. %d %d.%d (%d %d-%d) %d %d\n",j,plane,section,type,isEC,layer, m_ssm[plane][section][j].m_phiss, m_ssm[plane][section][j].m_phioffset);

	    // set the offset for the HW-like SS encoding
	    if (type==0) {
	      // for the SCT no internal offset will be set
	      m_ssm[plane][section][j].m_ssoff = ssround(phiwidth,phiss);
	      m_ssm[plane][section][j].m_ssoff_int = 0;
	    }
	    else if(type==1) {
	      m_ssm[plane][section][j].m_ssoff = ssround(phiwidth,phiss)*ssround(etawidth,etass);
	      m_ssm[plane][section][j].m_ssoff_int = ssround(phiwidth,phiss);
	    }

        m_ssm[plane][section][j].m_ndcx = ndcx;
        m_ssm[plane][section][j].m_ndcy = ndcy;
	  }
	  if(cmDebug)
             printf("SS info. : %d.%d (%d %d-%d) %d %d\n", plane,section,type,isEC,layer, m_ssm[plane][section][0].m_phiss, m_ssm[plane][section][0].m_phioffset);
	}
	else{ // cy else if scttrk
	  /* TEMP include all sections */
	  int section = pmap->getMap(type,isEC,layer).getSection();
	  int maxj = rmap->getRegionMapItem(-1,plane,section).getTotMod(); 
	  if(maxj<72) maxj=72;  // FIXME - cy temp hardcode for split arch (eta encoding)
	  for(int j=0;j<=maxj;++j) {
	    //m_ssm[plane][section][j].m_etass = etass; // width of pixel in eta direction
	    //m_ssm[plane][section][j].m_etawidth = etawidth; // total eta module width of j's module
	    // the line below makes module width a multiple of ss width in order to distinguish ss in the module overlap region.
	    // E.g, if ss=50 pix in eta, module width (144) will be rounded up to 150, so the total offset = 150*j
	    //	    m_ssm[plane][section][j].m_etaoffset = (etawidth-etawidth%etass+etass)*j; // total eta offset of j's module
	    // A special hack in case eta ss width is equal to eta module width (i.e. no eta partitioning in eta at all)
	    // Then we can simply use 144*j as total eta offset and not worry about overlap
	    //if(etawidth==etass) m_ssm[plane][section][j].m_etaoffset=etawidth*j;
	    
	    m_ssm[plane][section][j].m_fiss = fiss; // width of pixel or sct in r-phi direction
	    //cout<<"Reading: p, s, j, m_fiss: "<<plane<<"\t"<<section<<"\t"<<j<<"\t"<<fiss<<endl;
	    m_ssm[plane][section][j].m_fiwidth = fiwidth; // total phi module width of j's module.
	    // A similar correction rounding module phi width to nearest multiple of ss width - for SCT and pixels
	    // E.g., if ss=50 strips, module width (768) will be rounded up to 800, so the total offset = 800*j
	    if(fiss==fiwidth)
	      m_ssm[plane][section][j].m_fioffset = fiwidth * j;
	    else
	      m_ssm[plane][section][j].m_fioffset = (fiwidth-fiwidth%fiss+fiss)*j; // total phi offset of j's module.
	    m_ssm[plane][section][j].m_cottss = cottss; // width of pixel or sct in r-phi direction
	    m_ssm[plane][section][j].m_cottwidth = cottwidth; // total phi module width of j's module.
	    if(cottss==cottwidth)
	      m_ssm[plane][section][j].m_cottoffset = cottwidth*j;
	    else
	      m_ssm[plane][section][j].m_cottoffset = (cottwidth-cottwidth%cottss+cottss)*j; // total phi offset of j's module.
	    m_ssm[plane][section][j].m_curvss = curvss; // width of pixel or sct in r-phi direction
	    m_ssm[plane][section][j].m_curvwidth = curvwidth; // total phi module width of j's module.
	    if(curvss==curvwidth)
	      m_ssm[plane][section][j].m_curvoffset = curvwidth*j;
	    else
	      m_ssm[plane][section][j].m_curvoffset = (curvwidth-curvwidth%curvss+curvss)*j; // total phi offset of j's module.
	  }
          if(cmDebug)
             printf("SS info. : %d.%d (%d %d-%d) %d %d %d %d %d %d\n",
             plane,section,type,isEC,layer, m_ssm[plane][section][0].m_fiss, m_ssm[plane][section][0].m_fioffset, m_ssm[plane][section][0].m_cottss, m_ssm[plane][section][0].m_cottoffset, m_ssm[plane][section][0].m_curvss, m_ssm[plane][section][0].m_curvoffset);
	}
      
  }  // end loop to read all the elements
  
  
  // setup hardware-specific parameters. fix these here. they are
  // variables to make it easier to change them.
  if( true ) {
    // hardware representation
    set_nbitsssid(15);
    set_nbitsssid_SSB(16);
    set_nbitsssid_module_pixel(6); // tower module id
    set_nbitsssid_module_strip(6); // tower module id
    set_eta_nondcbits(2);
  } else {
    // uncompressed software representation
    // 11+17=28 bits total input spec
    //   ssid: 6+12/7+11 = 18 bits (strips/pixels + module id)
    set_nbitsssid(18);
    set_nbitsssid_module_pixel( 11 ); // global module id
    set_nbitsssid_module_strip( 12 ); // global module id
    set_eta_nondcbits(2);
  }
  
  
//  set_phi_ss_width(getSSPhiWidth(hit));
//  set_eta_ss_width(getSSEtaWidth(hit));
//  if( hit.getDim()==2 ) {
//    set_max_phi(getSSOffsetInternal(hit));
//    set_max_etaphi(getSSOffset(hit));
//  } else {
//    set_max_phi(ssmap->getSSOffset(hit));
//    set_max_etaphi(0);
//  }
//  
  checkPrint("from_text_file");

  m_isok = true;
}

void FTKSSMap::checkPrint(const char *tag) const {
   int error=0;
  for (int iplane=0; iplane<m_rmap->getPlaneMap()->getNPlanes(); ++iplane) {
    // retrieve the number of sections in this plane
     int nsections = m_rmap->getPlaneMap()->getNSections(iplane);
    for (int isec=0;isec<nsections;++isec) {
      // get the maximum module of the last region using -1
      int maxmod = m_rmap->getRegionMapItem(-1,iplane,isec).getTotMod();
      if(maxmod<72) maxmod=72; // FIXME - cy temp hardcode for split arch
      for(int imod=0;imod<=maxmod;imod++) {
         if(m_ssm[iplane][isec][imod].m_phiss<0) error++;
      }
    }
  }
  if(error) {
     cout<<"========= FTKSSMap::checkPrint "<<tag<<" error="<<error<<"\n";
  }
}

FTKSSMap::FTKSSMap(TDirectory* file, TString DCorTSP) : m_isok(false) 
{

   m_useHWCoords = false; // need to set a default
   //! Read SSMap class from root file

   //cout<<"Instantiate RegionMap from root file."<<endl;
   TDirectory* thisdir = gDirectory;
   TString dirname = GetDCorTSPDirname(DCorTSP);

   if ( !file->GetDirectory(dirname) ) {
      cerr << "Error in FTKSSMap::FTKSSMap. No SSMap found in root-file. "<< endl;
      return;
   }
   else 
      file->cd(dirname);
	 
   m_path = gDirectory->GetTitle();

   TTree* tConst        = (TTree*)gDirectory->Get("Constants");
   TTree* tMap          = (TTree*)gDirectory->Get("SSMap");
   if ( !tConst || !tMap ) {
      cerr << "Error in FTKSSMap::FTKSSMap. Could not find trees. "<< endl;
      cerr << "  tree('Constants')     "<<tConst<<endl;
      cerr << "  tree('Map')          "<<tMap<<endl;
      return;
   }

   m_isok=true;

   // constants
   tConst->GetBranch("fraction")->SetAddress(&m_fraction);
   tConst->GetBranch("bound_checks")->SetAddress(&m_bound_checks);
   tConst->GetBranch("eta_nondcbits")->SetAddress(&m_eta_nondcbits);
   tConst->GetBranch("nbitsssid")->SetAddress(&m_nbitsssid);
   tConst->GetBranch("nbitsssid_SSB")->SetAddress(&m_nbitsssid_SSB);
   tConst->GetBranch("nbitsssid_module_pixel")->SetAddress(&m_nbitsssid_module_pixel);
   tConst->GetBranch("nbitsssid_module_strip")->SetAddress(&m_nbitsssid_module_strip);
   int chksum;
   tConst->GetBranch("Checksum")->SetAddress(&chksum);
   tConst->GetEntry(0);

   // RegionMap (and PlaneMap)
   m_rmap = FTKRegionMap::GetRMapFromRootFile(file);

   if ((!m_rmap)||(!m_rmap->getIsOK()) ) {
      cerr<<"Error in FTKSSMap::FTKSSMap. Could not get valid region map from file."<<endl;
      m_isok = false;
      delete m_rmap;
      return;
   }
   
   // ssmap
   struct ssm temp;
   tMap->GetBranch("phiss")->SetAddress(&temp.m_phiss);
   tMap->GetBranch("phiwidth")->SetAddress(&temp.m_phiwidth);
   tMap->GetBranch("phioffset")->SetAddress(&temp.m_phioffset);
   tMap->GetBranch("etass")->SetAddress(&temp.m_etass);
   tMap->GetBranch("etawidth")->SetAddress(&temp.m_etawidth);
   tMap->GetBranch("etaoffset")->SetAddress(&temp.m_etaoffset);

   tMap->GetBranch("ssoff")->SetAddress(&temp.m_ssoff);
   tMap->GetBranch("ssoff_int")->SetAddress(&temp.m_ssoff_int);
   tMap->GetBranch("ndcx")->SetAddress(&temp.m_ndcx);
   tMap->GetBranch("ndxy")->SetAddress(&temp.m_ndcy);

   tMap->GetBranch("fiss")->SetAddress(&temp.m_fiss);
   tMap->GetBranch("fiwidth")->SetAddress(&temp.m_fiwidth);
   tMap->GetBranch("fioffset")->SetAddress(&temp.m_fioffset);

   tMap->GetBranch("cottss")->SetAddress(&temp.m_cottss);
   tMap->GetBranch("cottwidth")->SetAddress(&temp.m_cottwidth);
   tMap->GetBranch("cottoffset")->SetAddress(&temp.m_cottoffset);

   tMap->GetBranch("curvss")->SetAddress(&temp.m_curvss);
   tMap->GetBranch("curvwidth")->SetAddress(&temp.m_curvwidth);
   tMap->GetBranch("curvoffset")->SetAddress(&temp.m_curvoffset);

   int ichk, jchk, kchk;
   tMap->GetBranch("iplane")->SetAddress(&ichk);
   tMap->GetBranch("isec")->SetAddress(&jchk);
   tMap->GetBranch("imod")->SetAddress(&kchk);
   
   int nev=0;
   const int nplanes = m_rmap->getPlaneMap()->getNPlanes();
   m_ssm = new struct ssm**[nplanes];
   for ( int iplane = 0 ; iplane<nplanes ; iplane++ ) {
      tMap->GetEntry(nev++);
      const int nsections = m_rmap->getPlaneMap()->getNSections(iplane);
      m_ssm[iplane] = new struct ssm*[nsections];
      for ( int jsec = 0 ; jsec<nsections ; jsec++ ) {
         if ( jsec!= 0 ) tMap->GetEntry(nev++);
	 const int maxmod = m_rmap->getRegionMapItem(-1,iplane,jsec).getTotMod();
	 m_ssm[iplane][jsec] = new struct ssm[maxmod+1];
	 for ( int kmod = 0 ; kmod<maxmod+1 ; kmod++ ) {
	    if ( kmod!= 0 ) tMap->GetEntry(nev++);
	    if ( ichk!= iplane || jsec!= jchk || kmod !=kchk  ){
	       cerr << "Error reading FTKSSMap."<<endl;
               m_isok = false;
	    }
	    m_ssm[iplane][jsec][kmod] = temp;
	 }
      }
   }

   thisdir->cd();
   m_isok =  m_isok && m_rmap->getIsOK();
   if ( chksum != CalcChecksum() ) {
      cerr<<"FTKSSMap::FTKSSMap. Error reading file. Checksum of file is not identical to new calculation."<<endl;
      m_isok=false;
   }
  checkPrint("from_root_file");
}


TString FTKSSMap::GetDCorTSPDirname(TString DCorTSP){
   TString dirname="";
   if ( DCorTSP == "DC" ) 
      dirname = FTKSSMap::s_dirnameDC;
   else if ( DCorTSP == "TSP" ) 
      dirname = FTKSSMap::s_dirnameTSP;
   else {
      cerr<<"Error in FTKSSMap::GetDCorTSPDirname. Argument DCorTSP must be 'DC' or 'TSP' but is: "<<DCorTSP<<endl;
      cerr<<"  Cannot write SSMap to root-file."<<endl;
   }
   return dirname;
}


void FTKSSMap::WriteMapToRootFile(TDirectory* file, TString DCorTSP){
   //! Write SSMap to root file
   //! Readable again using FTKSSMap(TDirectory* dir).
   
   TString dirname = GetDCorTSPDirname(DCorTSP);

   TDirectory* thisdir = gDirectory;
   // check if SSMap already exists
   if ( file->GetKey(dirname) ) {
      if ( FTKSSMap::GetChecksum(file,DCorTSP) == CalcChecksum()  ) {
	 //cout<<"Info. Identical SSMap already written to disk. Skipping writing."<<endl;
	 return;
      }
      else { 
	 cerr << "Error: [FTKSSMap::WriteMapToRootFile] There is already a SSMap in this file ("<<file->GetTitle()<<"), but with a different checksum" << endl;
	 cerr << "                                      SSMap title:            "<<ftk::StripFileName(getPath())<<endl;
	 cerr << "                                      This SSMap Checksum:    "<<CalcChecksum()<<endl;
	 cerr << "                                      SSMap in file checksum: "<<FTKSSMap::GetChecksum(file,DCorTSP)<<endl;
	 cerr << "                                      SSMap in file title:    "<<file->GetKey(dirname)->GetTitle()<<endl;
	 cerr << "       *** Skip writing *** "<<endl;
	 return;
      }
   }
   // create new directory for RegionMap content
   // std::string m_path; // configuration file path
   TString title = ftk::StripFileName(getPath());
   TDirectory* ssmapdir = file->mkdir(dirname,title);
   ssmapdir->cd();

   // // coordinate offset fraction. E.g, 0.5 for 1/2-ss shifted banks
   // double m_fraction;
   // bool m_bound_checks;
   // // global parameters for hardware compressed/DC SSID representation
   // int m_eta_nondcbits; // number of eta non-dc bits
   // int m_nbitsssid; // total number of ssid bits
   // int m_nbitsssid_module_pixel; // number of ssid bits to devote to the module id
   // int m_nbitsssid_module_strip; // number of ssid bits to devote to the module id

   TTree tConst("Constants","Constants");
   tConst.Branch("fraction",&m_fraction,"fraction/D");
   tConst.Branch("bound_checks",&m_bound_checks,"bound_checks/O");
   tConst.Branch("eta_nondcbits",&m_eta_nondcbits,"eta_nondcbits/I");
   tConst.Branch("nbitsssid",&m_nbitsssid,"nbitsssid/I");
   tConst.Branch("nbitsssid_SSB",&m_nbitsssid_SSB,"nbitsssid_SSB/I");
   tConst.Branch("nbitsssid_module_pixel",&m_nbitsssid_module_pixel,"nbitsssid_module_pixel/I");
   tConst.Branch("nbitsssid_module_strip",&m_nbitsssid_module_strip,"nbitsssid_module_strip/I");
   int chksum = CalcChecksum();
   tConst.Branch("Checksum",&chksum,"chksum/I");
   tConst.Fill();

   TTree tMap("SSMap","SSMap");
   struct ssm temp;
   tMap.Branch("phiss",&temp.m_phiss,"phiss/I");
   tMap.Branch("phiwidth",&temp.m_phiwidth,"phiwidth/I");
   tMap.Branch("phioffset",&temp.m_phioffset,"phioffset/I");
   tMap.Branch("etass",&temp.m_etass,"etass/I");
   tMap.Branch("etawidth",&temp.m_etawidth,"etawidth/I");
   tMap.Branch("etaoffset",&temp.m_etaoffset,"etaoffset/I");

   tMap.Branch("ssoff",&temp.m_ssoff,"ssoff/I");
   tMap.Branch("ssoff_int",&temp.m_ssoff_int,"ssoff_int/I");
   tMap.Branch("ndcx",&temp.m_ndcx,"ndcx/I");
   tMap.Branch("ndxy",&temp.m_ndcy,"ndcy/I");

   tMap.Branch("fiss",&temp.m_fiss,"fiss/I");
   tMap.Branch("fiwidth",&temp.m_fiwidth,"fiwidth/I");
   tMap.Branch("fioffset",&temp.m_fioffset,"fioffsett/I");

   tMap.Branch("cottss",&temp.m_cottss,"cottss/I");
   tMap.Branch("cottwidth",&temp.m_cottwidth,"cottwidth/I");
   tMap.Branch("cottoffset",&temp.m_cottoffset,"cottoffset/I");

   tMap.Branch("curvss",&temp.m_curvss,"curvss/I");
   tMap.Branch("curvwidth",&temp.m_curvwidth,"curvwidth/I");
   tMap.Branch("curvoffset",&temp.m_curvoffset,"curvoffset/I");
   // counters
   int iplane, jsec, kmod;
   tMap.Branch("iplane",&iplane,"iplane/I");
   tMap.Branch("isec",&jsec,"isec/I");
   tMap.Branch("imod",&kmod,"imod/I");

   // get the number of planes
   int nplanes = m_rmap->getPlaneMap()->getNPlanes();
   for ( iplane = 0 ; iplane<nplanes ; iplane++ ) {
      int nsections = m_rmap->getPlaneMap()->getNSections(iplane);
      for ( jsec = 0 ; jsec<nsections ; jsec++ ) {
	 int maxmod = m_rmap->getRegionMapItem(-1,iplane,jsec).getTotMod();
	 for ( kmod = 0 ; kmod<maxmod+1 ; kmod++ ) {
	    temp = m_ssm[iplane][jsec][kmod];
	    tMap.Fill();
	 }
      }
   }

   ssmapdir->Write();
   // FTKRegionMap *m_rmap;
   m_rmap->WriteMapToRootFile(file);
   
   thisdir->cd();
}

int FTKSSMap::GetChecksum(TDirectory* file, TString DCorTSP){
   // get nplanes from root file
   // if error: return -1
   TDirectory* thisdir = gDirectory;
   TString dirname = GetDCorTSPDirname(DCorTSP);

   if ( !file->cd(dirname) ) {
      cerr << "Error in FTKPlaneMap::GetNPlanes. No PlaneMap found in root-file. "<< endl;
	 return -1;
   } 

   TTree* tConst = (TTree*)gDirectory->Get("Constants");
   if ( !tConst ) {
      cerr << "Error in FTKPlaneMap::GetNPlanes. Could not find trees. "<< endl;
      cerr << "  tree('Constants')       "<<tConst<<endl;
	 return -1;
   }

   // constants:
   int ret;
   TBranch* br = tConst->GetBranch("Checksum");
   br->SetAddress(&ret);
   br->GetEntry(0);
   thisdir->cd();
   return ret;
}


int FTKSSMap::CalcChecksum() const {
   // calculate checksum of map
   vector<double> vec;

   vec.push_back((double)m_fraction);
   vec.push_back((double)m_bound_checks);
   vec.push_back((double)m_eta_nondcbits);
   vec.push_back((double)m_nbitsssid);
   vec.push_back((double)m_nbitsssid_module_pixel);
   vec.push_back((double)m_nbitsssid_module_strip);

   // get the number of planes
   int nplanes = m_rmap->getPlaneMap()->getNPlanes();
   for ( int iplane = 0 ; iplane<nplanes ; iplane++ ) {
      int nsections = m_rmap->getPlaneMap()->getNSections(iplane);
      for ( int jsec = 0 ; jsec<nsections ; jsec++ ) {
	 int maxmod = m_rmap->getRegionMapItem(-1,iplane,jsec).getTotMod();
	 for ( int kmod = 0 ; kmod<maxmod+1 ; kmod++ ) {
	    struct ssm temp = m_ssm[iplane][jsec][kmod];
	    vec.push_back((double)temp.m_phiss);
	    vec.push_back((double)temp.m_phiwidth);
	    vec.push_back((double)temp.m_phioffset);
	    vec.push_back((double)temp.m_etass);
	    vec.push_back((double)temp.m_etawidth);
	    vec.push_back((double)temp.m_etaoffset);
	    
	    vec.push_back((double)temp.m_ssoff);
	    vec.push_back((double)temp.m_ssoff_int);
	    vec.push_back((double)temp.m_ndcx);
	    vec.push_back((double)temp.m_ndcy);

	    // // buggy
	    vec.push_back((double)temp.m_fiss);
	    vec.push_back((double)temp.m_fiwidth);
	    vec.push_back((double)temp.m_fioffset);

	    // // buggy
	    vec.push_back((double)temp.m_cottss);
	    vec.push_back((double)temp.m_cottwidth);
	    vec.push_back((double)temp.m_cottoffset);
	    
	    // // buggy
	    vec.push_back((double)temp.m_curvss);
	    vec.push_back((double)temp.m_curvwidth);
	    vec.push_back((double)temp.m_curvoffset);
	 }
      }
   }

   // FTKRegionMap *m_rmap;
   int chkrmap = m_rmap->CalcChecksum();
   vec.push_back((double)chkrmap);

   size_t size = sizeof(double) * vec.size();
   int checksum = ftk::adler32chksum(&vec[0],size);

   //cerr<<"\t\t\tSSMAP::Checksum: "<<checksum<<",\t rmap.checksum: "<<chkrmap<<endl;
   return checksum;
   
}



FTKSSMap* FTKSSMap::GetSSMapFromRootFile(TDirectory* file,TString DCorTSP){
   //! Read regionmap class from root file
   //! return NULL if reading failed.
   //! check validity also using m_isok

   //if ( !ret->getIsOK() ) ...
   return new FTKSSMap(file,DCorTSP);
}


/** this method returns the phi width of the module rounded to be divided in an integer number of SS */
int FTKSSMap::getPhiWidthRounded(int plane) const
{
  const int &width = m_ssm[plane][0][0].m_phiwidth;
  const int &ss = m_ssm[plane][0][0].m_phiss;
  return width-width%ss+ss;
}


/** this method returns the eta width of the module rounded to be divided in an integer number of SS */
int FTKSSMap::getEtaWidthRounded(int plane) const
{
  /* replicates the formula: m_ssm[plane][section][j].m_etaoffset = (etawidth-etawidth%etass+etass)*j
   * when j is 1
   */

  const int &width = m_ssm[plane][0][0].m_etawidth;
  const int &ss = m_ssm[plane][0][0].m_etass;
  
  if (width==ss) return width; // this is an old days hack to avoid roundings when Z segmentation is off
  return width-width%ss+ss;
}


// second argument mode: 0 - regular, 1 - extrapolation
int FTKSSMap::getSSGlobal(const FTKHit &hit, bool quiet) const {
  int isector, iplane, isection(0), iphi(0), ieta(0), ss(0), ssadd(0);
  int ifi(0), icurv(0), icott(0); // cy
  int aside;
  double x(0),y(0);
  double z(0); // cy
  double offset = 0;


  // get the PMAP pointer
  const FTKPlaneMap *pmap = m_rmap->getPlaneMap();

  // cy debug
  //  if(hit.getDim()==0)
  //   cout << " the dimension is 0 here" << endl;

  isector = hit.getSector();
  iplane = hit.getPlane();
  if(hit.getDim()==1||hit.getDim()==2){
    ieta = hit.getEtaCode();
    iphi = hit.getPhiModule();
    isection = hit.getSection();
  }
  else if(hit.getDim()==3){
    //    ifi = floor(isector/10000);
    //    icott = floor((isector%10000)/100);
    //    icurv = floor(isector%100);
    ifi = static_cast<int>(floor(isector*1.0/10000)); 
    icott = static_cast<int>(floor((isector%10000)*1.0/100));
    icurv = static_cast<int>(floor((isector%100)*1.0));
    // cy debug
    // cout << "isector: " << isector << "\tifi: " << ifi << "\ticott: "<<icott << "\ticurv: " << icurv << endl;
    isection = 0;
  }
  aside = hit.getASide();

  const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  int cmDebug = ftkset.getVerbosity();
  //int cmDebug = 6;

  if (cmDebug > 4) {
    printf("\nIn ss_hitM, plane %d section %d ieta %d iphi %d", iplane, isection, ieta, iphi );
    FTKSetup::PrintMessageFmt(ftk::info,"\tsector is %d\n",isector);
  }

  // Note that we use the ETA_CODE, not just module eta.
  // For the barrel, it's still just module eta
  // but for the endcap (ETA_CODE > 20) it also contains the section
  // information (see lib/ftk.h).  
  // We want to retain the same encoding for superstrips that we
  // used for encoding modules for sectors.	... cmills

  //FlagKY : modify to handle raw hits for pattern generation.
  if(hit.getDim() == 1) x = hit[0];
  else if(hit.getDim() == 2) {
    x = hit[0];
    y = hit[1];
  }
  else if(hit.getDim() == 3) {
    x = hit[0];
    y = hit[1];
    z = hit[2];
  }

  if(hit.getDim()==1 || hit.getDim()==2){
    // offset for 1/2 ss shifted banks
    offset = -m_fraction*m_ssm[iplane][isection][iphi].m_phiss;
    if(offset > 0) printf("\nIn ss_hitM, plane %d section %d ieta %d iphi %d offset %f", iplane, isection, ieta, iphi, offset );
    if(x+offset<0) offset=0; // protect left edge
  }

  // pixel - including eta partitioning
  if(hit.getDim() == 2) { 

    int phioff = FTKSSMap::getPhiOffset(false,FTKSetup::getFTKSetup().getITkMode()); // hardcoded in FTKSSMap.h (default = 200 for pixels, 1000 if ITkMode=true)
    // phi piece:
    // STS 15-06-10 add check phiss must be >0
    int phiss=m_ssm[iplane][isection][iphi].m_phiss;
    if(phiss<=0) {
       if (((cmDebug > 1 ) && !quiet) ||(cmDebug>5))
          FTKSetup::PrintMessageFmt
             (ftk::warn,"FTKSSMap %s iplane=%d isect=%d iphi=%d ieta=%d phiss=%d x=%lf y=%lf\n",
              m_path.c_str(),iplane,isection,iphi,ieta,phiss,x,y);
       // STS 15-06-10
       return -1;
    }
    ss = static_cast<int>(floor( (m_ssm[iplane][isection][iphi].m_phioffset+x+offset)/phiss )*phioff);
    if(x<0 || x>=m_ssm[iplane][isection][iphi].m_phiwidth) {
       if (((cmDebug > 2 )&&(!quiet))||(cmDebug>5)) {
        FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module x (%f) exceeds declared module bounds (pixel phi) (%f): plane=%d,section=%d,iphi=%d\n",
                                  x,(m_ssm[iplane][isection][iphi].m_phiwidth),iplane,isection,iphi);
        return -1;
      }
    }
    
    // Recall: SS_LOOKUP_SIZE = 360k
    // Min ss width is found via: 52*(328+width)/width*200 = 360k  ==> min pixel-phi ss is 10
    // for ibl we want 336, so 52*(336+width)/width*200 = 500k ==> min pixel-phi ss is ok 10 works

    // eta piece (for the barrel) - note that section=0
    // STS 15-06-10 add check: etass must be >0
    int etass=m_ssm[iplane][isection][ieta].m_etass;
    if(etass<=0) {
       if (((cmDebug > 1 ) && !quiet) ||(cmDebug>5))
          FTKSetup::PrintMessageFmt
             (ftk::warn,"FTKSSMap %s iplane=%d isect=%d iphi=%d ieta=%d etass=%d x=%lf y=%lf\n",
              m_path.c_str(),iplane,isection,iphi,ieta,etass,x,y);
       // STS 15-06-10
       return -1;
    }
    if(ieta<20) {
      ssadd = static_cast<int>(floor((m_ssm[iplane][isection][ieta].m_etaoffset+y)/etass));
      if(y<0 || y>=m_ssm[iplane][isection][ieta].m_etawidth) {
         if ((!quiet)||(cmDebug>5)) {
          FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module y (%f) exceeds declared module bounds (pixel eta barrel) (%d), iplane=%d, isection=%d, iphi=%d\n",
                                    y,m_ssm[iplane][isection][ieta].m_etawidth,iplane,isection,iphi);
          return -1;
        }
      }
    }
    // eta piece (for the endcap)
    else {
      ssadd = static_cast<int>(floor((m_ssm[iplane][isection][14+3*aside+isection].m_etaoffset+y)/etass));
      if(y<0 || y>=m_ssm[iplane][isection][14+3*aside+isection].m_etawidth) {
         if ((!quiet)||(cmDebug>5)) {
          FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module y (%f) exceeds declared module bounds (pixel eta endcap) (%f)\n",
                                    y,m_ssm[iplane][isection][14+3*aside+isection].m_etawidth);
          return -1;
        }
      }
    }
    // Min ss width is found via: 144*19/width = 200  ==> min pixel-eta ss is 15
    
    // check that the eta-piece doesn't exceed the allocated limit of 200
    if(ssadd>=phioff) {
       if ((!quiet)||(cmDebug>5)) {
        cout << "Plane " << iplane << " | Section " << isection
             << " | Phi " << iphi << " | Eta " << ieta
             << " | X " << x << " => ss " << ss << " | ssadd " << ssadd << endl;
        FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ssadd exceeds the limit of %d",phioff);
      }
    }
    ss+=ssadd;
  }
  
  // SCT - using original Corrinne's encoding
  else if (hit.getDim() == 1) {
    int phioff = FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode()); // hardcoded in FTKSSMap.h (default = 100 for SCT, 1000 if ITkMode=true)
    // STS 15-06-10 add check phiss must be >0
    int phiss=m_ssm[iplane][isection][iphi].m_phiss;
    if(phiss<=0) {
       if (((cmDebug > 1 ) && !quiet) ||(cmDebug>5))
          FTKSetup::PrintMessageFmt
             (ftk::warn,"FTKSSMap %s iplane=%d isect=%d iphi=%d ieta=%d phiss=%d x=%lf\n",
           m_path.c_str(),iplane,isection,iphi,ieta,phiss,x);
       return -1;
    }
    // STS 15-06-10 add check ieta <phioff
    if(ieta>=phioff) {
       if (((cmDebug > 1 ) && !quiet) ||(cmDebug>5))
          FTKSetup::PrintMessageFmt
             (ftk::warn,"FTKSSMap %s iplane=%d isect=%d iphi=%d ieta=%d>=phioff=%d phiss=%d x=%lf\n",
              m_path.c_str(),iplane,isection,iphi,ieta,phioff,phiss,x);
       //return -1;
    }
    ss = static_cast<int>(floor( (m_ssm[iplane][isection][iphi].m_phioffset+x+offset)/phiss )*phioff+(ieta));

    if( ftkset.getITkMode() ) {
      
      // An extra cross-check here just to ensure that new ITk changes are working
      // correctly. We try decoding the SS and ensure that we get output values that
      // match the inputs used to encode this SS value... basically a closure test.
      
      int jsection, jphimod, jlocalX, jetacode;
      decodeSSx( ss , iplane , jsection , jphimod , jlocalX , jetacode );
      
      if( jsection!=isection || iphi!=jphimod || jetacode!=ieta || abs(jlocalX-x)>phiss )
	FTKSetup::PrintMessageFmt( ftk::warn ,
				   "FTKSSMap::getSSglobal failed to get reasonable values in decode check for ss=%i hitsector=%i because section:%d!=%d || phimod:%d!=%d || etacode:%d!=%d || x:%f-%d>%d\n" ,
				   ss , hit.getSector() , isection , jsection , iphi , jphimod , ieta , jetacode , x , jlocalX , phiss );
      
    }
    
    
    if(x<0 || x>=m_ssm[iplane][isection][iphi].m_phiwidth) {
       if ((!quiet)||(cmDebug>5)) {
	 //cout << "Plane " << iplane << " | Section " << isection << " | Phi " << iphi << " | Eta " << ieta << " | X " << x << " => ss " << ss << endl;
	 //cout << "ss width = " << m_ssm[iplane][isection][iphi].m_phiss << ", offset = " << m_ssm[iplane][isection][iphi].m_phioffset << endl;
	 //printf("hit info: dPhi=%f\n",hit.getDeltaPhi()
      }
      if (cmDebug > 2 )
        FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module x (%f) exceeds declared module bounds (SCT phi) (%d)\n",
                                  x,m_ssm[iplane][isection][iphi].m_phiwidth);
      return -1;
    }
    if(ieta>=phioff) {
       if (((cmDebug > 2 )&& !quiet)||(cmDebug>5))
        FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ieta part of ss definition (%d) exceeds phi offset (SCT eta) (%d)\n",
                                  ieta,phioff);
    }
    // Recall: SS_LOOKUP_SIZE = 360k
    // So the max width is found via: 56*(768+width)/width*100 = 360k  ==> min sct ss is 15
  }
  else if (hit.getDim() == 3) {    

    double fisecwidth = 2*ftk::PI/pmap->getPlane(iplane,isection).getNumFi();
    double fimin = ifi*fisecwidth;
    if(x>0)
      x = (x-fimin)/fisecwidth*m_ssm[iplane][isection][ifi].m_fiwidth;
    else
      x = (x+2*ftk::PI-fimin)/fisecwidth*m_ssm[iplane][isection][ifi].m_fiwidth;

#ifdef SPLIT_OLDCTHETA
    double cottsecwidth = 2*ftk::COTTMAX/pmap->getPlane(iplane,isection).getNumCott();
    double cottmin = icott*cottsecwidth-ftk::COTTMAX;
#else
    // for cot(theta), we now use a hardcoded table for sector bounds - see FTKSplitEncoder.{h,cxx}
    FTKSplitEncoder &senc = FTKSplitEncoder::get();
    senc.cthetaIndex(y);
    double cottsecwidth = senc.cthetaUpperBound()-senc.cthetaLowerBound();
    double cottmin = senc.cthetaLowerBound();
#endif
    y = (y-cottmin)/cottsecwidth*m_ssm[iplane][isection][icott].m_cottwidth;
    if(y<0) y = 0;

    double curvsecwidth = 2*ftk::CURVMAX/pmap->getPlane(iplane,isection).getNumCurv();
    double curvmin = icurv*curvsecwidth-ftk::CURVMAX;
    z = (z-curvmin)/curvsecwidth*m_ssm[iplane][isection][icurv].m_curvwidth;
    if(z<0) z = 0;

    ss = static_cast<int>(floor((m_ssm[iplane][isection][ifi].m_fioffset+x)/m_ssm[iplane][isection][ifi].m_fiss )*1000000+floor((m_ssm[iplane][isection][icott].m_cottoffset+y)/m_ssm[iplane][isection][icott].m_cottss)*1000+floor((m_ssm[iplane][isection][icurv].m_curvoffset+z)/m_ssm[iplane][isection][icurv].m_curvss ));
  }

  //   printf("Plane %d | Section %d | iPhi %d | Phiss %d | Phioffset %d | X %lf | iCott %d | Y %lf | iCurv %d | Z %lf => %d\n",
  //	   iplane, isection, ifi,m_ssm[iplane][isection][ifi].m_fiss ,m_ssm[iplane][isection][ifi].m_fioffset, x, icott, y, icurv, z, ss);
  // check if ss number is >=0
  if (!quiet)
    assert(ss>=0);

  if (cmDebug>2) 
    printf("Reporting superstrip %d for sector %d x %f\n",ss,isector,x);
  if(hit.getDim()==1 || hit.getDim()==2){
    if(ss<0 || (m_bound_checks && ss>=SS_LOOKUP_SIZE)) { 
      if (!quiet) {
         cout<<m_path 
             <<" dim="<<hit.getDim()
             << " Plane " << iplane << " | Section " << isection << " | Phi " << iphi << " | Eta " << ieta << " | X " << x;
         if(hit.getDim()==2) {
            cout<< " | Y "<< y;
         }
         cout << " => " << ss << endl;
         cout<<"maxmod="
             <<m_rmap->getRegionMapItem(-1,iplane,isection).getTotMod()
             <<" phiss="<<m_ssm[iplane][isection][iphi].m_phiss
             <<" etass="<<m_ssm[iplane][isection][ieta].m_etass
             <<"\n";
        FTKSetup::PrintMessage(ftk::sevr,"getSSGlobal() ss_hit: ss is negative or too large");
      }
    }
  }
  else if(0 && hit.getDim()==3){
    cout << "Plane " << iplane << "  | Section << " << isection << " | Phi " << ifi << " | Phiss " << m_ssm[iplane][isection][ifi].m_fiss << " | Phioffset " << m_ssm[iplane][isection][ifi].m_fioffset << " | X " <<  x << " => " << ss << endl;
  }
  
  if (cmDebug > 4) 
    FTKSetup::PrintMessageFmt(ftk::info,"Successfully exiting ss_hitM with ss = %d\n",ss);
  return ss;
}


int FTKSSMap::ssround(int width, int ss)
{
  // find the number of SS
  int nss = width/ss;
  if (width%ss!=0) nss += 1; // add 1 to keep not complete SS
  return nss;
}


int FTKSSMap::getSSTower(const FTKHit &hit, unsigned int TowerID,bool SecondStage) const {
  int ss(-1);

  // old calculation:
  //   SCT SSID is simply moduleLocalID*ssoff + x/phiss for a given module
  //   pixel SSID is 2d version of this (y/etass*ssoff+(x/phiss))
  //
  // new encoding: (lowest order bits of the eta and phi superstrip number are stored in consecutive bits.)
  //    	(eta/eta_ss_width)[n...1] (phi/phi_ss_width)[n...1] (eta/eta_ss_width)[0] (phi/phi_ss_width)[0]
  //
  // There are two lookup tables in the SSMAP (phi,eta -> local SSID and module_id -> offset). The SSID lookup table has the following 'configuration parameters':
  //
  //	phi_ss_width
  //	eta_ss_width
  //	phi_dcbits
  //	eta_dcbits
  //	max_phi
  //	max_eta
  //	compress/expanded
  //
  //  The module_id -> tower id hash
  //  -> max ss/module * tower mod_id
  //
  //  the max ss/module can be compressed or expanded depending on how many ssid bits are available.
  //  
  //  The global ssid is still the arithmetic sum of the ssid_offset and the local ssid.

  int localID(-1);
  
  if( FTKSetup::getFTKSetup().getHWModeSS()==2 ) {
      localID = m_rmap->getLocalId(TowerID,hit.getPlane(),hit.getIdentifierHash());
      if ( localID==-1 ) {
        ostringstream errmsg;
        errmsg << "getSSTower: Error in the DC configuration, ID " << hit.getIdentifierHash() << " not found in Tower " << TowerID << " layer " << hit.getPlane() << ends;
        throw FTKException(errmsg.str().c_str());
      }
      return compressed_ssid_word(hit,localID,SecondStage);
  }

  localID = m_rmap->getLocalModuleID(hit,TowerID);

  //  if(hit.getDim()==2) {
  //    // phi piece:
  //    ss = m_ssm[plane][section][phi].m_ssoff*localID;
  //    ss += (iy/m_ssm[plane][section][phi].m_etass)*m_ssm[plane][section][phi].m_ssoff_int+ix/m_ssm[plane][section][phi].m_phiss; // LSB used for the SSID within the module, no DC bits are reserved at this stage
  //  }
  //  // SCT - using original Corrinne's encoding
  //  else {
  //    ss = m_ssm[plane][section][phi].m_ssoff*localID;
  //    ss += ix/m_ssm[plane][section][phi].m_phiss; // LSB used for the SSID within the module, no DC bits are reserved at this stage
  //  }

  
  // get the PMAP pointer
  //const FTKPlaneMap *pmap = m_rmap->getPlaneMap();

  // cy debug
  //  if(hit.getDim()==0)
  //   cout << " the dimension is 0 here" << endl;

  const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  int cmDebug = ftkset.getVerbosity();
  //int cmDebug = 6;
  

  double x(0), y(0);
  if(hit.getDim() == 1) x = hit[0];
  else if(hit.getDim() == 2) {
    x = hit[0];
    y = hit[1];
  }
  else {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Hit with dim %d not supported",hit.getDim());
    return -1;
  }

  int plane = hit.getPlane(); // logical layer
  int eta = hit.getEtaCode(); //
  int phi = hit.getPhiModule();
  int section = hit.getSection();
  int sector = hit.getSector();

  int ix = x;
  int iy = y;

  // pixel - including eta partitioning
  if(hit.getDim()==2) {
    
    // phi piece:
    ss = m_ssm[plane][section][phi].m_ssoff*localID;
    ss += (iy/m_ssm[plane][section][phi].m_etass)*m_ssm[plane][section][phi].m_ssoff_int+ix/m_ssm[plane][section][phi].m_phiss; // LSB used for the SSID within the module, no DC bits are reserved at this stage
  }
  // SCT - using original Corrinne's encoding
  else {
    ss = m_ssm[plane][section][phi].m_ssoff*localID;
    ss += ix/m_ssm[plane][section][phi].m_phiss; // LSB used for the SSID within the module, no DC bits are reserved at this stage
  }
  
  if (cmDebug) 
    printf("Reporting superstrip %d for sector %d x %f\n",ss,sector,x);

  if(hit.getDim()==1 || hit.getDim()==2){
    if(ss<0 || ss>=SS_LOOKUP_SIZE) {
      printf("Plane %d | Section %d | Phi %d | Eta %d | X %f => %d\n",
             plane, section, phi, eta, x, ss);
      FTKSetup::PrintMessage(ftk::sevr,"getSSTower() ss_hit: ss is negative or too large");
    }
  }

  return ss;
}

#ifdef UNUSED
/* this method decode a SS ID used in the Tower format 
   as input it needs a SSID, the tower id and the logical layer ID.
*/
void FTKSSMap::decodeSSTower(int ssid, int towerid, int plane, int section,
                             int &phimod, int &etamod, 
                             float &localX, float &localY)
{
  // extract the local module ID and the local SS part
  unsigned int localID = ssid/m_ssm[plane][section][0].m_ssoff;
  unsigned int localSS = ssid%m_ssm[plane][section][0].m_ssoff;

  // convert the local module ID in absolute eta and phi
  m_rmap->convertLocalID(localID,towerid,plane,section,phimod,etamod);
  if (m_rmap->getPlaneMap()->getPlane(plane,section).getNDimension()==2) {
    // extract the X and Y part
    unsigned int localSSx = localSS%m_ssm[plane][section][0].m_ssoff_int;
    unsigned int localSSy = localSS/m_ssm[plane][section][0].m_ssoff_int;
    
    // strip dimension equal for all the phi
    localX = localSSx*m_ssm[plane][section][0].m_phiss;
    localY = localSSy*m_ssm[plane][section][0].m_etass;
  }
  else { // SCT case
    // same strip dimension for all phi
    localX = localSS*m_ssm[plane][section][0].m_phiss;
    localY = 1;
  }
}
#endif

/* this method decode a SS ID used in the Tower format (HWMODEID=2) 
   as input it needs a SSID, the tower id and the logical layer ID.
   It handles the pixel layers
*/
void FTKSSMap::decodeSSTowerXY(int ssid, int /*towerid*/, int plane, int section,
                               int &localmoduleID,int &localX, int &localY, bool SecondStage) {
   int phi_dc_bits = getSSDCX(plane);
   int eta_dc_bits = getSSDCY(plane);
   
   int nbitsSSID_available = m_nbitsssid;
   if(SecondStage) nbitsSSID_available = m_nbitsssid_SSB;

   const unsigned int max_compressed_ssid_phi_dc =
      std::min( 1<<phi_dc_bits , getSSOffsetInternal(plane,section) );
   const unsigned int max_compressed_ssid_etaphi_dc =
      std::min( 1<<eta_dc_bits<<phi_dc_bits , getSSOffset(plane,section) );
   const unsigned int total_localssid_bits =
      nbitsSSID_available-m_nbitsssid_module_pixel;
   const unsigned int total_nondc_bits =
     total_localssid_bits-phi_dc_bits-eta_dc_bits;
   const unsigned int nondcbits_x =
     std::min((unsigned int)
              ceil(log(getSSOffsetInternal(plane,section))/log(2.)),
              total_nondc_bits );
   const unsigned int max_nondc_lx =
      std::min( 1<<nondcbits_x,
                getSSOffsetInternal(plane,section) >> phi_dc_bits);
  const unsigned int max_nondc_lyx =
     std::min( 1<<total_nondc_bits,
               getSSOffset(plane,section) >> phi_dc_bits >> eta_dc_bits );
   unsigned int total_offset = max_nondc_lyx*max_compressed_ssid_etaphi_dc;
   total_offset = (int)ceil(total_offset/(1.*(1<<(phi_dc_bits+eta_dc_bits))))*
      (1<<(phi_dc_bits+eta_dc_bits));
   unsigned multP3=max_nondc_lx*max_compressed_ssid_etaphi_dc;

   localmoduleID=ssid/total_offset;
   //moduleID=m_rmap->getGlobalId(towerid,plane,localModuleID);

   unsigned local_ssid=ssid%total_offset;

   unsigned nondc_y=local_ssid/multP3;
   unsigned p0p1p2=local_ssid%multP3;
   unsigned nondc_x=p0p1p2/max_compressed_ssid_etaphi_dc;
   unsigned dc_xy=p0p1p2%max_compressed_ssid_etaphi_dc;
   unsigned dc_y=dc_xy/max_compressed_ssid_phi_dc;
   unsigned dc_x=dc_xy%max_compressed_ssid_phi_dc;
   // decode gray code 
   for(unsigned tmp=dc_x>>1;tmp; tmp>>=1) dc_x^=tmp;
   for(unsigned tmp=dc_y>>1;tmp; tmp>>=1) dc_y^=tmp;

   int x,y;
   x=(nondc_x<<phi_dc_bits)+dc_x;
   y=(nondc_y<<eta_dc_bits)+dc_y;
   localX= x*m_ssm[plane][section][0].m_phiss;
   localY= y*m_ssm[plane][section][0].m_etass;

   //std::cout<<"decodeSSTowerXY ssid="<<ssid<<" tower="<<towerid<<" plane="<<plane<<" section="<<section<<"\n";
   //std::cout<<" -> phimod="<<phimod<<" etamod="<<etamod 
   //         <<"  localX="<<localX<<" localY="<<localY<<"\n";
}

/* this method decode a SS ID used in the Tower format (HWMODEID=2) 
   as input it needs a SSID, the tower id and the logical layer ID.
   It handles the strip layers
*/
void FTKSSMap::decodeSSTowerX(int ssid, int /*towerid*/, int plane, int &section,
			      int &localmoduleID,int &localX, bool SecondStage) {
   int phi_dc_bits = getSSDCX(plane);

   int nbitsSSID_available = m_nbitsssid;
   if(SecondStage) nbitsSSID_available = m_nbitsssid_SSB;
   
   const unsigned int total_localssid_bits =
      nbitsSSID_available-m_nbitsssid_module_strip;
   unsigned int max_compressed_ssid =
      std::min((1<<total_localssid_bits)-1,getSSOffset(plane,section));
   max_compressed_ssid =
      (int)ceil(max_compressed_ssid/(1.*(1<<phi_dc_bits)))*(1<<phi_dc_bits);
   localmoduleID=ssid/max_compressed_ssid;
   //moduleID=m_rmap->getGlobalId(towerid,plane,localModuleID);

   int local_ssid= ssid%max_compressed_ssid;
   // extract DC bits
   int dc_x=local_ssid &((1<<phi_dc_bits)-1);
   // clear dc bits in result
   local_ssid^=dc_x;
   // gray-decode dc bits
   for(unsigned tmp=dc_x>>1;tmp;tmp>>=1) dc_x ^= tmp;
   // put gray-decoded bits back to result
   local_ssid |= dc_x;

   localX=local_ssid* m_ssm[plane][section][0].m_phiss;
   //std::cout<<"decodeSSTowerX ssid="<<ssid<<" tower="<<towerid<<" plane="<<plane<<" section="<<section<<"\n";
   //std::cout<<" -> phimod="<<phimod<<" etamod="<<etamod 
   //         <<"  localX="<<localX<<"\n";
}

/** This method returns, for Pixel layers, the SS number passing the
    module's information and the cluster coordinate */
int FTKSSMap::getSSxy(int plane,int section, 
                      int phimod, int etaoff,
                      int x,int y) const
{
  int phioff = FTKSSMap::getPhiOffset(false,FTKSetup::getFTKSetup().getITkMode()); // hardcoded in FTKSSMap.h (default = 100 for SCT, 1000 if ITkMode=true)

  int phiss=m_ssm[plane][section][phimod].m_phiss;
  int etass=m_ssm[plane][section][etaoff].m_etass;
  int ss=-1;
  if((phiss>0)&&(etass>0)) {
     ss= static_cast<int>
        (floor( (m_ssm[plane][section][phimod].m_phioffset+x)/phiss )*phioff);
     // Recall: SS_LOOKUP_SIZE = 360k
     // Min ss width is found via: 52*(328+width)/width*200 = 360k  ==> min pixel-phi ss is 10
     // for ibl we want 336, so 52*(336+width)/width*200 = 500k ==> min pixel-phi ss is ok 10 works

     int ssadd(0);
     // eta piece (for the barrel) - note that section=0
     ssadd = static_cast<int>
        (floor((m_ssm[plane][section][etaoff].m_etaoffset+y)/etass));

     // Min ss width is found via: 144*19/width = 200  ==> min pixel-eta ss is 15

     // check that the eta-piece doesn't exceed the allocated limit of 200
     if(ssadd>=phioff) {
        cout << "Plane " << plane << " | Section " << section   \
             << " | Phi " << phimod << " | Eta " << etaoff              \
             << " | X " << x << " => ss " << ss << " | ssadd " << ssadd << endl;
        FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ssadd exceeds the limit of %d",phioff);
     }
     ss+=ssadd;
  }      
  if(ss<0) {
     cout<<" FTKSSMap::getSS(pixel) "
         <<"ss="<<ss<<" phiss"<<phiss
         <<" plane,section,phimod,etaoff,x,y="
         <<plane<<" "<<section<<" "<<phimod<<" "<<etaoff<<" "<<x<<" "<<y
         <<"\n";
  }

  return ss;
}

/** This method returns, for SCT layers, the SS number passing the
    module's information and the cluster coordinate */
int FTKSSMap::getSSx(int plane,int section, 
                    int phimod,int etaoff,
                    int x) const
{
  int phioff = FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode()); // hardcoded in FTKSSMap.h (default = 100 for SCT, 1000 if ITkMode=true)
  
  int phiss=m_ssm[plane][section][phimod].m_phiss;
  int ss=-1;
  /* if(phiss>=100) {
     cout<<"FTKSSMap::getSS error: plane="<<plane<<" section="<<section
         <<" phimod="<<phimod<<" phiss="<<phiss<<"\n";
     checkPrint("getSS");
     } */
  if(phiss>0) {
     ss= static_cast<int>
        (floor( (m_ssm[plane][section][phimod].m_phioffset+x)/phiss )*phioff+
         (etaoff));
  }
  if(ss<0) {
     cout<<" FTKSSMap::getSSx() "
         <<"ss="<<ss
         <<" plane,section,phimod,etaoff,x="
         <<plane<<" "<<section<<" "<<phimod<<" "<<etaoff<<" "<<x
         <<" phioff="<<phioff
         <<" m_ssm[plane][section][phimod].m_phioffset="
         <<m_ssm[plane][section][phimod].m_phioffset
         <<" m_ssm[plane][section][phimod].m_phiss="
         <<m_ssm[plane][section][phimod].m_phiss
         <<"\n";
  }
  return ss;
}


/** this method get as input a valid SS for this map and return,
    in the reference variables: eta offset, phioffset and phi
    module id. These variables can be used to encode an hit in a
    different SS map.
    SCT version, with just a one-dimensional local SS id */
void FTKSSMap::decodeSSx(int SSid,int plane,int &section,
                        int &phimod,int &localX, int &etacode)
   const {
 
  /* start to decompose the information codified in the ID */
  // SSid: Superstrip ID
  // plane

  // OUTPUTS
  // etaoff: eta module
  // section: eta section
  // phimod: phi module value
  // phioff: offset for first phi superstrip within the module (note: in "phi SS units, meaning after eta part was separated)
  // localX: lower edge of the superstrip in strip units within the module

  // reduce the information to the phi information
  int SSid_x = SSid/FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode());
  // extract eta offset
  int SSid_y = SSid%FTKSSMap::getPhiOffset(true,FTKSetup::getFTKSetup().getITkMode());
  // decide barrel/endcap and section number
  static FTKHit dummyHit;
  dummyHit.setITkMode(FTKSetup::getFTKSetup().getITkMode());
  dummyHit.setSector(SSid_y);
  section=dummyHit.getSection();
  etacode=dummyHit.getEtaCode();
  phimod=-1;
  localX=-1;
  int nsolution_phi=0;
  int nConflict=0;
  int phiwidth_found=-1;
  int phiss_found=-1;
  int module_max=
     m_rmap->getRegionMapItem(-1,plane,section).getTotMod()-1;
  if(FTKSetup::getFTKSetup().getIBLMode()>=1) module_max+=3;
  if(FTKSetup::getFTKSetup().getITkMode()) module_max+=1;
  for(int test_module=module_max;test_module>=0;test_module--) {
     int phiwidth = m_ssm[plane][section][test_module].m_phiwidth;
     int phiss = m_ssm[plane][section][test_module].m_phiss;
     if((phiss>0)&&(phiwidth>0)) {
        int test_localX=
           (SSid_x*phiss-m_ssm[plane][section][test_module].m_phioffset);
        if((test_localX>=0)&&(test_localX<phiwidth)) {
           phimod=test_module;
           localX=test_localX;
           nsolution_phi++;
           if(phiwidth_found>0) {
              if((phiwidth_found!=phiwidth)||
                 (phiss_found!=phiss)) {
                 nConflict++;
              }
           } else {
              phiwidth_found=phiwidth;
              phiss_found=phiss;
           }
        }
     }
  }
  if(nConflict) {
     FTKSetup::PrintMessageFmt
        (ftk::sevr,"FTKSSMap::decodeSSx SSid=%d: %d solutions\n",SSid,nsolution_phi);
  }
  if(nsolution_phi!=1) {
     FTKSetup::PrintMessageFmt
        (ftk::warn,"FTKSSMap::decodeSSx [strip] ssid=%d plane=%d: found %d solutions in phi\n",
         SSid,plane,nsolution_phi);
  }
}


/** this method get as input a valid SS for this map and return,
    in the reference variables: eta offset, phioffset and phi
    module id. These variables can be used to encode an hit in a
    different SS map 
    PIXEL version, use two integers to return the local SS position */
void FTKSSMap::decodeSSxy(int SSid,int plane,int &section,
                          int &phimod, int &localX,
                          int &etaoff, int &localY) const
{
 
  /* start to decompose the information codified in the ID */
  // SSid: Superstrip ID
  // plane
  // section

  // OUTPUTS
  // etamod: eta module value
  // etaoff: offset for first eta pixel within the module (note: in "eta SS units, meaning after phi part was separated)
  // localY: lower edge of the eta SS in pixel units within the module
  // phimod: phi module value
  // phioff: offset for first phi pixel within the module (note: in "phi SS units, meaning after eta part was separated)
  // localX: lower edge of the phi SS in pixel units within the module


  // reduce the information to the phi information
  int SSid_x = SSid/FTKSSMap::getPhiOffset(false,FTKSetup::getFTKSetup().getITkMode());
  // extract eta information
  int SSid_y = SSid%FTKSSMap::getPhiOffset(false,FTKSetup::getFTKSetup().getITkMode());

  // STS: 2015/09/09
  //  the original code below assumed that all modules have the same
  //   eta,phi width, independent of the module position
  //   this may not be correct in all cases.
  //  now there is a loop over the possible section,eta,phi module coordinates

  section=-1;
  phimod=-1;
  localX=-1;
  etaoff=-1;
  localY=-1;
  int nSolution_eta=0;

  // barrel
  int barrel_max=13;  // quick fix, not valid for IBL
  int test_section=0;
  for(int test_module=barrel_max;test_module>=0;test_module--) {
     int etawidth = m_ssm[plane][test_section][test_module].m_etawidth;
     int etass = m_ssm[plane][test_section][test_module].m_etass;
     if((etass>0)&&(etawidth>0)) {
        int test_localY=
           SSid_y*etass-m_ssm[plane][test_section][test_module].m_etaoffset;
        if((test_localY>=0)&&(test_localY<etawidth)) {
           section = test_section;
           etaoff = test_module;
           localY =test_localY;
           nSolution_eta++;
        }
     }
  }
  int nsections = m_rmap->getPlaneMap()->getNSections(plane);
  for(test_section=0;test_section<nsections;test_section++) {
     for(int aside=0;aside<2;aside++) {
        int test_module=14+3*aside+test_section;
        int etawidth = m_ssm[plane][test_section][test_module].m_etawidth;
        int etass = m_ssm[plane][test_section][test_module].m_etass;
        if((etass>0)&&(etawidth>0)) {
           int test_localY=
              SSid_y*etass-m_ssm[plane][test_section][test_module].m_etaoffset;              
           if((test_localY>=0)&&(test_localY<etawidth)) {
              if(section>=0) {
                 cout<<"first solution: "<<section<<" "<<etaoff<<" "<<localY<<"\n";
                 cout<<"second solution: "<<test_section<<" "<<test_module<<" "<<test_localY<<" "<<etawidth<<" "<<etass<<"\n";
              }
              section = test_section;
              etaoff = test_module;
              localY =test_localY;
              nSolution_eta++;
           }
        }
     }
  }
  if(nSolution_eta!=1) {
     cout<<"SSid_x="<<SSid_x<<" SSid_y="<<SSid_y<<"\n";
     FTKSetup::PrintMessageFmt
        (ftk::sevr,"FTKSSMap::decodeSSxy %d plane=%d found %d valid sections\n",SSid,plane,nSolution_eta);
  }
  int nSolution_phi=0;
  int module_max=
     m_rmap->getRegionMapItem(-1,plane,section).getTotMod()-1;
  if(FTKSetup::getFTKSetup().getIBLMode()>=1) module_max+=3;
  for(int test_module=module_max;test_module>=0;test_module--) {
     int phiwidth = m_ssm[plane][section][test_module].m_phiwidth;
     int phiss = m_ssm[plane][section][test_module].m_phiss;
     if((phiss>0)&&(phiwidth>0)) {
        int test_localX=
           SSid_x*phiss-m_ssm[plane][section][test_module].m_phioffset;
        if((test_localX>=0)&&(test_localX<phiwidth)) {
           phimod=test_module;
           localX=test_localX;
           nSolution_phi++;
        }
     }
  }

  if(nSolution_phi!=1) {
     cout<<"SSid_x="<<SSid_x<<" SSid_y="<<SSid_y<<" section="<<section<<"\n";
     for(int test_module=module_max;test_module>=0;test_module--) {
        int phiwidth = m_ssm[plane][section][test_module].m_phiwidth;
        int phiss = m_ssm[plane][section][test_module].m_phiss;
        cout<<" module: "<<test_module<<" phiwidth: "<<phiwidth<<" phiss: "<<phiss<<"\n";
        if((phiss>0)&&(phiwidth>0)) {
           int test_localX=
              SSid_x*phiss-m_ssm[plane][section][test_module].m_phioffset;
           cout<<"  test_localX: "<<test_localX<<" phioffset: "<<m_ssm[plane][section][test_module].m_phioffset<<"\n";
        }
     }
     FTKSetup::PrintMessageFmt
        (ftk::sevr,"FTKSSMap::decodeSSxy ssid=%d plane=%d: found %d solutions in phi\n",
         SSid,plane,nSolution_phi);
  }
}


unsigned int
FTKSSMap::gray_code(unsigned int n, int s)
{
  // originally from numerical recipes, chapter 20
  int h;
  unsigned int ans, idiv;
  if( s >= 0 ) {
    return (n ^ (n >>1));
  }
  h = 1;
  ans = n;
  for (;;) {
    ans ^= (idiv=ans >> h);
    if( idiv <=1 || h==16 ) {
      return ans;
    }
    h <<= 1;
  }
  FTKSetup::PrintMessageFmt(ftk::sevr,"gray_code: execution should never reach here");  
}

// method to create hardware LUT of local SSIDs for a given module
std::vector<unsigned int>
FTKSSMap::get_lookup_table(const int plane,const int section, const int id) const
{
  const struct ssm& this_map = getMap(plane,section,id);
  // figure out range of module coordinates to probe.
  //   module type=0=SCT is indicated by the absence of a internal ssoffset
  //          in the ssm structure. (potential bug: if pixel SSs => zero offset?)
  int type = (bool)this_map.m_ssoff_int;
  //   maximum extent of local coordinates. round up to powers of 2 to align lookup.
  int nx = (1<<(int)ceil(log2(this_map.m_phiwidth)));
  int ny = type ? (1<<(int)ceil(log2(this_map.m_etawidth))) : 1;
  // make sequentially increasing table.
  std::vector<unsigned int> result;
  result.reserve(ny*nx);
  unsigned int lut_offset = 0;
  for( int iy=0; iy!=ny; ++iy ) {
    for( int ix=0; ix!=nx; ++ix, ++lut_offset ) {
      unsigned int ssid = 0xFFFF;
      if( type ) {
        // pixel
        if( ix<this_map.m_phiwidth && iy<this_map.m_etawidth ) {
          FTKHit tmp(2);
          tmp[0] = ix;
          tmp[1] = iy;
          tmp.setPlane(plane);
          tmp.setSector(0);
          ssid = compressed_ssid_word_pixel(0,tmp.getPlane(),0,tmp[0],tmp[1],0);
        }
      } else {
        // SCT
        if( ix<this_map.m_phiwidth ) {
          FTKHit tmp(1);
          tmp[0] = ix;
          tmp.setPlane(plane);
          tmp.setSector(0);
          ssid = compressed_ssid_word_strip(0,tmp.getPlane(),0,tmp[0]);
        }
      }
      //cout << " ix=" << ix << " iy=" << iy << " ilut=" << lut_offset << " ssid=" << ssid << endl;
      result.push_back(ssid);
    }
  }
  return result;
}

unsigned int
FTKSSMap::getNumSuperstrips(const int plane, const int section, const int towerid) const
{
  const struct ssm& this_map = getMap(plane,section,towerid);
  // figure out range of module coordinates to probe.
  //   module type=0=SCT is indicated by the absence of a internal ssoffset
  //          in the ssm structure. (potential bug: if pixel SSs => zero offset?)
  int type = (bool)this_map.m_ssoff_int;
  //   maximum extent of local coordinates
  //int nx = this_map.m_phiwidth;
  //int ny = type ? this_map.m_etawidth : 1;
  //cout << " ssmap input address size: pl=" << plane << " sec: " << section << " tower: " << towerid << " nx: " << nx << " ny: " << ny << " size: " << (nx*ny) << endl;
  // get ssid of maximum input strip. can't no funny business in ssid assignments (i.e., sequential ssids) because of the DC bit and gray encoding.
  if( type ) { // pixel
    const unsigned int total_localssid_bits = (m_nbitsssid-m_nbitsssid_module_pixel);
    const unsigned int total_nondc_bits = total_localssid_bits-this_map.m_ndcx-this_map.m_ndcy;
    const unsigned int nondcbits_x = total_nondc_bits - m_eta_nondcbits;
    const unsigned int max_compressed_ssid_etaphi_dc = std::min( 1<<this_map.m_ndcy<<this_map.m_ndcx , m_ssm[plane][section][0].m_ssoff );
    const unsigned int max_nondc_lyx = std::min( 1<<m_eta_nondcbits<<nondcbits_x , m_ssm[plane][section][0].m_ssoff >> this_map.m_ndcx >> this_map.m_ndcy );
    return( max_nondc_lyx*max_compressed_ssid_etaphi_dc );
  } else {
    const unsigned int total_localssid_bits = (m_nbitsssid-m_nbitsssid_module_strip);
    return( std::min( (1<<total_localssid_bits)-1 , m_ssm[plane][section][0].m_ssoff) );
  }
}


unsigned int
FTKSSMap::compressed_ssid_word(const FTKHit &hit, unsigned int localID, bool SecondStage) const
{
  //const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  //int debug = ftkset.getVerbosity();
  if( hit.getDim()==2 ) { return compressed_ssid_word_pixel(hit, localID,SecondStage); }
  else if( hit.getDim()==1 ) { return compressed_ssid_word_strip(hit,localID,SecondStage); }
  else {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Hit with dim %d not supported",hit.getDim());
  }
  return 0ul;
}

unsigned int
FTKSSMap::compressed_ssid_word_pixel(const FTKHit &hit, unsigned int local_module_id, bool SecondStage) const
{
  if(m_useHWCoords){
    // Note: getHWCoord(1) is in units of 6.25um, to get it into SSID units which are 50um divide by 8
    float localX = (hit.getHwCoord(0)>>3);
    
    // Note: getHWCoord(1) is in units of 25um, to get it into SSID units which are 400um divide by 16
    float localY = (hit.getHwCoord(1)>>4);    

    return compressed_ssid_word_pixel
      (local_module_id,hit.getPlane(),hit.getSection(),localX,localY,SecondStage);
  }

  // return compressed_ssid_word_pixel
  //   (local_module_id,hit.getPlane(),hit.getSection(),hit[0],hit[1]);
  return compressed_ssid_word_pixel
    (local_module_id,hit.getPlane(),0,hit[0],hit[1],SecondStage);

}

unsigned int
FTKSSMap::compressed_ssid_word_pixel
(unsigned int local_module_id,int plane,int section,
 float localX,float localY,bool SecondStage) const
{
   int phi_dc_bits = getSSDCX(plane);
   int eta_dc_bits = getSSDCY(plane);

   int nbitsSSID_available = m_nbitsssid;
   if(SecondStage) nbitsSSID_available = m_nbitsssid_SSB;

   const unsigned int total_localssid_bits =
      nbitsSSID_available-m_nbitsssid_module_pixel;
  const unsigned int total_nondc_bits =
     total_localssid_bits-phi_dc_bits-eta_dc_bits;
  const unsigned int nondcbits_x =
     std::min((unsigned int)
              ceil(log(getSSOffsetInternal(plane,section))/log(2.)),
              total_nondc_bits );
  const unsigned int nondcbits_y = total_nondc_bits - nondcbits_x;

  int x = (int)localX;
  int y = (int)localY;

  // full-precision representation of coordinate / ss size
  int phi_ss_width = m_ssm[plane][section][0].m_phiss;
  int eta_ss_width = m_ssm[plane][section][0].m_etass;

  if((x<0.)||(x>=m_ssm[plane][section][0].m_phiwidth)||
     (y<0.)||(y>=m_ssm[plane][section][0].m_etawidth)) {
#ifdef TRACEBACK_ON_BOUNDARY_VIOLATION
     void *buffer[100];int nptrs = backtrace(buffer, 100);
     backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
#endif
     cout<<"Error FTKSSMap::compressed_ssid_word_pixel bad coordinates plane="<<plane
         <<" localmodule="<<local_module_id
          <<" x="<<x<<" range [0;"<<m_ssm[plane][section][0].m_phiwidth<<"]"
          <<" y="<<y<<" range [0;"<<m_ssm[plane][section][0].m_etawidth<<"]\n";
      throw FTKException("compressed_ssid_word_pixel:  coordinate out of range");

  }
  x /= phi_ss_width; // (12-3=9 bits / size)
  y /= eta_ss_width; // (11-3=8 bits / size)
  // dc pieces
  unsigned int dc_x = x & (phi_dc_bits>0 ? (1<<phi_dc_bits)-1 : 0 );
  unsigned int dc_y = y & (eta_dc_bits>0 ? (1<<eta_dc_bits)-1 : 0 );
  // Alberto Annovi: gray code to be enabled below
  dc_x ^= dc_x>>1;
  dc_y ^= dc_y>>1;
  // unsigned int dc = (dc_y << phi_dc_bits) | dc_x;
  // non-dc piece. current compression scheme just drops higher order bits.
  // relative to uncompressed superstrip ids, this effectively
  // multiplexes the superstrips. the ideal choice of multiplexed
  // ssids will lead to the fewest false positive patterns.
  unsigned int nondc_x = (x >> phi_dc_bits) & ((1<<nondcbits_x)-1);
  unsigned int nondc_y = (y >> eta_dc_bits) & ((1<<nondcbits_y)-1);
  unsigned int nondc = (nondc_y << nondcbits_x) | nondc_x;
  if  ((nondc & ((1<<total_nondc_bits)-1)) != nondc )
		throw FTKException("compressed_ssid_word_pixel: Error in the DC configuration (1)");
  // local ssid
  const unsigned int max_compressed_ssid_phi_dc =
     std::min( 1<<phi_dc_bits , getSSOffsetInternal(plane,section) );
  const unsigned int max_compressed_ssid_etaphi_dc =
     std::min( 1<<eta_dc_bits<<phi_dc_bits , getSSOffset(plane,section) );
  const unsigned int max_nondc_lx =
     std::min( 1<<nondcbits_x,
               getSSOffsetInternal(plane,section) >> phi_dc_bits);
  const unsigned int max_nondc_lyx =
     std::min( 1<<total_nondc_bits,
               getSSOffset(plane,section) >> phi_dc_bits >> eta_dc_bits );
  const unsigned int p3 = max_nondc_lx*max_compressed_ssid_etaphi_dc*nondc_y;
  const unsigned int p2 = max_compressed_ssid_etaphi_dc*nondc_x;
  const unsigned int p1 = max_compressed_ssid_phi_dc*dc_y;
  const unsigned int p0 = dc_x;
  // Alberto Annovi 2014-08-08: gray code moved up and used only for dc_x and dc_y
  //  // Gray code the entire piece with the local SSID, per discussion with Guido 11 March 2014)
  //  unsigned int local_ssid = gray_code(p3 + p2 + p1 + p0,1);
  unsigned int local_ssid = p3 + p2 + p1 + p0;
  // compute ssid in global ss address space. in the absence of dc bits or
  // compression, the calculation would be just
  //    const unsigned int result = m_max_etaphi*localID + y*m_max_phi + x;
  // but now we want to make sure that the dc bits are the lowest order bits,
  // and that the address space offsets correspond to the compressed widths
  // and not the uncompressed space.
  // coordinate transformation. nondc bits address blocks of dcbit space. all dcbits address subdimensions in a space that can be ignored.
  unsigned int total_offset = max_nondc_lyx*max_compressed_ssid_etaphi_dc;
  total_offset = (int)ceil(total_offset/(1.*(1<<(phi_dc_bits+eta_dc_bits))))*(1<<(phi_dc_bits+eta_dc_bits));
  const unsigned int global_ssid = total_offset*local_module_id + local_ssid;
  const unsigned int result = global_ssid;
  const unsigned int result_mask = (1<<m_nbitsssid)-1;
  if ( (result & result_mask) != result ) {
     std::cout<<"local id="<<local_module_id
              <<" plane="<<plane
              <<" section="<<section
              <<" localX="<<localX
              <<" localY="<<localY<<"\n";
     std::cout<<"total_offset="<<total_offset
              <<" local_module_id="<<local_module_id
              <<" local_ssid="<<local_ssid<<"\n";
     std::cout<<" result="<<result<<" [0x"<<std::setbase(16)<<result
              <<"] mask=0x"<<result_mask<<std::setbase(10)
              <<" nbit="<<m_nbitsssid<<"\n";
     throw FTKException("compressed_ssid_word_pixel: Error in the DC configuration (2)");
  }
  return result;
}


unsigned int
FTKSSMap::compressed_ssid_word_strip(const FTKHit &hit, unsigned int local_module_id,bool SecondStage) const {

  if(m_useHWCoords){

    // Note: getHWCoord(1) is in units of 1/2 strip, to get it into SSID units which are strip divide by 2
    float localX = (hit.getHwCoord(0)>>1);

    return compressed_ssid_word_strip
      (local_module_id,hit.getPlane(),hit.getSection(),localX,SecondStage);
  }

  // return compressed_ssid_word_strip
  //   (local_module_id,hit.getPlane(),hit.getSection(),hit[0]);
  return compressed_ssid_word_strip
    (local_module_id,hit.getPlane(),0,hit[0]);
}

unsigned int
FTKSSMap::compressed_ssid_word_strip(int localID,int plane,int section,
                                     float localX,bool SecondStage) const
{
   int phi_dc_bits = getSSDCX(plane);

   int nbitsSSID_available = m_nbitsssid;
   if(SecondStage) nbitsSSID_available = m_nbitsssid_SSB;

  const unsigned int total_localssid_bits =
     nbitsSSID_available-m_nbitsssid_module_strip;
  const unsigned int total_nondc_bits = total_localssid_bits-phi_dc_bits;
  const unsigned int nondcbits_x = total_nondc_bits;
  // full-precision representation of coordinate / ss size (11-1=10 bits / size)
  int x = (int)localX;

  if((x<0.)||(x>=m_ssm[plane][section][0].m_phiwidth)) {
#ifdef TRACEBACK_ON_BOUNDARY_VIOLATION
     void *buffer[100];int nptrs = backtrace(buffer, 100);
     backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
#endif
     cout<<"Error FTKSSMap::compressed_ssid_word_strip bad coordinates plane="<<plane
         <<" localmodule="<<localID
         <<" x="<<x<<" range [0;"<<m_ssm[plane][section][0].m_phiwidth<<"]\n";
      throw FTKException("compressed_ssid_word_strip: coordinate out of range");
  }


  int phi_ss_width = m_ssm[plane][section][0].m_phiss;
  x /= phi_ss_width;
  // dc piece.
  unsigned int dc_x = x & ((1<<phi_dc_bits)-1);
  // gray-code DC bits
  dc_x ^= dc_x>>1;
  // check for SSID strip word logic error when masking bits
  assert( (dc_x & ((1<<phi_dc_bits)-1)) == dc_x );
  // non-dc piece. current compression scheme just drops higher order bits.
  // relative to uncompressed superstrip ids, this effectively
  // multiplexes superstrips. the ideal choice of multiplexed
  // ssids will lead to the fewest false positive patterns.
  // the implicit choice here is probably not ideal.
  unsigned int nondc_x = (x >> phi_dc_bits) & ((1<<nondcbits_x)-1);
  // check for SSID strip word logic error when masking bits
  assert( (nondc_x & ((1<<total_nondc_bits)-1)) == nondc_x );
  // local ssid (Gray code the local piece, per discussion with Guido 11 March 2014)
  //unsigned int local_ssid = gray_code((nondc_x << phi_dc_bits) | (dc_x),1);
  unsigned int local_ssid = (nondc_x << phi_dc_bits) | dc_x;
  assert( (local_ssid & ((1<<total_localssid_bits)-1)) == local_ssid );
  // convert to global ssid
  unsigned int max_compressed_ssid =
     std::min((1<<total_localssid_bits)-1,getSSOffset(plane,section));
  // the above needs to be a multiple of 2^(total dc bits)
  max_compressed_ssid = (int)ceil(max_compressed_ssid/(1.*(1<<phi_dc_bits)))*(1<<phi_dc_bits);
  const unsigned int ssPerModule = max_compressed_ssid;
  const unsigned int offset = (localID*ssPerModule) & ((1<<m_nbitsssid)-1);
  const unsigned int result = offset + local_ssid;
  assert( (result & ((1<<m_nbitsssid)-1)) == result );
  return result;
}

