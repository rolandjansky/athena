// Multiple inclusion protection
#ifndef SCT_DIGITIZATIONRTT_SCT_DIGITIZATIONRTT_H
#define SCT_DIGITIZATIONRTT_SCT_DIGITIZATIONRTT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h" 

#include "InDetSimEvent/SiHit.h"

#include "InDetPrepRawData/SCT_ClusterContainer.h"

class TH1F;
class TH2F;
class TProfile;

class ITHistSvc;
//class INTupleSvc;

namespace HepMC {
  class GenVertex;
}

class SCT_RDORawData;
class SCT_ID;
namespace InDetDD {
  class SCT_DetectorManager;
}
namespace InDet {
  class SiCluster;
  class SCT_Cluster;
  //class SCT_ClusterContainer;
}

class ISiliconConditionsSvc;
class ISiPropertiesSvc;

/** This class ..... */
class SCT_DigitizationRTT:public AthAlgorithm {

public:

  SCT_DigitizationRTT (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  ServiceHandle<ITHistSvc> m_tHistSvc; //!< Histogram service handle
  ServiceHandle<ISiliconConditionsSvc> m_siConditionsSvc;
  ServiceHandle<ISiPropertiesSvc> m_siPropertiesSvc;
  //ServiceHandle<INTupleSvc> m_nTupleSvc; //!< Ntuple service handle
  const InDetDD::SCT_DetectorManager *m_sctmgr; //!< Pointer to the SCT detector manager
  const SCT_ID *m_sctID;        //!<  Pointer to the SCT ID helper
  double fC;                    //!<  Number of electrons in one femtoCoulomb
  /** Switches
   */
  bool m_selectEvent;       //!< Toggle using event selection
  bool m_printGeomInfo;    //!< Toggle printing Geometry info
  bool m_printEvents;      //!< Toggle printing Events
  bool m_RawDataHists;      //!< Toggle making Raw Data histograms
  bool m_G4HitsHists;       //!< Toggle making G4 hit histograms
  bool m_SimDataHists;      //!< Toggle making Sim Data histograms
  bool m_CresHists;         //!< Toggle making cluster resolution histograms

  int m_min_SctBrlClusMult; //!< 
  int m_max_SctBrlClusMult; //!< 
  std::string m_rootRoot;   //!< Name of the ITHistSvc (default: /SCT_DigiRTT)


  /** Books the Raw Data histograms
      */
  StatusCode BookRawDataHists();

  /** Fills the Raw Data histograms for each event
      */
  StatusCode FillRawDataHists();

  /** Fills the Sim Data histograms for each event
      */
  StatusCode FillSimDataHists();

  /** Books the G4 hit histograms
      */
  StatusCode BookG4HitsHists();

  /** Fills the G4 hit histograms for each event
      */
  StatusCode FillG4HitsHists();

  /** Books the cluster resolution histograms
      */
  StatusCode BookCresHists();

  /** Fills the cluster resolution histograms for each event
      */
  StatusCode FillCresHists();

  /** Dummy algorithm to decide whether or not a event is worth
      looking at in detail
      */
  bool       selectEvent() const;

  /** 
      */
  StatusCode ProcessG4Hits();

  /** Simple test to decide if an SCT cluster is made purely from noise hits.
      */
  bool IsNoiseClusterFromDataOnly(const Identifier &clId, const InDet::SCT_ClusterContainer* SCTClusters);

  /** Fills histograms with details about the properties of SCT clusters flagged as being made purely from noise.
      */
  void FillNoiseClusterHistograms(const Identifier &clId, const std::vector<Identifier>& RDOsInCluster);

  /** Unpacks detailed information about a true G4 hit
      Note that the two input SiHit's can point to the same hit, in which case information about a single G4 step is unpacked.

      @param [in] hitStart  pointer to SiHit corresponding to track entry into wafer
      @param [in] hitEnd    pointer to SiHit corresponding to track exit  from wafer
      @return bool
      */
  bool unpackG4Hit (const SiHit& hitStart, const SiHit& hitEnd);

  /** Calculates the angles between a vector and the surface of a particular SCT wafer.

      @param [in]  vec
      @param [in]  id
      @param [out] theta  angle to wafer
      @param [out] phi    angle to wafer
      */
  void findAnglesToWaferSurface ( const double (&vec)[3], const Identifier &id, double& theta, double& phi );

  /** Finds the G4 hit corresponding to a given Identifier

      @param [in] id        Identifier
      @return pointer to G4 hit
      */
  const SiHit* findG4Hit (const Identifier &id);

  /** Finds the clusters in a given wafer, ordered by distance from a given x_local position (e.g. a track position).
      Note that calling findNearestClusters with xlocal=-50.0 (say) returns all the clusters in a given wafer ordered by strip number.

      @param [in] id        Identifier of wafer
      @param [in] xlocal    Local-x position within wafer
      @return vector of pointers to all clusters in the wafer, ordered by dxlocal distance from input xlocal position
      */
  std::vector<InDet::SCT_Cluster*> findNearestClusters ( const Identifier &id, const double (&xlocal)[3] );

  /** Finds the distance dx = xlocal(cluster) - xlocal

      @param [in] cluster   cluster
      @param [in] xlocal    Local position within wafer
      @return dx
      */
  double DistToCluster ( const InDet::SiCluster& cluster, const double (&xlocal)[3] );

  /** 
      */
  double ShiftEndcapLocalX (const InDetDD::SiDetectorElement& element, const double xLocalOld, const double yLocalOld, const double yLocalNew ) const;

  /** 
      */
  HepMC::GenVertex* findTrueVertex() const;

  /** Finds the RDO (if any) for a given strip

      @param [in] id        Identifier of strip
      @return pointer to RDO for the given strip
      */
  SCT_RDORawData* findRDO (const Identifier &id) const;

  /** Finds the decimal strip number for a given local position

      @param [in] element   pointer to detector element
      @param [in] locpos    position in simulation local coordinates
      @return     double    decimal strip number
      */
  HepGeom::Point3D<double> correctedLocalPosition ( const InDetDD::SiDetectorElement &element, const HepGeom::Point3D<double> &locpos );

  /** Finds the local position on the readout surface after Lorentz drifting

      @param [in] element   pointer to detector element
      @param [in] locpos    position in simulation local coordinates
      @return   HepGeom::Point3D<double>  position on readout surface in simulation local coordinates
      */
  double stripNumberFromPosition ( const InDetDD::SiDetectorElement &element, const HepGeom::Point3D<double> &locpos );

  /** Given a wafer identifier this function returns the identifier of the wafer on the
      other side of the same module.
      */
  Identifier waferIdOfOtherSide ( const Identifier &id ) const;

  /** Given an [x,y] this function returns the azimuthal
      angle phi in the range [0,pi]
      */
  double phiZeroTo2Pi ( double x, double y ) const;

  /** Returns x modulo dx
      */
  double moduloDouble ( double x, double dx ) const;

  /** Calculates the average occupancy of an SCT Barrel layer.
      Run during finalise.
      */
  double GetBarrelOccupancy(const TH2F* hist, const int layer, const double basescale, double& error);

  /** Calculates the average occupancy of an SCT Endcap disk.
      Run during finalise.
      */
  double GetEndcapOccupancy(const TH2F* hist, const int disk, const double basescale, double& error);

  /** General Histogram booking methods */
  TH1F* BookTH1F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh);
  TH2F* BookTH2F(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, int nbinsy, double ylow, double yhigh);
  TProfile* BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh);
  TProfile* BookTProfile(const char* histName, const char* histTitle, int nbinsx, double xlow, double xhigh, double ymin, double ymax);
  std::string m_rootName;
  //std::string m_treeName;

  
  int m_runNumber;                    //!< Run number
  int m_eventNumber;                  //!< Event number 
  int m_numberOfEventsInput;          //!< Total number of events seen by SCT_DigitizationRTT so far
  int m_numberOfEventsSelected;       //!< Total number of events selected so far

  Identifier m_G4_waferId;
  InDetDD::SiDetectorElement* m_G4_element;
  double     m_G4_xStep[3];
  double     m_G4_stepLength;
  double     m_G4_theta;
  double     m_G4_phi;
  HepGeom::Point3D<double> m_G4_localHitStartPoint;
  HepGeom::Point3D<double> m_G4_localHitEndPoint;
  HepGeom::Point3D<double> m_G4_localStartPoint;
  HepGeom::Point3D<double> m_G4_localEndPoint;
  HepGeom::Point3D<double> m_G4_globalHitStartPoint;
  HepGeom::Point3D<double> m_G4_globalHitEndPoint;
  HepGeom::Point3D<double> m_G4_globalStartPoint;
  HepGeom::Point3D<double> m_G4_globalEndPoint;
  double     m_G4_xlocalG4[3];
  bool       m_G4_withinFidVol;
  bool       m_G4_cleanHit;
  double     m_G4_stripStart;
  double     m_G4_stripEnd;
  HepGeom::Point3D<double> m_G4_corrLocalHitStartPoint;
  HepGeom::Point3D<double> m_G4_corrLocalHitEndPoint;
  double     m_G4_corrStripStart;
  double     m_G4_corrStripEnd;
  double     m_G4_corrActiveStripStart;
  double     m_G4_corrActiveStripEnd;
  double     m_G4_xlocFracAcrossStrip;
  std::map<Identifier,double> m_G4_mapWaferToEloss;
  std::map<Identifier,double> m_G4_mapStripToEloss;

  std::map<Identifier,double> m_G4_mapWaferToNTrueHit;
  std::map<Identifier,double> m_G4_mapWaferToEfficiency;

  std::map<Identifier,const SiHit*> m_G4_mapWaferToStartHit;
  std::map<Identifier,const SiHit*> m_G4_mapWaferToEndHit;

  TH1F* m_h_g4hits_h1000;
  TH1F* m_h_g4hits_h1001;
  TH1F* m_h_g4hits_h1002;
  TH1F* m_h_g4hits_h1003;
  TH1F* m_h_g4hits_h1004;
  TH1F* m_h_g4hits_h1005;
  TH1F* m_h_g4hits_h1006;
  TH1F* m_h_g4hits_h1007;
  TH1F* m_h_g4hits_h1011;
  TH1F* m_h_g4hits_h1012;
  TH1F* m_h_g4hits_h1013;
  TH1F* m_h_g4hits_h1021;
  TH1F* m_h_g4hits_h1022;
  TH1F* m_h_g4hits_h1023;
  TH2F* m_h_g4hits_h1031;
  TH2F* m_h_g4hits_h1032;
  TH2F* m_h_g4hits_h1033;
  TH1F* m_h_g4hits_h1601;
  TH1F* m_h_g4hits_h1602;
  TH1F* m_h_g4hits_h1603;
  TH1F* m_h_g4hits_h1610;
  TH1F* m_h_g4hits_h1611;
  TH1F* m_h_g4hits_h1612;
  TH1F* m_h_g4hits_h1613;
  TH1F* m_h_g4hits_h1620;
  TH1F* m_h_g4hits_h1621;
  TH1F* m_h_g4hits_h1622;
  TH1F* m_h_g4hits_h1623;
  TH2F* m_h_g4hits_h1700;
  TH2F* m_h_g4hits_h1701;
  TH2F* m_h_g4hits_h1710;
  TH2F* m_h_g4hits_h1711;
  TH1F* m_h_g4hits_h1721;
  TH1F* m_h_g4hits_h1722;
  TH1F* m_h_g4hits_h1723;
  TH1F* m_h_g4hits_h1724;
  TH1F* m_h_g4hits_h1725;
  TH1F* m_h_g4hits_h1731;
  TH1F* m_h_g4hits_h1732;
  TH1F* m_h_g4hits_h1733;
  TH1F* m_h_g4hits_h1734;
  TH1F* m_h_g4hits_h1102;
  TH1F* m_h_g4hits_h1103;
  TH2F* m_h_g4hits_h1211;
  TH2F* m_h_g4hits_h1212;
  TH1F* m_h_g4hits_h1310;
  TH1F* m_h_g4hits_h1311;
  TH1F* m_h_g4hits_h1312;
  TH1F* m_h_g4hits_h1313;
  TH1F* m_h_g4hits_h1314;
  TH1F* m_h_g4hits_h2200;
  TH1F* m_h_g4hits_h2201;
  TH1F* m_h_g4hits_h2202;
  TH1F* m_h_g4hits_h2300;
  TH1F* m_h_g4hits_h2301;
  TH2F* m_h_g4hits_h2302;
  TH1F* m_h_g4hits_h2320;
  TH1F* m_h_g4hits_h2321;
  TH1F* m_h_g4hits_h2322;
  TH1F* m_h_g4hits_h2390;
  TH1F* m_h_g4hits_h2391;
  TH1F* m_h_g4hits_h2400;
  TH1F* m_h_g4hits_h2401;
  TH1F* m_h_g4hits_h2402;
  TH1F* m_h_g4hits_h2410;
  TH1F* m_h_g4hits_h2411;
  TH1F* m_h_g4hits_h2412;
  TH1F* m_h_g4hits_h2450;
  TH1F* m_h_g4hits_h2451;
  TH1F* m_h_g4hits_h2452;
  TH1F* m_h_g4hits_h2460;
  TH1F* m_h_g4hits_h2461;
  TH1F* m_h_g4hits_h2462;

  TH1F* m_h_g4hits_brl_h1062;
  TH1F* m_h_g4hits_brl_h1063;
  TH2F* m_h_g4hits_brl_h1201;
  TH2F* m_h_g4hits_brl_h1202;
  TH2F* m_h_g4hits_brl_h1203;
  TH2F* m_h_g4hits_brl_h1204;
  std::vector<TH2F*> m_h_g4hits_brl_h1300;
  TH2F* m_h_g4hits_brl_h1310;
  TH2F* m_h_g4hits_brl_h1400[4][2];
  TH1F* m_h_g4hits_brl_h2000[4][2];
  std::vector<TH1F*> m_h_g4hits_brl_h2100;
  TH2F* m_h_g4hits_brl_h3201;
  TH2F* m_h_g4hits_brl_h3202;
  TH2F* m_h_g4hits_brl_h3203;
  TH2F* m_h_g4hits_brl_h3204;
  TH1F* m_h_g4hits_brl_h3211;
  TH1F* m_h_g4hits_brl_h3212;
  TH1F* m_h_g4hits_brl_h3213;
  TH1F* m_h_g4hits_brl_h3221;
  TH1F* m_h_g4hits_brl_h3222;
  TH1F* m_h_g4hits_brl_h3223;
  TH1F* m_h_g4hits_brl_h3224;
  TH1F* m_h_g4hits_brl_h3291;
  TH1F* m_h_g4hits_brl_h3292;
  TH1F* m_h_g4hits_brl_h3293;
  TH1F* m_h_g4hits_brl_h3311;
  TH1F* m_h_g4hits_brl_h3312;
  TH1F* m_h_g4hits_brl_h3313;
  TH1F* m_h_g4hits_brl_h3321;
  TH1F* m_h_g4hits_brl_h3322;
  TH1F* m_h_g4hits_brl_h3323;
  TH1F* m_h_g4hits_brl_h3324;
  TH1F* m_h_g4hits_brl_h3401;
  TH1F* m_h_g4hits_brl_h3402;
  TH1F* m_h_g4hits_brl_h3403;
  TH1F* m_h_g4hits_brl_h3404;
  TH1F* m_h_g4hits_brl_h3405;
  TH1F* m_h_g4hits_brl_h3406;
  TH1F* m_h_g4hits_brl_h3410;
  TH1F* m_h_g4hits_brl_h3411;
  TH1F* m_h_g4hits_brl_h3420;
  TH1F* m_h_g4hits_brl_h3421;
  TH1F* m_h_g4hits_brl_h3501;
  TH1F* m_h_g4hits_brl_h3502;
  TH1F* m_h_g4hits_brl_h3511;
  TH1F* m_h_g4hits_brl_h3512;
  TH2F* m_h_g4hits_brl_h3521;
  TH2F* m_h_g4hits_brl_h3522;

  TH1F* m_h_g4hits_end_h1002;
  TH1F* m_h_g4hits_end_h1004;
  std::vector<TH2F*> m_h_g4hits_end_h1010;
  std::vector<TH1F*> m_h_g4hits_end_h1020;
  TH1F* m_h_g4hits_end_h1062;
  TH1F* m_h_g4hits_end_h1063;
  TH2F* m_h_g4hits_end_h1201;
  TH2F* m_h_g4hits_end_h1202;
  std::vector<TH2F*> m_h_g4hits_end_h1300;
  std::vector<TH2F*> m_h_g4hits_end_h1310;
  std::vector<TH1F*> m_h_g4hits_end_h1320;
  std::vector<TH1F*> m_h_g4hits_end_h1330;
  std::vector<TH2F*> m_h_g4hits_end_h1340;
  TH2F* m_h_g4hits_end_h1350;
  TH2F* m_h_g4hits_end_h1351;
  TH2F* m_h_g4hits_end_h1352;
  TH2F* m_h_g4hits_end_h1353;

  TH1F* m_h_cres_clus_h1001; //!< Number of SCT clusters
  TH1F* m_h_cres_clus_h1002; //!< Number of SCT clusters with no primary G4 hit
  TH1F* m_h_cres_clus_h1003; //!< Number of SCT clusters with primary G4 hit in same wafer
  TH1F* m_h_cres_clus_h1004; //!< Number of SCT clusters with primary G4 hit in different wafer
  TH1F* m_h_cres_clus_h1201; //!< dx cluster-G4hit
  TH1F* m_h_cres_clus_h1202; //!< dx cluster-G4hit
  TH1F* m_h_cres_clus_h1203; //!< dx cluster-G4hit
  TH2F* m_h_cres_clus_h1301; //!< r (cluster,G4hit), different wafer
  TH2F* m_h_cres_clus_h1302; //!< phi (cluster,G4hit), different wafer
  TH2F* m_h_cres_clus_h1303; //!< z (cluster,G4hit), different wafer
  TH1F* m_h_cres_clus_h1311; //!< r.dphi, cluster-G4hit, different wafer
  TH1F* m_h_cres_clus_h1312; //!< dz cluster-G4hit, different wafer
  TH1F* m_h_cres_clus_h1313; //!< dz cluster-G4hit, different wafer, zclus<0
  TH1F* m_h_cres_clus_h1314; //!< dz cluster-G4hit, different wafer, zclus>0

  TH1F* m_h_cres_h1001; //!< Number of selected G4 hits
  TH1F* m_h_cres_h1002; //!< Number of G4 hits matched to SCT cluster
  TH1F* m_h_cres_h1003; //!< Number of clean G4 hits
  TH1F* m_h_cres_h1004; //!< Number of inefficient clean G4 hits

  TH1F* m_h_cres_brl_h1100;    //!< Lorentz drift angle
  TH1F* m_h_cres_brl_h1201;    //!< dx cluster-G4hit, nearest cluster
  TH1F* m_h_cres_brl_h1202;    //!< dx cluster-G4hit, nearest cluster
  TH1F* m_h_cres_brl_h1203;    //!< dx cluster-G4hit, nearest cluster
  TH1F* m_h_cres_brl_h1211;    //!< dx cluster-G4hit, 2nd nearest cluster
  TH1F* m_h_cres_brl_h1212;    //!< dx cluster-G4hit, 2nd nearest cluster
  TH1F* m_h_cres_brl_h1221;    //!< dx cluster-G4hit, 3rd nearest cluster
  TH1F* m_h_cres_brl_h1222;    //!< dx cluster-G4hit, 3rd nearest cluster
  TH1F* m_h_cres_brl_h1231;    //!< |dx| 2nd nearest - nearest
  TH1F* m_h_cres_brl_h1232;    //!< |dx| 3rd nearest - 2nd nearest
  TH1F* m_h_cres_brl_h1401;    //!< G4 hit local x			      
  TH1F* m_h_cres_brl_h1402;    //!< G4 hit local y			      
  TH1F* m_h_cres_brl_h1403;    //!< G4 hit local z			      
  TH1F* m_h_cres_brl_h1411;    //!< G4 hit energy loss
  TH1F* m_h_cres_brl_h1421;    //!< G4 hit phi to wafer surface
  TH1F* m_h_cres_brl_h1422;    //!< G4 hit theta to wafer surface
  TH1F* m_h_cres_brl_h1423;    //!< G4 hit phi to wafer surface, |theta|<30deg
  TH1F* m_h_cres_brl_h1424;    //!< G4 hit theta to wafer surface, |phi|<30deg
  TH1F* m_h_cres_brl_h1431;    //!< G4 hit layer/side
  TH1F* m_h_cres_brl_h1432;    //!< G4 hit eta module
  TH1F* m_h_cres_brl_h1433;    //!< G4 hit phi module
  TH1F* m_h_cres_brl_h1501;    //!< G4 hit local x, no nearby cluster
  TH1F* m_h_cres_brl_h1502;    //!< G4 hit local y, no nearby cluster
  TH2F* m_h_cres_brl_h1504;    //!< G4 hit local (y,x), no nearby cluster
  TH1F* m_h_cres_brl_h1511;    //!< G4 hit energy loss, no nearby cluster
  TH1F* m_h_cres_brl_h1521;    //!< G4 hit phi to wafer surface, no nearby cluster  
  TH1F* m_h_cres_brl_h1522;    //!< G4 hit theta to wafer surface, no nearby cluster
  TH1F* m_h_cres_brl_h1523;    //!< G4 hit phi to wafer surface, |theta|<30deg, no nearby cluster
  TH1F* m_h_cres_brl_h1524;    //!< G4 hit theta to wafer surface, |phi|<30deg, no nearby cluster
  TH1F* m_h_cres_brl_h1531;    //!< G4 hit layer/side, no nearby cluster
  TH1F* m_h_cres_brl_h1532;    //!< G4 hit eta module, no nearby cluster
  TH1F* m_h_cres_brl_h1533;    //!< G4 hit phi module, no nearby cluster
  std::vector<TH1F*> m_h_cres_brl_h1600; //!< [0] G4 hit local x, clean hit, [1] G4 hit local y, clean hit
  TH1F* m_h_cres_brl_h2000;    //!< No of strips, associated clusters
  std::vector<TH2F*> m_h_cres_brl_h2010; //!< (phi,nStrip), associated clusters [0] 36 bins in phi, [1] 180 bins in phi
  std::vector<TProfile*> m_h_cres_brl_p2010; //!< <nStrip> vs phi, associated clusters [0] 36 bins in phi, [1] 180 bins in phi
  std::vector<TH1F*> m_h_cres_brl_h2020;        //!< 
  std::vector<TProfile*>  m_h_cres_brl_p2110;    //!< <dx> vs phi [0] 36 bins in phi, [1] 180 bins in phi
  std::vector<TProfile*>  m_h_cres_brl_p2120;    //!< <dx> vs phi [0] side 0, [1] side 1
  std::vector<TH1F*>      m_h_cres_brl_h2250;    //!< 
  TH2F*     m_h_cres_brl_h2200[2][6];
  TProfile* m_h_cres_brl_p2200[2][6];
  TProfile* m_h_cres_brl_p2300[2][6];
  TH1F*     m_h_cres_brl_h2400[2][6];
  TH2F*     m_h_cres_brl_h2500[2][6];

  TH2F* m_h_cres_barrel_layer_eff_h2990;
  TH1F* m_h_cres_barrel_layer_eff_h2991;
  TH2F* m_h_cres_barrel_layer_NcleanHits_h2992;
  TH1F* m_h_cres_barrel_layer_NcleanHits_h2993;
  TH2F* m_h_cres_barrel_layer_NClusters_h2994;
  TH1F* m_h_cres_barrel_layer_NClusters_h2995;
  TH2F* m_h_cres_barrel_efficiency_h2996;
  TH2F* m_h_cres_ec1_efficiency_h2997;
  TH2F* m_h_cres_ec2_efficiency_h2998;

  TH1F* m_h_cres_barrel_noise_strips_h4000;
  TH1F* m_h_cres_endcap_noise_strips_h4001;
  TH1F* m_h_noisemap_barrel_h4002;
  TH1F* m_h_noisemap_ec1_h4003;
  TH1F* m_h_noisemap_ec2_h4004;

  TH1F* m_h_cres_end_h1100;
  TH1F* m_h_cres_end_h1200;
  TH1F* m_h_cres_end_h1201;
  TH1F* m_h_cres_end_h1202;
  TH1F* m_h_cres_end_h1203;
  TProfile* m_h_cres_end_h1300[2][2];
  TH1F* m_h_cres_end_h1401;
  TH1F* m_h_cres_end_h1402;
  TH1F* m_h_cres_end_h1403;
  std::vector<TH2F*> m_h_cres_end_h2220;
  TH1F* m_h_cres_end_h1411;
  TH1F* m_h_cres_end_h1421;
  TH1F* m_h_cres_end_h1422;
  TH1F* m_h_cres_end_h1423;
  TH1F* m_h_cres_end_h1424;
  TH1F* m_h_cres_end_h1430;
  TH1F* m_h_cres_end_h1431;
  TH1F* m_h_cres_end_h1432;
  TH1F* m_h_cres_end_h1433;
  std::vector<TH1F*> m_h_cres_end_h2500;
  std::vector<TH1F*> m_h_cres_end_h2510;
  std::vector<TH2F*> m_h_cres_end_h2520;
  TH1F* m_h_cres_end_h1511;
  TH1F* m_h_cres_end_h1521;
  TH1F* m_h_cres_end_h1522;
  TH1F* m_h_cres_end_h1523;
  TH1F* m_h_cres_end_h1524;
  TH1F* m_h_cres_end_h1530;
  TH1F* m_h_cres_end_h1531;
  TH1F* m_h_cres_end_h1532;
  TH1F* m_h_cres_end_h1533;
  std::vector<TH1F*> m_h_cres_end_h2400;
  std::vector<TH1F*> m_h_cres_end_h2000;
  std::vector<TProfile*> m_h_cres_end_p2010;
  TH1F* m_h_cres_end_h3000[2][4];
  TH1F* m_h_cres_end_h3100[6][4];
  TH1F* m_h_cres_end_h3200[6][4];
  TProfile* m_h_cres_end_p3200[6][4];
  TProfile* m_h_cres_end_p3300[6][4];
  TProfile* m_h_cres_end_p3400[6][4];

  /** Raw Data Histograms */
  TH1F* m_h_rdo_h0001; //!< Number of SCT RDO Collections
  TH1F* m_h_rdo_h0002; //!< Number of Empty SCT RDO Collections
  TH1F* m_h_rdo_h1001; //!< SCT barrel/endcap
  TH1F* m_h_rdo_h1002; //!< SCT barrel layer
  TH1F* m_h_rdo_h1003; //!< SCT endcap disk
  TH1F* m_h_rdo_h1011; //!< SCT eta module, barrel
  TH1F* m_h_rdo_h1012; //!< SCT eta module, endcap
  TH1F* m_h_rdo_h1013; //!< SCT phi module, barrel
  TH1F* m_h_rdo_h1014; //!< SCT phi module, endcap
  TH1F* m_h_rdo_h1015; //!< SCT side: endcap/barrel/endcap
  TH1F* m_h_rdo_h1016; //!< SCT layer: endcap
  TH1F* m_h_rdo_h1021; //!< SCT strip number, barrel
  TH1F* m_h_rdo_h1022; //!< SCT strip number, endcap
  TH1F* m_h_rdo_h1031; //!< SCT cluster size, barrel
  TH1F* m_h_rdo_h1032; //!< SCT cluster size, endcap
  TH1F* m_h_rdo_h1201; //!< Phi module: Layer 0/1/2/3
  TH1F* m_h_rdo_h1210; //!< Phi module: Layer 0/1/2/3, Side0
  TH1F* m_h_rdo_h1211; //!< Phi module: Layer 0/1/2/3, Side1
  TH1F* m_h_rdo_h1301; //!< Eta module: Layer 0/1/2/3
  TH1F* m_h_rdo_h1310; //!< Eta module: Layer 0/1/2/3, Side0
  TH1F* m_h_rdo_h1311; //!< Eta module: Layer 0/1/2/3, Side1

  TH1F* m_h_sct_barrel_occupancy;     //!< SCT Barrel Occupancy (%) per layer
  std::vector<TH2F*> m_h_sct_barrel_occ_layer;  //!< Histograms showing occupancy per module - one per barrel layer

  TH1F* m_h_sct_endcapA_occupancy;    //!< SCT Endcap A Occupancy (%) per disk
  std::vector<TH2F*> m_h_sct_endcapA_occ_layer; //!< Histograms showing occupancy per module - one per endcap disk

  TH1F* m_h_sct_endcapC_occupancy;    //!< SCT Endcap C Occupancy (%) per disk
  std::vector<TH2F*> m_h_sct_endcapC_occ_layer; //!< Histograms showing occupancy per module - one per endcap disk

  TH1F* m_sct_module_occupancy_by_type; //!< Noise Occupancy (%) of Each type of module
  std::map<int,int> NoBarrelModules;
  std::map<int,int> NoInnerModules;
  std::map<int,int> NoMiddleModules;
  std::map<int,int> NoShortMiddleModules;
  std::map<int,int> NoOuterModules;
  double m_NumberModulesVetoed[5];
  //No. Outer Modules        = 2*9*52 in total
  //No. Middle Modules       = 2*7*40 in total
  //No. Short Middle Modules = 2*1*40 in total
  //No. Inner Modules        = 2*5*40 in total

  TH1F* m_h_rdo_barrel_modulemap_h2001; //!< Barrel RDOs Eta module [-6,-1],[1,6]:Phi module: Layer 0/1/2/3
  TH1F* m_h_rdo_ec1_modulemap_h2002;    //!< EndCapA RDOs Eta module 0/1/2:Phi module: Layer 0/1/2/3
  TH1F* m_h_rdo_ec2_modulemap_h2003;    //!< EndCapC RDOs Eta module 0/1/2:Phi module: Layer 0/1/2/3

  //** Debug methods */
  StatusCode BookAndFillTestHists();
  StatusCode PrintGeomInfo();
  StatusCode RetrieveEventInfo();
  StatusCode PrintCablingData();
  StatusCode PrintTrueEvent();
  StatusCode PrintG4Hits();
  StatusCode PrintSDOs();
  StatusCode PrintRDOs();
  StatusCode PrintClusters();


};
#endif
