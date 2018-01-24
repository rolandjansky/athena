/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_RadiationMapsMaker_H
#define G4UserActions_RadiationMapsMaker_H

#include <vector>
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"

class TGraph;

namespace G4UA
{
  
  class RadiationMapsMaker: 
    public G4UserRunAction,
    public G4UserSteppingAction
    {
    
    public:

      /// @brief Simple struct for holding the radiation maps
      struct Report
      {
	const int nBinsr = 120;
	const int nBinsz = 240;
	
	const double rMinZoom =    0.; // cm
	const double rMinFull =    0.; // cm
	
	const double rMaxZoom =  480.; // cm
	const double rMaxFull = 1200.; // cm
	
	const double zMinZoom =    0.; // cm
	const double zMinFull =    0.; // cm
	
	const double zMaxZoom =  960.; // cm
	const double zMaxFull = 2400.; // cm

	const int nBinsr3d   = 30;
	const int nBinsz3d   = 60;
	const int nBinsphi3d = 32;
	
	const double phiMinZoom = -180.; // degrees
	const double phiMaxZoom =  180.; // degrees

        /// vector of tid seen by thread in zoomed area
	std::vector<double> m_rz_tid;
        /// vector of ionizing energy density seen by thread in zoomed area
	std::vector<double> m_rz_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in zoomed area
	std::vector<double> m_rz_niel;
        /// vector of >20 MeV hadron flux seen by thread in zoomed area
	std::vector<double> m_rz_h20;

        /// vector of tid seen by thread in full area
	std::vector<double> m_full_rz_tid;
        /// vector of ionizing energy density seen by thread in full area
	std::vector<double> m_full_rz_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in full area
	std::vector<double> m_full_rz_niel;
        /// vector of >20 MeV hadron flux seen by thread in full area
	std::vector<double> m_full_rz_h20;

        /// vector of tid seen by thread in 3d
	std::vector<double> m_3d_tid;
        /// vector of ionizing energy density seen by thread in 3d
	std::vector<double> m_3d_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in 3d
	std::vector<double> m_3d_niel;
        /// vector of >20 MeV hadron flux seen by thread in 3d
	std::vector<double> m_3d_h20;

	void merge(const Report& maps);
      };

      // initialize maps to 0
      virtual void BeginOfRunAction(const G4Run*) override final;

      // increment radiation maps
      virtual void UserSteppingAction(const G4Step*) override final;

      /// Retrieve my maps
      const Report& getReport() const
      { return m_maps; }

    private:

      Report m_maps;

      TGraph * m_tgpSiA = 0;
      TGraph * m_tgpSiB = 0;
      TGraph * m_tgnSiA = 0;
      TGraph * m_tgnSiB = 0;
      TGraph * m_tgpiSi = 0;
      
    }; // class RadiationMapsMaker
  
  
} // namespace G4UA 

#endif
