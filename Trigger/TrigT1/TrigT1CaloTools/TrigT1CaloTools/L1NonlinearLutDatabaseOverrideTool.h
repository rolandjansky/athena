// -*- C++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file L1NonlinearLutDatabaseOverrideTool.h
 * @author Veit Scharf <veit.scharf@cern.ch>
 * @date Feb, 2016
 * @brief Modifies the PprChanCalib database folders to use the nonlinear LUT.
 *
 * The PPM simulation uses several database folders as inputs. This tool
 * modifies the content of the PprChanCalib folder such that it configures the
 * nMCM module to use the nonlinear JEP Lut.
 */
#ifndef LVL1L1NONLINEARLUTDATABASEOVERRIDETOOL_H
#define LVL1L1NONLINEARLUTDATABASEOVERRIDETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigT1CaloToolInterfaces/IL1DatabaseOverrideTool.h"

namespace LVL1 {

class L1NonlinearLutDatabaseOverrideTool : virtual public IL1DatabaseOverrideTool,
                                                   public AthAlgTool
{
public:
  L1NonlinearLutDatabaseOverrideTool(const std::string&,const std::string&,const IInterface*);
  /** default destructor */
  virtual ~L1NonlinearLutDatabaseOverrideTool();
  
  StatusCode modifyChanDefaults(L1CaloPprChanDefaultsContainer&);
  StatusCode modifyChanCalib(L1CaloPprChanCalibContainer&);
  StatusCode modifyDisabledTowers(L1CaloDisabledTowersContainer&);
  StatusCode modifyDeadChannels(L1CaloPpmDeadChannelsContainer&);
private:
  double m_offset        = 0.;
  double m_slope         = 0.;
  double m_amplitude     = 0.;
  double m_exponent      = 0.;
  double m_noiseFraction = 0.;
  unsigned short m_strategy = 3;
};

} // end of namespace

#endif // LVL1L1NONLINEARLUTDATABASEOVERRIDETOOL_H
