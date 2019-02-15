// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCluster_v1.h 753412 2016-06-07 16:25:07Z ssnyder $
#ifndef XAODCALOEVENT_VERSIONS_CALOCLUSTER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOCLUSTER_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <bitset>

// xAOD include(s):
#include "xAODBase/IParticle.h"

//Defintion of CaloSamples (enum)
#include "CaloGeoHelpers/CaloSampling.h"

#include "xAODCaloEvent/CaloClusterBadChannelData.h"

#ifndef XAOD_ANALYSIS
#ifndef SIMULATIONBASE
#include "AthLinks/ElementLink.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"
#include "CaloEvent/CaloRecoStatus.h"
#endif // not SIMULATIONBASE
#endif // not XAOD_ANALYSIS

// Declare a dummy CaloClusterCellLink definition for standalone compilation:
#if defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)
class CaloClusterCellLink {};
typedef unsigned CaloRecoStatus;
#endif // defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)

class CaloClusterChangeSignalState;

namespace xAOD {

   /// Description of a calorimeter cluster
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Walter Lampl <Walter.Lampl@cern.ch>
   ///
   /// $Revision: 753412 $
   /// $Date: 2016-06-07 18:25:07 +0200 (Tue, 07 Jun 2016) $
   ///
   class CaloCluster_v1 : public IParticle {
     friend class ::CaloClusterChangeSignalState;

   public:

     typedef float flt_t;

     typedef CaloSampling::CaloSample CaloSample;

      /// @name Public types
      /// @{

      /// Enumeration to identify different cluster sizes
      enum ClusterSize {
         // electrons
         SW_55ele   = 1,
         SW_35ele   = 2,
         SW_37ele   = 3,
         // photons
         SW_55gam   = 4,
         SW_35gam   = 5,
         SW_37gam   = 6,
         // early converted photons
         SW_55Econv = 7,
         SW_35Econv = 8,
         SW_37Econv = 9,
         // soft electrons
         SW_softe   = 10,
         // topological clusters
         Topo_420   = 11,
         Topo_633   = 12,
         // transient cluster for AODCellContainer
         SW_7_11    = 13,
         //New (2016) egamma cluster
         SuperCluster=14,
         //New (2019) cluster representation of towers
         Tower_01_01 = 15,
         Tower_005_005 = 16,
	 Tower_fixed_area = 17,
         CSize_Unknown = 99
      };

      /// Enums to identify different moments
      ///
      ///  Each enum corresponds to a different characteristic moment of a
      ///  @c CaloCluster. The enums are loosly grouped by their type - i.e.
      ///  first moment over an angle, second moment, angular deviations etc.
      enum MomentType {
         FIRST_PHI         = 101, ///< First Moment in \f$\phi\f$
         FIRST_ETA         = 102, ///< First Moment in \f$\eta\f$
         SECOND_R          = 201, ///< Second Moment in \f$r\f$
         SECOND_LAMBDA     = 202, ///< Second Moment in \f$\lambda\f$
         /// Angular shower axis deviation (\f$\phi\f$) from IP-to-Center
         DELTA_PHI         = 301,
         /// Angular shower axis deviation (\f$\theta\f$) from IP-to-Center
         DELTA_THETA       = 302,
         /// Angular shower axis deviation from IP-to-Center
         DELTA_ALPHA       = 303,
         CENTER_X          = 401, ///< Cluster Centroid (\f$x\f$)
         CENTER_Y          = 402, ///< Cluster Centroid (\f$y\f$)
         CENTER_Z          = 403, ///< Cluster Centroid (\f$z\f$)
         /// Cluster Centroid (\f$\sqrt(x^2+y^2+z^2)\f$)
         CENTER_MAG        = 404,
         CENTER_LAMBDA     = 501, ///< Shower depth at Cluster Centroid
         LATERAL           = 601, ///< Normalized lateral moment
         LONGITUDINAL      = 602, ///< Normalized longitudinal moment
         ENG_FRAC_EM       = 701, ///< Energy fraction in EM calorimeters
         ENG_FRAC_MAX      = 702, ///< Energy fraction of hottest cell
         /// Energy fraction of the sum of the hottest cells in each sampling
         ENG_FRAC_CORE     = 703,
         FIRST_ENG_DENS    = 804, ///< First Moment in E/V
         SECOND_ENG_DENS   = 805, ///< Second Moment in E/V
         /// Energy weighted fraction of non-clustered perimeter cells
         ISOLATION         = 806,
         /// Total em-scale energy of bad cells in this cluster
         ENG_BAD_CELLS     = 807,
         N_BAD_CELLS       = 808, ///< number of bad cells
         /// Number of bad cells with energy density average correction applied
         N_BAD_CELLS_CORR  = 809,
         /// Energy of bad cells with energy density average correction applied
         BAD_CELLS_CORR_E  = 813,
         /// Energy fraction of LAr cells with quality larger than a given cut
         BADLARQ_FRAC      = 821,
         ENG_POS           = 822, ///< Total positive Energy of this cluster
         SIGNIFICANCE      = 823, ///< Cluster significance
         /// Cell significance = E/sig of the cell with the largest |E|/sig
         CELL_SIGNIFICANCE = 824,
         /// CaloSample of the cell with the largest |E|/sig
         CELL_SIG_SAMPLING = 825,
         /// Sum(E_cell_LAr^2 Q_cell_LAr)/Sum(E_cell_LAr^2)
         AVG_LAR_Q         = 826,
         /// Sum(E_cell_Tile^2 Q_cell_Tile)/Sum(E_cell_Tile^2)
         AVG_TILE_Q        = 827,
         /// Total em-scale energy of cells with bad HV in this cluster
         ENG_BAD_HV_CELLS  = 828,
         N_BAD_HV_CELLS    = 829, ///< number of cells with bad HV
         /// relative spread of pT of constiuent cells = sqrt(n)*RMS/Mean
         PTD               = 830,
         /// cell based mass i.e. the mass of the 4-vector sum of all massless positive energetic cells         
         MASS              = 831,

