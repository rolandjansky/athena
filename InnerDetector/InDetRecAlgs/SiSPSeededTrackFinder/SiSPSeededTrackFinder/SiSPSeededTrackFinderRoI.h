/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SiSPSeededTrackFinderRoI_H
#define SiSPSeededTrackFinderRoI_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h"
#include "InDetRecToolInterfaces/IZWindowRoISeedTool.h"
#include "TrkSpacePoint/SpacePointContainer.h" 

#include "TrkTrack/TrackCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/PerigeeSurface.h" 
#include "StoreGate/DataHandle.h"

namespace InDet {

  // Class-algorithm for track finding in Pixels and SCT
  // initiated by space points seeds filtering in a given
  // RoI within the z axis
  //
  class SiSPSeededTrackFinderRoI : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      SiSPSeededTrackFinderRoI(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~SiSPSeededTrackFinderRoI() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      int                            m_outputlevel        ; // Print level for debug
      int                            m_nprint             ; // Kind of  print    
      int                            m_nseeds             ; // Number seeds
      int                            m_ntracks            ; // Number found tracks
      int                            m_nseedsTotal        ; // Number seeds
      int                            m_ntracksTotal       ; // Number found tracks
      int                            m_neventsTotal       ; // Number events 
      int                            m_problemsTotal      ; // Numbe revents with number seeds > maxNumber
      int                            m_maxNumberSeeds     ; // Max. number used seeds
      int                            m_maxPIXsp           ; // Max. number pixels space points
      int                            m_maxSCTsp           ; // Max. number sct    space points
      int                            m_nfreeCut           ; // Min number free clusters
      bool                            m_doRandomSpot          ; // Low-pT tracking in random spot?

      SG::WriteHandle<TrackCollection>    m_outputTracks    ;

      std::string m_vxOutputName;

      ToolHandle< ISiSpacePointsSeedMaker > m_seedsmaker    ;  // Space poins seed     maker
      ToolHandle< ISiTrackMaker           > m_trackmaker    ;  // Track                maker     
    
      // For new strategy reconstruction
      //
      double                         m_pTcut     ;
      double                         m_imcut     ;

      std::string                                      m_beamconditions          ;
      std::string                                      m_fieldmode               ; 
      IBeamCondSvc*                                    m_beam     ;
      ToolHandle<Trk::IPatternParametersPropagator>    m_proptool ;
      Trk::MagneticFieldProperties                     m_fieldprop;
      ToolHandle<InDet::IZWindowRoISeedTool>           m_ZWindowRoISeedTool;
      ToolHandle<InDet::IZWindowRoISeedTool>           m_RandomRoISeedTool;
      std::vector<InDet::IZWindowRoISeedTool::ZWindow> m_listRoIs;
      std::vector<InDet::IZWindowRoISeedTool::ZWindow> m_listRandRoIs;
      float m_RoIWidth; /// width of z0 window; mostly to be used to bypass RoI if input is negative!

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      
      double trackQuality(const Trk::Track*);
      void filterSharedTracks(std::multimap<double,Trk::Track*>&);
      void magneticFieldInit();

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const SiSPSeededTrackFinderRoI&);
  std::ostream& operator << (std::ostream&,const SiSPSeededTrackFinderRoI&); 
}
#endif // SiSPSeededTrackFinderRoI_H
