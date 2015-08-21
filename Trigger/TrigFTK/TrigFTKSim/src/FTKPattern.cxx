/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKPattern.h"

#include <iostream>
using namespace std;

ClassImp(FTKPattern)

/** Defining the static property used to manage the patterns 
    in different scenarios */
bool FTKPattern::m_orderByCoverage = false;

/** default constructor, used by the ROOT streamer */
FTKPattern::FTKPattern() :
  TObject(),
  m_patternID(-1), m_patternDBID(0), m_nplanes(0), m_ssid(0x0),
  m_sectorID(-1), m_coverage(0),
  m_dcmask(-1), m_hbmask(-1),
  m_nsubpatterns(0),
  m_hashes(0)
{
  m_subpatterns = new TClonesArray("FTKPattern",1);
}


/** This is the preferred constructor to build an FTKPattern object.
    The only required argument is the number of planes used by
    this pattern */
FTKPattern::FTKPattern(int nplanes) :
  TObject(),
  m_patternID(-1), m_patternDBID(0), m_nplanes(0),m_ssid(0x0),
  m_sectorID(-1), m_coverage(0),
  m_dcmask(-1), m_hbmask(-1),
  m_nsubpatterns(0),
  m_hashes(0)
{
  m_subpatterns = new TClonesArray("FTKPattern",1);
  // set the number of planes
  setNPlanes(nplanes);
}


FTKPattern::FTKPattern(const FTKPattern &tocpy) :
  TObject(tocpy),
  m_patternID(tocpy.m_patternID),
  m_nplanes(0),
  m_sectorID(tocpy.m_sectorID),
  m_coverage(tocpy.m_coverage),
  m_nsubpatterns(tocpy.m_nsubpatterns)
{
  // copy the number of planes and the SS ids
  setNPlanes(tocpy.m_nplanes);
  for (int ip=0;ip!=m_nplanes;++ip)
    m_ssid[ip]= tocpy.m_ssid[ip];
  m_subpatterns = new TClonesArray(*tocpy.m_subpatterns);
}


/** destructor for the class */
FTKPattern::~FTKPattern()
{
  // delete SS ids
  if (m_nplanes>0) delete [] m_ssid;
  delete m_subpatterns;

}


/** set the numbet of the planes and sizes the SS id array */
void FTKPattern::setNPlanes(int np)
{
  // delete old array
  if (m_nplanes>0) delete [] m_ssid;

  // set new number of planes and zeroes the values
  m_nplanes = np;
  if (np>0) m_ssid = new int[np];
  for (int i=0;i!=np;++i) m_ssid[i] = -1;
}


/** add a child pattern in the children list */
FTKPattern* FTKPattern::addSubPattern(FTKPattern &pattern)
{
  return new ((*m_subpatterns)[m_nsubpatterns++]) FTKPattern(pattern);
}


/** Clear the sub-pattern list */
void FTKPattern::clearSubPatterns()
{
  m_nsubpatterns = 0;
  m_subpatterns->Delete();
}

istream& operator>>(istream &input,FTKPattern &patt)
{
  input >> patt.m_patternID;

  for (int iplane=0;iplane!=patt.m_nplanes;++iplane) { // loop on the planes
    // the m_nplnes+1 element is the number of the sector
    // the other are the SS index
    input >> patt.m_ssid[iplane];
  } // end loop on the planes

  input >> patt.m_sectorID;

  // number of events not used, temporary in npatt
  input >> patt.m_coverage;

  return input;
}



ostream& operator<<(ostream &output,FTKPattern &patt)
{
  output << patt.m_patternID;

  for (int iplane=0;iplane!=patt.m_nplanes;++iplane) { // loop on the planes
    // the m_nplnes+1 element is the number of the sector
    // the other are the SS index
    output << ' ' << patt.m_ssid[iplane];
  } // end loop on the planes

  output << ' ' << patt.m_sectorID;

  // number of events not used, temporary in npatt
  output << ' ' << patt.m_coverage;

  return output;
}


