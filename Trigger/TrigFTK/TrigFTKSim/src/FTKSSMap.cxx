/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKSplitEncoder.h"
#include "TrigFTKSim/ftkdefs.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <cassert>
using namespace std;

// These limits are not needed if FTK_AMBank.h uses SS_LOOKUP_MAP option
#define SS_LOOKUP_SIZE 500000 // was 360000
#define SS_MIN_SCT_PHI 15
#define SS_MIN_PIX_PHI 10
#define SS_MIN_PIX_ETA 10  // was 15

FTKSSMap::FTKSSMap() :
  m_rmap(0), m_ssm(0), m_fraction(0), m_bound_checks(false)
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
  : m_rmap(rmap), m_ssm(0), m_fraction(0), m_bound_checks(BoundChecks)
{
   // input stream
   ifstream fin(fname);
   if (!fin) {
      FTKSetup::PrintMessage(ftk::sevr,"Error opening SS file");
   }

   // read and verify the version
   string key;
   int version;
   fin >> key >> version;
   if (key!="Version" || version!=9) {
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
      }
   }


   // loop to read all the elements
   int nrlayers = pmap->getRLayers();
   for (int ilayer=0;ilayer<nrlayers;++ilayer) {
      int type, isEC, layer;
      int phiss, phiwidth, etass, etawidth;
      int fiss, fiwidth, cottss, cottwidth, curvss, curvwidth; // cy addition
      string line;
      getline(fin,line);
      if (line.size()==0 || line[0] == '!') {
	ilayer -= 1;
	continue;
      }
      cout << line << endl;
      istringstream sline(line);
      
      // read the element
      sline >> type >> isEC >> layer;
      
      // cy addition to get correct variables
      if (type!=2)
	sline >> phiss >> phiwidth >> etass >> etawidth;
      else
	sline >> fiss >> fiwidth >> cottss >> cottwidth >> curvss >> curvwidth;
      
      if (!sline) {
	if (type!=1) { // cy change if scttrk or sct, etass means means nothing
	  etass = -1;
	  etawidth = -1;
	}
        else if (FTKSetup::getFTKSetup().getIBLMode()==1 && type==1 && layer==0 && isEC==0){ // for ibl cy
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
	  int totMod = FTKSetup::getFTKSetup().getIBLMode()==1 ? rmap->getRegionMapItem(-1,plane,section).getTotMod()+3 : rmap->getRegionMapItem(-1,plane,section).getTotMod(); // cy ibl +3 addition
          //int totMod = rmap->getRegionMapItem(-1,plane,section).getTotMod(); // cy ibl +3 addition
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
	  }
	  printf("SS info. : %d.%d (%d %d-%d) %d %d\n",
	       plane,section,type,isEC,layer, m_ssm[plane][section][0].m_phiss, m_ssm[plane][section][0].m_phioffset);
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
	  printf("SS info. : %d.%d (%d %d-%d) %d %d %d %d %d %d\n",
		 plane,section,type,isEC,layer, m_ssm[plane][section][0].m_fiss, m_ssm[plane][section][0].m_fioffset, m_ssm[plane][section][0].m_cottss, m_ssm[plane][section][0].m_cottoffset, m_ssm[plane][section][0].m_curvss, m_ssm[plane][section][0].m_curvoffset);
	}
      
   }  // end loop to read all the elements
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

    int phioff = FTKSSMap::getPhiOffset(false); // hardcoded in FTKSSMap.h (default = 200 for pixels)
    // phi piece:
    ss = static_cast<int>(floor( (m_ssm[iplane][isection][iphi].m_phioffset+x+offset)/m_ssm[iplane][isection][iphi].m_phiss )*phioff);
    if(x<0 || x>=m_ssm[iplane][isection][iphi].m_phiwidth) {
      if (!quiet) {
        FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module x (%f) exceeds declared module bounds (pixel phi) (%f): plane=%d,section=%d,iphi=%d",
            x,(m_ssm[iplane][isection][iphi].m_phiwidth),iplane,isection,iphi);
        return -1;
      }
    }
    
    // Recall: SS_LOOKUP_SIZE = 360k
    // Min ss width is found via: 52*(328+width)/width*200 = 360k  ==> min pixel-phi ss is 10
    // for ibl we want 336, so 52*(336+width)/width*200 = 500k ==> min pixel-phi ss is ok 10 works

    // eta piece (for the barrel) - note that section=0
    if(ieta<20) {
      ssadd = static_cast<int>(floor((m_ssm[iplane][isection][ieta].m_etaoffset+y)/m_ssm[iplane][isection][ieta].m_etass));
      if(y<0 || y>=m_ssm[iplane][isection][ieta].m_etawidth) {
        if (!quiet) {
          FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module y (%f) exceeds declared module bounds (pixel eta barrel) (%d), iplane=%d, isection=%d, iphi=%d",
              y,m_ssm[iplane][isection][ieta].m_etawidth,iplane,isection,iphi);
          return -1;
        }
      }
    }
    // eta piece (for the endcap)
    else {
      ssadd = static_cast<int>(floor((m_ssm[iplane][isection][14+3*aside+isection].m_etaoffset+y)/m_ssm[iplane][isection][ieta].m_etass));
      if(y<0 || y>=m_ssm[iplane][isection][14+3*aside+isection].m_etawidth) {
        if (!quiet) {
          FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module y (%f) exceeds declared module bounds (pixel eta endcap) (%f)\n",
              y,m_ssm[iplane][isection][14+3*aside+isection].m_etawidth);
          return -1;
        }
      }
    }
    // Min ss width is found via: 144*19/width = 200  ==> min pixel-eta ss is 15
    
    // check that the eta-piece doesn't exceed the allocated limit of 200
    if(ssadd>=phioff) {
      if (!quiet) {
        cout << "Plane " << iplane << " | Section " << isection \
             << " | Phi " << iphi << " | Eta " << ieta \
             << " | X " << x << " => ss " << ss << " | ssadd " << ssadd << endl;
        FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ssadd exceeds the limit of %d",phioff);
      }
    }
    ss+=ssadd;
  }
  
  // SCT - using original Corrinne's encoding
  else if (hit.getDim() == 1) {
    int phioff = FTKSSMap::getPhiOffset(true); // hardcoded in FTKSSMap.h (default = 100 for SCT)
    ss = static_cast<int>(floor( (m_ssm[iplane][isection][iphi].m_phioffset+x+offset)/m_ssm[iplane][isection][iphi].m_phiss )*phioff+(ieta));
    if(x<0 || x>=m_ssm[iplane][isection][iphi].m_phiwidth) {
      if (!quiet) {
        cout << "Plane " << iplane << " | Section " << isection << " | Phi " << iphi << " | Eta " << ieta << " | X " << x << " => ss " << ss << endl;
        cout << "ss width = " << m_ssm[iplane][isection][iphi].m_phiss << ", offset = " << m_ssm[iplane][isection][iphi].m_phioffset << endl;
        //printf("hit info: dPhi=%f\n",hit.getDeltaPhi()
      }
      if (cmDebug > 4 )
        FTKSetup::PrintMessageFmt(ftk::warn,"ss_hit: local module x (%f) exceeds declared module bounds (SCT phi) (%d)",
				  x,m_ssm[iplane][isection][iphi].m_phiwidth);
      return -1;
    }
    if(ieta>=phioff) {
      if (!quiet)
        FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ieta part of ss definition (%d) exceeds phi offset (SCT eta) (%d)",
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
        FTKSetup::PrintMessage(ftk::sevr,"ss_hit: ss is negative or too large");
        cout << "Plane " << iplane << " | Section " << isection << " | Phi " << iphi << " | Eta " << ieta << " | X " << x << " => " << ss << endl;
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


int FTKSSMap::getSSTower(const FTKHit &hit, unsigned int TowerID) const {
  int ss(-1);

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

  unsigned int localID = m_rmap->getLocalModuleID(hit,TowerID);

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
      FTKSetup::PrintMessage(ftk::sevr,"ss_hit: ss is negative or too large");
    }
  }

  return ss;
}

