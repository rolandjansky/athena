/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiCombinatorialTrackFinder
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 06/04/2007 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiCombinatorialTrackFinder_H
#define ISiCombinatorialTrackFinder_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkTrack/Track.h"
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/EventContext.h"

#include <list>
#include <map>

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  class SiCombinatorialTrackFinderData_xk;
  class TrackQualityCuts;

  /**
   * @class ISiCombinatorialTrackFinder
   *
   * Base class for combinatorial track-finding in Pixels and SCT
   *
   * In AthenaMT, event dependent cache inside ISiCombinatorialTrackFinder
   * is not preferred. SiCombinatorialTrackFinderData_xk class holds
   * event dependent data for ISiCombinatorialTrackFinder.
   * An inherit class, ExtendedSiCombinatorialTrackFinderData_xk,
   * object is a data member of SiTrackMakerEventData_xk.
   * An inherit class, ExtendedSiTrackMakerEventData_xk, is instantiated
   * in SiSPSeededTrackFinder::execute.
   */

  class ISiCombinatorialTrackFinder : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      /// @name InterfaceID
      //@{
      DeclareInterfaceID(ISiCombinatorialTrackFinder, 1, 0);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Main methods for track-finding
      ///////////////////////////////////////////////////////////////////
      //@{
      
      virtual const std::list<Trk::Track*>& getTracks
	(SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::vector<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 const TrackQualityCuts&) const =0;

      virtual const std::list<Trk::Track*>& getTracks
	(SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&, 
	 const std::vector<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&) const =0;

      virtual const std::list<Trk::Track*>& getTracksWithBrem
	(SiCombinatorialTrackFinderData_xk& data,
         const Trk::TrackParameters&,
	 const std::vector<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::list<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*, const Trk::Track*>&,
	 bool) const =0;

      virtual void newEvent(const EventContext& ctx, SiCombinatorialTrackFinderData_xk& data) const =0;

      virtual void newEvent(const EventContext& ctx, SiCombinatorialTrackFinderData_xk& data,
                            Trk::TrackInfo, const TrackQualityCuts&) const =0;

      virtual void endEvent(SiCombinatorialTrackFinderData_xk& data) const =0;
      //@}
      
      virtual void fillStatistic(SiCombinatorialTrackFinderData_xk& data, std::array<bool,SiCombinatorialTrackFinderData_xk::kNCombStats>&) const=0;

      ///////////////////////////////////////////////////////////////////
      /// @name Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
      //@{
      virtual MsgStream& dump(SiCombinatorialTrackFinderData_xk& data, MsgStream& out) const=0;
      //@}     

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  //  Header file for class TrackQualityCuts
  /////////////////////////////////////////////////////////////////////////////////
  
  class  TrackQualityCuts 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      TrackQualityCuts()  {};
      TrackQualityCuts(const TrackQualityCuts&);
      TrackQualityCuts(const std::map<const std::string,int>&,
		       const std::map<const std::string,double>&);
      ~TrackQualityCuts() {};
      TrackQualityCuts& operator = (const TrackQualityCuts&);

      void setIntCut   (const std::string&,int   );
      void setDoubleCut(const std::string&,double);
      
      bool getIntCut   (const std::string&,int&   ) const; 
      bool getDoubleCut(const std::string&,double&) const; 

      int numIntCuts() const; 
      int numDoubleCuts() const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      std::map<const std::string,int>    m_idata;
      std::map<const std::string,double> m_ddata;

    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
  
  inline TrackQualityCuts::TrackQualityCuts(const TrackQualityCuts& TQC)
    {
      *this = TQC;
    }
  
  inline TrackQualityCuts& TrackQualityCuts::operator = (const TrackQualityCuts& TQC) 
    {
      m_idata = TQC.m_idata;
      m_ddata = TQC.m_ddata;
      return(*this);
    }
  
  inline TrackQualityCuts::TrackQualityCuts
    (const std::map<const std::string,int>& in,const std::map<const std::string,double>& db)
    {
      m_idata = in;
      m_ddata = db;
    }

  inline void TrackQualityCuts::setIntCut (const std::string& name, int cut)
    {
      std::map<const std::string,int>::iterator n = m_idata.find(name);
      if(n==m_idata.end()) {m_idata.insert(std::make_pair(name,cut));}
      else                 {           (*n).second = cut;            }
    }

  inline void TrackQualityCuts::setDoubleCut (const std::string& name, double cut)
    {
      std::map<const std::string,double>::iterator n = m_ddata.find(name);
      if(n==m_ddata.end()) {m_ddata.insert(std::make_pair(name,cut));}
      else                 {           (*n).second = cut;            }
    }

  inline bool TrackQualityCuts::getIntCut    (const std::string& name,int& cut) const
    {
      std::map<const std::string,int>::const_iterator n = m_idata.find(name);
      if(n==m_idata.end()) {cut = 0         ; return false;}
                            cut =(*n).second; return true ;
    }

  inline bool TrackQualityCuts::getDoubleCut (const std::string& name,double& cut) const
    {
      std::map<const std::string,double>::const_iterator n = m_ddata.find(name);
      if(n==m_ddata.end()) {cut = 0          ; return false;}
                            cut = (*n).second; return true ;
    }
  
  inline int TrackQualityCuts::numIntCuts() const
    {
      return m_idata.size();
    }

  inline int TrackQualityCuts::numDoubleCuts() const
    {
      return m_ddata.size();
    }

} // end of name space


#endif // ISiCombinatorialTrackFinder_H

