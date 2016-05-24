/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           LifetimeTag.cxx  -  Description
                             -------------------
    begin   : 10.03.04
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/
#include "JetTagTools/LifetimeTag.h"

//#include "JetEvent/Jet.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "xAODTracking/TrackParticle.h"
#include "JetTagInfo/LifetimeInfo.h"
#include "Navigation/NavigationToken.h"

#include "JetTagTools/LikelihoodTool.h"
#include "JetTagTools/HistoHelperRoot.h"

#include "ITrackToVertex/ITrackToVertex.h"
#include "GaudiKernel/ITHistSvc.h"
#include "JetTagEvent/TrackAssociation.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

// needed for ref mode
#include "JetTagInfo/TruthInfo.h"

// framework includes
#include "GaudiKernel/ServiceHandle.h"

namespace Analysis
{
typedef std::multimap<double, const xAOD::TrackParticle*>::iterator TrkMMIter;

LifetimeTag::LifetimeTag(const std::string& t, const std::string& n, const IInterface* p)
        : AthAlgTool(t,n,p),
        m_runModus("analysis"),
        m_histoHelperSig(NULL),
        m_histoHelperBkg(NULL),
        m_trackToVertexTool("Reco::TrackToVertex"),
        m_priVtx(0),
        m_likelihoodTool("Analysis::LikelihoodTool"),
        m_useVariables(std::vector<std::string>()),
        m_lifetimeMode("2D"),
        m_minpt(1.*Gaudi::Units::GeV),
        m_maxd0wrtPrimVtx(1.*Gaudi::Units::mm),
        m_maxz0wrtPrimVtx(1.5*Gaudi::Units::mm),
        m_precisionHits(9),
        m_PixelHits(2),
        m_BLayerHits(1)
{
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus",       m_runModus);
    declareProperty("useVariables",  m_useVariables);
    declareProperty("minpt", m_minpt);
    declareProperty("maxd0wrtPrimVtx", m_maxd0wrtPrimVtx);
    declareProperty("maxz0wrtPrimVtx", m_maxz0wrtPrimVtx);
    declareProperty("PrecisionHits", m_precisionHits);
    declareProperty("PixelHits",     m_PixelHits);
    declareProperty("BLayerHits",    m_BLayerHits);
    declareProperty("LifetimeModus", m_lifetimeMode);
    declareProperty("LikelihoodTool", m_likelihoodTool);
    declareProperty("TrackToVertexTool",  m_trackToVertexTool);
}

LifetimeTag::~LifetimeTag()
{
  delete m_histoHelperSig;
  delete m_histoHelperBkg;
}

StatusCode LifetimeTag::initialize()
{
    ServiceHandle<ITHistSvc> myHistoSvc("THistSvc", name());

    if( myHistoSvc.retrieve().isSuccess() ) {
      ATH_MSG_DEBUG(name()<<": HistoSvc loaded successfully.");
        m_histoHelperSig = new HistoHelperRoot(&*myHistoSvc);
        m_histoHelperBkg = new HistoHelperRoot(&*myHistoSvc);
        m_histoHelperSig->setCheckOverflows(true);
        m_histoHelperBkg->setCheckOverflows(true);
    } else {
      ATH_MSG_ERROR(name()<<": HistoSvc could NOT be loaded.");
      return StatusCode::FAILURE;
    }

    m_printParameterSettings();

    /* ----------------------------------------------------------------------------------- */
    /*                 READ IN REFHISTOS WITH AIDA IF IN ANALYSIS MODE                     */
    /* ----------------------------------------------------------------------------------- */
    if (m_runModus == "analysis")
    {
      /// retrieve the Likelihood tool
      if ( m_likelihoodTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("failed to retrieve tool "<<m_likelihoodTool);
        return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("retrieved tool "<<m_likelihoodTool);
      }
      m_likelihoodTool->readInHistosFromFile("SigCali/");
      m_likelihoodTool->readInHistosFromFile("BkgCali/");
      m_likelihoodTool->printStatus();
    }

    /* Retrieve TrackToVertex extrapolator tool */
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("failed to retrieve tool "<<m_trackToVertexTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("retrieved tool "<<m_trackToVertexTool);
    }

    /* ----------------------------------------------------------------------------------- */
    /*                         BOOK HISTOS IF IN REFERENCE MODE                            */
    /* ----------------------------------------------------------------------------------- */
    const std::string& lm = m_lifetimeMode; // just to make the book lines shorter ...
    if (m_runModus=="reference")
    {
      if (lm == "1D") 
      {
        m_histoHelperBkg->bookHisto("/BkgCali/significance1D", "track significance 1D", 200, -10., 13.);
        m_histoHelperSig->bookHisto("/SigCali/significance1D", "track significance 1D", 200, -10., 13.);
      }
      else if (lm == "2D") 
      {
        m_histoHelperBkg->bookHisto("/BkgCali/significance2D", "track significance 2D", 200, -20., 40.);
        m_histoHelperSig->bookHisto("/SigCali/significance2D", "track significance 2D", 200, -20., 40.);
      }
    }
    return StatusCode::SUCCESS;
}


StatusCode LifetimeTag::finalize()
{
  //  ATH_MSG_DEBUG(name()<<": Finalizing successfully");
    return StatusCode::SUCCESS;
}

void LifetimeTag::tagJet(xAOD::Jet& jetToTag)
{
    std::vector<const xAOD::TrackParticle*> myJetToTag;
    std::multimap<double, const xAOD::TrackParticle*> myTrackSIP;
    std::multimap<double, const xAOD::TrackParticle*> myTrackSignificance;

    /* Create the info class and append it to the Jet */
    std::string instanceName(name());
    LifetimeInfo * lifeInfo = new LifetimeInfo(instanceName.erase(0,8));
    jetToTag.addInfo(lifeInfo);

    /* Extract the TrackParticles from the jet and apply preselection on the track.*/
    std::vector<const xAOD::TrackParticle*>* trackVector = NULL;
    const Analysis::TrackAssociation *trkInJet = jetToTag.getAssociation<TrackAssociation>("Tracks");

    if(!trkInJet){
      ATH_MSG_WARNING("#BTAG#  Could not find tracks in jets");
    }
    else{
      trackVector = jetToTag.getAssociation<TrackAssociation>("Tracks")->tracks();
      for (std::vector<const xAOD::TrackParticle*>::iterator jetItr = trackVector->begin(); jetItr != trackVector->end() ; ++jetItr )
	{
	  if (m_preselect(*jetItr)==true) myJetToTag.push_back(*jetItr);
	}
      delete trackVector; trackVector=0;
    }

    /* Return if there are not enough tracks left after preselection. */
    if (myJetToTag.size()<2)
    {
	ATH_MSG_VERBOSE("Jet does not have enough tracks: "<<myJetToTag.size());
        return;
    }

    /* Calculate the impact significance with a lifetime sign. Check if the closest
    point of approach between the jet axis and the track is downstream: i.e. on
    the hemisphere of the jet (which starts at the prim.vertex) or upstream: i.e.
    on the other side of the jet hemisphere. In the following algorithm the jet
    is defined as: vec(p) + s*vec(u) where: vec(p) is the position of the primary
    vertex and vec(u) is the direction of the jet. And the track as: vec(q) + t*vec(v)
    where: vec(q) is the position of the perigee of the track and vec(v) the
    direction of the track momentum at the perigee. */

    /* here you can replace the jetDirection with the vector from the primary to the
    secondary vertex (u = SV - PV). */
    std::cout.precision(5);
    Amg::Vector3D jetDirection = m_jetMomentum(myJetToTag);
    Amg::Vector3D unit = jetDirection.unit();

    ATH_MSG_VERBOSE("Jetdirection: (" << unit[0] << ", " << unit[1] << ", " << unit[2] << ") ");

    for (std::vector<const xAOD::TrackParticle*>::iterator trkItr = myJetToTag.begin(); trkItr != myJetToTag.end(); ++trkItr)
    {
        const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(**trkItr, m_priVtx->recVertex().position());

        double trackSigD0 = Amg::error(*(perigee->covariance()), Trk::d0);
        double trackSigZ0 = Amg::error(*(perigee->covariance()), Trk::z0);
        if (trackSigD0<=0. || trackSigZ0<=0.)
        {
	  ATH_MSG_WARNING("Error on d0 or z0 negative!");
	  continue;
        }

        double signOfIP = m_calculateSignOfIP(perigee, unit);

        double d0wrtPriVtx( perigee->parameters()[Trk::d0] );
        double z0wrtPriVtx( perigee->parameters()[Trk::z0] );

        double sIP(0.);
        double significance(0.);

        // calculate errors as given in delphi paper
        // marseille does it the same way
        if (m_lifetimeMode == "1D")
        {
            sIP = signOfIP*fabs(z0wrtPriVtx);

            // error on zIP:
            double trackSigTheta = Amg::error(*(perigee->covariance()),Trk::theta);
	    double theta = (*trkItr)->theta();
            double sinTheta = sin(theta);
            double cosTheta = cos(theta);
            double deltaZ = perigee->parameters()[Trk::z0];
            z0wrtPriVtx = deltaZ*sinTheta;

            double dZIPdTheta = deltaZ*cosTheta;
            double dZIPdz0 = sinTheta;
            double dZIPdzV = -sinTheta;
            double DTheta2 = dZIPdTheta*dZIPdTheta*trackSigTheta*trackSigTheta;
            double DZ02 = dZIPdz0*dZIPdz0*trackSigZ0*trackSigZ0;
            double DZV2 = dZIPdzV*dZIPdzV*m_priVtx->recVertex().covariancePosition()(2,2);
            double DThetaZ0 = 2.*dZIPdTheta*dZIPdz0* (*(perigee->covariance()))(Trk::theta,Trk::z0);
            double newZ0Err = DTheta2 + DZ02 + DZV2 + DThetaZ0;
            double z0ErrwrtPriVtx = (newZ0Err>0 ? sqrt(newZ0Err) : -10e-9);

            significance= signOfIP*fabs(z0wrtPriVtx/z0ErrwrtPriVtx);
        }
        else if (m_lifetimeMode == "2D")
        {
          sIP = signOfIP*fabs(d0wrtPriVtx);

          // error on IP:
          double trackPhi = perigee->parameters()[Trk::phi];
          double dIPdx = sin(trackPhi);
          double dIPdy = -cos(trackPhi);
          double DD0 = trackSigD0*trackSigD0;
          double DXX = dIPdx*dIPdx*m_priVtx->recVertex().covariancePosition()(0,0);
          double DYY = dIPdy*dIPdy*m_priVtx->recVertex().covariancePosition()(1,1);
          double DXY = 2.*dIPdx*dIPdy*m_priVtx->recVertex().covariancePosition()(0,1);
          double newD0Err = DD0 + DXX + DYY + DXY;
          double d0ErrwrtPriVtx = (newD0Err>0 ? sqrt(newD0Err) : -10e-9);
          significance= signOfIP*fabs(d0wrtPriVtx/d0ErrwrtPriVtx);
        }
        else if (m_lifetimeMode == "3D")
        {
          double d0squared(d0wrtPriVtx*d0wrtPriVtx);
          double z0squared(z0wrtPriVtx*z0wrtPriVtx);

          double a0wrtPriVtx( sqrt(d0squared+z0squared) ); // 3D impact parameter
          double trackSigA0(1./a0wrtPriVtx * sqrt(d0squared*trackSigD0*trackSigD0 + z0squared*trackSigZ0*trackSigZ0));

          sIP = signOfIP*fabs(a0wrtPriVtx);
          significance= signOfIP*fabs(a0wrtPriVtx/trackSigA0);
        }
        else
        {
	  ATH_MSG_WARNING("No correct LifetimeTagMode chosen: " <<m_lifetimeMode);
        }

        delete perigee;
        myTrackSIP.insert(std::make_pair(sIP,(*trkItr)));
        myTrackSignificance.insert(std::make_pair(significance,(*trkItr)));
    } // end for loop over particles in the jet

    // push the ordered sigend IP and significance back into a FloatVec
    std::vector<double> sigImpPar;
    sigImpPar.clear();
    for (TrkMMIter i=myTrackSIP.begin(); i!=myTrackSIP.end(); i++)
        sigImpPar.push_back((*i).first);
    std::vector<double> signific;
    signific.clear();
    for (TrkMMIter i=myTrackSignificance.begin(); i!=myTrackSignificance.end(); i++)
        signific.push_back((*i).first);

    lifeInfo->setIP(sigImpPar);
    lifeInfo->setSignificance(signific);

    if (m_likelihoodTool!=0)
    {

      ATH_MSG_VERBOSE("Significances for this Jet:");
      for (std::vector<double>::iterator itr = signific.begin() ; itr != signific.end() ; itr++)
      {
	ATH_MSG_VERBOSE(*itr);
      }
      m_likelihoodTool->setLhVariableValue("significance"+m_lifetimeMode, signific);

      ATH_MSG_VERBOSE("Starting LH calculation: ");

      lifeInfo->setTagLikelihood(m_likelihoodTool->calculateLikelihood());
      lifeInfo->setWeight(m_likelihoodTool->calculateWeight());
    }

    /* Check if we are in reference histogram production mode. Fill the reference
    histogram with the signed impact parameters.
    The histogram for the integratedNegativeIP distributions is filled at the very
    end of the job (since it needs a finished negativeIP histo) */
    if (m_runModus=="reference")
    {
        if (jetToTag.pt() > 15.*Gaudi::Units::GeV && fabs(jetToTag.eta()) < 2.5)
        {
          // need to know some truth stuff now
          const TruthInfo* truthInfo = jetToTag.tagInfo<TruthInfo>("TruthInfo");
          if (truthInfo != 0)
          {
            HistoHelperRoot* histoHelper(0);
	    std::string hDir;
            double rmin(0.);
            if (truthInfo->jetTruthLabel() == "N/A")
            {
              // jetcleaning: no b or c in vicinity of light jet ...
              rmin = truthInfo->deltaRMinTo("C");
              if (rmin < truthInfo->deltaRMinTo("B")) rmin = truthInfo->deltaRMinTo("B");
              if (rmin > 0.8) {
		histoHelper = m_histoHelperBkg;
	        hDir = "/BkgCali/";
	      }
            }
            if (truthInfo->jetTruthLabel()=="B") {
	      histoHelper = m_histoHelperSig;
	      hDir = "/SigCali/";
	    }
            if (histoHelper != 0)
            {
              for (std::vector<double>::iterator i=signific.begin(); i!=signific.end();i++)
              {
                histoHelper->fillHisto(hDir+"significance"+m_lifetimeMode,(*i));
              }
            }
          } else
          {
	    ATH_MSG_WARNING("LifetimeTag" << m_lifetimeMode << " TruthLabel == 0!!!");
          }
        }
    }

    /* Tagging done. Make info object valid, i.e. tag was ok. Fill the JetTag and return ... */
    lifeInfo->makeValid();
    return;
}

void LifetimeTag::finalizeHistos()
{
  if (m_runModus=="reference")
  {
    m_histoHelperSig->normalizeHistos();
    m_histoHelperBkg->normalizeHistos();
//     m_histoHelperSig->smoothHistos("significance"+m_lifetimeMode);
  }
  return;
}

/* ------------------------------------------------------------------------------ */
/*                        Private Helper Functions                                */
/* ------------------------------------------------------------------------------ */
bool LifetimeTag::m_preselect(const xAOD::TrackParticle *trkPrt)
{
    const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*trkPrt, m_priVtx->recVertex().position());

