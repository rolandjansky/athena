/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TrkVKalVrtFitter.h
//
#ifndef TRKVKALVRTFITTER_VKALVRTFITTER_H
#define TRKVKALVRTFITTER_VKALVRTFITTER_H
// Normal STL and physical vectors
// Gaudi includes
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
//
// Interfaces
#include  "TrkVertexFitterInterfaces/IVertexFitter.h"
#include  "TrkVKalVrtFitter/ITrkVKalVrtFitter.h"
#include  "TrkVKalVrtFitter/IVertexCascadeFitter.h"
#include  "TrkVKalVrtFitter/VKalVrtAtlas.h"
#include  "TrkVKalVrtCore/TrkVKalVrtCore.h"

#include  "TrkNeutralParameters/NeutralParameters.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
//
class IChronoStatSvc;

namespace Trk{

  class VKalVrtControl;

  enum { NTrMaxVFit=200 };
  typedef std::vector<double> dvect;
  class VKalAtlasMagFld;
  class IExtrapolator;
  class IMagneticFieldTool;
  class VKalExtPropagator;
  class Track;
  class TrackParticleBase;

  struct PartialMassConstraint{
    VertexID VRT;
    std::vector<int> trkInVrt;              // positions of participating tracks in common track list
    std::vector<VertexID> pseudoInVrt;      // VertexIDs of pseudos
    double Mass;
  };

  struct cascadeV{                        //  general description of vertex in cascade
    VertexID vID;                         //  vertex ID - used everywhere
    std::vector<int> trkInVrt;            //  list of tracks attached to vertex directly
    VertexID outPointingV;                //  to which vertex  it points
    std::vector<VertexID> inPointingV;    //  which vertices points to it
    VertexID mergedTO;                    //  merged to another vertex (not separate anymore)
    std::vector<VertexID> mergedIN;       //  vertices attached to current
    int indexInSimpleCascade;
    cascadeV(){ vID=-999; outPointingV=0; mergedTO=0; indexInSimpleCascade=0;};
   ~cascadeV() = default;
  };


//------------------------------------------------------------------------
  //Tool should not be used reentrantly or used publically
  class TrkVKalVrtFitter : public extends<AthAlgTool, IVertexFitter,
                                          ITrkVKalVrtFitter,
					  IVertexCascadeFitter>
  {
    friend class VKalExtPropagator;
    public:

      // The following 'using' can be removed when IVertexFitter::fit has been fully migrated to the one with the EventContext
      using Trk::IVertexFitter::fit;
      using Trk::ITrkVKalVrtFitter::makeState;

        virtual StatusCode initialize() override final;
        virtual StatusCode finalize() override final;

        TrkVKalVrtFitter(const std::string& t, const std::string& name, const IInterface*  parent);
        virtual ~TrkVKalVrtFitter();

        //
        // IVertexFitter interface
        //

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>& perigeeList,
          const Amg::Vector3D& startingPoint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>& perigeeList,
          const std::vector<const NeutralParameters*>& /*neutralPerigeeList*/,
          const Amg::Vector3D& startingPoint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>& perigeeList,
          const xAOD::Vertex& constraint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>& perigeeList,
          const std::vector<const NeutralParameters*>& /*neutralPerigeeList*/,
          const xAOD::Vertex& constraint) const override final;

        virtual std::unique_ptr<xAOD::Vertex> fit(
          const EventContext& ctx,
          const std::vector<const xAOD::TrackParticle*>& vectorTrk,
          const Amg::Vector3D& startingPoint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const xAOD::TrackParticle*>& vectorTrk,
          const xAOD::Vertex& constraint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const xAOD::TrackParticle*>& vectorTrk,
          const std::vector<const xAOD::NeutralParticle*>& vectorNeu,
          const Amg::Vector3D& startingPoint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const xAOD::TrackParticle*>& vectorTrk,
          const std::vector<const xAOD::NeutralParticle*>& vectorNeu,
          const xAOD::Vertex& constraint) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>&) const override final;

        virtual xAOD::Vertex* fit(
          const std::vector<const TrackParameters*>&,
          const std::vector<const Trk::NeutralParameters*>&) const override final;


        /*--------------  Additional  xAOD  interfaces -------------*/
        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                           const Amg::Vector3D& constraint,
                           IVKalState& istate) const;
        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                           const xAOD::Vertex& constraint,
                           IVKalState& istate) const;
