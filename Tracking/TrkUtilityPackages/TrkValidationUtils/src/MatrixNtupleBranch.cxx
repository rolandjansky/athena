/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkValidationUtils/MatrixNtupleBranch.h"
#include <TTree.h>
#include <TString.h>

namespace Trk {
  bool MatrixNtupleBranch::initForWrite(TTree& tree, std::string varname, int ncol, int nrow, std::string prefix ) {
    if( ncol >= COLMAX ) return false;
    if( nrow >= ROWMAX ) return false;
    m_ncols = ncol;
    m_nrows = nrow;
    for( int i=0;i<nrow;++i ){
      for( int j=0;j<ncol;++j ){
	TString bname = prefix.c_str();
	bname += varname;
	bname += i;
	bname += j;
	tree.Branch(bname,&m_matrix[i][j]);
      }
    }
    return true;
  }

  bool MatrixNtupleBranch::initForRead(TTree& tree, std::string varname, int ncol, int nrow, std::string prefix ) { 
    if( ncol >= COLMAX ) return false;
    if( nrow >= ROWMAX ) return false;
    m_ncols = ncol;
    m_nrows = nrow;
    for( int i=0;i<nrow;++i ){
      for( int j=0;j<ncol;++j ){
	TString bname = prefix.c_str();
	bname += varname;
	bname += i;
	bname += j;
	tree.SetBranchAddress(bname,&m_matrix[i][j]);
      }
    }
    return true;
  }

  bool MatrixNtupleBranch::fill( const Amg::MatrixX& matrix ) {
    if( m_ncols == -1 ) return false;
    if( matrix.rows() > m_nrows && matrix.cols() > m_ncols ) return false; 

    for( int i=0;i<m_nrows;++i ){
      for( int j=0;j<m_ncols;++j ){
	if( i < matrix.rows() && j < matrix.cols() ) m_matrix[i][j] = matrix(i,j);
	else                                         m_matrix[i][j] = 0.;
      }
    }
    return true;
  }

  bool MatrixNtupleBranch::fill( const HepGeom::Rotate3D& matrix ) {
    if( m_ncols == -1 ) return false;
    if(  m_nrows != 3  && m_ncols != 3 ) return false; 

    for( int i=0;i<m_nrows;++i ){
      for( int j=0;j<m_ncols;++j ){
	if( i < 3 && j < 3 ) m_matrix[i][j] = matrix(i,j);
	else                 m_matrix[i][j] = 0.;
      }
    }
    return true;
  }

}
