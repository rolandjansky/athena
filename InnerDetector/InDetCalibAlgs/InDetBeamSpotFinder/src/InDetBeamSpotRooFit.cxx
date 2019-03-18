/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBeamSpotRooFit.h"
#include "GaudiKernel/ITHistSvc.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include <RooRealVar.h>
#include <RooDataSet.h>
#include <RooArgSet.h>

using namespace RooFit;
using namespace InDet;

InDetBeamSpotRooFit::InDetBeamSpotRooFit( const std::string& type, 
					  const std::string& name, 
					  const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_cov(10), m_ax(0),m_ay(0),m_k(0),
  m_mx(0),m_my(0),m_mz(0),m_rho(0),
  m_sx(0),m_sy(0),m_sz(0),m_nUsed(0),
  m_vertexCount(0),
  m_fitStatus(unsolved),m_vtxResCut(0)
{
  declareInterface<IInDetBeamSpotTool>(this);
  declareProperty("vtxResCut", m_vtxResCut = 0.05 );//m_vtxCut="abs(vxx) < .0025 && abs(vyy) < .0025 && abs(z) < 300 && abs(x)<3 && abs(y)<3");
  declareProperty("InitialKFactor", m_kStart=1.0);
  declareProperty("RMSCut", m_rmsCutNum = 16);
  declareProperty("ConstantKFactor", m_kConst=false);
}

//Copy constructor: any variables that can be set with job options should be copied here
InDetBeamSpotRooFit::InDetBeamSpotRooFit( const InDetBeamSpotRooFit& rhs ) : 
  IInDetBeamSpotTool(rhs), 
  AthAlgTool(rhs.type(), rhs.name(), rhs.parent()),
  m_cov(10), m_ax(0),m_ay(0),m_k(0),
  m_mx(0),m_my(0),m_mz(0),m_rho(0),
  m_sx(0),m_sy(0),m_sz(0),m_nUsed(0),
  m_vertexCount(0),
  m_fitStatus(unsolved),
  m_vtxResCut(rhs.m_vtxResCut),
  m_kStart(rhs.m_kStart),
  m_rmsCutNum(rhs.m_rmsCutNum),
  m_kConst(rhs.m_kConst)
{}

StatusCode InDetBeamSpotRooFit::initialize() {
  ATH_MSG_DEBUG( "In initialize" );
  return StatusCode::SUCCESS;
}

StatusCode InDetBeamSpotRooFit::finalize() {
  ATH_MSG_DEBUG( "In finalize");
  return StatusCode::SUCCESS;
}

InDetBeamSpotRooFit::FitStatus InDetBeamSpotRooFit::fit(std::vector< BeamSpot::VrtHolder > &vtxData){ 
  m_vertexData = vtxData;

  //This is to determine the min and max values of the data for setting up the RooDataSet
  double xMin = m_vertexData[0].x ;
  double xMax = m_vertexData[0].x;
  double yMin = m_vertexData[0].y ;
  double yMax = m_vertexData[0].y;
  double zMin = m_vertexData[0].z;
  double zMax = m_vertexData[0].z;
  double vxxMin=m_vertexData[0].vxx ;
  double vxxMax=m_vertexData[0].vxx;
  double vyyMin=m_vertexData[0].vyy;
  double vyyMax=m_vertexData[0].vyy;
  double vxyMin=m_vertexData[0].vxy;
  double vxyMax=m_vertexData[0].vxy;

  for(Int_t i = 1;i < (Int_t)m_vertexData.size() ; i++){
    if (m_vertexData[i].x < xMin) xMin = m_vertexData[i].x;
    if (m_vertexData[i].x > xMax) xMax = m_vertexData[i].x;
    if (m_vertexData[i].y < yMin) yMin = m_vertexData[i].y;
    if (m_vertexData[i].y > yMax) yMax = m_vertexData[i].y;
    if (m_vertexData[i].z < zMin) zMin = m_vertexData[i].z;
    if (m_vertexData[i].z > zMax) zMax = m_vertexData[i].z;
    if (m_vertexData[i].vxx < vxxMin) vxxMin = m_vertexData[i].vxx;
    if (m_vertexData[i].vxx > vxxMax) vxxMax = m_vertexData[i].vxx;
    if (m_vertexData[i].vyy < vyyMin) vyyMin = m_vertexData[i].vyy;
    if (m_vertexData[i].vyy > vyyMax) vyyMax = m_vertexData[i].vyy;
    if (m_vertexData[i].vxy < vxyMin) vxyMin = m_vertexData[i].vxy;
    if (m_vertexData[i].vxy > vxyMax) vxyMax = m_vertexData[i].vxy;     
  }

  //Declare the observables
  RooRealVar x("x","Primary Vertex x [mm]",xMin,xMax);
  RooRealVar y("y","Primary Vertex y [mm]",yMin,yMax);
  RooRealVar z("z","Primary Vertex z [mm]",zMin,zMax);
  RooRealVar vxx("vxx","vxx [mm^2]",vxxMin,vxxMax);
  RooRealVar vyy("vyy","vyy [mm^2]",vyyMin,vyyMax);
  RooRealVar vxy("vxy","vyy [mm^2]",vxyMin,vxyMax);


  m_vtxCutString = "abs(x) < 3 && abs(y) < 3 && abs(z) < 300";
  m_vtxCutString += " && abs(vxx) < "+std::to_string(m_vtxResCut*m_vtxResCut)+" && abs(vyy) < "+std::to_string(m_vtxResCut*m_vtxResCut);
  const char *vtxCut = (const char*)m_vtxCutString.c_str();

  //Declare the RooDataSet and add data to it
  RooDataSet rfData("rfData","RooFit data",RooArgSet(x,y,z,vxx,vyy,vxy));

  m_vertexCount = m_vertexData.size();
 
  for( unsigned int j = 0; j < m_vertexCount; j++){
    x.setVal(m_vertexData[j].x);
    y.setVal(m_vertexData[j].y);
    z.setVal(m_vertexData[j].z);
    vxx.setVal(m_vertexData[j].vxx);
    vyy.setVal(m_vertexData[j].vyy);
    vxy.setVal(m_vertexData[j].vxy);
    rfData.add(RooArgSet(x,y,z,vxx,vyy,vxy));
  }

  Double_t vxxMean = rfData.reduce(Cut(vtxCut))->mean(vxx);
  Double_t vyyMean = rfData.reduce(Cut(vtxCut))->mean(vyy);
  Double_t axStart = 0, ayStart = 0, kStart = m_kStart;
  Double_t wxxStart = rfData.reduce(Cut(vtxCut))->sigma(x);
  Double_t wyyStart = rfData.reduce(Cut(vtxCut))->sigma(y);
  Double_t sxStart = sqrt(wxxStart*wxxStart - kStart*kStart*vxxMean);
  Double_t syStart = sqrt(wyyStart*wyyStart - kStart*kStart*vyyMean);
  Double_t szStart = rfData.reduce(Cut(vtxCut))->sigma(z);
  Double_t mxStart = rfData.reduce(Cut(vtxCut))->mean(x);
  Double_t myStart = rfData.reduce(Cut(vtxCut))->mean(y);
  Double_t mzStart = rfData.reduce(Cut(vtxCut))->mean(z);
  Double_t rhoStart = 0;

  RooRealVar ax("ax","Tilt x",axStart,-1,1);
  RooRealVar ay("ay","Tilt y",ayStart,-1,1);
  RooRealVar k("k","k factor",kStart,0,3);
  if (m_kConst){ k.setConstant(kTRUE); }
  RooRealVar mx("mx","Mean x",mxStart,xMin,xMax);
  RooRealVar my("my","Mean y",myStart,yMin,yMax);
  RooRealVar mz("mz","Mean z",mzStart,zMin,zMax);
  RooRealVar rho("rho","x-y correlation",rhoStart,-1,1);
  RooRealVar sx("sx","Width x",sxStart,0,1);
  RooRealVar sy("sy","Width y",syStart,0,1);
  RooRealVar sz("sz","Width z",szStart,1,200);


  //Perform the fit and add results to member variables   
  
  BeamSpotPdf fitModel("fitModel","BeamSpot PDF",x,y,z,vxx,vyy,vxy,mx,sx,ax,my,sy,ay,mz,sz,k,rho);

  //Apply the specified cut on position and vertex resolution
  //RooDataSet *reducedData = (RooDataSet*)rfData.reduce( Cut(vtxCut) );
  //We now need to combine the vtx resolution cut with the rms cut
  //std::string combinedCutString = combineCuts(rfData);
  //const char *combinedCut = (const char*)combinedCutString.c_str();

  double rmsCutXLow = mxStart - m_rmsCutNum * wxxStart;
  double rmsCutXHigh = mxStart + m_rmsCutNum * wxxStart;
  double rmsCutYLow = myStart - m_rmsCutNum * wyyStart;
  double rmsCutYHigh = myStart + m_rmsCutNum * wyyStart;
  double rmsCutZLow = mzStart - m_rmsCutNum * szStart;
  double rmsCutZHigh = mzStart + m_rmsCutNum * szStart;

  std::string rmsCutString = static_cast<std::ostringstream*>( & (std::ostringstream() << "x > " << rmsCutXLow << " && "
							     << "x < " << rmsCutXHigh << " && "
							     << "y > " << rmsCutYLow  << " && "
							     << "y < " << rmsCutYHigh << " && "
							     << "z > " << rmsCutZLow  << " && "
							     << "z < " << rmsCutZHigh) ) -> str();

  std::string combinedCutString = m_vtxCutString + std::string(" && ") + rmsCutString;
  const char *combinedCut = (const char*)combinedCutString.c_str();

  ATH_MSG_INFO( "combinedCut = "<<combinedCut );
  

  //Convert these numbers to strings, then concatenate the strings
  //rmsCutX = x > rmsCutLow && x < rmsCutXHigh;
  //repeat for y and z


  RooFitResult *r = fitModel.fitTo( *(rfData.reduce(Cut(combinedCut))) ,ConditionalObservables(RooArgSet(vxx,vyy,vxy)),Save(),PrintLevel(-1));

  r->Print();
  m_nUsed = rfData.reduce(Cut(combinedCut))->numEntries();

  ATH_MSG_INFO( "A total of " << m_vertexCount << " vertices passed pre-selection. Of which "
			           << rfData.reduce(Cut(combinedCut))->numEntries()<<" vertices will be used in the ML fit.") ;


  if ( r->edm() <= 10e-04 && r->covQual() == 3){
    m_fitStatus = successful;
  }
  else m_fitStatus = failed;
  m_ax  = ax.getVal();
  m_ay  = ay.getVal();
  m_k   = k.getVal();
  m_mx  = mx.getVal();
  m_my  = my.getVal();
  m_mz  = mz.getVal();
  m_rho = rho.getVal();
  m_sx  = sx.getVal();
  m_sy  = sy.getVal();
  m_sz  = sz.getVal();
  
  if( m_kConst ){
    m_cov.ResizeTo(9,9);
  }
  m_cov = r->covarianceMatrix();

  return (  m_fitStatus );
}


std::map< std::string, double > InDetBeamSpotRooFit::getCovMap() const{
  int numParams=0;
  if (m_kConst){
    numParams = 9;
  }
  else{
    numParams = 10;
  }
 
  const std::string offDiagArr[] = {"TiltX","TiltY","k","X","Y","Z","RhoXY","Sx","Sy","Sz"};
  const std::string diagArr[] = {"tiltX","tiltY","k","posX","posY","posZ","rhoXY","sigmaX","sigmaY","sigmaZ"};

  const std::string offDiagArrConstK[] = {"TiltX","TiltY","X","Y","Z","RhoXY","Sx","Sy","Sz"};
  const std::string diagArrConstK[] = {"tiltX","tiltY","posX","posY","posZ","rhoXY","sigmaX","sigmaY","sigmaZ"};
  
  std::map< std::string, double > covMap;
  std::vector< std::string > offDiagVector;
  std::vector< std::string > diagVector;
  std::string covString = "cov";
  std::string errString = "Err";

  std::string key;

  for(int i = 0; i < numParams; i++){
    if(m_kConst){
      offDiagVector.push_back(offDiagArrConstK[i]);
      diagVector.push_back(diagArrConstK[i]);
    }
    else{
      offDiagVector.push_back(offDiagArr[i]);
      diagVector.push_back(diagArr[i]);
    }
  }

  for( unsigned int i = 0; i < offDiagVector.size(); i++){
    for( unsigned int j = 0; j <= i; j++){
      if( i == j ){
	key = diagVector[i] + errString;
	covMap[key] = sqrt(m_cov[i][j]);
      }
      else {
	key = covString + offDiagVector[i] + offDiagVector[j];
	covMap[key] = m_cov[i][j];
	key = covString + offDiagVector[j] + offDiagVector[i];
	covMap[key] = m_cov[i][j];
      }

    }
  }
  
  if( m_kConst ){
    covMap["kErr"] = 0;
  }

  //We've filled the entire covariance matrix, but we only want to keep the top half, in the order:
  //x,y,z,ax,ay,sx,sy,sz,rho,k
  const std::string offDiagRemoveArr[] = {"k","RhoXY","Sz","Sy","Sx","TiltY","TiltX","Z","Y","X"};
  const std::string offDiagRemoveArrConstK[] = {"RhoXY","Sz","Sy","Sx","TiltY","TiltX","Z","Y","X"};
  std::vector< std::string > offDiagRemoveVector;
  for (int i = 0; i < numParams; i++){
    if( m_kConst ){
      offDiagRemoveVector.push_back(offDiagRemoveArrConstK[i]);
    }
    else{
      offDiagRemoveVector.push_back(offDiagRemoveArr[i]);
    }
  }
  for( unsigned int i = 0; i < offDiagRemoveVector.size(); i++){
    for( unsigned int j = i+1; j < offDiagRemoveVector.size(); j++){
      covMap.erase(covString+offDiagRemoveVector[i]+offDiagRemoveVector[j]);
    }
  }

  return covMap;
}

std::map<std::string,double> InDetBeamSpotRooFit::getParamMap() const{

  std::map<std::string,double> paramMap;
  paramMap["tiltX"]  = m_ax;
  paramMap["tiltY"]  = m_ay;
  paramMap["k"]      = m_k;
  paramMap["posX"]   = m_mx;
  paramMap["posY"]   = m_my;
  paramMap["posZ"]   = m_mz;
  paramMap["sigmaX"] = m_sx;
  paramMap["sigmaY"] = m_sy;
  paramMap["sigmaZ"] = m_sz;
  paramMap["rhoXY"]  = m_rho;
  paramMap["nUsed"] = m_nUsed;
    
  return paramMap;
}
