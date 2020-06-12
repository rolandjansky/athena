/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DistortionsTool.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PathResolver/PathResolver.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

SCT_DistortionsTool::SCT_DistortionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_sctID(nullptr)
{
  declareInterface<ISCT_ModuleDistortionsTool>(this);
  declareProperty("TextFileName1",m_textFileNameJ1="HashJap1.txt","Read this file for hash id"); 
  declareProperty("TextFileName2",m_textFileNameJ2="HashJap2.txt","Read this file for hash id");
  declareProperty("TextFileName3",m_textFileNameUK="HashUK.txt","Read this file for hash id");
  declareProperty("TextFileName4",m_textFileNameUSA="HashUSA.txt","Read this file for hash id");
  declareProperty("TextFileName5",m_textFileNameScand="HashScand.txt","Read this file for hash id");
  declareProperty("TextFileName6",m_textFileNameProfiles="TotalCommonProfile.txt","Read this file for profiles");
  declareProperty("distortionsScale",m_distortionsScale=1.0);
}

StatusCode SCT_DistortionsTool::initialize(){
  ATH_MSG_DEBUG("initialize()");
  
  // Get the SCT helper
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL( "Could not get SCT ID helper");
    return StatusCode::FAILURE;
  }
  
  bool readFiles = loadData();
  if (!readFiles){
    ATH_MSG_FATAL( "Could not Read Files" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DistortionsTool::execute() {
  ATH_MSG_DEBUG( "execute()");
  ATH_MSG_INFO( "SCT_DistortionsTool successfully installed");
  return StatusCode::SUCCESS;
}

double SCT_DistortionsTool::correctReconstruction( const Trk::TrackParameters & trackPar, const InDetDD::SiDetectorElement & Element, Trk::LocalParameters & LocPar, const Amg::Vector2D & loct) const 
{
  ATH_MSG_DEBUG( "*** Including Distortions In Reconstruction ***");
  Trk::LocalParameters * locpar(&LocPar);
  Amg::Vector2D locpos( loct.y(),(*locpar)[Trk::locX] );

  const InDetDD::SiDetectorElement * EL(&Element);
  IdentifierHash id = EL->identifyHash();

  Amg::Vector3D my_track = trackPar.momentum();
  Amg::Vector3D my_normal = (*EL).normal();
  Amg::Vector3D my_phiax = (*EL).phiAxis();
  Amg::Vector3D my_etaax = (*EL).etaAxis();
  double mag_normal = sqrt( my_normal.x()* my_normal.x() + my_normal.y()* my_normal.y() + my_normal.z()* my_normal.z() );
  double mag_phi = sqrt( my_phiax.x() * my_phiax.x() + my_phiax.y() * my_phiax.y()  + my_phiax.z() * my_phiax.z() );
  double mag_eta = sqrt( my_etaax.x() * my_etaax.x() + my_etaax.y() * my_etaax.y() + my_etaax.z() * my_etaax.z() );

  double xdir = ( my_track.dot(my_phiax) ) / mag_phi;
  double ydir = ( my_track.dot(my_etaax) ) / mag_eta;
  double zdir = ( my_track.dot(my_normal) ) / mag_normal;
  Amg::Vector3D dir(ydir ,xdir ,zdir );
  Amg::Vector2D Corr = correction(id, locpos, dir);// Correction gives X, Y shift

  ATH_MSG_DEBUG( "correction = "<<Corr);
  return (Corr.y());
}

Amg::Vector2D SCT_DistortionsTool::correctSimulation(IdentifierHash id, double xhit, double yhit, double cEta, double cPhi, double cDep ) const 
{
  ATH_MSG_DEBUG( "*** Including Distortions In Digitization ***");
  Identifier waferIdHash = m_sctID->wafer_id(id);
  int sctSide   = m_sctID->side(waferIdHash);
  ATH_MSG_DEBUG( "hash = "<< id <<" side = "<<sctSide <<" xhit = "<< xhit<< " yhit = " << yhit << "cEta = " << cEta << " cPhi = " << cPhi << "cDep = " << cDep );
  double Dx = cEta, Dy = cPhi, Dz = cDep;
  double NewX = xhit, NewY = yhit;
  if(sctSide == 0)NewY = -yhit;
  if(sctSide == 0)Dy = -cPhi;
  Amg::Vector2D newlocpos(NewX,NewY);
  Amg::Vector3D Direction( Dx, Dy, Dz );

  Amg::Vector2D TempCorrSim;
  TempCorrSim = correction(id, newlocpos, Direction);// Correction gives X, Y shift
  ATH_MSG_DEBUG( "TempCorrSim = " << TempCorrSim );
  xhit += TempCorrSim.x();
  if(sctSide == 1)yhit += TempCorrSim.y();
  if(sctSide == 0)yhit -= TempCorrSim.y();
  Amg::Vector2D NewPos( xhit, yhit);
  ATH_MSG_DEBUG( " newpos = "<<NewPos );
  return NewPos;
}
// **********************************************
// *** Main function to calculate distortions ***
// **********************************************
Amg::Vector2D SCT_DistortionsTool::correction(IdentifierHash id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const {

  double xhit = locpos[0];
  double yhit = locpos[1];
  double xdir = direction.x();
  double ydir = direction.y();
  double zdir = direction.z();
  int Region = 0;
  double XCorr = 0.0;
  double YCorr = 0.0;

  IdentifierHash ID = id;
  Identifier waferId = m_sctID->wafer_id(ID);
  int Side   = m_sctID->side(waferId);

  if( Side != 0) ID = ID - 1;
  Region = identifyRegion(ID);
  if (Region != 0) {
    ATH_MSG_DEBUG( " Hash = "<< id<< " ID = " << ID <<" Side = "<< Side <<" Region = "<< Region );
    // ***** Read in appropriate Z profile *****
    const std::vector<float>* ZVec = readDistortions(Region,Side);
    // ***** Find shift in Z for x,y position *****
    float delZ = zShift(xhit, yhit, ZVec );
    ATH_MSG_DEBUG( " x = "<< xhit<< " y = " << yhit<<" delZ = "<< delZ );
    // ***** Calculate correction *****
    float tanThetaX = xdir/zdir; 
    float tanThetaY = ydir/zdir;
    float delX = std::abs(delZ * tanThetaX);
    float delY = std::abs(delZ * tanThetaY);

    // Add/subtract depending on angle of track
    // Signs appropriate for Digitization
    const float moveX(std::abs(delX));
    if(xdir > 0 && Side == 0)XCorr = -moveX;//'track' moving in +ve x dir
    if(xdir > 0 && Side != 0)XCorr = moveX;//'track' moving in +ve x dir
    if(xdir < 0 && Side == 0)XCorr = moveX;//'track' moving in -ve x dir
    if(xdir < 0 && Side != 0)XCorr = -moveX;//'track' moving in -ve x dir
    		   
    const float moveY(std::abs(delY));
    if(ydir > 0 && Side == 0)YCorr = -moveY;//'track' moving in +ve y dir
    if(ydir > 0 && Side != 0)YCorr = moveY;//'track' moving in +ve y dir
    if(ydir < 0 && Side == 0)YCorr = moveY;//'track' moving in -ve y dir
    if(ydir < 0 && Side != 0)YCorr = -moveY;//'track' moving in -ve y dir
    
  }
  return Amg::Vector2D( XCorr, YCorr); 
  
}

// Crude method to find Z shift for (x,y)
// Reads from a table and does nearest neighbour average
// Smoother paramaterisation planned
float SCT_DistortionsTool::zShift(const double xhit,const double yhit,const std::vector<float>* ZVec ) const
{
  float delZ = 0.0;
  float zGrid[50];// Set up x, y and z arrays for common profile
  std::vector<float>::const_iterator ZVecFirst = ZVec->begin();
  std::vector<float>::const_iterator ZVecLast = ZVec->end();
  int k = 0;
  for(; ZVecFirst != ZVecLast; ++ZVecFirst){
    zGrid[k] = (*ZVecFirst) * m_distortionsScale;
    ATH_MSG_DEBUG( "zvec =  " <<(*ZVecFirst)  ); 
    k++;
  }

  float xGrid[10];
  float yGrid[5];
  // Average x and y positions of module grid reference
  xGrid[0] = -62.8;   xGrid[1] = -47.5;   xGrid[2] = -32.2;   xGrid[3] =  -16.8;  xGrid[4] = -1.84 ;
  xGrid[5] = 1.41;  xGrid[6] = 16.7;  xGrid[7] = 32.1;  xGrid[8] = 47.5;  xGrid[9] = 62.6;

  yGrid[0] = -31.33835;  yGrid[1] = -15.67632;  yGrid[2] = -0.01504;  yGrid[3] = 15.64652;  yGrid[4] = 31.30763;

  int Xarray = -999;//Temp array number
  int Yarray = -999;

  // Find square containing hit (Xarray and Yarray are top right corner)
  if(yhit > yGrid[4]) Yarray = 5;
  if(xhit > xGrid[9]) Xarray = 10;
  if(yhit < yGrid[0]) Yarray = 0;
  if(xhit < xGrid[0]) Xarray = 0;
  for(int i = 1; i < 10; i++){// Loop over x
    if(xhit < xGrid[i] && xhit > xGrid[i-1]) Xarray = i;
  }
  //note, sroe: original code had the following loop embedded inside the x loop above
  for(int j = 1; j < 5; j++){// Loop over y
	if(yhit < yGrid[j] && yhit > yGrid[j-1]) Yarray = j;
  }

  // Find Z of hit point from 2 nearest neighbours
  // Outer corners of wafer
  if(Xarray == 0 && Yarray == 0) delZ = zGrid[0];
  if(Xarray == 0 && Yarray == 5) delZ = zGrid[20];
  if(Xarray == 10 && Yarray == 0) delZ = zGrid[29];
  if(Xarray == 10 && Yarray == 5) delZ = zGrid[49];
  // Left edge
  if(Xarray == 0 && Yarray < 5 && Yarray > 0){
    int TempZ1 = Yarray*5;
    int TempZ2 = (Yarray-1)*5;
    delZ = 0.5*(zGrid[TempZ1]+zGrid[TempZ2]);
  }
  // Right edge
  if(Xarray == 10 && Yarray < 5 && Yarray > 0){
    int TempZ1 = 29 + Yarray*5;
    int TempZ2 = 29 + (Yarray-1)*5;
    delZ = 0.5*(zGrid[TempZ1]+zGrid[TempZ2]);
  }
  // Top Edge
  if(Yarray == 5 && Xarray > 0 && Xarray < 10){
    int TempZ1, TempZ2;
    if(Xarray < 5)TempZ1 = 20 + Xarray;
    if(Xarray > 4)TempZ1 = 40 + Xarray;
    if(Xarray == 5) TempZ2 = 24;    
    else TempZ2 = TempZ1 -1;
    delZ = 0.5*(zGrid[TempZ1]+zGrid[TempZ2]);
  }
  // Bottom edge
  if(Yarray == 0 && Xarray > 0 && Xarray < 10){
    int TempZ1, TempZ2;
    if(Xarray < 5)TempZ1 =  Xarray;
    if(Xarray > 4)TempZ1 = 20 + Xarray;
    if(Xarray == 5) TempZ2 = 4;    
    else TempZ2 = TempZ1 -1;
    delZ = 0.5*(zGrid[TempZ1]+zGrid[TempZ2]);
  }
  // Central Rectangle
  if(Xarray < 10 && Xarray > 0 && Yarray < 5 && Yarray > 0){
    int TempZ1;
    // Left wafer
    if(Xarray < 5 ) TempZ1 = Xarray + Yarray*5;
    // Right wafer
    if(Xarray > 4 ) TempZ1 = Xarray + 20 + Yarray*5;
    
    int TempZ2 = TempZ1 - 1;
    int TempZ3 = TempZ2 - 5;
    int TempZ4 = TempZ1 - 5;  
    //2***1
    //*   *
    //3***4
    float NNDist1 = ( (xhit - xGrid[Xarray])*(xhit - xGrid[Xarray]) + (yhit - yGrid[Yarray])*(yhit - yGrid[Yarray])  );
    float NNDist2 = ( (xhit - xGrid[Xarray-1])*(xhit - xGrid[Xarray-1]) + (yhit - yGrid[Yarray])*(yhit - yGrid[Yarray])  );
    float NNDist3 = ( (xhit - xGrid[Xarray-1])*(xhit - xGrid[Xarray-1]) + (yhit - yGrid[Yarray-1])*(yhit - yGrid[Yarray-1])  );
    float NNDist4 = ( (xhit - xGrid[Xarray])*(xhit - xGrid[Xarray]) + (yhit - yGrid[Yarray-1])*(yhit - yGrid[Yarray-1])  );

    float Sum1 = NNDist1 + NNDist2;
    float Sum2 = NNDist3 + NNDist4;

    float Sum3 = NNDist1 + NNDist3;
    float Sum4 = NNDist2 + NNDist4;

    float Sum5 = NNDist1 + NNDist4;
    float Sum6 = NNDist2 + NNDist3;

    if(Sum1 < Sum2 && Sum1 < Sum3 && Sum1 < Sum4 && Sum1 < Sum5 && Sum1 < Sum6 )delZ = 0.5*(zGrid[TempZ1] + zGrid[TempZ2]); 
    if(Sum2 < Sum1 && Sum2 < Sum3 && Sum2 < Sum4 && Sum2 < Sum5 && Sum2 < Sum6 )delZ = 0.5*(zGrid[TempZ3] + zGrid[TempZ4]); 
    if(Sum3 < Sum2 && Sum3 < Sum1 && Sum3 < Sum4 && Sum3 < Sum5 && Sum3 < Sum6 )delZ = 0.5*(zGrid[TempZ1] + zGrid[TempZ3]); 
    if(Sum4 < Sum2 && Sum4 < Sum3 && Sum4 < Sum1 && Sum4 < Sum5 && Sum4 < Sum6 )delZ = 0.5*(zGrid[TempZ2] + zGrid[TempZ4]); 
    if(Sum5 < Sum2 && Sum5 < Sum3 && Sum5 < Sum4 && Sum5 < Sum1 && Sum5 < Sum6 )delZ = 0.5*(zGrid[TempZ1] + zGrid[TempZ4]); 
    if(Sum6 < Sum2 && Sum6 < Sum3 && Sum6 < Sum4 && Sum6 < Sum5 && Sum6 < Sum1 )delZ = 0.5*(zGrid[TempZ3] + zGrid[TempZ2]); 

  }
  return delZ;
}

const std::vector<float>* SCT_DistortionsTool::readDistortions(int RegionID, int Side) const
{
  if(RegionID == 1 ){//Jap1 LL and RL
    if ( Side == 0 )
      return &m_dataJap1_S0;
    else
      return &m_dataJap1_S1;
  }
  else if(RegionID == 2){//Jap2
    if ( Side == 0 )
      return &m_dataJap2_S0;
    else
      return &m_dataJap2_S1;
  }
  else if(RegionID == 3){//UK
    if ( Side == 0 )
      return &m_dataUK_S0;
    else
      return &m_dataUK_S1;
  }
  else if(RegionID == 4){//USA
    if ( Side == 0 )
      return &m_dataUSA_S0;
    else 
      return &m_dataUSA_S1;
  }
  else if(RegionID == 5 ){//Scand
    if ( Side == 0 )
      return &m_dataScand_S0;
    else
      return &m_dataScand_S1;
  }
  return nullptr;
}


bool SCT_DistortionsTool::loadData()
{

  int temp(0);
  
  // ***** Jap1 *****
  std::string file_name =  PathResolver::find_file(m_textFileNameJ1, "DATAPATH");
  if (file_name.size()==0) {
    msg(MSG::WARNING) << "Hash file " << m_textFileNameJ1   << " not found! " << endmsg; 
    return false;
  }
  std::ifstream input(file_name.c_str());
  if ( !input.good() ) {
    msg(MSG::WARNING) <<"Cannot open " << file_name   << " not found! "  << endmsg;
    return false;
  }
  while ( input >> temp ) {
	  m_moduleSites.insert(std::make_pair(temp,1));
  }
  input.close();

  // ***** Jap2 *****  
  std::string file_name2 =  PathResolver::find_file(m_textFileNameJ2, "DATAPATH");
  if (file_name2.size()==0) {
    msg(MSG::WARNING) <<"Hash file " << m_textFileNameJ2   << " not found! "  << endmsg;
    return false;
  }
  std::ifstream input2(file_name2.c_str());
  if ( !input2.good() ) {
    msg(MSG::WARNING)<< "Cannot open " << file_name2   << " not found! " << endmsg;
    return false;
  }

  while ( input2 >> temp ) {
	  m_moduleSites.insert(std::make_pair(temp,2));
  }
  input2.close();

  // ***** UK *****
  std::string file_name3 =  PathResolver::find_file(m_textFileNameUK, "DATAPATH");
  if (file_name3.size()==0) {
    msg(MSG::WARNING) << "Hash file " << m_textFileNameUK   << " not found! " << endmsg;
    return false;
  }
  std::ifstream input3(file_name3.c_str());
  if ( !input3.good() ) {
    msg(MSG::WARNING) << "Cannot open " << file_name3   << " not found! " << endmsg;
    return false;
  }

  while ( input3 >> temp ) {
	  m_moduleSites.insert(std::make_pair(temp,3));
  }
  input3.close();

  // ***** USA *****
  std::string file_name4 =  PathResolver::find_file(m_textFileNameUSA, "DATAPATH");
  if (file_name4.size()==0) {
    msg(MSG::WARNING)<< "Hash file " << m_textFileNameUSA   << " not found! " << endmsg;
    return false;
  }
  std::ifstream input4(file_name4.c_str());
  if ( !input4.good() ) {
     msg(MSG::WARNING) << "Cannot open " << file_name4   << " not found! " << endmsg;
    return false;
  }
  while ( input4 >> temp ) {
	  m_moduleSites.insert(std::make_pair(temp,4));
  }
  input4.close();
  
  // ***** Scand *****
  std::string file_name5 =  PathResolver::find_file(m_textFileNameScand, "DATAPATH");
  if (file_name5.size()==0) {
     msg(MSG::WARNING) << "Hash file " << m_textFileNameScand   << " not found! " << endmsg;
    return false;
  }
  std::ifstream input5(file_name5.c_str());
  if ( !input5.good() ) {
    msg(MSG::WARNING) << "Cannot open " << file_name5   << " not found! " << endmsg;
    return false;
  }
  while ( input5 >> temp ) {
	  m_moduleSites.insert(std::make_pair(temp,5));
  }
  input5.close();

  std::string pro_file_name =  PathResolver::find_file(m_textFileNameProfiles, "DATAPATH");
  if (pro_file_name.size()==0) {
    msg(MSG::WARNING) << "Profile file " << m_textFileNameProfiles   << " not found! "  << endmsg;
  }
  std::ifstream inputProfile(pro_file_name.c_str());
  if ( !inputProfile.good() ) {
    msg(MSG::WARNING) <<"Cannot open " << pro_file_name   << " not found! "  << endmsg;
  }
 
  for ( int lines = 1; lines < 560; lines++ ) {
    std::string linestr;
    std::getline(inputProfile, linestr);
    std::istringstream ss(linestr);
    float ZVal;
    ss >> ZVal;
    // **** Lower surface ****

    if(lines < 364 && lines > 338) //LL
    {
      m_dataJap1_S1.push_back(ZVal);
    }
    if(lines < 392 && lines > 366) //RL
    {
      m_dataJap1_S1.push_back(ZVal);
    }
    if(lines < 476 && lines > 450) //LL
    {
      m_dataJap2_S1.push_back(ZVal);
    }
    if(lines < 504 && lines > 478) //RL
    {
      m_dataJap2_S1.push_back(ZVal);
    }
    if(lines < 252 && lines > 226) //LL
    {
      m_dataUK_S1.push_back(ZVal);
    }
    if(lines < 280 && lines > 254) //RL
    {
      m_dataUK_S1.push_back(ZVal);
    }
    if(lines < 140 && lines > 114) //LL
    {
      m_dataUSA_S1.push_back(ZVal);
    }
    if(lines < 168 && lines > 142) //RL
    {
      m_dataUSA_S1.push_back(ZVal);
    }
    if(lines < 28 && lines > 2) //LL
    {
      m_dataScand_S1.push_back(ZVal);
    }
    if(lines < 56 && lines > 30) //RL
    {
      m_dataScand_S1.push_back(ZVal);
    }

    // **** Upper surface ****
    if(lines < 420 && lines > 394) //LU
    {
      m_dataJap1_S0.push_back(ZVal);
    }
    if(lines < 448 && lines > 422) //RU
    {
      m_dataJap1_S0.push_back(ZVal);
    }
    if(lines < 532 && lines > 506) //LU
    {
      m_dataJap2_S0.push_back(ZVal);
    }
    if(lines < 560 && lines > 534) //RU
    {
      m_dataJap2_S0.push_back(ZVal);
    }
    if(lines < 308 && lines > 282) //LU
    {
      m_dataUK_S0.push_back(ZVal);
    }
    if(lines < 336 && lines > 310) //RU
    {
      m_dataUK_S0.push_back(ZVal);
    }
    if(lines < 196 && lines > 170) //LU
    {
      m_dataUSA_S0.push_back(ZVal);
    }
    if(lines < 224 && lines > 198) //RU
    {
      m_dataUSA_S0.push_back(ZVal);
    }
    if(lines < 84 && lines > 58) //LU
    {
      m_dataScand_S0.push_back(ZVal);
    }
    if(lines < 112 && lines > 86) //RU
    {
      m_dataScand_S0.push_back(ZVal);
    }
   
    if ( inputProfile.bad() || inputProfile.eof() ) break;
  }
  inputProfile.close();

  return true;
}


   
int SCT_DistortionsTool::identifyRegion(IdentifierHash id) const 
{  
  ATH_MSG_DEBUG( "Identifying module common profile region: " ); 
  //int REGION=0;//1=J1,2=J2,3=UK,4=USA,5=Scand
  //Identifier::value_type ID = id.get_compact();
  std::map<int,int>::const_iterator it;
  
  int ID = id;
  it = m_moduleSites.find(ID);
  if (it == m_moduleSites.end() ){
    ATH_MSG_ERROR("Module ID : "<< ID << "not found in map");
    return -1;   
  }

  return it->second ;

}

StatusCode SCT_DistortionsTool::finalize() {
  ATH_MSG_INFO("finalize()");
  
  return StatusCode::SUCCESS;
}





