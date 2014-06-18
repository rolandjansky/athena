/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRegionMap.h"


#include <cstdlib>
#include <string>
#include <cstdio>
#include <iostream>
using namespace std;
using namespace ftk;

FTKRegionMapItem::FTKRegionMapItem() :
  m_phi_min(0), m_phi_max(0), m_phi_tot(0),
  m_eta_min(0), m_eta_max(0), m_eta_tot(0)
{
  // nothing to do here
}


FTKRegionMap::FTKRegionMap() :
  m_isok(false), m_old_format(false), m_nregions(0), m_nregions_phi(0)
{
  // nothing to do
}


/** create a regions' map file from an ASCII file describing it */
FTKRegionMap::FTKRegionMap(FTKPlaneMap *pmap, const char *path) :
  m_isok(false), m_old_format(false), m_path(path), m_pmap(pmap), m_nregions(0), m_nregions_phi(0)
{

  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  FILE *infile = fopen(path,"r");

  // cmDebug was a variable used in the old version, here
  // redfined as a boolean true if the FTK verbosity level 
  // is greater than 1
  bool cmDebug = ftkset.getVerbosity()>1 ? true : false;

  // to fix a macro definition
  const int MAXPLANES(20);

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
    if(region != i)
      FTKSetup::PrintMessage(sevr, "readRegionMap: syntax error in rmap file");
    for(j = 0; j < m_pmap->getRLayers(); ++j) {
      if (cmDebug > 3) 
	printf("i = %d, j = %d, m_pmap->rlayers = %d\n",i,j,m_pmap->getRLayers());
      fgets(line,100,infile);
      if (cmDebug) FTKSetup::PrintMessageFmt(debg,"read %s\n",line);
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

      if((plane = m_pmap->getMap(type,isEC,layer).getPlane()) > -1) {
	section = m_pmap->getMap(type,isEC,layer).getSection();
	m_map[i][plane][section].setPhiMin(phi_min);
	m_map[i][plane][section].setPhiMax(phi_max);
	m_map[i][plane][section].setPhiTot(phi_tot);
	m_map[i][plane][section].setEtaMin(eta_min);
	m_map[i][plane][section].setEtaMax(eta_max);
	m_map[i][plane][section].setEtaTot(eta_tot);
	if(cmDebug>3)  printf("map[reg=%d,plane=%d,sec=%d] = (%d/%d/%d:%d/%d/%d)\n",
			      i,plane,section,phi_min,phi_max,phi_tot,eta_min,eta_max,eta_tot);
      }
    }
    if(cmDebug>3) printf("\n");
  } // end region loop
  if (cmDebug) FTKSetup::PrintMessage(debg,"\t\t...done\n");

  m_nplanes = m_pmap->getNPlanes();

  if ((m_sections = (int *)calloc(MAXPLANES,sizeof(int))) == NULL)
    FTKSetup::PrintMessage(sevr,"m_pmap_rd: Allocation of rmap sections failed.");
  for (j = 0; j < MAXPLANES; j++) m_sections[j] = m_pmap->getNSections(j);

  if (cmDebug) FTKSetup::PrintMessage(debg,"Done with readRegionMap\n\n");
  
  m_isok = true;
}

// new version that supports 64-tower rmap geometry
bool FTKRegionMap::isHitInRegion(const FTKHit &hit,int ID) const {
  // if old format, just use the 8-region legacy function:
  if(m_old_format) return isHitInRegion_old(hit,ID);

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
  if(cside && reta==0) res_eta=true;
  if(aside && reta==3) res_eta=true;
  if(!(aside||cside)) { // barrel
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
FTKRegionMapItem &FTKRegionMap::getRegionMapItem(int ireg, int ipl, int isec) const
{
   if (ireg<0) {
      ireg = m_nregions+ireg;
   }

   if (ipl<0) {
      ipl = m_nplanes+ipl;
   }

   return m_map[ireg][ipl][isec];
}


/** return the local module ID for a module, assuming a tower index */
unsigned int FTKRegionMap::getLocalModuleID(const FTKHit &hit, unsigned int TowerID)
{
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
  const int &phimin = m_map[TowerID][plane][section].getPhiMin();
  int phimax = m_map[TowerID][plane][section].getPhiMax();
  //const int &etamin = m_map[TowerID][plane][section].getEtaMin();
  //const int &etamax = m_map[TowerID][plane][section].getEtaMax();

  // adjust the phi boundaries
  int nphi = phimax-phimin+1;
  if (nphi<=0) nphi += m_map[TowerID][plane][section].getPhiTot();

 
  modphi = localID%nphi+phimin;
  if (modphi>=phimax) modphi-=phimax;
  modeta = localID/nphi+phimax;
}
