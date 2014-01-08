/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcsimrecStand_H
#define AmdcsimrecStand_H

#include "AmdcStand/AmdcStandMisc.h"

#include "AmdcCore/Amdcsimrec.h"

  /**
   @class AmdcsimrecStand

   This class is an interface to Amdcsimrec functionnalities
   It derives from Amdcsimrec

  @author samusog@cern.ch

  */

class AmdcsimrecStand: public Amdcsimrec{
public:
    AmdcsimrecStand();
    virtual ~AmdcsimrecStand();

    static int initFromAscii (std::string fileName);

public:
///////////////////////////////////

// Range Sizes
   virtual int Mtec();
   virtual int Msta();
   virtual int Mtyp();
   virtual int Mgeo();
   virtual int Mphi();
   virtual int Mzz();
   virtual int Mstrp();
   virtual int Mgang();

// CHAPOS
   virtual double PosPhi (int jtyp,int jphi,int jzz);
   virtual double PosZ   (int jtyp,int jphi,int jzz);
   virtual double PosR   (int jtyp,int jphi,int jzz);
   virtual double PosS   (int jtyp,int jphi,int jzz);
   virtual double PosAlfa(int jtyp,int jphi,int jzz);
   virtual double PosBeta(int jtyp,int jphi,int jzz);
   virtual double PosGama(int jtyp,int jphi,int jzz);
   virtual int    IGEO    (int jtyp,int jphi,int jzz);
   virtual int    ICUT    (int jtyp,int jphi,int jzz);
   virtual int    IBAREL  (int jtyp,int jphi,int jzz);

// CHAGEO
   virtual double Geodx (int Jtyp,int Jgeo,int Jobj);
   virtual double Geody (int Jtyp,int Jgeo,int Jobj);
   virtual double Geodz (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoWs (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoWl (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoLe (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoEx (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoD1 (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoD2 (int Jtyp,int Jgeo,int Jobj);
   virtual double GeoD3 (int Jtyp,int Jgeo,int Jobj);
   virtual int    NOBJ  (int Jtyp,int Jgeo);
   virtual int    ITEC  (int Jtyp,int Jgeo,int Jobj);
   virtual int    ISTA  (int Jtyp,int Jgeo,int Jobj);
   virtual int    ISPLIX(int Jtyp,int Jgeo,int Jobj);
   virtual int    ISPLIY(int Jtyp,int Jgeo,int Jobj);
   virtual int    ISHAPE(int Jtyp,int Jgeo,int Jobj);

// CHACUT
   virtual double Cutdx(int jcut,int jocu);
   virtual double Cutdy(int jcut,int jocu);
   virtual double CutWs(int jcut,int jocu);
   virtual double CutWl(int jcut,int jocu);
   virtual double CutLe(int jcut,int jocu);
   virtual double CutEx(int jcut,int jocu);
   virtual double CutAn(int jcut,int jocu);
   virtual int    INOCUT(int jtyp, int jgeo, int joc2);
   virtual int    NOBCUT(int joc2);
   virtual int    IOBCUT(int jcut,int jocu);

// CHASTA
   virtual double STAX0 (int Jtec,int Jsta);
   virtual double STAPP (int Jtec,int Jsta);
   virtual double STARR (int Jtec,int Jsta);
   virtual double STAEE (int Jtec,int Jsta);
   virtual double STAPG (int Jtec,int Jsta);
   virtual double STATT (int Jtec,int Jsta,int Jl);
   virtual double STAOO (int Jtec,int Jsta,int Jl);
   virtual int    NLAZ  (int Jtec,int Jsta);
   virtual int    NLAS  (int Jtec,int Jsta);
   virtual int    ISTAMA(int Jtec,int Jsta,int Jl);
   virtual int    IFORMA(int Jtec,int Jsta);
   virtual int    IVALIDSTA(int Jtec,int Jsta);

// CHACAR
   virtual int  GetJtyp(std::string TheStationName);
   virtual std::string StationName(int Jtyp);
   virtual int  GetJtec(std::string TheTechnologyName);
   virtual std::string TechnologyName(int Jtec);

// CHASTATGC
   virtual double XtgcAdd  (int Jsta,int I116);
   virtual double XtgcStrp (int Jsta,int Istrp,int I12);
   virtual int    NtgcAdd  (int Jsta,int I110);
   virtual int    NtgcGang (int Jsta,int Jgang,int I13);

// CHADJUST
   virtual int    NBadjust ();

// CHADEFOR
   virtual int    NBdeform ();

// CHDWNLAY
   virtual int    NBdwnlay ();

// COVERS 
   virtual std::string AmdcName() ;
   virtual int    AmdcVersion () ;
   virtual int    VersionAmdc () ;

// HARDSOFT
   virtual int          GetHARDENTRIES()                                                 ;
   virtual std::string  GetHARDNAME   (int iEntry)                                       ;
   virtual std::string  GetSOFTNAME   (int iEntry)                                       ;
   virtual int          GetSOFTOCTANT (int iEntry)                                       ;
   virtual int          GetSOFTIZ     (int iEntry)                                       ;
   virtual int          GetEntryRank  (std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) ;
   virtual int          GetEntryRank  (std::string HARDNAME)                             ;
   virtual void         TestHardSoftStuff()                                              ;

   virtual int          GetHARDSector   (std::string HARDNAME,int& ierr) ;
   virtual int          GetHARDOctant   (std::string HARDNAME,int& ierr) ;
   virtual int          GetHARDIZ       (std::string HARDNAME,int& ierr) ;
   virtual int          GetHARDSignedIz (std::string HARDNAME,int& ierr) ;
   
// XTOMO
   virtual int           GetXTOMOENTRIES() ;
   virtual std::string   GetXtomoChberName (int iEntry) ;
   virtual std::string   GetXtomoSite      (int iEntry) ;
   virtual int           GetXtomoSiteId    (int iEntry) ;
   virtual int           GetXtomoTime      (int iEntry) ;
   virtual int           GetXtomoPassed    (int iEntry) ;
   virtual std::string   GetXtomoSide      (int iEntry) ;
   virtual int           GetXtomoNberTube1 (int iEntry) ;
   virtual int           GetXtomoNberTube2 (int iEntry) ;
   virtual int           GetXtomoNberML    (int iEntry) ;
   virtual int           GetXtomoNberLayer (int iEntry) ;
   virtual double        GetXtomoML1NYtub (int iEntry) ;
   virtual double        GetXtomoML1NZtub (int iEntry) ;
   virtual double        GetXtomoML1NDely (int iEntry) ;
   virtual double        GetXtomoML1NDelz (int iEntry) ;
   virtual double        GetXtomoML1NDela (int iEntry) ;
   virtual double        GetXtomoML1NYpit (int iEntry) ;
   virtual double        GetXtomoML1NZpit (int iEntry) ;
   virtual double        GetXtomoML1PYtub (int iEntry) ;
   virtual double        GetXtomoML1PZtub (int iEntry) ;
   virtual double        GetXtomoML1PDely (int iEntry) ;
   virtual double        GetXtomoML1PDelz (int iEntry) ;
   virtual double        GetXtomoML1PDela (int iEntry) ;
   virtual double        GetXtomoML1PYpit (int iEntry) ;
   virtual double        GetXtomoML1PZpit (int iEntry) ;
   virtual double        GetXtomoML2NYtub (int iEntry) ;
   virtual double        GetXtomoML2NZtub (int iEntry) ;
   virtual double        GetXtomoML2NDely (int iEntry) ;
   virtual double        GetXtomoML2NDelz (int iEntry) ;
   virtual double        GetXtomoML2NDela (int iEntry) ;
   virtual double        GetXtomoML2NYpit (int iEntry) ;
   virtual double        GetXtomoML2NZpit (int iEntry) ;
   virtual double        GetXtomoML2PYtub (int iEntry) ;
   virtual double        GetXtomoML2PZtub (int iEntry) ;
   virtual double        GetXtomoML2PDely (int iEntry) ;
   virtual double        GetXtomoML2PDelz (int iEntry) ;
   virtual double        GetXtomoML2PDela (int iEntry) ;
   virtual double        GetXtomoML2PYpit (int iEntry) ;
   virtual double        GetXtomoML2PZpit (int iEntry) ;
   virtual int           GetXtomoEntryRank (std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) ;
   virtual int           GetXtomoEntryRank (std::string HARDNAME)                             ; 
   virtual void          TestXtomoStuff ()                                              ;
  
   /**Get Amdc string */
   virtual std::string GetAmdcString() ;

   /**Get Agdd string */
   virtual std::string GetAgddString() ;

// More Infos
   virtual int GetIzzMax(std::string TheStationName);
   virtual int GetJzzMax(std::string TheStationName);
   virtual int GetNobMax(std::string TheStationName);
   virtual int GetJobj(std::string TheTechnologyName,std::string TheStationName,
               int Jff,int Jzz,int IndexOjct);

//
//
//  Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes
// *****************************************************************************
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetMDTindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube );
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetCSCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip );
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetRPCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip );
//
// ---------->    ***
   /**Indexes converters:  AMDC scheme -> "official" TEChnology dependent schemes */
   virtual void        GetTGCindexFromAMDC( std::string& StationName,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel );

//
//  Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme
// *****************************************************************************
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromMDT( std::string& StationName,
               int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube );
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromCSC( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip,
               int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag );
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromRPC( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri );
//
// ---------->                 ***
   /**Indexes converters:  "official" TEChnology dependent schemes -> AMDC scheme */
   virtual void        GetAMDCindexFromTGC( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
               int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri );
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
               double* SZT1, double* SZT2, double& Radius );
//
// ---------->    *****            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetStripPositionFromCSCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi, int& Strip,
	       double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4, double* XYZCenter,
	       double* SZT1, double* SZT2, double* SZT3, double* SZT4, double* SZTCenter );
//
// ---------->    *****            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetStripPositionFromRPCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi, int& Strip,
               double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
               double* SZT1, double* SZT2, double* SZT3, double* SZT4 );
//
//  ---------->   *******            ***
   /**Get position of active elements in both global (XYZ) or local (SZT) coordinates*/
   virtual void        GetChannelPositionFromTGCindex( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
               double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
               double* SZT1, double* SZT2, double* SZT3, double* SZT4 );
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
               double* XYZ, int& Tube );
