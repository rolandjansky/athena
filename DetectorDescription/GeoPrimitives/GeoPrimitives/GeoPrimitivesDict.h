/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPrimitives/GeoPrimitives.h"


struct GeoPrimitivesInstan
{
  Eigen::Transform<double,3,2,0> m_t;
  Eigen::Matrix<double,3,3,0,3,3> m_m3;
  Eigen::Matrix<double,5,5,0,5,5> m_m5;
  Eigen::Matrix<double,2,1,0,2,1> m_v2;
  Eigen::Matrix<double,3,1,0,3,1> m_v3;
  Eigen::Matrix<double,5,1,0,5,1> m_v5;
  Eigen::Matrix<double,4,4,0,4,4> m_m4;
  Eigen::Block<const Eigen::Matrix<double,4,4,0,4,4>,3,1,false> m_b4;
  Eigen::MatrixBase<Eigen::Block<const Eigen::Matrix<double,4,4,0,4,4>,3,1,false> > m_mbb4;
  Eigen::MatrixBase<Eigen::Matrix<double,3,3,0,3,3> > m_mb3;
  Eigen::MatrixBase<Eigen::Matrix<double,4,4,0,4,4> > m_mb4;
  Eigen::MatrixBase<Eigen::Matrix<double,5,5,0,5,5> > m_mb5;
  Eigen::MatrixBase<Eigen::Matrix<double,2,1,0,2,1> > m_vb2;
  Eigen::MatrixBase<Eigen::Matrix<double,3,1,0,3,1> > m_vb3;
  Eigen::MatrixBase<Eigen::Matrix<double,5,1,0,5,1> > m_vb5;
  Eigen::DenseBase<Eigen::Block<const Eigen::Matrix<double,4,4,0,4,4>,3,1,false> > m_db4;
  Eigen::DenseCoeffsBase<Eigen::Block<const Eigen::Matrix<double,4,4,0,4,4>,3,1,false>,2> m_dc4_2;
  Eigen::DenseCoeffsBase<Eigen::Block<const Eigen::Matrix<double,4,4,0,4,4>,3,1,false>,0> m_dc4_0;
  Eigen::PlainObjectBase<Eigen::Matrix<double,3,3,0,3,3> > m_pm3;
  Eigen::PlainObjectBase<Eigen::Matrix<double,4,4,0,4,4> > m_pm4;
  Eigen::PlainObjectBase<Eigen::Matrix<double,5,5,0,5,5> > m_pm5;
  Eigen::PlainObjectBase<Eigen::Matrix<double,2,1,0,2,1> > m_pv2;
  Eigen::PlainObjectBase<Eigen::Matrix<double,3,1,0,3,1> > m_pv3;
  Eigen::PlainObjectBase<Eigen::Matrix<double,5,1,0,5,1> > m_pv5;
  Eigen::DenseBase<Eigen::Matrix<double,2,1,0,2,1> > m_vdb2;
  Eigen::DenseBase<Eigen::Matrix<double,3,1,0,3,1> > m_vdb3;
  Eigen::DenseBase<Eigen::Matrix<double,5,1,0,5,1> > m_vdb5;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,2,1,0,2,1>,1> m_dcv2_1;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,2,1,0,2,1>,3> m_dcv2_3;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,3,1,0,3,1>,3> m_dcv3_3;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,3,1,0,3,1>,1> m_dcv3_1;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,5,1,0,5,1>,3> m_dcv5_3;
  Eigen::DenseCoeffsBase<Eigen::Matrix<double,5,1,0,5,1>,1> m_dcv5_1;

  Eigen::Matrix<double,-1,-1,0,-1,-1> m_m_m1;
  Eigen::PlainObjectBase<Eigen::Matrix<double,-1,-1,0,-1,-1> > m_p_m1;
  Eigen::MatrixBase<Eigen::Matrix<double,-1,-1,0,-1,-1> > m_mb_m1;
};
