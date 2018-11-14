/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkGlobalChi2Fitter/GXFLUDecomp.h"


namespace Trk {
  GXFLUDecomp::GXFLUDecomp():m_luSet(false){}


 void GXFLUDecomp::SetMatrix( Amg::SymMatrixX& matrix)
 {
   m_matrix = matrix;
   m_luSet = false;
 }

 Amg::VectorX GXFLUDecomp::Solve( Amg::VectorX&  vector, bool& ok)
 {
   ok = true;
   if(!m_luSet){
     m_lu = Eigen::LLT<Eigen::MatrixXd>(m_matrix);
     m_luSet = true;
   }
   if( m_lu.info() != Eigen::Success ){
     ok = false;
     return Eigen::VectorXd::Zero( vector.size() );
   }
   return m_lu.solve(vector);
 }

 Amg::SymMatrixX GXFLUDecomp::Invert(bool& ok)
 {
   ok = true;
   if(!m_luSet){
     m_lu = Eigen::LLT<Eigen::MatrixXd>(m_matrix);
     m_luSet = true;
   }
   if( m_lu.info() != Eigen::Success ){
     ok = false;
     return Eigen::MatrixXd::Identity( m_matrix.cols(), m_matrix.cols() );
   }

   Amg::SymMatrixX a = Eigen::MatrixXd::Identity( m_matrix.cols(), m_matrix.cols() );
   return m_lu.solve(a);
 }


}// end namespace Trk
