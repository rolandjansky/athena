/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/tsp/TSPMap.h"

#include <iostream>
#include <cmath>
using namespace std;

/** empty constructor, this doesn't generate a valid TSPMap */
TSPMap::TSPMap() :
    m_amssmap(0x0), m_tspssmap(0x0),
    m_nplanes(0),
    m_nbits(0x0), m_nfactor(0x0), m_internal_nbits(0x0),
    m_bitoffset(0x0), m_internal_bitoffset(0x0),
    m_ndim(0x0), m_npos(0x0),
    m_totbits(0), m_max_nbits(0)
{;}


/** full constructor, this is able to generate a real TSPMap object */
TSPMap::TSPMap(const FTKSSMap *amssmap, const FTKSSMap *tspssmap) :
    m_amssmap(0x0), m_tspssmap(0x0),
    m_nplanes(0),
    m_nbits(0x0), m_nfactor(0x0), m_internal_nbits(0x0),
    m_bitoffset(0x0), m_internal_bitoffset(0x0),
    m_ndim(0x0), m_npos(0x0),
    m_totbits(0), m_max_nbits(0)
{
  generate(amssmap,tspssmap);
}


/** copy constructor */
TSPMap::TSPMap(const TSPMap &cpy) :
   m_amssmap(cpy.m_amssmap),
   m_tspssmap(cpy.m_tspssmap),
   m_nplanes(cpy.m_nplanes),
   m_nbits(0x0), m_nfactor(0x0), m_internal_nbits(0x0),
   m_bitoffset(0x0), m_internal_bitoffset(0x0),
   m_ndim(0x0), m_npos(0x0),
   m_totbits(cpy.m_totbits),
   m_max_nbits(cpy.m_max_nbits),
   m_B2G_table(cpy.m_B2G_table),
   m_G2B_table(cpy.m_G2B_table)
{
  if (cpy.m_nplanes==0) return;

  m_nbits = new int[m_nplanes];
  m_bitoffset = new int[m_nplanes];
  m_nfactor = new int*[m_nplanes];
  m_internal_nbits = new int*[m_nplanes];
  m_internal_bitoffset = new int*[m_nplanes];
  
  for (int ip=0;ip!=m_nplanes;++ip) {
     m_nbits[ip] = cpy.m_nbits[ip];
     m_bitoffset[ip] =cpy.m_bitoffset[ip];
     m_ndim[ip] = cpy.m_ndim[ip];
     if (m_ndim[ip]>1) {
        m_internal_bitoffset[ip] = new int[m_ndim[ip]-1];
        for (int id=0;id!=m_ndim[ip]-1;++id) 
           m_internal_bitoffset[ip][id] = cpy.m_internal_bitoffset[ip][id];
     }
     m_nfactor[ip] = new int[m_ndim[ip]];
     m_internal_nbits[ip] = new int[m_ndim[ip]];
     for (int id=0;id!=m_ndim[ip];++id) {
        m_nfactor[ip][id] = cpy.m_nfactor[ip][id];
        m_internal_nbits[ip][id] = cpy.m_internal_nbits[ip][id];
     }
  }
}


TSPMap::~TSPMap()
{
  if (m_nplanes==0) return;

  for (int ip=0;ip!=m_nplanes;++ip) {
    if (m_internal_bitoffset[ip]) delete [] m_internal_bitoffset[ip];
    delete [] m_internal_nbits[ip];
  }

  delete [] m_nbits;
  delete [] m_internal_nbits;
  delete [] m_bitoffset;
  delete [] m_internal_bitoffset;
  delete [] m_ndim;
  delete [] m_npos;
  delete [] m_nfactor;
}


/** get 2 SS maps to create the bitmask diescription.
    The first map is the AM level, with larger SS, the second
    is the TSP level, finer. */