         EM_PROBABILITY    = 900, ///< Classification probability to be em-like
         HAD_WEIGHT        = 901, ///< Hadronic weight (E_w/E_em)
         OOC_WEIGHT        = 902, ///< Out-of-cluster weight (E_ooc/E_w)
         DM_WEIGHT         = 903, ///< Dead-material weight (E_dm/E_ooc)
         /// Confidence Level of a tile calorimeter cluster to be noise
         TILE_CONFIDENCE_LEVEL = 904,
         
         VERTEX_FRACTION = 1000, /**< Vertex fraction of this cluster wrt. primary vertex of the event. Calculated in CaloRec/CaloClusterVertexFractionMaker.cxx */
         NVERTEX_FRACTION = 1001, /**< slightly updated vertex fraction more pile up independent (similar to nJVF) */
         
         ETACALOFRAME  = 1100, ///< Eta in the calo frame (for egamma)
         PHICALOFRAME  = 1101, ///< Phi in the calo frame (for egamma)
         ETA1CALOFRAME = 1102, ///< Eta of sampling 1 in the calo frame (for egamma)
         PHI1CALOFRAME = 1103, ///< Phi of sampling 1 in the calo frame (for egamma)
         ETA2CALOFRAME = 1104, ///< Eta of sampling 2 in the calo frame (for egamma)
         PHI2CALOFRAME = 1105, ///< Phi of sampling 2 in the calo frame (for egamma)

