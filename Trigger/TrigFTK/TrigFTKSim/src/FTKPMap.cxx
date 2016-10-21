/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKPMap.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <TTree.h>
#include <TKey.h>
using namespace std;

#include "TrigFTKSim/ftkdefs.h"
using namespace ftk;

FTKPlaneSection::FTKPlaneSection()
  : m_plane(-1), m_section(0)
{
  // nothing to do
}


FTKPlaneSection::FTKPlaneSection(int plane, int section)
  : m_plane(plane), m_section(section)
{
  // nothing to do 
}


FTKModuleInfo::FTKModuleInfo() :
  m_isEndcap(0), m_layer(-1), m_pdisk(0), m_nphi(-1),
  m_neta(-1), 
  m_nfi(0), m_ncott(0), m_ncurv(0),
  m_nste(0), m_ndimension(0)
{
   // nothing to do here
}


FTKModuleInfo::FTKModuleInfo(int isEndcap,int layer,int pdisk,
			     int nphi,int neta,int nfi,int ncott,int ncurv,int nste,int ndimension) : 
   m_isEndcap(isEndcap), m_layer(layer), m_pdisk(pdisk), m_nphi(nphi), m_neta (neta), 
   m_nfi(nfi), m_ncott(ncott) ,m_ncurv(ncurv) ,m_nste(nste), m_ndimension(ndimension) 
{
   //
}


const TString FTKPlaneMap::s_dirname = "PlaneMap";


