/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_SeededSpacePointFinder_ATL
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 04/15/2006 T.Koffas
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <set>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h" 
#include "TRT_SeededSpacePointFinderTool/TRT_SeededSpacePointFinder_ATL.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"

//Cluster collections
//
#include "InDetPrepRawData/SCT_ClusterContainer.h"
//SCT Geometry
//
#include "InDetIdentifier/SCT_ID.h"

//Magnetic field
//
//#include "TrkMagFieldInterfaces/IMagneticFieldTool.h"
//#include "TrkMagFieldUtils/MagneticFieldMapSolenoid.h"
//#include "MagFieldInterfaces/IMagFieldSvc.h"

//Association tool
//
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include "StoreGate/ReadHandle.h"

using namespace std;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededSpacePointFinder_ATL::TRT_SeededSpacePointFinder_ATL
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_fieldServiceHandle("AtlasFieldSvc",n),
    m_fieldService(nullptr),
    m_assotool("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_nprint(0),
    m_sctId(nullptr),
    r_size(0),
    m_ns(0),
    m_fNmax(0),
    m_nr(0),
    m_nrf(0),
    m_sF(0.)    
    
{
  m_fieldmode = "MapSolenoid"              ;
  m_ptmin     =   500.  ;  //Lowest pT of track.Up to 2000MeV bending in (r,phi) is +-4
  r_Sorted    = 0       ;
  r_index     = 0       ;
  r_map       = 0       ;
  r_rmax      = 600.    ;  //Max radial extend of Si ID
  r_rmin      = 0.      ;  //Min radial extend for loading SPs in ID
  r_rstep     =  10.    ;  //Step size for SP and SCT histogramming
  m_r1max     = 560.    ;  //Max radius of 1st SCT layer
  m_r12min    = 400.    ;  //Min radius of 2nd SCT layer
  m_r2min     = 340.    ;  //Min radius of 3rd SCT layer.Never search below that
  m_dzdrmin   = -4.00   ;  //Min pseudorapidity cut
  m_dzdrmax   = +4.00   ;  //Max pseudorapidity cut
//   m_xiC       = 15.     ;  //Momentum chi2 cut between seed and TRT segment estimate
//   m_xiTC      = 30.     ;  //Polar angle chi2 cut between seed and TRT segment estimate
//   m_xiFC      = 50.     ;  //Azimuthal angle chi2 cut between seed and TRT segment estimate
  m_xiC      = 100.     ;  //Momentum chi2 cut between seed and TRT segment estimate
  m_xiTC     = 100.     ;  //Polar angle chi2 cut between seed and TRT segment estimate
  m_xiFC     = 100.     ;  //Azimuthal angle chi2 cut between seed and TRT segment estimate
  m_search    = true    ;  //Search into neighboring sectors
  m_useasso   = false   ;  //Use association tool to clean up the seeds
  m_loadFull  = true    ;  //Load all the Si space points, otherwise only from the last 3 SCT layers
  m_doCosmics = false   ;  //Disable seed selection cuts when reconstructing cosmics tracks

  
  declareInterface<ITRT_SeededSpacePointFinder>(this);

  declareProperty("MagneticTool"          ,m_fieldServiceHandle    );
  declareProperty("AssociationTool"       ,m_assotool              );
  declareProperty("MagneticFieldMode"     ,m_fieldmode             );
  declareProperty("pTmin"                 ,m_ptmin                 );
  declareProperty("UseAssociationTool"    ,m_useasso               );
  declareProperty("NeighborSearch"        ,m_search                );
  declareProperty("LoadFull"              ,m_loadFull              );
  declareProperty("DoCosmics"             ,m_doCosmics             );
  declareProperty("Xi2C"                  ,m_xiC                   );
  declareProperty("Xi2TC"                 ,m_xiTC                  );
  declareProperty("Xi2FC"                 ,m_xiFC                  );

}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededSpacePointFinder_ATL::~TRT_SeededSpacePointFinder_ATL()
{
  if(r_index)  delete [] r_index;
  if(r_map  )  delete [] r_map  ; 
  if(r_Sorted) delete [] r_Sorted;

}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededSpacePointFinder_ATL::initialize()
{
 
  
  // Get magnetic field service
  if(m_fieldmode != "NoField" ) {
    
    if( !m_fieldServiceHandle.retrieve() ){
      ATH_MSG_FATAL("Failed to retrieve " << m_fieldServiceHandle );
      return StatusCode::FAILURE;
    }   
    ATH_MSG_DEBUG("Retrieved " << m_fieldServiceHandle );
    m_fieldService = &*m_fieldServiceHandle;
  }

  magneticFieldInit(); 

  // Get association tool
  //
  if(m_useasso){
    if(m_assotool.retrieve().isFailure()) {
      msg(MSG::FATAL)<<"Could not get "<<m_assotool<<endmsg; return StatusCode::FAILURE;
    }
  }

  StatusCode sc = AthAlgTool::initialize(); 

  sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Could not get SCT_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  // Build framework
  //
  buildFrameWork();

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)){m_nprint=0; msg(MSG::DEBUG) << (*this) << endmsg;}

  ATH_CHECK(m_spacepointsPixname.initialize());
  ATH_CHECK(m_spacepointsSCTname.initialize());
  ATH_CHECK(m_spacepointsOverlapname.initialize()); 

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededSpacePointFinder_ATL::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); return sc;

}

