/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKPATTKDTREE_H
#define FTKPATTKDTREE_H

#include "TrigFTKBankGen/FTKBankGenAlgo.h"
#include <stdio.h>

#include <set>
#include <map>
#include <list>
#include <vector>
#include <utility>

/** this class describes a KD-Tree to simplify
    the search for patterns */
class FTKPattKDTree {
private:
  int m_dim; // dimension of the space
  int m_depth; // depth level, 0 is the top level
  int m_below; // This element plus the number below it
  int m_splitplane; // coordinate compared at this level
  std::vector<int> m_pattern; // the pattern
  std::vector<int> m_hashID; // the pattern
  float m_hitcount; // number of times this pattern was hit
  double m_tmpC;//tmpc
  double m_tmpD;//tmpD
  double m_tmpPhi;//tmpPhi
  double m_tmpCoto;//tmCoto
  double m_tmpZ;//tmpZ
  float m_tmpnhit;//tmpnhit
  std::vector<double> m_tmphitc; // the pattern
  std::vector<double> m_tmpxC;
  std::vector<double> m_tmpxD; 
  std::vector<double> m_tmpxPhi;
  std::vector<double> m_tmpxCoto;
  std::vector<double> m_tmpxZ; 
  std::vector<double> m_tmpcovx; 
  std::vector<short> m_intc;
  std::vector<short> m_intphi;
  std::vector<short> m_intd0;
  std::vector<short> m_intz0;
  std::vector<short> m_inteta; 

  int m_nmissing; // Number of wildcard hits in this pattern
  FTKPattKDTree *m_parent; // parent element
  FTKPattKDTree *m_left; // left sub-tree
  FTKPattKDTree *m_right; // right sub-tree
  

public:
  //    FTKPattKDTree(const std::vector<int>& pattern, 
    FTKPattKDTree(const std::vector<int>& pattern, 
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
		  int depth=0, FTKPattKDTree *parent=0,
		  const double& tmpC=0,const double& tmpD=0,const double& tmpPhi=0,
		  const double& tmpCoto=0,const double& tmpZ=0,const float& tmpnhit=0
		  );
    
  ~FTKPattKDTree();

  int addNode(const std::vector<int>& pattern,
	      const std::vector<int>& hashID,
	      const std::vector<double>& tmphic,
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
	      const float& hitcount,const double& tmpC,
	      const double& tmpD,const double& tmpPhi,const double& tmpCoto,const double& tmpZ,
	      const float& tmpnhit);
  
  int getCount() const { return m_below; }
  float getHitCount() const { return m_hitcount; }
  void incHitCount( float v=1 ) { m_hitcount += v; }
  
  void settmpC(double v){m_tmpC=v;};
  void settmpD(double v){m_tmpD=v;};
  void settmpPhi(double v){m_tmpPhi=v;};
  void settmpCoto(double v){m_tmpCoto=v;};
  void settmpZ(double v){m_tmpZ=v;};
  void settmphitc(const std::vector<double>& v){m_tmphitc=v;};
  void settmpxC(const std::vector<double>& v){m_tmpxC=v;};
  void settmpxD(const std::vector<double>& v){m_tmpxD=v;};
  void settmpxPhi(const std::vector<double>& v){m_tmpxPhi=v;};
  void settmpxCoto(const std::vector<double>& v){m_tmpxCoto=v;};
  void settmpxZ(const std::vector<double>& v){m_tmpxZ=v;};
  void settmpcovx(const std::vector<double>& v){m_tmpcovx=v;};
  
  void setintc(const std::vector<short>& v){m_intc=v;};
  void setintphi(const std::vector<short>& v){m_intphi=v;};
  void setintd0(const std::vector<short>& v){m_intd0=v;};
  void setintz0  (const std::vector<short>& v){m_intz0=v;};
  void setinteta  (const std::vector<short>& v){m_inteta=v;};
  
  void settmpnhit(float v){m_tmpnhit=v;};

  double gettmpC() const { return m_tmpC;}
  double gettmpD() const { return m_tmpD;}
  double gettmpPhi() const { return m_tmpPhi;}
  double gettmpCoto() const { return m_tmpCoto;}
  double gettmpZ() const { return m_tmpZ;}
  double gettmpnhit() const{ return m_tmpnhit;}

