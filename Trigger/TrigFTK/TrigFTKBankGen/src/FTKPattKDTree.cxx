/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigFTKBankGen/FTKPattKDTree.h"
#include "TrigFTKBankGen/FTKBankGenAlgo.h"
#include "TrigFTKBankGen/FTKConstGenAlgo.h"

#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cassert>

#include <iostream>

using namespace std;

// kdtree: First index is the bank, second index is the bitmask
// We keep one kdtree for each bitmask
static vector<map<unsigned int, FTKPattKDTree*> > kdtree;
static vector<vector< FTKPattKDTree* > > sortedPatterns;
static int kdplanes(0);
static int kddims(0);
matrix added;
vector<double> added_hitc;
vector<double> added_xC;
vector<double> added_xD;
vector<double> added_xPhi;
vector<double> added_xCoto;
vector<double> added_xZ;
vector<double> added_covx;
vector<short> new_intc;
vector<short> new_intphi;
vector<short> new_intd0;
vector<short> new_intz0;
vector<short> new_inteta;


//FTKPattKDTree::FTKPattKDTree(const std::vector<int>& pattern,
FTKPattKDTree::FTKPattKDTree(const std::vector<int>& pattern,
			     const std::vector<int>& hashID,
			     const std::vector<double>& tmphitc,
			     const std::vector<double>& tmpxC,
			     const std::vector<double>& tmpxD,
			     const std::vector<double>& tmpxPhi,
			     const std::vector<double>& tmpxCoto,
			     const std::vector<double>& tmpxZ,
			     const std::vector<double>& tmpcovx,
			     const std::vector<short>& int_c,
			     const std::vector<short>& int_phi,
			     const std::vector<short>& int_d0,
			     const std::vector<short>& int_z0,
			     const std::vector<short>& int_eta,
			     const float& hitcount,
			     int depth, FTKPattKDTree *parent,
			     const double& tmpC,const double& tmpD,const double& tmpPhi,
			     const double& tmpCoto,const double& tmpZ,const float& tmpnhit
			     ) :
  // Convention for m_dim is that the last number is just the sector
  m_dim(pattern.size()-1), m_depth(depth), m_below(1), m_pattern(pattern), m_hashID(hashID), m_hitcount(hitcount),m_tmpC(tmpC),
  m_tmpD(tmpD),m_tmpPhi(tmpPhi),m_tmpCoto(tmpCoto),m_tmpZ(tmpZ),m_tmpnhit(tmpnhit),m_tmphitc(tmphitc),
  m_tmpxC(tmpxC),m_tmpxD(tmpxD),m_tmpxPhi(tmpxPhi),m_tmpxCoto(tmpxCoto),m_tmpxZ(tmpxZ), m_tmpcovx(tmpcovx),
  m_intc(int_c),m_intphi(int_phi),m_intd0(int_d0),m_intz0(int_z0),m_inteta(int_eta),m_nmissing(0), m_parent(parent), m_left(0), m_right(0)
{
 
  assert(m_dim>0);
  // determine the SS that splits the hyper-space
  m_splitplane = parent ? (parent->getSplitPlane()+1) : 0;
  m_splitplane %= m_dim;

  while( m_pattern[m_splitplane]/100 == -999 ) { m_splitplane++; m_splitplane %= m_dim; }
  
  for( unsigned int i = 0, np=m_dim; i < np; i++ )
    if( m_pattern[i]/100 == -999 ) m_nmissing++;
  
}

/** destructor of the Tree, recursively destroy all the level */
FTKPattKDTree::~FTKPattKDTree()
{
  if (m_left) delete m_left;
  if (m_right) delete m_right;
}