FTKPlaneMap::FTKPlaneMap(const char *fname) :
   m_path(fname), m_nplanes(0), // m_nlayers(0), 
   m_rlayers(0),
   m_totaldim(0),   m_planeType(0),
   m_isZ(0),
   m_planePair(0),
   m_enablePlanePair(false),  
   m_nsections(0),
   m_isok(false)
{
   //! Default constructor
   FTKSetup &ftkset = FTKSetup::getFTKSetup();
   m_idim[0] = 0; m_idim[1] = 0;
   m_planes = 0x0;

   // cmDebug was a variable used in the old version, here
   // redefined as a boolean true if the FTK verbosity level 
   // is greater than 1
   bool cmDebug = ftkset.getVerbosity()>1 ? true : false;

   // pmap file pointer
  FILE *fpmap = fopen(fname,"r");
  if (!fpmap) {
    cerr << "*** error reading pmap file: " << fname << endl;
    return;
  }
  

  /* the code to read PMAP file is taken from the original
   F  pmap_rd.c. */

  // store the maximum ID of an unused plane, 
  // needed to size the m_planes_unused
  int max_plane_id(-1);

  int used_plane_dim[ftk::MAXPLANES];
  int used_plane_sections[ftk::MAXPLANES];
  int section;
  char line[100];
  int npixel=0, npixelEC=0, nSCT=0, nSCTEC=0, ntot, isEC, i, pd, plane, nphi, neta, nste, ndimension;
  int nSCTtrk=0, nfi, ncott, ncurv; // cy addition
  char buf1[20];
  char buf2[20];

  if (cmDebug) {
    FTKSetup::PrintMessageFmt(warn,"In pmap_rd, barrelOnly = %d",ftkset.getBarrelOnly());
  }

  // zeroes the counters
  for (int j=0; j < ftk::MAXPLANES; j++) {
    used_plane_dim[j] = 0;
    used_plane_sections[j] = 0;
  }

  /* check for correct file format */
  if (fgets(line,100,fpmap)!=NULL) {
    if (cmDebug) printf("%s\n",line);
    sscanf(line,"%d %19s %19s",&npixel,buf1,buf2);
    if (cmDebug) printf("%s %s\n",buf1,buf2);
    if (strcmp(buf2,"barrel")) {
      // pre end-cap extension format
      FTKSetup::PrintMessage(ftk::sevr,"In pmap_rd, old pmap file format (before endcap extension) is no longer supported");
    }
  } else {
    FTKSetup::PrintMessage(ftk::sevr,"In pmap_rd, pmap file empty?");
  }
  // after the check return at the begin
  rewind(fpmap);

  if (cmDebug) FTKSetup::PrintMessage(info,"getting pmap size, new format\n");
  
  /*
   * retrive the information on the number of pixel and SCT layers
   */

  if(fgets(line,100,fpmap)==NULL)
    FTKSetup::PrintMessage(sevr,"pmap_rd: Reading of pmap file failed.");
  sscanf(line,"%d pixel barrel",&npixel);
  if(fgets(line,100,fpmap)==NULL)
    FTKSetup::PrintMessage(sevr,"pmap_rd: Reading of pmap file failed.");
  sscanf(line,"%d pixel endcap",&npixelEC);

  // Check if we have SCTtrk or real SCT planes:  
  if(fgets(line,100,fpmap)==NULL)
    FTKSetup::PrintMessage(sevr,"pmap_rd: Reading of pmap file failed.");
  if (strstr(line,"SCTtrk")) {
    sscanf(line,"%d SCTtrk",&nSCTtrk);
  }
  else {
    nSCTtrk=0;
    sscanf(line,"%d SCT barrel",&nSCT);
    if(fgets(line,100,fpmap)==NULL)
      FTKSetup::PrintMessage(sevr,"pmap_rd: Reading of pmap file failed.");
    sscanf(line,"%d SCT endcap",&nSCTEC);
  }

  // fix the total number of layers to setup
  ntot=nSCTtrk+nSCT+nSCTEC+npixel+npixelEC;

  if (cmDebug) FTKSetup::PrintMessage(debg,"Allocating pmap...\n");
  /* Allocation of map. This way we also defaults all to plane 0
   * because of calloc behaviour.
   */
  m_map[PIXEL][BARREL] = new FTKPlaneSection[npixel];
  if (!m_map[PIXEL][BARREL]) 
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap pixel barrel failed.");

  m_map[PIXEL][ENDCAP] = new FTKPlaneSection[npixelEC];
  if (!m_map[PIXEL][ENDCAP]) 
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap pixel endcap failed.");

  m_map[SCTtrk][BARREL] = new FTKPlaneSection[nSCTtrk];
  if (!m_map[SCTtrk][BARREL])
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap SCTtrk barrel failed.");
  
  m_map[SCT][BARREL] = new FTKPlaneSection[nSCT];
  if (!m_map[SCT][BARREL]) 
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap SCT barrel failed.");
  m_map[SCT][ENDCAP] = new FTKPlaneSection[nSCTEC];
  if (!m_map[SCT][ENDCAP]) 
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap SCT endcap failed.");
  if (cmDebug) FTKSetup::PrintMessage(debg,"\t\t...done\n");

  printf("=== Layer Configuration ===\n");

  while(fgets(line,100,fpmap)!=NULL) { // first read loop
    switch(line[0]) {

    case 'p':

      /* read */
      sscanf(line,"pixel %d %d %d dim %d plane %d",&isEC,&pd,&i,&ndimension,&plane);


      /* print */
      if(plane!=-1 ) {
	if (plane>max_plane_id) max_plane_id = plane;
	if (!isEC) 
	  printf("Pixel barrel: i = %d, NofLogPlane = %d section %d ndim %d\n",
		 i, plane, used_plane_sections[plane],ndimension);
	else 
	  printf("Pixel endcap: i = %d, NofLogPlane = %d section %d ndim %d\n",
		 i, plane, used_plane_sections[plane],ndimension);
      }

      /* store to map */
      m_map[PIXEL][isEC][i].setPlane(plane);
      if (plane != -1)
	m_map[PIXEL][isEC][i].setSection(used_plane_sections[plane]);
      m_rlayers++;

/*       if (cmDebug) {
 * 	printf("Read back plane %d section %d",
 * 	    m_map[PIXEL][isEC][i].plane,
 * 	    m_map[PIXEL][isEC][i].section);
 * 	FTKSetup::PrintMessage(debg,"\n");
 *      }
 */

      /* count number of modules, record dimension, used plane number */
      if (plane < ftk::MAXPLANES) {
	if (plane != -1) {
	  used_plane_dim[plane] = ndimension;
	  used_plane_sections[plane]++;
	}
      } else {
	FTKSetup::PrintMessage(sevr,"pmap_rd: Logical plane above maximum allowed.");
      }

      break;

    case 'S':

      /* read */
      if(nSCTtrk==0)
	sscanf(line,"SCT %d %d %d dim %d stereo %d plane %d",&isEC,&pd,&i,&ndimension,&nste,&plane);
      else
	{
	  sscanf(line,"SCTtrk %d dim %d plane %d phi %d cott %d curv %d",&i,&ndimension,&plane, &nfi,&ncott,&ncurv);
	  isEC=0;
	  pd=-1;
	}


      /* print */
      if(plane!=-1) {
	if (plane>max_plane_id) max_plane_id = plane;
	if(nSCTtrk==0) {
	  if (!isEC) 
	    printf("SCT barrel: i = %d, NofLogPlane = %d section %d \n",
		   i, plane, used_plane_sections[plane]);
	  else 
	    printf("SCT endcap: i = %d, NofLogPlane = %d section %d \n",
		   i, plane, used_plane_sections[plane]);
	}
	else
	  printf("SCTtrk: i = %d, NofLogPlane = %d section %d ndim %d\n",
		 i, plane, used_plane_sections[plane],ndimension);
      }

      /* store to map */
      if(nSCTtrk==0) {
	m_map[SCT][isEC][i].setPlane(plane);
	if (plane != -1)
	  m_map[SCT][isEC][i].setSection(used_plane_sections[plane]);
      }
      else {
	m_map[SCTtrk][isEC][i].setPlane(plane);
	if (plane != -1)
	  m_map[SCTtrk][isEC][i].setSection(used_plane_sections[plane]);
      }
      m_rlayers++;
      
      /* count number of modules, record dimension, used plane number */
      if (plane < ftk::MAXPLANES) {
	if(plane != -1) {
	  used_plane_dim[plane] = ndimension;
	  used_plane_sections[plane]++;
	}
      } else {
	FTKSetup::PrintMessage(sevr,"pmap_rd: Logical plane above maximum allowed.");
      }
      break;

    default:
      FTKSetup::PrintMessageFmt(sevr,"pmap_rd: Bad file format: %s",line);
      break;
    }
  } // end first read loop

  if (cmDebug) FTKSetup::PrintMessage(debg,"First pmap file read done\n");

  /* decode used_plane_dim to infer number of valid logical planes and their dimension */
  m_nplanes = max_plane_id+1;

  for (int j = 0; j < ftk::MAXPLANES; j++) {
    if (used_plane_dim[j]) {
      m_totaldim += used_plane_dim[j];
    }
  }


  // allocate the array to store the number of used sections
  m_nsections = new int[ftk::MAXPLANES];
  if ((m_nsections = (int *)calloc(ftk::MAXPLANES,sizeof(int))) == NULL)
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap nsections failed.");
  for (int j = 0; j < ftk::MAXPLANES; j++) {
    m_nsections[j] = 0; // initialize the value
    if (used_plane_dim[j]) {
      m_nsections[j] = used_plane_sections[j];
      if (cmDebug) printf("Plane %d has %d sections\n",j,m_nsections[j]);
    }
  }


  /* Need to rewire this to accomodate multiple physical sections per plane */
  /* with the inclusion of the endcap, it is no longer a 1<->1 mapping    */
  /* Now: 2d array, multiple sections per plane.			  */

  if (cmDebug) FTKSetup::PrintMessage(debg,"Allocating moduleInfo array...\n");

  if(!(m_planes = new FTKModuleInfo*[m_nplanes]))
    FTKSetup::PrintMessage(sevr,"pmap_rd: Failed to alloc plane details space 1");

  for (int j = 0; j < m_nplanes; j++) {
    if(!(m_planes[j] = new FTKModuleInfo[m_nsections[j]]))
      FTKSetup::PrintMessage(sevr,"pmap_rd: Failed to alloc plane details space 2");
  }


  if (cmDebug) {
    FTKSetup::PrintMessage(debg,"\t\t...done\n");
    printf("Building isZ, idim maps, m_totaldim = %d, ntot = %d\n",m_totaldim,ntot);
    FTKSetup::PrintMessage(debg,"\n");
  }

  m_isZ = new int[m_totaldim];
  m_planePair = new int[m_nplanes];
  m_planeType = new int[m_nplanes];
  m_idim[0] = new int[m_nplanes];
  m_idim[1] = new int[m_nplanes];
  for(int l=0;l<m_nplanes;l++) {
    m_idim[0][l]=-1;
    m_idim[1][l]=-1;
    m_planeType[l]=ftk::NOASSIGNED; // to check if the value changed
    m_planePair[l]=-1;
  }
  // move again to the file begin
  rewind(fpmap);
  fgets(line,100,fpmap);
  fgets(line,100,fpmap);
  fgets(line,100,fpmap);
  if(nSCTtrk==0)
    fgets(line,100,fpmap);

  for (int l = 0; l < ftk::MAXPLANES; l++) {
    used_plane_sections[l] = 0;
  }
    

  int jcoord(0);
  int kcoord(0);
  while(fgets(line,100,fpmap)!=NULL) {
    switch(line[0]) {

    case 'p':

      /* read */
      sscanf(line,"pixel %d %d %d dim %d plane %d phi %d eta %d",&isEC,&pd,&i,&ndimension,&plane,&nphi,&neta);

      if (cmDebug) {
	printf("Pixel module isEC %d, ndim %d, plane %d, phi %d, eta %d",
	    isEC,ndimension,plane,nphi,neta);
	FTKSetup::PrintMessage(debg,"\n");
      }

      /* store */
      if(plane != -1) { // Plane -1 is ignored, this way we have plane mask
	section = used_plane_sections[plane]++;
	m_planes[plane][section].setIsEndcap(isEC);
	m_planes[plane][section].setPDisk(pd);
	m_planes[plane][section].setNumEta(neta);
	m_planes[plane][section].setNumPhi(nphi);
	m_planes[plane][section].setNSte(-1);
	m_planes[plane][section].setNumFi(-1);
	m_planes[plane][section].setNumCott(-1);
	m_planes[plane][section].setNumCurv(-1);
	m_planes[plane][section].setNDimension(ndimension);
	m_planes[plane][section].setLayer(i);
	// FlagAK: if coordinate ordering changes, this code must be changed
	// corrinne says: only count barrel (want information per plane), note
	// that this requires all parts of a plane to have the same structure
	m_planeType[plane]=ftk::PIXEL;
	if (!isEC) {
	  for(int l=0;l<ndimension && l<2;l++) {
	    m_isZ[jcoord+l] = l;
	    m_idim[l][kcoord] = jcoord+l;
        /// cmDebug is a bool, cannot be here
	    // if (cmDebug > 4) {
	    //   printf("\t\tisZ[j+l] = l:    isZ[%d+%d] = %d\n",jcoord,l,l);
	    //   printf("\t\tidim[l][k] = j+l idim[%d][%d] = %d\n",l,kcoord,jcoord+l);
	    // }
	  }
	  jcoord += ndimension; 
	  kcoord += 1;
	}
      }
      continue;

    case 'S':
      /* read */
      if(nSCTtrk==0) 
	sscanf(line,"SCT %d %d %d dim %d stereo %d plane %d phi %d eta %d",&isEC,&pd,&i,&ndimension,&nste,&plane, &nphi,&neta);
      else{
	sscanf(line,"SCTtrk %d dim %d plane %d phi %d cott %d curv %d",&i,&ndimension,&plane, &nfi,&ncott,&ncurv);
	isEC=0;
	pd=-1;
      }
      if (cmDebug) {
	printf("SCT module isEC %d, ndim %d, plane %d, phi %d, eta %d",
	    isEC,ndimension,plane,nphi,neta);
	FTKSetup::PrintMessage(debg,"\n");
      }

      /* store */
      if(plane != -1) { // used plane block
	section = used_plane_sections[plane]++;
	m_planes[plane][section].setIsEndcap(isEC);
	m_planes[plane][section].setPDisk(pd);
	if(nSCTtrk==0) { // default SCT block
	  m_planes[plane][section].setNumEta(neta);
	  m_planes[plane][section].setNumPhi(nphi);
	  m_planes[plane][section].setNSte(nste);
	  m_planes[plane][section].setNumFi(-1);
	  m_planes[plane][section].setNumCott(-1);
	  m_planes[plane][section].setNumCurv(-1);
	} // end default SCT block
	else { // SCTtrk block
	  m_planes[plane][section].setNumEta(-1);
	  m_planes[plane][section].setNumPhi(-1);
	  m_planes[plane][section].setNumFi(nfi);
	  m_planes[plane][section].setNumCott(ncott);
	  m_planes[plane][section].setNumCurv(ncurv);
	  m_planes[plane][section].setNSte(-2);
	} // end SCTtrk block
	m_planes[plane][section].setNDimension(ndimension);
	m_planes[plane][section].setLayer(i);
	// FlagAK: if coordinate ordering changes, this code must be changed
	if(nSCTtrk==0)
	  m_planeType[plane]=ftk::SCT;
	else
	  m_planeType[plane]=ftk::SCTtrk;
	// corrinne says: only count barrel (want information per plane), note
	// that this requires all parts of a plane to have the same structure
	if (!isEC) {
	  for(int l=0;l<ndimension && l<2;l++) {
	    m_isZ[jcoord+l] = l;
	    m_idim[l][kcoord] = jcoord+l;
        //// cmdebug bool, cannot be here
	    // if (cmDebug > 4) {
	    //   printf("isZ[j+l] = l:    isZ[%d+%d] = %d\n",jcoord,l,l);
	    //   printf("idim[l][k] = j+l idim[%d][%d] = %d\n",l,kcoord,jcoord+l);
	    // }
	  }
	  jcoord += ndimension; 
	  kcoord += 1;
	}
      } // end used plane block
      continue;

    default:
      FTKSetup::PrintMessageFmt(sevr,"pmap_rd: Bad file format: %s",line);
    }
  }

  if (cmDebug) {
     DumpPMap();
     //FTKSetup::PrintMessage(debg,"\n\n");
  }
  
      if(nSCTtrk==0){
  // set the m_planePair array. When the 1st SCT plane is found
  // it tests the following, if is a different configuration sets the
  // pair to next, if is the same doesn't set any connection
  for (int ip=0;ip!=m_nplanes;++ip) {
    // default -1 value for non-SCT planes
    if (m_planeType[ip]!=ftk::SCT) {
      m_planePair[ip] = -1;
    }
    else {
      if (ip==m_nplanes-1) {
	// if reach the last element this is unpaired by definition
	m_planePair[ip] = ip;
	continue;
      }

      if (m_planeType[ip+1]!=ftk::NOASSIGNED) {
	// the PMAP can have discontinuities, ensure this is not the case
	// if this layer and the next have a different orientation
	// make the connection
	if (m_planes[ip][0].getNSte()!=m_planes[ip+1][0].getNSte()) {
	  m_planePair[ip  ] = ip+1;
	  m_planePair[ip+1] = ip;
	  ++ip; // move to the next unpaired SCT layer
	}
	else {
	  // this to not broke the FTK_AMBank::informationBank() behaviour, 
	  // that assumes pairs of R-phi and stereo layers
	  m_planePair[ip] = ip; 
	}
      }
      else {
	// no connection is possible also if the next layer is not assigned
	m_planePair[ip] = ip; 
      }
    }


  }
      }
      fclose(fpmap);
  m_isok = true;
}