bool FTKPattern::operator<(const FTKPattern &tocmp)
{  
  if (m_orderByCoverage) { // used to order the pattern by coverage
    if (m_coverage>tocmp.m_coverage) return true;
    else return false;
  }
  else { // use the ordering according the sector and SS ids
    if(m_sectorID<tocmp.m_sectorID) return true;
    if(m_sectorID>tocmp.m_sectorID) return false;
    for(int i=0; i<m_nplanes; i++) {
      if(m_ssid[i]<tocmp.m_ssid[i]) return true;
      if(m_ssid[i]>tocmp.m_ssid[i]) return false;
    }
    // in case of equality, we choose "false":
    return false;
  }
}

FTKPattern& FTKPattern::operator=(const FTKPattern &tocpy)
{
  m_patternID = tocpy.m_patternID;
  m_sectorID = tocpy.m_sectorID;
  m_coverage= tocpy.m_coverage;
  m_nsubpatterns = tocpy.m_nsubpatterns;
  m_subpatterns = new TClonesArray(*tocpy.m_subpatterns);

  if (m_nplanes!=tocpy.m_nplanes)
    setNPlanes(tocpy.m_nplanes);

  for (int ip=0;ip!=m_nplanes;++ip)
    m_ssid[ip] = tocpy.m_ssid[ip];

  return *this;
}


/** this function evaluate the hash related to this pattern assuming
    a missing plane */
unsigned int FTKPattern::getHash(int plane)
{
  if (!m_hashes) generateHashes();

  return m_hashes[plane];
}