         /// Calibration Hit energy inside the cluster
         ENG_CALIB_TOT     = 10001,
         ENG_CALIB_OUT_L   = 10010, /**< Attached Calibration Hit energy
                                       outside clusters but inside the calorimeter
                                       with loose matching (Angle < 1.0)
                                    */ 
         ENG_CALIB_OUT_M   = 10011, /**< Attached Calibration Hit energy
                                       outside clusters but inside the
                                       calorimeter with medium matching
                                       (Angle < 0.5). */ 
         ENG_CALIB_OUT_T   = 10012, /**< Attached Calibration Hit energy
                                       outside clusters but inside the
                                       calorimeter with tight matching
                                       (Angle < 0.3). */ 
         ENG_CALIB_DEAD_L  = 10020, /**< Attached Calibration Hit energy in
                                       dead material with loose matching
                                       (Angle < 1.0). */ 
         ENG_CALIB_DEAD_M  = 10021, /**< Attached Calibration Hit energy in
                                       dead material with medium matching
                                       (Angle < 0.5). */ 
         ENG_CALIB_DEAD_T  = 10022,  /**< Attached Calibration Hit energy in
                                        dead material with tight matching
                                        (Angle < 0.3). */ 
         /// Calibration Hit energy inside the cluster barrel presampler
         ENG_CALIB_EMB0         = 10030,
         /// Calibration Hit energy inside the cluster endcap presampler
         ENG_CALIB_EME0         = 10031,
         /// Calibration Hit energy inside the cluster scintillator
         ENG_CALIB_TILEG3       = 10032,
         /// Attached Calibration Hit energy in dead material
         ENG_CALIB_DEAD_TOT     = 10040,
         /// Attached Calibration Hit energy in dead material before EMB0,
         /// between EMB0 and EMB1
         ENG_CALIB_DEAD_EMB0    = 10041,
         /// Attached Calibration Hit energy in dead material between EMB3
         /// and TILE0
         ENG_CALIB_DEAD_TILE0   = 10042,
         /// Attached Calibration Hit energy in dead material before scintillator
         ENG_CALIB_DEAD_TILEG3  = 10043,
         /// Attached Calibration Hit energy in dead material before EME0,
         /// between EME0 and EME1
         ENG_CALIB_DEAD_EME0    = 10044,
         /// Attached Calibration Hit energy in dead material between EME3
         /// and HEC0
         ENG_CALIB_DEAD_HEC0    = 10045,
         /// Attached Calibration Hit energy in dead material before FCAL,
         /// between FCAL and HEC
         ENG_CALIB_DEAD_FCAL    = 10046,
         /// Attached Calibration Hit energy in dead material behind calorimeters
         ENG_CALIB_DEAD_LEAKAGE = 10047,
         /// Attached Calibration Hit energy in dead material in unclassified
         /// areas of the detector
         ENG_CALIB_DEAD_UNCLASS = 10048,
         /// Calibration Hit energy inside the cluster caused by e/gamma/pi0
         ENG_CALIB_FRAC_EM      = 10051,
         /// Calibration Hit energy inside the cluster caused by charged pi+
         /// and pi-
         ENG_CALIB_FRAC_HAD     = 10052,
         ///  Calibration Hit energy inside the cluster caused by other particles
         ENG_CALIB_FRAC_REST    = 10053,


