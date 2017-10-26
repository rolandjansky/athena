/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        TrkV0VertexFitter.cxx  -  Description
 ***************************************************************************/
#include "TrkV0Fitter/TrkV0VertexFitter.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "GaudiKernel/ToolFactory.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
//#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h" 
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h" 

/* These are some local helper classes only needed for convenience, therefore
within anonymous namespace. They contain temporary calculations of matrices
and vectors resulting from the vertex calculation. */
namespace
{
  struct V0FitterTrack
  {
    V0FitterTrack() : originalPerigee(0), chi2(-1.) {}
    virtual ~V0FitterTrack() {}
    const Trk::TrackParameters * originalPerigee;
    double chi2;
    AmgVector(5) TrkPar;
    AmgSymMatrix(5) Wi_mat;
  };
}

namespace Trk
{
  TrkV0VertexFitter::TrkV0VertexFitter(const std::string& t, const std::string& n, const IInterface*  p) : AthAlgTool(t,n,p),
      m_maxIterations(10),
      m_maxDchi2PerNdf(0.1),
      m_maxR(2000.),
      m_maxZ(5000.),
      m_firstMeas(true),
      m_deltaR(false),
      m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
      m_magFieldSvc("AtlasFieldSvc",n)
  {
    declareProperty("MaxIterations",             m_maxIterations);
    declareProperty("MaxChi2PerNdf",             m_maxDchi2PerNdf);
    declareProperty("MaxR",                      m_maxR);
    declareProperty("MaxZ",                      m_maxZ);
    declareProperty("FirstMeasuredPoint",        m_firstMeas);
    declareProperty("Use_deltaR",                m_deltaR);
    declareProperty("Extrapolator",              m_extrapolator);
    declareProperty("MagFieldSvc",               m_magFieldSvc);
    declareInterface<IVertexFitter>(this);
  }

  TrkV0VertexFitter::~TrkV0VertexFitter() {}

  StatusCode TrkV0VertexFitter::initialize()
  {
    if ( m_extrapolator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_extrapolator << endmsg;
    }

  /* Get the magnetic field tool from ToolSvc */
    if ( m_magFieldSvc.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve service " << m_magFieldSvc << endmsg;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved service " << m_magFieldSvc << endmsg;
    }

    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode TrkV0VertexFitter::finalize()
  {
    msg(MSG::INFO) << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }



  /** Interface for Trk::Track with Amg::Vector3D starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::Track*> & vectorTrk,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    return fit(vectorTrk, masses, constraintMass, pointingVertex, firstStartingPoint );
  }

  /** Interface for Trk::Track with xAOD::Vertex starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::Track*> & vectorTrk,
                                        const xAOD::Vertex& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    const Amg::Vector3D startingPoint = firstStartingPoint.position();
    return fit(vectorTrk, masses, constraintMass, pointingVertex, startingPoint );
  }

  /** Interface for Trk::Track with no starting point. (0,0,0) will be assumed */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::Track*>& vectorTrk)
  {
    Amg::Vector3D tmpVtx;
    return fit(vectorTrk, tmpVtx);
  }

  /** Interface for Trk::Track with mass and pointing constraints */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::Track*>& vectorTrk, 
                                        const std::vector<double> masses,
                                        const double& constraintMass,
                                        const xAOD::Vertex* pointingVertex,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    // push_back measured perigees at first measurements into vector<const Trk::ParametersBase*>
    std::vector<const Trk::TrackParameters*> measuredPerigees;
    for (std::vector<const Trk::Track*>::const_iterator trItr = vectorTrk.begin();
        trItr != vectorTrk.end() ; ++trItr)
    {
      const Trk::TrackParameters* firstTrkPar = 0;
      const DataVector<const Trk::TrackStateOnSurface>* tsos = (*trItr)->trackStateOnSurfaces();
      if(!tsos) {
        ATH_MSG_DEBUG("Couldn't find first measurement, using perigee parameters");
        measuredPerigees.push_back((*trItr)->perigeeParameters());
      } else {
        int ntp = 0;
        DataVector<const Trk::TrackStateOnSurface>::const_iterator its,itse = tsos->end();
        for(its=tsos->begin();its!=itse;++its) {
          const Trk::MeasurementBase* mb = (*its)->measurementOnTrack();
          if(!mb) continue;
          const Trk::TrackParameters* trkP = (*its)->trackParameters();
          const Trk::TrackParameters* mTrkP = dynamic_cast<const Trk::TrackParameters*>(trkP);
          if(!mTrkP) continue;
          double loc = trkP->associatedSurface().center().perp();
          if(loc<10.) continue;
          if(ntp==0) firstTrkPar = trkP;
          ntp++;
        }
        measuredPerigees.push_back(firstTrkPar);
      }
    }

    xAOD::Vertex * fittedVxCandidate = fit(measuredPerigees, masses, constraintMass, pointingVertex, firstStartingPoint);
    if ( fittedVxCandidate == 0 ) return fittedVxCandidate;

    // assign the used tracks to the VxCandidate // needs fixing!
    for (unsigned int k = 0 ; k < vectorTrk.size() ; ++k)
    {
      LinkToTrack* link = new LinkToTrack;
      link->setElement(vectorTrk[k]);
      (fittedVxCandidate->vxTrackAtVertex())[k].setOrigTrack(link);
    }

