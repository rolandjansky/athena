/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////
//                                             //
//  File:  EventBoost.h                        //
//                                             //
//  Algorithm designed to                      //
//  boost the HepMC record according           //
//  beam inclination.                          //
//                                             //
//  Author: Esben Klinkby <klinkby@nbi.dk>     //
//                                             //
/////////////////////////////////////////////////

#ifndef EVENTBOOST_H
#define EVENTBOOST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "HepMC/GenEvent.h"


namespace HepMC {
  class GenParticle;
}


#include <string>


class EventBoost: public AthAlgorithm {
public:

  //Standard algorithm methods:

  EventBoost( const std::string& name,
			ISvcLocator* pSvcLocator) ;
			 
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode Analyse_BeginEvent();
  StatusCode Analyse_EndEvent();
  StatusCode GenAnalysis_initialize();
  StatusCode GenAnalysis_finalize();
  StatusCode AnalyseGenEvent(const HepMC::GenEvent*);
  StatusCode EventCopy(const HepMC::GenEvent* evt) const;

  bool doModification(HepMC::GenParticle * part, double& pxsum);
  bool doVertexModification(HepMC::GenVertex * ver, double rand_x, double rand_y, double rand_z); 

private:

  //////////// INTERNALS ////////////

  
  int m_nModifiedEvent;
  int m_nFailedEvent;
  int m_nModifiedTotal;
  int m_nFailedTotal;

  double m_beam_inclination;

  std::string m_genEvtInKey;
  std::string m_genEvtOutKey;
  bool m_gaussian_vertex_smearing;
  bool m_flat_vertex_smearing;

  double m_flat_rand_x;
  double m_flat_rand_y;
  double m_flat_rand_z;
  double m_gauss_rand_x;
  double m_gauss_rand_y;
  double m_gauss_rand_z;

  double m_pxsum;

  std::vector<double> m_gaussian_width;
  std::vector<double> m_gaussian_mean;
  std::vector<double> m_flat_smearing_boundary_min;
  std::vector<double> m_flat_smearing_boundary_max;

};

#endif