/** find an empty leaf to place this road in the kd-tree*/
int
//FTKPattKDTree::addNode(const std::vector<int>& pattern,
FTKPattKDTree::addNode(const std::vector<int>& pattern,
		       const std::vector<int>& hashID,
		       const std::vector<double>& tmphitc,
		       const std::vector<double>& tmpxC,
		       const std::vector<double>& tmpxD,
		       const std::vector<double>& tmpxPhi,
		       const std::vector<double>& tmpxCoto,
		       const std::vector<double>& tmpxZ,
		       const std::vector<double>& tmpcovx,
		       const std::vector<short>& int_c,
		       const std::vector<short>& int_phi,
		       const std::vector<short>& int_d0,
		       const std::vector<short>& int_z0,
		       const std::vector<short>& int_eta,
		       const float& hitcount,
		       const double& tmpC,const double& tmpD,const double& tmpPhi,
		       const double& tmpCoto,const double& tmpZ,const float& tmpnhit)
{
  
  int nadded = 0;
  // determine the direction
  if ( pattern[m_splitplane] <= m_pattern[m_splitplane] ) {
    // is the left part the axis
    if (!m_left) {
      // create a new sub-tree
      m_left = new FTKPattKDTree(pattern,hashID,tmphitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
				 int_c,int_phi,int_d0,int_z0,int_eta,
				 hitcount,m_depth+1,this,tmpC,tmpD,tmpPhi,tmpCoto,tmpZ,tmpnhit);
      ++nadded;
    }
    else {
      // continue the search
      nadded+=m_left->addNode(pattern,hashID,tmphitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
			      int_c,int_phi,int_d0,int_z0,int_eta,
			      hitcount,tmpC,tmpD,tmpPhi,tmpCoto,tmpZ,tmpnhit);
    }
  }
  else if( pattern[m_splitplane] > m_pattern[m_splitplane] ) {
    // is on the right part of the axis
    if (!m_right) {
      // create a new sub-tree
      m_right = new FTKPattKDTree(pattern,hashID,tmphitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
				  int_c,int_phi,int_d0,int_z0,int_eta,
				  hitcount,m_depth+1,this,tmpC,tmpD,tmpPhi,tmpCoto,tmpZ,tmpnhit);
      ++nadded;
    }
    else {
      // continue the search
      nadded+=m_right->addNode(pattern,hashID,tmphitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
			       int_c,int_phi,int_d0,int_z0,int_eta,
			       hitcount,tmpC,tmpD,tmpPhi,tmpCoto,tmpZ,tmpnhit);
    }
  }
    
  m_below += nadded;
  return nadded;
} 

void
FTKPattKDTree::markForRemoval() {

  m_hitcount = -1;

}

void FTKPattKDTree::getAllBelow( std::vector< FTKPattKDTree* >& activePatts) {
  


  if( !markedForRemoval() )
    activePatts.push_back( this );

  if( m_left )
    m_left->getAllBelow(activePatts);
  
  if( m_right )
    m_right->getAllBelow(activePatts);


}

