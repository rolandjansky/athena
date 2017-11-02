/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// FTK_VertexFinder tool
// -------------------------------
// ATLAS Collaboration
//
// Return the fitted vertex
//
// 10.08.2015 Package created
//
// Author: Rui Wang, ANL
// e-mail: Rui.Wang@cern.ch
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <vector>
#include "TVector3.h"
#include "GaudiKernel/MsgStream.h"

#include "FTK_RecTools/FTK_VertexFinderTool.h"
#include "FTK_DataProviderInterfaces/IFTK_UncertaintyTool.h"
#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "TrkVxEdmCnv/IVxCandidateXAODVertex.h"
#include <map>
#include <vector>
#include <utility>
//#include "TrkSurfaces/PlaneSurface.h"

using std::map; using std::string;
using std::cout; using std::endl;
using std::vector; using std::iterator;
using Trk::RecVertex;using Trk::VxTrackAtVertex;

FTK_VertexFinderTool::FTK_VertexFinderTool(const std::string& t,
                                               const std::string& n,
                                               const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_barrelOnly(false),
  m_hasIBL(false),
  m_cluster_size(1.87),
  m_chi2cut(5.),
  m_constTrkPt(1.),
  m_constTrkEta(1.1),
  m_z0errfactor(1.0),
  m_uncertaintyTool("FTK_UncertaintyTool",this),
  m_VertexEdmFactory("VertexInternalEdmFactory",this)
{
  declareInterface< IFTK_VertexFinderTool >( this );
  declareProperty( "HasIBL",  m_hasIBL);
  declareProperty( "BarrelOnly",  m_barrelOnly);
  declareProperty( "Cluster_size",  m_cluster_size);
  declareProperty( "Chi2cut",  m_chi2cut);
  declareProperty( "ConstTrkPt",  m_constTrkPt);
  declareProperty( "ConstTrkEta",  m_constTrkEta);
  declareProperty("UncertaintyTool",m_uncertaintyTool);
  declareProperty("VertexInternalEdmFactory",m_VertexEdmFactory);
  declareProperty( "z0ErrFactor", m_z0errfactor);
}

StatusCode FTK_VertexFinderTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  if ( m_VertexEdmFactory.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Failed to retrievel tool " << m_VertexEdmFactory);
    return StatusCode::FAILURE;
  }

  athenaLog << MSG::DEBUG << "FTK_VertexFinderTool initialized "<< endmsg;
  return sc;
}

