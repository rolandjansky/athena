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

class MsgStream;

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
    std::string m_input_tracks_collection; ///< input tracks collection name

    /** defgroup kinematic-selections 
     * @{ */

    float m_trk_leading_pt; ///< min. p_{T} of leading track
    float m_trk_subleading_pt; ///< min. p_{T} of sub-leading track
    float m_trk_eta_max; ///< max |eta| for tracks consideration
    float m_trk_d0_max; ///< max |d0| for tracks consideration
    float m_max_delta_z; ///< maximum delta z0 between leading tracks pair
    float m_z0_window; /// width of z0 window
    
    /** @} */
	
    static bool tracks_pt_greater_than(const Trk::Track* const &track1, const Trk::Track* const &track2)
      {
	float theta1 = track1->perigeeParameters()->parameters()[Trk::theta];
	float ptinv1 = fabs(track1->perigeeParameters()->parameters()[Trk::qOverP]) / sin(theta1);
	float theta2 = track2->perigeeParameters()->parameters()[Trk::theta];
	float ptinv2 = fabs(track2->perigeeParameters()->parameters()[Trk::qOverP]) / sin(theta2);
	//return less than of inverse 
	return (ptinv1 < ptinv2);
      }

  }; // ZWindowRoISeedTool
} //InDet namespace

#endif // SiSpacePointsSeedMaker_ZWindowRoISeedTool

