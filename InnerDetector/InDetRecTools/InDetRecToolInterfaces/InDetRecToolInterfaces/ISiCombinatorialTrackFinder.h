/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class ISiCombinatorialTrackFinder
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
//  Base class for combinatorial track-finding in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 06/04/2007 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef ISiCombinatorialTrackFinder_H
#define ISiCombinatorialTrackFinder_H

#include <list>
#include <map>
#include "GaudiKernel/AlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkTrack/Track.h"

class MsgStream;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  class TrackQualityCuts;

  static const InterfaceID IID_ISiCombinatorialTrackFinder
    ("InDet::ISiCombinatorialTrackFinder",1,0);

  class ISiCombinatorialTrackFinder : virtual public IAlgTool 
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:

      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      static const InterfaceID& interfaceID();
      virtual StatusCode initialize ()=0;
      virtual StatusCode finalize   ()=0;

      ///////////////////////////////////////////////////////////////////
      // Main methods for track-finding
      ///////////////////////////////////////////////////////////////////

      
      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 const TrackQualityCuts&)=0;

      virtual const std::list<Trk::Track*>& getTracks
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&)=0;

      virtual const std::list<Trk::Track*>& getTracksWithBrem
	(const Trk::TrackParameters&, 
	 const std::list<const Trk::SpacePoint*>&,
	 const std::list<Amg::Vector3D>&,
	 std::vector<const InDetDD::SiDetectorElement*>&,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
	 bool)=0;

      virtual void newEvent()=0;
      virtual void newEvent(Trk::TrackInfo,const TrackQualityCuts&)=0;

      virtual void endEvent()=0;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////
     
      virtual MsgStream&    dump(MsgStream&    out) const=0;
      virtual std::ostream& dump(std::ostream& out) const=0;
     
    };
  
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
  
  MsgStream&    operator << (MsgStream&   ,const ISiCombinatorialTrackFinder&);
  std::ostream& operator << (std::ostream&,const ISiCombinatorialTrackFinder&);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& ISiCombinatorialTrackFinder::interfaceID()
    {
      return IID_ISiCombinatorialTrackFinder;
    }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    << 
    (MsgStream& sl,const ISiCombinatorialTrackFinder& se)
    { 
      return se.dump(sl); 
    }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
  
  inline std::ostream& operator << 
    (std::ostream& sl,const ISiCombinatorialTrackFinder& se)
    { 
      return se.dump(sl); 
    }   


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