// Find all patterns in the tree similar to refpatt. If refpatt is in
// the tree, it will be included.
void FTKPattKDTree::findSimilar(bool const_test,
				const vector<int>& refpatt,
				const vector<double>& hitc,
				const std::vector<double>& tmpxC,
				const std::vector<double>& tmpxD,
				const std::vector<double>& tmpxPhi,
				const std::vector<double>& tmpxCoto,
				const std::vector<double>& tmpxZ,
				const std::vector<double>& tmpcovx,
				const std::vector<short>& int_c,
				const std::vector<short>& int_phi,
				const std::vector<short>& int_d0,
				const std::vector<short>& int_z0,
				const std::vector<short>& int_eta,
				set<FTKPattKDTree*> &nodes,
				bool exactmatch,const double& C,const double& D,
				const double& Phi,const double& Coto,const double& Z,
				const float& nhit)
{
  
  if( m_hitcount > 0) {
    // If refpatt matches this node, automatically insert it:
    bool fullmatch(true);
    for(unsigned int i = 0, j = m_dim; i < j && fullmatch; i++) {
      if( refpatt[i] != m_pattern[i] &&
	  (refpatt[i]/100 != -999 || m_pattern[i]/100 != -999) )
	fullmatch = false;
    }
    
        
    if( fullmatch ) {
      nodes.insert(this);
      if(!const_test){
	added.C=m_tmpC+C;
	added.D=m_tmpD+D;
	added.Phi=m_tmpPhi+Phi;
	added.Coto=m_tmpCoto+Coto;
	added.Z=m_tmpZ+Z;
            
	for (int i=0;i < kddims ;++i) {
	  added_hitc[i]=m_tmphitc[i]+hitc[i];
	  added_xC[i]=m_tmpxC[i]+tmpxC[i];
	  added_xD[i]=m_tmpxD[i]+tmpxD[i];
	  added_xPhi[i]=m_tmpxPhi[i]+tmpxPhi[i];
	  added_xCoto[i]=m_tmpxCoto[i]+tmpxCoto[i];
	  added_xZ[i]=m_tmpxZ[i]+tmpxZ[i];
	  for (int j=i;j < kddims ;++j) {
	    added_covx[i*kddims+j]=m_tmpcovx[i*kddims+j]+tmpcovx[i*kddims+j];
	  }
	}
            
	added.nhit=m_tmpnhit+nhit;
      
	new_intc.resize(int(added.nhit));
	new_intphi.resize(int(added.nhit));
	new_intd0.resize(int(added.nhit));
	new_intz0.resize(int(added.nhit));
	new_inteta.resize(int(added.nhit));

	for (int i=0;i <added.nhit ;++i) {
	  if(i<m_tmpnhit){
	    new_intc[i]= m_intc[i];
	    new_intphi[i]= m_intphi[i];
	    new_intd0[i]= m_intd0[i];
	    new_intz0[i]= m_intz0[i];
	    new_inteta[i]= m_inteta[i];
	  	  
	  }else{
	    new_intc[i]= int_c[i-m_tmpnhit];
	    new_intphi[i]= int_phi[i-m_tmpnhit];
	    new_intd0[i]= int_d0[i-m_tmpnhit];
	    new_intz0[i]= int_z0[i-m_tmpnhit];
	    new_inteta[i]= int_eta[i-m_tmpnhit];
	  	  
	  }
	}
      }//const test      

      // If we're looking for an exact match, we're done
      if( exactmatch )
	return;
    }
    else if( !exactmatch ) {
      // evaluate if the superstrips are similar
      bool match(true);
      int refmissing(0);
      
      for (int ip=0;ip < m_dim && match;++ip) {
	// Count how many hits are missing. We only want to find similar
	// patterns with more or fewer hits. The only found pattern with
	// the same number of hits as the refpatt should be that pattern
	// itself, if it is in the tree.
	
	if( refpatt[ip]/100 == -999 ) {
	  refmissing++;
	  continue;
	}
	
	if( m_pattern[ip]/100 == -999 ) continue;
	
	if( refpatt[ip] != m_pattern[ip] )
	  match = false;
      }
      if (match && refmissing != m_nmissing ) {
 	// this node is similar. 
	nodes.insert(this);
      }
    }
  }
  
  if (m_left) {
    // if similar patterns could be in this part continue the search
    if ( (!exactmatch && refpatt[m_splitplane]/100 == -999) ||
	 refpatt[m_splitplane] <= m_pattern[m_splitplane] ) {
      m_left->findSimilar(const_test,refpatt,hitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
			  int_c,int_phi,int_d0,int_z0,int_eta,
			  nodes,exactmatch,C,D,Phi,Coto,Z,nhit);
    }
  }
  
  if (m_right) {
    // if similar patterns could be in this part continue the search
    if ( (!exactmatch && refpatt[m_splitplane]/100 == -999) ||
	 refpatt[m_splitplane] > m_pattern[m_splitplane] ) {
      m_right->findSimilar(const_test,refpatt,hitc,tmpxC,tmpxD,tmpxPhi,tmpxCoto,tmpxZ,tmpcovx,
			   int_c,int_phi,int_d0,int_z0,int_eta,
			   nodes,exactmatch,C,D,Phi,Coto,Z,nhit);
    }
  }
}

// Interface functions
void initPattTree(int nbanks, int nplanes,int ndims) {
  
  for(int i = 0; i < nbanks; i++) { kdtree.push_back( map<unsigned int, FTKPattKDTree*>() ); }
  kdplanes = nplanes;
  kddims = ndims;
  added_hitc.resize(kddims);
  added_xC.resize(kddims);
  added_xD.resize(kddims);
  added_xPhi.resize(kddims);
  added_xCoto.resize(kddims);
  added_xZ.resize(kddims);
  added_covx.resize(kddims*kddims);
    
}

