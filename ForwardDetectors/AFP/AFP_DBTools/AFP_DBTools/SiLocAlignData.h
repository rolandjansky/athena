/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiLocAlignData.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-17
 * 
 * @brief  Header file for class SiLocAlignData.
 * 
 */


#ifndef AFP_DBTOOLS_SILOCALIGNDATA_H
#define AFP_DBTOOLS_SILOCALIGNDATA_H


namespace AFP
{

  /// Class storing information about alignment.
  class SiLocAlignData 
  {
  public:
  
    SiLocAlignData(int st, int la): m_stationID(st), m_layerID(la), m_xShift(0.), m_yShift(0.), m_zShift(0.), m_alpha(0.), m_beta(0.), m_gamma(0.) {}
    SiLocAlignData(int st, int la, double x, double y, double z, double a, double b, double g) : m_stationID(st), m_layerID(la), m_xShift(x), m_yShift(y), m_zShift(z), m_alpha(a), m_beta(b), m_gamma(g) {}
    
  
    /// ID of the station to which aligment applies (see xAOD::AFPStationID)
    int stationID () const {return m_stationID;}

    /// ID of the layer in station to which aligment applies (see xAOD::AFPPixelLayerID)
    int layerID () const {return m_layerID;}

    /// Shift of the plane in X direction with respect to the nominal position
    double xShift () const {return m_xShift;}

    /// Shift of the plane in Y direction with respect to the nominal position
    double yShift () const {return m_yShift;}

    /// Shift of the plane in Z direction with respect to the nominal position
    double zShift () const {return m_zShift;}

    /// Rotation angle
    double alpha () const {return m_alpha;}

    /// Rotation angle
    double beta () const {return m_beta;}

    /// Rotation angle
    double gamma () const {return m_gamma;}

  private:
    int m_stationID;
    int m_layerID;
    double m_xShift;
    double m_yShift;
    double m_zShift;
    double m_alpha;
    double m_beta;
    double m_gamma;
  };

} // namespace AFP

#endif	//  AFP_DBTOOLS_SILOCALIGNDATA_H
