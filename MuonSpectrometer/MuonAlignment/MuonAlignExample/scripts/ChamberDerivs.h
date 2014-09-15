/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHAMBERDERIVS_H
#define CHAMBERDERIVS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "TMatrix.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixTUtils.h"

using namespace std;

namespace CLHEP {
  class HepSymMatrix;
}

class ChamberDerivs {

 public:

  enum SolveType { CLHEP,ROOT };
  
  ChamberDerivs();
  ChamberDerivs(int npar, int nchambers);
  virtual ~ChamberDerivs();

  int npar() const { return m_npar; }
  int nchambers() const { return m_nchambers; }
  int nindices() const { return m_nindices; }

  // chamber ID
  void setChamberId(int ich, int chid) { m_chamberIds[ich]=chid; } 
  int  chamberId(int ich) const { return m_chamberIds[ich]; }

  int getChIndex(int id) const;

  // matrix index
  void setMatrixIndex(int index, int matindex) { m_matrixIndices[index]=matindex; } 
  void setMatrixIndex(int ich, int ipar, int matindex) { m_matrixIndices[ich*m_npar+ipar]=matindex; } 
  int  matrixIndex(int index) const { return m_matrixIndices[index]; }
  int  matrixIndex(int ich, int ipar) const { return m_matrixIndices[ich*m_npar+ipar]; }

  // first derivs
  void   setFirstDeriv(int index, double val) { m_firstDeriv[index]=val; }
  void   setFirstDeriv(int ich, int ipar, double val) { m_firstDeriv[ich*m_npar+ipar]=val; }
  void   addFirstDeriv(int index, double delta) { m_firstDeriv[index]+=delta; }
  void   addFirstDeriv(int ich, int ipar, double delta) { m_firstDeriv[ich*m_npar+ipar]+=delta; }
  double firstDeriv(int i) const { return m_firstDeriv[i]; }
  double firstDeriv(int ich, int ipar) const { return m_firstDeriv[ich*m_npar+ipar]; }

  // second derivs
  void   setSecondDeriv(int i_index, int j_index, double val);
  void   setSecondDeriv(int ich, int i_index, int jch, int j_index, double val);
  
  void   addSecondDeriv(int i_index, int j_index, double delta);
  void   addSecondDeriv(int ich, int i_index, int jch, int j_index, double delta);

  double secondDeriv(int i_index, int j_index) const;
  double secondDeriv(int ich, int i_index, int jch, int j_index) const;

  // hits
  void incrementHitmap(int ich, int jch);
  void incrementHitmap(int index);
  void setNHits(int ich, int jch, int nhits);
  void setNHits(int index,int nhits);

  int  nhits(int ich, int jch) const { return m_hitmap[ich*m_nchambers+jch]; }

  // alignment parameters
  double robustAlignPar(int ich, int ipar) const;
  double robustAlignErr(int ich, int ipar) const;
  double robustAlignPar(int index) const;
  double robustAlignErr(int index) const;

  // matrix manipulation methods
  ChamberDerivs trimmedChamberDerivs(int minHitsOnDiagonal, int minHitsOffDiagonal) const;
  ChamberDerivs trimmedChamberDerivsBySV(double singularValueCut, int offdiagcut=0) const;
  ChamberDerivs* oneChamberDerivs(int ich) const;
  
  CLHEP::HepSymMatrix* secDerivMatrixCLHEP(int ich=-1) const;
  TMatrixDSym* secDerivMatrixROOT(int ich=-1, int npar=0) const;

  // dump methods
  void PrintSecondDerivs(int ich=-1) const;
  void PrintFirstDerivs(int ich=-1) const;
  void PrintChambers(int ich=-1) const;

  // solve method
  double* solve(double*& err, int ich=-1, SolveType solveType=CLHEP) const;

  // determinant
  double determinant(int ich=-1) const;
  double condition(int ich=-1) const;
  double conditionChamberRemoved(int ich) const;


  void singularValues(vector<double>& sv,
		      const TMatrixD*& u, const TMatrixD*& vinv,int ich=-1) const;

  // add constraint
  //ChamberDerivs addParameter(int parameterToAdd, int totalNPar, 
  //		     double* valueToAdd=0);
  void addConstraint(int ich, int ipar, double constraint);
  void addFirstDerivConstraint(int ich, int ipar, double constraint);
  void constrainLowestEigenvector(const std::map<int,string>& chamberIdMap);

 private:
  int            m_npar;
  int            m_nchambers;
  int            m_nindices;

  //std::list< std::pair<int,int>, double* >
  int*           m_chamberIds;
  int*           m_matrixIndices;
  int*           m_hitmap;

  double*        m_secDeriv;
  double*        m_firstDeriv;
};

inline double ChamberDerivs::secondDeriv(int i_index, int j_index) const { return m_secDeriv[i_index*m_nindices+j_index]; }

inline double ChamberDerivs::secondDeriv(int ich, int ipar, int jch, int jpar) const { int i_index=ich*m_npar+ipar; int j_index=jch*m_npar+jpar; return m_secDeriv[i_index*m_nindices+j_index]; }

//________________________________________________________________________
inline void ChamberDerivs::addSecondDeriv(int i_index, int j_index, double delta) { m_secDeriv[i_index*m_nindices+j_index]+=delta; }

//________________________________________________________________________
inline void ChamberDerivs::addSecondDeriv(int ich, int ipar, int jch, int jpar, double delta) { 
  int i_index=ich*m_npar+ipar; 
  int j_index=jch*m_npar+jpar;
  m_secDeriv[i_index*m_nindices+j_index]+=delta; 
}

//________________________________________________________________________
inline void ChamberDerivs::setSecondDeriv(int i_index, int j_index, double val) 
{ 
  m_secDeriv[i_index*m_nindices+j_index]=val; 
}

//________________________________________________________________________
inline void ChamberDerivs::setSecondDeriv(int ich, int ipar, int jch, int jpar, 
					  double val) 
{ 
  int i_index=ich*m_npar+ipar;
  int j_index=jch*m_npar+jpar;
  m_secDeriv[i_index*m_nindices+j_index]=val; 
}

//________________________________________________________________________
inline void ChamberDerivs::incrementHitmap(int ich, int jch)
{ m_hitmap[ich*m_nchambers+jch]++; }

//________________________________________________________________________
inline void ChamberDerivs::incrementHitmap(int index)
{ m_hitmap[index]++; }

//________________________________________________________________________
inline void ChamberDerivs::setNHits(int ich, int jch, int nhits)
{ m_hitmap[ich*m_nchambers+jch]=nhits; }

//________________________________________________________________________
inline void ChamberDerivs::setNHits(int index,int nhits)
{ m_hitmap[index]=nhits; }

#endif // CHAMBERDERIVS_H