//
//  Cascade fitter interface
//
        VertexID startVertex(
          const std::vector<const xAOD::TrackParticle*>& list,
          const std::vector<double>& particleMass,
          IVKalState& istate,
          double massConstraint = 0.) const override final;

        VertexID nextVertex(const std::vector<const xAOD::TrackParticle*>& list,
                            const std::vector<double>& particleMass,
                            IVKalState& istate,
                            double massConstraint = 0.) const override final;

        VertexID nextVertex(const std::vector<const xAOD::TrackParticle*>& list,
                            const std::vector<double>& particleMass,
                            const std::vector<VertexID>& precedingVertices,
                            IVKalState& istate,
                            double massConstraint = 0.) const override final;

        VxCascadeInfo* fitCascade(IVKalState& istate,
                                  const Vertex* primVertex = 0,
                                  bool FirstDecayAtPV = false) const override final;

        StatusCode addMassConstraint(
          VertexID Vertex,
          const std::vector<const xAOD::TrackParticle*>& tracksInConstraint,
          const std::vector<VertexID>& verticesInConstraint,
          IVKalState& istate,
          double massConstraint) const override final;

        //------ specific VKalVrt staff

        virtual std::unique_ptr<IVKalState> makeState(
          const EventContext& ctx) const override final;

        virtual StatusCode VKalVrtFit(
          const std::vector<const xAOD::TrackParticle*>&,
          const std::vector<const xAOD::NeutralParticle*>&,
          Amg::Vector3D& Vertex,
          TLorentzVector& Momentum,
          long int& Charge,
          dvect& ErrorMatrix,
          dvect& Chi2PerTrk,
          std::vector<std::vector<double>>& TrkAtVrt,
          double& Chi2,
          IVKalState& istate,
          bool ifCovV0 = false) const override final;

        virtual StatusCode VKalVrtFit(
          const std::vector<const Track*>&,
          Amg::Vector3D& Vertex,
          TLorentzVector& Momentum,
          long int& Charge,
          dvect& ErrorMatrix,
          dvect& Chi2PerTrk,
          std::vector<std::vector<double>>& TrkAtVrt,
          double& Chi2,
          IVKalState& istate,
          bool ifCovV0 = false) const override final;

        virtual StatusCode VKalVrtFit(
          const std::vector<const TrackParameters*>&,
          const std::vector<const NeutralParameters*>&,
          Amg::Vector3D& Vertex,
          TLorentzVector& Momentum,
          long int& Charge,
          dvect& ErrorMatrix,
          dvect& Chi2PerTrk,
          std::vector<std::vector<double>>& TrkAtVrt,
          double& Chi2,
          IVKalState& istate,
          bool ifCovV0 = false) const override final;
        //------
        virtual StatusCode VKalVrtCvtTool(const Amg::Vector3D& Vertex,
                                          const TLorentzVector& Momentum,
                                          const dvect& CovVrtMom,
                                          const long int& Charge,
                                          dvect& Perigee,
                                          dvect& CovPerigee,
                                          IVKalState& istate) const override final;
        //-----
        virtual StatusCode VKalVrtFitFast(
          const std::vector<const xAOD::TrackParticle*>&,
          Amg::Vector3D& Vertex, double &minDZ,
          IVKalState& istate) const;

        virtual StatusCode VKalVrtFitFast(
          const std::vector<const xAOD::TrackParticle*>&,
          Amg::Vector3D& Vertex,
          IVKalState& istate) const override final;

        virtual StatusCode VKalVrtFitFast(
          const std::vector<const TrackParameters*>&,
          Amg::Vector3D& Vertex,
          IVKalState& istate) const override final;
        //-----
        virtual
        Trk::Track* CreateTrkTrack(const std::vector<double>& VKPerigee,
                                   const std::vector<double>& VKCov,
                                   const IVKalState& istate) const override final;

        virtual StatusCode VKalGetTrkWeights(
          dvect& Weights,
          const IVKalState& istate) const override final;

        virtual StatusCode VKalGetFullCov(long int,
                                          dvect& CovMtx,
                                          const IVKalState& istate,
                                          bool = false) const override final;

        virtual StatusCode VKalGetMassError(
          double& Mass,
          double& MassError,
          const IVKalState& istate) const override final;

        virtual void setApproximateVertex(double X,
                                          double Y,
                                          double Z,
                                          IVKalState& istate) const override final;

        virtual void setMassForConstraint(double Mass,
                                          IVKalState& istate) const override final;

        virtual void setMassForConstraint(double Mass,
                                          const std::vector<int>&,
                                          IVKalState& istate) const override final;

        virtual void setRobustness(int, IVKalState& istate) const override final;

        virtual void setRobustScale(double, IVKalState& istate) const override final;

        virtual void setCnstType(int, IVKalState& istate) const override final;

        virtual void setVertexForConstraint(const xAOD::Vertex&,
                                            IVKalState& istate) const override final;

        virtual void setVertexForConstraint(double X,
                                            double Y,
                                            double Z,
                                            IVKalState& istate) const override final;

        virtual void setCovVrtForConstraint(double XX,
                                            double XY,
                                            double YY,
                                            double XZ,
                                            double YZ,
                                            double ZZ,
                                            IVKalState& istate) const override final;

        virtual void setMassInputParticles(const std::vector<double>&,
                                           IVKalState& istate) const override final;

        virtual double VKalGetImpact(const xAOD::TrackParticle*,
                                     const Amg::Vector3D& Vertex,
                                     const long int Charge,
                                     dvect& Impact,
                                     dvect& ImpactError,
                                     IVKalState& istate) const override final;

        virtual double VKalGetImpact(const Track*,
                                     const Amg::Vector3D& Vertex,
                                     const long int Charge,
                                     dvect& Impact,
                                     dvect& ImpactError,
                                     IVKalState& istate) const override final;

        virtual double VKalGetImpact(const xAOD::TrackParticle*,
                                     const Amg::Vector3D& Vertex,
                                     const long int Charge,
                                     dvect& Impact,
                                     dvect& ImpactError) const override final;

        virtual double VKalGetImpact(const Track*,
                                     const Amg::Vector3D& Vertex,
                                     const long int Charge,
                                     dvect& Impact,
                                     dvect& ImpactError) const override final;

        //
        // ATLAS related code
        //
      private:
        SimpleProperty<int> m_Robustness;
        SimpleProperty<double> m_RobustScale;
        SimpleProperty<double> m_cascadeCnstPrecision;
        SimpleProperty<double> m_massForConstraint;
        SimpleProperty<int> m_IterationNumber;
        SimpleProperty<double> m_IterationPrecision;
        SimpleProperty<double> m_IDsizeR;
        SimpleProperty<double> m_IDsizeZ;
        std::vector<double> m_c_VertexForConstraint;
        std::vector<double> m_c_CovVrtForConstraint;
        std::vector<double> m_c_MassInputParticles;

        ToolHandle<IExtrapolator> m_extPropagator; // External propagator
        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj>
          m_fieldCacheCondObjInputKey{ this,
                                       "AtlasFieldCacheCondObj",
                                       "fieldCondObj",
                                       "Name of the Magnetic Field key" };
        SimpleProperty<bool> m_firstMeasuredPoint;
        SimpleProperty<bool> m_firstMeasuredPointLimit;
        SimpleProperty<bool> m_makeExtendedVertex;
        SimpleProperty<bool> m_useFixedField;

        bool m_isAtlasField;

        bool m_useAprioriVertex;
        bool m_useThetaCnst;
        bool m_usePhiCnst;
        bool m_usePointingCnst;
        bool m_useZPointingCnst;
        bool m_usePassNear;
        bool m_usePassWithTrkErr;
        void initCnstList();

        //  Track material effects control
        //
        struct TrkMatControl
        {
          // Track reference point(hit) in global ATLAS frame
          Amg::Vector3D trkRefGlobPos;
          int extrapolationType;
          int TrkID;
          const TrackParameters* TrkPnt;
          double prtMass;
          Amg::Vector3D trkSavedLocalVertex; // Local VKalVrtCore vertex
        };

      class CascadeState
      {
      public:
        int m_cascadeSize = 0;
        std::vector<const xAOD::TrackParticle*> m_partListForCascade;

        std::vector<double>  m_partMassForCascade;
        std::vector<PartialMassConstraint> m_partMassCnstForCascade;
        //
        std::vector<cascadeV>    m_cascadeVList;               // list of cascade vertex objects
      };

      class State : public IVKalState
      {
      public:
        //
        //  Origin of global reference frame.
        //  (0,0,0) by default but can be changed by input tracks
        //
        double m_refFrameX = 0, m_refFrameY = 0, m_refFrameZ = 0;
        std::vector < TrkMatControl > m_trkControl;

        //
        // Arrays needed for fitting kernel
        //
        double m_apar[NTrMaxVFit][5];   //used only for fit preparation
        double m_awgt[NTrMaxVFit][15];  //used only for fit preparation
        long int m_ich[NTrMaxVFit];
        double m_parfs[NTrMaxVFit][3];

        VKalAtlasMagFld m_fitField;
        VKalVrtControl m_vkalFitControl;
        MagField::AtlasFieldCache m_fieldCache;

        const TrackParameters *  m_globalFirstHit = nullptr;

        double m_save_xyzfit[3];  //  Save vertex after successful fit

        int m_FitStatus = 0;    /* Fit Status flag*/
                                /* =0 - no fit. All "after fit" routines fail*/
                                /* >1 - good fit. "After fit" routines work*/

        //
        // Array for full error matrix
        //
        std::vector<double> m_ErrMtx;

        int m_planeCnstNDOF = 0;  /* NDOF addition if plane constraint is used */
        std::vector<double>    m_ApproximateVertex;

        bool m_useAprioriVertex = false;
        bool m_useThetaCnst = false;
        bool m_usePhiCnst = false;
        bool m_usePointingCnst = false;
        bool m_useZPointingCnst = false;
        bool m_usePassNear = false;
        bool m_usePassWithTrkErr = false;

        std::vector<double>    m_VertexForConstraint;
        std::vector<double>    m_CovVrtForConstraint;
        double                 m_massForConstraint = -1;
        std::vector<double>    m_partMassCnst;
        std::vector< std::vector<int> >    m_partMassCnstTrk;
        int m_Robustness = 0;
        double m_RobustScale = 1;
        std::vector<double>    m_MassInputParticles;

        std::unique_ptr<CascadeState> m_cascadeState;

        State()
          : m_vkalFitControl (VKalVrtControlBase(&m_fitField,nullptr,nullptr,nullptr,this)) {
          m_ApproximateVertex.reserve(3);
        }
      };

