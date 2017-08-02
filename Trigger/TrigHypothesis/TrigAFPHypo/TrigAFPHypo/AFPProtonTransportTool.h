/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H
#define TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigAFPHypo/AFPProtonTransportParam.h"

#include <string>

/**
 * @brief Class for the tool that parameterizes proton transport 
 *
 * This tool does the transport of the proton from the central system to 
 * the AFP detector, using a parameterization loaded from a txt file.
 * Given as input the initial x, y and z components of the proton position
 * and the initial x, y and z components of the proton momentum it returns
 * the predicted position of the proton with respect to the AFP near station
 */

class AFPProtonTransportTool
{
public:
  AFPProtonTransportTool(std::string filename);

  virtual ~AFPProtonTransportTool();

  /// Loads parametrization file and gets the value of positions, slopes and nominal energy.
  int load(std::string filename);


  /// Evaluates value of horizontal position equation.
  double x(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_px->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of vertical position equation. 
  double y(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_py->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of horizontal slope equation. 
  double sx(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_psx->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                           sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of vertical slope equation.
  double sy(double x0, double y0, double z0, double sx0, double sy0, double E) const {
   return m_psy->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP, sy0 + m_ySlopeInitIP, E);
                                                                                                                                                 }

  /// Returns position for which parametrization was performed.
  double parametrisationPosition() const {return m_parametrisationPosition;}

  /// Returns nominal energy for which parametrization was performed.
  double energy() const {return m_energy;};
private:

  ///AFPProtonTransportParam object used for the x postion
  AFPProtonTransportParam* m_px;
  ///AFPProtonTransportParam object used for the y postion
  AFPProtonTransportParam* m_py;
  ///AFPProtonTransportParam object used for the x slope
  AFPProtonTransportParam* m_psx;
  ///AFPProtonTransportParam object used for the y slope
  AFPProtonTransportParam* m_psy;

  /// Crossing angle from x axis at the interaction point
  double m_xSlopeInitIP;
  /// Crossing angle from y axis at the interaction point
  double m_ySlopeInitIP; 
  /// Initial x position at the interaction point
  double m_xPositionInitIP;
  /// Initial y position at the interaction point
  double m_yPositionInitIP;
  /// Initial z position at the interaction point
  double m_zPositionInitIP;
  /// Nominal beam energy
  double m_energy; 
  /// Position for which the parameterisation has been done
  double m_parametrisationPosition; 
};

#endif
