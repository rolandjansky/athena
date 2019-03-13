/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrkVKalVrtFitter.h
//
#ifndef _TrkVKalVrtFitter_VKalVrtFitter_H
#define _TrkVKalVrtFitter_VKalVrtFitter_H
// Normal STL and physical vectors
//#include <vector>
// Gaudi includes
//#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
//
// Interfaces
#include  "TrkVertexFitterInterfaces/IVertexFitter.h"
#include  "TrkVKalVrtFitter/ITrkVKalVrtFitter.h"
#include  "TrkVKalVrtFitter/IVertexCascadeFitter.h"

//#include  "xAODTracking/NeutralParticleContainer.h"
//#include  "xAODTracking/TrackParticleContainer.h"
//#include  "xAODTracking/VertexContainer.h"

#include  "TrkParameters/TrackParameters.h" 
#include  "TrkNeutralParameters/NeutralParameters.h"
//#include  "VxVertex/ExtendedVxCandidate.h"
#include  "MagFieldInterfaces/IMagFieldSvc.h"
//
#include <thread>
#include <mutex>
//
class IChronoStatSvc;
class IMagFieldAthenaSvc;

namespace Trk{

  class VKalVrtControl;

  enum { NTrMaxVFit=300 };
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
   ~cascadeV(){};
  };
  

//------------------------------------------------------------------------

  class TrkVKalVrtFitter : public AthAlgTool, virtual public IVertexFitter,
                                              virtual public ITrkVKalVrtFitter,
					      virtual public IVertexCascadeFitter
  {
    friend class VKalExtPropagator;
    public:

        StatusCode initialize();
        StatusCode finalize();

        TrkVKalVrtFitter(const std::string& t, const std::string& name, const IInterface*  parent);
        virtual ~TrkVKalVrtFitter(); 

//
// IVertexFitter interface
//
        xAOD::Vertex * fit(const std::vector<const Track*> & vectorTrk,
                                  const Amg::Vector3D& startingPoint);

        xAOD::Vertex * fit(const std::vector<const Track*>& vectorTrk,
                                  const xAOD::Vertex& constraint);

        xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
                                  const Amg::Vector3D& startingPoint);

        xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
			  const std::vector<const NeutralParameters*> & /*neutralPerigeeList*/,
			  const Amg::Vector3D& startingPoint);
 
        xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
                                  const xAOD::Vertex& constraint);

        xAOD::Vertex * fit(const std::vector<const TrackParameters*> & perigeeList,
			  const std::vector<const NeutralParameters*> & /*neutralPerigeeList*/,
			  const xAOD::Vertex& constraint);

        /*--------------   Interface for xAOD    -------------*/ 
        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
                                 const Amg::Vector3D& startingPoint); 
        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk, 
                                 const xAOD::Vertex& constraint); 

        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>   & vectorTrk,
			   const std::vector<const xAOD::NeutralParticle*> & vectorNeu, 
			   const Amg::Vector3D& startingPoint);
        xAOD::Vertex * fit(const std::vector<const xAOD::TrackParticle*>   & vectorTrk, 
			   const std::vector<const xAOD::NeutralParticle*> & vectorNeu, 
			   const xAOD::Vertex& constraint);


        xAOD::Vertex * fit(const std::vector<const TrackParticleBase*> & vectorTrk,
	                          const Amg::Vector3D & startingPoint);

        xAOD::Vertex * fit(const std::vector<const TrackParticleBase*>& vectorTrk,
	                          const xAOD::Vertex& constraint);

        xAOD::Vertex * fit(const std::vector<const TrackParameters*> & );
        xAOD::Vertex * fit(const std::vector<const TrackParameters*> &, const std::vector<const Trk::NeutralParameters*> & );
        xAOD::Vertex * fit(const std::vector<const Track*>& );

//
//  Cascade fitter interface
//
        VertexID startVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                             const  std::vector<double>& particleMass,
				   double massConstraint = 0.);
 
        VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                             const  std::vector<double>& particleMass,
				   double massConstraint = 0.);
 
        VertexID  nextVertex(const  std::vector<const xAOD::TrackParticle*> & list,
                             const  std::vector<double>& particleMass,
		             const  std::vector<VertexID> precedingVertices,
				   double massConstraint = 0.);
        VxCascadeInfo * fitCascade(const Vertex * primVertex = 0, bool FirstDecayAtPV = false );

        StatusCode  addMassConstraint(VertexID Vertex,
                                      const std::vector<const xAOD::TrackParticle*> & tracksInConstraint,
                                      const std::vector<VertexID> verticesInConstraint, 
				      double massConstraint );