//-----------------------------------------------------------------
//  Cascade related stuff
//
      void makeSimpleCascade(std::vector< std::vector<int> > &, std::vector< std::vector<int> > &,
                             CascadeState& cstate) const;
      void printSimpleCascade(std::vector< std::vector<int> > &, std::vector< std::vector<int> > &,
                              const CascadeState& cstate) const;
      int findPositions(const std::vector<int> &,const std::vector<int> &, std::vector<int> &) const;
      int getSimpleVIndex( const VertexID &, const CascadeState& cstate) const;
      int indexInV( const VertexID &, const CascadeState& cstate) const;
      int getCascadeNDoF (const CascadeState& cstate) const;
//----------------------
//  Control variables
//

      double m_BMAG;       /* const magnetic field  if needed */
      double m_CNVMAG;     /* Conversion constant */


      VKalExtPropagator*     m_fitPropagator;
      const IExtrapolator*   m_InDetExtrapolator;     //!< Pointer to Extrapolator AlgTool
//
//
//



//
//  Private technical functions
//
        void setAthenaPropagator(const Trk::IExtrapolator*);
        // context-aware init of state
        void initState (const EventContext& ctx, State& state) const;
        // init of state for backwards compartibility - calls context-aware version. Can be removed
        // when fully migrated to EventContext
        void initState (State& state) const;

