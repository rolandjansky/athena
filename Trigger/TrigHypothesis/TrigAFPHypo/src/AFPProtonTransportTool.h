/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H
#define TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "IAFPProtonTransportTool.h"
#include "AFPProtonTransportParam.h"

#include <string>

/**
 * @brief Class for the tool that parameterizes proton transport 
 *
 * This tool does the transport of the proton from ATLAS to 
 * the AFP detector, using a parameterization loaded from a txt file.
 * Given as input the initial x, y and z components of the proton position
 * and the initial x, y and z components of the proton momentum it returns
 * the predicted position of the proton with respect to the AFP near station
 */

class AFPProtonTransportTool : public AthAlgTool, virtual public IAFPProtonTransportTool {
public:
  AFPProtonTransportTool(const std::string & type, const std::string & name, const IInterface* parent);

  virtual ~AFPProtonTransportTool();
  
  virtual StatusCode initialize();
  
  /// Loads parametrization file and gets the value of positions, slopes and nominal energy. It takes as argument the path to the txt file containing this information.
  StatusCode load();

  void setParamFile(std::string paramFile) { m_filename = paramFile; }

  /// Evaluates value of the horizontal position equation. It takes as arguments the initial position, slope and nominal beam energy.
  double x(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_x_position->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of vertical position equation. It takes as arguments the initial position, slope and nominal beam energy.
  double y(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_y_position->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of horizontal slope equation. It takes as arguments the initial position, slope and nominal beam energy.
  double sx(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_x_slope->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                           sy0 + m_ySlopeInitIP, E);
  }

  /// Evaluates value of vertical slope equation. It takes as arguments the initial position, slope and nominal beam energy.
  double sy(double x0, double y0, double z0, double sx0, double sy0, double E) const {
   return m_y_slope->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP, sy0 + m_ySlopeInitIP, E);
                                                                                                                                                 }

  /// Returns the detector position for which parametrization was performed.
  double parametrisationPosition() const {return m_parametrisationPosition;}

  /// Returns nominal beam energy for which parametrization was performed.
  double energy() const {return m_energy;};
private:

  ///AFPProtonTransportParam object used for the x position evaluation
  AFPProtonTransportParam* m_x_position;
  ///AFPProtonTransportParam object used for the y position evaluation
  AFPProtonTransportParam* m_y_position;
  ///AFPProtonTransportParam object used for the x slope
  AFPProtonTransportParam* m_x_slope;
  ///AFPProtonTransportParam object used for the y slope
  AFPProtonTransportParam* m_y_slope;

  /// Beam crossing angle from x axis at the interaction point
  Gaudi::Property<double> m_xSlopeInitIP {this, "xSlopeInitIp", 0, "Initial x slope at IP"};
  /// Beam crossing angle from y axis at the interaction point
  Gaudi::Property<double> m_ySlopeInitIP {this, "ySlopeInitIp", 0, "Initial y slope at IP"}; 
  /// Proton's initial x position at the interaction point
  Gaudi::Property<double> m_xPositionInitIP {this, "xPositionInitIp", 0, "Initial x position at IP"};
  /// Proton's initial y position at the interaction point
  Gaudi::Property<double> m_yPositionInitIP {this, "yPositionInitIp", 0, "Initial y position at IP"};
  /// Proton's initial z position at the interaction point
  Gaudi::Property<double> m_zPositionInitIP {this, "zPositionInitIp", 0, "Initial z position at IP"};
  /// Nominal beam energy
  Gaudi::Property<double> m_energy {this, "beamEnergy", 0, "Nominal beam energy"}; 
  /// Detector position for which the parameterisation has been done
  Gaudi::Property<double> m_parametrisationPosition {this, "parameterisationPosition", 0, "Detector position for which parameterisation was derived"}; 
  
  Gaudi::Property<std::string> m_filename {this, "filename", "", "Parameterisation file"};

};

#endif
