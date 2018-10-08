/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Common Muon Detector Element properties
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonDetectorElement.h 
//<version>	$Name: not supported by cvs2svn $


//!  A MuonDetectorElement Class
/*!
  MuonDetectorElements are as granular as offline muon data collections
  (prepRawData); hence they can be retrieved using the collection hash 
  identifier.  MuonDetectorElement will hold an array of MuonReadoutElements 
  which will actually implement the real functionality; MuonDetectorElement 
  methods will delegate the job to the appropriate MuonReadoutElement.  
*/


#ifndef MUONGEOMODEL_MUONDETECTORELEMENT_H
# define MUONGEOMODEL_MUONDETECTORELEMENT_H

#include <string>
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/MsgStreamMember.h"


class MsgStream;
class GeoPhysVol;

namespace MuonGM {

class MuonStation;
class MuonDetectorManager;

/**
   Base class for the XxxDetectorelement, 
   with Xxx = Mdt, Rpc, Tgc, Csc. It's a Trk::TrkDetElementBase,
   therefore it must implement the generic tracking interfaces requested 
   to the geometry: center, normal, surfaceBound, transform.  It is 
   synched to the raw geometry via the pointer, required by the constructor, 
   to its corresponding GeoVFullPhysVol.  A MuonDetectorElement holds a 
   pointer to its parent MuonStation.
*/

class MuonDetectorElement: public Trk::TrkDetElementBase 
{

public:    

   MuonDetectorElement(GeoVFullPhysVol* pv, MuonDetectorManager* mgr, Identifier id, IdentifierHash idHash);
   virtual ~MuonDetectorElement();
    
    //Declaring the Message method for further use
    MsgStream& msg( MSG::Level lvl ) const ;
    //Declaring the Method providing Verbosity Level
    bool msgLevel( MSG::Level lvl ) ;

   // Identification methods
   Identifier identify() const {return m_id;}//!< returns the collection extended identifier (EDM granularity)
   IdentifierHash identifyHash() const {return m_idhash;} ;   //!< returns the collectionHash (EDM granularity)
   virtual int getStationEta() const = 0; //!< returns stationEta 
   virtual int getStationPhi() const = 0; //!< returns stationPhi
    
   // DetectorElement content
   unsigned int nReadoutElements() const {return m_nREinDetectorElement ;}
   //!< returns the number of ReadoutElements collected under this detector
   // Common tracking generic interfaces

   virtual unsigned int nMDTinStation() const = 0; 
   virtual unsigned int nCSCinStation() const = 0; 
   virtual unsigned int nTGCinStation() const = 0; 
   virtual unsigned int nRPCinStation() const = 0;     

    //   inline MsgStream& reLog() const;

protected:    

   // compute from MuonStation ---- 
   //    double m_Ssize, m_Rsize, m_Zsize, m_LongSsize, 
   //    m_LongRsize, m_LongZsize; //!< size in the specified direction
   inline const MuonDetectorManager* manager() const; 
   // hold in RE --- std::string       m_techname; //!< MDT or RPC or TGC or CSC plus a two digits subtype; example RPC17
   // hold in RE --- std::string       m_statname; //!< examples are BMS5, CSS1, EML1
   Identifier        m_id;       //!< extended data-collection identifier 
   IdentifierHash    m_idhash;   //!< data-collection hash identifier

   unsigned int m_nREinDetectorElement;
    
    //Declaring private message stream member.
    mutable Athena::MsgStreamMember m_msg ;

    //MsgStream*      m_MsgStream;
   bool m_debug;
   bool m_verbose;
    
private:

     MuonDetectorManager* m_muon_mgr;

};

// MsgStream& MuonDetectorElement::reLog() const
//   {return *m_MsgStream;} 
const MuonDetectorManager* MuonDetectorElement::manager() const
  {return m_muon_mgr;}
 
} // namespace MuonGM

#endif // MUONGEOMODEL_MUONDETECTORELEMENT_H
