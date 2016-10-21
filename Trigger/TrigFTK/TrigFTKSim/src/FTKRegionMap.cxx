/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRegionMap.h"


#include <cstdlib>
#include <string>
#include <cstdio>
#include <iostream>
#include <boost/foreach.hpp>
#include <TTree.h>
#include <TKey.h>

// #define IGNORE_LUT_INCONSISTENCY 

using namespace std;
using namespace ftk;

FTKRegionMapItem::FTKRegionMapItem() :
  m_phi_min(0), m_phi_max(0), m_phi_tot(0),
  m_eta_min(0), m_eta_max(0), m_eta_tot(0)
{
  // nothing to do here
}


FTKRegionMap::FTKRegionMap() :
   m_isok(false), m_old_format(false), m_pmap(NULL), m_m_pmap(NULL),
   m_nregions(0), m_nregions_phi(0),
   m_nplanes(0), m_sections(0), m_map(0)
{
  // nothing to do
}

const TString FTKRegionMap::s_dirname = "RegionMap";
FTKRegionMap::~FTKRegionMap()
{
   if (m_sections) delete m_sections;
   if (m_map) delete [] m_map;
   if (m_m_pmap && m_m_pmap==m_pmap) {
      delete m_pmap;
      m_m_pmap = NULL;
      m_pmap = NULL;
   }
}


/** create a regions' map file from an ASCII file describing it */
FTKRegionMap::FTKRegionMap(FTKPlaneMap *pmap, const char *path) :
  m_path(path), m_isok(false), m_old_format(false), m_pmap(pmap), m_nregions(0), m_nregions_phi(0),
  m_nplanes(0), m_sections(0x0), m_map(0x0)
{
  m_m_pmap = NULL;
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  FILE *infile = fopen(path,"r");
  if (!infile) {
    cerr << "*** error reading rmap file: " << path << endl;
    return;
  }

  // cmDebug was a variable used in the old version, here
  // redfined as a boolean true if the FTK verbosity level 
  // is greater than 1
  bool cmDebug = ftkset.getVerbosity()>1 ? true : false;

  // most of the the code from original TrigFTKLib/src/lib/rmap_rd.c file
  char line[100];
  int i,j,region;
  int type,isEC,layer;
  int phi_min,phi_max,phi_tot,eta_min,eta_max,eta_tot;
  int plane,section;

  // number of the regions in the first line of the file
  fgets(line, 100, infile);
  // figure out if this is old or new format:
  sscanf(line, "towers %d phi %d", &m_nregions,&m_nregions_phi);
  if(m_nregions==0) {
    sscanf(line, "regions %d", &m_nregions);
    m_nregions_phi = m_nregions;
    if(m_nregions==0) {
      sscanf(line, "slices %d phi %d", &m_nregions, &m_nregions_phi);
      if(m_nregions==0) {
	FTKSetup::PrintMessage(sevr, "readRegionMap: Failed to load rmap file in either region or tower format");
      }
    }
    else m_old_format = true;
  }

  printf("readRegionMap (old_format=%d) - %d regions / %d planes\n",
	 m_old_format, m_nregions, m_pmap->getNPlanes());

  if(!(m_map = new FTKRegionMapItem**[m_nregions]))
    FTKSetup::PrintMessage(sevr, "readRegionMap: cannot allocate map elm region space");

  for(i = 0; i < m_nregions; ++i) {
    if(!(m_map[i] = new FTKRegionMapItem*[m_pmap->getNPlanes()]))
      FTKSetup::PrintMessage(sevr, "readRegionMap: cannot allocate map elm space");

    /* nsections per plane number i */
    for(j = 0; j < m_pmap->getNPlanes(); ++j) 
      if(!(m_map[i][j] = new FTKRegionMapItem[m_pmap->getNSections(j)]))
	FTKSetup::PrintMessage(sevr, "readRegionMap: cannot allocate map elm space");
  }

  if (cmDebug) FTKSetup::PrintMessage(debg,"Reading file now\n");

  for(i = 0; i < m_nregions; ++i) { // region loop
    fgets(line, 100, infile); /* skip a line */
    fgets(line, 100, infile); /* check region number */
    sscanf(line, "%d", &region);
    if(region != i) // verify the regions are read in sequence
      FTKSetup::PrintMessage(sevr, "readRegionMap: syntax error in rmap file");
    for(j = 0; j < m_pmap->getRLayers(); ++j) { // loop over the real layers
      // cannot be here for bool

      if( ftkset.getITkMode() && j > 7 ) {
	FTKSetup::PrintMessage(warn, "ITkMode currently only set up to process first 8 layers in tmap");
	break;
      }
      
      // if (cmDebug > 3) 
      //    printf("i = %d, j = %d, m_pmap->rlayers = %d\n",i,j,m_pmap->getRLayers());
      fgets(line,100,infile); // reading the line

      if (cmDebug) FTKSetup::PrintMessageFmt(debg,"read %s\n",line);

      // initialize variables not mandatory in all the formats supported by this routine
      isEC = 0; phi_tot=-1; eta_min=-1; eta_max=-1; eta_tot=-1;
      if (m_old_format) {
         const int nread = sscanf(line, "%d %d %d %d %d %d", &type, &isEC, &layer, &phi_min, &phi_max, &phi_tot);
         // compatibility: the old rmap file gives max_phi. Since numbering starts with 0, we need to increment by one
         if (nread!=6) phi_tot=phi_max+1; else phi_tot+=1;
      } else {
         sscanf(line, "%d %d %d %d %d %d %d %d %d", &type, &isEC, &layer, &phi_min, &phi_max, &phi_tot, &eta_min, &eta_max, &eta_tot);
      }
      
      if (isEC && m_pmap->getRLayers() == m_pmap->getNPlanes()) {
         FTKSetup::PrintMessage(sevr, "readRegionMap: New rmap file format but old m_pmap format\n");
      }

      // if the physical layer is used by the current pmap the information is read
      if((plane = m_pmap->getMap(type,isEC,layer).getPlane()) > -1) {
         section = m_pmap->getMap(type,isEC,layer).getSection();
         m_map[i][plane][section].setPhiMin(phi_min);
         m_map[i][plane][section].setPhiMax(phi_max);
         m_map[i][plane][section].setPhiTot(phi_tot);
         m_map[i][plane][section].setEtaMin(eta_min);
         m_map[i][plane][section].setEtaMax(eta_max);
         m_map[i][plane][section].setEtaTot(eta_tot);

         /* if((phi_min>phi_max)||(eta_min>eta_max)) {
            printf("FTKRegionMap::FTKRegionMap problem type=%d isEC=%d layer=%d phimin=%d phimax=%d phitot=%d etamin=%d etamax=%d eta_tot=%d\n",
                   type,isEC,layer, phi_min, phi_max, phi_tot,
                   eta_min,eta_min,eta_tot);
                   } */
         // Cannot be here for bool
         // if(cmDebug>3)  printf("map[reg=%d,plane=%d,sec=%d] = (%d/%d/%d:%d/%d/%d)\n",
         // 		      i,plane,section,phi_min,phi_max,phi_tot,eta_min,eta_max,eta_tot);
      }
    } // end loop over the real layers
     //    if(cmDebug>3) printf("\n");
  } // end region loop
  if (cmDebug) FTKSetup::PrintMessage(debg,"\t\t...done\n");
  

  m_nplanes = m_pmap->getNPlanes();

  if ((m_sections = (int *)calloc(ftk::MAXPLANES,sizeof(int))) == NULL)
    FTKSetup::PrintMessage(sevr,"m_pmap_rd: Allocation of rmap sections failed.");
  for (j = 0; j < ftk::MAXPLANES; j++) m_sections[j] = m_pmap->getNSections(j);

  if (cmDebug) FTKSetup::PrintMessage(debg,"Done with readRegionMap\n\n");
  fclose(infile);
  
  m_isok = true;
}

