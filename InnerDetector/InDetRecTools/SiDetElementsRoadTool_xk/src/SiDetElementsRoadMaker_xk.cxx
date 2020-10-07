/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::SiDetElementsRoadMaker_xk
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "SiDetElementsRoadTool_xk/SiDetElementsRoadMaker_xk.h"

#include "SiDetElementsRoadUtils_xk.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsComparison.h"
#include "SiDetElementsRoadUtils_xk.h"
#include "StoreGate/ReadCondHandle.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include <ostream>
#include <iomanip>

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiDetElementsRoadMaker_xk::SiDetElementsRoadMaker_xk
(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::initialize()
{
  //Class optimization checks
  static_assert(std::is_trivially_copyable<SiDetElementLink_xk::UsedFlag>::value);
  static_assert(std::is_trivially_destructible<SiDetElementLink_xk::UsedFlag>::value);
  static_assert(std::is_trivially_copyable<SiDetElementLink_xk::ElementWay>::value);
  static_assert(std::is_trivially_destructible<SiDetElementLink_xk::ElementWay>::value);
  static_assert(std::is_trivially_copyable<SiDetElementLink_xk>::value);
  static_assert(std::is_trivially_destructible<SiDetElementLink_xk>::value);
  static_assert(std::is_nothrow_move_constructible<SiDetElementsLayer_xk>::value);


  if (!m_usePIX && !m_useSCT) {
    ATH_MSG_FATAL("Please don't call this tool if usePixel and useSCT are false");
    return StatusCode::FAILURE;
  }
 
  if (m_fieldmode == "NoField") m_fieldModeEnum = Trk::NoField;
  else if (m_fieldmode == "MapSolenoid") m_fieldModeEnum = Trk::FastField;
  else m_fieldModeEnum = Trk::FullField;

  // Get propagator tool
  //
  ATH_CHECK(m_proptool.retrieve());

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;

  computeBounds();

  ATH_CHECK(m_layerVecKey.initialize());
  ATH_CHECK(m_fieldCondObjInputKey.initialize());

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::SiDetElementsRoadMaker_xk::finalize()
{
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiDetElementsRoadMaker_xk::dump(MsgStream& out) const
{
  out<<"\n";
  return dumpConditions(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::SiDetElementsRoadMaker_xk::dumpConditions(MsgStream& out) const
{
  int n = 62-m_proptool.type().size();
  std::string s1;
  for (int i=0; i<n; ++i) s1.append(" ");
  s1.append("|");

  std::string fieldmode[9] = {"NoField"       , "ConstantField", "SolenoidalField",
                              "ToroidalField" , "Grid3DField"  , "RealisticField" ,
                              "UndefinedField", "AthenaField"  , "?????"         };

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);

  SG::ReadCondHandle<AtlasFieldCacheCondObj> fieldHandle(m_fieldCondObjInputKey);
  const AtlasFieldCacheCondObj* fieldCondObj(*fieldHandle);
  if (fieldCondObj) {
    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache(fieldCache);
    if (!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  }

  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

  int mode = fieldprop.magneticFieldMode();
  if (mode<0 || mode>8) mode = 8; 

  n = 62-fieldmode[mode].size();
  std::string s3;
  for (int i=0; i<n; ++i) s3.append(" ");
  s3.append("|");

  n = 62-m_sct.size();
  std::string s5;
  for (int i=0; i<n; ++i) s5.append(" ");
  s5.append("|");

  n = 62-m_pix.size();
  std::string s6;
  for (int i=0; i<n; ++i) s6.append(" ");
  s6.append("|");

  const SiDetElementsLayerVectors_xk &layer = *getLayers();

  int maps = 0;
  if (layer[0].size()) ++maps;
  if (layer[1].size()) ++maps;
  if (layer[2].size()) ++maps;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<"\n";
  if (m_useSCT) {
    out<<"| SCT   detector manager  | "<<m_sct             <<s5<<"\n";
  }
  if (m_usePIX) {
    out<<"| Pixel detector manager  | "<<m_pix             <<s6<<"\n";
  }
  out<<"| Tool for propagation    | "<<m_proptool.type() <<s1<<"\n";
  out<<"| Magnetic field mode     | "<<fieldmode[mode]   <<s3<<"\n";
  out<<"| Width of the road (mm)  | "
     <<std::setw(12)<<std::setprecision(5)<<m_width
     <<"                                                  |"<<"\n";
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|"
     <<"\n";

  if (!maps || m_outputlevel==0) return out;

  if (layer[1].size()) {
    int nl = layer[1].size();
    int nc = 0;
    for (unsigned int i=0; i!=layer[1].size(); ++i) nc+=layer[1].at(i).nElements();
    out<<"|----------------------------------------------------------------|"
       <<"\n";
    out<<"| Barrel map containt "
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements               |"
       <<"\n";
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    out<<"|   n  |     R     |   Z min    |   Z max    |  max dF    | nEl  |"
       <<"\n";
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    for (unsigned int i=0; i!=layer[1].size(); ++i) {
      double zmin = layer[1].at(i).z()-layer[1].at(i).dz();
      double zmax = layer[1].at(i).z()+layer[1].at(i).dz();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[1].at(i).r ()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<                zmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<< layer[1].at(i).dfe()<<" | "
	 <<std::setw(4)<<layer[1].at(i).nElements()<<" | "
	 <<"\n";
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";

  }
  if (layer[0].size()) {
    int nl = layer[0].size();
    int nc = 0;
    for (unsigned int i=0; i!=layer[0].size(); ++i) nc+=layer[0].at(i).nElements();
    out<<"|----------------------------------------------------------------|"
       <<"\n";
    out<<"| L.Endcap map containt"
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements              |"
       <<"\n";

    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<"\n";
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    for (unsigned int i=0; i!=layer[0].size(); ++i) {
      double rmin = layer[0].at(i).r()-layer[0].at(i).dr();
      double rmax = layer[0].at(i).r()+layer[0].at(i).dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[0].at(i).z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<layer[0].at(i).dfe()<<" | "
	 <<std::setw(4)<<layer[0].at(i).nElements()<<" | "
	 <<"\n";
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
  }
  if (layer[2].size()) {
    int nl = layer[2].size();
    int nc = 0;
    for (unsigned int i=0; i!=layer[2].size(); ++i) nc+=layer[2].at(i).nElements();
    out<<"|----------------------------------------------------------------|"
       <<"\n";
    out<<"| R.Endcap map containt"
       <<std::setw(3)<<nl<<" layers and"
       <<std::setw(5)<<nc<<" elements              |"
       <<"\n";
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    out<<"|   n  |     Z     |   R min    |   R max    |  max dF    | nEl  |"
       <<"\n";
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
    for (unsigned int i=0; i!=layer[2].size(); ++i) {
      double rmin = layer[2].at(i).r()-layer[0].at(i).dr();
      double rmax = layer[2].at(i).r()+layer[0].at(i).dr();
      out<<"| "
	 <<std::setw(4)<<i<<" |"
	 <<std::setw(10)<<std::setprecision(4)<<  layer[2].at(i).z()<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmin<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<               rmax<<" | "
	 <<std::setw(10)<<std::setprecision(4)<<layer[2].at(i).dfe()<<" | "
	 <<std::setw(4)<<layer[2].at(i).nElements()<<" | "
	 <<"\n";
    }
    out<<"|------|-----------|------------|------------|------------|------|"
       <<"\n";
  }
  out<<"\n";
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::SiDetElementsRoadMaker_xk::dump(std::ostream& out) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator <<
(MsgStream& sl, const InDet::SiDetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator <<
(std::ostream& sl, const InDet::SiDetElementsRoadMaker_xk& se)
{ 
  return se.dump(sl); 
}   


///////////////////////////////////////////////////////////////////
// Main methods for road builder using input list global positions
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::detElementsRoad
(std::list<Amg::Vector3D>& GP,
 std::list<const InDetDD::SiDetectorElement*>& Road,
 bool testDirection) const
{  
  if (!m_usePIX && !m_useSCT) return;

  /// this is a vector of vectors of detector elements
  /// prepared by the SiDetElementsRoadCondAlg. 
  /// The outer vector has 3 elements: 
  /// 0 --> left endcap 
  /// 1 --> barrel 
  /// 2 --> right endcap
  /// Each of the inner vectors is internally sorted.
  /// For the endcaps, sorting is first in z, then for same Z in r, then 
  /// for same r in phi.
  /// For the barrel, we first sort in R, then for same R in phi, then for same 
  /// phi in Z. 
  const SiDetElementsLayerVectors_xk &layer = *getLayers();

  /// iterators over the positions to consider
  std::list<Amg::Vector3D>::iterator currentPosition=GP.begin(), endPositions=GP.end();

  /// fill an array with the reference point (start with the first one), the road width and a placeholder
  /// for the step length
  std::array<float,6> par_startingPoint{static_cast<float>((*currentPosition).x()),            // x of first position
                static_cast<float>((*currentPosition).y()),             // y of first position
                static_cast<float>((*currentPosition).z()),             // Z of first position
                static_cast<float>(sqrt((*currentPosition).x()*(*currentPosition).x()+(*currentPosition).y()*(*currentPosition).y())),    // r of first position 
                m_width,    // road width 
                0.};    
  
  /// check the left endcap layers
  /// increment n0 until we are at the layer closest to 
  /// the first position from the left side
  int n0 = 0;
  for (; n0!=static_cast<int>(layer[0].size()); ++n0) {
    if (par_startingPoint[2] > layer[0].at(n0).z()) break;
  }

  /// check the barrel layers
  /// increment n1 until we are at the layer closest to 
  /// the first position in the radial direction
  int n1 = 0;
  for (; n1!=static_cast<int>(layer[1].size()); ++n1) {
    if (par_startingPoint[3] < layer[1].at(n1).r()) break;
  }
  /// and finally, the left endcap. 
  /// this time, look for the layer closest on the right side. 
  int n2 = 0;
  for (; n2!=static_cast<int>(layer[2].size()); ++n2) {
    if (par_startingPoint[2] < layer[2].at(n2).z()) break;
  }

  
  std::vector<InDet::SiDetElementLink_xk::ElementWay> lDE;
  /// book an impressively nested structure to keep track of which detector elements we use 
  ElementUsageTracker & used = *getElementUsageTracker(); 

  /// done with the first probed position. Now we can start to move along the trajectory
  ++currentPosition;
  while (currentPosition!=endPositions) {
    /// store the point we are aiming towards
    std::array<float,4>  par_targetPoint{static_cast<float>((*currentPosition).x()),         /// x of the current position
                    static_cast<float>((*currentPosition).y()),         /// y of the current position
                    static_cast<float>((*currentPosition).z()),         /// z of the current position
		                static_cast<float>(sqrt((*currentPosition).x()*(*currentPosition).x()+(*currentPosition).y()*(*currentPosition).y()))   /// r of the current position
                    };

    /// perform linearisation
    float dx = par_targetPoint[0]-par_startingPoint[0];         /// dx between the current and the first position
    float dy = par_targetPoint[1]-par_startingPoint[1];         /// dy between the current and the first position 
    float dz = par_targetPoint[2]-par_startingPoint[2];         /// dz between the current and the first position 
    float dist3D = sqrt(dx*dx+dy*dy+dz*dz);   /// 3D distance between the current and the first position
    if (dist3D <=0.) {                    /// if geometry breaks down or two points are duplicates, 
      ++currentPosition;                  /// we whistle innocently and make a point of looking somewhere else 
      continue;
    }
    float inverseDistance = 1./dist3D;                   /// inverse distance to the first position 
    /// now we can book the linearised search direction 
    std::array<float,3> searchDirection{dx*inverseDistance, dy*inverseDistance, dz*inverseDistance};

    /// Having found the search direction, we are ready to look for detector elements and iterate.
    /// Before doing so, we add an additional test to ensure we probe the perigee point
    /// if we cross it on the way. 
    float unitSepTransverseComp = searchDirection[0]*searchDirection[0]+searchDirection[1]*searchDirection[1];     /// transverse component of the separation vector 
    float dr = 0.                 ;     
    if (unitSepTransverseComp!=0.) {                               
      /// *negative* component of the global location of the previous position into the direction connecting our positions in the x-y plane 
      /// corresponds to the path length opposite to the linearised direction to reach the perigee 
      float sm = -( searchDirection[0]*par_startingPoint[0] +
                    searchDirection[1]*par_startingPoint[1])
                    /unitSepTransverseComp; 

      /// a positive value of SM means the closest approach to the beamline is between the two positions we are considering. 
      /// In this case, we do not want to iterate to the next point, but instead insert an additional step where we use the 
      /// perigee point as our reference. We do this by setting the target point to the perigee, which will be made the 
      /// reference point when repeating the loop. 
      /// Since the perigee is estimated using the linearised direction, this direction stays valid and does not need to be updated.
      if (sm > 1. && sm < dist3D) {   /// only add the perigee point if the next point is beyond the perigee, and if we are not too close anyway
        par_targetPoint[0] = par_startingPoint[0]+searchDirection[0]*sm;
        par_targetPoint[1] = par_startingPoint[1]+searchDirection[1]*sm;
        par_targetPoint[2] = par_startingPoint[2]+searchDirection[2]*sm;
        par_targetPoint[3] = sqrt(par_targetPoint[0]*par_targetPoint[0]+par_targetPoint[1]*par_targetPoint[1]);
        /// now, the target point is the perigee estimate, while 
        /// the reference point for this round stays unchanged.
	      dr    = 20.;  /// allow 2cm on top of the perigee location when extrapolating inward. 
      } else {
        ++currentPosition;
      }
    } else {
      ++currentPosition;
    }

    /// Start collecting detector elements traversed by our linearised path. 

    /// First, barrel elements

    /// if we are moving outwards in r: 
    if (par_targetPoint[3]>par_startingPoint[3]) {
      /// loop over all barrel layers (starting with the closest one previously identified)
      for (; n1<static_cast<int>(layer[1].size()); ++n1) {
        /// stop if we moved past the targer point in R
	      if (par_targetPoint[3] < layer[1].at(n1).r()) break;
        assert( used.at(1).size() > static_cast<unsigned int>(n1) );
        /// collect all compatible detector elements from the current layer
	      layer[1].at(n1).getBarrelDetElements(par_startingPoint, searchDirection, lDE, used[1][n1]);
      }
      /// if we are moving inward in R, iterate the other way for the barrel
    } else {
      for (--n1; n1>=0; --n1) {
        /// stop if we moved past the test point in R
	      if (par_targetPoint[3] > layer[1].at(n1).r()+dr) break;
        assert( used.at(1).size() > static_cast<unsigned int>(n1) );
        /// collect all compatible detector elements        
	      layer[1].at(n1).getBarrelDetElements(par_startingPoint, searchDirection, lDE, used[1][n1]);
      }
      ++n1;
    }

    /// Positive endcap
    /// again check if we are moving forward or back in z
    if (par_targetPoint[2]>par_startingPoint[2]) {
      for (; n2<static_cast<int>(layer[2].size()); ++n2) {
	      if (par_targetPoint[2] < layer[2].at(n2).z()) break;
        assert( used.at(2).size() > static_cast<unsigned int>(n2) );
        /// collect all compatible detector elements        
	      layer[2].at(n2).getEndcapDetElements(par_startingPoint, searchDirection, lDE,used[2][n2]);
      }
    } else {
      for (--n2; n2>=0; --n2) {
	      if (par_targetPoint[2] > layer[2].at(n2).z()) break;
        assert( used.at(2).size() > static_cast<unsigned int>(n2) );
        /// collect all compatible detector elements        
	      layer[2].at(n2).getEndcapDetElements(par_startingPoint, searchDirection, lDE, used[2][n2]);
      }
      ++n2;
    }

    /// Negative endcap
    /// same game as above
    if (par_targetPoint[2]<par_startingPoint[2]) {
      for (; n0<static_cast<int>(layer[0].size()); ++n0) {
	      if (par_targetPoint[2] > layer[0].at(n0).z()) break;
        assert( used.at(0).size() > static_cast<unsigned int>(n0) );
        /// collect all compatible detector elements        
	      layer[0].at(n0).getEndcapDetElements(par_startingPoint, searchDirection, lDE,used[0][n0]);
      }
    } else {
      for (--n0; n0>=0; --n0) {
	      if (par_targetPoint[2] < layer[0].at(n0).z()) break;
        assert( used.at(0).size() > static_cast<unsigned int>(n0) );
        /// collect all compatible detector elements        
	      layer[0].at(n0).getEndcapDetElements(par_startingPoint, searchDirection, lDE,used[0][n0]);
      }
      ++n0;
    }
    /// update the starting point to be the current target point
    par_startingPoint[0] = par_targetPoint[0];
    par_startingPoint[1] = par_targetPoint[1];
    par_startingPoint[2] = par_targetPoint[2];
    par_startingPoint[3] = par_targetPoint[3];
    /// and increment the total propagation distance
    par_startingPoint[5]+= dist3D;
  }
  auto vec2 = lDE; 
  std::sort(lDE.begin(),lDE.end(),InDet::compDetElementWays());
  // Fill list pointers to detector elements
  for (auto & d : lDE){
    if (testDirection && d.way() < 0) continue; 
    Road.push_back(d.link()->detElement());
  }
}

/// obtain an event usage tracker object 
InDet::SiDetElementsRoadMaker_xk::ElementUsageTracker* InDet::SiDetElementsRoadMaker_xk::getElementUsageTracker() const{

    const SiDetElementsLayerVectors_xk &layer = *getLayers();

    static Gaudi::Hive::ContextSpecificPtr<ElementUsageTracker> p_aux_usageTracker ATLAS_THREAD_SAFE;
    ElementUsageTracker* theTracker{nullptr}; 
    if (!p_aux_usageTracker){
       p_aux_usageTracker = new ElementUsageTracker();
       theTracker = p_aux_usageTracker.get();
       /// book sufficient space
       /// module_i: iterate over the detector side 
       for ( unsigned int side_i=0; side_i<3; ++side_i) {
          (*theTracker)[side_i].resize( layer[side_i].size() ); /// for each side, book the number of layers we expect to see
          for (unsigned int layer_i=0; layer_i < layer[side_i].size(); ++layer_i) {
              /// for each layer, book one slot for each detector element on the layer
              (*theTracker)[side_i][layer_i].resize( layer[side_i][layer_i].nElements() );
          }
       }
    }
    else{
       theTracker = p_aux_usageTracker.get();
      /// reset this tracker 
       for ( unsigned int side_i=0; side_i<3; ++side_i) {
          for (unsigned int layer_i=0; layer_i < layer[side_i].size(); ++layer_i) {
              /// for each layer, book one slot for each detector element on the layer
              std::fill((*theTracker)[side_i][layer_i].begin(), (*theTracker)[side_i][layer_i].end(), InDet::SiDetElementLink_xk::UsedFlag{});
          }
       }
    }
    return p_aux_usageTracker.get();
}

///////////////////////////////////////////////////////////////////
// Main methods for road builder using track parameters and direction
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::detElementsRoad
(const EventContext& ctx,
 MagField::AtlasFieldCache& fieldCache,
 const Trk::TrackParameters& Tp,
 Trk::PropDirection D,
 std::list<const InDetDD::SiDetectorElement*>& R) const
{
  if (!m_usePIX && !m_useSCT) return;
  /// 500 MeV / pT 
  double qp   = fabs(500.*Tp.parameters()[4]);
  /// truncate at huge pt 
  if (qp < 1.e-10) qp = 1.e-10; 
  /// step size - scaled by pt / 500 MeV 
  double S    = m_step/qp;      
  /// upper limit to step size: 1000               ;
  if (S  > 1000. ) S  = 1000. ;

  bool testDirection = true;
  if (D<0) {
    testDirection = false;
    S=-S;
  }

  Trk::MagneticFieldMode fieldModeEnum(m_fieldModeEnum);
  if (!fieldCache.solenoidOn()) fieldModeEnum = Trk::NoField;
  Trk::MagneticFieldProperties fieldprop(fieldModeEnum);

  // Note: could also give fieldCache directly to propagator if it would be more efficient - would
  // need to add interface RDS 2020/03
  std::list<Amg::Vector3D> G;

  /// get a list of global positions for the road search by starting from the first surface 
  /// and walking along the trajectory using the RK propagator 
  m_proptool->globalPositions(ctx, G, Tp, fieldprop,getBound(fieldCache, Tp), S, Trk::pion);
  /// should find at least 2 positions to sample 
  if (G.size()<2) return;

  /// if we are extrapolating along them momentum direction, 
  /// we pick out the part ascending in R 
  if (D > 0) {
    std::list<Amg::Vector3D>::iterator currentPosition=G.begin(), nextPosition, endPositions=G.end();
    float r0 = (*currentPosition).x()*(*currentPosition).x()+(*currentPosition).y()*(*currentPosition).y();

    while (currentPosition!=endPositions) {
      nextPosition = currentPosition;
      if (++nextPosition == endPositions) break;
      
      float r = (*nextPosition).x()*(*nextPosition).x()+(*nextPosition).y()*(*nextPosition).y();
      /// if the next point is at lower r than the previous point, remove the previous one  
      if (r < r0) {
        r0 = r;
        G.erase(currentPosition++);
      } else {
        break;
      }
    }
  }
  /// now perform the road building using our set of positions
  detElementsRoad(G, R,testDirection);
}


///////////////////////////////////////////////////////////////////
// Map of detector elements production
///////////////////////////////////////////////////////////////////

void InDet::SiDetElementsRoadMaker_xk::computeBounds()
{

  StatusCode sc;

  // Get  Pixel Detector Manager
  //
  const InDetDD::PixelDetectorManager* pixmgr = nullptr;
  if (m_usePIX) {
    sc = detStore()->retrieve(pixmgr, m_pix);
    if (sc.isFailure() || !pixmgr) {
      ATH_MSG_INFO("Could not get PixelDetectorManager  !");
      return;
    }
  }
  
  // Get  SCT Detector Manager
  //
  const InDetDD::SCT_DetectorManager* sctmgr = nullptr;
  if (m_useSCT) {
    sc = detStore()->retrieve(sctmgr, m_sct);
    if (sc.isFailure() || !sctmgr) {
      ATH_MSG_INFO("Could not get SCT_DetectorManager !");
      return;
    }
  }

  const PixelID* IDp = nullptr;
  const SCT_ID*  IDs = nullptr;

  if (m_usePIX && detStore()->retrieve(IDp, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
  }
  
  if (m_useSCT && detStore()->retrieve(IDs, "SCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get SCT ID helper");
  }


  if (!IDs && !IDp) return;

  InDetDD::SiDetectorElementCollection::const_iterator s, se;
  std::vector<InDetDD::SiDetectorElement*> pW[3];            

  if (IDp) {
    // Loop over each wafer of pixels
    //
    s  =  pixmgr->getDetectorElementBegin();
    se =  pixmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {
      if      ((*s)->isBarrel()       ) pW[1].push_back((*s)); // Barrel
      else if ((*s)->center().z() > 0.) pW[2].push_back((*s)); // Right endcap
      else                              pW[0].push_back((*s)); // Left  endcap
    }
  }

  if (IDs) {
    // Loop over each wafer of sct
    //
    s  = sctmgr->getDetectorElementBegin();
    se = sctmgr->getDetectorElementEnd  ();

    for (; s!=se; ++s) {
      if      ((*s)->isBarrel()       ) pW[1].push_back((*s)); // Barrel
      else if ((*s)->center().z() > 0.) pW[2].push_back((*s)); // Right endcap
      else                              pW[0].push_back((*s)); // Left  endcap
    }
  }

  int nel = pW[0].size()+pW[1].size()+pW[2].size();
  if (!nel) return;

  std::sort(pW[1].begin(), pW[1].end(), InDet::compDetElements_RAZ());
  std::sort(pW[0].begin(), pW[0].end(), InDet::compDetElements_ZRA());
  std::sort(pW[2].begin(), pW[2].end(), InDet::compDetElements_ZRA());

  double   mzmin [3];  // min Z coordinate 
  double   mzmax [3];  // max Z coordinate
  double   mrmin [3];  // min radius
  double   mrmax [3];  // max radius
  bool     has[3] {false,false,false};
 
  for (int N=0; N!=3; ++N) {
    double P[40];
    int im    = static_cast<int>(pW[N].size()-1); 
    int If    = 0      ;
    double z0 = 0.     ;
    double r0 = 0.     ;
    mrmin[N] = 100000.;
    mrmax[N] =-100000.;
    mzmin[N] = 100000.;
    mzmax[N] =-100000.;
    
    for (int i = 0; i<= im; ++i) {
      InDet::SiDetElementsRoadUtils_xk::detElementInformation(*(pW[N][i]), P); 

      if (P[ 9] < mrmin[N]) mrmin[N] = P[ 9]; 
      if (P[10] > mrmax[N]) mrmax[N] = P[10]; 
      if (P[11] < mzmin[N]) mzmin[N] = P[11]; 
      if (P[12] > mzmax[N]) mzmax[N] = P[12]; 
 
      double r    = P[0];
      double z    = P[1];
      bool   newl = false;
      if (N==1) {
	if (fabs(r-r0) > 10.) {
          newl=true;
          r0=r;
        }
      } else {
	if (fabs(z-z0) > 10.) {
          newl=true;
          r0=r;
          z0=z;
        }
      }

      if (newl || i==im) {
	int Il = i-1;
        if (i==im) ++Il;

	if (If<=Il) {
          has[N]=true;
	}
	If = i;
      }
    }
  }

  // CylinderBounds production
  //
  double zmi = +100000;
  double zma = -100000;
  double rma = -100000;
  for (int i=0; i!=3; ++i) {
    if (has[i]) {
      if (mzmin[i]<zmi) zmi=mzmin[i]; 
      if (mzmax[i]>zma) zma=mzmax[i]; 
      if (mrmax[i]>rma) rma=mrmax[i];
    }
  }

  double hz = fabs(zma);
  if (hz<fabs(zmi)) hz = fabs(zmi);
  const Trk::CylinderBounds CB(rma+20., hz+20.);
  m_bounds  = CB;
}


///////////////////////////////////////////////////////////////////
// Distance to detector element according stright line model
///////////////////////////////////////////////////////////////////

float InDet::SiDetElementsRoadMaker_xk::stepToDetElement
(const InDetDD::SiDetectorElement*& de, Amg::Vector3D& r, Amg::Vector3D& a) const
{
  Amg::Vector3D R = de->center();
  Amg::Vector3D A = de->normal();
  double D = a.x()*A.x()+a.y()*A.y()+a.z()*A.z();
  if (D==0.) return static_cast<float>(D);
  return static_cast<float>((A.x()*(R.x()-r.x())+A.y()*(R.y()-r.y())+A.z()*(R.z()-r.z()))/D);
}

///////////////////////////////////////////////////////////////////
// Cylinder bounds parameters estimation
///////////////////////////////////////////////////////////////////

Trk::CylinderBounds InDet::SiDetElementsRoadMaker_xk::getBound
(MagField::AtlasFieldCache& fieldCache,
 const Trk::TrackParameters& Tp) const
{
  const double cor = 1.;

  double zfield = 0.;
  if (m_fieldModeEnum!=Trk::NoField && fieldCache.solenoidOn()) {
    const Amg::Vector3D& pos = Tp.position();
    double f[3], p[3] = {pos[Amg::x], pos[Amg::y], pos[Amg::z]};

    fieldCache.getFieldZR(p, f);

    zfield = 299.7925*f[2];
  }

  if (fabs(zfield) < .0000001) return m_bounds;

  const AmgVector(5)& Vp = Tp.parameters();
  
  double cur = zfield*Vp[4]/sin(Vp[3]);

  if (fabs(cur)*m_bounds.r() < cor) return m_bounds;

  double rad = 1./cur;
  if (cor*fabs(rad) > m_bounds.r()  ) return m_bounds;

  const  Amg::Vector3D& Gp = Tp.position();

  double S, C;
  sincos(Vp[2], &S, &C);

  double xc = Gp.x()+S*rad;
  double yc = Gp.y()-C*rad;
  double rm = (sqrt(xc*xc+yc*yc)+fabs(rad))*cor;
  if (rm > m_bounds.r()) return m_bounds;
  Trk::CylinderBounds CB(rm, m_bounds.halflengthZ());
  return CB;
}
