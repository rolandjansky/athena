/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Common Muon Readout Element properties
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonReadoutElement.h,v 1.3 2009-03-03 00:27:38 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_MUONREADOUTELEMENT_H
# define MUONGEOMODEL_MUONREADOUTELEMENT_H

#include <string>
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

//<<<<<< PUBLIC VARIABLES >>>>>>
#define maxfieldindex 10

class MsgStream;
class GeoPhysVol;

namespace MuonGM {

class MuonDetectorManager;
class MuonStation;

/**
   Base class for the XxxReadoutElement, 
   with Xxx = Mdt, Rpc, Tgc, Csc. It's a Trk::TrkDetElementBase,
   therefore it must implement the generic tracking interfaces requested to 
   the geometry: center, normal, surfaceBound, transform.
   It is synched to the raw geometry via the pointer, required by the 
   constructor, to its corresponding GeoVFullPhysVol.
   A MuonReadoutElement holds a pointer to its parent MuonStation.
*/

class MuonReadoutElement: public Trk::TrkDetElementBase 
{

public:
    
   // Id set/get methods
   MuonReadoutElement(GeoVFullPhysVol* pv, int zi, int fi, bool is_mirrored, 
                      MuonDetectorManager* mgr);
   virtual ~MuonReadoutElement();
    
   inline const Identifier identifyATLAS() const; 
   inline Identifier identify() const;
   inline IdentifierHash identifyHash() const; 
   //!< returns the collectionHash (EDM granularity)
   inline IdentifierHash collectionHash() const;
   //!< returns the collectionHash (EDM granularity)
   inline IdentifierHash detectorElementHash() const;
    //!<returns the detectorElementHash (EDM granularity)

   inline void  setIdentifier(Identifier id);
   inline void  setIdentifierHash(IdentifierHash id);
   inline void  setCollectionHash(IdentifierHash id);
   inline void  setDetectorElementHash(IdentifierHash id);
   virtual bool containsId(Identifier id) const = 0;

   inline int getStationEta() const;
   inline int getStationPhi() const;
   inline void setStationEta(int );
   inline void setStationPhi(int );

    const Amg::Vector3D globalPosition() const;
    const Amg::Transform3D & absTransform() const;
    const Amg::Transform3D & defTransform() const;
    const HepGeom::Transform3D & absTransformCLHEP() const;
    const HepGeom::Transform3D & defTransformCLHEP() const;

    //Amdb local (szt) to global coord
    const Amg::Vector3D AmdbLRSToGlobalCoords(Amg::Vector3D x) const;
    const Amg::Transform3D AmdbLRSToGlobalTransform() const; 
    //Global to Amdb local (szt) coord
    const Amg::Vector3D GlobalToAmdbLRSCoords(Amg::Vector3D x) const;
    const Amg::Transform3D GlobalToAmdbLRSTransform() const;

    // like MDT, RPC, TGC, CSC
    inline const std::string getTechnologyType() const;
    // like MDT1, RPC4, TGC1, etc... 
    inline const std::string getTechnologyName() const;
    inline void setTechnologyName(std::string str);
    // BOL, EIS, BMF, T1F, etc ... 
    inline const std::string getStationType() const;
    // BOL1, BEE1, etc... 
    inline const std::string getStationName() const;
    inline void setStationName(std::string );

    // max num. of fields initialised
    inline void setLastInitField(int idf);
    inline int getLastInitField() const;

    // size of the detector
    inline double getSsize() const;
    inline double getRsize() const;
    inline double getZsize() const;
    inline double getLongSsize() const;
    inline double getLongRsize() const;
    inline double getLongZsize() const;
    inline void setLongSsize(double );
    inline void setLongRsize(double );
    inline void setLongZsize(double );
    inline void setSsize(double );
    inline void setRsize(double );
    inline void setZsize(double );
    inline double getStationS() const;
    inline void setStationS(double );

    inline bool isMirrored() const;
    bool barrel() const;
    bool endcap() const;
    bool largeSector() const;
    bool smallSector() const;
    inline int getAmdbZi() const;
    inline int getAmdbFi() const;
    inline bool isDescrAtNegZ() const;
    inline bool sideA() const;
    inline bool sideC() const;