    return fittedVxCandidate;
  }



  /** Interface for Trk::TrackParticleBase with Amg::Vector3D starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    return fit(vectorTrk, masses, constraintMass, pointingVertex, firstStartingPoint );
  }

  /** Interface for Trk::TrackParticleBase with xAOD::Vertex starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                                        const xAOD::Vertex& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    const Amg::Vector3D startingPoint = firstStartingPoint.position();
    return fit(vectorTrk, masses, constraintMass, pointingVertex, startingPoint );
  }

  /** Interface for Trk::TrackParticleBase with no starting point. (0,0,0) will be assumed */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParticleBase*>& vectorTrk)
  {
    Amg::Vector3D tmpVtx;
    return fit(vectorTrk, tmpVtx);
  }

  /** Interface for Trk::TrackParticleBase with mass and pointing constraints */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParticleBase*> & vectorTrk,
                                        const std::vector<double> masses,
                                        const double& constraintMass,
                                        const xAOD::Vertex* pointingVertex,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    // push_back measured perigees at first measurements into vector<const Trk::ParametersBase*>
    std::vector<const Trk::TrackParameters*> measuredPerigees;
    for (std::vector<const Trk::TrackParticleBase*>::const_iterator trItr = vectorTrk.begin();
        trItr != vectorTrk.end() ; ++trItr)
    {
      const Trk::TrackParameters* firstTrkPar = 0; int ntp = 0;
      const std::vector<const Trk::TrackParameters*> vecPar = (*trItr)->trackParameters();
      for(int i=0; i<int(vecPar.size()); ++i) {
        if(vecPar[i]->position().perp()<10.) continue;
        if(ntp==0) firstTrkPar = vecPar[i];
        ntp++;
      }
      measuredPerigees.push_back(firstTrkPar);
      //measuredPerigees.push_back(&((*trItr)->definingParameters()));
    }

    xAOD::Vertex* fittedVxCandidate = fit(measuredPerigees, masses, constraintMass, pointingVertex, firstStartingPoint);
    if ( fittedVxCandidate == 0 ) return fittedVxCandidate;

    // assign the used tracks to the V0Candidate // needs fixing!
    for (unsigned int k = 0 ; k < vectorTrk.size() ; ++k)
    {
      LinkToTrackParticleBase* link = new LinkToTrackParticleBase;
      link->setElement(vectorTrk[k]);
      (fittedVxCandidate->vxTrackAtVertex())[k].setOrigTrack(link);
    }

    return fittedVxCandidate;
  }


  /** Interface for xAOD::TrackParticle with Amg::Vector3D starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    return fit(vectorTrk, masses, constraintMass, pointingVertex, firstStartingPoint);
  }

  /** Interface for xAOD::TrackParticle with xAOD::Vertex starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk,
                                        const xAOD::Vertex& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    const Amg::Vector3D startingPoint = firstStartingPoint.position();
    return fit(vectorTrk, masses, constraintMass, pointingVertex, startingPoint);
  }

  /** Interface for xAOD::TrackParticle with no starting point. (0,0,0) will be assumed */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const xAOD::TrackParticle*>& vectorTrk)
  {
    Amg::Vector3D tmpVtx;
    return fit(vectorTrk, tmpVtx);
  }

  /** Interface for xAOD::TrackParticle with mass and pointing constraints */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const xAOD::TrackParticle*> & vectorTrk,
                                        const std::vector<double> masses,
                                        const double& constraintMass,
                                        const xAOD::Vertex* pointingVertex,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    std::vector<const Trk::TrackParameters*> measuredPerigees;
    std::vector<const Trk::TrackParameters*> measuredPerigees_delete;
    for (const xAOD::TrackParticle* p : vectorTrk)
    {
      if (m_firstMeas) {
        unsigned int indexFMP;
        if (p->indexOfParameterAtPosition(indexFMP, xAOD::FirstMeasurement)) {
          measuredPerigees.push_back(new CurvilinearParameters(p->curvilinearParameters(indexFMP)));
          msg(MSG::DEBUG) << "first measurement on track exists" << endmsg;
          msg(MSG::DEBUG) << "first measurement " << p->curvilinearParameters(indexFMP) << endmsg;
          msg(MSG::DEBUG) << "first measurement covariance " << *(p->curvilinearParameters(indexFMP)).covariance() << endmsg;
        } else {
          Amg::Transform3D * CylTrf = new Amg::Transform3D;
          CylTrf->setIdentity();
          Trk::CylinderSurface estimationCylinder(CylTrf, p->radiusOfFirstHit(), 10e10);
          const Trk::TrackParameters* chargeParameters = &p->perigeeParameters();
          MaterialUpdateMode mode = Trk::removeNoise;
          const Trk::TrackParameters* extrapolatedPerigee(0);
          extrapolatedPerigee = m_extrapolator->extrapolate(*chargeParameters, estimationCylinder, Trk::alongMomentum, true, Trk::pion, mode);
          if (extrapolatedPerigee!=0) {
            msg(MSG::DEBUG) << "extrapolated to first measurement" << endmsg;
            measuredPerigees.push_back (extrapolatedPerigee);
            measuredPerigees_delete.push_back (extrapolatedPerigee);
          } else {
            extrapolatedPerigee = m_extrapolator->extrapolateDirectly(*chargeParameters, estimationCylinder, Trk::alongMomentum, true, Trk::pion);
            if (extrapolatedPerigee!=0) {
              msg(MSG::DEBUG) << "extrapolated (direct) to first measurement" << endmsg;
              measuredPerigees.push_back (extrapolatedPerigee);
              measuredPerigees_delete.push_back (extrapolatedPerigee);
            } else {
              msg(MSG::DEBUG) << "Failed to extrapolate to the first measurement on track, using Perigee parameters" << endmsg;
              measuredPerigees.push_back (&p->perigeeParameters());
            }
          }
        }
      } else {
        measuredPerigees.push_back (&p->perigeeParameters());
      }
    }

    xAOD::Vertex * fittedVxCandidate = fit(measuredPerigees, masses, constraintMass, pointingVertex, firstStartingPoint);
    
    // assign the used tracks to the V0Candidate
    if (fittedVxCandidate) {
      for (const xAOD::TrackParticle* p : vectorTrk)
      {
        ElementLink<xAOD::TrackParticleContainer> el;
        el.setElement(p);
        fittedVxCandidate->addTrackAtVertex (el);
      }
    }

    for (auto ptr : measuredPerigees_delete){ delete ptr; }

    return fittedVxCandidate;
  }
  


  /** Interface for Trk::TrackParameters with Amg::Vector3D starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParameters*> & originalPerigees,
                                        const Amg::Vector3D& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    return fit(originalPerigees, masses, constraintMass, pointingVertex, firstStartingPoint);
  }

  /** Interface for Trk::TrackParameters with xAOD::Vertex starting point */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParameters*> & originalPerigees,
                                        const xAOD::Vertex& firstStartingPoint)
  {
    std::vector<double> masses;
    double constraintMass = -9999.;
    xAOD::Vertex * pointingVertex = 0;
    const Amg::Vector3D startingPoint = firstStartingPoint.position();
    return fit(originalPerigees, masses, constraintMass, pointingVertex, startingPoint);
  }

  /** Interface for Trk::TrackParameters with no starting point. (0,0,0) will be assumed */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParameters*>& originalPerigees)
  {
    Amg::Vector3D tmpVtx;
    return fit(originalPerigees, tmpVtx);
  }

  /** Interface for Trk::TrackParameters with mass and pointing constraints */
  xAOD::Vertex * TrkV0VertexFitter::fit(const std::vector<const Trk::TrackParameters*>& originalPerigees,
                                        const std::vector<double> masses,
                                        const double& constraintMass,
                                        const xAOD::Vertex* pointingVertex,
                                        const Amg::Vector3D& firstStartingPoint)
  {

    if ( originalPerigees.empty() )
    {
      m_error = NOTRKS;
      ATH_MSG_DEBUG("No tracks to fit in this event.");
      return 0;
    }
 
    // Initialisation of variables
    bool pointingConstraint = false;
    bool massConstraint = false;
    if(constraintMass > -100.) massConstraint = true;
    bool conversion = false;
    if(constraintMass == 0. && originalPerigees.size() == 2) conversion = true;
    double x_point=0., y_point=0., z_point=0.;
    AmgSymMatrix(3) pointingVertexCov; pointingVertexCov.setIdentity();
    if (pointingVertex != 0) {
      if (pointingVertex->covariancePosition().trace() != 0.) {
        pointingConstraint = true;
        Amg::Vector3D pv = pointingVertex->position();
        x_point = pv.x();
        y_point = pv.y();
        z_point = pv.z();
        pointingVertexCov = pointingVertex->covariancePosition().inverse();
      }
    }

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "massConstraint " << massConstraint << " pointingConstraint " << pointingConstraint << " conversion " << conversion << endmsg;
      msg(MSG::DEBUG) << "V0Fitter called with: " << endmsg;
      if (massConstraint && masses.size() != 0) msg(MSG::DEBUG) << "mass constraint, V0Mass = " << constraintMass << " particle masses " << masses << endmsg;
      if (pointingConstraint) msg(MSG::DEBUG) << "pointing constraint, x = " << x_point << " y = " << y_point << " z = " << z_point << endmsg;
    }

    m_iter = 0;
    bool restartFit = true;
    double chi2 = 2000000000000.;
    unsigned int nTrk  = originalPerigees.size();   // Number of tracks to fit
    unsigned int nMeas = 5*nTrk;                    // Number of measurements
    unsigned int nVert = 1;                         // Number of vertices

    unsigned int nCnst = 2*nTrk;                    // Number of constraint equations
    unsigned int nPntC = 2;                         // Contribution from pointing constraint in 2D
    unsigned int nMass = 1;                         // Contribution from mass constraint

    if (massConstraint) {
      nCnst = nCnst + nMass;
    }
    if (pointingConstraint) {
      nCnst = nCnst + nPntC;
      nMeas = nMeas + 3;
      nVert = nVert + 1;
    }

    unsigned int nPar  = 5*nTrk + 3*nVert;           // Number of parameters
    int ndf = nMeas - (nPar - nCnst);                // Number of degrees of freedom
    if (ndf < 0) {ndf = 1;}

    unsigned int dim = nCnst;                      //
    unsigned int n_dim = nMeas;                      //

    ATH_MSG_DEBUG("ndf " << ndf << " n_dim " << n_dim << " dim " << dim);

    std::vector<V0FitterTrack> v0FitterTracks;

    Amg::VectorX Y_vec(n_dim); Y_vec.setZero();
    Amg::VectorX Y0_vec(n_dim); Y0_vec.setZero();
    Amg::Vector3D A_vec; A_vec.setZero();

    Amg::MatrixX Wmeas_mat(n_dim,n_dim); Wmeas_mat.setZero();
    Amg::MatrixX Wmeas0_mat(n_dim,n_dim); Wmeas0_mat.setZero();
    Amg::MatrixX Bjac_mat(dim,n_dim); Bjac_mat.setZero();
    Amg::MatrixX Ajac_mat(dim,3); Ajac_mat.setZero();
    Amg::MatrixX C11_mat(n_dim,n_dim); C11_mat.setZero();
    Amg::MatrixX C22_mat(3,3); C22_mat.setZero();
    Amg::MatrixX C21_mat(3,n_dim); C21_mat.setZero();
    Amg::MatrixX C31_mat(dim,n_dim); C31_mat.setZero();
    Amg::MatrixX C32_mat(dim,3); C32_mat.setZero();
    Amg::MatrixX Wb_mat(dim,dim); Wb_mat.setZero();
    Amg::MatrixX Btemp_mat(dim,n_dim); Btemp_mat.setZero();
    Amg::MatrixX Atemp_mat(dim,3); Atemp_mat.setZero();
    Amg::VectorX DeltaY_vec(n_dim); DeltaY_vec.setZero();
    Amg::Vector3D DeltaA_vec; DeltaA_vec.setZero();
    Amg::VectorX DeltaY0_vec(n_dim); DeltaY0_vec.setZero();
    Amg::VectorX F_vec(dim); F_vec.setZero();
    Amg::VectorX C_vec(dim); C_vec.setZero();
    Amg::VectorX C_cor_vec(dim); C_cor_vec.setZero();
    Amg::MatrixX V_mat(nPar,nPar); V_mat.setZero();
    Amg::MatrixX Chi_vec(1,n_dim); Chi_vec.setZero();
    AmgSymMatrix(1) Chi_mat; Chi_mat.setZero();
    Amg::MatrixX ChiItr_vec(1,n_dim); ChiItr_vec.setZero();
    AmgSymMatrix(1) ChiItr_mat; ChiItr_mat.setZero();
    Amg::VectorX F_fac_vec(dim); F_fac_vec.setZero();

    const Amg::Vector3D * globalPosition = &(firstStartingPoint);
    ATH_MSG_DEBUG("globalPosition of starting point: " << (*globalPosition)[0] << ", " << (*globalPosition)[1] << ", " << (*globalPosition)[2]);
    if (globalPosition->perp() > m_maxR && globalPosition->z() > m_maxZ) return 0;