    double d0wrtPriVtx( perigee->parameters()[Trk::d0] );
    double z0wrtPriVtx( perigee->parameters()[Trk::z0] );

    delete perigee;

    ///////////////////


    // next comment is not true anymore with the above lines ...
    // have to check what TrackToVertexTool does when there is no PrimVtx
    ///////
    // this means that there was no primary vertex and the TrackParticle
    // has been created with d0wrtPrimVtx == 0
    // should not happen here anymore because BJetBuilder would stop before
    // and go to the next event ... just to make sure!
    if (d0wrtPriVtx == 0.)
    {
        //std::cout << "d0 to vtx is 0 ... " << trkPrt->d0wrtPrimVtx() << std::endl;
        return false;
    }

    ATH_MSG_DEBUG(name() << " m_preselect(): trkPrt->pt(): " << trkPrt->pt());
    ATH_MSG_DEBUG(name() << " m_preselect(): d0 to primary vertex: " << d0wrtPriVtx);
    ATH_MSG_DEBUG(name() << " m_preselect(): z0 to primary vertex: " << z0wrtPriVtx);

    if (trkPrt->pt()<m_minpt)
        return false;

    if (fabs(d0wrtPriVtx)>m_maxd0wrtPrimVtx)
        return false;
    if (fabs(z0wrtPriVtx)>m_maxz0wrtPrimVtx)
        return false;