         ENERGY_DigiHSTruth         = 40101, ///< First Moment in \f$\phi\f$
         ETA_DigiHSTruth         = 401024, ///< Eta moment that I am trying to include
         PHI_DigiHSTruth         = 401034, ///< phi moment I would like to have
         TIME_DigiHSTruth         = 40104, ///< First Moment in \f$\phi\f$
         ENERGY_CALIB_DigiHSTruth         = 40105, ///< First Moment in \f$\phi\f$
         ETA_CALIB_DigiHSTruth         = 40106, ///< First Moment in \f$\phi\f$
         PHI_CALIB_DigiHSTruth         = 40107, ///< First Moment in \f$\phi\f$
         TIME_CALIB_DigiHSTruth         = 40108, ///< First Moment in \f$\phi\f$
         FIRST_PHI_DigiHSTruth         = 50101, ///< First Moment in \f$\phi\f$
         FIRST_ETA_DigiHSTruth         = 50102, ///< First Moment in \f$\eta\f$
         SECOND_R_DigiHSTruth          = 50201, ///< Second Moment in \f$r\f$
         SECOND_LAMBDA_DigiHSTruth     = 50202, ///< Second Moment in \f$\lambda\f$
         DELTA_PHI_DigiHSTruth         = 50301,
         DELTA_THETA_DigiHSTruth       = 50302,
         DELTA_ALPHA_DigiHSTruth       = 50303,
         CENTER_X_DigiHSTruth          = 50401, ///< Cluster Centroid (\f$x\f$)
         CENTER_Y_DigiHSTruth          = 50402, ///< Cluster Centroid (\f$y\f$)
         CENTER_Z_DigiHSTruth          = 50403, ///< Cluster Centroid (\f$z\f$)
         CENTER_MAG_DigiHSTruth        = 50404,
         CENTER_LAMBDA_DigiHSTruth     = 50501, ///< Shower depth at Cluster Centroid
         LATERAL_DigiHSTruth           = 50601, ///< Normalized lateral moment
         LONGITUDINAL_DigiHSTruth      = 50602, ///< Normalized longitudinal moment
         ENG_FRAC_EM_DigiHSTruth       = 50701, ///< Energy fraction in EM calorimeters
         ENG_FRAC_MAX_DigiHSTruth      = 50702, ///< Energy fraction of hottest cell
         ENG_FRAC_CORE_DigiHSTruth     = 75003,
         FIRST_ENG_DENS_DigiHSTruth    = 50804, ///< First Moment in E/V
         SECOND_ENG_DENS_DigiHSTruth   = 50805, ///< Second Moment in E/V
         ISOLATION_DigiHSTruth         = 50806,
         ENG_BAD_CELLS_DigiHSTruth     = 50807,
         N_BAD_CELLS_DigiHSTruth       = 50808, ///< number of bad cells
         N_BAD_CELLS_CORR_DigiHSTruth  = 50809,
         BAD_CELLS_CORR_E_DigiHSTruth  = 50813,
         BADLARQ_FRAC_DigiHSTruth      = 50821,
         ENG_POS_DigiHSTruth           = 50822, ///< Total positive Energy of this cluster
         SIGNIFICANCE_DigiHSTruth      = 50823, ///< Cluster significance
         CELL_SIGNIFICANCE_DigiHSTruth = 50824,
         CELL_SIG_SAMPLING_DigiHSTruth = 50825,
         AVG_LAR_Q_DigiHSTruth         = 50826,
         AVG_TILE_Q_DigiHSTruth        = 50827,
         ENG_BAD_HV_CELLS_DigiHSTruth  = 50828,
         N_BAD_HV_CELLS_DigiHSTruth    = 50829, ///< number of cells with bad HV
         EM_PROBABILITY_DigiHSTruth    = 50900, ///< Classification probability to be em-like
         HAD_WEIGHT_DigiHSTruth        = 50901, ///< Hadronic weight (E_w/E_em)
         OOC_WEIGHT_DigiHSTruth        = 50902, ///< Out-of-cluster weight (E_ooc/E_w)
         DM_WEIGHT_DigiHSTruth         = 50903  ///< Dead-material weight (E_dm/E_ooc)
      };

     /// enum of possible signal states. 
     enum State { 
       UNKNOWN             = -1,
       UNCALIBRATED        =  0,
       CALIBRATED          =  1,
       ALTCALIBRATED       =  2,
       NSTATES             =  3
     };
     
     /// @}

     /// Default constructor
     CaloCluster_v1();

     /// Copy constructor
     CaloCluster_v1(const CaloCluster_v1& other);

     /// Destructor
     virtual ~CaloCluster_v1();

     /// Assignment operator
     CaloCluster_v1& operator=(const xAOD::CaloCluster_v1& other);

     /// @name Functions implementing the xAOD::IParticle interface
     /// @{

     /// The transverse momentum (\f$p_T\f$) of the particle (negative for negative-energy clusters)
     virtual double           pt() const;
     /// The pseudorapidity (\f$\eta\f$) of the particle
     virtual double           eta() const;
     /// The azimuthal angle (\f$\phi\f$) of the particle
     virtual double           phi() const;
     /// The invariant mass of the particle
     virtual double           m() const;
     /// The total energy of the particle
     virtual double           e() const;
     /// The true rapidity (y) of the particle
     virtual double           rapidity() const;

     /// Definition of the 4-momentum type
     typedef IParticle::FourMom_t FourMom_t;

      /// The full 4-momentum of the particle
     virtual const FourMom_t& p4() const;

     const FourMom_t& p4(const State s) const;
     
     /// The type of the object as a simple enumeration
     virtual Type::ObjectType type() const;
      /// @}

     /// @name convinience functions
     /// {
     /// The transverse energy of the cluster (negative for neg-Energy clusters with zero mass)
     double et() const;

     ///}
  // Cint has trouble with the types of these members.
  // Just hide them from reflex for now.
     /*
#ifndef __REFLEX__
   private:
     typedef flt_t (CaloCluster_v1::*GET_VALUE)() const;
     mutable GET_VALUE m_getE;
     mutable GET_VALUE m_getEta;
     GET_VALUE m_getPhi;
     GET_VALUE m_getM;
#endif
     */
   public:

     /// @name Energy/Eta/Phi per sampling
     /// @{
     /// Retrieve energy in a given sampling
     float eSample(const CaloSample sampling) const;
     /// Retrieve barycenter \f$ \eta \f$ in a given sample
     float etaSample(const CaloSample sampling) const;
     /// Retrieve barycenter \f$ \varphi \f$ in a given sample
     float phiSample(const CaloSample sampling) const;
   
      /*! \brief Retrieve maximum cell energy in given sampling */
     float energy_max(const CaloSample sampling) const;
     /*! \brief Retrieve \f$ \eta \f$ of cell with maximum energy in 
      *         given sampling */
     float etamax(const CaloSample sampling) const;
     /*! \brief Retrieve \f$ \varphi \f$ of cell with maximum energy in 
      *         given sampling */
     float phimax(const CaloSample sampling) const;

     /*! \brief Returns cluster size in \f$ \eta \f$ for a given sampling */
     float etasize(const CaloSample sampling) const;
     /*! \brief Returns cluster size in \f$ \varphi \f$ for a given sampling */
     float phisize(const CaloSample sampling) const;
     




     /**@brief Get the energy in one layer of the EM Calo 
      * @param layer Layer between 0 (Presampler) and 3 (Back)
      * @return energy
      * Works for both, barrel and endcap
      */
     float energyBE(const unsigned layer) const;

     /**@brief Get the eta in one layer of the EM Calo 
      * @param layer Layer between 0 (Presampler) and 3 (Back)
      * @return energy
      * Works for both, barrel and endcap
      */
     float etaBE(const unsigned layer) const;
     /**@brief Get the phi in one layer of the EM Calo 
      * @param layer Layer between 0 (Presampler) and 3 (Back)
      * @return energy
      * Works for both, barrel and endcap
      */
     float phiBE(const unsigned layer) const;

  
     /// Set energy for a given sampling. Returns false if the sample isn't part of the cluster
     bool setEnergy(const CaloSample sampling, const float e);
     /// Set \f$ \eta \f$ in a given sampling. Returns false if the sample isn't part of the cluster
     bool setEta(const CaloSample sampling, const float eta);
     /// Set \f$ \varphi \f$ in a given sampling. Returns false if the sample isn't part of the cluster
     bool setPhi(const CaloSample sampling, const float phi );

     /// Set the Energy of the cell with the highest energy in a particular sampling
     bool setEmax(const CaloSample sampling, const float eMax );
     /// Set the eta of the cell with the highest energy in a particular sampling
     bool setEtamax(const CaloSample sampling, const float etaMax );
     /// Set the phi of the cell with the highest energy in a particular sampling
     bool setPhimax(const CaloSample sampling, const float phiMax );  
     /// Set the cluster size in \f$ \varphi \f$ for a given sampling 
     bool setEtasize(const CaloSample sampling, const float etaSize );
     /// Set the cluster size in \f$ \vareta \f$ for a given sampling 
     bool setPhisize(const CaloSample sampling, const float phiSize );  





     /// @}

     /// @name Cluster moments
     /// @{
     /// Set individual moment
     void insertMoment( MomentType type, double value );

     /// Retrieve individual moment
     bool retrieveMoment( MomentType type, double& value ) const;

     /// Retrieve individual moment - no check for existance! Returns -999 on error
     double getMomentValue( MomentType type) const;

     /// @}



     /// @name Special Cluster Variables
     /// @{
     /// Set raw \f$ \eta \f$ of cluster seed
     void setEta0(flt_t);
     /// Returns raw \f$ \eta \f$ of cluster seed
     flt_t eta0() const;

     /// Set raw \f$ \phi \f$ of cluster seed
     void setPhi0(flt_t);
     /// Returns raw \f$ \phi \f$ of cluster seed
     flt_t phi0() const;

     /// Set cluster time 
     void setTime(flt_t);
     /// Access cluster time
     flt_t time() const;

     /// Access to sampling pattern (one bit per sampling) (Method may be removed later)
     unsigned samplingPattern() const; 
     /// Set sampling pattern (one bit per sampling
     void  setSamplingPattern(const unsigned sp, const bool clearSamplingVars=false);

     ///Clear the sampling data
     void clearSamplingData();

     unsigned nSamples() const;
     