//------ Personal VKalVrt staff

        StatusCode VKalVrtFit(const std::vector<const xAOD::TrackParticle*>&,
                              const std::vector<const xAOD::NeutralParticle*>&,
                              Amg::Vector3D&         Vertex,
                              TLorentzVector&     Momentum,
	                      long int&           Charge,
		              dvect&              ErrorMatrix,
	                      dvect&              Chi2PerTrk,
		              std::vector< std::vector<double> >& TrkAtVrt,
		              double& Chi2 );
        StatusCode VKalVrtFit(const std::vector<const Track*>&,
                              Amg::Vector3D&         Vertex,
                              TLorentzVector&     Momentum,
	                      long int&           Charge,
		              dvect&              ErrorMatrix,
	                      dvect&              Chi2PerTrk,
		              std::vector< std::vector<double> >& TrkAtVrt,
		              double& Chi2 );
        StatusCode VKalVrtFit(const std::vector<const TrackParticleBase*>&,
                              Amg::Vector3D&         Vertex,
                              TLorentzVector&     Momentum,
	                      long int&           Charge,
		              dvect&              ErrorMatrix,
	                      dvect&              Chi2PerTrk,
		              std::vector< std::vector<double> >& TrkAtVrt,
		              double& Chi2 );
        StatusCode VKalVrtFit(const std::vector<const TrackParameters*>&,
                              const std::vector<const NeutralParameters*>&,
                              Amg::Vector3D&         Vertex,
                              TLorentzVector&     Momentum,
	                      long int&           Charge,
		              dvect&              ErrorMatrix,
	                      dvect&              Chi2PerTrk,
		              std::vector< std::vector<double> >& TrkAtVrt,
		              double& Chi2 );
//------
        StatusCode VKalVrtCvtTool(const Amg::Vector3D& Vertex,
	                          const TLorentzVector& Momentum,
	                          const dvect& CovVrtMom,
				  const long int& Charge,
				  dvect& Perigee,
				  dvect& CovPerigee);
//-----
        StatusCode VKalVrtFitFast(const std::vector<const Trk::Track*>&, Amg::Vector3D& Vertex);
        StatusCode VKalVrtFitFast(const std::vector<const xAOD::TrackParticle*>&, Amg::Vector3D& Vertex);
        StatusCode VKalVrtFitFast(const std::vector<const TrackParticleBase*>&, Amg::Vector3D& Vertex);
        StatusCode VKalVrtFitFast(const std::vector<const TrackParameters*>&, Amg::Vector3D& Vertex);
//-----
        Trk::Track* CreateTrkTrack(const std::vector<double>& VKPerigee,
                                   const std::vector<double>& VKCov);
        StatusCode VKalGetTrkWeights(dvect& Weights);
        StatusCode VKalGetTrkCov(long int, long int,dvect& CovMtx);
        StatusCode VKalGetFullCov(long int, dvect& CovMtx, int =0);

        StatusCode VKalGetMassError(double& Mass, double& MassError);
        int VKalGetNDOF();
//        VxCandidate * makeVxCandidate( int ,
//                const Amg::Vector3D& , const std::vector<double> & , 
//	        const std::vector<double> & ,  const std::vector< std::vector<double> >& , double ); 
        xAOD::Vertex * makeXAODVertex( int,  const Amg::Vector3D&,
                            const dvect&, const dvect&, const std::vector< dvect >&, double); 
//-----

        void setApproximateVertex(double,double,double);
        void setMassForConstraint(double);
        void setMassForConstraint(double,std::vector<int>);
        void setRobustness(int);
        void setRobustScale(double);
        void setCascadeCnstPrec(double);
        void setCnstType(int);
        void setIterations(int, double);
        void setVertexForConstraint(const xAOD::Vertex & );
        void setVertexForConstraint(double,double,double);
        void setCovVrtForConstraint(double,double,double,double,double,double);
				  
        void setMassInputParticles( const std::vector<double>& );
        void setMomCovCalc(int);
        void setDefault();
        void setZeroCharge(int);
        void clearMemory();
      double VKalGetImpact(const xAOD::TrackParticle*,const Amg::Vector3D& Vertex, const long int Charge,
                         dvect& Impact, dvect& ImpactError);
      double VKalGetImpact(const TrackParticleBase*,const Amg::Vector3D& Vertex, const long int Charge,
                         dvect& Impact, dvect& ImpactError);
      double VKalGetImpact(const Track*,const Amg::Vector3D& Vertex, const long int Charge,
                         dvect& Impact, dvect& ImpactError);