void TSPMap::generate(const FTKSSMap *amssmap, const FTKSSMap *tspssmap)
{
  m_amssmap = amssmap;
  m_tspssmap = tspssmap;

  // get the plane maps
  const FTKPlaneMap *ampmap = amssmap->getPlaneMap();
  const FTKPlaneMap *tsppmap = tspssmap->getPlaneMap();

  if (ampmap->getNPlanes()!=tsppmap->getNPlanes()) {
    FTKSetup::PrintMessage(ftk::sevr,"SS maps have different number of planes");
  }


  FTKSetup::PrintMessageFmt(ftk::info,"TSP/DC mapping preparation for %d layers\n",ampmap->getNPlanes());

  // set the number of the planes
  m_nplanes = ampmap->getNPlanes();
  // set the arrays
   m_nbits = new int[m_nplanes];
  m_ndim = new int[m_nplanes];
  m_npos = new int[m_nplanes];
  m_nfactor = new int*[m_nplanes];
  m_internal_nbits = new int*[m_nplanes];
  m_internal_bitoffset = new int*[m_nplanes];

  // reset the maximum number of bits counter
  m_max_nbits = 0;

  for (int ip=0;ip!=m_nplanes;++ip) { // planes loop
    int ndim = ampmap->getPlane(ip,0).getNDimension();
    m_ndim[ip] = ndim;

    // dim the internal arrays
    if (ndim>1)
      m_internal_bitoffset[ip] = new int[ndim-1];
    else 
      m_internal_bitoffset[ip] = 0x0; // no internal structure for SCT

    m_nfactor[ip] = new int[ndim];
    m_internal_nbits[ip] = new int[ndim];

    if (ndim==1) { // 1-dim plane
      m_nfactor[ip][0] = amssmap->getMap(ip,0,0).m_phiss/tspssmap->getMap(ip,0,0).m_phiss;
      m_internal_nbits[ip][0] = static_cast<int>(ceil(log2(m_nfactor[ip][0])));
      // check the maximum number of bits
      if (m_internal_nbits[ip][0]>m_max_nbits) 
	m_max_nbits = m_internal_nbits[ip][0];
      m_nbits[ip] = m_internal_nbits[ip][0];
      // each bits identifies 2 positions
      m_npos[ip] = 1<<m_internal_nbits[ip][0];
      FTKSetup::PrintMessageFmt(ftk::info,"TSP/DC mapping layer %d: dim %d, n bits %d, mask pos %d\n",ip,m_nfactor[ip][0],m_nbits[ip],m_npos[ip]);
    }
    else if (ndim==2) { // 2-dim plane
      m_nfactor[ip][0] = amssmap->getMap(ip,0,0).m_phiss/tspssmap->getMap(ip,0,0).m_phiss;
      m_nfactor[ip][1] = amssmap->getMap(ip,0,0).m_etass/tspssmap->getMap(ip,0,0).m_etass;
      m_internal_nbits[ip][0] = static_cast<int>(ceil(log2(m_nfactor[ip][0])));
      // check the maximum number of bits
      if (m_internal_nbits[ip][0]>m_max_nbits) 
	m_max_nbits = m_internal_nbits[ip][0];
      m_internal_nbits[ip][1] = ceil(log2(m_nfactor[ip][1]));
      // check the maximum number of bits
      if (m_internal_nbits[ip][1]>m_max_nbits) 
	m_max_nbits = m_internal_nbits[ip][1];
      m_nbits[ip] = m_internal_nbits[ip][0]+m_internal_nbits[ip][1];
      m_internal_bitoffset[ip][0] = m_internal_nbits[ip][0];

      // the internal SS are a grid of size (nbitsX**2)*(nbitsY**2)
      m_npos[ip] = (1<<m_internal_nbits[ip][0])*(1<<m_internal_nbits[ip][1]);
      FTKSetup::PrintMessageFmt(ftk::info,"TSP/DC mapping layer %d: dim (%d,%d),n bits %d (%d,%d), mask pos %d\n",ip,m_nfactor[ip][0],m_nfactor[ip][1],m_nbits[ip],m_internal_nbits[ip][0],m_internal_nbits[ip][1],m_npos[ip]);
    }
    else { // error
      FTKSetup::PrintMessageFmt(ftk::sevr,"Illegal number of dimension: %d\n",ndim);
    }

    m_totbits += m_nbits[ip]; // count the total number of bits
  } // end planes loop

  m_bitoffset = new int[m_nplanes];
  m_bitoffset[0] = 0;
  cout << "DC mask offsets: " << 0 /* << ends */ ;
  for (int ip=1;ip<m_nplanes;++ip) {
    m_bitoffset[ip] = m_bitoffset[ip-1]+m_nbits[ip-1];
    cout << " - " << m_bitoffset[ip] /* << ends */ ;  
  }
  cout << ". Tot bits: " << m_totbits << endl;

  generateGrayCodeTables();
}