FTKPlaneMap::FTKPlaneMap(TDirectory* file) {
   //! Read pmap class from root file
   //! return NULL if reading failed.
   //! check validity also using m_isok

   //cout<<"Instantiate PlaneMap from root file."<<endl;
   TDirectory* thisdir = gDirectory;

   m_isok=false;

   if ( !file->cd(s_dirname) ) {
      cerr << "Error in FTKPlaneMap::FTKPlaneMap. No PlaneMap found in root-file. "<< endl;
      return;
   } 

   m_path = gDirectory->GetTitle();
   //cout<<"PathName:        "<<m_path<<endl;

   TTree* tplane = (TTree*)gDirectory->Get("Planes");
   TTree* tree   = (TTree*)gDirectory->Get("PlaneConstants");
   TTree* tConst = (TTree*)gDirectory->Get("Constants");
   if ( !tplane || !tree || !tConst ) {
      cerr << "Error in FTKPlaneMap::FTKPlaneMap. Could not find trees. "<< endl;
      cerr << "  tree('Planes')          "<<tplane<<endl;
      cerr << "  tree('PlaneConstants')  "<<tree<<endl;
      cerr << "  tree('Constants')       "<<tConst<<endl;
      m_nplanes=0;
      m_rlayers=0;
      m_totaldim=0;
      m_planeType=0;
      m_isZ=0;
      m_planePair=0;
      m_enablePlanePair=0;
      m_idim[0] = 0; m_idim[1] = 0;
      m_nsections=0;
      m_planes=0;
      return;
   }

   m_isok=true;

   int chksum;
   // constants:
   tConst->GetBranch("nplanes")->SetAddress(&m_nplanes);
   tConst->GetBranch("rlayers")->SetAddress(&m_rlayers);
   tConst->GetBranch("totaldim")->SetAddress(&m_totaldim);
   tConst->GetBranch("enablePlanePair")->SetAddress(&m_enablePlanePair);
   tConst->GetBranch("Checksum")->SetAddress(&chksum);
   // read constants
   tConst->GetEntry(0);
   // cout<<"nplanes          "<<m_nplanes<<endl;
   // cout<<"rlayers          "<<m_rlayers<<endl;
   // cout<<"totaldim         "<<m_totaldim<<endl;
   // cout<<"enablePlanePair: "<<m_enablePlanePair<<endl;

   if ( tree->GetEntries() != m_nplanes ) cerr<<"Error in FTKPlaneMap::FTKPlaneMap. Inconsistent nplanes."<<endl;
   int isZ,planePair,planeType,nsections;
   int idim[2];
   tree->GetBranch("isZ")->SetAddress(&isZ);
   tree->GetBranch("planePair")->SetAddress(&planePair);
   tree->GetBranch("planeType")->SetAddress(&planeType);
   tree->GetBranch("idim_0")->SetAddress(&idim[0]);
   tree->GetBranch("idim_1")->SetAddress(&idim[1]);
   tree->GetBranch("nsections")->SetAddress(&nsections);

   m_planeType = new int[m_nplanes]; // SCT / PIXEL / SCTtrk
   m_isZ = new int[m_totaldim]; // legacy (unused)
   m_planePair = new int[m_nplanes];
   m_idim[0]= new int[m_nplanes];
   m_idim[1]= new int[m_nplanes];
   m_nsections = new int[ftk::MAXPLANES]; // number of sections in the used planes
   for ( int i = 0 ; i < ftk::MAXPLANES ; i++ ) m_nsections[i]=0;

   for ( int j = 0 ; j<m_nplanes ; j++ ) {
      tree->GetEntry(j);
      m_planeType[j] = planeType;
      m_isZ[j] = isZ;
      m_planePair[j] = planePair;
      m_idim[0][j] = idim[0];
      m_idim[1][j] = idim[1];   
      m_nsections[j] = nsections;
   }
   
   int iplane,isection;
   int isEndcap,layer,pdisk,nphi,neta,nfi,ncott,ncurv,nste,ndimension;
   int mapSection,mapPlane,mapPlaneType,mapEC,mapItype;
   tplane->GetBranch("iplane")->SetAddress(&iplane);
   tplane->GetBranch("isection")->SetAddress(&isection);

   tplane->GetBranch("isEndcap")->SetAddress(&isEndcap);
   tplane->GetBranch("layer")->SetAddress(&layer);
   tplane->GetBranch("pdisk")->SetAddress(&pdisk);
   tplane->GetBranch("nphi")->SetAddress(&nphi);
   tplane->GetBranch("neta")->SetAddress(&neta);
   tplane->GetBranch("nfi")->SetAddress(&nfi);
   tplane->GetBranch("ncott")->SetAddress(&ncott);
   tplane->GetBranch("ncurv")->SetAddress(&ncurv);
   tplane->GetBranch("nste")->SetAddress(&nste);
   tplane->GetBranch("ndimension")->SetAddress(&ndimension);

   tplane->GetBranch("mapSection")->SetAddress(&mapSection);
   tplane->GetBranch("mapPlane")->SetAddress(&mapPlane);
   tplane->GetBranch("mapPlaneType")->SetAddress(&mapPlaneType);
   tplane->GetBranch("mapEC")->SetAddress(&mapEC);
   tplane->GetBranch("mapItype")->SetAddress(&mapItype);

   const int nmax = 34;
   //m_map = new FTKPlaneSection**[ftk::NHITTYPES];
   for ( int i = 0 ; i < ftk::NHITTYPES ; i++ ){
      //m_map[i] = new FTKPlaneSection*[2];
      for ( int j = 0 ; j < 2 ; j++ ) {
	 //FTKPlaneSection *m_map[ftk::NHITTYPES][2];
	 m_map[i][j] = new FTKPlaneSection[nmax];
      }
   }



   // map, planes 
   int nev=0;
   m_planes = new FTKModuleInfo*[m_nplanes];
   for (int j = 0; j < m_nplanes; j++) {
      tplane->GetEntry(nev++);
      //cout <<"j="<<j<<", iplane="<<iplane<<endl;
      //printf("\tPlane %d has %d sections\n",j,m_nsections[j]);
      m_planes[j] = new FTKModuleInfo[m_nsections[j]];
      for (int k = 0; k < m_nsections[j]; k++) {	
	 if ( k!= 0) tplane->GetEntry(nev++);
	 if ( k!= isection || j!= iplane ) {
	    cerr << "Error reading PlaneMap."<<endl;
	    m_isok = false;
	 }
	 // planes
	 // m_planes[j][k] = {isEndcap,layer,pdisk, nphi,neta,nfi,ncott,ncurv,nste,ndimension};
	 m_planes[j][k] =  FTKModuleInfo{isEndcap,layer,pdisk,
					 nphi,neta,nfi,ncott,ncurv,nste,ndimension};
	 // maps
         // plane   = m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane();
         // section = m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection();
	 m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()] = FTKPlaneSection{mapPlane,mapSection};
      }
   }

   // cout<<"FTKPlaneMap::FTKPlaneMap read from file. Dumping PlaneMap"<<endl;
   // cout<<"-----------------------------------------------------"<<endl;
   // DumpPMap();
   // cout<<"-----------------------------------------------------"<<endl;

   thisdir->cd(); // go back
 
   if ( chksum != CalcChecksum() ) {
      cerr<<"FTKPMap::FTKPMap. Error. Checksum after reading is not consistent with Checksum stored in file."<<endl;
      m_isok=false;
   }


}

