/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKHit.h"
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
    int ndcx(0), ndcy(0);
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
      
    if( version>=10 ) {
      sline >> ndcx;
      if( type==1 ) { // pixel
        sline >> ndcy;
      };
      if (!sline) { // problem in the format
          FTKSetup::PrintMessage(ftk::warn,"Expected DC width in the format, DC=0 forced");
          ndcx = ndcy = 0;
      }
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
  
  
  // setup hardware-specific parameters. fix these here. they are
  // variables to make it easier to change them.
  if( true ) {
    // hardware representation
    set_nbitsssid(15);
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
      if ( localID==-1 )
              throw FTKException("Error in the DC configuration");
      return compressed_ssid_word(hit,localID);
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


unsigned int
FTKSSMap::gray_code(unsigned int n, int s) const
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
          ssid = compressed_ssid_word_pixel(tmp,0);//localModuleID=0
        }
      } else {
        // SCT
        if( ix<this_map.m_phiwidth ) {
          FTKHit tmp(1);
          tmp[0] = ix;
          tmp.setPlane(plane);
          tmp.setSector(0);
          ssid = compressed_ssid_word_strip(tmp,0);//localModuleID=0
        }
      }
      cout << " ix=" << ix << " iy=" << iy << " ilut=" << lut_offset << " ssid=" << ssid << endl;
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
  int nx = this_map.m_phiwidth;
  int ny = type ? this_map.m_etawidth : 1;
  cout << " ssmap input address size: pl=" << plane << " sec: " << section << " tower: " << towerid << " nx: " << nx << " ny: " << ny << " size: " << (nx*ny) << endl;
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
FTKSSMap::compressed_ssid_word(const FTKHit &hit, unsigned int localID) const
{
  //const FTKSetup &ftkset = FTKSetup::getFTKSetup();
  //int debug = ftkset.getVerbosity();
  if( hit.getDim()==2 ) { return compressed_ssid_word_pixel(hit, localID); }
  else if( hit.getDim()==1 ) { return compressed_ssid_word_strip(hit,localID); }
  else {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Hit with dim %d not supported",hit.getDim());
  }
  return 0ul;
}

unsigned int
FTKSSMap::compressed_ssid_word_pixel(const FTKHit &hit, unsigned int local_module_id) const
{
  //int plane = hit.getPlane();
  //int section = 0; // hit.getSection() is in principle irrelevant, because the ssmaps for each module are identical.
  //TODO this part can be pre-calculated at the initialization
  int phi_dc_bits = getSSDCX(hit);
  int eta_dc_bits = getSSDCY(hit);
  const unsigned int total_localssid_bits = m_nbitsssid-m_nbitsssid_module_pixel;
  const unsigned int total_nondc_bits = total_localssid_bits-phi_dc_bits-eta_dc_bits;
  const unsigned int nondcbits_x = std::min( (unsigned int)ceil(log(getSSOffsetInternal(hit))/log(2.)) , total_nondc_bits );
  const unsigned int nondcbits_y = total_nondc_bits - nondcbits_x;
  //TODO hit[?] can be a fraction of strip, the result is 99% not sensitive to that but HW-compatibility has to be checked
  int x = (int)hit[0];
  int y = (int)hit[1];
  // full-precision representation of coordinate / ss size
  int phi_ss_width = getSSPhiWidth(hit);
  int eta_ss_width = getSSEtaWidth(hit);
  x /= phi_ss_width; // (12-3=9 bits / size)
  y /= eta_ss_width; // (11-3=8 bits / size)
  // dc pieces
  unsigned int dc_x = x & (phi_dc_bits>0 ? (1<<phi_dc_bits)-1 : 0 );
  unsigned int dc_y = y & (eta_dc_bits>0 ? (1<<eta_dc_bits)-1 : 0 );
  // Alberto Annovi: gray code to be enabled below
  //dc_x = gray_code(dc_x, VALUE);
  //dc_y = gray_code(dc_y, VALUE);
  // unsigned int dc = (dc_y << phi_dc_bits) | dc_x;
  // non-dc piece. current compression scheme just drops higher order bits.
  // relative to uncompressed superstrip ids, this effectively
  // multiplexes the superstrips. the ideal choice of multiplexed
  // ssids will lead to the fewest false positive patterns.
  unsigned int nondc_x = (x >> phi_dc_bits) & ((1<<nondcbits_x)-1);
  unsigned int nondc_y = (y >> eta_dc_bits) & ((1<<nondcbits_y)-1);
  unsigned int nondc = (nondc_y << nondcbits_x) | nondc_x;
  if  ((nondc & ((1<<total_nondc_bits)-1)) != nondc )
		throw FTKException("Error in the DC configuration");
  // local ssid
  const unsigned int max_compressed_ssid_phi_dc = std::min( 1<<phi_dc_bits , getSSOffsetInternal(hit) );
  const unsigned int max_compressed_ssid_etaphi_dc = std::min( 1<<eta_dc_bits<<phi_dc_bits , getSSOffset(hit) );
  const unsigned int max_nondc_lx = std::min( 1<<nondcbits_x, getSSOffsetInternal(hit) >> phi_dc_bits);
  const unsigned int max_nondc_lyx = std::min( 1<<total_nondc_bits, getSSOffset(hit) >> phi_dc_bits >> eta_dc_bits );
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
  if ( (result & result_mask) != result )
	  throw FTKException("Error in the DC configuration");
  return result;
}


unsigned int
FTKSSMap::compressed_ssid_word_strip(const FTKHit &hit, unsigned int local_module_id) const
{
  //int plane = hit.getPlane();
  //int section = 0; // hit.getSection() is in principle irrelevant, because the ssmaps for each module are identical.
  int phi_dc_bits = getSSDCX(hit);
  const unsigned int total_localssid_bits = m_nbitsssid-m_nbitsssid_module_strip;
  const unsigned int total_nondc_bits = total_localssid_bits-phi_dc_bits;
  const unsigned int nondcbits_x = total_nondc_bits;
  // full-precision representation of coordinate / ss size (11-1=10 bits / size)
  int x = (int)hit[0];
  int phi_ss_width = getSSPhiWidth(hit);
  x /= phi_ss_width;
  // dc piece.
  unsigned int dc_x = x & ((1<<phi_dc_bits)-1);
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
  unsigned int max_compressed_ssid = std::min((1<<total_localssid_bits)-1,getSSOffset(hit));
  // the above needs to be a multiple of 2^(total dc bits)
  max_compressed_ssid = (int)ceil(max_compressed_ssid/(1.*(1<<phi_dc_bits)))*(1<<phi_dc_bits);
  const unsigned int ssPerModule = max_compressed_ssid;
  const unsigned int offset = (local_module_id*ssPerModule) & ((1<<m_nbitsssid)-1);
  const unsigned int result = offset + local_ssid;
  assert( (result & ((1<<m_nbitsssid)-1)) == result );
  return result;
}