//
// ---------->    ***         ****
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetCSCindexFromStripPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
               int& MeasuresPhi,
               double* XYZ, int& Strip );
//
// ---------->    ***         *****
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetRPCindexFromStripPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
               int& GasGap, int& MeasuresPhi,
               double* XYZ, int& Strip );
//
// ---------->    ***         *******
   /**Get index of active elements from its absolute position (XYZ) */
   virtual void        GetTGCindexFromChannelPosition( std::string& StationName,
               int& StationEta, int& StationPhi, int& GasGap, int& IsStrip,
               double* XYZ, int& Channel );
//
//
//  Extract information on Station Geometries and Positions from AMDC (using AMDC indexes)
// ****************************************************************************************
//
// ----------> Overall Range of Index  Jtyp  referring to the various Station Types
   /**Overall Range of Index  Jtyp  referring to the various Station Types */
   virtual int         StationTypeBegin();
   virtual int         StationTypeEnd();
//
// ----------> Name of Station Type corresponding to Index Jtyp (e.g. BIS, BIL, BML, BOH, EMS, T3L, CSL, ...)
   /**Name of Station Type corresponding to Index Jtyp (e.g. BIS, BIL, BML, BOH, EMS, T3L, CSL, ...) */
   virtual std::string GetStationType( int& Jtyp );
