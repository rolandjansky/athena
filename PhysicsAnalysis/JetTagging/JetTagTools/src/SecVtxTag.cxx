/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           BtagAlgTool.cxx  -  Description
                             -------------------
    begin   : 10.03.04
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :
 
 ***************************************************************************/

// framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "JetTagTools/SecVtxTag.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
//#include "JetEvent/Jet.h"
#include "JetTagInfo/SecVtxInfo.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Vector/LorentzVector.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "xAODTracking/TrackParticle.h"

#include "JetTagTools/LikelihoodTool.h"
#include "JetTagTools/HistoHelperRoot.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "ITrackToVertex/ITrackToVertex.h"

#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SvxSummary.h"

#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
//#include "TrkParticleBase/TrackParticleBase.h"

#include "JetTagEvent/TrackAssociation.h"

namespace Analysis
{
  SecVtxTag::SecVtxTag(const std::string& t, const std::string& n, const IInterface* p)
      : AthAlgTool(t,n,p),
      
      m_runModus("analysis"),

      m_histoHelperSig(NULL),
      m_histoHelperBkg(NULL),

      m_priVtx(0),
      
      m_fitTool("Trk::FastVertexFitter"),
      m_trackToVertexTool("Reco::TrackToVertex"),
      m_likelihoodTool("Analysis::LikelihoodTool"),

      m_useVKalTool(true),

      m_useVariables(std::vector<std::string>()),

      m_vertexFindingMode("BU"),

      m_minpt(1.*Gaudi::Units::GeV),
      m_maxd0wrtPrimVtx(1.*Gaudi::Units::mm),
      m_maxz0wrtPrimVtx(1.5*Gaudi::Units::mm),
      m_min2DSigToPrimVtx(3.),

      m_precisionHits(9),
      m_PixelHits(2),
      m_BLayerHits(1),

      m_buMaxChi2OfTrack(4.),
      m_minDistToPrimVtx(0.2),
      m_secVxFinderName("InDetVKalVxInJetTool")
  {
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus",      m_runModus);
    declareProperty("useVariables",  m_useVariables);

    declareProperty("VertexFitterTool"  , m_fitTool);
    declareProperty("TrackToVertexTool" , m_trackToVertexTool);
    declareProperty("LikelihoodTool"    , m_likelihoodTool);
    
    declareProperty("VertexFindingMode", m_vertexFindingMode);

    declareProperty("minpt", m_minpt);
    declareProperty("maxd0wrtPrimVtx", m_maxd0wrtPrimVtx);
    declareProperty("maxz0wrtPrimVtx", m_maxz0wrtPrimVtx);
    declareProperty("min2DSigToPrimVtx", m_min2DSigToPrimVtx);
    declareProperty("PrecisionHits", m_precisionHits);
    declareProperty("PixelHits", m_PixelHits);
    declareProperty("BLayerHits", m_BLayerHits);

    declareProperty("buMaxChi2OfTrack", m_buMaxChi2OfTrack);
    declareProperty("minDistToPrimVtx", m_minDistToPrimVtx);

    declareProperty("SecVxFinderName",m_secVxFinderName);
  }

  SecVtxTag::~SecVtxTag()
  {
    delete m_histoHelperSig;
    delete m_histoHelperBkg;
  }