///////////////////////////////////////////////////////////////////
// Initialize tool for new event 
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::newEvent ()
{
  erase();

  double irstep = 1./r_rstep;

  if(m_loadFull){
    // Get pixel space points containers from store gate 
    //
    SG::ReadHandle<SpacePointContainer> spacepointsPix(m_spacepointsPixname);
    if (spacepointsPix.isValid()) {
     SpacePointContainer::const_iterator spc =  spacepointsPix->begin  (); 
      SpacePointContainer::const_iterator spce =  spacepointsPix->end  ();

      for(; spc != spce; ++spc) {

        SpacePointCollection::const_iterator sp  = (*spc)->begin();
        SpacePointCollection::const_iterator spe = (*spc)->end  ();
        for(; sp != spe; ++sp) {

	  double r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  int   ir = int(r*irstep); 
	  const Trk::SpacePoint* sps = (*sp); 
          r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  SG::ReadHandle<SpacePointContainer> spacepointsSCT(m_spacepointsSCTname);
  if (spacepointsSCT.isValid()) {

    SpacePointContainer::const_iterator spc  =  spacepointsSCT->begin();
    SpacePointContainer::const_iterator spce =  spacepointsSCT->end  ();

    for(; spc != spce; ++spc) {

      SpacePointCollection::const_iterator sp  = (*spc)->begin();
      SpacePointCollection::const_iterator spe = (*spc)->end  ();
      for(; sp != spe; ++sp) {

        if(m_useasso){
          bool u1=false; bool u2=false;
          const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=m_assotool->isUsed(*p1);
          const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=m_assotool->isUsed(*p2);
          if(u1 || u2){continue;}
        }

        if(!m_loadFull) r_rmin=m_r2min;
 	double r = (*sp)->r(); if(r<r_rmin || r>=r_rmax) continue;
	int   ir = int(r*irstep); 
	const Trk::SpacePoint* sps = (*sp); 
        r_Sorted[ir].push_back(sps); ++r_map[ir];
	if(r_map[ir]==1) r_index[m_nr++] = ir;
	++m_ns;
      }
    }
  }

  // Get sct overlap space points containers from store gate 
  //
  SG::ReadHandle<SpacePointOverlapCollection> spacepointsOverlap(m_spacepointsOverlapname);
  if (spacepointsOverlap.isValid()) {
    SpacePointOverlapCollection::const_iterator sp  = spacepointsOverlap->begin();
    SpacePointOverlapCollection::const_iterator spe = spacepointsOverlap->end  ();

    for (; sp!=spe; ++sp) {

      if(m_useasso){
        bool u1=false; bool u2=false;
        const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=m_assotool->isUsed(*p1);
        const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=m_assotool->isUsed(*p2);
        if(u1 || u2){continue;}
      }

      double r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
      int   ir = int(r*irstep); 
      const Trk::SpacePoint* sps = (*sp); 
      r_Sorted[ir].push_back(sps); ++r_map[ir];
      if(r_map[ir]==1) r_index[m_nr++] = ir;
      ++m_ns;
    }
  }

  fillLists();  //Fill the R-phi sectors with the corresponding space points
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT)
{
  erase();

  double irstep = 1./r_rstep;

  if(m_loadFull && vPixel.size()){
    // Get pixel space points containers from store gate 
    //
    SG::ReadHandle<SpacePointContainer> spacepointsPix(m_spacepointsPixname);
    if (spacepointsPix.isValid()) {
      SpacePointContainer::const_iterator spce =  spacepointsPix->end  ();

      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {
	
	SpacePointContainer::const_iterator  w =  spacepointsPix->indexFind((*l));
	if(w==spce) continue;


        SpacePointCollection::const_iterator sp  = (*w)->begin();
        SpacePointCollection::const_iterator spe = (*w)->end  ();
        for(; sp != spe; ++sp) {

	  double r = (*sp)->r(); if(r<0. || r>=r_rmax) continue;
	  int   ir = int(r*irstep); 
	  const Trk::SpacePoint* sps = (*sp); 
          r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate 
  //
  if(vSCT.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT(m_spacepointsSCTname);
    if (spacepointsSCT.isValid()) {

      //SpacePointContainer::const_iterator spc  =  m_spacepointsSCT->begin();
      SpacePointContainer::const_iterator spce =  spacepointsSCT->end  ();

      std::vector<IdentifierHash>::const_iterator l = vSCT.begin(), le = vSCT.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	SpacePointContainer::const_iterator  w =  spacepointsSCT->indexFind((*l));
	if(w==spce) continue;

        SpacePointCollection::const_iterator sp  = (*w)->begin();
        SpacePointCollection::const_iterator spe = (*w)->end  ();
        for(; sp != spe; ++sp) {

          if(m_useasso){
            bool u1=false; bool u2=false;
            const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=m_assotool->isUsed(*p1);
            const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=m_assotool->isUsed(*p2);
            if(u1 || u2){continue;}
          }

          if(!m_loadFull) r_rmin=m_r2min;
 	  double r = (*sp)->r(); if(r<r_rmin || r>=r_rmax) continue;
	  int   ir = int(r*irstep); 
	  const Trk::SpacePoint* sps = (*sp); 
          r_Sorted[ir].push_back(sps); ++r_map[ir];
	  if(r_map[ir]==1) r_index[m_nr++] = ir;
	  ++m_ns;
        }
      }
    }
  }

  fillLists();  //Fill the R-phi sectors with the corresponding space points
}

///////////////////////////////////////////////////////////////////
// Methods to initialize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> >* 
InDet::TRT_SeededSpacePointFinder_ATL::find2Sp(const Trk::TrackParameters& tP)
{
  const double pi2 = 2.*M_PI;

  m_outputListBuffer.clear();

  ///Get the phi segment index to use
  double F = 0.;
  double GPx=tP.position().x(); double GPy=tP.position().y();
  F=atan2(GPy,GPx); if(F<0.) F+=pi2;
  int f = int(F*m_sF);
  if (f < 0)
    f += m_fNmax;
  else if (f > m_fNmax)
    f -= m_fNmax;


  production2Spb (tP,f); //Get a list of SP pairs.

  if(msgLvl(MSG::DEBUG)) {
    m_nprint=1; msg(MSG::DEBUG)<<(*this)<<endmsg;
  }

  if(m_outputListBuffer.size()>10000.) m_outputListBuffer.clear();

  return &m_outputListBuffer;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpEvent(out);
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_fieldServiceHandle.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 42-m_spacepointsSCTname.key().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlapname.key().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  n     = 42-m_assotool.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode(); 
  if(mode<0 || mode>8 ) mode = 8; 

  n     = 62-fieldmode[mode].size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Tool for magentic field | "<<m_fieldServiceHandle.type()<<s1
     <<std::endl;
  out<<"| Association tool        | "<<m_assotool.type()<<s2
     <<std::endl;
  out<<"| SCT      space points   | "<<m_spacepointsSCTname.key()<<s3
     <<std::endl;
  out<<"| Overlap  space points   | "<<m_spacepointsOverlapname.key()<<s4
     <<std::endl;
  out<<"| Magnetic field mode     | "<<fieldmode[mode]<<s5
     <<std::endl;
  out<<"| pTmin  (mev)            | "
     <<std::setw(12)<<std::setprecision(5)<<m_ptmin
     <<"                              |"<<std::endl;
  out<<"| max radius SP           | "
     <<std::setw(12)<<std::setprecision(5)<<r_rmax 
     <<"                              |"<<std::endl;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<r_rstep
     <<"                              |"<<std::endl;
  out<<"| min radius second SP(3) | "
     <<std::setw(12)<<std::setprecision(5)<<m_r2min
     <<"                              |"<<std::endl;
  out<<"| min radius first SP(3)  | "
     <<std::setw(12)<<std::setprecision(5)<<m_r12min
     <<"                              |"<<std::endl;
  out<<"| max radius first  SP(3) | "
     <<std::setw(12)<<std::setprecision(4)<<m_r1max
     <<"                              |"<<std::endl;
  out<<"| min seeds dZ/dR         | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrmin
     <<"                              |"<<std::endl;
  out<<"| max seeds dZ/dR         | "
     <<std::setw(12)<<std::setprecision(5)<<m_dzdrmax
     <<"                              |"<<std::endl;
  out<<"| momentum chi2 cut       | "
     <<std::setw(12)<<std::setprecision(5)<<m_xiC
     <<"                              |"<<std::endl;
  out<<"| polar angle chi2 cut    | "
     <<std::setw(12)<<std::setprecision(5)<<m_xiTC
     <<"                              |"<<std::endl;
  out<<"| azimuthal angle chi2 cut    | "
     <<std::setw(12)<<std::setprecision(5)<<m_xiFC
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////
namespace {
    class StreamState
    {
    public:
      StreamState(std::ostream& out)
           : m_out(out), m_prec(out.precision())
       {
       }

       ~StreamState()
       {
           m_out.precision(m_prec);
       }

    private:
       std::ostream& m_out;
       std::streamsize m_prec;
    };
}

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dumpEvent( MsgStream& out ) const
{
  const double pi2    = 2.*M_PI;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| m_ns                    | "
     <<std::setw(12)<<m_ns
     <<"                              |"<<std::endl;
  out<<"|---------------------------------------------------------------------|"
     <<std::endl;

  if(msgLvl(MSG::DEBUG)) return out; 

  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;

  out<<"|  Azimuthal  |    n   | z[ 0] | z[ 1] | z[ 2] | z[ 3] | z[4]  |";
  out<<" z[ 5] | z[ 6] | z[ 7] | z[ 8] | z[ 9] | z[10] |"
     <<std::endl;
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  
  double sF1 = pi2/double(m_fNmax+1);
  
  StreamState restore_precision(cout); 
  for(int f=0; f<=m_fNmax; ++f) {
    out<<"|  "
       <<std::setw(10)<<std::setprecision(4)<<sF1*double(f)<<" | "
       <<std::setw(6)<<rf_map[f]<<" |";
    out<<std::endl;
  }
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<std::endl;
  out<<std::endl;
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_SeededSpacePointFinder_ATL::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_SeededSpacePointFinder_ATL& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_SeededSpacePointFinder_ATL& se)
{ 
  return se.dump(sl); 
}   

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::buildFrameWork() 
{

  m_ns = m_nr = m_nrf = 0;

  // Build radius sorted containers
  //
  r_size = int((r_rmax+.1)/r_rstep);
  r_Sorted = new std::list<const Trk::SpacePoint*>[r_size];
  r_index  = new int[r_size];
  r_map    = new int[r_size];  
  m_nr   = 0; for(int i=0; i!=r_size; ++i) {r_index[i]=0; r_map[i]=0;}

  // Build radius-azimuthal sorted containers
  //
  const double pi2    = 2.*M_PI           ;
  const int   NFmax   = 530               ;
  const double sFmax  = double(NFmax )/pi2;
  m_sF        = m_ptmin /60. ; if(m_sF    >sFmax ) m_sF    = sFmax  ;
  m_fNmax     = int(pi2*m_sF); if(m_fNmax >=NFmax) m_fNmax = NFmax-1;
  m_nrf   = 0; for(int i=0; i!= 530; ++i) {rf_index  [i]=0; rf_map  [i]=0;}

}

///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::fillLists() 
{
  const double pi2 = 2.*M_PI;
  std::list<const Trk::SpacePoint*>::iterator r;
  
  for(int i=0; i!= r_size;  ++i) {
    if(!r_map[i]) continue;
    r = r_Sorted[i].begin();

    while(r!=r_Sorted[i].end()) {

      // Azimuthal angle sort
      //
      double F = (*r)->phi(); if(F<0.) F+=pi2;
      int   f = int(F*m_sF);
      if (f < 0) 
        f += m_fNmax;
      else if (f > m_fNmax)
        f -= m_fNmax;
      int isBRL = 1000; int isLYR = 1000; int DD = 1000;
 
      geoInfo((*r),isBRL,isLYR);

      // Use 4 lower bits (Mask ==  ((2^4 -1) == 15)) for isLYR
      // the upper 28 bits for isBRL (including sign)
      DD = ((isBRL+3) << 4) + (isLYR & 15);

      rf_Sorted[f].push_back(std::make_pair((*r),DD)); 
      if(!rf_map[f]++) rf_index[m_nrf++] = f;

      r_Sorted[i].erase(r++);
    }
    r_map[i] = 0;
  }

  m_nr    = 0;
}

///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::erase()
{
  for(int i=0; i!=m_nr;    ++i) {
    int n = r_index[i]; r_map[n] = 0;
    r_Sorted[n].erase(r_Sorted[n].begin(),r_Sorted[n].end());
  }

  for(int i=0; i!=m_nrf;   ++i) {
    int n = rf_index[i]; rf_map[n] = 0;
    rf_Sorted[n].erase(rf_Sorted[n].begin(),rf_Sorted[n].end());
  }

  m_ns    = 0;
  m_nr    = 0;
  m_nrf   = 0;
}

// // // // // // // // // // // // // // // // // // // // // // // // // //

// Monotonic function of the angle to map the comparison to replace atan2
// Computation is not in the inner loop, so trigonometric functions are OK
double
rollrating(double angle) {
  while (angle < 0) {
    angle += 2*M_PI;
  }
  while (angle > 2*M_PI) {
    angle -= 2*M_PI;
  }
  double rotations = angle/(2*M_PI);
  double rquadrant = rotations*4.0;
  long quadrant = (long)rquadrant & 3;
  double twist;
  if ((quadrant & 1) != 0) {
    twist = cos(angle);
  }
  else {
    twist = sin(angle);
  }
  twist *= twist;
  quadrant -= ((quadrant & 2) << 1);
  return quadrant + twist;
}

// Since the theta and phi cuts in cutTP are essentially
// checks for the angle to be within a specified region,
// only the upper and the lower boundaries have to calculated,
// once again, not in the inner loop but just once
// The logic is: 
// {lower <= angle <= upper_} necessary and sufficient for
// {rollrating(lower) <= rollrating(angle) <= rollrating(upper)}  
void
bracket_angle(double angle, double delta,
	      double *min, double *max) {
  double amin = rollrating(angle - delta);
  double amax = rollrating(angle + delta);
  if (delta >= M_PI) {
    amin = 0.0;
    amax = 4.0;
  }
  *min = amin;
  *max = amax;
}


// rotrating(sin(angle), cos(angle)) == rollrating(angle)
// Called many times, for each pair still in consideration
// Performance critical, so no SQRTs or transcendentals here
inline double
rotrating(double y, double x) {
  long asign_x = (long)(x < 0.0);
  long asign_y = (long)(y < 0.0);
  long quadrant = -(asign_y << 1) + (asign_y ^ asign_x);
  double x2 = x*x;
  double y2 = y*y;
  double denominator = x2 + y2;
  double numerator = ((quadrant & 1) != 0) ? x2 : y2;
  return (double)quadrant + numerator/denominator;
}

void
InDet::TRT_SeededSpacePointFinder_ATL::production2Spb(const Trk::TrackParameters& tP,int phi)
{ 
  uint64_t spcount = 0;
  // // // // // // <Fill the invar_bypass // // // // // // //

  //const Trk::MeasuredAtaStraightLine &ntP = 
  //dynamic_cast<const Trk::MeasuredAtaStraightLine&>(tP);
  const AmgVector(5)& pTS=tP.parameters();
  double xiC = m_xiC; double xiTC = m_xiTC; double xiFC = m_xiFC;


  const AmgSymMatrix(5)* vCM = tP.covariance();
  
  double sPhi = (*vCM)(2,2)  ; //Sigma on TRT segment azimuthal angle
  double sTheta = (*vCM)(3,3); //Sigma on TRT segment polar angle
  double sp = (*vCM)(4,4)    ; //Sigma on TRT segment inverse momentum estimate

  double ipdelta = sqrt(xiC*sp);

  invar_bypass_struct tmp_invar_bypass;
  tmp_invar_bypass.invp_min = pTS[4] - ipdelta;
  tmp_invar_bypass.invp_max = pTS[4] + ipdelta;

  tmp_invar_bypass.invp_min2 = tmp_invar_bypass.invp_min*tmp_invar_bypass.invp_min;
  tmp_invar_bypass.invp_max2 = tmp_invar_bypass.invp_max*tmp_invar_bypass.invp_max;

  double theta_center = pTS[3];
  double theta_delta = sqrt(xiTC*sTheta);

  double phi_center = pTS[2];
  double phi_delta = sqrt(xiFC*sPhi);

  bracket_angle(theta_center, theta_delta,
		&(tmp_invar_bypass.min_theta), &(tmp_invar_bypass.max_theta));
  bracket_angle(phi_center, phi_delta,
		&(tmp_invar_bypass.min_phi), &(tmp_invar_bypass.max_phi));

  // // // // // // Fill the invar_bypass> // // // // // // //

  ///Get the momentum information from the track parameters 
  double x0=tP.position().x()  ; 
  double y0=tP.position().y()  ;
  double z0=tP.position().z()  ;
  double H[3]; double gP[3] = {x0,y0,z0}; 
  m_fieldService->getField(gP,H);

  //need conversion kilotesla -> kilogauss - Previously used getMagneticFiledKiloGauss, whereas new function returns value in kiloTesla...
  H[0] *= 10000;
  H[1] *= 10000;
  H[2] *= 10000;

  std::list<std::pair<const Trk::SpacePoint*,int> >::iterator r0,r0e,r,re, rb;
  const Trk::SpacePoint* SpToPair = 0;

  int m_nri = 0;

  ///Set up the azinuthal width to look for SPs
  //
  int fmin=phi; int fmax=phi;
  if(m_search){fmin = phi-1; fmax = phi+1;}
  for(int f=fmin; f<=fmax; ++f) {
    int j=0; f<0 ? j=f+m_fNmax+1 : f>m_fNmax ? j=f-m_fNmax-1 : j=f;
    if(!rf_map[j]){
      continue;
    }
    r0 = rf_Sorted[j].begin();
    r0e = rf_Sorted[j].end();

    ///Fill a list with the SP in the azimouthal region indicated by the TRT track segment
    for(; r0!=r0e; ++r0){
      if((((*r0).first)->r() > m_r1max) || 
	 (((*r0).first)->r() < m_r2min)) {
	continue; //Fill only the SCT SPs
      }
      newRfi_Sorted.push_back(*r0);
      m_nri++;      
    }
  }

  if(newRfi_Sorted.size()>5000 && newRfi_Sorted.size()<=10000) {
    newRfi_Sorted.erase(newRfi_Sorted.begin(),newRfi_Sorted.end()); 
    int fmin=phi; int fmax=phi;
    for(int f=fmin; f<=fmax; ++f) {
      int j=0; f<0 ? j=f+m_fNmax+1 : f>m_fNmax ? j=f-m_fNmax-1 : j=f;
      if(!rf_map[j]){
	continue;
      }
      r0 = rf_Sorted[j].begin();
      r0e = rf_Sorted[j].end();
      
      ///Fill a list with the SP in the azimouthal region indicated by the TRT track segment
      for(; r0!=r0e; ++r0){
	if((((*r0).first)->r()>m_r1max) || (((*r0).first)->r()<m_r2min)) {
	  continue; //Fill only the SCT SPs
	}
	newRfi_Sorted.push_back(*r0);
	m_nri++;      
      }
    }
  }
  if(newRfi_Sorted.size()>10000) {
    newRfi_Sorted.erase(newRfi_Sorted.begin(),newRfi_Sorted.end());
    return;
  }

  newRfi_Sorted.sort(MyNewDataSortPredicate());

  spcount = newRfi_Sorted.size();

  r  = newRfi_Sorted.begin();
  re = newRfi_Sorted.end();

  std::vector<bypass_struct> tmp_prod_bypass;
  tmp_prod_bypass.reserve(spcount);
  Trk::SpacePoint const**vrp = new Trk::SpacePoint const*[spcount];
  double *rk = new double[spcount];
  long *geo_info = new long[spcount];
  double *zSP = new double[spcount];

  if (vrp != 0 && rk != 0 && geo_info != 0 && zSP != 0) {

    // // // // // // <Fill prod_bypass and the local array // // // //
    for (long i = 0; r != re; i++, r++) {
      const Trk::SpacePoint *vrpi = (*r).first;

      geo_info[i] = (*r).second;
      vrp[i] = vrpi;
      rk[i] = vrpi->r();
      
      double X = vrpi->globalPosition().x() - x0;
      double Y = vrpi->globalPosition().y() - y0;
      double zSPi = vrpi->globalPosition().z();
      zSP[i] = zSPi;
      double Z = zSPi - z0;

      double RR = X*X + Y*Y;
      double R = sqrt(RR);
      double invR = 1.0/R;

      double a = X*invR;
      double b = Y*invR;
     
      tmp_prod_bypass.push_back(bypass_struct()); 
      tmp_prod_bypass.back().X = X;
      tmp_prod_bypass.back().Y = Y;
      tmp_prod_bypass.back().Z = Z;
      
      tmp_prod_bypass.back().R = R;
      tmp_prod_bypass.back().invR = invR;

      tmp_prod_bypass.back().a = a;
      tmp_prod_bypass.back().b = b;
    }

    // // // // // // Fill prod_bypass and the local array> // // // //
    
    ///////////////////////////////////////////////////////

    if (m_doCosmics) { // no need to check this every time in the loop
      for (long i = 0; i < (long)spcount; i++) {
	SpToPair = 0;
	const Trk::SpacePoint *up = vrp[i];
	for (long j = i + 1; j < (long)spcount; j++) {
	  const Trk::SpacePoint *bp = vrp[j];
	  SpToPair = bp;
	  m_outputListBuffer.push_back(std::make_pair(up, SpToPair));
	}
	if(!SpToPair) {
	  m_outputListBuffer.push_back(std::make_pair(up, up));
	}
      }
    }
    else { // (!m_doCosmics)
      for (long i = 0; i < (long)spcount; i++) {
	SpToPair = 0;
	const Trk::SpacePoint *up = vrp[i];
	double R = rk[i];
	if(R<m_r12min) {
	  continue;
	}   
	double Z = zSP[i];
	long geoi = geo_info[i];
	int isBU = (geoi >> 4)-3;
	int eleU = geoi & 15;
    
	for (long j = i + 1; j < (long)spcount; j++) {
	  const Trk::SpacePoint *bp = vrp[j];
	  double Zb = zSP[j];
	  double Rb = rk[j];
	  long geoj = geo_info[j];
	  int isBB = (geoj >> 4)-3;
	  int eleB = geoj & 15;
	  // // // // // // // // // // // // // // // // // // // // // //
	
	  // Equivalent to  {
	  // if ((isBU == 0) && (isBB != isBU)) continue;
	  // if((isBU == isBB) && (eleU <= eleB)) continue;
	  // }
	  // Rather cryptic but 2 to 3 times faster 
	  // than the 4 branches above...

	  int Bd = (isBU - isBB) | (isBB - isBU);
	  int Ed = (eleB - eleU);
	  int BUzero = (isBU | -isBU);
	  if (((BUzero | ~Bd) & (Bd | Ed) & (((unsigned)(-1) >> 1) + 1))
	      == 0) {
	    continue;
	  }	
	  
	  // // // // // // // // // // // // // // // // // // // // // //
	  double dR = R - Rb;
	  double dZ = Z - Zb;
	  double dz_min = m_dzdrmin*dR;
	  double dz_max = m_dzdrmax*dR;
	  if (dZ < dz_min || dZ > dz_max) {
	    continue;//Should be within the +-2.5 pseudorapidity range
	  }
	  if(m_fieldServiceHandle->solenoidOn()) {
	    if(!cutTPb(tmp_invar_bypass, tmp_prod_bypass,i, j, H[2])) {
	      continue;
	    }
	  }
	  SpToPair = bp;
	  m_outputListBuffer.push_back(std::make_pair(up, SpToPair));
	}
	if(!SpToPair) {
	  m_outputListBuffer.push_back(std::make_pair(up, up));
	}
      }
    }
  }
  else { // from  if (prod_bypass != 0 &&...)
    // Not enough memory
    // Do nothing; release the memory and return
  }

  delete []zSP;
  delete []geo_info;
  delete []rk;
  delete []vrp;

  newRfi_Sorted.erase(newRfi_Sorted.begin(),newRfi_Sorted.end());
}

// comment out to enable angle discontinuity correction
// so that e.g. angles PI-epsilon and -Pi+epsilon are treated
// as being 2*epsilon apart as they should, instead of 2*Pi-2*epsilon
// as they would w/o this correction
//#define ANGLE_DISCO_COMPAT

bool
InDet::TRT_SeededSpacePointFinder_ATL::cutTPb(const invar_bypass_struct  &tmp_invar_bypass,
                                              const std::vector<bypass_struct> &tmp_prod_bypass,
                                              long bSP1, long bSP2, double H) 
{

  double inv_r2 = tmp_prod_bypass[bSP2].invR;
  double inv_r1 = tmp_prod_bypass[bSP1].invR; // == u1 in original cutTP
  double r1 = tmp_prod_bypass[bSP1].R;

  double inv_rr2 = inv_r2*inv_r2;
  double x2 = tmp_prod_bypass[bSP2].X;
  double y2 = tmp_prod_bypass[bSP2].Y;
  double a1 = tmp_prod_bypass[bSP1].a;
  double b1 = tmp_prod_bypass[bSP1].b;

  double u2 = (a1*x2 + b1*y2)*inv_rr2;
  double v2 = (a1*y2 - b1*x2)*inv_rr2;

  double A = v2/(u2 - inv_r1);
  double B = 2.0*(v2 - A*u2);
  double CC = B*B/(1.0 + A*A);
  double rcrc = CC*r1*r1;
  double z1 =  tmp_prod_bypass[bSP1].Z;
  double T = -z1/(r1*(1.0 + 0.04*rcrc));

  if(H==0.) return false;
  
  double invpSignature = B*H;

  double invP2 = CC/(0.03*0.03*H*H*(1.0 + T*T));

  if (invpSignature >= 0 && invP2*0.9*0.9*m_ptmin*m_ptmin > 1.0) {
    return false;
  }

  double invp_min = tmp_invar_bypass.invp_min;
  double invp_max = tmp_invar_bypass.invp_max;

  double invp_min2 = tmp_invar_bypass.invp_min2;
  double invp_max2 = tmp_invar_bypass.invp_max2;
  
  if (invp_min >= 0) {
    if (invpSignature < 0 || invP2 < invp_min2) {
      return false;
    }
  }
  else {
    if (invpSignature < 0 && invP2 > invp_min2) {
      return false;
    }
  }
  if (invp_max >= 0) {
    if (invpSignature >= 0 && invP2 > invp_max2) {
      return false;
    }
  }
  else {
    if (invpSignature >= 0 || invP2 < invp_max2) {
      return false;
    }
  }
  
  //Estimate the seed polar angle. Make a chi2 cut based on that suggested by the TRT segment

  double theta_rating = rotrating(1.0, T);
  double tmin = tmp_invar_bypass.min_theta;
  double tmax = tmp_invar_bypass.max_theta;

  if (tmin > tmax) {
#ifndef ANGLE_DISCO_COMPAT
  // correct math but incompatible with old version
    if (theta_rating >= 0) {
      tmax += 4.0; 
    }
    else {
      tmin -= 4.0;
    }
#else
  // compatibility mode
    if (tmin + tmax <= 0) { 
      // center in "+" (YES, "+") range; any negative theta_rating => false
      tmax = 2.0; // forcing range into all positive
    }
    else {
      // center in "-" (YES, "-") range; any positive theta_rating => false
      tmin = -2.0; // forcing range into all negative
    }
#endif
  }

  if (theta_rating < tmin || theta_rating > tmax) {
    return false;
  }

  double phi_rating = rotrating(-(b1 + a1*A), -(a1 - b1*A));
  double pmin = tmp_invar_bypass.min_phi;
  double pmax = tmp_invar_bypass.max_phi;

  if (pmin > pmax) {
#ifndef ANGLE_DISCO_COMPAT
    // correct math but incompatible with old version
    if (phi_rating >= 0) {
      pmax += 4.0;
    }
    else {
      pmin -= 4.0; 
    }
#else
    // compatibility mode
    if (pmin + pmax <= 0) { 
      // center in "+" (YES, "+") range; any negative phi_rating => false
      pmax = 2.0; // forcing range into all positive
    }
    else {
      // center in "-" (YES, "-") range; any positive phi_rating => false
      pmin = -2.0; // forcing range into all negative
    }
#endif
  }

  if (phi_rating < pmin || phi_rating > pmax) {
    return false;
  }

  return true;
}

// // // // // // // // // // // // // // // // // // // // // // // // // //


///////////////////////////////////////////////////////////////////
// Check whether the SP belongs to a barrel or an endcap element
///////////////////////////////////////////////////////////////////

void 
InDet::TRT_SeededSpacePointFinder_ATL::geoInfo(const Trk::SpacePoint* SP,int& isB,int& ld)
{  
  const Trk::PrepRawData* p1;
  const InDet::SCT_Cluster* c1;
  Identifier id;

  p1 = SP->clusterList().first;
  if(p1){
    c1=dynamic_cast<const InDet::SCT_Cluster*>(p1);
    if(c1){
      id=c1->detectorElement()->identify();
      isB = m_sctId->barrel_ec(id);
      ld = m_sctId->layer_disk(id);
    }
  }

}

///////////////////////////////////////////////////////////////////
// MagneticFieldProperties production
///////////////////////////////////////////////////////////////////

void  InDet::TRT_SeededSpacePointFinder_ATL::magneticFieldInit()
{
Trk::MagneticFieldProperties* pMF = 0; 
 if(m_fieldmode == "NoField") pMF = new Trk::MagneticFieldProperties(Trk::NoField  ); 
 else if(m_fieldmode == "MapSolenoid") pMF = new Trk::MagneticFieldProperties(Trk::FastField); 
 else pMF = new Trk::MagneticFieldProperties(Trk::FullField); 
 m_fieldprop = *pMF; 

 delete pMF; 
}
