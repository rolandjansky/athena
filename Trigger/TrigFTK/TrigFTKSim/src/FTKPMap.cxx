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
  m_neta(-1), m_nste(0), m_ndimension(0)
{
   // nothing to do here
}


FTKPlaneMap::FTKPlaneMap(const char *fname) :
  m_path(fname), m_nplanes(0), m_rlayers(0), m_totaldim(0), 
  m_enablePlanePair(false),  m_isok(false)
{
   FTKSetup &ftkset = FTKSetup::getFTKSetup();


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

  const int MAXPLANES(20);

  // store the maximum ID of an unused plane, 
  // needed to size the m_planes_unused
  int max_plane_id(-1);

  int used_plane_dim[MAXPLANES];
  int used_plane_sections[MAXPLANES];
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
  for (int j=0; j < MAXPLANES; j++) {
    used_plane_dim[j] = 0;
    used_plane_sections[j] = 0;
  }

  /* check for correct file format */
  if (fgets(line,100,fpmap)!=NULL) {
    if (cmDebug) printf("%s\n",line);
    sscanf(line,"%d %s %s",&npixel,buf1,buf2);
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
      if (plane < MAXPLANES) {
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
      if (plane < MAXPLANES) {
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

  for (int j = 0; j < MAXPLANES; j++) {
    if (used_plane_dim[j]) {
      m_totaldim += used_plane_dim[j];
    }
  }


  // allocate the array to store the number of used sections
  m_nsections = new int[MAXPLANES];
  if ((m_nsections = (int *)calloc(MAXPLANES,sizeof(int))) == NULL)
    FTKSetup::PrintMessage(sevr,"pmap_rd: Allocation of pmap nsections failed.");
  for (int j = 0; j < MAXPLANES; j++) {
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

  m_isZ =  new int[m_totaldim];
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

  for (int l = 0; l < MAXPLANES; l++) {
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
	    if (cmDebug > 4) {
	      printf("\t\tisZ[j+l] = l:    isZ[%d+%d] = %d\n",jcoord,l,l);
	      printf("\t\tidim[l][k] = j+l idim[%d][%d] = %d\n",l,kcoord,jcoord+l);
	    }
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
	    if (cmDebug > 4) {
	      printf("isZ[j+l] = l:    isZ[%d+%d] = %d\n",jcoord,l,l);
	      printf("idim[l][k] = j+l idim[%d][%d] = %d\n",l,kcoord,jcoord+l);
	    }
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
    printf("\nDumping pmap\n");
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
    FTKSetup::PrintMessage(debg,"\n\n");
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
  m_isok = true;
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
