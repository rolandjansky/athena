/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_SeededSpacePointFinder_ATL
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 04/15/2006 T.Koffas
///////////////////////////////////////////////////////////////////


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "TRT_SeededSpacePointFinderTool/TRT_SeededSpacePointFinder_ATL.h"
#include "SiSpacePointsSeed/SiSpacePointsSeed.h"

//Cluster collections
//
#include "InDetPrepRawData/SCT_Cluster.h"
//SCT Geometry
//
#include "InDetIdentifier/SCT_ID.h"

//Association tool
//

#include "StoreGate/ReadHandle.h"

#include <ostream>
#include <iomanip>
#include <set>

using namespace std;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_SeededSpacePointFinder_ATL::TRT_SeededSpacePointFinder_ATL
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_sctId(nullptr)
{
  m_fieldmode = "MapSolenoid"              ;
  m_ptmin     =   500.  ;  //Lowest pT of track.Up to 2000MeV bending in (r,phi) is +-4
  m_r_rmax      = 600.  ;  //Max radial extend of Si ID
  m_r_rmin      = 0.    ;  //Min radial extend for loading SPs in ID
  m_r_rstep     =  10.  ;  //Step size for SP and SCT histogramming
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
  m_loadFull  = true    ;  //Load all the Si space points, otherwise only from the last 3 SCT layers
  m_doCosmics = false   ;  //Disable seed selection cuts when reconstructing cosmics tracks


  declareInterface<ITRT_SeededSpacePointFinder>(this);

  declareProperty("MagneticFieldMode"     ,m_fieldmode             );
  declareProperty("pTmin"                 ,m_ptmin                 );
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
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_SeededSpacePointFinder_ATL::initialize()
{
  magneticFieldInit();

  // PRD-to-track association (optional)
  ATH_CHECK( m_prdToTrackMap.initialize( !m_prdToTrackMap.key().empty()));
  ATH_CHECK( m_fieldCondObjInputKey.initialize());
  StatusCode sc = detStore()->retrieve(m_sctId, "SCT_ID");
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Could not get SCT_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }

  // Build framework
  //

  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)){ dumpConditions(msg(MSG::DEBUG)); msg(MSG::DEBUG)<<endmsg; }

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

std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData>
InDet::TRT_SeededSpacePointFinder_ATL::newEvent () const
{
  std::unique_ptr<InDet::TRT_SeededSpacePointFinder_ATL::EventData> event_data_p = std::make_unique<InDet::TRT_SeededSpacePointFinder_ATL::EventData>();
  event_data_p->buildFrameWork(m_r_rmax, m_r_rstep, m_ptmin);
  // @TODO remove m_r_Sorted and directly fill m_rf_Sorted ?

  double irstep = 1./m_r_rstep;

  std::vector< std::vector<const Trk::SpacePoint*> > r_Sorted;
  r_Sorted.resize(event_data_p->m_r_size);

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

	  double r = (*sp)->r(); if(r<0. || r>=m_r_rmax) continue;
	  int   ir = int(r*irstep);
	  const Trk::SpacePoint* sps = (*sp);
          r_Sorted[ir].push_back(sps);
          ++event_data_p->m_r_map[ir];
	  if(event_data_p->m_r_map[ir]==1) event_data_p->m_r_index[event_data_p->m_nr++] = ir;
	  ++event_data_p->m_ns;
        }
      }
    }
  }

  SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
  if (!m_prdToTrackMap.key().empty()) {
    prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap);
    if (!prd_to_track_map.isValid()) {
      ATH_MSG_ERROR("Failed to read PRD to track association map.");
    }
  }
  // Get sct space points containers from store gate
  //
  SG::ReadHandle<SpacePointContainer> spacepointsSCT(m_spacepointsSCTname);
  if (spacepointsSCT.isValid()) {

    SpacePointContainer::const_iterator spc  =  spacepointsSCT->begin();
    SpacePointContainer::const_iterator spce =  spacepointsSCT->end  ();

    double r_rmin = (!m_loadFull) ? m_r2min : m_r_rmin;
    for(; spc != spce; ++spc) {

      SpacePointCollection::const_iterator sp  = (*spc)->begin();
      SpacePointCollection::const_iterator spe = (*spc)->end  ();
      for(; sp != spe; ++sp) {

        if(prd_to_track_map.cptr()){
          bool u1=false; bool u2=false;
          const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=prd_to_track_map->isUsed(*p1);
          const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=prd_to_track_map->isUsed(*p2);
          if(u1 || u2){continue;}
        }

        double r = (*sp)->r(); if(r<r_rmin || r>=m_r_rmax) continue;
	int   ir = int(r*irstep);
	const Trk::SpacePoint* sps = (*sp);
        r_Sorted[ir].push_back(sps); ++event_data_p->m_r_map[ir];
	if(event_data_p->m_r_map[ir]==1) event_data_p->m_r_index[event_data_p->m_nr++] = ir;
	++event_data_p->m_ns;
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

      if(prd_to_track_map.cptr()){
        bool u1=false; bool u2=false;
        const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=prd_to_track_map->isUsed(*p1);
        const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=prd_to_track_map->isUsed(*p2);
        if(u1 || u2){continue;}
      }

      double r = (*sp)->r(); if(r<0. || r>=m_r_rmax) continue;
      int   ir = int(r*irstep);
      const Trk::SpacePoint* sps = (*sp);
      r_Sorted[ir].push_back(sps); ++event_data_p->m_r_map[ir];
      if(event_data_p->m_r_map[ir]==1) event_data_p->m_r_index[event_data_p->m_nr++] = ir;
      ++event_data_p->m_ns;
    }
  }

  fillLists(r_Sorted, *event_data_p);  //Fill the R-phi sectors with the corresponding space points
  return std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData>(event_data_p.release());
}

