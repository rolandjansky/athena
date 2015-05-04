/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store raw Tgc information in                   //
// Author: Zdenko van Kesteren                             //
// Date  : 21st june 2006                                  //
//*********************************************************//

#ifndef MuonCalibRawTgcHit_h
#define MuonCalibRawTgcHit_h

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {
  /**
     @class MuonCalibRawTgcHit
     Calib-EDM version of TgcPrepData class, containing uncalibrated information. 
     - an identifier, stored in the MuonFixedId format (not an Identifier)
     - position in global coordinates
     - occupancy: value denoting how many times this raw hit is assigned to a MuonCalibSegment. When this value yields -1, the raw hit is not assinged to any MuonCalibSegment 
     - strip length and width
   */

  class MuonCalibRawTgcHit {
    public:
    MuonCalibRawTgcHit()
     : m_Gpos(0.,0.,0.), m_occupancy(0), m_station(0), m_eta(0), m_phi(0), m_gasGap(0), m_isStrip(0),
       m_channel(0), m_bcTag(0), m_width(0.), m_shortWidth(0.), m_longWidth(0.), m_length(0.)
    {};//!< default constructor
    MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, int station, int eta, int phi, 
			int gasGap, int isStrip, int channel, int bcTag, float width, float shortWidth, float longWidth ); //!< constructor setting almost all attributes
    MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, int station, int eta, int phi, 
			int gasGap, int isStrip, int channel, int bcTag, float width, float shortWidth, float longWidt, float length ); //!< constructor setting all attributes
    MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float width, float length );
    MuonCalibRawTgcHit( const MuonCalibRawTgcHit& hit ); //!< copy constructor
    ~MuonCalibRawTgcHit() {}; //!< destructor
  
    MuonCalibRawTgcHit& operator=( const MuonCalibRawTgcHit& rhs ); //!< assingment operator

    std::ostream& dump( std::ostream& stream) const;//!< dump to be used for operator<<() to dump the MuonCalibRawTgcHit

    const MuonFixedId& identify()         const { return m_id  ;}       //!< retrieve the MuonFixedId 
    const Amg::Vector3D&  globalPosition()   const { return m_Gpos ;}      //!< retrieve the position expressed in global coordinates
    int                occupancy()        const { return m_occupancy ;} //!< retrieve the occupancy (-1 == not assigned to any segment)
    int                station()          const { return m_station ;}
    int                eta()              const { return m_eta ;}
    int                phi()              const { return m_phi ;}
    int                gasGap()           const { return m_gasGap ;}
    int                isStrip()          const { return m_isStrip ;}
    int                channel()          const { return m_channel ;}
    int                bcTag()            const { return m_bcTag ;}

    float              width()            const { return m_width ;}
    float              shortWidth()       const { return m_shortWidth ;}
    float              longWidth()        const { return m_longWidth ;}
    float              length()           const { return m_length ;}    //!< retrieve the strip length

    void               setId(MuonFixedId Id) { m_id = Id ;}                        //!< sets the MuonFixedId 
    void               setGlobalPosition(const Amg::Vector3D& glob) { m_Gpos = glob;} //!< sets the position expressed in global coordinates
    void               setOccupancy(int occ) { m_occupancy = occ ;}                //!< sets the occupancy
    void               setStation(int station) {m_station = station ;}
    void               setEta(int eta) { m_eta = eta ;}
    void               setPhi(int phi) { m_phi = phi ;}
    void               setGasGap(int gasGap) { m_gasGap = gasGap ;}
    void               setIsStrip(int isStrip) { m_isStrip = isStrip ;}
    void               setChannel(int channel) { m_channel = channel ;}    
    void               setBcTag(int bcTag) { m_bcTag = bcTag ;}    

    void               setWidth(float width) { m_width = width ;}
    void               setShortWidth(float shortWidth) { m_shortWidth = shortWidth ;}
    void               setLongWidth(float longWidth) { m_longWidth = longWidth ;}                  //!< sets the strip width
    void               setLength(float length){ m_length = length ;}              //!< sets the strip length

    private:
    MuonFixedId m_id;         //!< identifier of the raw hit, (is a MuonFixedId, not an Identifier) 
    Amg::Vector3D  m_Gpos;       //!< position of the hit in global coordinates
    int         m_occupancy;  //!< occupancy (-1 == not assigned to any segment)
    int         m_station;
    int         m_eta;
    int         m_phi;
    int         m_gasGap;
    int         m_isStrip;
    int         m_channel;
    int         m_bcTag;

    float       m_width;
    float       m_shortWidth; 
    float       m_longWidth;      //!< strip width
    float       m_length;     //!< strip length

  };

  inline MuonCalibRawTgcHit::MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, float width, float length )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_width(width), m_length(length) {
  }


  inline MuonCalibRawTgcHit::MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, int station, int eta, int phi, 
						 int gasGap, int isStrip, int channel, int bcTag, float width, float shortWidth, float longWidth )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_station(station), m_eta(eta), m_phi(phi), m_gasGap(gasGap), 
      m_isStrip(isStrip), m_channel(channel), m_bcTag(bcTag), m_width(width), m_shortWidth(shortWidth), m_longWidth(longWidth), m_length(0) {
  }

  inline MuonCalibRawTgcHit::MuonCalibRawTgcHit( const MuonFixedId& id, const Amg::Vector3D& glob, int occ, int station, int eta, int phi, 
						 int gasGap, int isStrip, int channel, int bcTag, float width, float shortWidth, float longWidth, float length )
    : m_id(id), m_Gpos(glob), m_occupancy(occ), m_station(station), m_eta(eta), m_phi(phi), m_gasGap(gasGap), 
      m_isStrip(isStrip), m_channel(channel), m_bcTag(bcTag), m_width(width), m_shortWidth(shortWidth), m_longWidth(longWidth), m_length(length) {  
  }
} // namespace MuonCalib

std::ostream& operator << (std::ostream& stream, const MuonCalib::MuonCalibRawTgcHit& rawHit);

#endif // MuonCalibRawTgcHit_h