void finishPattTree() {
  // Get rid of the pattern KD trees
  sortedPatterns.clear();

  for( unsigned int i = 0, nt = kdtree.size(); i < nt; i++ )
    for( map<unsigned int, FTKPattKDTree*>::iterator j = kdtree[i].begin(),
	   jend = kdtree[i].end(); j != jend; j++ ) 
      delete j->second;
  
  kdtree.clear();

} 


//int addKDPattern(unsigned int bank, bool const_test_mode, int* pattern, int mult,matrix Mtmp,double* hitc,
int addKDPattern(unsigned int bank, bool const_test_mode, int* pattern, int *hashID, int mult,matrix Mtmp,double* hitc,
		 double* xC,double* xD,double* xPhi,double* xCoto,double* xZ,double* covx,
		 vector<short> int_c,vector<short> int_phi,vector<short> int_d0,
		 vector<short> int_z0,vector<short> int_eta){

  
  // Return codes:
  // -1 - cannot find matched pattern in const_test_mode
  // 0 - Error
  // 1 - Added
  // 2 - Already existed
  
  int nmissing = 0;
  vector<int> thePatt;
  vector<int> theHash;
  vector<double> thehitc;
  vector<double> thexC;
  vector<double> thexD;
  vector<double> thexPhi;
  vector<double> thexCoto;
  vector<double> thexZ;
  vector<double> thecovx;
  
  vector<short> intc;
  vector<short> intphi;
  vector<short> intd0;
  vector<short> intz0;
  vector<short> inteta;
  
  set<FTKPattKDTree*> sims;
  
  thePatt.resize(kdplanes);
  theHash.resize(kdplanes);
  thehitc.resize(kddims);
  thexC.resize(kddims);
  thexD.resize(kddims);
  thexPhi.resize(kddims);
  thexCoto.resize(kddims);
  thexZ.resize(kddims);
  thecovx.resize(kddims*kddims);

  unsigned int bitmask = 0;
  for( int i = 0; i < kdplanes; i++ ) {
    if( pattern[i]/100 == -999 ) nmissing++;
    else if( i < kdplanes-1 )
      bitmask |= (1<<i);
    
    thePatt[i] = pattern[i];
    theHash[i] = hashID[i];
  }
  
  for( int i = 0; i < kddims; i++ ) {
    thehitc[i]=hitc[i];
    thexC[i]=xC[i];
    thexD[i]=xD[i];
    thexPhi[i]=xPhi[i];
    thexCoto[i]=xCoto[i];
    thexZ[i]=xZ[i];
    for( int j = i; j < kddims; j++ ) {
      thecovx[i*kddims+j]=covx[i*kddims+j];
    }
  }
 
  intc.resize(int(Mtmp.nhit));
  intphi.resize(int(Mtmp.nhit));
  intd0.resize(int(Mtmp.nhit));
  intz0.resize(int(Mtmp.nhit));
  inteta.resize(int(Mtmp.nhit));

  
  for( int i = 0; i < Mtmp.nhit; i++ ) {
    intc[i]=int_c[i];
    intphi[i]=int_phi[i];
    intd0[i]=int_d0[i];
    intz0[i]=int_z0[i];
    inteta[i]=int_eta[i];
  }
            
  // Initialize the KD tree for this bank if it's not yet done:
  if( kdtree[bank].count(bitmask) == 0 ) {
    kdtree[bank][bitmask] = new FTKPattKDTree(thePatt,theHash,thehitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
					      intc,intphi,intd0,intz0,inteta,
					      mult,0,0,Mtmp.C,Mtmp.D,Mtmp.Phi,Mtmp.Coto,Mtmp.Z,Mtmp.nhit);
    return 1;
  }
  
  // Check for an exact match:
  kdtree[bank][bitmask]->findSimilar(const_test_mode,thePatt,thehitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
				     intc,intphi,intd0,intz0,inteta,
				     sims, true,Mtmp.C,Mtmp.D,Mtmp.Phi,Mtmp.Coto,Mtmp.Z,Mtmp.nhit);
  assert(sims.size()<=1); // sanity check

  // If the pattern is already in the tree, keep the one with the
  // lower sector number.
  if( sims.size() == 1 ) {
    
    if( (*sims.begin())->getPattern()[kdplanes-1] <= thePatt[kdplanes-1] ) {
      // Just increment the one in the tree and exit
      if(const_test_mode){
	return (*sims.begin())->getPattern()[kdplanes-1];
      }else{
	(*sims.begin())->incHitCount(mult);
	(*sims.begin())->settmpC(added.C);
	(*sims.begin())->settmpD(added.D);
	(*sims.begin())->settmpPhi(added.Phi);
	(*sims.begin())->settmpCoto(added.Coto);
	(*sims.begin())->settmpZ(added.Z);
	(*sims.begin())->settmpnhit(added.nhit);
	(*sims.begin())->settmphitc(added_hitc);
	(*sims.begin())->settmpxC(added_xC);
	(*sims.begin())->settmpxD(added_xD);
	(*sims.begin())->settmpxPhi(added_xPhi);
	(*sims.begin())->settmpxCoto(added_xCoto);
	(*sims.begin())->settmpxZ(added_xZ);
	(*sims.begin())->settmpcovx(added_covx);
	(*sims.begin())->setintc(new_intc);
	(*sims.begin())->setintphi(new_intphi);
	(*sims.begin())->setintd0(new_intd0);
	(*sims.begin())->setintz0(new_intz0);
	(*sims.begin())->setinteta(new_inteta);
      }//const test mode
    }
    else {
      // Need to deactivate the one in the tree add add this one
      if(!const_test_mode){
	float newCount = mult;
	newCount += (*sims.begin())->getHitCount();
	(*sims.begin())->markForRemoval();
	kdtree[bank][bitmask]->addNode(thePatt,theHash,added_hitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
				       intc,intphi,intd0,intz0,inteta,
				       newCount,added.C,added.D,added.Phi,added.Coto,added.Z,added.nhit);
      }//const test mode
    }
    return 2;
  }

  sims.clear();

  // Get all higher patterns similar to this one:
  if( nmissing ) {
    for( map<unsigned int, FTKPattKDTree*>::iterator it = kdtree[bank].begin(),
	   end = kdtree[bank].end(); it != end; it++ )
      if( it->second->getNMissing() < nmissing )
	it->second->findSimilar(const_test_mode,thePatt,thehitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
				int_c,int_phi,int_d0,int_z0,int_eta,sims);
        
    // If it is similar to any higher patterns, increment those and exit
    int nhigher = sims.size();
    
    if( nhigher ) {
      // Only increment by this amount since the current pattern could
      // have multiple higher patterns, but we want to keep the overall
      // normalization of tracks constant.
      double inc = (double)mult/(double)nhigher;
      for( set<FTKPattKDTree*>::iterator it = sims.begin(), end = sims.end();
	   it != end; it++ ) {
	(*it)->incHitCount(inc);
      }
      
      return 2;
    }
  }
  
  if(const_test_mode == true && Mtmp.nhit == 5){
    return -1;//not match pattern
  }else{
    // If we are at this point, then we are going to add the pattern.
    sims.clear();
    for( map<unsigned int, FTKPattKDTree*>::iterator it = kdtree[bank].begin(),
	   end = kdtree[bank].end(); it != end; it++ )
      if( it->second->getNMissing() > nmissing )
	it->second->findSimilar(const_test_mode,thePatt,thehitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
				intc,intphi,intd0,intz0,inteta,sims);

    // Absorb the hit counts of any similar patterns with fewer hits,
    // then remove them from the tree.
    double newmult = mult;
    for( set<FTKPattKDTree*>::iterator it = sims.begin(), end = sims.end();
	 it != end; it++ ) {
    
      newmult += (*it)->getHitCount();
      (*it)->markForRemoval();
    }
    
    // Add the new pattern to the list
    kdtree[bank][bitmask]->addNode(thePatt,theHash,thehitc,thexC,thexD,thexPhi,thexCoto,thexZ,thecovx,
				   intc,intphi,intd0,intz0,inteta,
				   newmult,Mtmp.C,Mtmp.D,Mtmp.Phi,Mtmp.Coto,Mtmp.Z,Mtmp.nhit);

    return 1;
  }
}

