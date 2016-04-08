/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
// filename: MonitoredTrackParticle.h
//
// author: Arthur.Bolz@cern.ch, 2016
//
// Description: Class to store TrackParticle parameters in std::vectors and provide
//              them for monitoring by a monitored HLT algorithm in the HLT monitoring scheme
//***************************************************************************

#ifndef __TRIGFTK_TRIGFTK_MONITORING_MONITOREDTRACKPARTICLE_H__
#define __TRIGFTK_TRIGFTK_MONITORING_MONITOREDTRACKPARTICLE_H__

// HLT algo includes
#include "TrigInterfaces/IMonitoredAlgo.h"

// xAOD::TrackParticle includes
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

// std includes
#include <vector>
#include <string>

namespace TrigFTK{

  /*
   * @brief Class to monitor xAOD::TrackParticle parameters within the HLT monitoring framework 
   */
  class MonitoredTrackParticle 
  {
    public:
        /*
       * @brief constructor
       */
      MonitoredTrackParticle(void);
      /*
       * @brief destructor 
       */
      ~MonitoredTrackParticle(void) {};

      /*
       * @brief initialize parameters for 2d -> 1d track parameter projections
       * @param n_eta number of eta bins
       * @param eta_min
       * @param eta_max
       * @param z0_min
       * @param z0_max
       */
      void initialize(int n_eta, double eta_min, double eta_max, double z0_min, double z0_max);

       /*
       * @brief declare member std::vectors for monitoring by an HLT algo
       * @param p monitored HLT algorithm
       * @param prefix
       * @param suffix
       */
      void declareForMonitoring( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" );

      /*
       * @brief fill track parameter vectors
       * @param track
       */
      void push_back(const xAOD::TrackParticle* track);

      /*
       * @brief allocate memory for parameter vectors
       * @param size
       */
      void reserve(int size);

      /* 
       * @brief clear parameter vectors
       */
      void clear(void);


    private:
      /*
       * @brief calculate eta bin from eta, m_2d_n_eta, m_2d_eta_min, and m_2d_eta_max
       * @param eta
       */
      int getEtaBin(const double &eta);
      /*
       * @brief combine eta and phi to continuous variable phi + BinEta * 2Pi
       * @param eta
       * @param phi
       */
      double getEtaPhi(const double &eta, const double &phi);
      /*
       * @brief combine eta and z0 to continuous variable phi + BinEta * (z0_max-z0_min)
       * @param eta
       * @param z0
       */
      double getEtaZ0(const double &eta, const double &z0);


      // track parameter vectors to be monitored
      std::vector<double> m_eta_vec;
      std::vector<double> m_phi_vec;
      std::vector<double> m_pt_vec;
      std::vector<double> m_z0_vec;
      std::vector<double> m_d0_vec;
      std::vector<double> m_eta_phi_vec;
      std::vector<double> m_eta_z0_vec;

      std::vector<uint8_t> m_nPix_vec;
      std::vector<uint8_t> m_nSct_vec;
      std::vector<uint8_t> m_nSi_vec;    
      std::vector<double> m_chi2Ndf_vec;


      // 2d to 1d projection variables and vectors
      // don't forget to adjust histograms accordingly
      int m_2d_n_eta;
      double m_2d_eta_min;
      double m_2d_eta_max;
      double m_2d_z0_min;
      double m_2d_z0_max;
  }; // end class



   /*
   * @brief Class to monitor a pair of xAOD::TrackParticles 
   */
  class MonitoredTrackParticlePair 
  {
    public:
        /*
       * @brief constructor
       */
      MonitoredTrackParticlePair(void) {};
      /*
       * @brief destructor 
       */
      ~MonitoredTrackParticlePair(void) {};

      /*
       * @brief declare member std::vectors for monitoring by an HLT algo
       * @param p monitored HLT algorithm
       * @param prefix
       * @param suffix
       */
      void declareForMonitoring( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" );

      /*
       * @brief fill track parameter vectors
       * @param track
       */
      void push_back(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2);

      /*
       * @brief allocate memory for parameter vectors
       * @param size
       */
      void reserve(int size);

      /* 
       * @brief clear parameter vectors
       */
      void clear(void);


    private:
      // track parameter vectors to be monitored
      std::vector<double> m_delta_eta_vec;
      std::vector<double> m_delta_phi_vec;
      std::vector<double> m_delta_pt_vec;
      std::vector<double> m_delta_z0_vec;
      std::vector<double> m_delta_d0_vec;
      std::vector<double> m_delta_R_vec;
      
  }; // end class


}// end namespace

#endif