int FTKPlaneMap::CalcChecksum() const {
   // calculate checksum of map
   vector<double> vec;

   // fill vec with member variables
   vec.push_back((double)m_nplanes);
   vec.push_back((double)m_rlayers);
   vec.push_back((double)m_totaldim);
   vec.push_back((double)m_enablePlanePair);

   // plane, map, m_planes
   // m_plane, m_map
   for (int j = 0; j < m_nplanes; j++) {
      for (int k = 0; k < m_nsections[j]; k++) {
	 vec.push_back((double)j);
	 vec.push_back((double)k);
	 vec.push_back((double)m_planes[j][k].getIsEndcap());
	 vec.push_back((double)m_planes[j][k].getLayer());
	 vec.push_back((double)m_planes[j][k].getPDisk());
	 vec.push_back((double)m_planes[j][k].getNumPhi());
	 vec.push_back((double)m_planes[j][k].getNumEta());
	 vec.push_back((double)m_planes[j][k].getNumFi());
	 vec.push_back((double)m_planes[j][k].getNumCott());
	 vec.push_back((double)m_planes[j][k].getNumCurv());
	 vec.push_back((double)m_planes[j][k].getNSte()); 
	 vec.push_back((double)m_planes[j][k].getNDimension());
	 // m_map
	 vec.push_back((double)m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane() );
	 vec.push_back((double)m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection());
	 vec.push_back((double)m_planes[j][k].getNSte() == -1);
	 vec.push_back((double)m_planes[j][k].getIsEndcap());
	 vec.push_back((double)m_planes[j][k].getLayer());
      }
   } 

   for ( int i = 0 ; i<m_nplanes ; i++ ) {
      vec.push_back((double)m_isZ[i]);
      vec.push_back((double)m_planePair[i]);
      vec.push_back((double)m_planeType[i]);
      vec.push_back((double)m_idim[0][i]);
      vec.push_back((double)m_idim[1][i]);
      vec.push_back((double)m_nsections[i]);
   }
   
   // path
   /*
   TString title = ftk::StripFileName(getPath());
   std::string smd5 = title.MD5().Data();
   double* dmd5 = (double*)&smd5[0];
   for ( unsigned int i = 0 ; i<smd5.size()/sizeof(double); i++ ) {
      vec.push_back(dmd5[i]);
   }
   */

   size_t size = sizeof(double) * vec.size();
   int checksum = ftk::adler32chksum(&vec[0],size);
   //cout<<"Checksum PMap: "<<checksum<<endl;

   return checksum;
   
}