    int SCTHits, PixelHits;
    trkPrt->summaryValue(SCTHits, xAOD::numberOfSCTHits);
    trkPrt->summaryValue(PixelHits, xAOD::numberOfPixelHits);


    ATH_MSG_DEBUG(name() << " m_preselect(): SCTHits " << SCTHits );
    ATH_MSG_DEBUG(name() << " m_preselect(): PixelHits: " << PixelHits);

    if ((SCTHits+PixelHits)<m_precisionHits)
        return false;
    if (PixelHits<m_PixelHits)
        return false;

    int BLayerHits;
    trkPrt->summaryValue(BLayerHits, xAOD::numberOfBLayerHits);
    ATH_MSG_DEBUG(name() << " m_preselect(): BLayerHits: " << BLayerHits);

    if (BLayerHits<m_BLayerHits)
        return false;
    return true;
}

Amg::Vector3D LifetimeTag::m_jetMomentum(const std::vector<const xAOD::TrackParticle*>& tracksInJet)
{
    Amg::Vector3D jetDir;
    double px(0.);
    double py(0.);
    double pz(0.);
    /* Calculate the sum of the charged momenta in the jet. TODO: Use the jet momentum
    of the jet, including clusters instead. */
    for (std::vector<const xAOD::TrackParticle*>::const_iterator i=tracksInJet.begin(); i!=tracksInJet.end(); ++i)
    {
      xAOD::IParticle::FourMom_t p4 = (*i)->p4();
      px += p4.Px();
      py += p4.Py();
      pz += p4.Pz();
    }
    jetDir.x() = px;
    jetDir.y() = py;
    jetDir.z() = pz;
    return jetDir;
}