/* this method decode a SS ID used in the Tower format 
   as input it needs a SSID, the tower id and the logical layer ID.
   The method can 
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
			     

/** This method returns, for Pixel layers, the SS number passing the
    module's information and the cluster coordinate */
int FTKSSMap::getSS(const int &plane, const int &section, 
		    const int &phimod, const int &etaoff,
		    const double &x, const double &y) const
{
  int phioff = FTKSSMap::getPhiOffset(false); // hardcoded in FTKSSMap.h (default = 100 for SCT)
  
  int ss( static_cast<int>(floor( (m_ssm[plane][section][phimod].m_phioffset+x)/m_ssm[plane][section][phimod].m_phiss )*phioff) );
  
  // Recall: SS_LOOKUP_SIZE = 360k
  // Min ss width is found via: 52*(328+width)/width*200 = 360k  ==> min pixel-phi ss is 10
  // for ibl we want 336, so 52*(336+width)/width*200 = 500k ==> min pixel-phi ss is ok 10 works

  int aside = 0;
  int ssadd(0);
  // eta piece (for the barrel) - note that section=0
  if(etaoff<20) {
    ssadd = static_cast<int>(floor((m_ssm[plane][section][etaoff].m_etaoffset+y)/m_ssm[plane][section][etaoff].m_etass));
  }
  // eta piece (for the endcap)
  else {
    ssadd = static_cast<int>(floor((m_ssm[plane][section][14+3*aside+section].m_etaoffset+y)/m_ssm[plane][section][etaoff].m_etass));
  }
  // Min ss width is found via: 144*19/width = 200  ==> min pixel-eta ss is 15

  // check that the eta-piece doesn't exceed the allocated limit of 200
  if(ssadd>=phioff) {
    cout << "Plane " << plane << " | Section " << section \
      << " | Phi " << phimod << " | Eta " << etaoff \
      << " | X " << x << " => ss " << ss << " | ssadd " << ssadd << endl;
    FTKSetup::PrintMessageFmt(ftk::sevr,"ss_hit: ssadd exceeds the limit of %d",phioff);
  }
  ss+=ssadd;

  return ss;
}