  StatusCode SecVtxTag::initialize()
  {
    ServiceHandle<ITHistSvc> myHistoSvc("THistSvc", name());

    if( myHistoSvc.retrieve().isSuccess() )
    {
      ATH_MSG_DEBUG(name() << ": HistoSvc loaded successfully.");
      m_histoHelperSig = new HistoHelperRoot(&*myHistoSvc);
      m_histoHelperBkg = new HistoHelperRoot(&*myHistoSvc);
      m_histoHelperSig->setCheckOverflows(true);
      m_histoHelperBkg->setCheckOverflows(true);
    }
    else ATH_MSG_ERROR(name() << ": HistoSvc could NOT be loaded.");

    m_printParameterSettings();
    
    /* ----------------------------------------------------------------------------------- */
    /*                 Get the vertexing tool                                              */
    /* ----------------------------------------------------------------------------------- */
    if (!m_useVKalTool)
    {
      if ( m_fitTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("failed to retrieve tool " << m_fitTool);
        return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("retrieved tool " << m_fitTool);
      }
  
      /* Retrieve TrackToVertex extrapolator tool */
      if ( m_trackToVertexTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("failed to retrieve tool " << m_trackToVertexTool);
        return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("retrieved tool " << m_trackToVertexTool);
      }
    }
        
    /* ----------------------------------------------------------------------------------- */
    /*                 READ IN REFHISTOS IF IN ANALYSIS MODE                               */
    /* ----------------------------------------------------------------------------------- */
    if (m_runModus == "analysis")
    {
      /// retrieve the Likelihood tool
      if ( m_likelihoodTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("failed to retrieve tool " << m_likelihoodTool);
        return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("retrieved tool " << m_likelihoodTool);
      }
      
      // the t3333 has to be removed for new histos?
      m_likelihoodTool->readInHistosFromFile("SigCali/");
      m_likelihoodTool->readInHistosFromFile("BkgCali/");
      m_likelihoodTool->printStatus();
    }

    /* ----------------------------------------------------------------------------------- */
    /*                         BOOK HISTOS WITH IF IN REFERENCE MODE                       */
    /* ----------------------------------------------------------------------------------- */
    if (m_runModus=="reference")
    {
        m_histoHelperSig->bookHisto("/SigCali/vtxmassBU", "", 40, 0., 5.);
        m_histoHelperBkg->bookHisto("/BkgCali/vtxmassBU", "", 40, 0., 5.);
        m_histoHelperSig->bookHisto("/SigCali/vtxmultBU", "", 10, 0.1, 10.1);
        m_histoHelperBkg->bookHisto("/BkgCali/vtxmultBU", "", 10, 0.1, 10.1);
        m_histoHelperSig->bookHisto("/SigCali/vtxdistBU", "", 80, 0.1, 40.1);
        m_histoHelperBkg->bookHisto("/BkgCali/vtxdistBU", "", 80, 0.1, 40.1);
        m_histoHelperSig->bookHisto("/SigCali/vtxenergyFractionBU", "", 40, 0., 1.);
        m_histoHelperBkg->bookHisto("/BkgCali/vtxenergyFractionBU", "", 40, 0., 1.);
        m_histoHelperSig->bookHisto("/SigCali/vtxNGTVtxBU", "", 21, 0.1, 21.1);
        m_histoHelperBkg->bookHisto("/BkgCali/vtxNGTVtxBU", "", 21, 0.1, 21.1);
    }

    ATH_MSG_DEBUG(name() << ": Initializing successfully.");
    return StatusCode::SUCCESS;
  }

  StatusCode SecVtxTag::finalize()
  {
    //    ATH_MSG_DEBUG(name() << ": Finalizing successfully" );
    return StatusCode::SUCCESS;
  }