    void setParentStationPV(PVConstLink);
    void setParentStationPV();
    PVConstLink  parentStationPV()     const;
    const MuonStation* parentMuonStation() const;
    void setParentMuonStation(const MuonStation*);
    Amg::Transform3D toParentStation() const;
    Amg::Vector3D parentMuonStationPos()const;
    double parentStationRsize()     const;
    double parentStationSsize()     const;
    double parentStationZsize()     const;
    double parentStationLongRsize() const;
    double parentStationLongSsize() const;
    double parentStationLongZsize() const;
    double parentStation_s_amdb()   const;

   void setIndexOfREinMuonStation();
   int getIndexOfREinMuonStation() const;
   unsigned int nMDTinStation() const {return m_nMDTinStation;}
   unsigned int nCSCinStation() const {return m_nCSCinStation;}
   unsigned int nTGCinStation() const {return m_nTGCinStation;}
   unsigned int nRPCinStation() const {return m_nRPCinStation;}

   void setNofREinStation(int nMdt, int nRpc, int nTgc, int nCsc) 
    {m_nMDTinStation = nMdt; 
     m_nCSCinStation=nCsc; 
     m_nTGCinStation=nTgc; 
     m_nRPCinStation=nRpc;}

   bool hasCutouts() const {return m_hasCutouts;}
   void setHasCutouts(bool flag) {m_hasCutouts = flag;}
    
   // Tracking related interfaces
   // Element Surface
   // This creates a new surface. The client is responsible for deleting it.
   inline int cachingFlag() const;
   inline void setCachingFlag(int value);
   virtual void clearCache() const = 0;
   virtual void fillCache() const = 0;
   virtual void refreshCache() const = 0;

  // clean-up internal cache
   void clear() const;

   const Amg::Transform3D& defTransform(const Identifier&) const
   {return defTransform();};

   inline MsgStream& reLog() const;
 
protected:

   double m_Ssize, m_Rsize, m_Zsize, m_LongSsize, m_LongRsize, m_LongZsize;
   //!< size in the specified direction
   bool m_descratzneg;
   //!< true for stations that have explicit P-lines in amdb even if zi<0
   inline const MuonDetectorManager* manager() const; 
   std::string m_techname;
   //!< MDT or RPC or TGC or CSC plus a two digits subtype; example RPC17
   std::string m_statname; //!< examples are BMS5, CSS1, EML1
   Identifier m_id;       //!< extended data-collection identifier 
   IdentifierHash m_idhash;   //!< data-collection hash identifier
   IdentifierHash m_detectorElIdhash;   //!< detector element hash identifier 
   mutable int m_caching;
   //!< 0 if we want to avoid caching geometry info for tracking interface
   int m_indexOfREinMuonStation; //!<  index of this RE in the mother MuonStation
   bool m_hasCutouts; //!<  true is there are cutouts in the readdout-element
   unsigned int m_nMDTinStation;
   unsigned int m_nCSCinStation;
   unsigned int m_nRPCinStation;
   unsigned int m_nTGCinStation;
    
   MsgStream* m_MsgStream;
    
private:

   double m_stationS;
   int m_zi, m_fi;
   int m_eta, m_phi;
   bool m_mirrored;
   int m_id_max_init_field;
   //int m_id_fields[maxfieldindex];
   PVConstLink m_parentStationPV;
   const MuonStation* m_parentMuonStation; 
   MuonDetectorManager* m_muon_mgr;