FTKRegionMap::FTKRegionMap(TDirectory* file){
   //! Read pmap class from root file
   //! check validity also using GetIsOK()

   //cout<<"Instantiate RegionMap from root file."<<endl;
   TDirectory* thisdir = gDirectory;

   m_isok = false;

   if ( !file->cd(s_dirname) ) {
      cerr << "Error in FTKRegionMap::FTKRegionMap. No RegionMap found in root-file. "<< endl;
      return;
   }

   m_path = gDirectory->GetTitle();
   //cout<<"PathName:        "<<m_path<<endl;

   TTree* tConst        = (TTree*)gDirectory->Get("Constants");
   TTree* tMap          = (TTree*)gDirectory->Get("Map");
   TTree* tTowLoc       = (TTree*)gDirectory->Get("TowerGlobalToLocalMap");
   TTree* tGlobToOffMap = (TTree*)gDirectory->Get("GlobalToOffMap");
   
   if ( !tConst || !tMap ) {
      cerr << "Error in FTKRegionMap::FTKRegionMap. Could not find trees. "<< endl;
      cerr << "  tree('Constants')     "<<tConst<<endl;
      cerr << "  tree('Map')          "<<tMap<<endl;
      m_old_format = false;
      m_pmap = NULL;
      m_m_pmap = NULL;
      m_nregions = 0;
      m_nregions_phi = 0;
      m_nplanes = 0;
      m_sections = 0;
      m_map = 0;
      return;
   }

   m_isok = true;

   // constants:
   int chksum, chksumLUT;
   tConst->GetBranch("old_format")->SetAddress(&m_old_format);
   tConst->GetBranch("nregions")->SetAddress(&m_nregions);
   tConst->GetBranch("nregions_phi")->SetAddress(&m_nregions_phi);
   tConst->GetBranch("nplanes")->SetAddress(&m_nplanes);
   tConst->GetBranch("Checksum")->SetAddress(&chksum);
   tConst->GetBranch("ChecksumLUT")->SetAddress(&chksumLUT);
   tConst->GetEntry(0);
   // cout<<"old_format       "<<m_old_format<<endl;   
   // cout<<"nregions         "<<m_nregions<<endl;
   // cout<<"nregions_phi     "<<m_nregions_phi<<endl;
   // cout<<"nplanes          "<<m_nplanes<<endl;
   
   // PlaneMap
   m_pmap = FTKPlaneMap::GetPMapFromRootFile(file);
   m_m_pmap = m_pmap;
   if ( !m_pmap->getIsOK() ) {
      cerr<<"Error in FTKRegionMap::FTKRegionMap. Could not get valid plane map from file."<<endl;
      m_isok = false;
      return;
   }

   m_sections = new int[ftk::MAXPLANES];
   for ( int i = 0 ; i<ftk::MAXPLANES ; i++ ) m_sections[i] = m_pmap->getNSections(i);


   // FTKRegionMapItem***            m_map;
   double phi_min, phi_max, phi_tot, eta_min, eta_max, eta_tot;
   int ireg,iplane,isec;
   tMap->GetBranch("ireg")->SetAddress(&ireg);
   tMap->GetBranch("iplane")->SetAddress(&iplane);
   tMap->GetBranch("isec")->SetAddress(&isec);
   tMap->GetBranch("phi_min")->SetAddress(&phi_min);
   tMap->GetBranch("phi_max")->SetAddress(&phi_max);
   tMap->GetBranch("phi_tot")->SetAddress(&phi_tot);
   tMap->GetBranch("eta_min")->SetAddress(&eta_min);
   tMap->GetBranch("eta_max")->SetAddress(&eta_max);
   tMap->GetBranch("eta_tot")->SetAddress(&eta_tot);
   m_map = new FTKRegionMapItem**[m_nregions];
   int nev=0;
   for(int i = 0; i < m_nregions; ++i) {
      tMap->GetEntry(nev++);
      m_map[i] = new FTKRegionMapItem*[m_pmap->getNPlanes()];
      for(int j = 0; j < m_pmap->getNPlanes(); ++j) {
	 if ( j!= 0 ) tMap->GetEntry(nev++);
	 m_map[i][j] = new FTKRegionMapItem[m_pmap->getNSections(j)];
	 for(int n = 0; n<m_pmap->getNSections(j) ; n++ ) {
	    if ( n!= 0 ) tMap->GetEntry(nev++);
	    if ( i!= ireg || j!= iplane || n !=isec ) {
	       cerr << "Error reading FTKRegionMap."<<endl;
	       m_isok = false;
	    }
	    m_map[i][j][n] = FTKRegionMapItem(phi_min, phi_max, phi_tot, eta_min, eta_max, eta_tot);
	 }
      }
   }


   // lookup table for local module ID within a tower:
   // init: tower_global_to_local_map_type m_tower_global_to_local_map;
   if ( tTowLoc ) {
      ULong64_t imtowerid,implane,imglobalid,localid;
      tTowLoc->GetBranch("itowerid")->SetAddress(&imtowerid);
      tTowLoc->GetBranch("iplane")->SetAddress(&implane);
      tTowLoc->GetBranch("iglobalid")->SetAddress(&imglobalid);
      tTowLoc->GetBranch("localid")->SetAddress(&localid);
      for ( int n = 0 ; n<tTowLoc->GetEntries() ; n++ ) {
	 tTowLoc->GetEntry(n);
	 m_tower_global_to_local_map[imtowerid][implane][imglobalid] = localid;
      }
   }

   // init: global_to_offline_map_type m_global_to_offline_map;
   // conversion between global module IDs and offline
   if ( tGlobToOffMap ) {
      ULong64_t ig;
      bool isPixel;
      int barrelEC, layer, etamod, phimod;
      tGlobToOffMap->GetBranch("iglobtooffmap")->SetAddress(&ig);
      tGlobToOffMap->GetBranch("isPixel")->SetAddress(&isPixel);
      tGlobToOffMap->GetBranch("barrelEC")->SetAddress(&barrelEC);
      tGlobToOffMap->GetBranch("layer")->SetAddress(&layer);
      tGlobToOffMap->GetBranch("etamod")->SetAddress(&etamod);
      tGlobToOffMap->GetBranch("phimod")->SetAddress(&phimod);
      for ( int n = 0 ; n<tGlobToOffMap->GetEntries() ; n++ ) {
	 tGlobToOffMap->GetEntry(n);
	 m_global_to_offline_map[ig] = FTKRegionMapOfflineId(isPixel, barrelEC, layer, etamod, phimod);
      }
   }
      
   // m_path = gDirectory->GetTitle();
   // cout<<"PathName:        "<<m_path<<endl;

   m_isok = m_isok && m_pmap->getIsOK();
   thisdir->cd(); // go back

   if ( chksum != CalcChecksum() ) {
      cerr<<"FTKRegionMap::FTKRegionMap. Error. Checksum after reading is not consistent with Checksum stored in file."<<endl;
      m_isok=false;
   }
   if ( chksumLUT != CalcChecksumLUT() ) {
      cout<<"FTKRegionMap::FTKRegionMap. Warning. ChecksumLUT after reading ("<< CalcChecksumLUT()<<") is not consistent with Checksum stored in file ("<<chksumLUT<<")."<<endl;
#ifndef IGNORE_LUT_INCONSISTENCY 
      m_isok=false;
#endif
   }

}