//
// ATLAS related code
//
    private:

      void setAthenaField(MagField::IMagFieldSvc*);
      void setAthenaField(const double );
      void setAthenaPropagator(const Trk::IExtrapolator*);

    private:


      SimpleProperty<int>    m_Robustness;
      SimpleProperty<double> m_RobustScale;
      SimpleProperty<double> m_cascadeCnstPrecision;
      SimpleProperty<double> m_massForConstraint;
      SimpleProperty<int>    m_IterationNumber;
      SimpleProperty<double> m_IterationPrecision;
      SimpleProperty<double> m_IDsizeR;
      SimpleProperty<double> m_IDsizeZ;
      std::vector<double>    m_c_VertexForConstraint;
      std::vector<double>    m_c_CovVrtForConstraint;
      std::vector<double>    m_c_MassInputParticles;
      std::vector<int>       m_c_TrackCharge;
      std::vector<double>    m_VertexForConstraint;
      std::vector<double>    m_CovVrtForConstraint;
      std::vector<double>    m_MassInputParticles;
      std::vector<int>       m_TrackCharge;

      ToolHandle < IExtrapolator >          m_extPropagator;   //External propagator
      ////ServiceHandle < IMagFieldAthenaSvc >  m_magFieldAthenaSvc;            //Athena magnetic field----old version
      ServiceHandle<MagField::IMagFieldSvc> m_magFieldAthenaSvc;                //Athena magnetic field
      SimpleProperty<bool>   m_firstMeasuredPoint;
      SimpleProperty<bool>   m_firstMeasuredPointLimit;
      SimpleProperty<bool>   m_makeExtendedVertex;

      bool m_isAtlasField;
      std::once_flag m_isFieldInitialized;

      bool m_useAprioriVertex ;
      bool m_useThetaCnst;
      bool m_usePhiCnst;
      bool m_usePointingCnst;
      bool m_useZPointingCnst;
      bool m_usePassNear;
      bool m_usePassWithTrkErr;
      void initCnstList();

      std::vector<double>    m_ApproximateVertex;
      std::vector<double>    m_partMassCnst;
      std::vector< std::vector<int> >    m_partMassCnstTrk;
      std::vector<int>       m_PosTrack0Charge;


      Amg::Vector3D            m_Vertex;
      TLorentzVector        m_Momentum;
      long int              m_Charge;
      std::vector<double>   m_ErrorMatrix,m_Chi2PerTrk;
      std::vector< std::vector<double> >  m_TrkAtVrt;
      double                m_Chi2;

      const TrackParameters *  m_globalFirstHit;
//-----------------------------------------------------------------
//  Cascade related stuff
//
      int m_cascadeSize;
      std::vector<const xAOD::TrackParticle*> m_partListForCascade;
      std::vector< std::vector<int> >    m_vertexDefinition;             // track indices for vertex;
      std::vector< std::vector<int> >     m_cascadeDefinition;           // cascade structure
      std::vector<double>  m_partMassForCascade;
      std::vector<PartialMassConstraint> m_partMassCnstForCascade;
//
      std::vector<cascadeV>    m_cascadeVList;               // list of cascade vertex objects
      void makeSimpleCascade(std::vector< std::vector<int> > &, std::vector< std::vector<int> > &);
      void printSimpleCascade(std::vector< std::vector<int> > &, std::vector< std::vector<int> > &);
      int findPositions(const std::vector<int> &,const std::vector<int> &, std::vector<int> &);
      int getSimpleVIndex( const VertexID &);
      int indexInV( const VertexID &);
      int getCascadeNDoF() const;
//----------------------
//  Timing measurements
//    
      IChronoStatSvc * m_timingProfile;
//----------------------
//  Control variables
//    
      int m_FitStatus;    /* Fit Status flag*/
                          /* =0 - no fit. All "after fit" routines fail*/
			  /* >1 - good fit. "After fit" routines work*/

      double m_BMAG;       /* const magnetic field  if needed */
      double m_CNVMAG;     /* Conversion constant */
      int m_ifcovv0;
      int m_planeCnstNDOF;  /* NDOF addition if plane constraint is used */