  void SecVtxTag::tagJet(xAOD::Jet& jetToTag)
  {
    ATH_MSG_DEBUG("in tagJet()");
    /* Create the info class and append it to the Jet */
    std::string instanceName(name());
    SecVtxInfo * secVtxInfo = new SecVtxInfo(instanceName.erase(0,8));
    jetToTag.addInfo(secVtxInfo);

    TrackVec2 tracksInSV;
    const Trk::VxCandidate* myVxCandidate(0);
    Trk::RecVertex VertInJet;
    double Prob(-1.);
    double RPhiDistance(-1.);
    double VtxMass(-1000.);
    double energyFraction(-1.);
    int NGood2TrackVertices(-1);
    int NTracksInJet(-1);
    int NTracksInSV(-1);
    double distance(-1.);

    ATH_MSG_VERBOSE("PVX x = " << m_priVtx->recVertex().position().x() << " y = " << m_priVtx->recVertex().position().y() << " z = " << m_priVtx->recVertex().position().z());

    /* Return if there are not enough tracks left after preselection. */
    if (!m_useVKalTool)
    {
      /* Extract the TrackParticles from the jet and apply preselection on the track.
      Ignore other constituents for now. */
      std::vector<const xAOD::TrackParticle*> myJetToTag;
      /* Extract the TrackParticles from the jet and apply preselection on the track.*/
      std::vector<const xAOD::TrackParticle*>* trackVector = jetToTag.getAssociation<TrackAssociation>("Tracks")->tracks();
      for (std::vector<const xAOD::TrackParticle*>::iterator jetItr = trackVector->begin(); jetItr != trackVector->end() ; ++jetItr )
      {
          if (m_preselect(*jetItr)==true) myJetToTag.push_back(*jetItr);
      }
      delete trackVector; trackVector=0;
      
      ATH_MSG_VERBOSE(myJetToTag.size() << " tracks after preselect for sec vtx fit.");
      if (myJetToTag.size()<2)
      {
	ATH_MSG_VERBOSE("Less than two tracks in the jet. Returning ...");
        secVtxInfo->setFitType(SecVtxInfo::NoFit);
        return;
      }

      /**
       Calculate secondary vertex information
       */
      if (m_vertexFindingMode == "BU")
      {
        myVxCandidate=m_fitBuildUpVertex(myJetToTag, tracksInSV);
      }
      else if (m_vertexFindingMode == "TD")
      {
        myVxCandidate=m_fitTearDownVertex(myJetToTag, tracksInSV);
      }
      else
      {
	ATH_MSG_DEBUG("Wrong vertex fitting method chosen ..." << m_vertexFindingMode);
        return;
      }

      if (myVxCandidate!=0)
      {
        const Trk::RecVertex& vertexInJet = myVxCandidate->recVertex();
        int ndof = vertexInJet.fitQuality().numberDoF();
        double chi2 = vertexInJet.fitQuality().chiSquared();
        if(ndof>0 && chi2>=0.)
        {
          Genfun::CumulativeChiSquare myCumulativeChiSquare(ndof);
          Prob = 1.-myCumulativeChiSquare(chi2);
        }

        // vertex found
        if (Prob >= 0.)
        {
          if      (m_vertexFindingMode == "BU")
            secVtxInfo->setFitType(SecVtxInfo::BuildUp);
          else if (m_vertexFindingMode == "TD")
            secVtxInfo->setFitType(SecVtxInfo::TearDown);

	  ATH_MSG_VERBOSE(m_vertexFindingMode << " Vtx fitted with " << tracksInSV.size() << " tracks. Probability = " << Prob);
          VtxMass=m_massOfVertex(tracksInSV);
	  ATH_MSG_VERBOSE(m_vertexFindingMode << " Vtx mass = " << VtxMass/1000. << " GeV");
          if (VtxMass<0.)
            VtxMass=0.;

          double xMom(0.);
          double yMom(0.);
          double zMom(0.);
          double Energy(0.);
          for (TrackIter i=tracksInSV.begin(); i!=tracksInSV.end(); i++)
          {
            double x((*i)->perigeeParameters().momentum()[Trk::px]);
            double y((*i)->perigeeParameters().momentum()[Trk::py]);
            double z((*i)->perigeeParameters().momentum()[Trk::pz]);
            xMom += x; //std::cout << xMom << std::endl;
            yMom += y;
            zMom += z;
            Energy += sqrt(x*x + y*y + z*z);
          }
          energyFraction = Energy/jetToTag.e();

          RPhiDistance = sqrt(
                           pow( m_priVtx->recVertex().position()[0] - vertexInJet.position()[0] , 2 ) +
                           pow( m_priVtx->recVertex().position()[1] - vertexInJet.position()[1] , 2 )   );

          distance = sqrt(
                       pow( RPhiDistance , 2 ) +
                       pow( m_priVtx->recVertex().position()[2] - vertexInJet.position()[2] , 2 ) );
        }
        // no vertex found
        else if (Prob < 0.)
        {
	  ATH_MSG_DEBUG("Fitted secondary vertex probability < 0!");
          secVtxInfo->setFitType(SecVtxInfo::NoFit);
          tracksInSV.clear();
          return;
        }
      }
      else // vertex pointer zero ... also no fit
      {
	ATH_MSG_DEBUG("Fitted secondary vertex pointer is zero!");
        secVtxInfo->setFitType(SecVtxInfo::NoFit);
        tracksInSV.clear();
        return;
      }
    }
    else
    {
      /* for comparisson use vadims secvtx */
      /* Need to retrieve the VKalVrt information in the new way */

      //std::vector<const xAOD::TrackParticle*> TrkFromV0;

      bool vertexIsThere(true);
      
      const ISvxAssociation* newSvxAssociation=jetToTag.getAssociation<ISvxAssociation>(m_secVxFinderName);
      
      if (newSvxAssociation==0) {
	ATH_MSG_DEBUG(" No VKalVrt vertex found, attached to the Jet as association. Not going on with tagging...");
        return;
      }
      
      const Trk::VxSecVertexInfo* myVertexInfo=newSvxAssociation->vertexInfo();
      if (myVertexInfo==0) {
        vertexIsThere=false;
      }

      if (vertexIsThere)
      {
        
        const Trk::VxSecVKalVertexInfo* myVKalVertexInfo=dynamic_cast<const Trk::VxSecVKalVertexInfo*>(myVertexInfo);
        
        if (myVKalVertexInfo==0) {
	  ATH_MSG_WARNING(" Could not cast to a VKalVrt Info object ");
          return;
        } 
        
        const std::vector<Trk::VxCandidate*> & myVertices=myVKalVertexInfo->vertices();
        
        if (myVertices.size()>0) {
        
          VtxMass=myVKalVertexInfo->mass(); 
          energyFraction=myVKalVertexInfo->energyFraction();
          NGood2TrackVertices=(long int)myVKalVertexInfo->n2trackvertices();
          NTracksInJet=-1;//not given for the moment (TO DO for 14.1.x?)

          NTracksInSV=0;

          const std::vector<Trk::VxCandidate*>::const_iterator verticesBegin=myVertices.begin();
          const std::vector<Trk::VxCandidate*>::const_iterator verticesEnd=myVertices.end();
          
          for (std::vector<Trk::VxCandidate*>::const_iterator verticesIter=verticesBegin;
               verticesIter!=verticesEnd;verticesIter++) {
            if ((*verticesIter)==0)
            {
	      ATH_MSG_ERROR(" Secondary vertex from InDetVKalVxInJetFinder has zero pointer. Skipping... ");
              continue;
            }
            const std::vector<Trk::VxTrackAtVertex*>* myTracks=(*verticesIter)->vxTrackAtVertex();
            if (myTracks!=0) {
              NTracksInSV+=myTracks->size();
            }
          }
        }
        
        if (myVertices[0]!=0) {
          const Trk::RecVertex & myVertex=myVertices[0]->recVertex();
          VertInJet  = myVertex;
          //GP (18-02-2008): calculate the distance/error directly here in the code
          if (m_priVtx)
          {
            distance=(myVertex.position()-m_priVtx->recVertex().position()).mag();
            RPhiDistance=(myVertex.position()-m_priVtx->recVertex().position()).perp();
          }
          else
          {
	    ATH_MSG_WARNING("Tagging requested, but no primary vertex supplied.");
          }
        }
        else
        {
	  ATH_MSG_VERBOSE("No vertex. Cannot calculate distances.");
        }
        
      }
    }

    ATH_MSG_VERBOSE("SecVtx Position x = " << VertInJet.position().x() << " y = " << VertInJet.position().y() << " z = " << VertInJet.position().z());
    ATH_MSG_VERBOSE("(norm)Distance: " <<distance);
    ATH_MSG_VERBOSE("RPhi distance to PV: " << RPhiDistance);
    ATH_MSG_VERBOSE("N tracks in jet from VKalVrt "<<NTracksInJet<< " in SVX "<<NTracksInSV);
    ATH_MSG_VERBOSE("Svx Mass = "<< VtxMass/1000. << " Svx E frac = " << energyFraction << " NGood2TrackVertices = " << NGood2TrackVertices);

    // these variables are only filled if a sec vertex could be fitted
    if (m_useVKalTool) secVtxInfo->setSecVtx(VertInJet, Prob, tracksInSV);
    else secVtxInfo->setSecVtx(myVxCandidate->recVertex(), Prob, tracksInSV);
    secVtxInfo->setNumSelTracksForFit(NTracksInJet);
    secVtxInfo->setMult(NTracksInSV);
    secVtxInfo->setMass(VtxMass/1000.);
    secVtxInfo->setRPhiDist(RPhiDistance);
    secVtxInfo->setDist(distance);
    secVtxInfo->setEnergyFraction(energyFraction);
    secVtxInfo->setNumberOfG2TrackVertices(NGood2TrackVertices);

    /* Check if we are in reference histogram production mode. Fill the reference
    histograms. */
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
              histoHelper->fillHisto(hDir+"vtxmassBU", VtxMass/1000.);
              histoHelper->fillHisto(hDir+"vtxdistBU", distance);
              histoHelper->fillHisto(hDir+"vtxmultBU", (double)tracksInSV.size());
              histoHelper->fillHisto(hDir+"vtxenergyFractionBU", energyFraction);
              histoHelper->fillHisto(hDir+"vtxNGTVtxBU", (double)NGood2TrackVertices);
            }
          } else
          {
	    ATH_MSG_WARNING("SecVtxTag TruthLabel == 0!!!");
          }
        }
    }

    if (m_likelihoodTool!=0)
    {
      //         m_likelihoodTool->setLhVariableValue("vtxprob"+m_vertexFindingMode, 0.);
      m_likelihoodTool->setLhVariableValue("vtxmass"+m_vertexFindingMode, VtxMass/1000.);
      m_likelihoodTool->setLhVariableValue("vtxdist"+m_vertexFindingMode, distance);
      m_likelihoodTool->setLhVariableValue("vtxmult"+m_vertexFindingMode, (double)NTracksInSV);
      m_likelihoodTool->setLhVariableValue("vtxenergyFraction"+m_vertexFindingMode, energyFraction);
      m_likelihoodTool->setLhVariableValue("vtxNGTVtx"+m_vertexFindingMode, (double)NGood2TrackVertices);
      secVtxInfo->setTagLikelihood(m_likelihoodTool->calculateLikelihood());
      secVtxInfo->setWeight(m_likelihoodTool->calculateWeight());
    }

    /* Tagging successful. Make the info object valid. */
    secVtxInfo->makeValid();
    ATH_MSG_DEBUG("finished");
    return;
  }

  void SecVtxTag::finalizeHistos()
  {
    if (m_runModus=="reference")
    {
      m_histoHelperSig->normalizeHistos();
      m_histoHelperBkg->normalizeHistos();
//       m_histoHelperSig->smoothHistos("vtxmassBU");
//       m_histoHelperSig->smoothHistos("vtxdistBU");
//       m_histoHelperSig->smoothHistos("vtxmultBU");
//       m_histoHelperSig->smoothHistos("vtxenergyFractionBU");
//       m_histoHelperSig->smoothHistos("vtxNGTVtxBU");
    }
    return;
  }

  /* ------------------------------------------------------------------------------ */
  /*                        Private Helper Functions                                */
  /* ------------------------------------------------------------------------------ */

  Amg::Vector3D SecVtxTag::m_jetMomentum(const TrackVec2& tracksInJet)
  {
    double px=0., py=0., pz=0.;
    /* Calculate the sum of the charged momenta in the jet. TODO: Use the jet momentum
    of the jet, including clusters instead. */
    for (TrackIter i=tracksInJet.begin(); i!=tracksInJet.end(); ++i)
    {
      px += (*i)->perigeeParameters().momentum()[Trk::px];
      py += (*i)->perigeeParameters().momentum()[Trk::py];
      pz += (*i)->perigeeParameters().momentum()[Trk::pz];
    }

    return Amg::Vector3D(px,py,pz);
  }

  /* ------------------------------------------------------------------------------ */

  bool SecVtxTag::m_preselect(const xAOD::TrackParticle *trkPrt)
  {
    const Trk::Perigee* perigee(0);
    const Trk::Perigee* perigeeAtGlobal = &trkPrt->perigeeParameters();
    const Trk::Perigee* perigeeAtVertex = m_trackToVertexTool->perigeeAtVertex(*trkPrt, m_priVtx->recVertex().position());

    double d0wrtPriVtx( perigeeAtVertex->parameters()[Trk::d0] );
    //     double z0wrtPriVtx( perigeeAtVertex->parameters()[Trk::z0] );

    // this error does not include the primary vertex error
    //     double trackSigD0 = perigeeAtVertex->localErrorMatrix().error(Trk::d0);
    //     double trackSigZ0 = perigeeAtVertex->localErrorMatrix().error(Trk::z0);

    // see lifetimetag for remarks with new TrackToVertex tool
    ///////
    // this means that there was no primary vertex and the TrackParticle
    // has been created with d0wrtPrimVtx == 0
    // should not happen here anymore because BJetBuilder would stop before
    // and go to the next event ... just to make sure!
    if (d0wrtPriVtx == 0.)
    {
      return false;
    }

    bool print(false);
    //     if (print) {
    //       std::cout <<"pt:         " << trkPrt->pt() << std::endl;
    //       std::cout <<"d0:         " << d0wrtPriVtx << std::endl;
    //       std::cout <<"z0:         " << z0wrtPriVtx << std::endl;
    //     }

    if (trkPrt->pt()<m_minpt)
    {
      ATH_MSG_VERBOSE("Skipping track because of pt " << trkPrt->pt() << " < " << m_minpt);
      return false;
    }
    // SELECTION WRT PRIM VTX
    perigee=perigeeAtVertex;
    if (fabs(perigee->parameters()[Trk::d0])>m_maxd0wrtPrimVtx)
    {
      ATH_MSG_VERBOSE("Skipping track because of d0 " << fabs(perigee->parameters()[Trk::d0]) << " > " << m_maxd0wrtPrimVtx);
      delete perigeeAtVertex;
      return false;
    }
    if (fabs(perigee->parameters()[Trk::z0])>m_maxz0wrtPrimVtx)
    {
      ATH_MSG_VERBOSE("Skipping track because of z0 " << fabs(perigee->parameters()[Trk::z0]) << " > " << m_maxz0wrtPrimVtx);
      delete perigeeAtVertex;
      return false;
    }

    // SELECTION WRT GLOBAL ZERO
    perigee=perigeeAtGlobal;
    double signif2D(perigee->parameters()[Trk::d0]/std::sqrt((*perigee->covariance())(Trk::d0, Trk::d0)));
    if ( fabs(signif2D) < m_min2DSigToPrimVtx )
    {
      ATH_MSG_VERBOSE("Skipping track because of d0/sigd0 " << signif2D << " < " << m_min2DSigToPrimVtx);
      delete perigeeAtVertex;
      return false;
    }
    delete perigeeAtVertex;
    //////////
    int SCTHits, PixelHits;
    trkPrt->summaryValue(SCTHits, xAOD::numberOfSCTHits);
    trkPrt->summaryValue(PixelHits, xAOD::numberOfPixelHits);

    if (print)
    {
      std::cout <<"SCTHits:    " << SCTHits << std::endl;
      std::cout <<"PixelHits:  " << PixelHits  << std::endl;
    }
    if ((SCTHits+PixelHits)<m_precisionHits)
      return false;
    if (PixelHits<m_PixelHits)
      return false;

    int BLayerHits;
    trkPrt->summaryValue(BLayerHits, xAOD::numberOfBLayerHits);
    if (print)
      std::cout <<"BLayerHits: " << BLayerHits  << std::endl;
    if (BLayerHits<m_BLayerHits)
      return false;
    return true;
  }

  const Trk::VxCandidate* SecVtxTag::m_fitTearDownVertex(TrackVec2 tracksInJet, TrackVec2& tracksInSV)
  {
    ATH_MSG_DEBUG("starting m_fitTearDown");

    // VxBilloirTools does not take TrackParticles as input anymore
    std::vector<const Trk::Track*> vecOfMeasuredPerigee;
    for (TrackVec2::iterator trkPrtItr = tracksInJet.begin();
         trkPrtItr != tracksInJet.end(); ++ trkPrtItr)
    {
      vecOfMeasuredPerigee.push_back(*(*trkPrtItr)->track());
    }

    const Trk::VxCandidate* myVxCandidate(0);
    if (vecOfMeasuredPerigee.size()>1)
    {
      Amg::Vector3D startpoint(0.,0.,0.);

      std::vector<int> indexOfSortedChi2;
      myVxCandidate = m_fitTool->fit(vecOfMeasuredPerigee, Trk::Vertex(startpoint));
      if (myVxCandidate==0) return 0;
      m_sortTracksInChi2(indexOfSortedChi2, myVxCandidate);

      startpoint=myVxCandidate->recVertex().position();
      // !!! VxBilloirTools does not return the track chi2 ordered anymore
      // the last element of indexOfSortedChi2 is the index of the track
      // with the highest chi2
      while ( (*(myVxCandidate->vxTrackAtVertex()))[*(indexOfSortedChi2.end()-1)]->trackQuality().chiSquared() > 5. &&
              vecOfMeasuredPerigee.size() > 2 )
      {
        delete myVxCandidate; // delete the previous one!
        // remove both from the vectors. The Track and the Measured Perigee.
        tracksInJet.erase( tracksInJet.begin()+(*(indexOfSortedChi2.end()-1)) );
        vecOfMeasuredPerigee.erase( vecOfMeasuredPerigee.begin()+(*(indexOfSortedChi2.end()-1)) );

        myVxCandidate = m_fitTool->fit(vecOfMeasuredPerigee, Trk::Vertex(startpoint));
        if (myVxCandidate==0) return 0;
        m_sortTracksInChi2(indexOfSortedChi2, myVxCandidate);
        startpoint=myVxCandidate->recVertex().position();
      }
      for (unsigned int i = 0; i < tracksInJet.size(); ++i)
      {
        tracksInSV.push_back(tracksInJet[indexOfSortedChi2[i]]);
      }
    }
    return myVxCandidate;
  }

  /* ------------------------------------------------------------------------------ */

  const Trk::VxCandidate* SecVtxTag::m_fitBuildUpVertex(TrackVec2 tracksInJet, TrackVec2& tracksInSV)
  {
    ATH_MSG_DEBUG("starting m_fitBuildUp");

    std::vector<const Trk::Track*> trktracksInBUSV;
    double fitProb(-1.);
    // VxBilloirTools does not take TrackParticles as input anymore
    std::vector<const Trk::Track*> vecOfMeasuredPerigee;
    for (TrackVec2::iterator trkPrtItr = tracksInJet.begin();
         trkPrtItr != tracksInJet.end(); ++trkPrtItr)
    {
      vecOfMeasuredPerigee.push_back(*(*trkPrtItr)->track());
    }

    Amg::Vector3D startpoint(0.,0.,0.);
    const Trk::VxCandidate* myVxCandidate(0);

    if (vecOfMeasuredPerigee.size()>1)
    {
      // Looking for seed vertex
      for (std::vector<const Trk::Track*>::iterator i=vecOfMeasuredPerigee.begin(); i!=vecOfMeasuredPerigee.end()-1; i++)
      {
        for (std::vector<const Trk::Track*>::iterator j=i+1; j!=vecOfMeasuredPerigee.end(); j++)
        {
          std::vector<const Trk::Track*> tracksToFit;
          tracksToFit.push_back((*i));
          tracksToFit.push_back((*j));
          const Trk::VxCandidate* tmpVxCandidate(m_fitTool->fit(tracksToFit, Trk::Vertex(startpoint)));
          if (tmpVxCandidate==0)
          {
            if (myVxCandidate!=0) delete myVxCandidate;
            return 0;
          }
          const Trk::RecVertex& tempVtx(tmpVxCandidate->recVertex());
          int ndof = tempVtx.fitQuality().numberDoF();
          double chi2 = tempVtx.fitQuality().chiSquared();
          double tempProb(-1.);
          if(ndof>0 && chi2>=0.)
          {
            Genfun::CumulativeChiSquare myCumulativeChiSquare(ndof);
            tempProb = 1.-myCumulativeChiSquare(chi2);
          }

          double tempDist(sqrt(pow(tempVtx.position()[0]-m_priVtx->recVertex().position()[0],2)+
                               pow(tempVtx.position()[1]-m_priVtx->recVertex().position()[1],2)+
                               pow(tempVtx.position()[2]-m_priVtx->recVertex().position()[2],2)
                              ));
          if ((tempProb>fitProb) && (tempDist>m_minDistToPrimVtx))
          {
            if (myVxCandidate!=0)
              delete myVxCandidate;
            fitProb=tempProb;
            trktracksInBUSV.clear();
            trktracksInBUSV.push_back((*i));
            trktracksInBUSV.push_back((*j));
            myVxCandidate=tmpVxCandidate;
          }
          else
            delete tmpVxCandidate;
        }
      }
      // no seed vertex found
      if (trktracksInBUSV.size() == 0)
        return 0;

      // Adding remaining tracks
      for (std::vector<const Trk::Track*>::iterator i=vecOfMeasuredPerigee.begin(); i!=vecOfMeasuredPerigee.end(); i++)
      {
        if ((*i)==trktracksInBUSV[0] || (*i)==trktracksInBUSV[1])
          continue;
        trktracksInBUSV.push_back((*i));
        startpoint=myVxCandidate->recVertex().position();
        const Trk::VxCandidate* tmpVxCandidate(m_fitTool->fit(trktracksInBUSV, Trk::Vertex(startpoint)));
        if (tmpVxCandidate==0)
        {
          if (myVxCandidate!=0) delete myVxCandidate;
          return 0;
        }
        const Trk::RecVertex& tempVtx(tmpVxCandidate->recVertex());
        int ndof = tempVtx.fitQuality().numberDoF();
        double tchi2 = tempVtx.fitQuality().chiSquared();
        double tempProb(-1.);
        if(ndof>0 && tchi2>=0.)
        {
          Genfun::CumulativeChiSquare myCumulativeChiSquare(ndof);
          tempProb = 1.-myCumulativeChiSquare(tchi2);
        }
        double chi2( (*(tmpVxCandidate->vxTrackAtVertex()))[trktracksInBUSV.size()-1]->trackQuality().chiSquared() );
        if ( (chi2 > m_buMaxChi2OfTrack) && (trktracksInBUSV.size() > 1) )
        {
          trktracksInBUSV.pop_back();
          delete tmpVxCandidate;
        }
        else
        {
          delete myVxCandidate;
          myVxCandidate=tmpVxCandidate;
          fitProb=tempProb;
        }
      }
      // find the TrackParticles which belong to the used track
      for (TrackVec2::iterator trkPrtItr = tracksInJet.begin();
           trkPrtItr != tracksInJet.end(); ++ trkPrtItr)
      {
        for (std::vector<const Trk::Track*>::iterator i=trktracksInBUSV.begin(); i!=trktracksInBUSV.end(); i++)
        {
          if ( *(*trkPrtItr)->track() == (*i) )
            tracksInSV.push_back((*trkPrtItr));
        }
      }
    }

    return myVxCandidate;
  }

  double SecVtxTag::m_massOfVertex(const TrackVec2& TPInVtx)
  {
    if (TPInVtx.size()<2)
      return -1.;
    double xMom(0.);
    double yMom(0.);
    double zMom(0.);
    double Energy(0.);
    for (TrackIter i=TPInVtx.begin(); i!=TPInVtx.end(); i++)
    {
      double x((*i)->perigeeParameters().momentum()[Trk::px]);
      double y((*i)->perigeeParameters().momentum()[Trk::py]);
      double z((*i)->perigeeParameters().momentum()[Trk::pz]);
      xMom += x; //std::cout << xMom << std::endl;
      yMom += y;
      zMom += z;
      Energy += sqrt(x*x + y*y + z*z);
    }

    CLHEP::HepLorentzVector final(xMom, yMom, zMom, Energy);
    return final.m(); // invMass m = sqrt( sum(E)2 - sum(vec_p)2 ), with E = vec_p.mag() ... hephy lim
  }

  void SecVtxTag::m_sortTracksInChi2(std::vector<int>& indexOfSortedChi2, const Trk::VxCandidate* myVxCandidate)
  {
    // we need an index vector here which tells us the right order from smallest to
    // largest of the chi2PerTrack vector
    // then loop over the index vector and replace all iRP with iRP = index[i]
    std::map<double, int> mapOfChi2;
    for (unsigned int i=0; i < myVxCandidate->vxTrackAtVertex()->size(); ++i)
    {
      /* possible bug 1 */
      double chi2PerTrack = (*(myVxCandidate->vxTrackAtVertex()))[i]->trackQuality().chiSquared();
      mapOfChi2.insert( std::map<double, int>::value_type( chi2PerTrack, i ) );
    }
    indexOfSortedChi2.clear();
    std::map<double, int>::const_iterator mItr = mapOfChi2.begin();
    for ( ; mItr != mapOfChi2.end() ; ++mItr )
    {
      indexOfSortedChi2.push_back( (*mItr).second );
    }
  }

  void SecVtxTag::m_printParameterSettings()
  {
    ATH_MSG_INFO(name() << " initialize(): Parametersettings ");
    ATH_MSG_INFO("I am in " << m_runModus << " modus.");
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

}

