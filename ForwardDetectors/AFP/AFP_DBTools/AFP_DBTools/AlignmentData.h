/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AlignmentData.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-12-03
 * 
 * @brief  File with implementation of AlignmentData.
 */

#ifndef AFP_DBTOOLS_ALIGNMENTDATA_H
#define AFP_DBTOOLS_ALIGNMENTDATA_H


namespace AFP
{

  /// Class storing information about alignment.
  class AlignmentData 
  {
  public:
    AlignmentData (double xShift, double yShift, double zShift, double alpha = 0, double beta = 0, double gamma = 0) :
      m_xShift (xShift), m_yShift (yShift), m_zShift (zShift), m_alpha (alpha), m_beta (beta), m_gamma (gamma)
    {}
  
    double xShift () const {return m_xShift;}
    double yShift () const {return m_yShift;}
    double zShift () const {return m_zShift;}
    double alpha () const {return m_alpha;}
    double beta () const {return m_beta;}
    double gamma () const {return m_gamma;}

  private:
    double m_xShift;
    double m_yShift;
    double m_zShift;
    double m_alpha;
    double m_beta;
    double m_gamma;
  };

} // namespace AFP

#endif	// AFP_DBTOOLS_ALIGNMENTDATA_H