//
// Arrays needed for fitting kernel
//
      double m_par0[NTrMaxVFit][3];   //used only for fit preparation
      double m_apar[NTrMaxVFit][5];   //used only for fit preparation
      double m_awgt[NTrMaxVFit][15];  //used only for fit preparation
      long int m_ich[NTrMaxVFit];
      double m_chi2tr[NTrMaxVFit];
      double m_parfs[NTrMaxVFit][3];
      //double m_wm[NTrMaxVFit];     // obsolete now
      double m_VrtCst[3];
      double m_CovVrtCst[6];

      double m_save_xyzfit[3];  //  Save vertex after successful fit


      VKalAtlasMagFld*       m_fitField;
      VKalExtPropagator*     m_fitPropagator;
      const IExtrapolator*   m_InDetExtrapolator;     //!< Pointer to Extrapolator AlgTool
//
//
//
      VKalVrtControl * m_vkalFitControl;

    
//
//  Origin of global reference frame.
//  (0,0,0) by default but can be changed by input tracks 
//
        double m_refFrameX, m_refFrameY, m_refFrameZ ;
        Amg::Vector3D m_refGVertex;

//
//  Private technical functions
//
        void setInitializedField();
//
//
        int getCnstDOF();
        void FillMatrixP(AmgSymMatrix(5)& , std::vector<double>& );
        void FillMatrixP(int iTrk, AmgSymMatrix(5)& , std::vector<double>& );
        Amg::MatrixX * GiveFullMatrix(int NTrk, std::vector<double>&);
        bool convertAmg5SymMtx(const AmgSymMatrix(5)*, double[] );

        void  VKalTransform( double MAG,double A0V,double ZV,double PhiV,double ThetaV,double  PInv, double[],
                                    long int & Charge, double[], double[]);

  
        StatusCode          CvtTrkTrack(const std::vector<const Track*>& list,                int& ntrk);
        StatusCode     CvtTrackParticle(const std::vector<const TrackParticleBase*>& list,    int& ntrk);
        StatusCode     CvtTrackParticle(const std::vector<const xAOD::TrackParticle*>& list,  int& ntrk);
        StatusCode   CvtNeutralParticle(const std::vector<const xAOD::NeutralParticle*>& list,int& ntrk);
        StatusCode   CvtTrackParameters(const std::vector<const TrackParameters*>& InpTrk,    int& ntrk);
        StatusCode CvtNeutralParameters(const std::vector<const NeutralParameters*>& InpTrk,  int& ntrk);

        void VKalVrtConfigureFitterCore(int NTRK);
        void VKalToTrkTrack( double, double  , double  , double , double& , double& , double& );

        int VKalVrtFit3(  int ntrk, Amg::Vector3D& Vertex, TLorentzVector&   Momentum,
	                   long int& Charge, dvect& ErrorMatrix, dvect& Chi2PerTrk, 
                           std::vector< std::vector<double> >& TrkAtVrt, double& Chi2 );

        Perigee * CreatePerigee(double , double , double ,
     			           const std::vector<double>& VKPerigee,
                                   const std::vector<double>& VKCov);

//
//
        const Perigee* GetPerigee( const TrackParticleBase* i_ntrk); 
        const Perigee* GetPerigee( const TrackParameters*   i_ntrk); 
        const TrackParameters*  GetFirstPoint(const Trk::TrackParticleBase* i_ntrk);
      /*const TrackParameters*  GetFirstPoint(const xAOD::TrackParticle* i_ntrk);  //VK Cannot be implemented. xAOD::TrackParticle
                                                                                   //returns local copy(!!!) of first point  */
//
// Array for full error matrix
//
        double * m_ErrMtx;

//  Track material effects control
// 
       struct TrkMatControl{
          Amg::Vector3D trkRefGlobPos;                // Track reference point(hit) in global ATLAS frame
	  int extrapolationType;
	  int TrkID;
          const TrackParameters* TrkPnt;
	  double prtMass;
          Amg::Vector3D         trkSavedLocalVertex;         // Local VKalVrtCore vertex
       };
       std::vector < TrkMatControl > m_trkControl;
   };

} //end of namespace

#endif
