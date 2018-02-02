/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////
//                                  //
//  Class: TRT_SimDriftTimeTool     //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
////////////////////////////////////

#ifndef TRT_SIMDRIFTTIMETOOL_H
#define TRT_SIMDRIFTTIMETOOL_H
#include "TRT_Digitization/ITRT_SimDriftTimeTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

class IAtRndmGenSvc;

class TRT_SimDriftTimeTool : public AthAlgTool,  virtual public ITRT_SimDriftTimeTool
{
 public:

  // Standard Constructor:
  TRT_SimDriftTimeTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  // ITRT_SimDriftTimeTool interface implementation:
  virtual double getAverageDriftTime( const double& distIndex,
                                      const double& effectivefield_squared,
                                      int strawGasType) const;

  // override intialize() method:
  virtual StatusCode initialize();

 private:

  const double m_minDistance;
  const double m_maxDistance;
  double m_maxField;
  double m_maxFieldSquaredLimit;
  double m_invMaxFieldSquared;

  unsigned int m_digversion;

  const unsigned int m_nTabulatedDistances;
  const double m_invDeltaTabulatedDistances;

  std::vector < std::vector<double> > m_table_of_dist2meanDT_at_noField;
  std::vector < std::vector<double> > m_table_of_dist2meanDT_at_maxField;

};

#endif