void LifetimeTag::m_printParameterSettings()
{
  ATH_MSG_INFO(name() << " initialize(): Parametersettings ");
  ATH_MSG_INFO("I am in " << m_runModus << " modus." );
  ATH_MSG_INFO("I am using the following variables:");
  
  for (std::vector<std::string>::iterator i = m_useVariables.begin();
       i!=m_useVariables.end() ; ++i)
    {
      ATH_MSG_INFO((*i));
    }

  ATH_MSG_INFO("Track quality cuts:");
  ATH_MSG_INFO("pt            >  " << m_minpt << "\t MeV ");
  ATH_MSG_INFO("d0wrtPrimVtx  <  " << m_maxd0wrtPrimVtx << "\t mm ");
  ATH_MSG_INFO("z0wrtPrimVtx  <  " << m_maxz0wrtPrimVtx << "\t mm ");
  ATH_MSG_INFO("PrecisionHits >= " << m_precisionHits);
  ATH_MSG_INFO("PixelHits     >= " << m_PixelHits);
  ATH_MSG_INFO("BLayerHits    >= " << m_BLayerHits);
}

double LifetimeTag::m_calculateSignOfIP(
    const Trk::Perigee* perigee, Amg::Vector3D& unit)
{
    double sign(0.);

// these quantities are wrt Primary Vertex
    if (m_lifetimeMode == "2D")
    {
      double trackD0(perigee->parameters()[Trk::d0]);
      double trackPhi(perigee->parameters()[Trk::phi]);
      sign = sinf( atan2(unit.y(),unit.x()) - trackPhi )*trackD0;
    }
    if (m_lifetimeMode == "1D")
    {
      double trackTheta  = perigee->parameters()[Trk::theta];
      double trackEta = -logf(tanf(trackTheta/2.));
      double trackZ0  = perigee->parameters()[Trk::z0];
      CLHEP::HepLorentzVector jetP4(unit.x(), unit.y(), unit.z());
      double jetEta = jetP4.pseudoRapidity();
      sign = (jetEta - trackEta)*trackZ0;
    }
    return (sign>=0. ? 1. : -1.);

//     CLHEP::Hep3Vector q(-trackD0*sin(trackPhi), trackD0*cos(trackPhi), trackZ0);
//     mLog << MSG::VERBOSE << "Perigee position of track: ("
//     << q[0] << " ," << q[1] << " ," << q[2] << ")" << endreq;
    /* Below one should change this to use the PxPyPzE interface of the TrackParticle
    instead of the momentum() method from the perigee. */
///////// TP global perigee
//     CLHEP::Hep3Vector trackDir( (*trkItr)->measuredPerigee()->momentum()[Trk::px],
//        (*trkItr)->measuredPerigee()->momentum()[Trk::py],
//        (*trkItr)->measuredPerigee()->momentum()[Trk::pz] );
///////// or extrapolated to pri vtx
//     CLHEP::Hep3Vector trackDir( perigee->momentum()[Trk::px],
//                          perigee->momentum()[Trk::py],
//                          perigee->momentum()[Trk::pz] );
/////////////
//     CLHEP::Hep3Vector v = trackDir.unit();
//     mLog << MSG::VERBOSE << "Dir of track at the perigee: ("
//     << v[0] << " ," << v[1] << " ," << v[2] << ")" << endreq;

//     CLHEP::Hep3Vector diff = m_priVtx->position() - q;

//     double A1 = unit*v;
//     double A2 = v*v;

//     double s = ( A1*(v*diff) - A2*(unit*diff) ) / ( (unit*unit)*A2 - A1*A1 );

//     delete perigee;
//     return (s>=0.?1.:-1.);
}

}