  const std::vector<double>& gettmphitc() const { return m_tmphitc;}
  const std::vector<double>& gettmpxC() const { return m_tmpxC;}
  const std::vector<double>& gettmpxD() const { return m_tmpxD;}
  const std::vector<double>& gettmpxPhi() const { return m_tmpxPhi;}
  const std::vector<double>& gettmpxCoto() const { return m_tmpxCoto;}
  const std::vector<double>& gettmpxZ() const { return m_tmpxZ;}
  const std::vector<double>& gettmpcovx() const { return m_tmpcovx;}
  
  const std::vector<short>& getintc() const { return m_intc;}
  const std::vector<short>& getintphi() const { return m_intphi;}
  const std::vector<short>& getintd0() const { return m_intd0;}
  const std::vector<short>& getintz0() const { return m_intz0;}
  const std::vector<short>& getinteta() const { return m_inteta;}
  
  int getNMissing() const { return m_nmissing; }
  void getEndangered(std::set<std::pair<std::vector<int>, float> > &, bool);
  void getAllBelow(std::vector< FTKPattKDTree* >& );
  int recursiveRemove();
  void markForRemoval();
  void cleanup();
  const std::vector<int>& getPattern() const { return m_pattern; }
  const std::vector<int>& gethashID() const { return m_hashID; }
  bool markedForRemoval() const { return (m_hitcount < 0); }
  const int &getDepth() const { return m_depth; }
  int getSplitPlane() const { return m_splitplane; }
  void findSimilar(bool const_test,
		   const std::vector<int>&, 
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<double>&,
		   const std::vector<short>&,
		   const std::vector<short>&,
		   const std::vector<short>&,
		   const std::vector<short>&,
		   const std::vector<short>&,
		   std::set<FTKPattKDTree*> &, 
		   bool exactmatch = false,const double& C=0,
		   const double& tmpD=0,const double& tmpPhi=0,const double& tmpCoto=0,const double& tmpZ=0,
		   const float& tmpnhit=0);
};

// Interface functions go here
void initPattTree(int nbanks, int nplanes,int ndims);
void finishPattTree();
int addKDPattern(unsigned int bank, bool const_test_mode, int* pattern, int *hashID, int mult,matrix Mtmp,double* hitc,
		 double* xC,double* xD,double* xPhi,double* xCoto,double* xZ,double* covx,
		 vector<short> int_c,vector<short> int_phi,vector<short> int_d0,
		 vector<short> int_z0,vector<short> int_eta);

int loadPatterns(unsigned int bank, FILE *infile, int npatterns);
int isTherePatt(unsigned int bank, int* pattern);
int sortPatterns(unsigned int bank);
int sortMPatterns(unsigned int bank);
int getNPatterns( unsigned int bank);
int getPatternSS(unsigned int bank, unsigned int pattid, int plane);
int getPatthashID(unsigned int bank, unsigned int pattid, int plane);
int getPattNHits(unsigned int bank, unsigned int pattid);
double getPattC(unsigned int bank, unsigned int pattid);
double getPattD(unsigned int bank, unsigned int pattid);
double getPattPhi(unsigned int bank, unsigned int pattid);
double getPattCoto(unsigned int bank, unsigned int pattid);
double getPattZ(unsigned int bank, unsigned int pattid);
double getPatthitc(unsigned int bank, unsigned int pattid,int dim);
float  getPattnhit(unsigned int bank, unsigned int pattid);

double getPattxC(unsigned int bank, unsigned int pattid,int dim);
double getPattxD(unsigned int bank, unsigned int pattid,int dim);
double getPattxPhi(unsigned int bank, unsigned int pattid,int dim);
double getPattxCoto(unsigned int bank, unsigned int pattid,int dim);
double getPattxZ(unsigned int bank, unsigned int pattid,int dim);
double getPattcovx(unsigned int bank, unsigned int pattid,int dim);

short getPattintc(unsigned int bank, unsigned int pattid,int nhit);
short getPattintphi(unsigned int bank, unsigned int pattid,int nhit);
short getPattintd0(unsigned int bank, unsigned int pattid,int nhit);
short getPattintz0(unsigned int bank, unsigned int pattid,int nhit);
short getPattinteta(unsigned int bank, unsigned int pattid,int nhit);

//void print(); // used for debugging

bool compare_patt(const FTKPattKDTree* a, const FTKPattKDTree* b);
bool compare_Mpatt(const FTKPattKDTree* a, const FTKPattKDTree* b);

#endif // FTKPATTKDTREE_H
