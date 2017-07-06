/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H
#define TRIGAFPHYPO_AFPPROTONTRANSPORTTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigAFPHypo/AFPProtonTransportParam.h"

#include <string>

class AFPProtonTransportTool
{
public:
  AFPProtonTransportTool(std::string filename);

  virtual ~AFPProtonTransportTool();

  /*! Loads parametrization file. */
  int load(std::string filename);


  /*! Evaluates value of horizontal position equation. */
  double x(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_px->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /*! Evaluates value of vertical position equation. */
  double y(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_py->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                          sy0 + m_ySlopeInitIP, E);
  }

  /*! Evaluates value of horizontal slope equation. */
  double sx(double x0, double y0, double z0, double sx0, double sy0, double E) const {
    return m_psx->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP,
                           sy0 + m_ySlopeInitIP, E);
  }

  /*! Evaluates value of vertical slope equation. */
  double sy(double x0, double y0, double z0, double sx0, double sy0, double E) const {
   return m_psy->evaluate(x0 + m_xPositionInitIP, y0 + m_yPositionInitIP, z0 + m_zPositionInitIP, sx0 + m_xSlopeInitIP, sy0 + m_ySlopeInitIP, E);
                                                                                                                                                 }

  /*! Returns position for which parametrization was performed. */
  double parametrisationPosition() const {return m_parametrisationPosition;}

  /*! Returns nominal energy for which parametrization was performed. */
  double energy() const {return m_energy;};
private:
  AFPProtonTransportParam* m_px;
  AFPProtonTransportParam* m_py;
  AFPProtonTransportParam* m_psx;
  AFPProtonTransportParam* m_psy;

  double m_xSlopeInitIP, m_ySlopeInitIP; // crossing angles
  double m_xPositionInitIP, m_yPositionInitIP, m_zPositionInitIP;

  double m_energy; // nominal beam energy
  double m_parametrisationPosition; // position for which the parameterisation has been done
};

#endif