bool compare_patt(const FTKPattKDTree* a, const FTKPattKDTree* b) {
  // The std::sort function takes a comparison operator that returns
  // true if 'a' goes before the 'b'. Since we want to sort from
  // most number of hits to least number, 'a' goes before 'b' if it
  // has more hits.

  if( a->getHitCount() > b->getHitCount() ) return true;
  else return false;
}

bool compare_Mpatt(const FTKPattKDTree* a, const FTKPattKDTree* b) {
  
  if( a->gettmpnhit() > b->gettmpnhit() ) return true;
  else return false;
}

int sortPatterns(unsigned int bank) {
  

  if( sortedPatterns.size() <= bank )
    sortedPatterns.resize(bank+1);

  sortedPatterns[bank].clear();
  
  for(map<unsigned int, FTKPattKDTree*>::iterator it = kdtree[bank].begin(),
	end = kdtree[bank].end(); it != end; it++ ) {
    it->second->getAllBelow(sortedPatterns[bank]);
  }
  kdtree[bank].clear();

  sort( sortedPatterns[bank].begin(), 
	sortedPatterns[bank].end(),
	compare_patt );
  
  return 1;

}

int sortMPatterns(unsigned int bank) {
  

  if( sortedPatterns.size() <= bank )
    sortedPatterns.resize(bank+1);

  sortedPatterns[bank].clear();
  
  for(map<unsigned int, FTKPattKDTree*>::iterator it = kdtree[bank].begin(),
	end = kdtree[bank].end(); it != end; it++ ) {
    it->second->getAllBelow(sortedPatterns[bank]);
  }
  kdtree[bank].clear();

  sort( sortedPatterns[bank].begin(), 
	sortedPatterns[bank].end(),
	compare_Mpatt );
  
  return 1;

}


