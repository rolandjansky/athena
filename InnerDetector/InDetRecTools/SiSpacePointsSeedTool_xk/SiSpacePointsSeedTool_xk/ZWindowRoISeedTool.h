/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for Z-window RoI from leading two track middle vertex z-.
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedTool_xk_ZWindowRoISeedTool_h
#define SiSpacePointsSeedTool_xk_ZWindowRoISeedTool_h

#include "InDetRecToolInterfaces/IZWindowRoISeedTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrack/Track.h"

#include <vector>
#include <string>

namespace Reco 
{ 
  class ITrackToVertex; 
}

namespace InDet {

  class ZWindowRoISeedTool : 
   virtual public IZWindowRoISeedTool ,public AthAlgTool
  {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    ZWindowRoISeedTool(const std::string&,const std::string&,const IInterface*);
    virtual ~ZWindowRoISeedTool();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    /** Compute RoI */
    virtual std::vector<ZWindow> getRoIs();

  private:

    ToolHandle< Reco::ITrackToVertex > m_trackToVertex;

  protected:

    /**    @name Disallow default instantiation, copy, assignment **/
    ZWindowRoISeedTool() = delete;
    ZWindowRoISeedTool(const ZWindowRoISeedTool&) = delete;
    ZWindowRoISeedTool &operator=(const ZWindowRoISeedTool&) = delete;

    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
  
    //* Input track collection *//
    std::string m_inputTracksCollection; ///< input tracks collection name

    /** defgroup kinematic-selections 
     * @{ */

    float m_trkLeadingPt; ///< min. p_{T} of leading track
    float m_trkSubLeadingPt; ///< min. p_{T} of sub-leading track
    float m_trkEtaMax; ///< max |eta| for tracks consideration
    float m_trkD0Max; ///< max |d0| for tracks consideration
    float m_maxDeltaZ; ///< maximum delta z0 between leading tracks pair
    float m_z0Window; /// width of z0 window
    
    /** @} */
	
    static bool tracksPtGreaterThan(const Trk::Track* const &track1, const Trk::Track* const &track2)
      {
	float theta1 = track1->perigeeParameters()->parameters()[Trk::theta];
	float ptinv1 = std::abs(track1->perigeeParameters()->parameters()[Trk::qOverP]) / std::sin(theta1);
	float theta2 = track2->perigeeParameters()->parameters()[Trk::theta];
	float ptinv2 = std::abs(track2->perigeeParameters()->parameters()[Trk::qOverP]) / std::sin(theta2);
	//return less than of inverse 
	return (ptinv1 < ptinv2);
      }

  }; // ZWindowRoISeedTool
} //InDet namespace

#endif // SiSpacePointsSeedMaker_ZWindowRoISeedTool