StatusCode FTK_VertexFinderTool::finalize() {
  StatusCode sc = AlgTool::finalize();
  return sc;
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> FTK_VertexFinderTool::findVertex(const FTK_RawTrackContainer* trks)
{
  vector<MyTrack> mytrk;
  mytrk=getTracks(trks);
  return findVertex(mytrk);
}

std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> FTK_VertexFinderTool::findVertex(const TrackCollection* trks)
{
  vector<MyTrack> mytrk;
  mytrk=getTracks(trks);
  return findVertex(mytrk);
}
//
// Covariance Matrix if there is a BLayer Hit
//
std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> FTK_VertexFinderTool::findVertex(std::vector<MyTrack>& mytrk)
{
  MsgStream athenaLog(msgSvc(), name());
  double mGkx=0.0001;
  double mGky=0.0001;
  double mGkz=16.;
  athenaLog << MSG::DEBUG << "FTK_VertexFinderTool:: barrel " << m_barrelOnly<< endmsg;
  xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
  theVertexContainer->setStore( theVertexAuxContainer );
  vector<MyTrack>::iterator trkbegin;
  vector<MyTrack>::iterator trkend;
  vector<MyTrack> trkatvtx;

  athenaLog << MSG::DEBUG << "FTK_VertexFinderTool:: total " << mytrk.size()<< " tracks "<< endmsg;
  sort(mytrk.begin(),mytrk.end());//sorting mytrk by pt(Track7.h)

  int numdo=0;
  do{
    athenaLog << MSG::DEBUG << "findVertex: finding vertex "<< endmsg;
    //////////////////////
    //clustering
    //////////////////////
    numdo++;
    vector<MyTrack> vxtrk;
    //high pt track selecting
    trkbegin=mytrk.begin();
    trkend=mytrk.end();
    float seed_z=(*trkbegin).m_z0;

    //selecting tracks near high pt track
    for(vector<MyTrack>::iterator i=trkbegin ; i<trkend;){
            if(fabs((*i).m_z0 - seed_z) < m_cluster_size){
              vxtrk.push_back(*i);
              i=mytrk.erase(i);
            }
            else{
              i++;
            }
            trkbegin=mytrk.begin();
            trkend=mytrk.end();
    }

    //calculating seed z position(z average weighted by z error)
    double oldz=0;
    double z_weight=0;
    trkbegin=vxtrk.begin();
    trkend=vxtrk.end();
    for(vector<MyTrack>::iterator i=trkbegin ; i<trkend;i++){
            oldz += (*i).m_z0*(*i).m_z0err;
      z_weight += (*i).m_z0err;
    }
    oldz /= z_weight;

    double oldx=0;
    double oldy=0;
    double Gk[3][3]={{mGkx,0.,0.},{0.,mGky,0.},{0.,0.,mGkz}};//error
    Amg::MatrixX C22_mat(3,3);
    double chi2=0;

    trkbegin=vxtrk.begin();
    trkend=vxtrk.end();
    int trknumber=0;
    athenaLog << MSG::DEBUG << "FTK_VertexFinderTool:: fit VTX, using "<<vxtrk.size()<<" trks."<< endmsg;
    for(  vector<MyTrack>::iterator i=trkbegin ; i<trkend ;){
            trknumber++;
            double tmpchi2 = chi2;
            athenaLog << MSG::VERBOSE << "getTracks: "<<trknumber<<", pt "<<(*i).m_pt<<", eta "<<(*i).m_theta<<", phi "<<(*i).m_phi<<", d0 "<<(*i).m_d0<<", z0 "<<(*i).m_z0<<", pt err "<<(*i).m_pterr<<", theta err "<<(*i).m_thetaerr<<", phi err "<< (*i).m_phierr<<", d0 err"<< (*i).m_d0err<<", z0 err "<<(*i).m_z0err<< endmsg;
            //track parameter reading
            double xv,yv,zv,P0,phi0,theta0;
            TVector3 trk;
            xv=oldx;
            yv=oldy;
            zv=oldz;
            P0=(*i).m_pt;
            phi0=(*i).m_phi;
            theta0=(*i).m_theta;

            //parameter of angle

            double cosPhi0,sinPhi0,sinPsi,cosPsi;
            double psi,ctt,sint;
            double alpha=0.02997*20.84;///1000.0;

            cosPhi0=cos(phi0);sinPhi0=sin(phi0);
            sinPsi=-alpha*(xv*cosPhi0+yv*sinPhi0)/P0;
            cosPsi=sqrt(1.0-sinPsi*sinPsi);
            psi=asin(sinPsi);
            sint=sin(theta0);
            ctt=cos(theta0)/sint;

            //difference of values expected and observed

            double A[2][3],B[2][3],u[2],h[2],resid[2];

            A[0][0]=-sin(phi0+psi)/cosPsi;
            A[0][1]= cos(phi0+psi)/cosPsi;
            A[0][2]=0.0;

            A[1][0]=-ctt*cosPhi0/cosPsi;
            A[1][1]=-ctt*sinPhi0/cosPsi;
            A[1][2]=1.0;

            B[0][0]=-xv*A[0][1]+yv*A[0][0];
            B[0][1]=0.0;
            B[0][2]=(1.0-1.0/cosPsi)/alpha;

            B[1][0]=-xv*A[1][1]+yv*A[1][0];
            B[1][1]=-P0*psi/(alpha*sint*sint);
            B[1][2]=ctt*(psi-sinPsi/cosPsi)/alpha;

            u[0]=(*i).m_d0;
            u[1]=(*i).m_z0;

            h[0]=yv*cosPhi0-xv*sinPhi0+P0*(1-cosPsi)/alpha;
            h[1]=zv+P0*ctt*psi/alpha;

            resid[0]=u[0]-h[0];
            resid[1]=u[1]-h[1];

            //error of difference

            double Vqq[3][3]={{(*i).m_phierr*(*i).m_phierr,0.,0.},{0.,(*i).m_thetaerr*(*i).m_thetaerr,0.},{0.,0.,(*i).m_pterr*(*i).m_pterr}};
            double Vuu[2][2]={{(*i).m_d0err*(*i).m_d0err,0.},{0.,(*i).m_z0err*(*i).m_z0err}};
            double AC[2][3],BV[2][3],Sk[2][2];

            for(int i1=0;i1<2;i1++) for(int j=0;j<2;j++) Sk[i1][j]=Vuu[i1][j];
            for(int i1=0;i1<2;i1++) for(int j=0;j<3;j++)
                                      {
                                        AC[i1][j]=0.0;
                                        for(int k=0;k<3;k++) AC[i1][j]+=A[i1][k]*Gk[j][k];
                                      }
            for(int i1=0;i1<2;i1++) for(int j=0;j<2;j++)
                                      {
                                        for(int k=0;k<3;k++) Sk[i1][j]+=AC[i1][k]*A[j][k];
                                      }
            for(int i1=0;i1<2;i1++)
              for(int j=0;j<3;j++)
                {
                  BV[i1][j]=0.0;
                  for(int k=0;k<3;k++) BV[i1][j]+=B[i1][k]*Vqq[k][j];
                }
            for(int i1=0;i1<2;i1++)
              for(int j=0;j<2;j++)
                {
                  for(int k=0;k<3;k++) Sk[i1][j]+=BV[i1][k]*B[j][k];
                }

            //error determinant
            double detr,V[2][2];

            detr=1.0/(Sk[0][0]*Sk[1][1]-Sk[0][1]*Sk[1][0]);
             V[0][0]=Sk[1][1]*detr;
            V[1][1]=Sk[0][0]*detr;
            V[0][1]=V[1][0]=-Sk[0][1]*detr;

            //chi2
            chi2=V[0][0]*resid[0]*resid[0]+V[1][1]*resid[1]*resid[1]+2.0*V[0][1]*resid[1]*resid[0];
            if(chi2>m_chi2cut || chi2<0){
              i=vxtrk.erase(i);
              chi2 = tmpchi2;
            }
            else{
              trkatvtx.push_back(*i);
              i++;
            }
            trkbegin=vxtrk.begin();
            trkend=vxtrk.end();
            //vertex position & covariance update
            double K[2][3],dV[3];

            for(int i2=0;i2<2;i2++){
              for(int j=0;j<3;j++){
                for(int k=0;k<2;k++)K[i2][j]=AC[k][j]*V[k][i2];
              }
            }

            for(int i2=0;i2<3;i2++){
              dV[i2]=K[0][i2]*resid[0]+K[1][i2]*resid[1];
              for(int j2=i2;j2<3;j2++){
                Gk[i2][j2]-=K[0][i2]*AC[0][j2]+K[1][i2]*AC[1][j2];
                Gk[j2][i2]=Gk[i2][j2];
              }
            }
            oldx+=dV[0];
            oldy+=dV[1];
            oldz+=dV[2];
            C22_mat(0,0)=Gk[0][0];C22_mat(0,1)=Gk[0][1];C22_mat(0,2)=Gk[0][0];
            C22_mat(1,0)=Gk[1][0];C22_mat(1,1)=Gk[1][1];C22_mat(1,2)=Gk[1][2];
            C22_mat(2,0)=Gk[2][0];C22_mat(2,1)=Gk[2][1];C22_mat(2,2)=Gk[2][2];
    }//track loop end
    if (vxtrk.size()==0)continue;
    athenaLog << MSG::DEBUG << "findVertex: find vertex at "<< oldx<<";"<<oldy<<";"<<oldz<< endmsg;
    Amg::Vector3D frameOrigin(oldx,oldy,oldz);
    int ndf = 2*vxtrk.size()-3;
    xAOD::Vertex* vx = new xAOD::Vertex;
    vx->makePrivateStore();
    vx->setPosition (frameOrigin);
    vx->setCovariancePosition (C22_mat);
    vx->setFitQuality(chi2,static_cast<float>(ndf));
    vx->setVertexType(xAOD::VxType::PriVtx);

    std::vector<VxTrackAtVertex> & tracksAtVertex = vx->vxTrackAtVertex(); tracksAtVertex.clear();
    // Store the tracks at vertex
    Amg::Vector3D Vertex(frameOrigin[0],frameOrigin[1],frameOrigin[2]);
    const Trk::PerigeeSurface Surface(Vertex);
    Trk::Perigee * refittedPerigee(0);
    vector<MyTrack>::iterator i;
    for (i = trkatvtx.begin(); i != trkatvtx.end() ; ++i)
    {
      AmgSymMatrix(5) * CovMtxP = new AmgSymMatrix(5);
      double eta = ctheta2eta((*i).m_theta);
      double etaerr=(*i).m_thetaerr*cosh(eta);
      double qoverperr=cosh(eta)? 2./cosh(eta) * sqrt(4*(*i).m_pterr*(*i).m_pterr + 1/4/(*i).m_pt/(*i).m_pt*tanh(eta)*tanh(eta)*etaerr*etaerr) : 10;
      (*CovMtxP)(0,0)=(*i).m_d0err*(*i).m_d0err;
      (*CovMtxP)(0,0)=(*i).m_z0err*(*i).m_z0err;
      (*CovMtxP)(0,0)=(*i).m_phierr*(*i).m_phierr;
      (*CovMtxP)(0,0)=(*i).m_thetaerr*(*i).m_thetaerr;
      (*CovMtxP)(0,0)=qoverperr*qoverperr;
      double qoverp = 2/(*i).m_pt/cosh(eta);
      refittedPerigee = new Trk::Perigee ((*i).m_d0,(*i).m_z0,(*i).m_phi,(*i).m_theta,qoverp, Surface, CovMtxP);
      tracksAtVertex.emplace_back(-1, refittedPerigee);
    }
/*
    Trk::VxCandidate *  fittedVxCandidate = new Trk::VxCandidate(*fittedVertex, *tracksAtVertex);
    athenaLog << MSG::VERBOSE << "debug line _31_ "<<fittedVxCandidate->recVertex().fitQuality().doubleNumberDoF()<<"; number of tracks is"<<fittedVxCandidate->vxTrackAtVertex()->size()<< endmsg;
    myVtx->push_back(fittedVxCandidate);
    trkatvtx.clear();
    delete fittedVertex;
    //delete fittedVxCandidate; JTB mustn't delete object in container
    delete tracksAtVertex;
  }while(mytrk.size()>0);//vertex loop end
  athenaLog << MSG::VERBOSE << "debug line _326_ "<< myVtx->size()<< endmsg;
  xAOD::VertexContainer *xAODContainer(0);
  xAOD::VertexAuxContainer *xAODAuxContainer(0);
  if (m_VertexEdmFactory->createXAODVertexContainer(*myVtx, xAODContainer, xAODAuxContainer) != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("Cannot convert output vertex container to xAOD. Returning null pointer.");
  }
*/
    theVertexContainer->push_back(vx);

    trkatvtx.clear();
    //delete vx;
  }while(mytrk.size()>0);//vertex loop end

  for (auto pv = theVertexContainer->begin(); pv != theVertexContainer->end(); ++pv) {
    athenaLog << MSG::DEBUG << "vertex x = " <<(*pv)->position().x()<< endmsg;
  }
  return std::make_pair(theVertexContainer, theVertexAuxContainer);
  //return theVertexContainer;

}


vector<FTK_VertexFinderTool::MyTrack> FTK_VertexFinderTool::getTracks(const FTK_RawTrackContainer* trks){

  MsgStream athenaLog(msgSvc(), name());
  vector<MyTrack> mytrk;
  athenaLog << MSG::DEBUG << "getRawTracks: find "<< trks->size()<< " tracks "<< endmsg;
  for (unsigned int ftk_track_index = 0; ftk_track_index != trks->size(); ++ftk_track_index){
    const FTK_RawTrack* ftk_track= trks->at(ftk_track_index);
    float trk_theta = std::atan2(1.0,ftk_track->getCotTh());
    double invpt= ftk_track->getInvPt();
    float trk_eta = -std::log(std::tan(trk_theta/2));
    float trk_phi = ftk_track->getPhi();
    float trk_d0 = ftk_track->getD0();
    float trk_z0 = ftk_track->getZ0();
    float trk_pt = 1/invpt;
/*    double trk_phierr=sqrt(3.446e-7+29.24*invpt*invpt);
    double trk_thetaerr=sqrt(7.093e-6+38.4*invpt*invpt);
    double trk_pterr=1;
    double trk_d0err=sqrt(1.662e-3+6.947e4*invpt*invpt);
    double trk_z0err=sqrt(6.472e-2+1.587e5*invpt*invpt);
*/
    double trk_d0err= sqrt(m_uncertaintyTool->getParamCovMtx(*ftk_track,  m_hasIBL,  FTKTrackParam::d0,     FTKTrackParam::d0));
    double trk_z0err= sqrt(m_uncertaintyTool->getParamCovMtx(*ftk_track,  m_hasIBL,    FTKTrackParam::z0,     FTKTrackParam::z0))*m_z0errfactor;
    double trk_phierr= sqrt(m_uncertaintyTool->getParamCovMtx(*ftk_track,  m_hasIBL,    FTKTrackParam::phi,    FTKTrackParam::phi));
    double trk_thetaerr= sqrt(m_uncertaintyTool->getParamCovMtx(*ftk_track,  m_hasIBL,    FTKTrackParam::theta,  FTKTrackParam::theta));
    double trk_pterr= sqrt(m_uncertaintyTool->getParamCovMtx(*ftk_track,  m_hasIBL,    FTKTrackParam::pt,    FTKTrackParam::pt));

    if(fabs(trk_pt)<m_constTrkPt)continue;
    if(m_barrelOnly&&fabs(trk_eta)>m_constTrkEta)continue;
    if(ftk_track_index<10)athenaLog << MSG::DEBUG << "getRawTracks: get pt "<< trk_pt<<"+/-"<<trk_pterr << ", eta "<<trk_eta<<"+/-"<<trk_thetaerr<<", phi"<<trk_phi<<"+/-"<<trk_phierr<<", d0"<<trk_d0<<"+/-"<<trk_d0err<<", z0"<<trk_z0<<"+/-"<<trk_z0err <<endmsg;
    MyTrack Trk(ftk_track_index,trk_pt,trk_theta,trk_phi,trk_d0,trk_z0,trk_pterr,trk_thetaerr,trk_phierr,trk_d0err,trk_z0err);
    mytrk.push_back(Trk);
  }
  athenaLog << MSG::DEBUG << "getRawTracks: total "<< mytrk.size() << " track "<< endmsg;
  return mytrk;
}

vector<FTK_VertexFinderTool::MyTrack> FTK_VertexFinderTool::getTracks(const TrackCollection* trks){
  MsgStream athenaLog(msgSvc(), name());
  vector<MyTrack> mytrk;
  athenaLog << MSG::DEBUG << "getRefitTracks: find "<< trks->size()<< " tracks "<< endmsg;
  TrackCollection::const_iterator track_it   = trks->begin();
  TrackCollection::const_iterator last_track = trks->end();
   for (int iTrack = 0 ; track_it!= last_track; track_it++, iTrack++) {
//    double invpt= sin(trk_Theta)*ftk_track->getCurv()/2.;
    double trk_qOverp= (*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
    float trk_theta = (*track_it)->perigeeParameters()->parameters()[Trk::theta];
    float trk_eta = (*track_it)->perigeeParameters()->eta();//ctheta2eta(trk_theta);
    float trk_phi = (*track_it)->perigeeParameters()->parameters()[Trk::phi0];
    float trk_d0 = (*track_it)->perigeeParameters()->parameters()[Trk::d0];
    float trk_z0 = (*track_it)->perigeeParameters()->parameters()[Trk::z0];
    float trk_pt = 1/(trk_qOverp/sin(trk_theta));
/*    
    float invpt = 1/(trk_pt);
    double trk_phierr=sqrt(3.446e-7+29.24*invpt*invpt);
    double trk_thetaerr=sqrt(7.093e-6+38.4*invpt*invpt);
    double trk_pterr=1;
    double trk_d0err=sqrt(1.662e-3+6.947e4*invpt*invpt);
    double trk_z0err=sqrt(6.472e-2+1.587e5*invpt*invpt)*m_z0errfactor;
*/
    const AmgSymMatrix(5) *cov=(*track_it)->perigeeParameters()->covariance();
    double trk_d0err=sqrt((*cov)(0,0));
    double trk_z0err=sqrt((*cov)(1,1));
    double trk_phierr=sqrt((*cov)(2,2));
    double trk_thetaerr=sqrt((*cov)(3,3));
    double trk_qOverperr=sqrt((*cov)(4,4));
/*    double trk_d0err=(*track_it)->measurementsOnTrack()->localCovariance().error(Trk::d0);
    double trk_z0err=(*track_it)->measurementsOnTrack()->localErrorMatrix().error(Trk::z0);
    double trk_phi0err=(*track_it)->measurementsOnTrack()->localErrorMatrix().error(Trk::phi0);
    double trk_thetaerr=(*track_it)->measurementsOnTrack()->localErrorMatrix().error(Trk::theta);
    double trk_qOverperr=(*track_it)->measurementsOnTrack()->localErrorMatrix().error(Trk::qOverP);
*/    double trk_pterr=-sin(trk_theta)*trk_qOverperr/trk_qOverp/trk_qOverp+cos(trk_theta)*trk_thetaerr/trk_qOverp;
    if(fabs(trk_pt)<m_constTrkPt)continue;
    if(m_barrelOnly&&fabs(trk_eta)>m_constTrkEta)continue;
    if(iTrack<10)athenaLog << MSG::DEBUG << "getRefitTracks: get pt "<< trk_pt<<"+/-"<<trk_qOverperr << ", eta "<<trk_eta<<"+/-"<<trk_thetaerr<<", phi"<<trk_phi<<"+/-"<<trk_phierr<<", d0"<<trk_d0<<"+/-"<<trk_d0err<<", z0"<<trk_z0<<"+/-"<<trk_z0err <<endmsg;
    mytrk.push_back(MyTrack(iTrack,trk_pt,trk_theta,trk_phi,trk_d0,trk_z0,trk_pterr,trk_thetaerr,trk_phierr,trk_d0err,trk_z0err));
  }
  return mytrk;
}

double FTK_VertexFinderTool::ctheta2eta(double cot){
  double eta=0.0;
  double tempX=0.0;

  if(atan(1.0/cot) >= 0) tempX = atan(1.0/cot);
  if(atan(1.0/cot) < 0) tempX = atan(1.0/cot)+M_PI;

  eta = -log(tan(tempX/2.0));

  return eta;
}