//
//
        void FillMatrixP(AmgSymMatrix(5)& , std::vector<double>& ) const;
        void FillMatrixP(int iTrk, AmgSymMatrix(5)& , std::vector<double>& ) const;
        Amg::MatrixX * GiveFullMatrix(int NTrk, std::vector<double>&) const;
        bool convertAmg5SymMtx(const AmgSymMatrix(5)*, double[] ) const;

        void VKalTransform(double MAG,
                           double A0V,
                           double ZV,
                           double PhiV,
                           double ThetaV,
                           double PInv,
                           double[],
                           long int& Charge,
                           double[],
                           double[]) const;

        xAOD::Vertex* makeXAODVertex(int,
                                     const Amg::Vector3D&,
                                     const dvect&,
                                     const dvect&,
                                     const std::vector<dvect>&,
                                     double,
                                     const State& state) const;

        StatusCode CvtTrkTrack(const std::vector<const Track*>& list,
                               int& ntrk,
                               State& state) const;

        StatusCode CvtTrackParticle(
          const std::vector<const xAOD::TrackParticle*>& list,
          int& ntrk,
          State& state) const;

        StatusCode CvtNeutralParticle(
          const std::vector<const xAOD::NeutralParticle*>& list,
          int& ntrk,
          State& state) const;

        StatusCode CvtTrackParameters(
          const std::vector<const TrackParameters*>& InpTrk,
          int& ntrk,
          State& state) const;

        StatusCode CvtNeutralParameters(
          const std::vector<const NeutralParameters*>& InpTrk,
          int& ntrk,
          State& state) const;

        void VKalVrtConfigureFitterCore(int NTRK, State& state) const;

        void VKalToTrkTrack(double curBMAG,
                            double vp1,
                            double vp2,
                            double vp3,
                            double& tp1,
                            double& tp2,
                            double& tp3) const;

        int VKalVrtFit3(int ntrk,
                        Amg::Vector3D& Vertex,
                        TLorentzVector& Momentum,
                        long int& Charge,
                        dvect& ErrorMatrix,
                        dvect& Chi2PerTrk,
                        std::vector<std::vector<double>>& TrkAtVrt,
                        double& Chi2,
                        State& state,
                        bool ifCovV0) const;

        Perigee* CreatePerigee(double Vx,
                               double Vy,
                               double Vz,
                               const std::vector<double>& VKPerigee,
                               const std::vector<double>& VKCov,
                               const State& state) const;

        //
        //
        const Perigee* GetPerigee(const TrackParameters* i_ntrk) const;

        int VKalGetNDOF(const State& state) const;
   };

} //end of namespace

#endif
