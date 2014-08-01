/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUOND3PDMAKER_TRACKRECORDFILLERTOOL_H
#define MUOND3PDMAKER_TRACKRECORDFILLERTOOL_H

// EDM include(s):
#include "TrackRecord/TrackRecordCollection.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PD {

   /**
    *  @short Filler tool for the basic properties of TrackRecord
    *
    *         This tool can be used to save the simple properties of an
    *         TrackRecord object.
    *
    */
   class TrackRecordFillerTool : public BlockFillerTool< TrackRecord > {

   public:
      /// Regular Gaudi AlgTool constructor
      TrackRecordFillerTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Function booking the ntuple variables
      virtual StatusCode book();
      /// Function filling the ntuple variables for a single object
      virtual StatusCode fill( const TrackRecord& obj );

   private:

      int*   m_barcode;
      int*   m_pdgid;
      float* m_energy;
      float* m_pt;
      float* m_eta;
      float* m_phi;
      float* m_posx;
      float* m_posy;
      float* m_posz;
      float* m_time;

      // Property: fill info for only muons or all particles entering Muon Spectrometer?
      bool m_OnlyMuon;
      // Property: minimum particle energy
      bool m_EnergyThreshold;
   }; // class TrackRecordFillerTool

} // namespace D3PD

#endif // MUOND3PDMAKER_TRACKRECORDFILLERTOOL_H
