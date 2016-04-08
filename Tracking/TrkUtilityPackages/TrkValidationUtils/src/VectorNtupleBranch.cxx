/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValidationUtils/VectorNtupleBranch.h"
#include <TTree.h>
#include <TString.h>

namespace Trk {
  bool VectorNtupleBranch::initForWrite(TTree& tree, std::string varname, int nrow, std::string prefix ) {
    if( nrow >= ROWMAX ) return false;
    m_nrows = nrow;
    for( int i=0;i<nrow;++i ){
      TString bname = prefix.c_str();
      bname += varname;
      bname += i;
      tree.Branch(bname,&m_vector[i]);
    }
    return true;
  }

  bool VectorNtupleBranch::initForRead(TTree& tree, std::string varname, int nrow, std::string prefix ) { 
    if( nrow >= ROWMAX ) return false;
    m_nrows = nrow;
    for( int i=0;i<nrow;++i ){
      TString bname = prefix.c_str();
      bname += varname;
      bname += i;
      tree.SetBranchAddress(bname,&m_vector[i]);
    }
    return true;
  }

  bool VectorNtupleBranch::fill( const Amg::VectorX& vector ) {
    if( m_nrows == -1 ) return false;
    if( vector.rows() > m_nrows ) return false; 

    for( int i=0;i<m_nrows;++i ){
	if( i < vector.rows() ) m_vector[i] = vector[i];
	else                    m_vector[i] = 0.;
    }
    return true;
  }

  bool VectorNtupleBranch::fill( const CLHEP::HepVector& vector ) {
    if( m_nrows == -1 ) return false;
    if( vector.num_row() > m_nrows ) return false; 
    for( int i=0;i<m_nrows;++i ){
	if( i < vector.num_row() ) m_vector[i] = vector[i];
	else                       m_vector[i] = 0.;
    }    
    return true;
  }

  bool VectorNtupleBranch::fill( const HepGeom::Point3D<double>& vector ) {
    if( m_nrows == -1 ) return false;
    if( m_nrows != 3 ) return false; 
    for( int i=0;i<m_nrows;++i ) m_vector[i] = vector[i];
    return true;
  }

  bool VectorNtupleBranch::fill( const CLHEP::Hep3Vector& vector ) {
    if( m_nrows == -1 ) return false;
    if( m_nrows != 3 ) return false; 
    for( int i=0;i<m_nrows;++i ) m_vector[i] = vector[i];
    return true;
  }

  bool VectorNtupleBranch::fill( const HepGeom::Vector3D<double>& vector ) {
    if( m_nrows == -1 ) return false;
    if( m_nrows != 3 ) return false; 
    for( int i=0;i<m_nrows;++i ) m_vector[i] = vector[i];
    return true;
  }

}