     ///  Checks if certain smapling contributes to cluster
     bool hasSampling(const CaloSample s) const;


     /// Get cluster size 
     ClusterSize clusterSize() const; 
     /// Get cluster size
     void  setClusterSize(const ClusterSize);

     /// Returns true if at least one clustered cell in the barrel
     bool inBarrel() const;
     /// Returns true if at least one clustered cell in the endcap
     bool inEndcap() const;

     /// }@

     /// @name 4Momentum setter (signal-state dependent) 
     /// @{
     /// Set Energy for the current signal state
     void setE(flt_t);
     /// Set Eta for the current signal state
     void setEta(flt_t);
     /// Set Phi for the current signal state
     void setPhi(flt_t);
     /// Set Mass for the current signal state
     void setM(flt_t);

     /// }@

     /// @name 4Momentum in different signal states
     /// @{
     /// Get Energy in signal state UNCALIBRATED
     flt_t rawE() const; 
     /// Set Energy for signal state UNCALIBRATED
     void  setRawE(flt_t);
     /// Get \f$ \eta \f$ in signal state UNCALIBRATED
     flt_t rawEta() const; 
     /// Set \f$ \eta \f$ for signal state UNCALIBRATED
     void  setRawEta(flt_t);
     /// Get \f$ \phi \f$ in signal state UNCALIBRATED
     flt_t rawPhi() const; 
     /// Set \f$ \phi \f$ for signal state UNCALIBRATED
     void  setRawPhi(flt_t);
     /// Get mass in signal state UNCALIBRATED
     flt_t rawM() const; 
     /// Set mass for singal state UNCALIBRATED
     void  setRawM(flt_t);

     /// Get Energy in signal state ALTCALIBRATED
     flt_t altE() const; 
     /// Set Energy for signal state ALTCALIBRATED
     void  setAltE(flt_t);
     /// Get \f$ \eta \f$ in signal state ALTCALIBRATED
     flt_t altEta() const;
     /// Set \f$ \eta \f$ for signal state ALTCALIBRATED
     void  setAltEta(flt_t);
     /// Get \f$ \phi \f$ in signal state ALTCALIBRATED
     flt_t altPhi() const;
     /// Set \f$ \phi \f$ for signal state ALTCALIBRATED
     void  setAltPhi(flt_t);
     /// Get mass in signal state ALTCALIBRATED
     flt_t altM() const; 
     /// Set mass for singal state ALTCALIBRATED
     void  setAltM(flt_t);                

     /// Geet Energy in signal state CALIBRATED
     flt_t calE() const;
      /// Set Energy for signal state CALIBRATED
     void  setCalE(flt_t);
     /// Get \f$ \eta \f$ in signal state CALIBRATED
     flt_t calEta() const;
     /// Set \f$ \eta \f$ for signal state CALIBRATED
     void  setCalEta(flt_t);
     /// Get \f$ \phi \f$ in signal state  CALIBRATED
     flt_t calPhi() const;
     /// Set \f$ \phi \f$ for signal state CALIBRATED
     void  setCalPhi(flt_t);
     /// Get mass in signal state CALIBRATED
     flt_t calM() const;
     /// Set mass for singal state CALIBRATED
     void  setCalM(flt_t);
#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
   private:
#endif //not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)
     /// Switch signal state (mutable)
     bool setSignalState(const State s) const;
   public:
     /// Get the current signal state
     State signalState() const {return m_signalState;}

     /// pt with a given signal state
     double pt(const State s) const;

     /// energy with a given signal state
     double e(const State s) const;

     /// eta with a given signal state
     double eta(const State s) const;

     /// eta with a given signal state
     double phi(const State s) const;

     ///  @}


     /// Get eta size from cluster size 
     unsigned int getClusterEtaSize() const;

     /// Get phi size from cluster size 
     unsigned int getClusterPhiSize() const;

     void setBadChannelList(const CaloClusterBadChannelList& bcl); 
     const CaloClusterBadChannelList& badChannelList() const;
     
     /// Get a pointer to a 'sister' cluster (eg the non-calibrated counterpart)
     const CaloCluster_v1* getSisterCluster() const;

#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))
     /// Set a pointer to a 'sister' cluster (eg the non-calibrated counterpart)
     bool setSisterCluster(const std::string& sisterSgKey, const unsigned sisterIndex, IProxyDictWithPool* sg= nullptr);
#endif