///////////////////////////////////////////////////////////////////
// Initialize tool for new region
///////////////////////////////////////////////////////////////////

std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData> InDet::TRT_SeededSpacePointFinder_ATL::newRegion
(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const
{
  std::unique_ptr<InDet::TRT_SeededSpacePointFinder_ATL::EventData> event_data_p = std::make_unique<InDet::TRT_SeededSpacePointFinder_ATL::EventData>();
  event_data_p->buildFrameWork(m_r_rmax, m_r_rstep, m_ptmin);

  std::vector< std::vector<const Trk::SpacePoint*> > r_Sorted;
  r_Sorted.resize(event_data_p->m_r_size);

  double irstep = 1./m_r_rstep;

  if(m_loadFull && vPixel.size()){
    // Get pixel space points containers from store gate
    //
    SG::ReadHandle<SpacePointContainer> spacepointsPix(m_spacepointsPixname);
    if (spacepointsPix.isValid()) {
      std::vector<IdentifierHash>::const_iterator l = vPixel.begin(), le = vPixel.end();

      // Loop through all trigger collections
      //
      for(; l!=le; ++l) {

	const SpacePointCollection  *w =  spacepointsPix->indexFindPtr(*l);
	if(w==nullptr) continue;
        SpacePointCollection::const_iterator sp  = w->begin();
        SpacePointCollection::const_iterator spe = w->end  ();
        for(; sp != spe; ++sp) {

	  double r = (*sp)->r(); if(r<0. || r>=m_r_rmax) continue;
	  int   ir = int(r*irstep);
	  const Trk::SpacePoint* sps = (*sp);
          r_Sorted[ir].push_back(sps); ++event_data_p->m_r_map[ir];
	  if(event_data_p->m_r_map[ir]==1) event_data_p->m_r_index[event_data_p->m_nr++] = ir;
	  ++event_data_p->m_ns;
        }
      }
    }
  }

  // Get sct space points containers from store gate
  //
  if(vSCT.size()) {

    SG::ReadHandle<SpacePointContainer> spacepointsSCT(m_spacepointsSCTname);
    if (spacepointsSCT.isValid()) {

      SG::ReadHandle<Trk::PRDtoTrackMap>  prd_to_track_map;
      if (!m_prdToTrackMap.key().empty()) {
        prd_to_track_map=SG::ReadHandle<Trk::PRDtoTrackMap>(m_prdToTrackMap);
        if (!prd_to_track_map.isValid()) {
          ATH_MSG_ERROR("Failed to read PRD to track association map.");
        }
      }
      std::vector<IdentifierHash>::const_iterator l = vSCT.begin(), le = vSCT.end();

      // Loop through all trigger collections
      //
      double r_rmin = (!m_loadFull) ? m_r2min : m_r_rmin;
      for(; l!=le; ++l) {

	const SpacePointCollection  *w =  spacepointsSCT->indexFindPtr(*l);
	if(w==nullptr) continue;
        SpacePointCollection::const_iterator sp  = w->begin();
        SpacePointCollection::const_iterator spe = w->end  ();
        for(; sp != spe; ++sp) {

          if(prd_to_track_map.cptr()){
            bool u1=false; bool u2=false;
            const Trk::PrepRawData* p1=(*sp)->clusterList().first; u1=prd_to_track_map->isUsed(*p1);
            const Trk::PrepRawData* p2=(*sp)->clusterList().second;u2=prd_to_track_map->isUsed(*p2);
            if(u1 || u2){continue;}
          }

          double r = (*sp)->r(); if(r<r_rmin || r>=m_r_rmax) continue;
	  int   ir = int(r*irstep);
	  const Trk::SpacePoint* sps = (*sp);
          r_Sorted[ir].push_back(sps); ++event_data_p->m_r_map[ir];
	  if(event_data_p->m_r_map[ir]==1) event_data_p->m_r_index[event_data_p->m_nr++] = ir;
	  ++event_data_p->m_ns;
        }
      }
    }
  }

  fillLists(r_Sorted,*event_data_p);  //Fill the R-phi sectors with the corresponding space points
  return std::unique_ptr<InDet::ITRT_SeededSpacePointFinder::IEventData>(event_data_p.release());
}

///////////////////////////////////////////////////////////////////
// Methods to initialize different strategies of seeds production
// with two space points with or without vertex constraint
///////////////////////////////////////////////////////////////////

std::list<std::pair<const Trk::SpacePoint*, const Trk::SpacePoint*> >
InDet::TRT_SeededSpacePointFinder_ATL::find2Sp(const EventContext& ctx,
                                               const Trk::TrackParameters& tP,
                                               ITRT_SeededSpacePointFinder::IEventData &virt_event_data) const
{
  InDet::TRT_SeededSpacePointFinder_ATL::EventData &event_data = EventData::getPrivateEventData(virt_event_data);

  const double pi2 = 2.*M_PI;

  /** List of produced space point seeds  */
  std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> > outputListBuffer;

  ///Get the phi segment index to use
  double F = 0.;
  double GPx=tP.position().x(); double GPy=tP.position().y();
  F=atan2(GPy,GPx); if(F<0.) F+=pi2;
  int f = int(F*event_data.m_sF);
  if (f < 0)
    f += event_data.m_fNmax;
  else if (f > event_data.m_fNmax)
    f -= event_data.m_fNmax;


  production2Spb (ctx, tP,f, outputListBuffer,event_data); //Get a list of SP pairs.

  if(msgLvl(MSG::DEBUG)) {
     dumpEvent( msg(MSG::DEBUG), event_data);
     dumpConditions( msg(MSG::DEBUG));
     msg(MSG::DEBUG) << endmsg;
  }

  if(outputListBuffer.size()>10000.) outputListBuffer.clear();

  return outputListBuffer;
}


///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dumpConditions( MsgStream& out ) const
{
  int n = 42-m_fieldCondObjInputKey.key().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 42-m_spacepointsSCTname.key().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 42-m_spacepointsOverlapname.key().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  std::string fieldmode[9] ={"NoField"       ,"ConstantField","SolenoidalField",
			     "ToroidalField" ,"Grid3DField"  ,"RealisticField" ,
			     "UndefinedField","AthenaField"  , "?????"         };

  int mode = m_fieldprop.magneticFieldMode();
  if(mode<0 || mode>8 ) mode = 8;

  n     = 62-fieldmode[mode].size();
  std::string s5; for(int i=0; i<n; ++i) s5.append(" "); s5.append("|");

  out<<"|---------------------------------------------------------------------|"
     <<std::endl;
  out<<"| Key of magentic field condition Object | "<<m_fieldCondObjInputKey.key()<<s1
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
     <<std::setw(12)<<std::setprecision(5)<<m_r_rmax
     <<"                              |"<<std::endl;
  out<<"| radius step             | "
     <<std::setw(12)<<std::setprecision(5)<<m_r_rstep
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

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dumpEvent( MsgStream& out, InDet::TRT_SeededSpacePointFinder_ATL::EventData &event_data) const
{
  const double pi2    = 2.*M_PI;
  out<<"|---------------------------------------------------------------------|"
     <<"\n";
  out<<"| m_ns                    | "
     <<std::setw(12)<<event_data.m_ns
     <<"                              |"<<"\n";
  out<<"|---------------------------------------------------------------------|"
     <<"\n";

  if(msgLvl(MSG::DEBUG)) return out;

  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<"\n";

  out<<"|  Azimuthal  |    n   | z[ 0] | z[ 1] | z[ 2] | z[ 3] | z[4]  |";
  out<<" z[ 5] | z[ 6] | z[ 7] | z[ 8] | z[ 9] | z[10] |"
     <<"\n";
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<"\n";

  double sF1 = pi2/double(event_data.m_fNmax+1);

  //StreamState restore_precision(out);
  auto prec(out.precision());
  for(int f=0; f<=event_data.m_fNmax; ++f) {
    out<<"|  "
       <<std::setw(10)<<std::setprecision(4)<<sF1*double(f)<<" | "
       <<std::setw(6)<<event_data.m_rf_map[f]<<" |";
    out<<"\n";
  }
  out<<"|-------------|--------|-------|-------|-------|-------|-------|";
  out<<"-------|-------|-------|-------|-------|-------|"
     <<"\n";
  out<<endmsg;
  out.precision(prec);
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_SeededSpacePointFinder_ATL::dump( MsgStream& out ) const
{
   return dumpConditions(out);
}

std::ostream& InDet::TRT_SeededSpacePointFinder_ATL::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Initiate frame work for seed generator
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::EventData::buildFrameWork(double r_rmax, double r_rstep, double ptmin)
{

  m_ns = m_nr = m_nrf = 0;

  // Build radius sorted containers
  //
  m_r_size = int((r_rmax+.1)/r_rstep);
  m_r_index  = new int[m_r_size];
  m_r_map    = new int[m_r_size];
  m_nr   = 0; for(int i=0; i!=m_r_size; ++i) {m_r_index[i]=0; m_r_map[i]=0;}

  // Build radius-azimuthal sorted containers
  //
  const double pi2    = 2.*M_PI           ;
  const int   NFmax   = 530               ;
  const double sFmax  = double(NFmax )/pi2;
  m_sF        = ptmin /60. ; if(m_sF    >sFmax ) m_sF    = sFmax  ;
  m_fNmax     = int(pi2*m_sF); if(m_fNmax >=NFmax) m_fNmax = NFmax-1;
  m_nrf   = 0; for(int i=0; i!= 530; ++i) {m_rf_index  [i]=0; m_rf_map  [i]=0;}

}

///////////////////////////////////////////////////////////////////
// Initiate space points seed maker
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::fillLists(std::vector< std::vector<const Trk::SpacePoint*> > &r_Sorted,
                                                      InDet::TRT_SeededSpacePointFinder_ATL::EventData &event_data) const
{
  assert( static_cast<size_t>(event_data.m_r_size) == r_Sorted.size());
  const double pi2 = 2.*M_PI;

  for(int i=0; i!= event_data.m_r_size;  ++i) {
    if(!event_data.m_r_map[i]) continue;
    for(const Trk::SpacePoint *space_point : r_Sorted[i]) {

      // Azimuthal angle sort
      //
      double F = space_point->phi(); if(F<0.) F+=pi2;
      int   f = int(F*event_data.m_sF);
      if (f < 0)
        f += event_data.m_fNmax;
      else if (f > event_data.m_fNmax)
        f -= event_data.m_fNmax;
      int isBRL = 1000; int isLYR = 1000; int DD = 1000;

      geoInfo(space_point,isBRL,isLYR);

      // Use 4 lower bits (Mask ==  ((2^4 -1) == 15)) for isLYR
      // the upper 28 bits for isBRL (including sign)
      DD = ((isBRL+3) << 4) + (isLYR & 15);

      event_data.m_rf_Sorted[f].push_back(std::make_pair(space_point,DD));
      if(!event_data.m_rf_map[f]++) event_data.m_rf_index[event_data.m_nrf++] = f;

    }
    event_data.m_r_map[i] = 0;
  }

  event_data.m_nr    = 0;
}

///////////////////////////////////////////////////////////////////
// Erase space point information
///////////////////////////////////////////////////////////////////

void InDet::TRT_SeededSpacePointFinder_ATL::EventData::erase()
{

  for(int i=0; i!=m_nrf;   ++i) {
    int n = m_rf_index[i]; m_rf_map[n] = 0;
    m_rf_Sorted[n].erase(m_rf_Sorted[n].begin(),m_rf_Sorted[n].end());
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
InDet::TRT_SeededSpacePointFinder_ATL::production2Spb(const EventContext& ctx,
                                                      const Trk::TrackParameters& tP,
                                                      int phi,
                                                      std::list<std::pair<const Trk::SpacePoint*,const Trk::SpacePoint*> > &outputListBuffer,
                                                      InDet::TRT_SeededSpacePointFinder_ATL::EventData &event_data) const
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

  // Get field cache object
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("TRT_SeededSpacePointFinder_ATL: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCondObjInputKey.key());
    return;
  }
  MagField::AtlasFieldCache    fieldCache;
  fieldCondObj->getInitializedCache (fieldCache);
  fieldCache.getField (gP, H);

  //need conversion kilotesla -> kilogauss - Previously used getMagneticFiledKiloGauss, whereas new function returns value in kiloTesla...
  H[0] *= 10000;
  H[1] *= 10000;
  H[2] *= 10000;

  std::list<std::pair<const Trk::SpacePoint*,int> >::iterator r0,r0e,r,re, rb;
  const Trk::SpacePoint* SpToPair = 0;

  int nri = 0;

  ///Set up the azinuthal width to look for SPs
  //
  int fmin=phi; int fmax=phi;
  if(m_search){fmin = phi-1; fmax = phi+1;}
  for(int f=fmin; f<=fmax; ++f) {
    int j=0; f<0 ? j=f+event_data.m_fNmax+1 : f>event_data.m_fNmax ? j=f-event_data.m_fNmax-1 : j=f;
    if(!event_data.m_rf_map[j]){
      continue;
    }
    r0 = event_data.m_rf_Sorted[j].begin();
    r0e = event_data.m_rf_Sorted[j].end();

    ///Fill a list with the SP in the azimouthal region indicated by the TRT track segment
    for(; r0!=r0e; ++r0){
      if((((*r0).first)->r() > m_r1max) ||
	 (((*r0).first)->r() < m_r2min)) {
	continue; //Fill only the SCT SPs
      }
      event_data.m_newRfi_Sorted.push_back(*r0);
      nri++;
    }
  }

  if(event_data.m_newRfi_Sorted.size()>5000 && event_data.m_newRfi_Sorted.size()<=10000) {
    event_data.m_newRfi_Sorted.erase(event_data.m_newRfi_Sorted.begin(),event_data.m_newRfi_Sorted.end());
    int fmin=phi; int fmax=phi;
    for(int f=fmin; f<=fmax; ++f) {
      int j=0; f<0 ? j=f+event_data.m_fNmax+1 : f>event_data.m_fNmax ? j=f-event_data.m_fNmax-1 : j=f;
      if(!event_data.m_rf_map[j]){
	continue;
      }
      r0 = event_data.m_rf_Sorted[j].begin();
      r0e = event_data.m_rf_Sorted[j].end();

      ///Fill a list with the SP in the azimouthal region indicated by the TRT track segment
      for(; r0!=r0e; ++r0){
	if((((*r0).first)->r()>m_r1max) || (((*r0).first)->r()<m_r2min)) {
	  continue; //Fill only the SCT SPs
	}
	event_data.m_newRfi_Sorted.push_back(*r0);
	nri++;
      }
    }
  }
  if(event_data.m_newRfi_Sorted.size()>10000) {
    event_data.m_newRfi_Sorted.erase(event_data.m_newRfi_Sorted.begin(),event_data.m_newRfi_Sorted.end());
    return;
  }

  event_data.m_newRfi_Sorted.sort(MyNewDataSortPredicate());

  spcount = event_data.m_newRfi_Sorted.size();

  r  = event_data.m_newRfi_Sorted.begin();
  re = event_data.m_newRfi_Sorted.end();

  std::vector<bypass_struct> tmp_prod_bypass;
  std::vector<const Trk::SpacePoint *> vrp;
  std::vector<double> rk;
  std::vector<long> geo_info;
  std::vector<double> zSP;
  tmp_prod_bypass.reserve(spcount);
  vrp.reserve(spcount);
  rk.reserve(spcount);
  geo_info.reserve(spcount);
  zSP.reserve(spcount);

  // // // // // // <Fill m_prod_bypass and the local array // // // //
  for (; r != re; r++) {
	  const Trk::SpacePoint *vrpi = (*r).first;

	  geo_info.push_back((*r).second);
	  vrp.push_back(vrpi);
	  rk.push_back(vrpi->r());

	  double X = vrpi->globalPosition().x() - x0;
	  double Y = vrpi->globalPosition().y() - y0;
	  double zSPi = vrpi->globalPosition().z();
	  zSP.push_back(zSPi);
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

  // // // // // // Fill m_prod_bypass and the local array> // // // //

  ///////////////////////////////////////////////////////

  if (m_doCosmics) { // no need to check this every time in the loop
	  for (long i = 0; i < (long)spcount; i++) {
		  SpToPair = 0;
		  const Trk::SpacePoint *up = vrp[i];
		  for (long j = i + 1; j < (long)spcount; j++) {
			  const Trk::SpacePoint *bp = vrp[j];
			  SpToPair = bp;
			  outputListBuffer.push_back(std::make_pair(up, SpToPair));
		  }
		  if(!SpToPair) {
			  outputListBuffer.push_back(std::make_pair(up, up));
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
			  if(fieldCache.solenoidOn()) {
				  if(!cutTPb(tmp_invar_bypass, tmp_prod_bypass,i, j, H[2])) {
					  continue;
				  }
			  }
			  SpToPair = bp;
			  outputListBuffer.push_back(std::make_pair(up, SpToPair));
		  }
		  if(!SpToPair) {
			  outputListBuffer.push_back(std::make_pair(up, up));
		  }
	  }
  }

  event_data.m_newRfi_Sorted.erase(event_data.m_newRfi_Sorted.begin(),event_data.m_newRfi_Sorted.end());
}

// comment out to enable angle discontinuity correction
// so that e.g. angles PI-epsilon and -Pi+epsilon are treated
// as being 2*epsilon apart as they should, instead of 2*Pi-2*epsilon
// as they would w/o this correction
//#define ANGLE_DISCO_COMPAT

bool
InDet::TRT_SeededSpacePointFinder_ATL::cutTPb(const invar_bypass_struct  &tmp_invar_bypass,
                                              const std::vector<bypass_struct> &tmp_prod_bypass,
                                              long bSP1, long bSP2, double H) const
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
InDet::TRT_SeededSpacePointFinder_ATL::geoInfo(const Trk::SpacePoint* SP,int& isB,int& ld) const
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
 if(m_fieldmode == "NoField") m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField);
 else if(m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
 else m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);
}