void FTKPlaneMap::DumpPMap() const {
    printf("Dumping pmap\n");
    printf("%d planes\n",m_nplanes);
    for (int j = 0; j < m_nplanes; j++) {
      printf("\tPlane %d has %d sections\n",j,m_nsections[j]);
      for (int k = 0; k < m_nsections[j]; k++) {
	printf("\t\tSection %d: ",k);
	if (m_planes[j][k].getNSte() == -2) printf("SCT track ");
	else if (m_planes[j][k].getNSte() == 0) printf("SCT axial ");
	else if (m_planes[j][k].getNSte() == 1) printf("SCT stereo ");
	else printf("pixel ");
	if (m_planes[j][k].getIsEndcap()) printf("endcap ");
	else printf("barrel ");
	printf("(physical layer %d / %d, map gives back plane %d section %d): \n",
	       m_planes[j][k].getPDisk(), 
	       m_planes[j][k].getLayer(), 
	       //     m_map[1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane(),
	       // m_map[1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection());
	       m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane(),
	       m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection());
	if (m_planes[j][k].getNSte() == -2)
	  printf("\t\t%d modules in fi %d in cott %d in curv\n", 
		 m_planes[j][k].getNumFi(), m_planes[j][k].getNumCott(),m_planes[j][k].getNumCurv());
	else
	  printf("\t\t%d modules in eta %d in phi\n", 
		 m_planes[j][k].getNumEta(), m_planes[j][k].getNumPhi());
      }
    }
}


