/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"

#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignModuleList.h"
#include "TrkAlignEvent/AlignTrack.h"
#include "TrkAlignEvent/AlignPar.h"
#include "TrkAlignEvent/AlignVertex.h"

#include "TrkAlignInterfaces/IAlignModuleTool.h"

#include "TrkGlobalChi2AlignTools/GlobalChi2AlignTool.h"
#include "xAODEventInfo/EventInfo.h"

#include "TTree.h"
#include "TFile.h"
#include "TMatrixD.h"

namespace Trk {

  constexpr int MAXNCHAMBERS=50;
  constexpr int MAXNINDICES =50*6;

  //_______________________________________________________________________
  GlobalChi2AlignTool::GlobalChi2AlignTool(const std::string& type, const std::string& name,
					   const IInterface* parent)
    : AthAlgTool(type,name,parent)
    , m_matrixTool("Trk::MatrixTool",this)
    , m_alignModuleTool("Trk::AlignModuleTool/AlignModuleTool")
    , m_ntracks(0)
    , m_nmeas(0)
    , m_nhits(0)
    , m_chi2(0.)
    , m_nDoF(0)
    , m_ntuple(nullptr)
    , m_tree(nullptr)
    , m_run{}
    , m_event{}
    , m_materialOnTrack{}
    , m_momentum{}
    , m_nChambers(0)
    , m_chamberIds(new int[MAXNCHAMBERS])
    , m_nMatrixIndices(0)
    , m_matrixIndices(new int[MAXNINDICES])
    , m_nSecndDeriv(0)
    , m_secndDeriv(new double[MAXNINDICES*MAXNINDICES])
    , m_firstDeriv(new double[MAXNINDICES])
    , m_actualSecndDeriv(new double[MAXNINDICES])
    , m_eta{}
    , m_phi{}
    , m_perigee_x{}, m_perigee_y{}, m_perigee_z{}
    , m_trackInfo{}
    , m_bremFit{}
    , m_bremFitSuccessful{}
    , m_straightTrack{}
    , m_slimmedTrack{}
    , m_hardScatterOrKink{}
    , m_lowPtTrack{}
    , m_fromFiles(false)
  {      
    declareInterface<IAlignTool>(this);

    declareProperty("MatrixTool", m_matrixTool, "tool for storing and inverting matrix");
    declareProperty("AlignModuleTool", m_alignModuleTool);

    declareProperty("SecondDerivativeCut", m_secondDerivativeCut = -1e5);

    declareProperty("DoTree",              m_doTree = false);
    declareProperty("WriteActualSecDeriv", m_writeActualSecDeriv = false);

    declareProperty("StoreLocalDerivOnly", m_storeLocalDerivOnly = false);

    m_logStream = 0;
  }

  //_______________________________________________________________________
  GlobalChi2AlignTool::~GlobalChi2AlignTool()
  {
    ATH_MSG_DEBUG("in GlobalChi2AlignTool d'tor");
    //delete m_tree;

    delete [] m_chamberIds;
    delete [] m_matrixIndices;
    delete [] m_secndDeriv;
    delete [] m_firstDeriv;
    delete [] m_actualSecndDeriv;

    ATH_MSG_DEBUG("done with GlobalChi2AlignTool d'tor");
  }