FTKRegionMap* FTKRegionMap::GetRMapFromRootFile(TDirectory* file){
   //! Read regionmap class from root file
   //! return NULL if reading failed.
   //! check validity also using m_isok

   TDirectory* thisdir = gDirectory;
   FTKRegionMap* ret = new FTKRegionMap(file);

   if ( !ret->getIsOK() ) {
      cerr <<"FTKRegionMap::GetRMapFromRootFile. Error. RegionMap seems not to be ok (isok==false). Returning nullptr."<<endl;
      delete ret;
      return NULL;
   }

   thisdir->cd(); // go back
   return  ret;
}


void FTKRegionMap::WriteMapToRootFile(TDirectory* dir){
   //! Write RegionMap to root file
   //! Readable again using FTKRegionMap(TDirectory* dir).

   TDirectory* thisdir = gDirectory;
   // check if RegionMap already exists
   if ( dir->GetKey(s_dirname) ) {
      if ( FTKRegionMap::GetChecksum(dir) == CalcChecksum()  ) {
	 cout<<"Info. Identical RegionMap already written to disk. Skipping writing. (checksum: "<<CalcChecksum()<<")."<<endl;
	 dir->ls();
	 return;
      }
      else {
	 cerr << "Error: [FTKRegionMap::WriteMapToRootFile] There is already a RegionMap in this file: " << dir->GetTitle() << endl;
	 cerr << "                                          RegionMap title: "<<dir->GetKey(s_dirname)->GetTitle()<<endl;
	 cerr << "       *** Skipping writing *** "<<endl;
	 return;
      }
   }
   if ( !m_isok ) {
      cerr << "Error: [FTKRegionMap::WriteMapToRootFile] RegionMap is not properly initalized. m_isok= " << m_isok << endl;
      cerr << "       *** Skipping writing *** "<<endl;
      return;
   }

   // create new directory for RegionMap content
   // std::string m_path; // configuration file path
   TString title = getPath();
   TObjArray* as = title.Tokenize("/");
   TObjString* asend = (TObjString*)as->At(as->GetEntries()-1);
   title = asend->GetString();
   delete as;
   TDirectory* rmapdir = dir->mkdir(s_dirname,title);
   rmapdir->cd();

   //cout<<"Writing constants."<<endl;
   // bool m_old_format; // old_format = 8 regions; otherwise 64 towers
   // int m_nregions;
   // int m_nregions_phi;
   // int m_nplanes;
   TTree tConst("Constants","Constants");
   tConst.Branch("old_format",&m_old_format,"old_format/O");
   tConst.Branch("nregions",&m_nregions,"nregions/I");
   tConst.Branch("nregions_phi",&m_nregions_phi,"nregions_phi/I");
   tConst.Branch("nplanes",&m_nplanes,"nplanes/I");
   int chksum = CalcChecksum();
   int chksumLUT = CalcChecksumLUT();
   tConst.Branch("Checksum",&chksum,"chksum/I");
   tConst.Branch("ChecksumLUT",&chksumLUT,"chksumLUT/I");
   tConst.Fill();

   // FTKPlaneMap *m_pmap;
   //cout<<"writing pmap file."<<endl;
   m_pmap->WriteMapToRootFile(dir);
   //m_pmap->WriteMapToRootFile(thisdir);

   // int *m_sections;
   //for (j = 0; j < MAXPLANES; j++) m_sections[j] = m_pmap->getNSections(j);
   // -> Not necessary to write, since anyhow taken from pmap

   //cout<<"writing m_map."<<endl;
   // FTKRegionMapItem***            m_map;
   double phi_min, phi_max, phi_tot, eta_min, eta_max, eta_tot;
   //int ireg,iplane,isec;
   TTree tMap("Map","Map");
   int i,j,n;
   tMap.Branch("ireg",&i,"ireg/I");
   tMap.Branch("iplane",&j,"iplane/I");
   tMap.Branch("isec",&n,"isec/I");
   tMap.Branch("phi_min",&phi_min,"phi_min/D");
   tMap.Branch("phi_max",&phi_max,"phi_max/D");
   tMap.Branch("phi_tot",&phi_tot,"phi_tot/D");
   tMap.Branch("eta_min",&eta_min,"eta_min/D");
   tMap.Branch("eta_max",&eta_max,"eta_max/D");
   tMap.Branch("eta_tot",&eta_tot,"eta_tot/D");
   for(i = 0; i < m_nregions; ++i) {
      for(j = 0; j < m_pmap->getNPlanes(); ++j) {
	 for(n = 0; n<m_pmap->getNSections(j) ; n++ ) {
	    phi_min = m_map[i][j][n].getPhiMin();	    
	    phi_max = m_map[i][j][n].getPhiMax();
	    phi_tot = m_map[i][j][n].getPhiTot();
	    eta_min = m_map[i][j][n].getEtaMin();
	    eta_max = m_map[i][j][n].getEtaMax();
	    eta_tot = m_map[i][j][n].getEtaTot();
	    //  std::set<unsigned int> m_global_module_ids;
	    tMap.Fill();
	 }
      }
   }
   
   // typedef std::map<unsigned int,unsigned int> global_to_local_map_type;
   // typedef std::map<unsigned int,global_to_local_map_type> plane_global_to_local_map_type;
   // typedef std::map<unsigned int, plane_global_to_local_map_type > tower_global_to_local_map_type;
   // tower_global_to_local_map_type m_tower_global_to_local_map;

   TTree* tTowLoc=NULL;
   TTree* tGlobToOffMap=NULL;
   
   if ( m_tower_global_to_local_map.empty() ) 
      cout<<"Info. FTKRegionMap::WriteMapToRootFile. m_tower_global_to_local_map is empty. Nothing to write."<<endl;
   else {
      ULong64_t imtowerid,implane,imglobalid,localid;
      //unsigned int itowerid,iplane,iglobalid,localid;
      tTowLoc = new TTree("TowerGlobalToLocalMap","TowerGlobalToLocalMap");
      tTowLoc->Branch("itowerid",&imtowerid,"itowerid/l");
      tTowLoc->Branch("iplane",&implane,"iplane/l");
      tTowLoc->Branch("iglobalid",&imglobalid,"iglobalid/l");
      tTowLoc->Branch("localid",&localid,"localid/l");
      //- l : a 64 bit unsigned integer (ULong64_t)
      //m_tower_global_to_local_map[towerid][plane][globalid] = localid;
      for ( auto atower : m_tower_global_to_local_map ) {
	 imtowerid = atower.first;
	 //plane_global_to_local_map_type& imap = atower.second;
	 for ( auto aplane : m_tower_global_to_local_map[imtowerid] ) {
	    implane = aplane.first;
	    for ( auto aglob : m_tower_global_to_local_map[imtowerid][implane] ) {
	       imglobalid = aglob.first;
	       localid = aglob.second;
	       //cout<<"Writing m_tower_global_to_local_map:   imtowerid="<<imtowerid<<"\timplane="<<implane<<"\timglobalid="<<imglobalid<<"\tlocalid="<<localid<<endl;
	       tTowLoc->Fill();
	    }
	 }
      }
   }
   
   // typedef std::map<unsigned int,FTKRegionMapOfflineId> global_to_offline_map_type;
   // global_to_offline_map_type     m_global_to_offline_map;
   if ( m_global_to_offline_map.empty() ) 
      cout<<"Info. FTKRegionMap::WriteMapToRootFile. m_global_to_offline_map is empty. Nothing to write."<<endl;
   else {
      tGlobToOffMap = new TTree("GlobalToOffMap","GlobalToOffMap");
      ULong64_t ig;
      bool isPixel;
      int barrelEC, layer, etamod, phimod;
      tGlobToOffMap->Branch("iglobtooffmap",&ig,"iglobtooffmap/l");
      tGlobToOffMap->Branch("isPixel",&isPixel,"isPixel/O");
      tGlobToOffMap->Branch("barrelEC",&barrelEC,"barrelEC/I");
      tGlobToOffMap->Branch("layer",&layer,"layer/I");
      tGlobToOffMap->Branch("etamod",&etamod,"etamod/I");
      tGlobToOffMap->Branch("phimod",&phimod,"phimod/I");
      for ( auto agltooff : m_global_to_offline_map ) {
	 ig = agltooff.first;
	 isPixel = agltooff.second.getIsPixel();
	 barrelEC = agltooff.second.getBarrelEC();
	 layer = agltooff.second.getLayer();
	 etamod = agltooff.second.getEtaMod();
	 phimod = agltooff.second.getPhiMod();
	 tGlobToOffMap->Fill();
      }
   }   

   rmapdir->Write();
   thisdir->cd();
   if ( tTowLoc )  delete tTowLoc;
   if ( tGlobToOffMap )  delete tGlobToOffMap;
   // bool m_isok;
}