/** This method returns, for SCT layers, the SS number passing the
    module's information and the cluster coordinate */
int FTKSSMap::getSS(const int &plane, const int &section, 
		     const int &phimod, const int &etaoff,
		     const double &x) const
{
  int phioff = FTKSSMap::getPhiOffset(true); // hardcoded in FTKSSMap.h (default = 100 for SCT)
  
  int ss( static_cast<int>(floor( (m_ssm[plane][section][phimod].m_phioffset+x)/m_ssm[plane][section][phimod].m_phiss )*phioff+(etaoff)) );
  
  return ss;
}


/** this method get as input a valid SS for this map and return,
    in the reference variables: eta offset, phioffset and phi
    module id. These variables can be used to encode an hit in a
    different SS map.
    SCT version, with just a one-dimensional local SS id */
void FTKSSMap::decodeSS(int SSid, const int &plane, const int &section,
			int &phioff, int &phimod,
			int &localX, int &etaoff)
{
 
  /* start to decompose the information codified in the ID */

  // extract eta offset
  etaoff = SSid%FTKSSMap::getPhiOffset(true);

  // reduce the information to the phi information
  SSid /= FTKSSMap::getPhiOffset(true);
  
  // evaluate the module width in SS
  const int& phiwidth = m_ssm[plane][section][0].m_phiwidth;
  const int& phiss = m_ssm[plane][section][0].m_phiss;
  int SSwidth = (phiwidth-phiwidth%phiss+phiss)/phiss;

  // extract the phi ID of the module
  phimod = SSid/SSwidth;
  // use the SSwidth to calculate the phi offset for the given module
  phioff = phimod*SSwidth;
  // return the lower edge of the strip
  localX = (SSid-phioff)*phiss;
}


/** this method get as input a valid SS for this map and return,
    in the reference variables: eta offset, phioffset and phi
    module id. These variables can be used to encode an hit in a
    different SS map 
    PIXEL version, use two integers to return the local SS position */
void FTKSSMap::decodeSS(int SSid, const int &plane, const int &section,
			int &phioff, int &phimod, int &localX,
			int &etaoff, int &etamod, int &localY)
{
 
  /* start to decompose the information codified in the ID */

  // extract eta offset
  int SSid_y = SSid%FTKSSMap::getPhiOffset(false);

  // reduce the information to the phi information
  int SSid_x = SSid/FTKSSMap::getPhiOffset(false);


  const int& phiwidth = m_ssm[plane][section][0].m_phiwidth;
  const int& phiss = m_ssm[plane][section][0].m_phiss;
  const int& etawidth = m_ssm[plane][section][0].m_etawidth;
  const int& etass = m_ssm[plane][section][0].m_etass;

  // evaluate again the module sizes in SS unit
  int SSwidth_x = (phiwidth-phiwidth%phiss+phiss)/phiss;
  int SSwidth_y = (etawidth-etawidth%etass+etass)/etass;
  
  // extract the phi ID of the module
  phimod = SSid_x/SSwidth_x;
  // use the SSwidth to calculate the phi offset for the given module
  phioff = phimod*SSwidth_x;
  // return the local ID
  localX = (SSid_x-phioff)*phiss;

  // extract the phi ID of the module
  etamod = SSid_y/SSwidth_y;
  // use the SSwidth to calculate the phi offset for the given module
  etaoff = etamod*SSwidth_y;
  // return the local ID
  localY = (SSid_y-etaoff)*etass;
}
