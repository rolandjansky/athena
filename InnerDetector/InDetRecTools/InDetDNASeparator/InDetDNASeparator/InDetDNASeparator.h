/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetDNASeparator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETINDETDNASEPARATOR_H
#define INDETINDETDNASEPARATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkFitterUtils/TrackBreakpointType.h"
#include "TrkFitterInterfaces/ITrackBreakpointAnalyser.h"
#include "InDetDNASeparator/MeanTrackParsAtFloor.h"
#include <vector>

class PixelID;
class SCT_ID;

namespace Trk {class Track;}

namespace InDet 
{

  /** @brief analyses the KalmanFitter's internal trajectory for
      DNA activity and if such activity has created a brem point or not.

      The confirmBreakpoint() methods work in the following way:

      @returns TrackBreakpointType::NoBreakpoint if DNA never kicked in
      during the fit, hence DNA = kf  (previously "-1")

      @returns TrackBreakpointType::DnaBremPointNotUseful if DNA kicked
      in, but momentum did not change => REFIT without DNA (previously "0")

      @returns TrackBreakpointType::DnaBremPointUseful if DNA kicked in
      and changed momentum => DNA fit is good (previously "+1")

      +1 also means that DNA fit believes the track was a bremming electron.
      
      @author  Vato Kartvelishvili <vakhtang.kartvelishvili@cern.ch>
  */  

  class InDetDNASeparator : virtual public Trk::ITrackBreakpointAnalyser,
    public AthAlgTool
    {
    public:
      InDetDNASeparator(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~InDetDNASeparator ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      //! see ITrackBreakpointAnalyser for documentation. 
      virtual Trk::TrackBreakpointType confirmBreakpoint(const Trk::Trajectory&) const;

      virtual double breakpointSignificance(const Trk::Trajectory&) const;

      virtual Trk::TrackBreakpointType confirmBreakpoint(const Trk::Track&) const;

      double breakpointSignificance(const Trk::Track&) const;
      
    private:

      //! method calculating the dna-separation variables on the floor-projected track pars
      std::vector<double> calculateSignificance
        (const std::vector<Trk::MeanTrackParsAtFloor>&) const;

      //! Cutoff used in the Separator (VK)
      double  m_kutoff;
      //!< min distance in r_xy between brem-floors
      const double m_floorseparation_drr;
      
      //! ID SCT helper
      const SCT_ID* m_SCT_Tool;

      //! ID Pixel helper
      const PixelID* m_pixelTool;

      //! bottom floor constant for max() logic and flag for no-DNA
      static const double s_sigmaNoDna;

    }; 
} // end of namespace

#endif 