int FTKRegionMap::CalcChecksum() const {
   // calculate checksum of map

   vector<double> vec;
   vec.push_back((double)m_old_format);
   vec.push_back((double)m_nregions);
   vec.push_back((double)m_nregions_phi);
   vec.push_back((double)m_nplanes);
   
   // m_pmap;
   int pmapchk = m_pmap->CalcChecksum();
   vec.push_back((double)pmapchk);
   
   // m_map;
   for(int i = 0; i < m_nregions; ++i) {
      for(int j = 0; j < m_pmap->getNPlanes(); ++j) {
	 for(int n = 0; n<m_pmap->getNSections(j) ; n++ ) {
	    vec.push_back((double)m_map[i][j][n].getPhiMin());
	    vec.push_back((double)m_map[i][j][n].getPhiMax());
	    vec.push_back((double)m_map[i][j][n].getPhiTot());
	    vec.push_back((double)m_map[i][j][n].getEtaMin());
	    vec.push_back((double)m_map[i][j][n].getEtaMax());
	    vec.push_back((double)m_map[i][j][n].getEtaTot());
	 }
      }
   }

   size_t size = sizeof(double) * vec.size();
   int checksum = ftk::adler32chksum(&vec[0],size);
   //cerr<<"\t\t\tRMAP::Checksum:  "<<checksum<<",\t pmap.checksum: "<<pmapchk<<endl;
   return checksum;
}