     //For debugging only...
     //std::vector<std::pair<std::string,float> > getAllMoments();

   private:
     /// bit-pattern describing the calo samplings contributing to this cluster
     unsigned m_samplingPattern;
     /// Cached 4-momentum objects (one per signal state)
     mutable FourMom_t m_p4[xAOD::CaloCluster_v1::NSTATES];
     /// Cache state of the internal 4-momentum (reset from the streamer)
     mutable std::bitset<xAOD::CaloCluster_v1::NSTATES> m_p4Cached;

     mutable double m_pt[xAOD::CaloCluster_v1::NSTATES];
     /// Cache state of the internal 4-momentum (reset from the streamer)
     mutable std::bitset<xAOD::CaloCluster_v1::NSTATES> m_ptCached;

     /// Current signal state
     mutable State m_signalState;
     ///Non-const ptr to cell links (for cluster building, transient-only)
     CaloClusterCellLink* m_cellLinks;

     ///Flag to indicate if the cell-links are owned by ths cluster or by a separate DataVector
     bool m_ownCellLinks;

     ///Reco status (transient only)
     CaloRecoStatus m_recoStatus;

     unsigned sampVarIdx(const CaloSample) const;

     float getSamplVarFromAcc(Accessor<std::vector<float > >& acc, 
			      const CaloSample sampling, const float errorvalue=-999) const; //FIXME find a better errorcode

     bool setSamplVarFromAcc(Accessor<std::vector<float> >& acc, 
			     const CaloSample sampling, const float value);
   public:
#if !(defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS))

     /// @name Athena-only methods, used during building stage
     /// @{
     /// Set up an ElementLink to a CaloClusterCellLink object
     void addCellLink(CaloClusterCellLink* CCCL) {
       if (m_ownCellLinks && m_cellLinks) {
	 //Delete link if there is one
	 delete m_cellLinks;
       }

       m_cellLinks=CCCL;
       m_ownCellLinks=true;
     }
     /**@brief Set up an ElementLink to a CaloClusterCellLink object
      * @param CCCL_key StoreGate key of the CaloClusterCellLinkContainer
      * @param index Index of inside the CaloClusterCellLinkContainer container
      * @return true on success
      */     
     //bool createCellElemLink(const std::string& CCCL_key, const size_t index);

     /**@brief Push the CaloClusterCellLink object into the cell-link container and hand-over ownership to it
      * @param CCCL pointer to the CaloClusterCellLinkContainer
      * @param sg Explicitly specify the store to use for the ElementLink.
      * @return true on success
      */  
     bool setLink(CaloClusterCellLinkContainer* CCCL,
                  IProxyDictWithPool* sg = nullptr);

     /**@brief Get a pointer to the CaloClusterCellLink object (const version)
      * @return ptr to CaloClusterCellLink obj, NULL if no valid link
      */  
     const CaloClusterCellLink* getCellLinks() const;

     /**@brief Get a pointer to the CaloClusterCellLink object (const version)
      * @return ptr to CaloClusterCellLink obj, NULL if no valid link
      */ 
     CaloClusterCellLink* getCellLinks() {
       return m_cellLinks;
     }

      /**@brief Method to add a cell to the cluster (Beware: Kinematics not updated!)
      * @param index Index of the cell in the CaloCelLContainer
      * @param weight Weight of the cell in the cluster
      * @return true if the link to the CaloClusterCellLinkContainer exists, false otherwise
      */ 
     bool addCell(const unsigned index, const double weight) {
       if (!m_cellLinks) return false;
       return m_cellLinks->addCell(index,weight);
     }

     /**@brief Method to remove a cell to the cluster (slow!) (Beware: Kinematics not updated!)
      * @param index Index of the cell in the CaloCelLContainer
      * @return true on success, false if the cell or the CaloCellLinkContainer is not found 
      * A more efficient way to remove cells is to use the iterator fucnctionality of the 
      * linked CaloCellLink object and use the remove(iterator) method from there.
      */ 
      bool removeCell(const CaloCell* ptr);


     /**@brief size method (forwarded from CaloClusterCellLink obj)
      *@return The number of cells 
      */
     size_t size() const {return getCellLinks()->size();}