//
// ----------> Overall Range of Index  Jgeo  referring to the various Station Geometries (for a given Type)
   /**Overall Range of Index  Jgeo  referring to the various Station Geometries (for a given Type) */
   virtual int         StationGeomBegin();
   virtual int         StationGeomEnd();
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
               double& CenterLocX, double& CenterLocY, double& CenterLocZ );
//
// ----------> Get total Number of Objects of Station : ( StationType, Jgeo )
   /**Get total Number of Objects of Station : ( StationType, Jgeo ) */
   virtual int         GetNumberOfStationObjects( std::string& StationType, int& Jgeo );
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
               double& GeomLe, double& GeomEx, double& GeomD1, double& GeomD2, double& GeomD3 );
//
// ----------> Overall Range of Index  Jff  referring to Phi positions of Stations (a priori 1 to 8)
   /**Overall Range of Index  Jff  referring to Phi positions of Stations (a priori 1 to 8) */
   virtual int         PositionPhiBegin();
   virtual int         PositionPhiEnd();
//
// ----------> Overall Range of Index  Jzz  referring to Z positions of Stations
// ----------> Warning : This index can be negative
   /**
     Overall Range of Index  Jzz  referring to Z positions of Stations
     Warning : This index can be negative
    */
   virtual int         PositionZBegin();
   virtual int         PositionZEnd();
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
               double& PosiAlfa, double& PosiBeta, double& PosiGamma );