int getNPatterns(unsigned int bank) {

  if( bank >= sortedPatterns.size() ) return 0;
  return sortedPatterns[bank].size();

}

int getPatternSS( unsigned int bank, unsigned int pattid, int plane) {

  if( bank >= sortedPatterns.size() ) return -1;
  if( pattid >= sortedPatterns[bank].size() ) return -1;

  return sortedPatterns[bank][pattid]->getPattern()[plane];

}

int getPatthashID( unsigned int bank, unsigned int pattid, int plane) {

  if( bank >= sortedPatterns.size() ) return -1;
  if( pattid >= sortedPatterns[bank].size() ) return -1;

  return sortedPatterns[bank][pattid]->gethashID()[plane];

}

int getPattNHits(unsigned int bank, unsigned int pattid) {

  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return (int) sortedPatterns[bank][pattid]->getHitCount();
  
}

double getPattC(unsigned int bank, unsigned int pattid) {

  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmpC();
  
}

double getPattD(unsigned int bank, unsigned int pattid) {

  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmpD();
  
}

double getPattPhi(unsigned int bank, unsigned int pattid) {

  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmpPhi();
  
}

double getPattCoto(unsigned int bank, unsigned int pattid) {

  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmpCoto();
  
}
double getPattZ(unsigned int bank, unsigned int pattid) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  
  return sortedPatterns[bank][pattid]->gettmpZ();
  
}

double getPatthitc(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmphitc()[dim];
  
}

float getPattnhit(unsigned int bank, unsigned int pattid) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;

  return sortedPatterns[bank][pattid]->gettmpnhit();
  
}

double getPattxC(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpxC()[dim];
}

double getPattxD(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpxD()[dim];
}

double getPattxPhi(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpxPhi()[dim];
}

double getPattxCoto(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpxCoto()[dim];
}

double getPattxZ(unsigned int bank, unsigned int pattid, int dim) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpxZ()[dim];
}

double getPattcovx(unsigned int bank, unsigned int pattid, int dim2) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->gettmpcovx()[dim2];
}




short getPattintc(unsigned int bank, unsigned int pattid ,int nhit) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->getintc()[nhit];
}


short getPattintphi(unsigned int bank, unsigned int pattid ,int nhit) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->getintphi()[nhit];
}


short getPattintd0(unsigned int bank, unsigned int pattid ,int nhit) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->getintd0()[nhit];
}

short getPattintz0(unsigned int bank, unsigned int pattid ,int nhit) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->getintz0()[nhit];
}


short getPattinteta(unsigned int bank, unsigned int pattid ,int nhit) {
  
  if( bank >= sortedPatterns.size() ) return 0;
  if( pattid >= sortedPatterns[bank].size() ) return 0;
  return sortedPatterns[bank][pattid]->getinteta()[nhit];
}