uint32_t FTKPattern::crc32_tab[] = {
     0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
     0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
     0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
     0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
     0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
     0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
     0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
     0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
     0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
     0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
     0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
     0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
     0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
     0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
     0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
     0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
     0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
     0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
     0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
     0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
     0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
     0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
     0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
     0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
     0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
     0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
     0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
     0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
     0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
     0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
     0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
     0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
     0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
     0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
     0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
     0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
     0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
     0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
     0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
     0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
     0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
     0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
     0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


unsigned int
FTKPattern::crc32(const void *buf, size_t size, unsigned int crc)
{
     const uint8_t *p = (const uint8_t*)buf;
         crc = crc ^ ~0U;
     while (size--)
             crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
     return crc ^ ~0U;
}

void FTKPattern::generateHashes()
{
  int bs = sizeof(m_ssid[0]);
  
  if (!m_hashes) m_hashes = new unsigned int[m_nplanes];

  for (int il=0;il<m_nplanes;++il) {
    //TMD5 hash_creator;
    unsigned int crc32_key(0);

    for (int ip=0;ip<m_nplanes;++ip) {
      if (ip!=il)
	//hash_creator.Update((UChar_t*) (m_ss_id+ip),bs);
	crc32_key = crc32((m_ssid+ip),bs,crc32_key);
    }

    //hash_creator.Final();
    //m_hashes[il] = PatternDefinition::MD5toInt(hash_creator.AsString());
    m_hashes[il] = crc32_key;
  }
}


/** overlad the TObject::Print function */
void FTKPattern::Print(const Option_t *) const {
  cout << m_patternID;

  for (int iplane=0;iplane!=m_nplanes;++iplane) { // loop on the planes
    // the m_nplnes+1 element is the number of the sector
    // the other are the SS index
    cout << ' ' << m_ssid[iplane];
  } // end loop on the planes

  cout << ' ' << m_sectorID;

  // number of events not used, temporary in npatt
  cout << ' ' << m_coverage << endl;
}

/** this function dump in an alternative ascii format the pattern content */
void FTKPattern::dump(ostream &sout)
{
  for (int il=0;il<m_nplanes-1;++il) {
    sout << m_ssid[il] << ", ";
  }
  sout << m_ssid[m_nplanes-1];
  sout << " w=" << m_coverage;
  sout << " in " << m_sectorID;
}



/*
 * This method applies a static RW method, that compares two patterns,
 * and decides if they seems similar or not 
 * this method copy just the scalar data members */
bool FTKPattern::doStaticRoadWarrior(const FTKPattern &patt1,
				     const FTKPattern &sec1,
				     const FTKPattern &patt2,
				     const FTKPattern &sec2)
{
  int ndiffs(0); // number of different SS between the two patterns
  int nshared(0); // number of shared roads
  int ndiffs_same(0); // number of shared roads in the same module

  // get the number of planes
  int nplanes = patt1.getNPlanes();
  
  for (int i=0;i<nplanes;++i) { // loop over the planes
    // the two patterns share the same SS, if not count it
    if ( patt1.getSSID(i) == patt2.getSSID(i) ) 
      nshared += 1;
    else if ( sec1.getSSID(i) == sec2.getSSID(i) )
      ndiffs_same += 1;
    else
      ndiffs += 1;
  } // end loop over the planes
  
  if (ndiffs+nshared==nplanes && ndiffs==1)
    return true;
  else
    return false;
}


/** the constructor should know the dimensione of the space */
FTKPatternKDTree::FTKPatternKDTree(int dim, FTKPattern *road, int pos,
			     int depth, FTKPatternKDTree *parent) :
  m_dim(dim), m_depth(depth), m_node(road), m_pos(pos),
  m_parent(parent), m_left(0), m_right(0),
  m_maxdepth(0), m_maxdepth_l(0), m_maxdepth_r(0)
{
  // determine the SS that splits the hyper-space
  m_splitplane = m_depth % m_dim;  
}


/** destructor of the Tree, recurivelly destroy all the level */
FTKPatternKDTree::~FTKPatternKDTree()
{
  if (m_left) delete m_left;
  if (m_right) delete m_right;
  delete m_node;
}


/** find an empty leaf to place this road in the kd-tree*/
FTKPatternKDTree* FTKPatternKDTree::addNode(FTKPattern *newpatt, int pos)
{
  FTKPatternKDTree *newnode(0);

  // check if this pattern is already in the list
  bool samepattern(true);
  for (int ip=0;ip!=m_dim;++ip) {
    if (m_node->getSSID(ip)!=newpatt->getSSID(ip)) {
      samepattern = false;
      break;
    }
  }
  
  // if the new pattern is already in the list, return the pointer to this node
  if (samepattern) {
    getNode()->addCoverage(newpatt->getCoverage());
    delete newpatt;
    return this;
  }

  // determine the direction
  if (newpatt->getSSID(m_splitplane) <= m_node->getSSID(m_splitplane) ) {
    // is the left part the axis
    if (!m_left) {
      // create a new sub-tree
      m_left = new FTKPatternKDTree(m_dim,newpatt,pos,m_depth+1,this);
      newnode = m_left;
    }
    else {
      // continue the search
      newnode = m_left->addNode(newpatt,pos);
    }
  }
  else {
    // is on the right part of the axis
    if (!m_right) {
      // create a new sub-tree
      m_right = new FTKPatternKDTree(m_dim,newpatt,pos,m_depth+1,this);
      newnode = m_right;
    }
    else {
      // continue the search
      newnode = m_right->addNode(newpatt,pos);
    }
  }

  int reldep = newnode->getDepth()-m_depth;
  if (reldep>m_maxdepth) m_maxdepth = reldep;
  if (reldep>m_maxdepth_l) m_maxdepth_l = reldep;
  if (reldep>m_maxdepth_r) m_maxdepth_r = reldep;

  return newnode;
} 


/** this method transform a KD-tree in a linear list of pattern, sorted
    by coverage. After the method the original kd-tree is empty */
int FTKPatternKDTree::makePatternList(list<FTKPattern> &pattlist)
{
  int nadded(1);

  // go in the next levels, delete the nodes
  if (m_right) {
    nadded += m_right->makePatternList(pattlist);
    delete m_right;
    m_right = 0x0;
  }
  if (m_left) {
    nadded += m_left->makePatternList(pattlist);
    delete m_left;
    m_left = 0x0;
  }

  pattlist.push_back(*m_node);

  return nadded;
}