     ///Iterator of the underlying CaloClusterCellLink (const version)
     typedef CaloClusterCellLink::const_iterator const_cell_iterator; 
     //Fixme: Check ret-val of getCellLinks (might be NULL);
     const_cell_iterator cell_begin() const { 
       const CaloClusterCellLink* links=getCellLinks();
       if (!links) 
	 return CaloClusterCellLink::dummyIt;
       else
	 return links->begin();
     }
     const_cell_iterator cell_end() const { 
       const CaloClusterCellLink* links=getCellLinks();
       if (!links) 
	 return CaloClusterCellLink::dummyIt;
       else
	 return getCellLinks()->end();
     } 
     
     ///Iterator of the underlying CaloClusterCellLink (non-const version)
     typedef CaloClusterCellLink::iterator cell_iterator; 
     //Fixme: Check ret-val of getCellLinks (might be NULL);
     cell_iterator cell_begin() { return getCellLinks()->begin();}
     cell_iterator cell_end() { return getCellLinks()->end();} 

     /// STL-compatible iterators.
     typedef const_cell_iterator const_iterator;
     typedef cell_iterator iterator;
     const_iterator begin() const { return cell_begin(); }
     const_iterator end() const { return cell_end(); }
     iterator begin() { return cell_begin(); }
     iterator end() { return cell_end(); }

     /**@brief Method to reweight a cell in the cluster (Beware: Kinematics not updated!)
      * @param it Non-const cell-iterator
      * @param weight The new weight
      */ 
     void reweightCell(cell_iterator it, const double weight) {it.reweight(weight);}
     
     /**@brief Accesssor to CaloRecoStatus (non-const)
      * @return Reference to the CaloRecoStatus of this cluster
      */ 
     CaloRecoStatus& recoStatus() {return m_recoStatus;}

     /**@brief Accesssor to CaloRecoStatus (const)
      * @return const reference to the CaloRecoStatus of this cluster
      */ 
     const CaloRecoStatus& recoStatus() const {return m_recoStatus;}
     ///  @}

#endif // not defined(SIMULATIONBASE) || defined(XAOD_ANALYSIS)

      /// Function preparing the object to be persistified
      void toPersistent();

   }; // class CaloCluster_v1


  inline  double CaloCluster_v1::et() const {
    if (this->m()==0) 
      return this->pt();
    else
      return this->p4().Et();
  }

  inline unsigned CaloCluster_v1::samplingPattern() const {
    return m_samplingPattern;
  }

  inline unsigned CaloCluster_v1::sampVarIdx(const CaloCluster_v1::CaloSample s) const {
    const uint32_t& pattern= m_samplingPattern;
    //std::cout << "Pattern=" << std::hex << pattern << std::dec << ", Sampling=" << s << std::endl;
    if ((pattern & (0x1U << s))==0)
      return CaloSampling::Unknown;
    else {
      if (s==0) return 0; //shifting a 32-bit int by 32 bits is undefined behavior! 
      return __builtin_popcount(pattern << (32-s)); 
      //Explanation: Need to get the number of bit (=samples) before the sampling in question
      //Shift to the left, so bits after the sampling in question fall off the 32bit integer
      //Then use gcc builtin popcount to count the numbers of 1 in the rest
      //AFAIK, this builtin is available for gcc, icc and clang (as well on ARM)
    }
  }


  inline unsigned CaloCluster_v1::nSamples() const {
    const uint32_t pattern=samplingPattern();
    return  __builtin_popcount(pattern);
  }


  inline bool CaloCluster_v1::hasSampling(const CaloSample s) const {
   const unsigned pattern=samplingPattern();
   return (pattern & (0x1U<<(uint32_t)s));
 }


  inline bool CaloCluster_v1::inBarrel() const {
       return (m_samplingPattern & CaloSampling::barrelPattern());
  }
     
  
  inline bool CaloCluster_v1::inEndcap() const {
    return (m_samplingPattern & CaloSampling::endcapPattern());
  }


  inline double CaloCluster_v1::getMomentValue( CaloCluster_v1::MomentType type) const {
    double value=-999;
    (void)this->retrieveMoment(type,value);
    return value;
  }

} // namespace xAOD

#endif // XAODCALOEVENT_VERSIONS_CALOCLUSTER_V1_H