int FTKPlaneMap::GetConst(TDirectory* file, TString ConstName){
   // get nplanes from root file
   // if error: return -1
   TDirectory* thisdir = gDirectory;


   if ( !file->cd(s_dirname) ) {
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
   TBranch* br = tConst->GetBranch(ConstName);
   br->SetAddress(&ret);
   br->GetEntry(0);
   thisdir->cd();
   return ret;
}

int FTKPlaneMap::GetNPlanes(TDirectory* file){
   // get nplanes from root file
   // if error: return -1
   return GetConst(file,"nplanes");
}

int FTKPlaneMap::GetTotalDim(TDirectory* file){
   // get totaldim from root file
   // if error: return -1
   return GetConst(file,"totaldim");
}

int FTKPlaneMap::GetChecksum(TDirectory* file){
   // get totaldim from root file
   // if error: return -1
   return GetConst(file,"Checksum");
}



FTKPlaneMap* FTKPlaneMap::GetPMapFromRootFile(TDirectory* file){
   //! Read pmap class from root file
   //! return NULL if reading failed.
   //! check validity also using m_isok

   TDirectory* thisdir = gDirectory;
   FTKPlaneMap* ret = new FTKPlaneMap(file);

   if ( !ret->getIsOK() ) {
      cerr <<"FTKPlaneMap::GetPMapFromRootFile. Error. PlaneMap seems not to be ok (isok==false). Returning nullptr."<<endl;
      delete ret;
      return NULL;
   }
   
   thisdir->cd(); // go back
   return  ret;
}

void FTKPlaneMap::WriteMapToRootFile(TDirectory* dir)
{
   //! Write PMap to root file
   //! PMap is stored in TDirectory* pmap
   //!    name:  "PMap"
   //!    title:  <filename>
   //! PMap is again readable using ReadMapFromRootFile()
   
   //cout<<"Writing PlaneMap to root file."<<endl;
   // cout<<"-----------------------------------------------------"<<endl;
   // DumpPMap();
   // cout<<"-----------------------------------------------------"<<endl;
   
   TDirectory* thisdir = gDirectory;
   // check if PMap already exists
   if ( dir->GetKey(s_dirname) ) {
      if ( FTKPlaneMap::GetChecksum(dir) == CalcChecksum()  ) {
	 cout<<"Info. Identical PlaneMap already written to disk. Skipping writing."<<endl;
	 return;
      }
      else {
	 cerr << "Error: [FTKPlaneMap::WriteMapToRootFile] There is already a PlaneMap in this file: " << dir->GetTitle() << endl;
	 cerr << "                                         PlaneMap title: "<<dir->GetKey(s_dirname)->GetTitle()<<endl;
	 cerr << "       *** Skipping writing *** "<<endl;
	 return;
      }
   }
   if ( !m_isok ) {
      cerr << "Error: [FTKPlaneMap::WriteMapToRootFile] PlaneMap is not properly initalized. m_isok= " << m_isok << endl;
      cerr << "       *** Skipping writing *** "<<endl;
      return;
   }

   // create new directory for PMap content
   TString title = ftk::StripFileName(getPath());
   TDirectory* pmapdir = dir->mkdir(s_dirname,title);
   pmapdir->cd();
   
   /*
   // write PMap content
   cout<<"ftk::NHITTYPES="<<ftk::NHITTYPES<<endl;
   cout<<"Writing m_planes, m_map"<<endl;
   printf("%d planes\n",m_nplanes);
   for (int j = 0; j < m_nplanes; j++) {
      printf("\tPlane %d has %d sections\n",j,m_nsections[j]);
      for (int k = 0; k < m_nsections[j]; k++) {
	 printf("\t\tSection %d: \n",k);

	 TString objname = Form("plane_%d_%d",j,k);
	 cout<<" * Writing m_planes: "<<objname<<endl;
	 pmapdir->WriteObject(&m_planes[j][k],objname);
	
	 printf("\t\t\t(physical layer %d / %d, map gives back plane %d section %d): \n",
		m_planes[j][k].getPDisk(), 
		m_planes[j][k].getLayer(), 
		m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane(),
		m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection());

	 int a = m_planes[j][k].getNSte() == -1; //ftk::NHITTypes
	 int b = m_planes[j][k].getIsEndcap(); // EC or not
	 int c = m_planes[j][k].getLayer();
	 TString mapname = Form("map_%d_%d_%d",a,b,c);
	 cout<<" * Writing m_map: "<<mapname<<endl;
	 pmapdir->WriteObject(&m_map[a][b][c],mapname);
      }
   }  
   */

   //cout<<"Writing planes into tree."<<endl;
   TTree tplane("Planes","Planes");

   // plane
   int iplane, isection;
   int isEndcap, layer, pdisk, nphi, neta, nfi, ncott, ncurv, nste, ndimension;
   // map
   int plane,section,i1,i2,i3;
   // m_planes
   tplane.Branch("iplane",&iplane,"iplane/I");
   tplane.Branch("isection",&isection,"isection/I");
   // m_planes
   tplane.Branch("isEndcap",&isEndcap  ,"isEndcap/I");
   tplane.Branch("layer",&layer     ,"layer/I");
   tplane.Branch("pdisk",&pdisk     ,"pdisk/I");
   tplane.Branch("nphi",&nphi      ,"nphi/I");
   tplane.Branch("neta",&neta      ,"neta/I");
   tplane.Branch("nfi",&nfi       ,"nfi/I");
   tplane.Branch("ncott",&ncott     ,"ncott/I");
   tplane.Branch("ncurv",&ncurv     ,"ncurv/I");
   tplane.Branch("nste",&nste      ,"nste/I");
   tplane.Branch("ndimension",&ndimension,"ndimension/I");
   // m_map
   tplane.Branch("mapSection",&section,"mapSection/I");
   tplane.Branch("mapPlane",&plane    ,"mapPlane/I");
   tplane.Branch("mapPlaneType",&i1     ,"mapPlaneType/I");
   tplane.Branch("mapEC",&i2          ,"EC/I");
   tplane.Branch("mapItype",&i3       ,"Itype/I");
   // filling
   //printf("%d planes\n",m_nplanes);
   for (int j = 0; j < m_nplanes; j++) {
      //printf("\tPlane %d has %d sections\n",j,m_nsections[j]);
      for (int k = 0; k < m_nsections[j]; k++) {
	 //printf("\t\tSection %d: \n",k);
	 // m_planes
	 iplane = j;
	 isection = k;
	 isEndcap  = m_planes[j][k].getIsEndcap();
	 layer     = m_planes[j][k].getLayer();
	 pdisk     = m_planes[j][k].getPDisk();
	 nphi      = m_planes[j][k].getNumPhi();
	 neta      = m_planes[j][k].getNumEta();
	 nfi       = m_planes[j][k].getNumFi();
	 ncott     = m_planes[j][k].getNumCott();
	 ncurv     = m_planes[j][k].getNumCurv();
	 nste      = m_planes[j][k].getNSte();
	 ndimension= m_planes[j][k].getNDimension();
	 // m_map
	 plane   = m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getPlane();
	 section = m_map[m_planes[j][k].getNSte() == -1][m_planes[j][k].getIsEndcap()][m_planes[j][k].getLayer()].getSection();
	 i1 = m_planes[j][k].getNSte() == -1;
	 i2 = m_planes[j][k].getIsEndcap();
	 i3 = m_planes[j][k].getLayer();
	 // TString objname = Form("plane_%d_%d",j,k);
	 // cout<<" * Writing m_planes: "<<objname<<endl;
	 //	 pmapdir->WriteObject(&m_planes[j][k],objname);
	 tplane.Fill();
      }
   } 

   
   //cout<<"Writing plane constants."<<endl;
   TTree tree("PlaneConstants","Plane constants");
   int isZ, planePair, planeType, idim_0, idim_1, nsections;
   tree.Branch("isZ",&isZ,"isZ/I");
   tree.Branch("planePair",&planePair,"planePair/I");
   tree.Branch("planeType",&planeType,"planeType/I");
   tree.Branch("idim_0",&idim_0,"idim_0/I"); 
   tree.Branch("idim_1",&idim_1,"idim_1/I");
   tree.Branch("nsections",&nsections,"nsections/I");
   for ( int i = 0 ; i<m_nplanes ; i++ ) {
      isZ = m_isZ[i];
      planePair = m_planePair[i];
      planeType = m_planeType[i];
      idim_0 =  m_idim[0][i];
      idim_1 = m_idim[1][i];
      nsections = m_nsections[i];
      tree.Fill();
   }
   
   //cout<<"Writing constants."<<endl;
   int chksum = CalcChecksum();

   TTree tConst("Constants","Constants");
   tConst.Branch("nplanes",&m_nplanes,"nplanes/I");
   tConst.Branch("rlayers",&m_rlayers,"rlayers/I");
   tConst.Branch("totaldim",&m_totaldim,"totaldim/I");
   tConst.Branch("enablePlanePair",&m_enablePlanePair,"enablePlanePair/O");
   tConst.Branch("Checksum",&chksum,"chksum/I");
   tConst.Fill();

   pmapdir->Write();

   // goback
   thisdir->cd();
}


FTKPlaneMap::~FTKPlaneMap()
{
  delete [] m_planeType;
  delete [] m_isZ;
  delete [] m_planePair;
  delete [] m_idim[0];
  delete [] m_idim[1];
  delete [] m_nsections;
  delete [] m_map[PIXEL][BARREL];
  delete [] m_map[PIXEL][ENDCAP];
  delete [] m_map[SCTtrk][BARREL];
  delete [] m_map[SCT][BARREL];
  delete [] m_map[SCT][ENDCAP];

  for (int ip=0;ip!=m_nplanes;++ip) delete [] m_planes[ip];
  delete [] m_planes;
}


int FTKPlaneMap::getSCTtrkPlane() const {
  for(int pl=m_nplanes-1; pl>=0; pl--) {
    if(isSCTtrk(pl)) return pl;
  }
  return -1;
}