int TSPMap::IEEE2GC(int nbits, int val)
{
  if (nbits==0) {
    assert(val==0);
    return 0;
  }

  return m_B2G_table[nbits-1][val];
}

int TSPMap::GC2IEEE(int nbits, int val)
{
  if (nbits==0) {
    assert(val==0);
    return 0;
  }

  return m_G2B_table[nbits-1][val];
}


/** trasform the position of a plane from the GC value to
    the binary value */
int TSPMap::getPosition(int plane, int val)
{
  // result
  int res;
  int posX, posY;
  switch(m_ndim[plane]) {
  case 2:
    // decompose the 2 coordinates
    posY = val>>m_internal_nbits[plane][0];
    posX = val&(~(~0<<m_internal_nbits[plane][0]));
    // convert the value in Gray code
    posX = GC2IEEE(m_internal_nbits[plane][0],posX);
    posY = GC2IEEE(m_internal_nbits[plane][1],posY);
    res = (posY<<m_internal_nbits[plane][0]) | posX;
    break;
  case 1:
    // direct convertion of the only value
    res = GC2IEEE(m_internal_nbits[plane][0],val);
    break;
  default:
    FTKSetup::PrintMessageFmt(ftk::sevr,"Number of dimensions (%d) not valid", m_ndim[plane]);
    res = 0;
    break;
  }

  return res;
}



/** Generate the tables to converta binary value in Gray code
    and back. The used method is the "mirroring". 
    The methods starts from the 1 bit case, where the to codes
    are the same. Than for the 2 bit case the sequence is doubled
    mirroing the 2 value, the sequence is {0,1}-->{0,1,1,0}, then
    the value mirrored the highe bit is set, obtaining {00,01,11,10}.
    The proceure is iterative and allow to build the table
    for any number of bits.
 */
void TSPMap::generateGrayCodeTables()
{

  // the 1 bit start, used as initialization is always created
  int start[] = {0,1};
  m_B2G_table.push_back(vector<int>(start,start+2));
  m_G2B_table.push_back(vector<int>(start,start+2));

  for (int ibit=2;ibit<=m_max_nbits;++ibit) { // bit loop
    // number of possible values
    int nvalues = 1<<ibit;

    // add the internal vector
    m_B2G_table.push_back(vector<int>(nvalues));
    m_G2B_table.push_back(vector<int>(nvalues));
    
    // values to mirror
    int nvaluesM = 1<<(ibit-1);

    // get the reference to the table to read
    const vector<int> &B2Gprev = m_B2G_table[ibit-2];
    // get the reference to the table to chage
    vector<int> &B2Gcur = m_B2G_table[ibit-1];
    vector<int> &G2Bcur = m_G2B_table[ibit-1];

    for (int iv=0; iv!=nvaluesM; ++iv) { // mirroring loop
      // mirrored position
      int ivM = nvalues-iv-1;

      // write the direct block
      int gval0 = B2Gcur[iv] = B2Gprev[iv];
      // write the mirrored block
      int gval1 = B2Gcur[ivM] = B2Gprev[iv] | 1<<(ibit-1);

      // build also the reverse field
      G2Bcur[gval0] = iv;
      G2Bcur[gval1] = ivM;
    } // end mirrogin loop
  } // end bit loop
}

/** This method evaluate the high resolution part of a SS, in relation
 * to the AM part. The Gray encoding.
 */
int TSPMap::getHighResSSPart(FTKHit const &hit) const {
  // retrieve constant values
  const int iplane(hit.getPlane());
  const int ndim(hit.getDim());

  if (ndim==1) { // SCT case
    // full precision local position
    int ss = static_cast<int>(hit[0]/m_tspssmap->getSSPhiWidth(hit)) % m_nfactor[iplane][0];
    // GC of the position
    return FTKSSMap::gray_code(ss);
  }
  else if (ndim==2) { // Pixel case
    // evaluate the high precision
    int ss_x = static_cast<int>(hit[0]/m_tspssmap->getSSPhiWidth(hit)) % m_nfactor[iplane][0];
    int ss_y = static_cast<int>(hit[1]/m_tspssmap->getSSEtaWidth(hit)) % m_nfactor[iplane][1];
    return (FTKSSMap::gray_code(ss_y) << m_internal_bitoffset[iplane][0]) | FTKSSMap::gray_code(ss_x);
  }

  return -1;
}
