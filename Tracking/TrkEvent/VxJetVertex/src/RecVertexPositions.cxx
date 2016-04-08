/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           RecVertexPositions.cxx  -  Description
                             -------------------
    begin   : Autumn 2006
    authors : Giacinto Piacquadio (Freiburg University)
    email   : Giacinto.Piacquadio@physik.uni-freiburg.de
    comments:
    changes :

 ***************************************************************************/

#include "VxJetVertex/RecVertexPositions.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h" //!< include for the track class
//#include "CLHEP/Vector/ThreeVector.h" //!< include for CLHEP::Hep3Vector
//#include "VxJetVertex/VxVertexOnJetAxis.h"


namespace Trk {
  
  RecVertexPositions::RecVertexPositions() :
    VertexPositions(),
    m_positionError(Amg::MatrixX())
  { m_positionError.setZero();}


  RecVertexPositions::RecVertexPositions(const VertexPositions& pos) :
    VertexPositions(pos),
    m_positionError(Amg::MatrixX())
  { m_positionError.setZero();}


  RecVertexPositions::RecVertexPositions(const Amg::VectorX &pos, const Amg::MatrixX &cov,
					 const double ndf, const double chi2,
					 bool isWeightTimesPosition) :
    VertexPositions(pos), m_positionError(cov), m_fitQuality(Trk::FitQuality(chi2,ndf)) {
    m_useWeightTimesPosition=isWeightTimesPosition;
  }


  RecVertexPositions::RecVertexPositions(const Amg::VectorX &pos, const double ndf, const double chi2) :
                VertexPositions(pos), 
		m_positionError(Amg::MatrixX()),
		m_fitQuality(Trk::FitQuality(chi2,ndf)) { m_positionError.setZero();}

  RecVertexPositions::RecVertexPositions(const RecVertexPositions& rhs) : VertexPositions(rhs),
									  m_positionError(rhs.m_positionError),
									  m_fitQuality(rhs.m_fitQuality)
  {}


  const Amg::VectorX& RecVertexPositions::weightTimesPosition() {
    if (!m_useWeightTimesPosition && m_positionError.determinant()!=0.0) {
      m_position=m_positionError.inverse()*m_position;
      m_useWeightTimesPosition=true;
    } else {
      std::cout << "Warning in RecVertexPositions: requested finalization but was already finalized..." << std::endl;
    }
    return m_position;
  }
  
  // Use storing weight matrix * position instead of position 
  // saves you from inverting the weight matrix at every fit iteration 
  // Then transparently, asking the position for the first time, 
  // the m_position will be restored to the correct value of position 
  // (using the invertion of the weight matrix only once...)
  void RecVertexPositions::setWeightTimesPosition(const Amg::VectorX & weightTimesPosition) {
    m_useWeightTimesPosition=true;
    m_position=weightTimesPosition;
  }

    //the finalizePosition() method is used to ask the RecVertexPositions class 
    //to invert the weight matrix and fill the m_position vector with the real 
    //position, in order to finalize the position (and make it available)
  void RecVertexPositions::finalizePosition() {
    if (m_useWeightTimesPosition) {
      m_useWeightTimesPosition=false;
      m_position=m_positionError*m_position;
    }
  }



  RecVertexPositions &RecVertexPositions::operator= (const RecVertexPositions& rhs)
  {
    if (this!=&rhs)
    {
      // you need to call the base class assignment operator
      VertexPositions::operator=(rhs);
      m_positionError = rhs.m_positionError;
      m_fitQuality = rhs.m_fitQuality;
    }
    return *this;
  }

