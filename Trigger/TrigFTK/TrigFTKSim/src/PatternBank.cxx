/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/PatternBank.h"

#include <stdint.h>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <string>
#include <set>
#include <TrigFTKSim/ftk_dcap.h>

using namespace std;

const uint32_t crc32_tab[] = {
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
crc32(const void *buf, size_t size, unsigned int crc)
{
     const uint8_t *p = (const uint8_t*)buf;
         crc = crc ^ ~0U;
     while (size--)
             crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
     return crc ^ ~0U;
}

/********************************************************\
 * Class to define the pattern within a pattern-bank.   *
 * this class stores the sequence of SS Ids for a       *
 * given pattern                                        *
\********************************************************/
PatternDefinition::PatternDefinition() : 
  m_id(-1), m_nplanes(0), m_ss_id(0),
  m_sector_id(-1), m_nevents(0), m_hashes(0)
{;}

PatternDefinition::PatternDefinition(int id, int nl, int *ids) :
  m_id(id), m_nplanes(nl), m_sector_id(-1), m_nevents(0),
  m_hashes(0)
{
  m_ss_id = new int[m_nplanes];
  if (ids) {
    for (int i=0;i<m_nplanes;++i) {
      m_ss_id[i] = ids[i];
    }
  }
}

PatternDefinition::PatternDefinition(int id, int nl, int secid, int nevents, int *ids) :
  m_id(id), m_nplanes(nl), m_sector_id(secid), m_nevents(nevents),
  m_hashes(0)
{
  m_ss_id = new int[m_nplanes];
  if (ids) {
    for (int i=0;i<m_nplanes;++i) {
      m_ss_id[i] = ids[i];
    }
  }
}

PatternDefinition::~PatternDefinition()
{
  if (m_ss_id) delete [] m_ss_id;
  if (m_hashes) delete [] m_hashes;
}

void PatternDefinition::setNPlanes(int v) {
  m_nplanes = v;
  delete [] m_ss_id;
  m_ss_id = new int[m_nplanes];
}

PatternDefinition::PatternDefinition(const PatternDefinition &ref)
{
  m_id = ref.m_id;
  m_nplanes = ref.m_nplanes;
  m_ss_id = new int[m_nplanes];
  if (ref.m_hashes) m_hashes = new unsigned int[m_nplanes];
  else m_hashes = 0;
  for (int il=0;il<m_nplanes;++il) {
    m_ss_id[il] = ref.m_ss_id[il];
    if (m_hashes) m_hashes[il] = ref.m_hashes[il];
  }
  m_sector_id = ref.m_sector_id;
  m_nevents = ref.m_nevents;
}


PatternDefinition& PatternDefinition::operator=(const PatternDefinition &ref)
{
#ifdef PROTECT_SA // speedup
  if (this != &ref)
#endif
    {
      m_id = ref.m_id;
      m_nplanes = ref.m_nplanes;
      delete m_ss_id;
      m_ss_id = new int[m_nplanes];
      delete m_hashes;
      if (ref.m_hashes) m_hashes = new unsigned int[m_nplanes];
      else m_hashes = 0;
      for (int il=0;il<m_nplanes;++il) {
	m_ss_id[il] = ref.m_ss_id[il];
	if (m_hashes) m_hashes[il] = ref.m_hashes[il];
      }
      m_sector_id = ref.m_sector_id;
      m_nevents = ref.m_nevents;
    }

  return *this;
}


/** this function evaluate the hash related to this pattern assuming
    a missing plane */
unsigned int PatternDefinition::getHash(int plane)
{
  if (!m_hashes) generateHashes();

  return m_hashes[plane];
}


void PatternDefinition::generateHashes()
{
  int bs = sizeof(m_ss_id[0]);
  
  if (!m_hashes) m_hashes = new unsigned int[m_nplanes];

  for (int il=0;il<m_nplanes;++il) {
    //TMD5 hash_creator;
    unsigned int crc32_key(0);

    for (int ip=0;ip<m_nplanes;++ip) {
      if (ip!=il)
	//hash_creator.Update((UChar_t*) (m_ss_id+ip),bs);
	crc32_key = crc32((m_ss_id+ip),bs,crc32_key);
    }

    //hash_creator.Final();
    //m_hashes[il] = PatternDefinition::MD5toInt(hash_creator.AsString());
    m_hashes[il] = crc32_key;
  }
}

/** this function dump in an ascii format the pattern content */
void PatternDefinition::dump(ostream &sout)
{
  for (int il=0;il<m_nplanes-1;++il) {
    sout << m_ss_id[il] << ", ";
  }
  sout << m_ss_id[m_nplanes-1];
  sout << " w=" << m_nevents;
  sout << " in " << m_sector_id;
}


istream& operator>>(istream &sin, PatternDefinition &road)
{
  
  sin >> road.m_id;
  for (int il=0;il<road.m_nplanes;++il) {
    sin >> road.m_ss_id[il];
  }
  sin >> road.m_sector_id >> road.m_nevents;
  return sin;
}

ostream& operator<<(ostream &sout,const PatternDefinition &road) {
  sout << road.m_id << ' ';
  for (int il=0;il<road.m_nplanes;++il) {
    sout << road.m_ss_id[il] << ' ';
  }
  sout << road.m_sector_id << ' ' << (int)road.m_nevents;

  return sout;
}

void PatternDefinition::Print() {
  cout << *this << endl;
}

/*
 * This method applies a static RW method, that compares two patterns,
 * and decides if they seems similar or not 
 * this method copy just the scalar data members */
bool PatternDefinition::doStaticRoadWarrior(const PatternDefinition &patt1,
					    const PatternDefinition &sec1,
					    const PatternDefinition &patt2,
					    const PatternDefinition &sec2)
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



/***************************************************************\
 * This class define and read a pattern bank ASCII file.       *
 * The patterns are not loaded in memory, fast but with high   *
 * memory requirements, instead the file is read a first time  *
 * searching for the start of each single pattern definition,  *
 * then when is asked for a pattern the file portion is read   *
 \**************************************************************/
PatternBank::PatternBank(bool usehash) :
  m_fname(""),m_ifile(), m_ready(false), m_okread(false),
  m_nplanes(0), m_npatterns(0), m_maxnhits(0), m_maxnmissing(0),
  m_usehash(usehash), m_maps(0)
{;}


PatternBank::PatternBank(const char *fname, bool usehash) :
   m_ready(false), m_nplanes(0), m_npatterns(0),
   m_maxnhits(0), m_maxnmissing(0), m_usehash(usehash),
   m_maps(0)
{
  m_fname = fname;
  setFile(fname);
}

void PatternBank::setFile(const char *fname)
{
  m_okread = ftk_dcap::open_for_read(fname,m_ifile);
  if (!m_ifile || !m_okread) {
    cerr << "*** Error loading the pattern bank: " << fname << endl;
  }
}

/** initialize read-on-demand access to patterns (using m_cache) */
void PatternBank::InitOnDemand()
{
  assert(!m_ready); // don't allow to init twice
  if (!m_ifile || !m_okread) return;

  // check if the header contains the 3 hashes
  // this is not the current standard but is possible that this
  // will be the next
  string key, value;
  m_ifile >> key >> value;
  if (key.substr(0,4)==string("hash")) {
    cout << "load the hashes" << endl;
    for (int i=0;i<2;++i)
      m_ifile >> key >> value;
  }
  else {
    m_ifile.seekg(ios::beg);
  }

  // retrieve the number of patterns and planes
  m_ifile >> m_npatterns >> m_nplanes;
  
  // initialize the maps that contain the hash lists
  if (m_usehash) {
    m_maps = new multimap<unsigned int,int>[m_nplanes];
  }

  // prepare the array used to generate the crc32 hash
  unsigned int *crc32_key = new unsigned int[m_nplanes];

  // scan the file to map the position of each pattern definition
  for (int i=0;i<m_npatterns;++i) { // patterns loop
     // add the position into the vector
    m_pos.push_back( m_ifile.tellg() );

    // test the sync
    char test[15];
    m_ifile >> test;
    if (atoi(test)!=i) {
      cerr << "*** sync err: " << test << "!=" << i << endl;
    }

    if (i%100000==0) {
      cout << "Scan pattern # " << setw(9) << i << endl;
    }

    if (m_usehash) {       // read the other part to create the hashes

      // each combination with ha missing planes as its own hash creator
      //TMD5 hash_creator[m_nplanes];
      for (int ic=0;ic<m_nplanes;++ic) // reset crc32 keys
	crc32_key[ic] = 0;

      for (int iv=0;iv<m_nplanes;++iv) { // loop over the SS IDs
	int ssid;
	m_ifile >> ssid;
	for (int ic=0;ic<m_nplanes;++ic) { // loop over HC
	  if (iv!=ic) // each HC skip the value of the SS for its layer
	    //hash_creator[ic].Update((UChar_t*) &ssid,sizeof(ssid));
	    crc32_key[ic] = crc32(&ssid,sizeof(ssid),crc32_key[ic]);
	} // end loop over HC
      } // end loop over the SS IDs


      // finalize the HC and store the obtained value in the maps
      for (int ic=0;ic<m_nplanes;++ic) { // loop over HC
	//hash_creator[ic].Final();
	//int tmpkey = PatternDefinition::MD5toInt(hash_creator[ic].AsString());
	m_maps[ic].insert( pair<unsigned int,int>(crc32_key[ic],i) );
      } // end loop over HC
    }
    // ignore the rest of the line
    m_ifile.ignore(300,'\n');
  } // end file scan

  delete [] crc32_key;
  m_ready = true;
}

/** preload all patterns into m_preload */
bool PatternBank::InitPreload()
{
  assert(!m_ready); // don't allow to init twice  
  if (!m_ifile || !m_okread) return false;

  // retrieve the number of patterns and planes
  m_ifile >> m_npatterns >> m_nplanes;

  cout << "Preloading " << m_npatterns << " patterns..." << endl;
  if(m_npatterns>0) {
    m_preload.resize(m_npatterns);
    for(int id=0;id<m_npatterns;id++) {
      PatternDefinition road(id,m_nplanes);
      m_ifile >> road;
      m_preload[id]=road;
    }
    m_ready = true;
  }

  return m_ready;
}

/** Initialize pattern lookup table from preloaded structure */
bool PatternBank::InitLookup(bool clear) {
  if(m_preload.empty()) return false;
  for(unsigned i=0; i<m_preload.size(); i++) {
    addPattern(m_preload[i]);
  }
  
  if(clear) m_preload.clear(); // saving memory - if we don't need vector list
  return true;
}

/** preload all patterns into m_preload */
bool PatternBank::AppendLookup(int npromote)
{
  int npatterns;
  if (!m_ifile || !m_okread) return false;
  
  // retrieve the number of patterns and planes
  m_ifile >> npatterns >> m_nplanes;
  PatternDefinition road(0,m_nplanes);

  cout << "Loading " << npatterns << " patterns..." << endl;
  if(npatterns>0) {
    for(int id=0;id<npatterns;id++) {
      m_ifile >> road;
      // artificially increase weight of a pattern (e.g, if merging pattgen patterns)
      if (npromote) road.setNEvents(road.getNEvents()+npromote);
      addPattern(road);
    }

    m_ready = true;
  }

  return m_ready;
}

void PatternBank::mergeOverlaps() {
  if( m_kdlookup.size() > 1 ) {
    cout << "Merging overlapping patterns..." << endl;
    for( int cur_miss = 1; cur_miss <= m_maxnmissing; cur_miss++ ) {
      // Absorb all patterns with cur_miss missing hits into
      // patterns with fewer missing hits.
	
      // Outer loop: Get patterns with cur_miss missing hits:
      for( map<unsigned int, KD_Elm*>::iterator outer = m_kdlookup.begin(),
	     out_end = m_kdlookup.end(); outer != out_end; outer++ ) {
	if( outer->second->getRoad().getNMissing() != cur_miss ) continue;
	// Now outer has cur_miss missing hits.
	set<KD_Elm*,KD_Elm::lessThanNevents> checkme;
	outer->second->getAllActive(checkme);
	for( set<KD_Elm*>::iterator cur_patt = checkme.begin(),
	       check_end = checkme.end(); cur_patt != check_end;
	     cur_patt++ ) {
	  set<KD_Elm*> sims;
	  
	  for( int comp_miss = cur_miss-1; comp_miss >= 0; comp_miss-- ) {
	    // Inner loop: See if any patterns with comp_miss missing
	    // hits match cur_patt
	    
	    for( map<unsigned int, KD_Elm*>::iterator inner = m_kdlookup.begin(),
		   in_end = m_kdlookup.end(); inner != in_end; inner++ ) {
	      if( inner->second->getRoad().getNMissing() != comp_miss ) continue;
	      
	      inner->second->findSimilar((*cur_patt)->getRoad(), sims);
	    }
	  }
	  
	  int nhigher = sims.size();
	  
	  if( nhigher ) {
	    // Only increment by this amount since the current pattern could
	    // have multiple higher patterns, but we want to keep the overall
	    // normalization of tracks constant.
	    float inc = (*cur_patt)->getRoad().getNEvents()/(float)nhigher;
	    for( set<KD_Elm*>::iterator it = sims.begin(), end = sims.end();
		 it != end; it++ ) {
	      (*it)->getRoad().setNEvents( (*it)->getRoad().getNEvents() + inc );
	      
	    }
	    // Deactivate current pattern:
	    (*cur_patt)->getRoad().setNEvents(-1);
	  } // if nhigher
	} // loop over the patterns in a set with cur_miss missing hits
      } // outer loop
    } // loop over sets of patterns with cur_miss missing hits
  } // End pattern merging
}

/** Dump the patterns in m_lookup */
// void PatternBank::Dump() {
//   cout << "Dumping " << m_lookup.size() << " patterns with max multiplicity of " << m_maxnhits << endl;
//   for(map<PatternDefinition,int>::iterator i = m_lookup.begin(); i!=m_lookup.end(); i++) {
//     cout << i->first << endl;
//   }
// }

/** Saves pattern bank in a file in a standard format */
void PatternBank::SaveAs(const char *fname) {
  ftk_dcap::ostream fout;
  bool success = ftk_dcap::open_for_write(fname,fout);
  if(!success) {
    cout << "Failed to open the file " << fname << endl;
    exit(0);
  }

  mergeOverlaps();

  // Let us order the patterns by coverage (aka number of events)
  cout << "Ordering the patterns..." << endl;
  set<KD_Elm*,KD_Elm::lessThanNevents> patterns;
  for( map<unsigned int, KD_Elm*>::iterator i = m_kdlookup.begin(),
	 iend = m_kdlookup.end(); i != iend; i++ ) {
    set<KD_Elm*> patts;
    i->second->getAllActive(patterns);
//     for( set<KD_Elm*>::iterator j = patts.begin(), jend = patts.end();
// 	 j != jend; j++ ) {

//       patterns.insert( (*j)->getRoad() );
//     }
    //delete i->second;
  }
  //m_kdlookup.clear();

  cout << "Will save " << patterns.size() << " patterns into " <<  fname << endl;
  fout << patterns.size() << " " << m_nplanes << endl;
  
  // Finally, dump the results into a text file
  cout << "Writing patterns to disk..." << endl;
  unsigned int gid(0);
  for(set<KD_Elm*,KD_Elm::lessThanNevents>::iterator i = patterns.begin(); i!=patterns.end(); i++) {
    PatternDefinition p((*i)->getRoad());
    p.setID(gid++);
    fout << p << endl;
  }
  // Clean up
  for( map<unsigned int, KD_Elm*>::iterator i = m_kdlookup.begin(),
	 iend = m_kdlookup.end(); i != iend; i++ )
    delete i->second;
  m_kdlookup.clear();
}

/** retrieve the pattern in O(log(n)) time using cache */
PatternDefinition PatternBank::getPattern(int id, bool toCache)
{
  map<int,PatternDefinition>::iterator item = m_pattcache.find(id);
  if (item!=m_pattcache.end()) // found
    return (*item).second;

  PatternDefinition road(id,m_nplanes);

  m_ifile.seekg(m_pos[id]);
  m_ifile >> road;

  if (toCache) m_pattcache[id] = road;

  return road;
}

/** load the pattern with the given ID into the cache */
void PatternBank::cacheLoad(int id)
{
  map<int,PatternDefinition>::iterator item = m_pattcache.find(id);

  if (item==m_pattcache.end()) { // to insert
    m_pattcache[id] = getPattern(id);
  }
}


/** clear the cached elements */
void PatternBank::cacheClear()
{
  m_pattcache.clear();
}


/** return the number of pattern in this bank with a given hash and
    ID of missing layer */
int PatternBank::countSimilar(int ip,PatternDefinition &pattern)
{
  int nsims(0);
  
  const unsigned int refhash = pattern.getHash(ip);
  multimap<unsigned int,int>::iterator item = m_maps[ip].find(refhash);
  while ((*item).first == refhash) {
    // compare all the elements
    PatternDefinition tocheck = getPattern((*item).second);
    // check if the pattern are equal excluding layer ip
    bool isok(true);
    for (int il=0;il<m_nplanes;++il) {
       if (il==ip) continue; // skip this layer
       if (pattern.getSSID(il)!=tocheck.getSSID(il)) {
	  isok = false;
	  break;
       }
    }

    if (isok) {
       cout << tocheck << endl;
       nsims += 1;
    }

    // go to the next item
    ++item;
  }
  
  return nsims;
}


/** return an STL vector of patterns that in this bank are similar 
    with a given hash and ID of missing layer */
vector<PatternDefinition> PatternBank::getSimilar(int ip,PatternDefinition &pattern)
{
  const unsigned int refhash = pattern.getHash(ip);
  multimap<unsigned int,int>::iterator item = m_maps[ip].find(refhash);

  vector<PatternDefinition> outvec;

  while (item != m_maps[ip].end()) {
     if ((*item).first != refhash) break;

    // compare all the elements
    PatternDefinition tocheck = getPattern((*item).second);
    // check if the pattern are equal excluding layer ip
    bool isok(true);
    for (int il=0;il<m_nplanes;++il) {
       if (il==ip) continue; // skip this layer
       if (pattern.getSSID(il)!=tocheck.getSSID(il)) {
	  isok = false;
	  break;
       }
    }

    if (isok) {
       outvec.push_back(tocheck);
    }

    // go to the next item
    ++item;
  }
  
  return outvec;
}

/** the constructor should know the dimension of the space */
KD_Elm::KD_Elm(const PatternDefinition& aRoad, int depth, KD_Elm *parent) :
  m_dim(aRoad.getNPlanes()), m_depth(depth), m_road(aRoad),
  m_parent(parent), m_left(0), m_right(0)
{
  // determine the SS that splits the hyper-space
  m_splitplane = parent ? (parent->getSplitPlane()+1) : 0;
  m_splitplane %= m_dim;
  while( m_road.getSSID(m_splitplane)/100 == -999 ) { m_splitplane++; m_splitplane %= m_dim; }
}


/** destructor of the Tree, recursively destroy all the level */
KD_Elm::~KD_Elm()
{
  if (m_left) delete m_left;
  if (m_right) delete m_right;
}

/** find an empty leaf to place this road in the kd-tree*/
int
KD_Elm::addNode(const PatternDefinition& aRoad)
{
  int nadded = 0;

  // determine the direction

  // In this implementation of the KD tree, all roads in the tree have
  // the same bitmask
  if ( aRoad.getSSID(m_splitplane) <= m_road.getSSID(m_splitplane) ) {
    // is the left part the axis
    if (!m_left) {
      // create a new sub-tree
      m_left = new KD_Elm(aRoad,m_depth+1,this);
      ++nadded;
    }
    else {
      // continue the search
      nadded+=m_left->addNode(aRoad);
    }
  }
  if( aRoad.getSSID(m_splitplane) > m_road.getSSID(m_splitplane) ) {
    // is on the right part of the axis
    if (!m_right) {
      // create a new sub-tree
      m_right = new KD_Elm(aRoad,m_depth+1,this);
      ++nadded;
    }
    else {
      // continue the search
      nadded+=m_right->addNode(aRoad);
    }
  }
  
  return nadded;
}

void KD_Elm::findSimilar(const PatternDefinition& refpatt,
			 set<KD_Elm*> &nodes)
{

  if( m_road.getNEvents() > 0) {

    // evaluate if the superstrips are similar
    bool match(true);
    
    for (int ip=0;ip < m_dim && match;++ip) {
      // Count how many hits are missing. We only want to find similar
      // patterns with more or fewer hits since there are no
      // duplicates.
      
      if( refpatt.getSSID(ip)/100 == -999 ||
	  m_road.getSSID(ip)/100 == -999 ) {
	continue;
      }
      
      if( refpatt.getSSID(ip) != m_road.getSSID(ip) )
	match = false;
    }
    if (match ) {
      nodes.insert(this);
    }
  }
  
  if (m_left) {
    // if similar patterns could be in this part continue the search
    if ( refpatt.getSSID(m_splitplane)/100 == -999 ||
         refpatt.getSSID(m_splitplane) <= m_road.getSSID(m_splitplane) ) {
      m_left->findSimilar(refpatt,nodes);
    }
  }
  
  if (m_right) {
    // if similar patterns could be in this part continue the search
    if ( refpatt.getSSID(m_splitplane)/100 == -999 ||
         refpatt.getSSID(m_splitplane) > m_road.getSSID(m_splitplane) ) {
      m_right->findSimilar(refpatt,nodes);
    }
  }
}

void KD_Elm::getAllActive(std::set<KD_Elm*,KD_Elm::lessThanNevents> &nodes) {

  if( m_road.getNEvents() > 0 )
    nodes.insert(this);

  if (m_left)
    m_left->getAllActive(nodes);
  if (m_right)
    m_right->getAllActive(nodes);

}
