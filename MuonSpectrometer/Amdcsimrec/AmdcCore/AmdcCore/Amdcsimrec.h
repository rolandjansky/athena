/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Amdcsimrec_H
#define Amdcsimrec_H

#include "AmdcCore/AmdcsimrecMisc.h"

  /**
   @class Amdcsimrec

   This class is the base class for the interface to Amdcsimrec functionnalities
   
  @author samusog@cern.ch
  
  */

class Amdcsimrec{
public:
    Amdcsimrec();
    virtual ~Amdcsimrec();

public:
///////////////////////////////////

// Range Sizes
   virtual int Mtec() = 0 ;
   virtual int Msta() = 0 ;
   virtual int Mtyp() = 0 ;
   virtual int Mgeo() = 0 ;
   virtual int Mphi() = 0 ;
   virtual int Mzz() = 0 ;
   virtual int Mstrp() = 0 ;
   virtual int Mgang() = 0 ;

// CHAPOS
   virtual double PosPhi (int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosZ   (int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosR   (int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosS   (int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosAlfa(int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosBeta(int jtyp,int jphi,int jzz) = 0 ;
   virtual double PosGama(int jtyp,int jphi,int jzz) = 0 ;
   virtual int    IGEO    (int jtyp,int jphi,int jzz) = 0 ;
   virtual int    ICUT    (int jtyp,int jphi,int jzz) = 0 ;
   virtual int    IBAREL  (int jtyp,int jphi,int jzz) = 0 ;

// CHAGEO
   virtual double Geodx (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double Geody (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double Geodz (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoWs (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoWl (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoLe (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoEx (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoD1 (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoD2 (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual double GeoD3 (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual int    NOBJ  (int Jtyp,int Jgeo) = 0 ;
   virtual int    ITEC  (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual int    ISTA  (int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual int    ISPLIX(int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual int    ISPLIY(int Jtyp,int Jgeo,int Jobj) = 0 ;
   virtual int    ISHAPE(int Jtyp,int Jgeo,int Jobj) = 0 ;

// CHACUT
   virtual double Cutdx(int jcut,int jocu) = 0 ;
   virtual double Cutdy(int jcut,int jocu) = 0 ;
   virtual double CutWs(int jcut,int jocu) = 0 ;
   virtual double CutWl(int jcut,int jocu) = 0 ;
   virtual double CutLe(int jcut,int jocu) = 0 ;
   virtual double CutEx(int jcut,int jocu) = 0 ;
   virtual double CutAn(int jcut,int jocu) = 0 ;
   virtual int    INOCUT(int jtyp, int jgeo, int joc2) = 0 ;
   virtual int    NOBCUT(int joc2) = 0 ;
   virtual int    IOBCUT(int jcut,int jocu) = 0 ;

// CHASTA
   virtual double STAX0 (int Jtec,int Jsta) = 0 ;
   virtual double STAPP (int Jtec,int Jsta) = 0 ;
   virtual double STARR (int Jtec,int Jsta) = 0 ;
   virtual double STAEE (int Jtec,int Jsta) = 0 ;
   virtual double STAPG (int Jtec,int Jsta) = 0 ;
   virtual double STATT (int Jtec,int Jsta,int Jl) = 0 ;
   virtual double STAOO (int Jtec,int Jsta,int Jl) = 0 ;
   virtual int    NLAZ  (int Jtec,int Jsta) = 0 ;
   virtual int    NLAS  (int Jtec,int Jsta) = 0 ;
   virtual int    ISTAMA(int Jtec,int Jsta,int Jl) = 0 ;
   virtual int    IFORMA(int Jtec,int Jsta) = 0 ;
   virtual int    IVALIDSTA(int Jtec,int Jsta) = 0;

// CHACAR
   virtual int  GetJtyp(std::string TheStationName) = 0 ;
   virtual std::string StationName(int Jtyp) = 0 ;
   virtual int  GetJtec(std::string TheTechnologyName) = 0 ;
   virtual std::string TechnologyName(int Jtec) = 0 ;

// CHASTATGC
   virtual double XtgcAdd  (int Jsta,int I116) = 0 ;
   virtual double XtgcStrp (int Jsta,int Istrp,int I12) = 0 ;
   virtual int    NtgcAdd  (int Jsta,int I110) = 0 ;
   virtual int    NtgcGang (int Jsta,int Jgang,int I13) = 0 ;

// CHADJUST
   virtual int    NBadjust () = 0 ;

// CHADEFOR
   virtual int    NBdeform () = 0 ;

// CHDWNLAY
   virtual int    NBdwnlay () = 0 ;

// COVERS 
   virtual std::string AmdcName() = 0 ;
   virtual int    AmdcVersion ()  = 0 ;
   virtual int    VersionAmdc ()  = 0 ;

// HARDSOFT
   virtual int          GetHARDENTRIES()                                                 = 0 ;
   virtual std::string  GetHARDNAME   (int iEntry)                                       = 0 ;
   virtual std::string  GetSOFTNAME   (int iEntry)                                       = 0 ;
   virtual int          GetSOFTOCTANT (int iEntry)                                       = 0 ;
   virtual int          GetSOFTIZ     (int iEntry)                                       = 0 ;
   virtual int          GetEntryRank  (std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) = 0 ;
   virtual int          GetEntryRank  (std::string HARDNAME)                             = 0 ;
   virtual void         TestHardSoftStuff()                                              = 0 ;

   virtual int          GetHARDSector   (std::string HARDNAME,int& ierr) = 0 ;
   virtual int          GetHARDOctant   (std::string HARDNAME,int& ierr) = 0 ;
   virtual int          GetHARDIZ       (std::string HARDNAME,int& ierr) = 0 ;
   virtual int          GetHARDSignedIz (std::string HARDNAME,int& ierr) = 0 ;
   
// XTOMO
   virtual int           GetXTOMOENTRIES() = 0 ;
   virtual std::string   GetXtomoChberName (int iEntry) = 0 ;
   virtual std::string   GetXtomoSite      (int iEntry) = 0 ;
   virtual int           GetXtomoSiteId    (int iEntry) = 0 ;
   virtual int           GetXtomoTime      (int iEntry) = 0 ;
   virtual int           GetXtomoPassed    (int iEntry) = 0 ;
   virtual std::string   GetXtomoSide      (int iEntry) = 0 ;
   virtual int           GetXtomoNberTube1 (int iEntry) = 0 ;
   virtual int           GetXtomoNberTube2 (int iEntry) = 0 ;
   virtual int           GetXtomoNberML    (int iEntry) = 0 ;
   virtual int           GetXtomoNberLayer (int iEntry) = 0 ;
   virtual double        GetXtomoML1NYtub (int iEntry) = 0 ;
   virtual double        GetXtomoML1NZtub (int iEntry) = 0 ;
   virtual double        GetXtomoML1NDely (int iEntry) = 0 ;
   virtual double        GetXtomoML1NDelz (int iEntry) = 0 ;
   virtual double        GetXtomoML1NDela (int iEntry) = 0 ;
   virtual double        GetXtomoML1NYpit (int iEntry) = 0 ;
   virtual double        GetXtomoML1NZpit (int iEntry) = 0 ;
   virtual double        GetXtomoML1PYtub (int iEntry) = 0 ;
   virtual double        GetXtomoML1PZtub (int iEntry) = 0 ;
   virtual double        GetXtomoML1PDely (int iEntry) = 0 ;
   virtual double        GetXtomoML1PDelz (int iEntry) = 0 ;
   virtual double        GetXtomoML1PDela (int iEntry) = 0 ;
   virtual double        GetXtomoML1PYpit (int iEntry) = 0 ;
   virtual double        GetXtomoML1PZpit (int iEntry) = 0 ;
   virtual double        GetXtomoML2NYtub (int iEntry) = 0 ;
   virtual double        GetXtomoML2NZtub (int iEntry) = 0 ;
   virtual double        GetXtomoML2NDely (int iEntry) = 0 ;
   virtual double        GetXtomoML2NDelz (int iEntry) = 0 ;
   virtual double        GetXtomoML2NDela (int iEntry) = 0 ;
   virtual double        GetXtomoML2NYpit (int iEntry) = 0 ;
   virtual double        GetXtomoML2NZpit (int iEntry) = 0 ;
   virtual double        GetXtomoML2PYtub (int iEntry) = 0 ;
   virtual double        GetXtomoML2PZtub (int iEntry) = 0 ;
   virtual double        GetXtomoML2PDely (int iEntry) = 0 ;
   virtual double        GetXtomoML2PDelz (int iEntry) = 0 ;
   virtual double        GetXtomoML2PDela (int iEntry) = 0 ;
   virtual double        GetXtomoML2PYpit (int iEntry) = 0 ;
   virtual double        GetXtomoML2PZpit (int iEntry) = 0 ;
   virtual int           GetXtomoEntryRank (std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) = 0 ; 
   virtual int           GetXtomoEntryRank (std::string HARDNAME)                            = 0 ;  
   virtual void          TestXtomoStuff () = 0;
 
   /**Get Amdc string */
   virtual std::string GetAmdcString() = 0 ;

   /**Get Agdd string */
   virtual std::string GetAgddString() = 0 ;

// More Infos
   virtual int GetIzzMax(std::string TheStationName) = 0 ;
   virtual int GetJzzMax(std::string TheStationName) = 0 ;
   virtual int GetNobMax(std::string TheStationName) = 0 ;
   virtual int GetJobj(std::string TheTechnologyName,std::string TheStationName,
               int Jff,int Jzz,int IndexOjct) = 0 ;

//
//
//  Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes
// *****************************************************************************
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetMDTindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube ) = 0 ;
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetCSCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip ) = 0 ;
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetRPCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip ) = 0 ;
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetTGCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel ) = 0 ;

//
//  Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme
// *****************************************************************************
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromMDT( std::string& StationName,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube ) = 0 ;
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromCSC( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag ) = 0 ;
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromRPC( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri ) = 0 ;
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromTGC( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri ) = 0 ;
//
//
//  Get position of active elements in both global (XYZ) or local (SZT) coordinates
// *********************************************************************************
//  for tubes, return values are coordinates of both ends (XYZ1, XYZ2 or SZT1, SZT2) + radius
//  for strips (or wire gang), return values are coordinates of four corners (XYZ1 to XYZ4 or SZT1 to SZT4)
//  x global coordinate coresponds to XYZi[0]         s local coordinate coresponds to SZTi[0]
//  y global coordinate coresponds to XYZi[1]         z local coordinate coresponds to SZTi[1]
//  z global coordinate coresponds to XYZi[2]         t local coordinate coresponds to SZTi[2]
//
// ---------->    ****            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates
      for tubes, return values are coordinates of both ends (XYZ1, XYZ2 or SZT1, SZT2) + radius
      for strips (or wire gang), return values are coordinates of four corners (XYZ1 to XYZ4 or SZT1 to SZT4)
      x global coordinate coresponds to XYZi[0]         s local coordinate coresponds to SZTi[0]
      y global coordinate coresponds to XYZi[1]         z local coordinate coresponds to SZTi[1]
      z global coordinate coresponds to XYZi[2]         t local coordinate coresponds to SZTi[2]
    */
   virtual void        GetTubePositionFromMDTindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube,
               double* XYZ1, double* XYZ2,
               double* SZT1, double* SZT2, double& Radius ) = 0 ;
//
// ---------->    *****            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetStripPositionFromCSCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip,
	       double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4, double* XYZCenter,
	       double* SZT1, double* SZT2, double* SZT3, double* SZT4, double* SZTCenter ) = 0 ;
//
// ---------->    *****            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetStripPositionFromRPCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip,
               double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
               double* SZT1, double* SZT2, double* SZT3, double* SZT4 ) = 0 ;
//
//  ---------->   *******            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetChannelPositionFromTGCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
               double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
               double* SZT1, double* SZT2, double* SZT3, double* SZT4 ) = 0 ;
//
//
//  Get index of active elements from its absolute position (XYZ)
// ***************************************************************
//  x global coordinate coresponds to XYZ[0]
//  y global coordinate coresponds to XYZ[1]
//  z global coordinate coresponds to XYZ[2]
//
// ---------->    ***         ****
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetMDTindexFromTubePosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer,
               double* XYZ, int& Tube ) = 0 ;
//
// ---------->    ***         ****
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetCSCindexFromStripPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi,
               double* XYZ, int& Strip ) = 0 ;
//
// ---------->    ***         *****
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetRPCindexFromStripPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi,
               double* XYZ, int& Strip ) = 0 ;
//
// ---------->    ***         *******
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetTGCindexFromChannelPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip,
               double* XYZ, int& Channel ) = 0 ;
//
//
//  Extract information on Station Geometries and Positions from AMDC (using AMDC indexes)
// ****************************************************************************************
//
// ----------> Overall Range of Index  Jtyp  referring to the various Station Types
   /**Overall Range of Index  Jtyp  referring to the various Station Types */
   virtual int         StationTypeBegin() = 0 ;
   virtual int         StationTypeEnd() = 0 ;
//
// ----------> Name of Station Type corresponding to Index Jtyp (e.g. BIS, BIL, BML, BOH, EMS, T3L, CSL, ...)
   /**Name of Station Type corresponding to Index Jtyp (e.g. BIS, BIL, BML, BOH, EMS, T3L, CSL, ...) */
   virtual std::string GetStationType( int& Jtyp ) = 0 ;
//
// ----------> Overall Range of Index  Jgeo  referring to the various Station Geometries (for a given Type)
   /**Overall Range of Index  Jgeo  referring to the various Station Geometries (for a given Type) */
   virtual int         StationGeomBegin() = 0 ;
   virtual int         StationGeomEnd() = 0 ;
//
// ----------> Get overall dimensions of Station : ( StationType, Jgeo )
// ----------> with the CutOut Index : ( Jcut )   (Jcut = 0 --> No CutOut's)
// ----------> and position of its center w.r.t. local frame of AMDC
   /**
    Get overall dimensions of Station : ( StationType, Jgeo )
    with the CutOut Index : ( Jcut )   (Jcut = 0 --> No CutOut's)
    and position of its center w.r.t. local frame of AMDC
    */
   virtual void        GetStationDimensions( std::string& StationType, int& Jgeo, int& Jcut,
               int& IsValid, double& DimLocX, double& DimLocY, double& DimLocZ,
               double& CenterLocX, double& CenterLocY, double& CenterLocZ ) = 0 ;
//
// ----------> Get total Number of Objects of Station : ( StationType, Jgeo )
   /**Get total Number of Objects of Station : ( StationType, Jgeo ) */
   virtual int         GetNumberOfStationObjects( std::string& StationType, int& Jgeo ) = 0 ;
//
// ----------> Get all Parameters of Object : ( Job ) of Station : ( StationType, Jgeo )
// ----------> Index  Job  goes from 1 to GetNumberOfStationObjects( StationType, Jgeo )
// ----------> (this info. correspond to the entries D of AMDC)
   /**
    Get all Parameters of Object : ( Job ) of Station : ( StationType, Jgeo )
    Index  Job  goes from 1 to GetNumberOfStationObjects( StationType, Jgeo )
    (this info. correspond to the entries D of AMDC)
    */
   virtual void        GetStationObjectParam( std::string& StationType, int& Jgeo, int& Job,
               int& GeomIsValid, std::string& GeomTechnoName, int& GeomTechnoIndex,
               int& GeomSplitX, int& GeomSplitY, int& GeomShapeIndex,
               double& GeomDx, double& GeomDy, double& GeomDz, double& GeomWs, double& GeomWl,
               double& GeomLe, double& GeomEx, double& GeomD1, double& GeomD2, double& GeomD3 ) = 0 ;
//
// ----------> Overall Range of Index  Jff  referring to Phi positions of Stations (a priori 1 to 8)
   /**Overall Range of Index  Jff  referring to Phi positions of Stations (a priori 1 to 8) */
   virtual int         PositionPhiBegin() = 0 ;
   virtual int         PositionPhiEnd() = 0 ;
//
// ----------> Overall Range of Index  Jzz  referring to Z positions of Stations
// ----------> Warning : This index can be negative
   /**
     Overall Range of Index  Jzz  referring to Z positions of Stations
     Warning : This index can be negative
    */
   virtual int         PositionZBegin() = 0 ;
   virtual int         PositionZEnd() = 0 ;
//
// ----------> Get all Parameters of Position of Station : ( StationType, Jff, Jzz )
// ----------> (this info. correspond to the entries P of AMDC)
   /**
    Get all Parameters of Position of Station : ( StationType, Jff, Jzz )
    (this info. correspond to the entries P of AMDC)
    */
   virtual void        GetStationPositionParam( std::string& StationType, int& Jff, int& Jzz,
               int& PosiIsValid, int& PosiJgeo, int& PosiJcut, int& PosiIsBarrel,
               double& PosiPhi, double& PosiZ, double& PosiR, double& PosiS,
               double& PosiAlfa, double& PosiBeta, double& PosiGamma ) = 0 ;
//
// ----------> Overall Range of Index  Jcut  referring to the (posible) various 
// ----------> cutout schemes for a given Station : ( StationType, Jgeo )
   /**
    Overall Range of Index  Jcut  referring to the (posible) various 
    cutout schemes for a given Station : ( StationType, Jgeo )
    */
   virtual int         StationCutOutBegin() = 0 ;
   virtual int         StationCutOutEnd() = 0 ;
//
// ----------> Get total Number of Objects with CutOuts
// ----------> of Station : ( StationType, Jgeo ) for CutOut Index : ( Jcut )
   /**
    Get total Number of Objects with CutOuts
    of Station : ( StationType, Jgeo ) for CutOut Index : ( Jcut )
    */
   virtual int         GetNumberOfCutOutObjects( std::string& StationType, int& Jgeo, int& Jcut ) = 0 ;
//
// ----------> Get all Parameters of the JcutObj'th Object with CutOut
// ----------> of Station : ( StationType, Jgeo ) with the CutOut Index : ( Jcut )
// ----------> Index  JcutObj  goes from 1 to GetNumberOfCutOutObjects(...)
// ----------> (this info. correspond to the entries H of AMDC)
   /**
    Get all Parameters of the JcutObj'th Object with CutOut
    of Station : ( StationType, Jgeo ) with the CutOut Index : ( Jcut )
    Index  JcutObj  goes from 1 to GetNumberOfCutOutObjects(...)
    (this info. correspond to the entries H of AMDC)
    */
   virtual void        GetStationCutOutObjParam( std::string& StationType, int& Jgeo, int& Jcut, int& JcutObj,
               int& CutJob,   double& CutDx, double& CutDy,
               double& CutWs, double& CutWl, double& CutLe, double& CutEx, double& CutAn ) = 0 ;
//
// ----------> Methods to access the parameters for the internal description of the various
// ----------> thechnologies (MDT, RPC, TGC and CSC) (entries W of AMDC) are still to be finalised...
// virtual void        GetMDTinternalParam( int& Jgeo, ... )
// virtual void        GetCSCinternalParam( int& Jgeo, ... )
// virtual void        GetRPCinternalParam( int& Jgeo, ... )
// virtual void        GetTGCinternalParam( int& Jgeo, ... )
//
// ----------> Get the Station (possible) Displacements (entries A of AMDC)
   /** Get the Station (possible) Displacements (entries A of AMDC) */
   virtual void        GetStationDisplacementFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT ) = 0 ;
   virtual void        GetStationDisplacementFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,int& Job,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT ) = 0 ;
// ----------> Get the Station (possible) Displacements (entries A of AMDC)
   /** Get the Station (possible) Displacements (entries A of AMDC) */
   virtual void        GetStationDisplacementFromAMDCJadjust(
               int& Jadjust,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT ) = 0 ;
//
// ----------> Get the Station (possible) Deformations (entries B of AMDC)
   /** Get the Station (possible) Deformations (entries B of AMDC) */
   virtual void        GetStationDeformationFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en ) = 0 ;
   virtual void        GetStationDeformationFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,int& Job,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en ) = 0 ;
// ----------> Get the Station (possible) Deformations (entries B of AMDC)
   /** Get the Station (possible) Deformations (entries B of AMDC) */
   virtual void        GetStationDeformationFromAMDCJdeform(
               int& Jdeform,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en ) = 0 ;
//
// ----------> Get the internal alignment (entries I of AMDC)
   /** Get the internal alignment (entries I of AMDC) */
   virtual void        GetStationInternalAlignmentFromAMDCJdwnlay(
               int& Jdwnlay,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj, int& Jlay,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT )  = 0 ;
//
//
//  Deals with Displacements (entries A of AMDC) and Deformations (entries B of AMDC) of Stations
// ***********************************************************************************************
//
// ----------> Transform an absolute point (Xin,Yin,Zin --> Xout,Yout,Zout) corresponding
// ----------> to the Displacement (entry A of AMDC) and Deformation (entry B of AMDC)
// ----------> of the Station : ( StationType, Jff, Jzz )
   /**
    Transform an absolute point (Xin,Yin,Zin --> Xout,Yout,Zout) corresponding
    to the Displacement (entry A of AMDC) and Deformation (entry B of AMDC)
    of the Station : ( StationType, Jff, Jzz )
    */
   virtual void        PerformDirectDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ) = 0 ;
   virtual void        PerformDirectDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,int& Job,
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ) = 0 ;
//
// ----------> Transform BACK an absolute point (Xin,Yin,Zin --> Xout,Yout,Zout) corresponding
// ----------> to the Displacement (entry A of AMDC) and Deformation (entry B of AMDC)
// ----------> of the Station : ( StationType, Jff, Jzz )
   /**
    Transform BACK an absolute point (Xin,Yin,Zin --> Xout,Yout,Zout) corresponding
    to the Displacement (entry A of AMDC) and Deformation (entry B of AMDC)
    of the Station : ( StationType, Jff, Jzz )
    */
   virtual void        PerformBackDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz, 
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ) = 0 ;
   virtual void        PerformBackDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,int& Job,
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ) = 0 ;
//
//
//
//
//
//
//  (Simple) example of use of the above methods for MC generation of volumes :
// *****************************************************************************
////...................................................................................
//   std::string GeomTechnoName;
//   int IsValid, NbObj;
//   int GeomIsValid,GeomTechnoIndex;
//   int GeomSplitX,GeomSplitY, GeomShapeIndex;
//   int PosiIsValid,PosiJgeo,PosiJcut,PosiIsBarrel;
//   double DimLocX,DimLocY,DimLocZ,CenterLocX,CenterLocY,CenterLocZ;
//   double GeomDx,GeomDy,GeomDz,GeomWs,GeomWl;
//   double GeomLe,GeomEx,GeomD1,GeomD2,GeomD3;
//   double PosiPhi,PosiZ,PosiR,PosiS;
//   double PosiAlfa, PosiBeta,PosiGamma ;
//   for(int Jtyp =  pAmdcsimrec->StationTypeBegin(); Jtyp <= pAmdcsimrec->StationTypeEnd(); Jtyp++){
//     std::string StationType = pAmdcsimrec->GetStationType( Jtyp );
//     for(int Jgeo =  pAmdcsimrec->StationGeomBegin(); Jgeo <= pAmdcsimrec->StationTypeEnd(); Jgeo++){
//       int Jcut = 0;
//        pAmdcsimrec->GetStationDimensions(StationType,Jgeo,Jcut,IsValid,DimLocX,DimLocY,DimLocZ,
//         CenterLocX,CenterLocY,CenterLocZ );
////------> HERE, ONE CAN CREATE A GLOBAL LOGICAL VOLUME CORESPONDING TO THE STATION StationType,Jgeo
//        if( IsValid > 0 ){ NbObj = pAmdcsimrec->GetNumberOfStationObjects( StationType, Jgeo );
// 	for(int Job = 1; Job <= NbObj; Job++){
// 	  pAmdcsimrec->GetStationObjectParam( StationType, Jgeo, Job,GeomIsValid,GeomTechnoName, 
//                                            GeomTechnoIndex, GeomSplitX, 
// 					      GeomSplitY,GeomShapeIndex, GeomDx,  GeomDy,  GeomDz,  
//                                            GeomWs,  GeomWl,
// 					      GeomLe,  GeomEx,  GeomD1,  GeomD2,  GeomD3 );
////---------> HERE, ONE CAN CREATE THE LOGICAL VOLUME CORESPONDING TO THE OBJECT Job OF STATION StationType,Jgeo
////---------> AND POSITION IT IN THE GLOBAL LOGICAL VOLUME CORESPONDING TO THE STATION StationType,Jgeo
// 	}
// 	for(int Jff =  pAmdcsimrec->PositionPhiBegin(); Jff <= pAmdcsimrec->PositionPhiEnd(); Jff++){
// 	  for(int Jzz =  pAmdcsimrec->PositionZBegin();   Jzz <= pAmdcsimrec->PositionZEnd();   Jzz++){
//             pAmdcsimrec->GetStationPositionParam( StationType, Jff, Jzz,
// 				      PosiIsValid,  PosiJgeo,  PosiJcut,  PosiIsBarrel, PosiPhi,  PosiZ,  
//                                    PosiR,  PosiS,
// 				      PosiAlfa,  PosiBeta,  PosiGamma );
//             if( PosiIsValid > 0 && PosiJgeo == Jgeo ){
////------------> HERE, ONE CAN POSITION A COPY OF THE ABOVE GLOBAL LOGICAL VOLUME
////------------> OF THE STATION StationType,Jgeo  TO GET A NEW PHYSICAL VOLUME
//             }
// 	  }
// 	}
//        }
//     }
//   }   
////...................................................................................
//

   virtual int    NberOfMDTMultiLayers  (std::string& StationName,int jphi,int jzz                    ) = 0 ;
   virtual int    NberOfMDTLayers       (std::string& StationName,int jphi,int jzz,int jMuLay         ) = 0 ;
   virtual int    NberOfMDTTubes        (std::string& StationName,int jphi,int jzz,int jMuLay,int jLay) = 0 ;

};

#endif