int FTKRegionMap::CalcChecksumLUT() {
   // calculate checksum of map

   vector<double> vec;
   if ( m_tower_global_to_local_map.empty() && m_global_to_offline_map.empty()) 
      // No LUT defined yet.
      return 0;
   else {
      ULong64_t imtowerid,implane;
      //ULong64_t imtowerid,implane,imglobalid,localid;
      //- l : a 64 bit unsigned integer (ULong64_t)
      //m_tower_global_to_local_map[towerid][plane][globalid] = localid;
      for ( const auto& atower : m_tower_global_to_local_map ) {
	 imtowerid = atower.first;	 
	 vec.push_back((double) imtowerid);
	 //plane_global_to_local_map_type& imap = atower.second;
	 for ( const auto& aplane : m_tower_global_to_local_map[imtowerid] ) {
	    implane = aplane.first;
	    vec.push_back((double)implane);
	    for ( const auto& aglob : m_tower_global_to_local_map[imtowerid][implane] ) {
	       vec.push_back((double)aglob.first);
	       vec.push_back((double)aglob.second);
	    }
	 }
      }
   
      // typedef std::map<unsigned int,FTKRegionMapOfflineId> global_to_offline_map_type;
      // global_to_offline_map_type     m_global_to_offline_map;
      // ULong64_t ig;
      // bool isPixel;
      // int barrelEC, layer, etamod, phimod;
      for ( const auto& agltooff : m_global_to_offline_map ) {
	 vec.push_back((double)agltooff.first);
	 vec.push_back((double)agltooff.second.getIsPixel());
	 vec.push_back((double)agltooff.second.getBarrelEC());
	 vec.push_back((double)agltooff.second.getLayer());
	 vec.push_back((double)agltooff.second.getEtaMod());
	 vec.push_back((double)agltooff.second.getPhiMod());
      }
   }   

   size_t size = sizeof(double) * vec.size();
   int checksum = ftk::adler32chksum(&vec[0],size);
   return checksum;
}


