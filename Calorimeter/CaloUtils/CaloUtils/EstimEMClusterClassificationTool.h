/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_ESTIMEMCLUSTERCLASSIFICATIONTOOL_H
#define CALOUTILS_ESTIMEMCLUSTERCLASSIFICATIONTOOL_H
//
// Author List:
//     stavina@mppmu.mpg.de 
//     fekete@melkor.dnp.fmph.uniba.sk
//
//
/**
 * @class EstimEMClusterClassificationTool
 *
 * @author stavina@mppmu.mpg.de
 * @author fekete@melkor.dnp.fmph.uniba.sk
 * @date February 2006
 *
 * @brief classify clusters according to their estimated EM component of deposited energy 
 *
 * concrete class implementing a IClusterClassificationTool to
 * classify clusters according to their electromagnetic component of deposited energy
 * calculated using three topological moments */

#include "CaloInterface/IClusterClassificationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>

class EstimEMClusterClassificationTool : public AthAlgTool, virtual public IClusterClassificationTool
{
 public:


  CaloRecoStatus::StatusIndicator classify(xAOD::CaloCluster* thisCluster);
  StatusCode initialize();

  virtual StatusCode LoadConditionsData(IOVSVC_CALLBACK_ARGS)
    { return StatusCode::SUCCESS;}

  EstimEMClusterClassificationTool(const std::string& type, 
				  const std::string& name,
				  const IInterface* parent);

  //functions for retrieving data from params array
  //index is from 0 -> 2
  //slice is from 0 ->24
  //in case of wrong index number or wrong slice number result is unknown
  double GetA(unsigned int index, unsigned int slice);
  double GetB(unsigned int index, unsigned int slice);
  double GetLL(unsigned int slice);
  double GetLC(unsigned int slice);
  double GetRR(unsigned int slice);
 private:
  
  // cut to define EM cluster  <0;1> (% / 100)
  double m_maxEstimEM;

  //parameters for 25 slices
  //structure of array:
  //a1,a2,a3,b1,b2,b3,ll,lc,rr '* 25' = 9values * 25 slices = 225 values
  std::vector<double> m_params;
  std::vector<double> m_eta_ene_cut;
  
  bool m_debug;

};

#endif