  //_______________________________________________________________________
  StatusCode GlobalChi2AlignTool::initialize()
  {
    ATH_MSG_DEBUG("initialize() of GlobalChi2AlignTool");  
    // Get MatrixTool  
    ATH_CHECK( m_matrixTool.retrieve() );
    ATH_CHECK(m_alignModuleTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void GlobalChi2AlignTool::setLogStream(std::ostream * os)
  {
    m_logStream = os;
    m_matrixTool->setLogStream(m_logStream);
  }
  
  //________________________________________________________________________
  StatusCode GlobalChi2AlignTool::firstEventInitialize()
  {
    ATH_MSG_DEBUG("in GlobalGhi2AlignTool::firstEventInitialize()");
    
    int nDoF=m_alignModuleTool->nAlignParameters();
    ATH_MSG_DEBUG("allocating matrix with "<<nDoF<<" degrees of freedom");

    if( m_matrixTool->allocateMatrix(nDoF).isFailure()) {
      msg(MSG::FATAL)<<"Problem with allocateMatrix"<<endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("done with firstEventInitialize()");
    
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  bool GlobalChi2AlignTool::accumulate(const AlignTrack* alignTrack)
  {
    ATH_MSG_DEBUG("in GlobalChi2AlignTool::accumulate()");

    // prepare ROOT tree for derivatives if it doesn't exist yet
    // Shouldn't this be in firstEventInitialize() ?
    if (m_doTree && !m_tree && m_ntuple) {

      m_ntuple->cd();
      m_tree=new TTree("globalChi2Deriv","globalChi2Deriv");
      m_tree->Branch("run",              &m_run,              "run/I");
      m_tree->Branch("event",            &m_event,            "event/I");
      m_tree->Branch("nChambers",        &m_nChambers,        "nChambers/I");
      m_tree->Branch("chamberIds",        m_chamberIds,       "chamberIds[nChambers]/I");
      m_tree->Branch("nMatrixIndices",   &m_nMatrixIndices,   "nMatrixIndices/I");
      m_tree->Branch("matrixIndices",     m_matrixIndices,    "matrixIndices[nMatrixIndices]/I");
      m_tree->Branch("nSecndDeriv",      &m_nSecndDeriv,       "nSecndDeriv/I");
      m_tree->Branch("secndDeriv",        m_secndDeriv,        "secndDeriv[nSecndDeriv]/D");
      m_tree->Branch("firstDeriv",        m_firstDeriv,       "firstDeriv[nMatrixIndices]/D");
      if (m_writeActualSecDeriv)
	m_tree->Branch("actualSecndDeriv",  m_actualSecndDeriv,  "actualSecndDeriv[nMatrixIndices]/D");

      m_tree->Branch("materialOnTrack",  &m_materialOnTrack,  "materialOnTrack/D");
      m_tree->Branch("momentum",         &m_momentum,         "momentum/D");
      m_tree->Branch("eta",              &m_eta,              "eta/D");
      m_tree->Branch("phi",              &m_phi,              "phi/D");
      m_tree->Branch("perigee_x",        &m_perigee_x,        "perigee_x/D");
      m_tree->Branch("perigee_y",        &m_perigee_y,        "perigee_y/D");
      m_tree->Branch("perigee_z",        &m_perigee_z,        "perigee_z/D");
      m_tree->Branch("trackInfo",         &m_trackInfo,          "trackInfo/I");
      m_tree->Branch("bremFit",           &m_bremFit,            "bremFit/I");           
      m_tree->Branch("bremFitSuccessful", &m_bremFitSuccessful,  "bremFitSuccessful/I"); 
      m_tree->Branch("straightTrack",     &m_straightTrack,      "straightTrack/I");     
      m_tree->Branch("slimmedTrack",      &m_slimmedTrack,       "slimmedTrack/I");      
      m_tree->Branch("hardScatterOrKink", &m_hardScatterOrKink,  "hardScatterOrKink/I"); 
      m_tree->Branch("lowPtTrack",        &m_lowPtTrack,         "lowPtTrack/I");        
    }


    // check if the track belongs to a vertex. If yes, retrieve the relevant objects:
    bool                                  fullVertex   = false;
    AlignVertex                         * ptrVertex    = alignTrack->getVtx();
    Amg::Vector3D                       * ptrPosition  = 0;
    AmgSymMatrix(3)                     * ptrCovariance = 0;
    std::vector<AlignModuleVertexDerivatives>   * ptrX = 0;
    AlignVertex::AlignVertexType          vtxType      = AlignVertex::Unknown;
    if( ptrVertex )  {
      fullVertex       = true;
      ptrPosition      = ptrVertex->position();
      ptrCovariance    = ptrVertex->covariance();
      ptrX             = ptrVertex->derivatives();
      vtxType          = ptrVertex->type();

      // check if pointers are valid
      if ( !ptrPosition || !ptrCovariance || !ptrX || vtxType<AlignVertex::Refitted ) {
        msg(MSG::ERROR)<<"something missing from alignVertex!"<<endmsg;
        if (!ptrPosition)     msg(MSG::ERROR)<<"no fitted position!"<<endmsg;
        if (!ptrCovariance)   msg(MSG::ERROR)<<"no covariance!"<<endmsg;
        if (!ptrX)            msg(MSG::ERROR)<<"no link to the X object!"<<endmsg;
        if (vtxType<AlignVertex::Refitted) msg(MSG::ERROR)<<"Vertex type = "<< vtxType << endmsg;
        return false;
      }
    }



    // reset tree variables
    m_nChambers=0;
    m_nMatrixIndices=0;

    // get pointers so we can reuse them if they're valid
    Amg::SymMatrixX                 * ptrWeights   = alignTrack->weightMatrix();
    Amg::SymMatrixX                 * ptrWeightsFD = alignTrack->weightMatrixFirstDeriv();
    Amg::VectorX                    * ptrResiduals = alignTrack->residualVector();
    std::vector<AlignModuleDerivatives> * ptrDerivs    = alignTrack->derivatives();

    // check if pointers are valid
    if (!ptrWeights || !ptrWeightsFD || !ptrResiduals || !ptrDerivs) {
      msg(MSG::ERROR)<<"something missing from alignTrack!"<<endmsg;
      if (!ptrWeights)   msg(MSG::ERROR)<<"no weights!"<<endmsg;
      if (!ptrWeightsFD) msg(MSG::ERROR)<<"no weights for first deriv!"<<endmsg;
      if (!ptrResiduals) msg(MSG::ERROR)<<"no residuals!"<<endmsg;
      if (!ptrDerivs)    msg(MSG::ERROR)<<"no derivatives!"<<endmsg;
      return false;
    }

    // get weight matrices
    Amg::SymMatrixX&                weights           = *ptrWeights;
    Amg::SymMatrixX&                weightsFirstDeriv = *ptrWeightsFD;
    ATH_MSG_VERBOSE("weights="<<weights);
    ATH_MSG_VERBOSE("weightsFirstDeriv="<<weightsFirstDeriv);

    // get vectors
    Amg::VectorX&                        residuals     = *ptrResiduals;
    std::vector<AlignModuleDerivatives>  derivatives   = *ptrDerivs;
    std::vector<AlignModuleDerivatives>* derivativeErr = alignTrack->derivativeErr();
    std::vector<std::pair<const AlignModule*,std::vector<double> > >* secDerivs = alignTrack->actualSecondDerivatives();



    std::vector<AlignPar*> allAlignPars;
    std::vector<Amg::VectorX*> allDerivatives;
    std::vector<Amg::VectorX*> allDerivativeErr;
    std::vector<double> allActualSecDeriv;
    int    WSize(weights.cols());
    std::unique_ptr<Amg::MatrixX>  matrix_F( new Amg::MatrixX(3,WSize) );
    matrix_F->setZero();

    // get all alignPars and all derivatives
    msg(MSG::DEBUG) << "accumulate: The derivative vector size is  " << derivatives.size() << endmsg;
    std::vector<AlignModuleDerivatives>::iterator derivIt     = derivatives.begin();
    std::vector<AlignModuleDerivatives>::iterator derivIt_end = derivatives.end();

    std::vector<AlignModuleDerivatives>::iterator derivErrIt;
    std::vector<std::pair<const AlignModule*,std::vector<double> > >::iterator secDerivIt;
    if (derivativeErr) derivErrIt=derivativeErr->begin();
    if (secDerivs)     secDerivIt=secDerivs->begin();
    for ( ; derivIt!=derivIt_end ; ++derivIt) {

      // get AlignModule
      const AlignModule* module=derivIt->first;

      // increment track counter
      module->addTrack();
      ATH_MSG_DEBUG("add track to module "<<module->identify().get_identifier32().get_compact());
      module->addNDoF(alignTrack->fitQuality()->numberDoF());
      module->addTrackChi2(alignTrack->fitQuality()->chiSquared());

      // tree variables
      if (m_nChambers<MAXNCHAMBERS) {
        m_chamberIds[m_nChambers]=module->identify().get_identifier32().get_compact();
        m_nChambers++;
      }
      else ATH_MSG_WARNING("number of chambers exceeded maximum");

      // get alignment parameters
      std::vector<Amg::VectorX>& deriv_vec = derivIt->second;
      ATH_MSG_DEBUG( "accumulate: The deriv_vec size is  " << deriv_vec.size() );
      DataVector<AlignPar>* alignPars = m_alignModuleTool->getAlignPars(module);
      int nModPars = alignPars->size();
      if (nModPars != (int)deriv_vec.size() && (nModPars+3) != (int)deriv_vec.size()) {
        ATH_MSG_ERROR("accumulate:  wrong size of the deriv_vec!");
        return false;
      }
      for (int i=0;i<3;i++) {
        for (int j=0;j<WSize;j++) {
          (*matrix_F)(i,j) += deriv_vec[nModPars+i][j];        // in the derivIT loop the complete F matrix is built
        }
      }

      // get derivatives and derivative errors
      for (int i=0;i<nModPars;i++) {
        ATH_MSG_DEBUG("align par for module "<<(*alignPars)[i]->alignModule()->identify());
        allAlignPars.push_back((*alignPars)[i]);      
        allDerivatives.push_back(&deriv_vec[i]);
        if (derivativeErr)
          allDerivativeErr.push_back(&(derivErrIt->second)[i]);
        if (secDerivs)
          allActualSecDeriv.push_back((secDerivIt->second)[i]);
      }
      if (derivativeErr) ++derivErrIt;
      if (secDerivs) ++secDerivIt;
    }

    // increment hit counters in modules
    AlignTSOSCollection::const_iterator iatsos      = alignTrack->firstAtsos();
    AlignTSOSCollection::const_iterator iatsos_last = alignTrack->lastAtsos();
    for (; iatsos != iatsos_last; ++iatsos)
      if ((*iatsos)->module())
        (*iatsos)->module()->addHit();

    // determine all derivatives and set in matrix tool
    int nAllPars = allAlignPars.size();
    m_nSecndDeriv=0;
    for (int ipar=0;ipar<nAllPars;ipar++) {

      // calculate first derivative
      Amg::MatrixX derivativesT = (*allDerivatives[ipar]).transpose();
      ATH_MSG_DEBUG("derivativesT (size "<<derivativesT.cols()<<"): "<<derivativesT);

      Amg::MatrixX RHM= weightsFirstDeriv * residuals;
      ATH_MSG_DEBUG("RHM: "<<RHM);

      // get global parameter index
      int index1 = allAlignPars[ipar]->index();

      Amg::MatrixX firstderivM = 2.* derivativesT * RHM;
      ATH_MSG_DEBUG("firstderivM for i="<<index1 <<firstderivM);
      if (firstderivM(0,0)==0.) {
        ATH_MSG_DEBUG("firstderivM[0][0] is zero : "<<firstderivM(0,0));
        continue;
      }



      // tree variables
      if (m_nChambers<MAXNCHAMBERS) {
        m_matrixIndices[m_nMatrixIndices]=index1;
        ATH_MSG_DEBUG("ipar: "<<ipar<<", m_nMatrixIndices="<<m_nMatrixIndices);
        if (m_writeActualSecDeriv) {
          ATH_MSG_DEBUG("alActualSecDeriv size: "<<allActualSecDeriv.size());
          m_actualSecndDeriv[m_nMatrixIndices]=allActualSecDeriv[ipar];
        }
        m_nMatrixIndices++;
      }

      // get module index
      int imodule1 = allAlignPars[ipar]->alignModule()->identifyHash();

      // calculate second derivatives
      bool goodSecondDeriv = true;
      for (int jpar=ipar;jpar<nAllPars;jpar++) {

        // get global parameter index
        int index2=allAlignPars[jpar]->index();
	
        // get module index
        int imodule2 = allAlignPars[jpar]->alignModule()->identifyHash();

        // if only local Chi2 method is used, the 2nd derivatives (correlations)
        // between different modules don't need to be calculated/stored
        if (m_storeLocalDerivOnly)
          if (imodule1 != imodule2)
            continue;

        Amg::MatrixX RHM2 = weights * (*allDerivatives[jpar]);
        Amg::MatrixX secondderivM = 2. * derivativesT * RHM2;

        // if second derivative on diagonal is below cut
        // ignore the whole row/column
        if (jpar==ipar && secondderivM(0,0)<0.) {
          ATH_MSG_WARNING("second deriv < 0 !!!");
        }
        if (jpar==ipar && secondderivM(0,0)<m_secondDerivativeCut) {
          ATH_MSG_DEBUG("secondderivM[0][0] is below threshold : "<<
                        secondderivM(0,0)<<" < "<<m_secondDerivativeCut);
          goodSecondDeriv = false;
          break;
        }
	
        ATH_MSG_DEBUG("secondderivM for i "<<index1<<", j "<<index2<<": "<<secondderivM(0,0));
        m_matrixTool->addSecondDerivative(index1, index2, secondderivM(0,0));

        // store derivatives and indices for ntuple
        m_secndDeriv[m_nSecndDeriv]=secondderivM(0,0);
        m_nSecndDeriv++;
      }
      
      // if second derivative on diagonal didn't pass the cut,
      // ignore also the first derivative
      if (goodSecondDeriv) {
        m_matrixTool->addFirstDerivative(index1, firstderivM(0,0));     
        m_firstDeriv[ipar]=firstderivM(0,0);
      }
      else 
        m_firstDeriv[ipar]=-999.;    
    }
    
    

    
    if( fullVertex )  {              //  this track is associated to a vertex.


      ATH_MSG_DEBUG( "accumulate: Contribution from the fullVTX will be added " );
     
      Amg::MatrixX RHM = (*ptrCovariance) * (*matrix_F) * (weightsFirstDeriv * residuals);

      std::vector<AlignPar*>         vtxAlignPars;
      std::vector<Amg::VectorX*>     vtxDerivatives;

      // get all alignPars and all derivatives
      msg(MSG::DEBUG) << "accumulate: The Vertex derivative vector size is  " << ptrX->size() << endmsg;
      std::vector<AlignModuleVertexDerivatives>::iterator XIt     = ptrX->begin();
      std::vector<AlignModuleVertexDerivatives>::iterator XIt_end = ptrX->end();

      for ( ; XIt!=XIt_end ; ++XIt) {

        // get AlignModule
        const AlignModule* module=XIt->first;

        // get alignment parameters
        std::vector<Amg::VectorX>& X_vec = XIt->second;
        msg(MSG::DEBUG) << "accumulate: The X_vec size is  " << X_vec.size() << endmsg;
        DataVector<AlignPar>* alignPars = m_alignModuleTool->getAlignPars(module);
        int nModPars = alignPars->size();
        if (nModPars != (int)X_vec.size()) {
          ATH_MSG_ERROR("accumulate:  wrong size of the X_vec!");
          return false;
        }

        // get derivatives and derivative errors
        for (int i=0;i<nModPars;i++) {
          ATH_MSG_DEBUG("align par for module "<<(*alignPars)[i]->alignModule()->identify());
          vtxAlignPars.push_back((*alignPars)[i]);      
          vtxDerivatives.push_back(&X_vec[i]);
	      }

      }

      // bit responsible for the constraint:
      int           ierr(0);
      AmgSymMatrix(3)  Qinv;
      Qinv.setZero();
      Amg::Vector3D    vtemp;
      vtemp.setZero();
      if( vtxType==AlignVertex::Refitted && ptrVertex->Constrained()  && ptrVertex->Qmatrix()->determinant() > 1.0e-24 )   {     // just my guess sigma>0.1 micron ???)    only once per vertex!
    
        bool invertible;
        double determinant;
        ptrVertex->Qmatrix()->computeInverseAndDetWithCheck(Qinv,determinant,invertible);
        
        if(!invertible) {
          std::cout <<"accumulate: Q inversion failed. CLHEP status flag = "<<ierr<< std::endl;
          return false;
        }
        vtemp(1) =   -ptrVertex->originalPosition()->x()+(*(ptrVertex->Vvector()))(1);
        vtemp(2) =   -ptrVertex->originalPosition()->y()+(*(ptrVertex->Vvector()))(2);
        vtemp(3) =   -ptrVertex->originalPosition()->z()+(*(ptrVertex->Vvector()))(3);
      }


      // determine all derivatives and set in matrix tool
      int nvtxPars = vtxAlignPars.size();
      for (int ipar=0;ipar<nvtxPars;ipar++) {

        // calculate first derivative
        Amg::MatrixX derivativesT = (*vtxDerivatives[ipar]).transpose();        
        Amg::MatrixX firstderivM = -2.* derivativesT * RHM;     //  -2 seems correct :)

        // bit responsible for the constraint:
        if( vtxType==AlignVertex::Refitted && ptrVertex->Constrained()  && ptrVertex->Qmatrix()->determinant() > 1.0e-24 )   {     // just my guess sigma>0.1 micron ???)   only once per vertex!
                firstderivM  += 2.* derivativesT * ((*ptrCovariance) * Qinv) * vtemp; 
        }

        // get global parameter index
        int index1 = vtxAlignPars[ipar]->index();

        ATH_MSG_DEBUG("vtx firstderivM for i="<<index1 <<firstderivM);


        if( vtxType==AlignVertex::Refitted ) {                       // only once per vertex!
          ATH_MSG_DEBUG("accumulate: Filling second derivative for this vertex... ");
          for (int jpar=0;jpar<nvtxPars;jpar++) {

            // calculate second derivative
   	        Amg::MatrixX secondderivM = -1.* derivativesT * (*ptrCovariance) * (*vtxDerivatives[jpar]);   //  -1 is the right factor  :)

            // bit responsible for the constraint:
            if( ptrVertex->Constrained()  && ptrVertex->Qmatrix()->determinant() > 1.0e-24 )   {     // just my guess sigma>0.1 micron ???)
              secondderivM += 2.* derivativesT * ((*ptrCovariance) * Qinv * (*ptrCovariance)) * (*vtxDerivatives[jpar]);
            }

            // get global parameter index
            int index2=vtxAlignPars[jpar]->index();

            if( index1<=index2 ) {
              ATH_MSG_DEBUG("vtx secondderivM for i="<<index1<<", j="<<index2<<": "<<secondderivM(0,0));
              m_matrixTool->addSecondDerivative(index1, index2, secondderivM(0,0));
            }

          }
          ptrVertex->setType(AlignVertex::Accumulated);              // only once per vertex!
        }

        m_matrixTool->addFirstDerivative(index1, firstderivM(0,0));     

      }

    }
    




    // fill tree
    if (m_tree) {      
      
      // get tree variables
      const xAOD::EventInfo* currentEvent;
      StatusCode sc = evtStore()->retrieve(currentEvent);
      if (sc==StatusCode::SUCCESS) {
	      m_run   = currentEvent->runNumber();
	      m_event = currentEvent->eventNumber();
      }
      else {
        m_run   = -999;
        m_event = -999;
      } 

      m_materialOnTrack=getMaterialOnTrack(alignTrack->originalTrack());
      ATH_MSG_DEBUG("materialOnTrack: "<<m_materialOnTrack);

      double pT=alignTrack->originalTrack()->perigeeParameters()->pT();
      double eta=alignTrack->originalTrack()->perigeeParameters()->eta();    
      m_momentum=std::fabs(pT)*std::sqrt(1.+sinh(eta)*sinh(eta));
      m_eta=eta;
      m_phi=alignTrack->originalTrack()->perigeeParameters()->momentum().phi();

      Amg::Vector3D pos=alignTrack->originalTrack()->perigeeParameters()->position();
      m_perigee_x=pos.x();
      m_perigee_y=pos.y();
      m_perigee_z=pos.z();
      
      const TrackInfo info=alignTrack->originalTrack()->info();
      m_trackInfo         = (int)info.trackFitter();
      m_bremFit           = (int)info.trackProperties(Trk::TrackInfo::BremFit);         
      m_bremFitSuccessful = (int)info.trackProperties(Trk::TrackInfo::BremFitSuccessful); 
      m_straightTrack     = (int)info.trackProperties(Trk::TrackInfo::StraightTrack);     
      m_slimmedTrack      = (int)info.trackProperties(Trk::TrackInfo::SlimmedTrack);       
      m_hardScatterOrKink = (int)info.trackProperties(Trk::TrackInfo::HardScatterOrKink); 
      m_lowPtTrack        = (int)info.trackProperties(Trk::TrackInfo::LowPtTrack);    
      
      m_ntuple->cd();
      m_tree->Fill();
    }	      
    
    // increment total number of accumulated tracks, hits and the total chi2
    m_ntracks++;
    m_nmeas += alignTrack->nAlignTSOSMeas();
    m_nhits += alignTrack->alignTSOSCollection()->size();
    m_chi2  += alignTrack->fitQuality()->chiSquared();
    m_nDoF  += alignTrack->fitQuality()->numberDoF();

    if(m_ntracks%50==0)
      ATH_MSG_INFO(">>>> Number of accumulated tracks: "<<m_ntracks);

    return true;
  }

  //________________________________________________________________________
  bool GlobalChi2AlignTool::accumulateFromFiles()
  {
    ATH_MSG_DEBUG("in accumulate");
    bool success = m_matrixTool->accumulateFromFiles();

    if (!success) {
      ATH_MSG_FATAL("failure in MatrixTool accumulateFromFiles");
      return StatusCode::FAILURE;
    }

    m_fromFiles = true;

    return StatusCode::SUCCESS;
  }  

  //________________________________________________________________________
  StatusCode GlobalChi2AlignTool::solve()
  {
    if(m_fromFiles)
      m_nhits = m_matrixTool->nHits();
    else {
      m_matrixTool->setNTracks(m_ntracks);
      m_matrixTool->setNMeasurements(m_nmeas);
      m_matrixTool->setNHits(m_nhits);
    }

    // print statistics
    if(m_logStream) {
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"******          Global Chi2 alignment solving          ******"<<std::endl;
      *m_logStream<<"*"<<std::endl;
      if(!m_fromFiles)
        *m_logStream<<"* number of accumulated tracks:   "<<m_ntracks<<std::endl;
      *m_logStream<<"* number of processed hits:       "<<m_nhits<<std::endl;
      if(!m_fromFiles) {
        *m_logStream<<"* number of used measurements:    "<<m_nmeas<<std::endl;
        *m_logStream<<"* total chi2 / nDoF:              "<<m_chi2<<" / "<<m_nDoF<<std::endl;
      }
      *m_logStream<<"*"<<std::endl;
      *m_logStream<<"* number of alignable objects:    "<<m_alignModuleTool->alignModules1D()->size()<<std::endl;
      *m_logStream<<"* total number of alignment DoFs: "<<m_alignModuleTool->nAlignParameters()<<std::endl;
      *m_logStream<<"*"<<std::endl;
    }

    ATH_MSG_INFO(">>>> -----------------------------------------------");
    ATH_MSG_INFO(">>>> Total number of accumulated tracks: "<<m_ntracks);
    ATH_MSG_INFO(">>>> -----------------------------------------------");  

    ATH_MSG_DEBUG("calling matrixTool->solve()");
    if (m_matrixTool->solve()>0)
      return StatusCode::SUCCESS;

    return  StatusCode::FAILURE;
  }

  //________________________________________________________________________
  StatusCode GlobalChi2AlignTool::fillNtuple()
  {
    ATH_MSG_DEBUG("writing tree");
    int success=1;
    if (m_tree) {
      m_ntuple->cd();
      success = m_tree->Write();
    }
    return success>0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }

  //_______________________________________________________________________
  StatusCode GlobalChi2AlignTool::finalize()
  {
    ATH_MSG_DEBUG("finalize() of GlobalChi2AlignTool");
    return StatusCode::SUCCESS;    
  }

  //________________________________________________________________________
  double GlobalChi2AlignTool::getMaterialOnTrack(const Trk::Track* track)
  {

    double materialOnTrack(0.);
    
    const DataVector<const Trk::TrackStateOnSurface>* states = track->trackStateOnSurfaces();
    if ( !states ) {
      ATH_MSG_WARNING("no states");
      return 0;
    }
    
    // loop over TSOSs, find MDT hits, and mark them as outliers:
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    ATH_MSG_DEBUG("looping over TSOS");

    for ( ; tsit!=tsit_end ; ++tsit ) {
      const Trk:: MaterialEffectsBase* materialEffects =  (*tsit)->materialEffectsOnTrack();
      if (materialEffects) {
        const Trk::MaterialEffectsOnTrack* meot=dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);
        if (meot) {
          materialOnTrack += meot->thicknessInX0();
          ATH_MSG_DEBUG("new materialOnTrack: "<<materialOnTrack);
        }
        else {
          ATH_MSG_DEBUG("not material on track");
        }      
      }      
    }
    
    return materialOnTrack;
  }

} // end of namespace
    