int FTKRegionMap::GetChecksum(TDirectory* file){
   // get nplanes from root file
   // if error: return -1
   TDirectory* thisdir = gDirectory;

   if ( !file->cd(s_dirname) ) {
      cerr << "Error in FTKRegionMap::GetNPlanes. No RegionMap found in root-file. "<< endl;
	 return -1;
   } 

   TTree* tConst = (TTree*)gDirectory->Get("Constants");
   if ( !tConst ) {
      cerr << "Error in FTKRegionMap::GetNPlanes. Could not find trees. "<< endl;
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


int FTKRegionMap::GetChecksumLUT(TDirectory* file){
   // get nplanes from root file
   // if error: return -1
   TDirectory* thisdir = gDirectory;

   if ( !file->cd(s_dirname) ) {
      cerr << "Error in FTKRegionMap::GetNPlanes. No RegionMap found in root-file. "<< endl;
	 return -1;
   } 

   TTree* tConst = (TTree*)gDirectory->Get("Constants");
   if ( !tConst ) {
      cerr << "Error in FTKRegionMap::GetNPlanes. Could not find trees. "<< endl;
      cerr << "  tree('Constants')       "<<tConst<<endl;
	 return -1;
   }

   // constants:
   int ret;
   TBranch* br = tConst->GetBranch("ChecksumLUT");
   br->SetAddress(&ret);
   br->GetEntry(0);
   thisdir->cd();
   return ret;
}



// read module id LUT (defining global -> tower-local module IDs)
void FTKRegionMap::loadModuleIDLUT(const char* path)
{
  // check that global->offline mapping is non-nil
#if 0
  if( m_global_to_offline_map.empty() ) {
    FTKSetup::PrintMessage(sevr, "load_moduleid_lut: bug: read offline ID lookup table first\n");
  }
#endif

  // read file
  FILE *infile = fopen(path,"r");
  if (!infile) {
    FTKSetup::PrintMessage(sevr, "load_moduleid_lut: error reading module ID lookup table\n");
    return;
  }
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const bool cmDebug = ftkset.getVerbosity()>1 ? true : false;

  char line[256];
  while( fgets(line, 256, infile) ) {
    // format is: <tower id> <plane> <global id> <local id>
    int towerid = -1;
    int plane = -1;
    int globalid = -1;
    int localid = -1;
    sscanf(line,"%d %d %d %d",&towerid,&plane,&globalid,&localid);
    if (plane>=m_pmap->getNPlanes()) {
        // error condition: the module ID map and the PMAP are not consistent
        FTKSetup::PrintMessage(sevr,"Mismatch in the number of logical layer between the PMAP and Module ID map");
    }
    if(cmDebug) {
      printf("line: %s defines tower:%d plane:%d global:%d local:%d\n",line,towerid,plane,globalid,localid);
    }
    if(towerid==-1 || plane==-1 || globalid==-1 || localid==-1) {
      FTKSetup::PrintMessage(sevr, "load_moduleid_lut: error reading module ID lookup table\n");
      continue;
    }
    m_tower_global_to_local_map[towerid][plane][globalid] = localid;
  }
  fclose(infile);
  // assign module ids for each regionitem
  
  //   const int &phimin = m_map[TowerID][plane][section].getPhiMin();
  /// int phimax = m_map[TowerID][plane][section].getPhiMax();

}

bool FTKRegionMap::isModuleIDLUTloaded(void) const {
   return !m_tower_global_to_local_map.empty();
}

// read module id LUT (defining global -> tower-local module IDs)
void FTKRegionMap::load_offline_lut(const char* path)
{
  FILE *infile = fopen(path,"r");
  if (!infile) {
    FTKSetup::PrintMessage(sevr, "load_offline_lut: error reading offline ID lookup table\n");
    return;
  }
  FTKSetup &ftkset = FTKSetup::getFTKSetup();
  const bool cmDebug = ftkset.getVerbosity()>1 ? true : false;
  
  char line[256];
  while( fgets(line, 256, infile) ) {
    // format is: <global id> <is pixel> <barrelEC> <layer> <etamod> <phimod>
    int globalid;
    int isPixel;
    int barrelEC;
    int layer=-1;
    int etamod;
    int phimod;
    sscanf(line,"%d %d %d %d %d %d",&globalid,&isPixel,&barrelEC,&layer,&etamod,&phimod);
    if(cmDebug) {
      printf("line: %s defines id: %d pix:%d bec:%d ly:%d eta:%d phi:%d\n",line,globalid,isPixel,barrelEC,layer,etamod,phimod);
    }
    if(layer==-1) {
      FTKSetup::PrintMessage(sevr, "load_offline_lut: error reading offline ID lookup table\n");
      continue;
    }
    FTKRegionMapOfflineId off;
    off.setIsPixel((bool)isPixel);
    off.setBarrelEC(barrelEC);
    off.setLayer(layer);
    off.setEtaMod(etamod);
    off.setPhiMod(phimod);
    m_global_to_offline_map[globalid] = off;
  }
  fclose(infile);
}



// new version that supports 64-tower rmap geometry
bool FTKRegionMap::isHitInRegion(const FTKHit &hit,int ID) const {
  // if old format, just use the 8-region legacy function:
  if(m_old_format) return isHitInRegion_old(hit,ID);
  if(FTKSetup::getFTKSetup().getITkMode()) return isHitInRegion_ITk(hit,ID);

  const int plane = hit.getPlane();
  const int section = hit.getSection();

  // check eta of the hit
  bool res_eta(false);
  const int eta = hit.getEtaModule();
  const int aside = hit.getASide();
  const int cside = hit.getCSide();
  const int reta = ID/m_nregions_phi;
  const int eta_min = m_map[ID][plane][section].getEtaMin();
  const int eta_max = m_map[ID][plane][section].getEtaMax();
  // it is aside (cside) module and the tower is on last (first) ring, indeed compatible in eta, other check based on phi
  if((cside && reta==0) || (aside && reta==3)) res_eta=true;
  // if is a aside (cside) the module but the tower is the barrel it may be accepted
  else if ((aside && reta==2) || (cside && reta==1) ) {
      if (eta_min!=-1) res_eta = true;
  }
  // if is a barrel module the eta compatibility has to be checked
  else if(!(aside||cside)) { // barrel
    if(eta_min==-1) return false;
    if(eta >= eta_min && eta<=eta_max ) {
      res_eta=true;
    }
  }

  if(!res_eta) return false;

  // check phi
  bool res_phi(false);
  const int phi = hit.getPhiModule();
  const int phi_min = m_map[ID][plane][section].getPhiMin();
  const int phi_max = m_map[ID][plane][section].getPhiMax();
  if(phi_min==-1) return false;
  if( phi_min <= phi_max ) { // away from phi=0 branch point
    if(phi >= phi_min && phi <= phi_max ) {
      res_phi = true;
    }
  } else { // phi=0 branch point
    if(phi >= phi_min || phi <= phi_max ) {
      res_phi = true;
    }
  }
  
  return res_phi;
}

/** this function returns true if a given FTKHit lays in
    region ID, false otherwise. _old means it applied to an 8-region rmap  */
bool FTKRegionMap::isHitInRegion_old(const FTKHit &hit,int ID) const
{
  int plane = hit.getPlane();
  int section = hit.getSection();
  int phimod = hit.getPhiModule();
  
  int min = m_map[ID][plane][section].getMin();
  int max = m_map[ID][plane][section].getMax();
  
  bool res(false);
  // region 0 has a special treatement
  if (ID==0) {
    /* the region 0 has a maximum, but due to the overlap with the last
       region the minimum is a negative number that has to be added
       to last module of the last region */
    if (phimod <= max || 
	phimod >= m_map[m_nregions-1][plane][section].getTotMod()+min+1)
      res = true;
  }
  else if ( phimod>= min && phimod <= max) {
    // hit is the region number ID
    res = true;
  }
  
  /*
    std::cout << "ID: " << ID << "\tplane/section: " << plane << "/"<<section;
    std::cout << "\tdim: " << hit.getDim() << "\tphimod: " << phimod << "\tres: " << res << std::endl;
    std::cout << "min: " << min << "\tmax: " << max << endl;
  */
  
  return res;
}

/** return  the element for a given (region,plane,section),
    -1 can be used to obtain the last element */
const FTKRegionMapItem &FTKRegionMap::getRegionMapItem(int ireg, int ipl, int isec) const
{
   if (ireg<0) {
     if( FTKSetup::getFTKSetup().getITkMode() ) {
       // JW: Only reg zero is set up correctly at the moment in ITk maps
       ireg = 0;
     } else {
       ireg = m_nregions+ireg;
     }
   }

   if (ipl<0) {
      ipl = m_nplanes+ipl;
   }

   return m_map[ireg][ipl][isec];
}


/** this function returns true if a given FTKHit lays in
    region ID, false otherwise. _ITk means it applied for the Phase-II geometry **/
bool FTKRegionMap::isHitInRegion_ITk(const FTKHit &hit,int ID) const {

  // Note : In this format -1 can be a read module ID --> no longer used
  //     to denote an unused layer
  
  const int plane = hit.getPlane();
  const int section = hit.getSection();

  // Not using ECs for now
  if( hit.getASide() || hit.getCSide() ) return false;
  
  const int eta	    = hit.getEtaModule();
  const int eta_min = m_map[ID][plane][section].getEtaMin();
  const int eta_max = m_map[ID][plane][section].getEtaMax();

  if( eta < eta_min || eta > eta_max ) return false;

  const int phi = hit.getPhiModule();
  const int phi_min = m_map[ID][plane][section].getPhiMin();
  const int phi_max = m_map[ID][plane][section].getPhiMax();

  if( phi_min <= phi_max ) { // away from phi=0 branch point
    if( phi < phi_min || phi > phi_max ) return false;
  } else { // phi=0 branch point
    if( phi < phi_min && phi > phi_max ) return false;
  }
  
  return true;
}


/** return the local module ID for a module, assuming a tower index */
unsigned int FTKRegionMap::getLocalModuleID(const FTKHit &hit, unsigned int TowerID) const {
  int plane = hit.getPlane();
  int eta = hit.getEtaCode();
  int phi = hit.getPhiModule();
  int sectionID = hit.getSection();
  //int aside = hit.getASide();

  // get the tower extremes
  const int &phimin = m_map[TowerID][plane][sectionID].getPhiMin();
  int phimax = m_map[TowerID][plane][sectionID].getPhiMax();
  const int &etamin = m_map[TowerID][plane][sectionID].getEtaMin();
  const int &etamax = m_map[TowerID][plane][sectionID].getEtaMax();

  // adjust the phi boundaries
  if (phimin>phimax) phimax += m_map[TowerID][plane][sectionID].getPhiTot();
  int nphi = phimax-phimin+1;


  // local 2D coordinates for the module
  int localPhi;
  int localEta;

  if (eta<20) { // is barrel
    // check that the module is really insider the TowerID
    if ( phi<phimin || phi>phimax || eta<etamin || eta>etamax )
      return -1;

    localPhi = phi-phimin;
    if (localPhi<0) localPhi += m_map[TowerID][plane][sectionID].getPhiTot();
    localEta = eta-etamin;

    return localEta*nphi+localPhi;
  }
  else { // is endcap
    // check that the module is really insider the TowerID
    if ( phi<phimin || phi>phimax)
      return -1;

    localPhi = phi-phimin;
    if (localPhi<0) localPhi += m_map[TowerID][plane][sectionID].getPhiTot();

    return sectionID*nphi+localPhi;
  }
}

void FTKRegionMap::convertLocalID(unsigned int localID, int TowerID, int plane, int section,
				  int &modphi, int &modeta)
{
 
  // get the tower extremes
  int phimin = m_map[TowerID][plane][section].getPhiMin();
  int phimax = m_map[TowerID][plane][section].getPhiMax();
  int etamin = m_map[TowerID][plane][section].getEtaMin();
  int etamax = m_map[TowerID][plane][section].getEtaMax();

  // adjust the phi boundaries
  int nphi = phimax-phimin+1;
  if (nphi<=0) nphi += m_map[TowerID][plane][section].getPhiTot();

 
  modphi = localID%nphi+phimin;
  // STS 150219: change >= to > , subtract nphi not phimax
  //  if (modphi>=phimax) modphi-=phimax;
  if (modphi>phimax) modphi-=nphi;
  // STS 150218: do not add phimax but add etamin
  //   modeta = localID/nphi+phimax;
  modeta = localID/nphi+etamin;


  if(((phimin<=phimax)&&((modphi<phimin)||(modphi>phimax)))||
     ((phimin> phimax)&& (modphi<phimin)&&(modphi>phimax) )||
     (modeta<etamin)||(modeta>etamax)) {
     std::cout<<"FTKRegionMap::convertLocalID:"
              <<" plane="<<plane<<" section="<<section
              <<" localID="<<localID<<" nphi="<<nphi<<" phimin="
              <<phimin<<" phi="<<modphi<<" phimax="<<phimax
              <<" etamin="<<etamin<<" eta="<<modeta<<" etamax="<<etamax<<"\n";
  }
}

unsigned int FTKRegionMap::getLocalID(int TowerID,int plane,int section,
                                      int modphi,int modeta) const {
  int phimin = m_map[TowerID][plane][section].getPhiMin();
  int etamin = m_map[TowerID][plane][section].getEtaMin();
  int etamax = m_map[TowerID][plane][section].getEtaMax();
  int phimax = m_map[TowerID][plane][section].getPhiMax();
  int phitot = m_map[TowerID][plane][section].getPhiTot();
  int nphi = phimax-phimin+1;
  if (nphi<=0) nphi += phitot;
  if(((phimin<=phimax)&&((modphi<phimin)||(modphi>phimax)))||
     ((phimin> phimax)&& (modphi<phimin)&&(modphi>phimax) )||
     (modeta<etamin)||(modeta>etamax)) {
     std::cout<<"FTKRegionMap::getLocalID phimin="
              <<phimin<<" modphi="<<modphi<<" phimax="<<phimax<<" phitot="<<phitot
              <<" etamin="<<etamin<<" modeta="<<modeta<<" etamax="<<etamax
              <<" tower="<<TowerID
              <<" plane="<<plane
              <<" section="<<section
              <<"\n";
  }
  return (modeta-etamin)*nphi+((modphi-phimin+phitot)%phitot);
}

//   (local id) == -1 => module is not in the tower
int FTKRegionMap::getLocalId(const unsigned int& towerId,const unsigned int& planeId,const unsigned int& globalModuleId) const
{
  tower_global_to_local_map_type::const_iterator imap = m_tower_global_to_local_map.find(towerId);
  if( imap == m_tower_global_to_local_map.end() ) {
    return -1;
  }
  const plane_global_to_local_map_type& plane_id_mapping( imap->second );
  plane_global_to_local_map_type::const_iterator jmap = plane_id_mapping.find(planeId);
  if( jmap == plane_id_mapping.end() ) {
    return -1;
  }
  const global_to_local_map_type& id_mapping( jmap->second );
  global_to_local_map_type::const_iterator i = id_mapping.find(globalModuleId);
  if( i==id_mapping.end() ) {
    return -1;
  }
  return i->second;
}

int FTKRegionMap::getGlobalId(const unsigned int& towerId,const unsigned int& planeId,const unsigned int& localModuleId) const
{
  tower_global_to_local_map_type::const_iterator imap = m_tower_global_to_local_map.find(towerId);
  if( imap == m_tower_global_to_local_map.end() ) {
    return -1;
  }
  const plane_global_to_local_map_type& plane_id_mapping( imap->second );
  plane_global_to_local_map_type::const_iterator jmap = plane_id_mapping.find(planeId);
  if( jmap == plane_id_mapping.end() ) {
    return -1;
  }
  const global_to_local_map_type& id_mapping( jmap->second );
  BOOST_FOREACH( const global_to_local_map_type::value_type& thispair , id_mapping ) {
    if( thispair.second==localModuleId ) {
      return thispair.first;
    }
  }
  return -1;
}

// construct total global to local mapping for ssmap hardware LUT
std::map<unsigned int,unsigned int> FTKRegionMap::getGlobalToLocalMapping(const unsigned int& towerId,const unsigned int& planeId) const
{
  std::map<unsigned int,unsigned int> empty_result;
  tower_global_to_local_map_type::const_iterator imap = m_tower_global_to_local_map.find(towerId);
  if( imap == m_tower_global_to_local_map.end() ) {
    FTKSetup::PrintMessage(warn,"getGlobalToLocalMapping: cannot find tower id\n");
    return empty_result;
}
  const plane_global_to_local_map_type& plane_id_mapping( imap->second );
  plane_global_to_local_map_type::const_iterator jmap = plane_id_mapping.find(planeId);
  if( jmap == plane_id_mapping.end() ) {
    FTKSetup::PrintMessage(warn,"getGlobalToLocalMapping: cannot find plane id\n");
    return empty_result;
  }
  const global_to_local_map_type& id_mapping( jmap->second );
  return id_mapping;
}