// magnetic field  
    double fieldXYZ[3];  double BField[3];
    fieldXYZ[0] = globalPosition->x();
    fieldXYZ[1] = globalPosition->y();
    fieldXYZ[2] = globalPosition->z();
    m_magFieldSvc->getField(fieldXYZ,BField);
    double B_z = BField[2]*299.792;            // should be in GeV/mm
    if (B_z == 0. || std::isnan(B_z)) {
      ATH_MSG_DEBUG("Could not find a magnetic field different from zero: very very strange");
      B_z = 0.60407;                            // Value in GeV/mm (ATLAS units)
    } else {
      ATH_MSG_VERBOSE("Magnetic field projection of z axis in the perigee position is: " << B_z << " GeV/mm ");
    }
//    double B_z = 1.998*0.3;


    v0FitterTracks.clear();
    Trk::PerigeeSurface perigeeSurface(*globalPosition);
    // Extrapolate the perigees to the startpoint of the fit
    std::vector<const Trk::TrackParameters*>::const_iterator iter(originalPerigees.begin());
    for (; iter != originalPerigees.end() ; ++iter)
    {
      const Trk::TrackParameters* chargeParameters(0);
      chargeParameters = dynamic_cast<const Trk::TrackParameters*> (*iter);
      if (chargeParameters != 0)
      {
        // Correct material changes
        const Amg::Vector3D gMomentum  = chargeParameters->momentum();
        const Amg::Vector3D gDirection = chargeParameters->position() - *globalPosition;
        const double extrapolationDirection = gMomentum.dot( gDirection );
        MaterialUpdateMode mode = Trk::removeNoise;
        if(extrapolationDirection > 0) mode = Trk::addNoise;
        const Trk::Perigee* extrapolatedPerigee(0);
        extrapolatedPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*chargeParameters, perigeeSurface, Trk::anyDirection, true, Trk::pion, mode));
        if (extrapolatedPerigee==0)
        {
          ATH_MSG_DEBUG("Perigee was not extrapolated! Taking original one!");
          const Trk::Perigee* tmpPerigee = dynamic_cast<const Trk::Perigee*>(chargeParameters);
          if (tmpPerigee!=0) extrapolatedPerigee = new Trk::Perigee(*tmpPerigee);
          else return 0;
        }

        // store track parameters at starting point
        V0FitterTrack locV0FitterTrack;
        locV0FitterTrack.TrkPar[0] = extrapolatedPerigee->parameters()[Trk::d0];
        locV0FitterTrack.TrkPar[1] = extrapolatedPerigee->parameters()[Trk::z0];
        locV0FitterTrack.TrkPar[2] = extrapolatedPerigee->parameters()[Trk::phi];
        locV0FitterTrack.TrkPar[3] = extrapolatedPerigee->parameters()[Trk::theta];
        locV0FitterTrack.TrkPar[4] = extrapolatedPerigee->parameters()[Trk::qOverP];
        locV0FitterTrack.Wi_mat = extrapolatedPerigee->covariance()->inverse().eval();
        locV0FitterTrack.originalPerigee = *iter;
        v0FitterTracks.push_back(locV0FitterTrack);
        delete extrapolatedPerigee;
      } else {
        ATH_MSG_DEBUG("Track parameters are not charged tracks ... fit aborted");
        return 0;
      }
    }

    // Iterate fits until the fit criteria are met, or the number of max iterations is reached
    double chi2New=0.; double chi2Old=chi2;
    double sumConstr=0.;
    bool onConstr = false;
    Amg::Vector3D frameOrigin = firstStartingPoint;
    Amg::Vector3D frameOriginItr = firstStartingPoint;
    for (m_iter=0; m_iter < m_maxIterations; ++m_iter)
    {
      ATH_MSG_DEBUG("Iteration number: " << m_iter);
      if (!restartFit) chi2Old = chi2New;
      chi2New = 0.;

      if (restartFit)
      {
        // ===> loop over tracks
        std::vector<V0FitterTrack>::iterator PTIter;
        int i=0;
        for (PTIter = v0FitterTracks.begin(); PTIter != v0FitterTracks.end() ; ++PTIter)
        {
          V0FitterTrack locP((*PTIter));
          Wmeas0_mat.block(5*i,5*i,5,5) = locP.Wi_mat;
          Wmeas_mat.block(5*i,5*i,5,5) = locP.Wi_mat;
          for (int j=0; j<5; ++j) {
            Y0_vec(j+5*i)  = locP.TrkPar[j];
          }
          ++i;
        }
        if(pointingConstraint) {
          Y0_vec(5*nTrk + 0) = x_point;
          Y0_vec(5*nTrk + 1) = y_point;
          Y0_vec(5*nTrk + 2) = z_point;
          Wmeas0_mat.block(5*nTrk,5*nTrk,3,3) = pointingVertexCov;
          Wmeas_mat.block(5*nTrk,5*nTrk,3,3) = pointingVertexCov;
        }
        Wmeas_mat = Wmeas_mat.inverse();
      }

      Y_vec = Y0_vec + DeltaY_vec;
      A_vec = DeltaA_vec;

      // check theta and phi ranges
      for (unsigned int i=0; i<nTrk; ++i) 
      {
        if ( fabs ( Y_vec(2+5*i) ) > 100. || fabs ( Y_vec(3+5*i) ) > 100. ) { return 0; }
        while ( fabs ( Y_vec(2+5*i) ) > M_PI ) Y_vec(2+5*i) += ( Y_vec(2+5*i) > 0 ) ? -2*M_PI : 2*M_PI;
        while ( Y_vec(3+5*i) > 2*M_PI ) Y_vec(3+5*i) -= 2*M_PI;
        while ( Y_vec(3+5*i) < -M_PI ) Y_vec(3+5*i) += M_PI;
        if ( Y_vec(3+5*i) > M_PI )
        {
          Y_vec(3+5*i) = 2*M_PI - Y_vec(3+5*i);
          if ( Y_vec(2+5*i) >= 0 )   Y_vec(2+5*i)  += ( Y_vec(2+5*i) >0 ) ? -M_PI : M_PI;
        }
        if ( Y_vec(3+5*i) < 0.0 )
        {
          Y_vec(3+5*i)  = - Y_vec(3+5*i);
          if ( Y_vec(2+5*i) >= 0 )  Y_vec(2+5*i) += ( Y_vec(2+5*i) >0 ) ? -M_PI : M_PI;
        }
      }

      double SigE=0., SigPx=0., SigPy=0., SigPz=0., Px=0., Py=0., Pz=0.;
      Amg::VectorX rho(nTrk), Phi(nTrk), charge(nTrk);
        rho.setZero(); Phi.setZero(); charge.setZero();
      Amg::VectorX d0Cor(nTrk), d0Fac(nTrk), xcphiplusysphi(nTrk), xsphiminusycphi(nTrk);
        d0Cor.setZero();  d0Fac.setZero(); xcphiplusysphi.setZero(); xsphiminusycphi.setZero();
      AmgVector(2) conv_sign;
        conv_sign[0] = -1; conv_sign[1] = 1;
      for (unsigned int i=0; i<nTrk; ++i) 
      {
        charge[i] = (Y_vec(4+5*i) < 0.) ? -1. : 1.;
        rho[i] = sin(Y_vec(3+5*i))/(B_z*Y_vec(4+5*i));
        xcphiplusysphi[i]  = A_vec(0)*cos(Y_vec(2+5*i))+A_vec(1)*sin(Y_vec(2+5*i));
        xsphiminusycphi[i] = A_vec(0)*sin(Y_vec(2+5*i))-A_vec(1)*cos(Y_vec(2+5*i));
        if(fabs(-xcphiplusysphi[i]/rho[i]) > 1.) return 0;
        d0Cor[i] = 0.5*asin(-xcphiplusysphi[i]/rho[i]);
        double d0Facsq = 1. - xcphiplusysphi[i]*xcphiplusysphi[i]/(rho[i]*rho[i]);
        d0Fac[i] = (d0Facsq>0.) ? sqrt(d0Facsq) : 0;
        Phi[i] = Y_vec(2+5*i) + 2.*d0Cor[i];

        if(massConstraint && masses.size() != 0 && masses[i] != 0.){
          SigE  += sqrt(1./(Y_vec(4+5*i)*Y_vec(4+5*i)) + masses[i]*masses[i]);
          SigPx += sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          SigPy += sin(Y_vec(3+5*i))*sin(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          SigPz += cos(Y_vec(3+5*i))*charge[i]/Y_vec(4+5*i);
        }
        Px += sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
        Py += sin(Y_vec(3+5*i))*sin(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
        Pz += cos(Y_vec(3+5*i))*charge[i]/Y_vec(4+5*i);
      }

      double FMass=0., dFMassdxs=0., dFMassdys=0., dFMassdzs=0.;
      double FPxy=0., dFPxydxs=0., dFPxydys=0., dFPxydzs=0., dFPxydxp=0., dFPxydyp=0., dFPxydzp=0.;
      double FPxz=0., dFPxzdxs=0., dFPxzdys=0., dFPxzdzs=0., dFPxzdxp=0., dFPxzdyp=0., dFPxzdzp=0.;
      Amg::VectorX Fxy(nTrk), Fxz(nTrk), dFMassdPhi(nTrk);
        Fxy.setZero(); Fxz.setZero(); dFMassdPhi.setZero();
      Amg::VectorX drhodtheta(nTrk), drhodqOverP(nTrk), csplusbc(nTrk), ccminusbs(nTrk);
        drhodtheta.setZero(); drhodqOverP.setZero(); csplusbc.setZero(); ccminusbs.setZero();
      Amg::VectorX dFxydd0(nTrk), dFxydz0(nTrk), dFxydphi(nTrk), dFxydtheta(nTrk), dFxydqOverP(nTrk);
        dFxydd0.setZero(); dFxydz0.setZero(); dFxydphi.setZero(); dFxydtheta.setZero(); dFxydqOverP.setZero();
      Amg::VectorX dFxydxs(nTrk), dFxydys(nTrk), dFxydzs(nTrk);
        dFxydxs.setZero(); dFxydys.setZero(); dFxydzs.setZero();
      Amg::VectorX dFxzdd0(nTrk), dFxzdz0(nTrk), dFxzdphi(nTrk), dFxzdtheta(nTrk), dFxzdqOverP(nTrk);
        dFxzdd0.setZero(); dFxzdz0.setZero(); dFxzdphi.setZero(); dFxzdtheta.setZero(); dFxzdqOverP.setZero();
      Amg::VectorX dFxzdxs(nTrk), dFxzdys(nTrk), dFxzdzs(nTrk);
        dFxzdxs.setZero(); dFxzdys.setZero(); dFxzdzs.setZero();
      Amg::VectorX dFMassdd0(nTrk), dFMassdz0(nTrk), dFMassdphi(nTrk), dFMassdtheta(nTrk), dFMassdqOverP(nTrk);
        dFMassdd0.setZero(); dFMassdz0.setZero(); dFMassdphi.setZero(); dFMassdtheta.setZero(); dFMassdqOverP.setZero();
      Amg::VectorX dFPxydd0(nTrk), dFPxydz0(nTrk), dFPxydphi(nTrk), dFPxydtheta(nTrk), dFPxydqOverP(nTrk);
        dFPxydd0.setZero(); dFPxydz0.setZero(); dFPxydphi.setZero(); dFPxydtheta.setZero(); dFPxydqOverP.setZero();
      Amg::VectorX dFPxzdd0(nTrk), dFPxzdz0(nTrk), dFPxzdphi(nTrk), dFPxzdtheta(nTrk), dFPxzdqOverP(nTrk);
        dFPxzdd0.setZero(); dFPxzdz0.setZero(); dFPxzdphi.setZero(); dFPxzdtheta.setZero(); dFPxzdqOverP.setZero();
      Amg::VectorX dPhidd0(nTrk), dPhidz0(nTrk), dPhidphi0(nTrk), dPhidtheta(nTrk), dPhidqOverP(nTrk);
        dPhidd0.setZero(); dPhidz0.setZero(); dPhidphi0.setZero(); dPhidtheta.setZero(); dPhidqOverP.setZero();
      Amg::VectorX dPhidxs(nTrk), dPhidys(nTrk), dPhidzs(nTrk);
        dPhidxs.setZero(); dPhidys.setZero(); dPhidzs.setZero();
      //
      // constraint equations for V0vertex fitter
      //
      // FMass = mass vertex constraint
      //
      if (conversion) {
        FMass = Phi[1] - Phi[0];
      } else {
        FMass = constraintMass*constraintMass - SigE*SigE + SigPx*SigPx + SigPy*SigPy + SigPz*SigPz;
      }
      //
      // FPxy = pointing constraint in xy
      //
      FPxy  = Px*(frameOriginItr[1] - y_point) - Py*(frameOriginItr[0]- x_point);
      //
      // FPxz = pointing constraint in xz
      //
      FPxz  = Px*(frameOriginItr[2] - z_point) - Pz*(frameOriginItr[0]- x_point);

      for (unsigned int i=0; i<nTrk; ++i) 
      {
      //
      // Fxy = vertex constraint in xy plane (one for each track)
      //
        Fxy[i] = Y_vec(0+5*i) + xsphiminusycphi[i] - 2.*rho[i]*sin(d0Cor[i])*sin(d0Cor[i]);
      //
      // Fxz = vertex constraint in xz plane (one for each track)
      //
	Fxz[i] = Y_vec(1+5*i) - A_vec(2) - rho[i]*2.*d0Cor[i]/tan(Y_vec(3+5*i));
      //
      // derivatives
      //
        drhodtheta[i]  =  cos(Y_vec(3+5*i))/(B_z*Y_vec(4+5*i));
        drhodqOverP[i] = -sin(Y_vec(3+5*i))/(B_z*Y_vec(4+5*i)*Y_vec(4+5*i));

        dFxydd0[i]     =  1.;
        dFxydphi[i]    =  xcphiplusysphi[i]*(1. + xsphiminusycphi[i]/(d0Fac[i]*rho[i]));
        dFxydtheta[i]  =  (xcphiplusysphi[i]*xcphiplusysphi[i]/(d0Fac[i]*rho[i]*rho[i])-2.*sin(d0Cor[i])*sin(d0Cor[i]))*drhodtheta[i];
        dFxydqOverP[i] =  (xcphiplusysphi[i]*xcphiplusysphi[i]/(d0Fac[i]*rho[i]*rho[i])-2.*sin(d0Cor[i])*sin(d0Cor[i]))*drhodqOverP[i];
        dFxydxs[i]     =  sin(Y_vec(2+5*i)) - cos(Y_vec(2+5*i))*xcphiplusysphi[i]/(d0Fac[i]*rho[i]);
        dFxydys[i]     = -cos(Y_vec(2+5*i)) - sin(Y_vec(2+5*i))*xcphiplusysphi[i]/(d0Fac[i]*rho[i]);

        dFxzdz0[i]     =  1.;
        dFxzdphi[i]    = -xsphiminusycphi[i]/(d0Fac[i]*tan(Y_vec(3+5*i)));
        dFxzdtheta[i]  = -((xcphiplusysphi[i]/(d0Fac[i]*rho[i]) + 2.*d0Cor[i])*tan(Y_vec(3+5*i))*drhodtheta[i] -
                                       rho[i]*2.*d0Cor[i]/(cos(Y_vec(3+5*i))*cos(Y_vec(3+5*i))))/(tan(Y_vec(3+5*i))*tan(Y_vec(3+5*i)));
        dFxzdqOverP[i] = -(xcphiplusysphi[i]/(d0Fac[i]*rho[i]) + 2.*d0Cor[i])*drhodqOverP[i]/tan(Y_vec(3+5*i));
        dFxzdxs[i]     =  cos(Y_vec(2+5*i))/(d0Fac[i]*tan(Y_vec(3+5*i)));
        dFxzdys[i]     =  sin(Y_vec(2+5*i))/(d0Fac[i]*tan(Y_vec(3+5*i)));
        dFxzdzs[i]     = -1.;

        dPhidphi0[i]   = 1. + xsphiminusycphi[i]/(d0Fac[i]*rho[i]);
        dPhidtheta[i]  =  xcphiplusysphi[i]*drhodtheta[i]/(d0Fac[i]*rho[i]*rho[i]);
        dPhidqOverP[i] =  xcphiplusysphi[i]*drhodqOverP[i]/(d0Fac[i]*rho[i]*rho[i]);
        dPhidxs[i]     = -cos(Y_vec(2+5*i))/(d0Fac[i]*rho[i]);
        dPhidys[i]     = -sin(Y_vec(2+5*i))/(d0Fac[i]*rho[i]);

        if (massConstraint && masses.size() != 0 && masses[i] != 0.){
          if (conversion) {
            dFMassdphi[i]    = conv_sign[i]*dPhidphi0[i];
            dFMassdtheta[i]  = conv_sign[i]*dPhidtheta[i];
            dFMassdqOverP[i] = conv_sign[i]*dPhidqOverP[i];
            dFMassdxs       += conv_sign[i]*dPhidxs[i];
            dFMassdys       += conv_sign[i]*dPhidys[i];
          } else {
            csplusbc[i]      = SigPy*sin(Y_vec(2+5*i))+SigPx*cos(Y_vec(2+5*i));
            ccminusbs[i]     = SigPy*cos(Y_vec(2+5*i))-SigPx*sin(Y_vec(2+5*i));
            dFMassdphi[i]    = 2.*sin(Y_vec(3+5*i))*ccminusbs[i]*charge[i]/Y_vec(4+5*i);
            dFMassdtheta[i]  = 2.*(cos(Y_vec(3+5*i))*csplusbc[i] - sin(Y_vec(3+5*i))*SigPz)*charge[i]/Y_vec(4+5*i);
            dFMassdqOverP[i] = 2.*SigE/(sqrt(1./(Y_vec(4+5*i)*Y_vec(4+5*i)) + masses[i]*masses[i])*Y_vec(4+5*i)*Y_vec(4+5*i)*Y_vec(4+5*i)) -
                               2.*charge[i]*(sin(Y_vec(3+5*i))*csplusbc[i] + cos(Y_vec(3+5*i))*SigPz)/(Y_vec(4+5*i)*Y_vec(4+5*i));
          }
        }

        if (pointingConstraint){
          dFPxydphi[i]    = -sin(Y_vec(3+5*i))*(sin(Y_vec(2+5*i))*(frameOriginItr[1]-y_point)+cos(Y_vec(2+5*i))*(frameOriginItr[0]-x_point))*charge[i]/Y_vec(4+5*i);
          dFPxydtheta[i]  =  cos(Y_vec(3+5*i))*(cos(Y_vec(2+5*i))*(frameOriginItr[1]-y_point)-sin(Y_vec(2+5*i))*(frameOriginItr[0]-x_point))*charge[i]/Y_vec(4+5*i);
          dFPxydqOverP[i] = -sin(Y_vec(3+5*i))*(cos(Y_vec(2+5*i))*(frameOriginItr[1]-y_point)-sin(Y_vec(2+5*i))*(frameOriginItr[0]-x_point))*charge[i]/(Y_vec(4+5*i)*Y_vec(4+5*i));
          dFPxydxs       += -sin(Y_vec(3+5*i))*sin(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxydys       +=  sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxydxp       +=  sin(Y_vec(3+5*i))*sin(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxydyp       += -sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);

          dFPxzdphi[i]    = -sin(Y_vec(3+5*i))*sin(Y_vec(2+5*i))*(frameOriginItr[2]-z_point)*charge[i]/Y_vec(4+5*i);
          dFPxzdtheta[i]  =  cos(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*(frameOriginItr[2]-z_point)*charge[i]/Y_vec(4+5*i)
                            +sin(Y_vec(3+5*i))*(frameOriginItr[0]-x_point)*charge[i]/Y_vec(4+5*i);
          dFPxzdqOverP[i] = -sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*(frameOriginItr[2]-z_point)*charge[i]/(Y_vec(4+5*i)*Y_vec(4+5*i))
                            +cos(Y_vec(3+5*i))*(frameOriginItr[0]-x_point)*charge[i]/(Y_vec(4+5*i)*Y_vec(4+5*i));
          dFPxzdxs       += -cos(Y_vec(3+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxzdzs       +=  sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxzdxp       +=  cos(Y_vec(3+5*i))*charge[i]/Y_vec(4+5*i);
          dFPxzdzp       += -sin(Y_vec(3+5*i))*cos(Y_vec(2+5*i))*charge[i]/Y_vec(4+5*i);
        }

        // fill vector of constraints
        F_vec[i]      = -Fxy[i];
        F_vec[i+nTrk] = -Fxz[i];
        F_fac_vec[i]      = 1.;
        F_fac_vec[i+nTrk] = 1.;
      }
      if(massConstraint) F_vec(2*nTrk+0) = -FMass;
      //if(massConstraint) F_fac_vec(2*nTrk+0) = 1.;
      if(massConstraint) F_fac_vec(2*nTrk+0) = 0.000001;
      if(pointingConstraint) {
        if(massConstraint) {
          F_vec(2*nTrk+1) = -FPxy;
          F_vec(2*nTrk+2) = -FPxz;
          F_fac_vec(2*nTrk+1) = 0.000001;
          F_fac_vec(2*nTrk+2) = 0.000001;
        } else {
          F_vec(2*nTrk+0) = -FPxy;
          F_vec(2*nTrk+1) = -FPxz;
          F_fac_vec(2*nTrk+0) = 0.000001;
          F_fac_vec(2*nTrk+1) = 0.000001;
        }
      }

      sumConstr = 0.;
      for (unsigned int i=0; i<dim; ++i) 
      {
        sumConstr += F_fac_vec[i]*fabs(F_vec[i]);
      }
      if ( std::isnan(sumConstr) ) { return 0; }
      if (sumConstr < 0.001) { onConstr = true; }
      ATH_MSG_DEBUG("sumConstr " << sumConstr);

      for (unsigned int i=0; i<nTrk; ++i) 
      {
        Bjac_mat(i,0+5*i)        = dFxydd0(i);
        Bjac_mat(i,1+5*i)        = dFxydz0(i);
        Bjac_mat(i,2+5*i)        = dFxydphi(i);
        Bjac_mat(i,3+5*i)        = dFxydtheta(i);
        Bjac_mat(i,4+5*i)        = dFxydqOverP(i);
        Bjac_mat(i+nTrk,0+5*i)   = dFxzdd0(i);
        Bjac_mat(i+nTrk,1+5*i)   = dFxzdz0(i);
        Bjac_mat(i+nTrk,2+5*i)   = dFxzdphi(i);
        Bjac_mat(i+nTrk,3+5*i)   = dFxzdtheta(i);
        Bjac_mat(i+nTrk,4+5*i)   = dFxzdqOverP(i);
        if(massConstraint) {
          Bjac_mat(2*nTrk,0+5*i) = dFMassdd0(i);
          Bjac_mat(2*nTrk,1+5*i) = dFMassdz0(i);
          Bjac_mat(2*nTrk,2+5*i) = dFMassdphi(i);
          Bjac_mat(2*nTrk,3+5*i) = dFMassdtheta(i);
          Bjac_mat(2*nTrk,4+5*i) = dFMassdqOverP(i);
        }
        if(pointingConstraint) {
          if(massConstraint) {
            Bjac_mat(2*nTrk+1,0+5*i)    = dFPxydd0(i);
            Bjac_mat(2*nTrk+1,1+5*i)    = dFPxydz0(i);
            Bjac_mat(2*nTrk+1,2+5*i)    = dFPxydphi(i);
            Bjac_mat(2*nTrk+1,3+5*i)    = dFPxydtheta(i);
            Bjac_mat(2*nTrk+1,4+5*i)    = dFPxydqOverP(i);
            Bjac_mat(2*nTrk+1,5*nTrk)   = dFPxydxp;
            Bjac_mat(2*nTrk+1,5*nTrk+1) = dFPxydyp;
            Bjac_mat(2*nTrk+1,5*nTrk+2) = dFPxydzp;
            Bjac_mat(2*nTrk+2,0+5*i)    = dFPxzdd0(i);
            Bjac_mat(2*nTrk+2,1+5*i)    = dFPxzdz0(i);
            Bjac_mat(2*nTrk+2,2+5*i)    = dFPxzdphi(i);
            Bjac_mat(2*nTrk+2,3+5*i)    = dFPxzdtheta(i);
            Bjac_mat(2*nTrk+2,4+5*i)    = dFPxzdqOverP(i);
            Bjac_mat(2*nTrk+2,5*nTrk)   = dFPxzdxp;
            Bjac_mat(2*nTrk+2,5*nTrk+1) = dFPxzdyp;
            Bjac_mat(2*nTrk+2,5*nTrk+2) = dFPxzdzp;
          } else {
            Bjac_mat(2*nTrk+0,0+5*i)    = dFPxydd0(i);
            Bjac_mat(2*nTrk+0,1+5*i)    = dFPxydz0(i);
            Bjac_mat(2*nTrk+0,2+5*i)    = dFPxydphi(i);
            Bjac_mat(2*nTrk+0,3+5*i)    = dFPxydtheta(i);
            Bjac_mat(2*nTrk+0,4+5*i)    = dFPxydqOverP(i);
            Bjac_mat(2*nTrk+0,5*nTrk)   = dFPxydxp;
            Bjac_mat(2*nTrk+0,5*nTrk+1) = dFPxydyp;
            Bjac_mat(2*nTrk+0,5*nTrk+2) = dFPxydzp;
            Bjac_mat(2*nTrk+1,0+5*i)    = dFPxzdd0(i);
            Bjac_mat(2*nTrk+1,1+5*i)    = dFPxzdz0(i);
            Bjac_mat(2*nTrk+1,2+5*i)    = dFPxzdphi(i);
            Bjac_mat(2*nTrk+1,3+5*i)    = dFPxzdtheta(i);
            Bjac_mat(2*nTrk+1,4+5*i)    = dFPxzdqOverP(i);
            Bjac_mat(2*nTrk+1,5*nTrk)   = dFPxzdxp;
            Bjac_mat(2*nTrk+1,5*nTrk+1) = dFPxzdyp;
            Bjac_mat(2*nTrk+1,5*nTrk+2) = dFPxzdzp;
          }
        }

        Ajac_mat(i,0) = dFxydxs(i);
        Ajac_mat(i,1) = dFxydys(i);
        Ajac_mat(i,2) = dFxydzs(i);
        Ajac_mat(i+nTrk,0) = dFxzdxs(i);
        Ajac_mat(i+nTrk,1) = dFxzdys(i);
        Ajac_mat(i+nTrk,2) = dFxzdzs(i);
        if(massConstraint) {
          Ajac_mat(2*nTrk,0) = dFMassdxs;
          Ajac_mat(2*nTrk,1) = dFMassdys;
          Ajac_mat(2*nTrk,2) = dFMassdzs;
        }
        if(pointingConstraint) {
          if(massConstraint) {
            Ajac_mat(2*nTrk+1,0) = dFPxydxs;
            Ajac_mat(2*nTrk+1,1) = dFPxydys;
            Ajac_mat(2*nTrk+1,2) = dFPxydzs;
            Ajac_mat(2*nTrk+2,0) = dFPxzdxs;
            Ajac_mat(2*nTrk+2,1) = dFPxzdys;
            Ajac_mat(2*nTrk+2,2) = dFPxzdzs;
          } else {
            Ajac_mat(2*nTrk+0,0) = dFPxydxs;
            Ajac_mat(2*nTrk+0,1) = dFPxydys;
            Ajac_mat(2*nTrk+0,2) = dFPxydzs;
            Ajac_mat(2*nTrk+1,0) = dFPxzdxs;
            Ajac_mat(2*nTrk+1,1) = dFPxzdys;
            Ajac_mat(2*nTrk+1,2) = dFPxzdzs;
          }
        }
      }

      Wb_mat = Wmeas_mat.similarity(Bjac_mat) ;
      Wb_mat = Wb_mat.inverse();

      C22_mat = Wb_mat.similarity(Ajac_mat.transpose());
      C22_mat = C22_mat.inverse();

      Btemp_mat = Wb_mat * Bjac_mat * Wmeas_mat;
      Atemp_mat = Wb_mat * Ajac_mat;

      C21_mat = - C22_mat * Ajac_mat.transpose() * Btemp_mat;
      C32_mat =   Atemp_mat * C22_mat;
      C31_mat =   Btemp_mat + Atemp_mat * C21_mat;
      Amg::MatrixX mat_prod_1 = Wmeas_mat * Bjac_mat.transpose();
      Amg::MatrixX mat_prod_2 = Wmeas_mat * Bjac_mat.transpose() * Wb_mat * Ajac_mat; 
      C11_mat =   Wmeas_mat - Wb_mat.similarity( mat_prod_1 ) + C22_mat.similarity( mat_prod_2 );	

      C_cor_vec = Ajac_mat*DeltaA_vec + Bjac_mat*DeltaY_vec;
      C_vec = C_cor_vec + F_vec;

      DeltaY_vec = C31_mat.transpose()*C_vec;
      DeltaA_vec = C32_mat.transpose()*C_vec;

      for (unsigned int i=0; i<n_dim; ++i) 
      {
        ChiItr_vec(0,i) = DeltaY_vec(i);
      }
      ChiItr_mat = Wmeas0_mat.similarity( ChiItr_vec );
      chi2New = ChiItr_mat(0,0);

      // current vertex position in global coordinates
      frameOriginItr[0] += DeltaA_vec(0);
      frameOriginItr[1] += DeltaA_vec(1);
      frameOriginItr[2] += DeltaA_vec(2);
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "New vertex, global coordinates: " << frameOriginItr.transpose() << endmsg;
        msg(MSG::DEBUG) << "chi2Old: " << chi2Old << " chi2New: " << chi2New << " fabs(chi2Old-chi2New): " << fabs(chi2Old-chi2New) << endmsg;
      }

      const Amg::Vector3D * globalPositionItr = &frameOriginItr;
      if (globalPositionItr->perp() > m_maxR && globalPositionItr->z() > m_maxZ) return 0;

      if (onConstr && fabs(chi2Old-chi2New) < 0.1) { break; }

      double fieldXYZItr[3];  double BFieldItr[3];
      fieldXYZItr[0] = globalPositionItr->x();
      fieldXYZItr[1] = globalPositionItr->y();
      fieldXYZItr[2] = globalPositionItr->z();
      m_magFieldSvc->getField(fieldXYZItr,BFieldItr);
      double B_z_new = BFieldItr[2]*299.792;            // should be in GeV/mm
      if (B_z_new == 0. || std::isnan(B_z)) {
        ATH_MSG_DEBUG("Using old B_z");
        B_z_new = B_z;
      }

      restartFit = false;
      double deltaR = sqrt(DeltaA_vec(0)*DeltaA_vec(0)+DeltaA_vec(1)*DeltaA_vec(1)+DeltaA_vec(2)*DeltaA_vec(2));
      double deltaB_z = fabs(B_z-B_z_new)/B_z;
      bool changeBz = false;

      if (m_deltaR) {
        if (deltaR > 5. && m_iter < m_maxIterations-1) changeBz = true;
      } else {
        if (deltaB_z > 0.000001 && m_iter < m_maxIterations-1) changeBz = true;
      }

      if (changeBz) {
        B_z = B_z_new;

        v0FitterTracks.clear();
        Trk::PerigeeSurface perigeeSurfaceItr(*globalPositionItr);
        // Extrapolate the perigees to the new startpoint of the fit
        std::vector<const Trk::TrackParameters*>::const_iterator iter(originalPerigees.begin());
        for (; iter != originalPerigees.end() ; ++iter)
        {
          const Trk::TrackParameters* chargeParameters(0);
          chargeParameters = dynamic_cast<const Trk::TrackParameters*> (*iter);
          if (chargeParameters != 0)
          {
            // Correct material changes
            const Amg::Vector3D gMomentum  = chargeParameters->momentum();
            const Amg::Vector3D gDirection = chargeParameters->position() - *globalPositionItr;
            const double extrapolationDirection = gMomentum .dot( gDirection );
            MaterialUpdateMode mode = Trk::removeNoise;
            if(extrapolationDirection > 0) mode = Trk::addNoise;
            const Trk::Perigee* extrapolatedPerigee(0);
            extrapolatedPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolate(*chargeParameters, perigeeSurfaceItr, Trk::anyDirection, true, Trk::pion, mode));
            if (extrapolatedPerigee==0)
            {
              ATH_MSG_DEBUG("Perigee was not extrapolated! Taking original one!");
              const Trk::Perigee* tmpPerigee = dynamic_cast<const Trk::Perigee*>(chargeParameters);
              if (tmpPerigee!=0) extrapolatedPerigee = new Trk::Perigee(*tmpPerigee);
              else return 0;
            }

            // store track parameters at new starting point
            V0FitterTrack locV0FitterTrack;
            locV0FitterTrack.TrkPar[0] = extrapolatedPerigee->parameters()[Trk::d0];
            locV0FitterTrack.TrkPar[1] = extrapolatedPerigee->parameters()[Trk::z0];
            locV0FitterTrack.TrkPar[2] = extrapolatedPerigee->parameters()[Trk::phi];
            locV0FitterTrack.TrkPar[3] = extrapolatedPerigee->parameters()[Trk::theta];
            locV0FitterTrack.TrkPar[4] = extrapolatedPerigee->parameters()[Trk::qOverP];
            locV0FitterTrack.Wi_mat = extrapolatedPerigee->covariance()->inverse().eval();
            locV0FitterTrack.originalPerigee = *iter;
            v0FitterTracks.push_back(locV0FitterTrack);
            delete extrapolatedPerigee;
          } else {
            ATH_MSG_DEBUG("Track parameters are not charged tracks ... fit aborted");
            return 0;
          }
        }
        frameOrigin = frameOriginItr;
        Y0_vec *= 0.;
        Y_vec *= 0.;
        A_vec *= 0.;
        DeltaY_vec *= 0.;
        DeltaA_vec *= 0.;
        chi2Old = 2000000000000.;
        chi2New = 0.;
        sumConstr = 0.;
        onConstr = false;
        restartFit = true;
      }

      //if (onConstr && fabs(chi2Old-chi2New) < 0.1) { break; }

    } // end of iteration

    frameOrigin[0] += DeltaA_vec(0);
    frameOrigin[1] += DeltaA_vec(1);
    frameOrigin[2] += DeltaA_vec(2);
    if ( std::isnan(frameOrigin[0]) || std::isnan(frameOrigin[1]) || std::isnan(frameOrigin[2]) ) return 0;

    Y_vec = Y0_vec + DeltaY_vec;

    // check theta and phi ranges
    for (unsigned int i=0; i<nTrk; ++i) 
    {
      if ( fabs ( Y_vec(2+5*i) ) > 100. || fabs ( Y_vec(3+5*i) ) > 100. ) { return 0; }
      while ( fabs ( Y_vec(2+5*i) ) > M_PI ) Y_vec(2+5*i) += ( Y_vec(2+5*i) > 0 ) ? -2*M_PI : 2*M_PI;
      while ( Y_vec(3+5*i) > 2*M_PI ) Y_vec(3+5*i) -= 2*M_PI;
      while ( Y_vec(3+5*i) < -M_PI ) Y_vec(3+5*i) += M_PI;
      if ( Y_vec(3+5*i) > M_PI )
      {
        Y_vec(3+5*i) = 2*M_PI - Y_vec(3+5*i);
        if ( Y_vec(2+5*i) >= 0 )   Y_vec(2+5*i)  += ( Y_vec(2+5*i) >0 ) ? -M_PI : M_PI;
      }
      if ( Y_vec(3+5*i) < 0.0 )
      {
        Y_vec(3+5*i)  = - Y_vec(3+5*i);
        if ( Y_vec(2+5*i) >= 0 )  Y_vec(2+5*i) += ( Y_vec(2+5*i) >0 ) ? -M_PI : M_PI;
      }
    }

    for (unsigned int i=0; i<n_dim; ++i) 
    {
      Chi_vec(0,i) = DeltaY_vec(i);
    }
    Chi_mat = Wmeas0_mat.similarity( Chi_vec );
    chi2 = Chi_mat(0,0);

    V_mat.setZero();
    V_mat.block(0,0,n_dim,n_dim) = C11_mat;
    V_mat.block(n_dim,n_dim,3,3) = C22_mat;
    V_mat.block(n_dim,0,3,n_dim) = C21_mat;
    V_mat.block(0,n_dim,n_dim,3) = C21_mat.transpose();

    // ===> loop over tracks
    std::vector<V0FitterTrack>::iterator BTIter;
    int iRP=0;
    for (BTIter = v0FitterTracks.begin(); BTIter != v0FitterTracks.end() ; ++BTIter)
    {
      // chi2 per track
      AmgSymMatrix(5) covTrk; covTrk.setZero();
      covTrk = Wmeas0_mat.block(5*iRP,5*iRP,4+5*iRP,4+5*iRP);
      AmgVector(5) chi_vec; chi_vec.setZero();
      for (unsigned int i=0; i<5; ++i) chi_vec(i) = DeltaY_vec(i+5*iRP);
      double chi2Trk = chi_vec.dot(covTrk*chi_vec);
      (*BTIter).chi2=chi2Trk;
      iRP++;
    }

    // Store the vertex 
    xAOD::Vertex* vx = new xAOD::Vertex;
    vx->makePrivateStore();
    vx->setPosition (frameOrigin);
    vx->setCovariancePosition (C22_mat);
    vx->setFitQuality(chi2,static_cast<float>(ndf));
    vx->setVertexType(xAOD::VxType::V0Vtx);

    // Store the tracks at vertex 
    std::vector<VxTrackAtVertex> & tracksAtVertex = vx->vxTrackAtVertex(); tracksAtVertex.clear();
    Amg::Vector3D Vertex(frameOrigin[0],frameOrigin[1],frameOrigin[2]);
    const Trk::PerigeeSurface Surface(Vertex);
    Trk::Perigee * refittedPerigee(0);
    unsigned int iterf=0;
    std::vector<V0FitterTrack>::iterator BTIterf;
    for (BTIterf = v0FitterTracks.begin(); BTIterf != v0FitterTracks.end() ; ++BTIterf)
    {
      AmgSymMatrix(5) * CovMtxP = new AmgSymMatrix(5);
      CovMtxP->setIdentity();
      for (unsigned int i=0; i<5; ++i) {
        for (unsigned int j=0; j<i+1; ++j) {
          double val = V_mat(5*iterf+i,5*iterf+j);
          CovMtxP->fillSymmetric(i,j,val);
        }
      }
      refittedPerigee = new Trk::Perigee (Y_vec(0+5*iterf),Y_vec(1+5*iterf),Y_vec(2+5*iterf),Y_vec(3+5*iterf),Y_vec(4+5*iterf), Surface, CovMtxP);
      tracksAtVertex.emplace_back((*BTIterf).chi2, refittedPerigee, (*BTIterf).originalPerigee);
      iterf++;
    }

    // Full Covariance Matrix
    unsigned int sfcmv = nPar*(nPar+1)/2;
    std::vector<float> floatErrMtx(sfcmv,0.);
    unsigned int ipnt = 0;
    for (unsigned int i=0; i<nPar; ++i) {
      for (unsigned int j=0; j<i+1; ++j) {
        floatErrMtx[ipnt++]=V_mat(i,j);
      }
    }
    vx->setCovariance(floatErrMtx);

    return vx;
  }


} //end of namespace definitions