//
// ----------> Overall Range of Index  Jcut  referring to the (posible) various 
// ----------> cutout schemes for a given Station : ( StationType, Jgeo )
   /**
    Overall Range of Index  Jcut  referring to the (posible) various 
    cutout schemes for a given Station : ( StationType, Jgeo )
    */
   virtual int         StationCutOutBegin();
   virtual int         StationCutOutEnd();
//
// ----------> Get total Number of Objects with CutOuts
// ----------> of Station : ( StationType, Jgeo ) for CutOut Index : ( Jcut )
   /**
    Get total Number of Objects with CutOuts
    of Station : ( StationType, Jgeo ) for CutOut Index : ( Jcut )
    */
   virtual int         GetNumberOfCutOutObjects( std::string& StationType, int& Jgeo, int& Jcut );
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
               double& CutWs, double& CutWl, double& CutLe, double& CutEx, double& CutAn );
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
               double& RotatS, double& RotatZ, double& RotatT );
   virtual void        GetStationDisplacementFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,int& Job,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT );
// ----------> Get the Station (possible) Displacements (entries A of AMDC)
   /** Get the Station (possible) Displacements (entries A of AMDC) */
   virtual void        GetStationDisplacementFromAMDCJadjust(
               int& Jadjust,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT );
//
// ----------> Get the Station (possible) Deformations (entries B of AMDC)
   /** Get the Station (possible) Deformations (entries B of AMDC) */
   virtual void        GetStationDeformationFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en );
   virtual void        GetStationDeformationFromAMDC(
               std::string& StationName, int& Jff, int& Jzz,int& Job,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en ) ;
// ----------> Get the Station (possible) Deformations (entries B of AMDC)
   /** Get the Station (possible) Deformations (entries B of AMDC) */
   virtual void        GetStationDeformationFromAMDCJdeform(
               int& Jdeform,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en );
//
// ----------> Get the internal alignment (entries I of AMDC)
   /** Get the internal alignment (entries I of AMDC) */
   virtual void        GetStationInternalAlignmentFromAMDCJdwnlay(
               int& Jdwnlay,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj, int& Jlay,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT );
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
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
   virtual void        PerformDirectDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,int& Job,
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
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
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
   virtual void        PerformBackDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,int& Job,
               double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout );
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

   virtual int    NberOfMDTMultiLayers  (std::string& StationName,int jphi,int jzz                    );
   virtual int    NberOfMDTLayers       (std::string& StationName,int jphi,int jzz,int jMuLay         );
   virtual int    NberOfMDTTubes        (std::string& StationName,int jphi,int jzz,int jMuLay,int jLay);

};

#endif