  mutable const Amg::Transform3D* m_absTransform;
  mutable const Amg::Transform3D* m_defTransform;
};

MsgStream& MuonReadoutElement::reLog() const {return *m_MsgStream;} 

const Identifier MuonReadoutElement::identifyATLAS() const    {return m_id;}
Identifier MuonReadoutElement::identify() const    {return m_id;}
IdentifierHash MuonReadoutElement::identifyHash() const {return m_idhash;}
IdentifierHash MuonReadoutElement::collectionHash() const {return m_idhash;}
IdentifierHash MuonReadoutElement::detectorElementHash() const {return m_detectorElIdhash;}    
void  MuonReadoutElement::setIdentifier(Identifier id)        {m_id = id;}
void  MuonReadoutElement::setIdentifierHash(IdentifierHash id){m_idhash = id;}
void  MuonReadoutElement::setCollectionHash(IdentifierHash id){m_idhash = id;}
void  MuonReadoutElement::setDetectorElementHash(IdentifierHash id) {m_detectorElIdhash = id;}
const std::string MuonReadoutElement::getTechnologyType() const {return std::string(m_techname,0,3);}
const std::string MuonReadoutElement::getTechnologyName() const {return m_techname;}
void MuonReadoutElement::setTechnologyName(std::string str){
    m_techname = str;}
const std::string MuonReadoutElement::getStationType() const    {return std::string(m_statname,0,3);}
const std::string MuonReadoutElement::getStationName() const    {
    return m_statname;}
void MuonReadoutElement::setStationName(std::string str)  {
    m_statname = str;}
int MuonReadoutElement::getStationEta() const {return m_eta;}
int MuonReadoutElement::getStationPhi() const {return m_phi;}
void MuonReadoutElement::setStationEta(int eta)     {m_eta = eta;} 
void MuonReadoutElement::setStationPhi(int phi)     {m_phi = phi;}
void MuonReadoutElement::setLastInitField(int idf)    {m_id_max_init_field = idf;}
int MuonReadoutElement::getLastInitField() const {return m_id_max_init_field;}
double MuonReadoutElement::getSsize() const      {return m_Ssize;}
double MuonReadoutElement::getRsize() const      {return m_Rsize;}
double MuonReadoutElement::getZsize() const      {return m_Zsize;}
double MuonReadoutElement::getLongSsize() const  {return m_LongSsize;}
double MuonReadoutElement::getLongRsize() const  {return m_LongRsize;}
double MuonReadoutElement::getLongZsize() const  {return m_LongZsize;}
double MuonReadoutElement::getStationS() const  {return m_stationS;}
void MuonReadoutElement::setStationS(double v)        {m_stationS = v;}
void MuonReadoutElement::setLongSsize(double v)        {m_LongSsize = v;}
void MuonReadoutElement::setLongRsize(double v)        {m_LongRsize = v;}
void MuonReadoutElement::setLongZsize(double v)        {m_LongZsize = v;}
void MuonReadoutElement::setSsize(double v)            {m_Ssize = v;}
void MuonReadoutElement::setRsize(double v)            {m_Rsize = v;}
void MuonReadoutElement::setZsize(double v)            {m_Zsize = v;}

const MuonDetectorManager* MuonReadoutElement::manager() const
  {return m_muon_mgr;}
bool MuonReadoutElement::isMirrored() const
  {return m_mirrored;}
bool MuonReadoutElement::isDescrAtNegZ() const
  {return m_descratzneg;}

int MuonReadoutElement::getAmdbZi() const 
  {return m_zi;}
int MuonReadoutElement::getAmdbFi() const
  {return m_fi;}
bool MuonReadoutElement::sideA() const
  {return (m_zi>=0);}
bool MuonReadoutElement::sideC() const
  {return (m_zi<0);}
int MuonReadoutElement::cachingFlag() const
  {return m_caching;}
void MuonReadoutElement::setCachingFlag(int value)
  {m_caching = value;}



inline const Amg::Transform3D & MuonReadoutElement::absTransform() const
{
  if( !m_absTransform ) m_absTransform = new Amg::Transform3D(Amg::CLHEPTransformToEigen(absTransformCLHEP()));
  return *m_absTransform;
}

inline const Amg::Transform3D & MuonReadoutElement::defTransform() const
{
  if( !m_defTransform ) m_defTransform = new Amg::Transform3D(Amg::CLHEPTransformToEigen(defTransformCLHEP()));
  return *m_defTransform;
}
inline const HepGeom::Transform3D & MuonReadoutElement::absTransformCLHEP() const
{
  return getMaterialGeom()->getAbsoluteTransform();
}

inline const HepGeom::Transform3D & MuonReadoutElement::defTransformCLHEP() const
{
  return getMaterialGeom()->getDefAbsoluteTransform();
}

} // namespace MuonGM


#endif // MUONGEOMODEL_MUONREADOUTELEMENT_H