  MsgStream& RecVertexPositions::dump(MsgStream& sl) const {
    sl << "Trk::RecVertexPositions: " << std::endl;
    //VertexOnJetAxis::dump(sl);
 
    if (m_useWeightTimesPosition) {
      sl << "Trk::VertexPositions weight times position: (" ;
    } else {
      sl << "Trk::VertexPositions position: (" ;
    }
    sl << "xv " << m_position(jet_xv) << "+/-" << sqrt(m_positionError(jet_xv,jet_xv)) << " , "
       << "yv " << m_position(jet_yv) << "+/-" << sqrt(m_positionError(jet_yv,jet_yv)) <<", "
       << "zv " << m_position(jet_zv) << "+/-" << sqrt(m_positionError(jet_zv,jet_zv)) <<", "
       << "phi " << m_position(jet_phi) << "+/-" << sqrt(m_positionError(jet_phi,jet_phi)) <<", "
       << "theta " << m_position(jet_theta) << "+/-" << sqrt(m_positionError(jet_theta,jet_theta)) << endreq;
    if (!m_useWeightTimesPosition) {
      for (int i=5;i<m_position.rows();i++) {
	sl << "dist" << i << " " << m_position(i) << "+/-" << sqrt(m_positionError(i,i))<<" , ";
      }
    } else {
      for (int i=5;i<m_position.rows();i++) {
	sl << "dist corrected" << i << " " << (m_positionError*m_position).eval()(i) << "+/-" << sqrt(m_positionError(i,i))<<" , ";
      }
    }
    sl << endreq;
   
   sl << m_positionError << std::endl;
   sl << "  ndf: " << m_fitQuality.doubleNumberDoF() << "\t chi2: " << m_fitQuality.chiSquared() << endreq;
   return sl;
  }

  std::ostream& RecVertexPositions::dump(std::ostream& sl) const {
    //    sl << "Trk::RecVertexPositions: " << std::endl;
    if (m_useWeightTimesPosition) {
      sl << "Trk::VertexPositions weight times position: (" ;
    } else {
      sl << "Trk::VertexPositions position: (" ;
    }
    sl << "xv " << m_position(jet_xv) << "+/-" <<sqrt(this->covariancePosition()(jet_xv,jet_xv)) << " , "
       << "yv " << m_position(jet_yv) << "+/-" <<sqrt(this->covariancePosition()(jet_yv,jet_yv)) <<", "
       << "zv " << m_position(jet_zv) << "+/-" <<sqrt(this->covariancePosition()(jet_zv,jet_zv)) <<", "
       << "phi " << m_position(jet_phi) << "+/-"<<sqrt(this->covariancePosition()(jet_phi,jet_phi)) <<", "
       << "theta " << m_position(jet_theta)<<"+/-"<<sqrt(this->covariancePosition()(jet_theta,jet_theta))
       << std::endl;
    if (!m_useWeightTimesPosition) {
      for (int i=5;i<m_position.rows();i++) {
	sl << "dist" << i << " " << m_position(i) << "+/-" << sqrt(this->covariancePosition()(i,i))<<" , ";
      }
    } else {
      for (int i=5;i<m_position.rows();i++) {
	sl << "dist corrected" << i << " " << (m_positionError*m_position).eval()(i) << "+/-" << sqrt(m_positionError(i,i))<<" , ";
      }
    }
    sl << std::endl;
    
    VertexPositions::dump(sl);
    sl << m_positionError << std::endl;
    sl << "  ndf: " << m_fitQuality.doubleNumberDoF() << "\t chi2: " << m_fitQuality.chiSquared() << std::endl;
    return sl;
  }

  RecVertexPositions::~RecVertexPositions() {}
 
  const Trk::FitQuality& RecVertexPositions::fitQuality() const
  {return m_fitQuality;}


  const Amg::MatrixX& RecVertexPositions::covariancePosition() const
  {return m_positionError; }

  
  void RecVertexPositions::setCovariancePosition(const Amg::MatrixX & newerror) {
    m_positionError=newerror;
  }
  
  void RecVertexPositions::setFitQuality(const Trk::FitQuality & newFitQuality) {
    m_fitQuality=newFitQuality;
  }

  

} // end of namespace
