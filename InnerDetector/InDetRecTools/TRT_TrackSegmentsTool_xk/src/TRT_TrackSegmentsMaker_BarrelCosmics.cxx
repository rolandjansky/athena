/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
//    TRT_TrackSegmentsMaker_BarrelCosmics.h, (c) ATLAS Detector software 
///////////////////////////////////////////////////////////////////////////////////////////

#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsMaker_BarrelCosmics.h"

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "StoreGate/ReadHandle.h"

//////////////////////////////////////////////////////////////////////////////////////////
//   standard methods: constructor, initialize, finalize
//////////////////////////////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsMaker_BarrelCosmics::TRT_TrackSegmentsMaker_BarrelCosmics
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p), 
    m_debugLevel(MSG::INFO),
    m_maxTotalHits(21000), // corresponds to 20% occupancy
    m_minHitsForSeed(-1),
    m_minHitsForSegment(20),
    m_minHitsAboveTOT(-1),
    m_nBinsInX(100),
    m_nBinsInPhi(10),
    m_minSeedTOT(10.),
    m_magneticField(true),
    m_mergeSegments(false),
    m_useAthenaFieldService(true), 
    m_TRTManagerName("TRT"),
//    m_trtmanager(0),
    m_trtid(0),
    m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels"),
    m_useAssoTool(false),
    //Endcap Trigger Hack
    m_segmentDriftCirclesCount(0)
{

  declareInterface<ITRT_TrackSegmentsMaker>(this);

  declareProperty("MaxTotalNumberOfBarrelHits", m_maxTotalHits); // if set to 0, this requirement is not used. total number of TRT barrel straws is 105088

  declareProperty("MinimalNumberOfTRTHits", m_minHitsForSegment);
  declareProperty("MinNumberOfHitsForSeed", m_minHitsForSeed);
  declareProperty("MinNumberOfHitsAboveTOT", m_minHitsAboveTOT);

  declareProperty("NbinsInX", m_nBinsInX);
  declareProperty("NbinsInPhi", m_nBinsInPhi);

  declareProperty("MinimalTOTForSeedSearch", m_minSeedTOT);
  declareProperty("IsMagneticFieldOn", m_magneticField);
  declareProperty("TrtManagerLocation", m_TRTManagerName);
  declareProperty("MergeSegments", m_mergeSegments);
//  declareProperty("SearchRoadWidth", m_searchRoadWidth);

  declareProperty("AssosiationTool"      ,m_assoTool   );
  declareProperty("UseAssosiationTool",m_useAssoTool);

  declareProperty( "UseAthenaFieldService",     m_useAthenaFieldService);


}

StatusCode InDet::TRT_TrackSegmentsMaker_BarrelCosmics::initialize() {

  m_debugLevel = msg().level();

  msg(MSG::INFO) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::initialize(), March 2012"
                 << ", magnetic field: " << (m_magneticField?"ON":"OFF") 
                 << " search bins: " << m_nBinsInX << ", " << m_nBinsInPhi << endmsg;

  StatusCode sc = StatusCode::SUCCESS;
  
  // Initialize ReadHandle
  ATH_CHECK(m_trtname.initialize());
  ATH_CHECK(m_driftCirclesName.initialize());
  // TRT
  if (detStore()->retrieve(m_trtid, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_minHitsForSeed<=0) m_minHitsForSeed = (int) ( 0.601 * m_minHitsForSegment );
  if (m_minHitsAboveTOT<=0) m_minHitsAboveTOT = (int) (0.751 * m_minHitsForSegment );

  msg(MSG::INFO) << "m_minHitsForSegment = " << m_minHitsForSegment << endmsg;
  msg(MSG::INFO) << "m_minHitsForSeed    = " << m_minHitsForSeed << endmsg;
  msg(MSG::INFO) << "m_minHitsAboveTOT   = " << m_minHitsAboveTOT << endmsg;

  if (m_minSeedTOT<0. || m_minSeedTOT>20.)
    msg(MSG::WARNING) << "initialize(): are you sure about the MinimalTOTForSeedSearch setting? (set at " << m_minSeedTOT << ")" << endmsg;

  msg(MSG::INFO) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::initialize(), jobProperties: "
                 << "MinimalNumberOfTRTHits " << m_minHitsForSegment << ", MinimalTOTForSeedSearch: " << m_minSeedTOT
                 << ", m_minHitsForSeed: " << m_minHitsForSeed << ", m_minHitsAboveTOT: " << m_minHitsAboveTOT << endmsg;

  return sc;
}

StatusCode InDet::TRT_TrackSegmentsMaker_BarrelCosmics::finalize() {

   msg(MSG::INFO) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::finalize()" << endmsg;

   return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////
//   other methods inherited from ITRT_TrackSegmentsMaker: newEvent, newRegion, endEvent
//////////////////////////////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::newEvent() {

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::newEvent()" << endmsg;

  clear(); // private method that clears data members from the previous event


  SG::ReadHandle<InDet::TRT_DriftCircleContainer> TRTDriftCircleContainer(m_driftCirclesName); // get TRT_DriftCircle list from StoreGate containers

  if (not TRTDriftCircleContainer.isValid()) { msg(MSG::ERROR) << "Could not find TRT_DriftCircles collection!" << endmsg; return; }
  if (TRTDriftCircleContainer.cptr()==0) { msg(MSG::ERROR) << "newEvent(): TRTDriftCircleContainer==0" << endmsg; return; }

  for(InDet::TRT_DriftCircleContainer::const_iterator it=TRTDriftCircleContainer->begin(); it!=TRTDriftCircleContainer->end(); it++) {

    const InDet::TRT_DriftCircleCollection *colNext=&(**it);
    if (!colNext) { msg(MSG::WARNING) << "newEvent(): !colNext " << endmsg; continue; }
    
    for (DataVector<InDet::TRT_DriftCircle>::const_iterator circleit=(*colNext).begin();circleit!=(*colNext).end();circleit++){
	
      if ( m_trtid->is_barrel((**circleit).identify()) ) { // TRT barrel
	    
	m_listHits.push_back( *circleit );

        double onMyTime = (*circleit)->timeOverThreshold();
        if ( (*circleit)->firstBinHigh()  && ((*circleit)->trailingEdge() != 24) && ((*circleit)->trailingEdge() != 0) ) onMyTime = ( (double) (*circleit)->trailingEdge() + 1 ) * 3.125;
        if (onMyTime < m_minSeedTOT) continue;
		
        const Amg::Vector3D &center = (*circleit)->detectorElement()->surface(( *circleit )->identify()).center();
        m_listHitCenter.push_back( center );  
      }
    }
  } // end TRTDriftCircleContainer loop

  if ( m_maxTotalHits && ((int)m_listHits.size()) > m_maxTotalHits ) { 
    if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "skipping high occupancy event of " << m_listHits.size() << " barrel hits, limit at " << m_maxTotalHits << endmsg;
    clear(); 
  }

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "newEvent(): Number of TRT barrel hits: " << m_listHits.size() 
                                        << " Number of hits with TOT > " << m_minSeedTOT << ": " << m_listHitCenter.size() << endmsg;
					
  return;
}

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::newRegion(const std::vector<IdentifierHash> &vTRT) {

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::newRegion()" << endmsg;

  clear();
  SG::ReadHandle<InDet::TRT_DriftCircleContainer> m_trtcontainer(m_trtname);
  if (not m_trtcontainer.isValid()) {
    msg(MSG::ERROR) << "m_trtcontainer is empty!!!" << endmsg;
    return;
  }   

  for( std::vector<IdentifierHash>::const_iterator d=vTRT.begin(); d!=vTRT.end(); d++ ) {
    for ( InDet::TRT_DriftCircleContainer::const_iterator w = m_trtcontainer->indexFind((*d)); w!=m_trtcontainer->end(); w++ ) {
	  for( InDet::TRT_DriftCircleCollection::const_iterator circleit=(*w)->begin(); circleit!=(*w)->end(); circleit++ ) {

        if(abs(m_trtid->barrel_ec( (*circleit)->identify() ))!=1) continue;

        m_listHits.push_back((*circleit));

        double onMyTime = (*circleit)->timeOverThreshold();
        if ( (*circleit)->firstBinHigh()  && ((*circleit)->trailingEdge() != 24) && ((*circleit)->trailingEdge() != 0) ) onMyTime = ( (double) (*circleit)->trailingEdge() + 1 ) * 3.125;
        if (onMyTime < m_minSeedTOT) continue;
/* 
        if ( (*circleit)->timeOverThreshold() < m_minSeedTOT ) {
          if ( (*circleit)->driftTimeBin() == 24 && (*circleit)->trailingEdge() > 2 ) { ; }
          else { continue; } // save hit center only for TOT>... hits
        } */
        const Amg::Vector3D &center = (*circleit)->detectorElement()->surface(( *circleit )->identify()).center();
        m_listHitCenter.push_back( center );  		
      }	
	}
  }

  if ( m_maxTotalHits && ((int)m_listHits.size()) > m_maxTotalHits ) { 
    if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "skipping high occupancy event of " << m_listHits.size() << " barrel hits, limit at " << m_maxTotalHits << endmsg;
    clear();
  }
  
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "newRegion(): Number of TRT barrel hits: " << m_listHits.size() 
                                      << " Number of hits with TOT > " << m_minSeedTOT << ": " << m_listHitCenter.size() << endmsg;

  return;
}

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::endEvent () {

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::endEvent()" << endmsg;

  if ( m_segmentDriftCirclesCount < m_segments.size() ) { // elements of m_segments created by new have not been passed on
    msg(MSG::WARNING) << "endEvent() you called the function t create the segments but not retrived them later??" << endmsg; 
    msg(MSG::WARNING) << "endEvent() deleting remaining elements of m_segments" << endmsg;
    for (unsigned int i=m_segmentDriftCirclesCount; i<m_segments.size(); i++) delete m_segments[i];
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//   segment finding algorithm function: find, calls findSeed 
//////////////////////////////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::find(){

  if (!m_magneticField) { findOld(); return; }

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::find()" << endmsg;

  static int eventCount(-1); eventCount++;
//  FILE *fout = fopen("findSeedInfo.txt", "a");


  if ((int)m_listHitCenter.size()<m_minHitsAboveTOT) return;

  if (m_segmentDriftCircles.size()) { // debug only
    msg(MSG::WARNING) << "TRT_TrackSegmentsMaker_BarrelCosmics::find() probably called twice per event? or newEvent / newRegion have not been called. check your program" << endmsg;
    clear(); return; 
  } 

  std::vector<double> x0, phi, pivotX, pivotY, Xparabola, cotanParabola, InverseR;
  std::vector<int> nHitsPosY, nHitsNegY;

 // int foutCount(0);

  // first find seeds
  int nIterations(15), countCalls(0);
  while (countCalls<150) {

    double xRange(1000.), phiRange(M_PI_4); 
    double par[] = {0., M_PI_2, 0., 0., 0., 0., 0., 0.}; 
    int foundSeed(0);

    for (int i=0; i<nIterations; i++) {
	
      countCalls++;
      foundSeed = findSeed( par[0]-xRange, par[0]+xRange, par[1]-phiRange, par[1]+phiRange, par);
      if ( !foundSeed ) break;
								  								  
      xRange /= 3.; 
      phiRange /= 2.; 
      if ( xRange < 0.01 || phiRange < 0.00001) break; 
    }
    if (!foundSeed) break;
	
	if (m_magneticField) findSeedInverseR(par);

// remove the hits associated with this region, save this region, search again
    int countAssociatedHits[] = {0, 0}; 
    double cosphi = cos( par[1] );
    double sinphi = sqrt( 1. - cosphi*cosphi );
	double range = (m_magneticField) ? 10. : 2.; // BE CAREFULL ABOUT THIS RANGE, IT USED TO BE 2 BUT PERHAPS IT SHOULD BE INCREASED TO 10 in the case of magnetic field!
	
	
	double measx[200], measy[200];
	int countMeas(0);
	
	for (std::vector< Amg::Vector3D>::iterator it = m_listHitCenter.begin(); it!=m_listHitCenter.end(); ) {

      double a = (par[3]-it->x())*sinphi+(it->y()-par[4])*cosphi; 
	  double b = (m_magneticField) ? 0.5 * (pow(it->x()-par[3], 2.) + pow(it->y()-par[4], 2.) - pow(a, 2)) : 0.;
	  if ( fabs(a+par[2]*b) > range ) { it++; continue; } 
	  
	  if (m_magneticField && countMeas<200) { measx[countMeas] = it->x(); measy[countMeas] = it->y(); countMeas++; }
	  
      countAssociatedHits[(it->y()>0)?0:1]++;
//	  fprintf(fout, "%d %lf %lf %lf %d\n", eventCount, it->x(), it->y(), a+par[2]*b, foutCount);

      it = m_listHitCenter.erase( it ); 
    } 
//	foutCount++;

    if ( countAssociatedHits[0]+countAssociatedHits[1] < m_minHitsAboveTOT ) continue;

    if (m_magneticField) segFit(measx, measy, countMeas, 0, par+3);

	if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "countAssociatedHits " << countAssociatedHits[0] << " " 
	                                    << countAssociatedHits[1] << " m_minHitsAboveTOT " << m_minHitsAboveTOT << endmsg;
    x0.push_back( par[0] ); phi.push_back( par[1] ); nHitsPosY.push_back( countAssociatedHits[0] ); nHitsNegY.push_back( countAssociatedHits[1] );

    pivotX.push_back( par[3] ); pivotY.push_back( par[4] ); Xparabola.push_back( par[5] ); cotanParabola.push_back( par[6] ); InverseR.push_back( par[7] );
  } // end for (int i=0; i<nIterations; i++) loop
// end finding seeds

// save all TRT hits for found segments

  int nFoundSegments = x0.size(); if (nFoundSegments>10) nFoundSegments = 10; // number of found segments
  { std::vector<const InDet::TRT_DriftCircle *> tmp; for (int i=0; i<nFoundSegments; i++) m_segmentDriftCircles.push_back(tmp); }

  double window = (m_magneticField) ? 4. : 2.; // change for magnetic field
  double residual;

  for (unsigned int i=0; i<m_listHits.size(); i++) {
  
    const Amg::Vector3D &center = m_listHits[i]->detectorElement()->surface(m_listHits[i]->identify()).center();  
    for (int j=0; j<nFoundSegments; j++) {
	
      if (nHitsPosY[j]<5 && center.y()>0.) continue;
      if (nHitsNegY[j]<5 && center.y()<0.) continue;
      	  
	  if (m_magneticField) {
        double sinphi = sqrt(1./(1+cotanParabola[j]*cotanParabola[j])); 
        double cosphi = sqrt(1.-sinphi*sinphi); if (cotanParabola[j]<0.) cosphi *= -1.;
        double a = (pivotX[j]+Xparabola[j]-center.x())*sinphi+(center.y()-pivotY[j])*cosphi; 
	    double b = 0.5 * (pow(center.x()-pivotX[j], 2.) + pow(center.y()-pivotY[j], 2.) - pow(a, 2)) ;        
	    residual = a + InverseR[j] * b;
		
	  } else {
		double cosphi = cos(phi[j]);
		double sinphi = sqrt(1.-cosphi*cosphi);
		residual = (x0[j]-center.x())*sinphi+center.y()*cosphi; 		
	  }	
	  
      if (fabs(residual)<window) { 
//		fprintf(fout, "%d %lf %lf %lf %d\n", eventCount, center.x(), center.y(), residual, 10+j);
        m_segmentDriftCircles[j].push_back(m_listHits[i]);
	    break;
      }
    }
  }
  // end saving TRT hits  



  // merge segments: can be simple: if 2 of them similar values of impact par -> copy hits from the second vector into the first one and clear the second one
  // XXXXXXXX to do: determine the optimal merging parameters

if (m_mergeSegments) { // merge segments, not yet tested properly

  if (x0.size()>1) {
    int mergeI = 0;
	int mergeJ = 0;
	double mergeX0(100.), mergePhi(0.1);
    for (int i=0; i<nFoundSegments; i++) {
	  for (int j=i+1; j<nFoundSegments; j++) {
	    if ( fabs(x0[i]-x0[j])<mergeX0 && fabs(phi[i]-phi[j])<mergePhi ) {
		  mergeI = i;
		  mergeJ = j;
		  mergeX0 = fabs(x0[i]-x0[j]);
		  mergePhi = fabs(phi[i]-phi[j]); 
		}
	  }
	}  
    if (mergeI != mergeJ) {
      if (m_debugLevel <= MSG::DEBUG)
	    msg(MSG::DEBUG) << "Merge segments " << mergeI << " and " << mergeJ << " of size " << m_segmentDriftCircles[mergeI].size() << ", " << m_segmentDriftCircles[mergeJ].size()
	        << "; difference in the impact par x: " << mergeX0 << " phi: " << mergePhi << endmsg;
	  for (unsigned int i=0; i<m_segmentDriftCircles[mergeJ].size(); i++) m_segmentDriftCircles[mergeI].push_back(m_segmentDriftCircles[mergeJ][i]);  
	  m_segmentDriftCircles[mergeJ].clear();
	}	
  }
} // end merge segments
  

  if (m_debugLevel <= MSG::DEBUG) {  // debug: check how many hits per found segments  
    msg(MSG::DEBUG) << "find() debug (" << nFoundSegments << ")" ;
    for (unsigned int i=0; i<m_segmentDriftCircles.size(); i++) msg(MSG::DEBUG) << " " << i << " " << m_segmentDriftCircles[i].size() ;
    msg(MSG::DEBUG) << endmsg;
  }
  
  for (unsigned int i=0; i<m_segmentDriftCircles.size(); i++) { // convert to segments
    if ((int)m_segmentDriftCircles[i].size()<m_minHitsForSegment) continue;	
    double trackpar[] = {x0[i], phi[i]};
    convert(m_segmentDriftCircles[i], trackpar);
  }
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "find(), number of converted segments: " << m_segments.size() << endmsg;

//  fclose(fout);
 
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//   the last method inherited from ITRT_TrackSegmentsMaker: next()
//   returns ptr to i-th found TrackSegment, until it returns 0 (no more segments)
//   assumes any provided segments will be deleted by those who request them!!!
//////////////////////////////////////////////////////////////////////////////////////////

Trk::TrackSegment *InDet::TRT_TrackSegmentsMaker_BarrelCosmics::next() { 

  // next 6 lines: for debugging purposes only
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::next(): return " 
                                      << m_segmentDriftCirclesCount << " out of " << m_segments.size() << endmsg;
									  
  if (m_segmentDriftCirclesCount > m_segments.size()) 
    msg(MSG::ERROR) << "m_segmentDriftCirclesCount = " << m_segmentDriftCirclesCount << ", m_segments.size() = " 
                      << m_segments.size() << endmsg;
		  
  return (m_segmentDriftCirclesCount<m_segments.size())?(m_segments[m_segmentDriftCirclesCount++]):0; 
}

//////////////////////////////////////////////////////////////////////////////////////////
//   utility methods: clear, findSeed 
//////////////////////////////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::clear() { // clears data members

  msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::clear(), current m_segments.size() = " << m_segments.size() << endmsg;

  m_listHits.clear();
  m_listHitCenter.clear();

  m_segmentDriftCircles.clear();
  m_segments.clear(); // need to delete them as well? check later 
 
  m_segmentDriftCirclesCount = 0;

  return;
}


int InDet::TRT_TrackSegmentsMaker_BarrelCosmics::findSeed(double xmin, double xmax, double phimin, double phimax, double *bestParameters) {

  if ((int)m_listHitCenter.size()<m_minHitsAboveTOT) return 0;

  double xbin = (xmax-xmin) / (1.*m_nBinsInX);
  double searchWindow = (xbin>2.) ? xbin : 2.; //searchWindow *= searchWindow; 
  double phibin = (phimax-phimin) / (1.*m_nBinsInPhi);
  int maxHits(0), index, indexmin, indexmax;

  int binInX[100]; for (int i=0; i<100; i++) binInX[i] = 0;

  for (int j=0; j<m_nBinsInPhi; j++) {
  
    double phi = phimin+(0.5+j)*phibin;
    double cosphi( cos(phi) ), sinphi( sin(phi) );
    for (int i=0; i<m_nBinsInX; i++) binInX[i] = 0;
    double transformXMin(xmin*sinphi); xbin = (xmax-xmin)*sinphi / (1.*m_nBinsInX);
	
    for (unsigned int k=0; k<m_listHitCenter.size(); k++) {
      double transformX = m_listHitCenter[k].x() * sinphi - m_listHitCenter[k].y() * cosphi; 
      indexmin = (int) ((transformX-transformXMin-searchWindow)/xbin);
      indexmax = (int) ((transformX-transformXMin+searchWindow)/xbin);
	  
      if (indexmin<0) indexmin=0;
      if (indexmax>99) indexmax=99;
      for (index = indexmin; index<=indexmax; index++) binInX[index]++;
    }
    index = -1;
    for (int i=0; i<m_nBinsInX; i++) if (binInX[i]>maxHits) { index = i; maxHits = binInX[i]; }
    if (index>=0) { 
      bestParameters[0] = ( xbin * index + transformXMin) / sinphi; 
      bestParameters[1] = phi; 
    }
  }
  
  return (maxHits>=m_minHitsForSeed) ? 1 : 0;
}

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::findSeedInverseR(double *par) {

  double cosphi = cos( par[1] );
  double sinphi = sqrt( 1. - cosphi*cosphi );

  double parTransformX = par[0] * sinphi;

  double meanTransformY(0.); 
  int countMeanTransformY(0);
  for (unsigned int k=0; k<m_listHitCenter.size(); k++) { // first determine the mean
  
    double transformX = m_listHitCenter[k].x() * sinphi - m_listHitCenter[k].y() * cosphi; 
	if ( fabs(transformX-parTransformX) > 2. ) continue;
	meanTransformY += m_listHitCenter[k].x() * cosphi + m_listHitCenter[k].y() * sinphi; 
	countMeanTransformY++;
  }
  if (!countMeanTransformY) {
    msg(MSG::WARNING) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::findSeedInverseR(), no hits in the seed region???" << endmsg;
    return;
  }
  meanTransformY /= (double) countMeanTransformY;


// hard-coded parameters in the second part of this function:
// search range: [-0.00202, 0.00202] divided into 101 bins of width 0.00004 (such that range [-0.00002, 0.00002] is bin 50)
// 1/R = 0.002/mm corresponds to R = 0.5 m
// circle search window: 200 mm (for R=1m, circle-to-line distance 0.5 m away from a common point is 130mm)

// -> pivot at: x_pivot = meanTransformY * cosphi + parTransformX * sinphi; y_pivot = meanTransformY * sinphi - parTransformX * cosphi;		
  double mean[] = { meanTransformY * cosphi + parTransformX * sinphi, meanTransformY * sinphi - parTransformX * cosphi };  
  int scanInverseR[101]; for (int i=0; i<101; i++)  scanInverseR[i] = 0;

  double window = 10.; // parabola search window BE CAREFULL OF THIS RANGE!

  for (std::vector< Amg::Vector3D >::iterator it = m_listHitCenter.begin(); it!=m_listHitCenter.end(); it++) {
  
    double transformX = it->x() * sinphi - it->y() * cosphi; 
	if ( fabs(transformX-parTransformX) > 200. ) continue;  // search circles in a broad band

	double a = (mean[0]-it->x())*sinphi+(it->y()-mean[1])*cosphi;  // TEST THAT THIS CONDITION ( if ( fabs(a) > 200. ) continue; ) IS THE SAME AS ABOVE!
	double b = 0.5 * (pow(it->x()-mean[0], 2.) + pow(it->y()-mean[1], 2.) - pow(a, 2));

    // estimated allowed range in radius: x_1,2 = ( +- window - a ) / b -> get integer as  ((int) (ceil(x/bin-0.5))) + 50 
	b *= 0.00004; // multiply by the bin width

    int i1(0), i2(0);
    if (fabs(b)>0.0000001) {
      double i1_tmp = ( window - a ) / b + 0.5;
      double i2_tmp = ( -window - a ) / b + 0.5; 
      if (fabs(i1_tmp)<1000.) { i1 = (int) (ceil( i1_tmp )); i1 += 50; }
      if (fabs(i2_tmp)<1000.) { i2 = (int) (ceil( i2_tmp )); i2 += 50; }
    }
    if (i1>100) { i1 = 100; } else { if (i1<0) i1 = 0; }
    if (i2>100) { i2 = 100; } else { if (i2<0) i2 = 0; }
    if (i1+i2==0 || i1+i2==200) continue; // out of search range
	if (i1<=i2) { for (int i=i1; i<=i2; i++) scanInverseR[i]++; }
	else { for (int i=i2; i<=i1; i++) scanInverseR[i]++; }	
  }

  int iMax(-1), hitsMax(0);
  for (int i=0; i<101; i++) if (scanInverseR[i]>hitsMax) { iMax = i; hitsMax = scanInverseR[i]; } // find max bin
	  
  par[2] = 0.00004 * (iMax-50); // save estimated inverse radius
  for (int i=0; i<2; i++) par[3+i] = mean[i]; // save pivot x, y

  return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//   convert list of TRT hits to "TrackSegment": convert()
// 
//   the only TrackSegment constructor one can use for the purpose:
// 
//   TrackSegment( const LocalParameters* locpars,  // LocalParameters(radius, z0, phi0, theta, QoverP) at Surface point "sf"
//                 const ErrorMatrix* locerr,       // error matrix for above params
//                 const Surface* sf,               // 
//                 DataVector<const MeasurementBase>* crots, // TRT_DriftCircleOnTrack : RIO_OnTrack : MeasurementBase
//                 FitQuality* fqual,               // FitQuality(float chi2, short ndf)
//                 Segment::Author author = Segment::AuthorUnknown
//               ); 
//////////////////////////////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::linearRegressionParabolaFit(double *mean, double *a) { // input: elsewhere calculated mean, output: result a

  double A[6], discriminant; 
  
  A[0] = mean[8]*mean[8]-mean[4]*mean[9];                    // parabola fit
  A[1] = mean[1]*mean[9]-mean[4]*mean[8];
  A[2] = mean[4]*mean[4]-mean[1]*mean[8];
  A[3] = mean[4]*mean[4]-mean[9];
  A[4] = mean[8]-mean[1]*mean[4];
  A[5] = mean[1]*mean[1]-mean[4];
  discriminant = fabs( pow(mean[4], 3) + mean[8]*mean[8] + mean[1]*mean[1]*mean[9] - mean[4]*(2.*mean[1]*mean[8]+mean[9]) );

  a[0] = A[0] * mean[0] + A[1] * mean[2] + A[2] * mean[6];
  a[1] = A[1] * mean[0] + A[3] * mean[2] + A[4] * mean[6];
  a[2] = A[2] * mean[0] + A[4] * mean[2] + A[5] * mean[6];
  discriminant *= -1.;
  for (int i=0; i<3; i++) { a[i] /= discriminant; }

  double inverseSin3phi = 1. + a[1]*a[1]; inverseSin3phi *= sqrt(inverseSin3phi); // 1/sin^3phi
  a[2] *= 2. / inverseSin3phi; // inverse R

  return;
}

bool InDet::TRT_TrackSegmentsMaker_BarrelCosmics::sortHits( const InDet::TRT_DriftCircle *dc1, const InDet::TRT_DriftCircle *dc2 ) {

  double y1 = (dc1->detectorElement())->surface(dc1->identify()).center().y();
  double y2 = (dc2->detectorElement())->surface(dc2->identify()).center().y();
  return ( y1 > y2 );
}

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::convert(std::vector<const InDet::TRT_DriftCircle *> &hits, double *trackpar) { 
//Track Segment production - based on TRT_TrackSegmentsMaker_ECcosmics Tool
  
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::convert() segment " << m_segments.size() << ", N hits = " << hits.size() << endmsg;
  
  if (hits.size()<5) { msg(MSG::ERROR) << "convert(): empty list of hits! size: " << hits.size() << endmsg; return; }
 
  // sort the vector of hits 
  std::sort( hits.begin(), hits.end(), InDet::TRT_TrackSegmentsMaker_BarrelCosmics::sortHits );

  // remove hits from the opposite side
  int countOppositeSide(0); 
  for (std::vector<const InDet::TRT_DriftCircle *>::iterator it = hits.begin(); it != hits.end(); ) { 
    int side = m_trtid->barrel_ec( (*it)->identify() );
    int previous = ( it == hits.begin() ) ? m_trtid->barrel_ec( (*(it+2))->identify() ) : m_trtid->barrel_ec( (*(it-1))->identify() );
    int next = ( it == hits.end()-1 ) ? m_trtid->barrel_ec( (*(it-2))->identify() ) : m_trtid->barrel_ec( (*(it+1))->identify() ); 
//    if ( previous==next && side*next==-1 ) { std::vector<const InDet::TRT_DriftCircle *>::iterator tmpit = it; it--; hits.erase( tmpit ); countOppositeSide++; }
    if ( previous==next && side*next==-1 ) { it = hits.erase( it ); countOppositeSide++; }
    else { it++; }
  }
  if (countOppositeSide>5) 
   if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "convert(): removed " << countOppositeSide << " hits from the other side, N remaining hits: " << hits.size() << endmsg; 
  if (hits.size()<5) { 
    msg(MSG::WARNING) << "convert(): not enough hits after opposite side removal: " << hits.size() << ", removed: " << countOppositeSide << endmsg; 
    return; 
  }

  // make the linear regression fit  
//  int side(0); // check if more hits on side A or C
  double mean[10]; for (int i=0; i<10; i++) mean[i] = 0.; 
  for (unsigned int i=0; i<hits.size(); i++) {  

    mean[0] += (hits[i]->detectorElement())->surface(hits[i]->identify()).center().x();
    mean[1] += (hits[i]->detectorElement())->surface(hits[i]->identify()).center().y();	
//	side += m_trtid->barrel_ec(hits[i]->identify());
  }
  for (int i=0; i<2; i++) mean[i] /= (double) hits.size();
  int iPivot(-1); double yPivot(10000000.); // choose pivot closest to the mean
  for (unsigned int i=0; i<hits.size(); i++) {  
    double x = (hits[i]->detectorElement())->surface(hits[i]->identify()).center().x() - mean[0];
	double y = (hits[i]->detectorElement())->surface(hits[i]->identify()).center().y() - mean[1];
	double x2 = x * x;
    double y2 = y * y;
    mean[2] += x * y;
    mean[3] += x2;	
    mean[4] += y2;	
    mean[5] += x2 * y;
    mean[6] += y2 * x;
    mean[7] += x2 * x;
    mean[8] += y2 * y;	
	mean[9] += y2 * y2; 	
	if (x2+y2<yPivot) { yPivot = x2+y2; iPivot = i; }
  }

  if (iPivot==-1) {
    msg(MSG::ERROR) << "SF pivot index not found!!! " << yPivot << " " << iPivot << endmsg;
    iPivot = 0;
  }

  double cotanPhi = mean[2] / mean[4];
  double phi = atan(1./cotanPhi); if (phi<0.) phi += M_PI;
  if (m_debugLevel <= MSG::DEBUG) {
    msg(MSG::DEBUG) << "compare parameters X  : " << trackpar[0] << " vs. " << mean[0]-mean[1]*cotanPhi << endmsg;
    msg(MSG::DEBUG) << "compare parameters phi: " << trackpar[1] << " vs. " << phi << endmsg;
  }	

  double qOverp = 0.; // units q / MeV, set only if there is magnetic field

  if (m_magneticField) { // fit parabola instead of line, add circle if neccessary

    double A[6], discriminant, a[3]; 
  
    A[0] = mean[8]*mean[8]-mean[4]*mean[9];                    // parabola fit
    A[1] = mean[1]*mean[9]-mean[4]*mean[8];
    A[2] = mean[4]*mean[4]-mean[1]*mean[8];
    A[3] = mean[4]*mean[4]-mean[9];
    A[4] = mean[8]-mean[1]*mean[4];
    A[5] = mean[1]*mean[1]-mean[4];
    discriminant = fabs( pow(mean[4], 3) + mean[8]*mean[8] + mean[1]*mean[1]*mean[9] - mean[4]*(2.*mean[1]*mean[8]+mean[9]) );

    a[0] = A[0] * mean[0] + A[1] * mean[2] + A[2] * mean[6];
    a[1] = A[1] * mean[0] + A[3] * mean[2] + A[4] * mean[6];
    a[2] = A[2] * mean[0] + A[4] * mean[2] + A[5] * mean[6];
    discriminant *= -1.;
    for (int i=0; i<3; i++) { a[i] /= discriminant; }
	
    double Xparabola = a[0];
    double cotanParabola = a[1];
    double inverseSin3phi = 1. + a[1]*a[1]; inverseSin3phi *= sqrt(inverseSin3phi); // 1/sin^3phi
    double inverseR = 2. * a[2] / inverseSin3phi;
    double sinphi = sqrt(1./(1+a[1]*a[1])); 
    double cosphi = sqrt(1.-sinphi*sinphi); if (cotanParabola<0.) cosphi *= -1.;

    if (m_debugLevel <= MSG::DEBUG) {
      msg(MSG::DEBUG) << "TRT_TrackSegmentsMaker_BarrelCosmics: parabola fit, X: " << Xparabola << endmsg;
	  msg(MSG::DEBUG) << "                                      parabola fit, cotan: " << cotanParabola << " compare to " << cotanPhi << endmsg;
	  msg(MSG::DEBUG) << "                                      parabola fit, 1/R: " << inverseR << "/ mm " << endmsg;
	}  
 
    qOverp = inverseR / 0.6;  // [1/MeV]; 1 / (eBRC) = 1 / (e 2T 3 100 M m/s mm) / R[mm] = (1/R[mm]) / 0.6 
  }

// errors from the fit: 
// - for d: sigma_r / sqrt(N hits) -> sigma_d^2 =  
// - for cotanPhi: sigma_r / (sqrt(N hits * <y^2>) * sinPhi) 
// - for phi: 1 / (1 + cotanPhi^2) * DcotanPhi // note that this mthod gives a biased estimate for phi!
    
  Amg::MatrixX cov(5,5);

  double f33 = 4. / ( 3. * (1.+cotanPhi*cotanPhi) * mean[4] ); // see above, mean[4] = N hits * <y^2> 
  cov<<    
    4.  ,    0.,   0.,     0.,  0.,
    0.  ,45633.,   0.,     0.,  0.,
    0.  ,    0.,  f33,     0.,  0.,
    0.  ,    0.,   0.,    0.2,  0.,
    0.  ,    0.,   0.,     0.,   1.;

//  delete cov;

//  if (mean[0]*sin(phi)+mean[1]*cos(phi)<0.) phi -= M_PI; // to satisfy TrigTRTSegFinder bug

//  const Trk::Surface *sur =  &(hits[iPivot]->detectorElement())->surface(hits[iPivot]->identify());  

  Amg::Vector3D hepVec( mean[0], mean[1], 
			 (hits[iPivot]->detectorElement())->surface(hits[iPivot]->identify()).center().z() );
  

  Amg::Transform3D* htrans = new Amg::Transform3D(Amg::Translation3D(hepVec)*Amg::RotationMatrix3D::Identity());
  
  

  Trk::StraightLineSurface *sur = new Trk::StraightLineSurface(htrans);
 
//  Trk::GlobalPosition gloPos(mean[0]-mean[1]*cotanPhi, mean[1], sur->center().z() );
//  const Trk::LocalPosition *locPos = sur->globalToLocal(gloPos);
//  trackpar[0] = mean[0]-mean[1]*cotanPhi; trackpar[1] = phi;
//  double locR = XXXXXXXX // distance function removed distance(trackpar, sur->center()); // locPos->get(Trk::loc1)

//  locR = 0.;
//  if (sur->center().x()*cos(phi)+sur->center().y()*sin(phi)<0.) phi -= M_PI;
//  if (mean[0]*cos(phi)+mean[1]*sin(phi)<0.) phi -= M_PI;
  if (phi>0.) phi -= M_PI;
  if (phi<-M_PI || phi>0.) msg(MSG::ERROR) << "phi value problem: " << phi << endmsg;

  Trk::LocalParameters par(0., 0., phi, M_PI_2, qOverp);

  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "pivot: " << sur->center() << ", cross-check: " << mean[0] << " " << mean[1] << endmsg;

  // calculate TrackParameters so that one can calculate input for TRT_DriftCircleOnTrack
  // Global direction of the track parameters
  Amg::Vector3D dir(cos(phi), sin(phi), 0.); 
  
  DataVector<const Trk::MeasurementBase> *rio = new DataVector<const Trk::MeasurementBase>;

  for (unsigned int i=0; i<hits.size(); i++) { // rewrite: InDet::TRT_DriftCircle -> InDet::TRT_DriftCircleOnTrack
  // based on http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/InnerDetector/InDetRecTools/TRT_DriftCircleOnTrackTool/src/TRT_DriftCircleOnTrackNoDriftTimeTool.cxx?revision=1.11&view=markup  
  
    const InDet::TRT_DriftCircle* DC = hits[i];

    // Straw identification
    const InDetDD::TRT_BaseElement* pE = DC->detectorElement(); if(!pE) {msg(MSG::ERROR) << "convert(): no detectorElement info!" << endmsg; continue; }
    Identifier     iD = DC->identify();
    IdentifierHash iH = m_trtid->straw_layer_hash(m_trtid->layer_id(iD));
 
    // TRT_DriftCircleOnTrack production
    Trk::DefinedParameter  radius(0.,Trk::locX);
    Trk::LocalParameters lp(radius);  
    Amg::MatrixX cov(1,1); cov<<1.33333; 
    const InDet::TRT_DriftCircleOnTrack *element = new InDet::TRT_DriftCircleOnTrack(DC, lp, cov, iH, 1., dir, Trk::NODRIFTTIME);

    rio->push_back( dynamic_cast<const Trk::MeasurementBase *>(element) ); 
  } // end fill rio

  // add pseudo measurement - follow up https://savannah.cern.ch/bugs/?41079
if (1) { // limit the scope of all these variables

  Trk::DefinedParameter pseudoz( 0., Trk::locZ);
  Trk::DefinedParameter pseudotheta( M_PI/2., Trk::theta);
  Trk::LocalParameters pseudoPar( pseudoz, pseudotheta );

  Amg::MatrixX pseudocov(2,2);
  pseudocov <<
    1.  ,   0.,
    0.  , .001;

  const Trk::Surface &surf =  (hits[hits.size()-1]->detectorElement())->surface(hits[hits.size()-1]->identify()); 

  Amg::RotationMatrix3D linerot=surf.transform().rotation();
  Amg::Vector3D         firstcenter=surf.center();
  Amg::Vector3D         faketransl(firstcenter.x(),firstcenter.y()-5.,firstcenter.z());
  Amg::Transform3D *    faketransf=new Amg::Transform3D(linerot* Amg::Translation3D(faketransl));
  Trk::StraightLineSurface *pseudoSurface = new Trk::StraightLineSurface( faketransf, 99999., 99999. );  

  Trk::PseudoMeasurementOnTrack *pseudo = new Trk::PseudoMeasurementOnTrack( pseudoPar, pseudocov, *pseudoSurface );
  rio->push_back( pseudo );
  delete pseudoSurface;
}
 
  double chi2 = mean[3] - 2.*cotanPhi*mean[2] + mean[4]*cotanPhi*cotanPhi;
  chi2 /= ( 1. + cotanPhi*cotanPhi );
  int ndf = (int) hits.size() - 2;
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "Chi2 = " << chi2 << ", ndf = " << ndf << ", chi2/ndf = " << chi2/ndf << endmsg;
  Trk::FitQuality      * fqu = new Trk::FitQuality(chi2, ndf);

  Trk::TrackSegment *segment = new Trk::TrackSegment(par,cov, sur, rio, fqu, Trk::Segment::TRT_SegmentMaker);
  
  //add segment to list of segments
  msg(MSG::DEBUG) << "Add " << m_segments.size() << "th segment to list" << endmsg;
  m_segments.push_back(segment);

  return; 
} 

//////////////////////////////////////////////////////////////////////////////////////////
// 
//   DEBUG FUNCTIONS
// 
//////////////////////////////////////////////////////////////////////////////////////////

void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::segFit(double *measx, double *measy, int nhits, double *residuals, double *result ) {

  if (nhits<10) return;

  double shift[] = {0., 0.};                                  // shift the points so that mean x = 0 and mean y = 0
  for (int i=0; i<nhits; i++) { shift[0] += measx[i]; shift[1] += measy[i]; }
  for (int i=0; i<2; i++) shift[i] /= (double) nhits;
  for (int i=0; i<nhits; i++) { measx[i] -= shift[0]; measy[i] -= shift[1]; }

  double mean[10]; for (int i=0; i<10; i++) mean[i] = 0.;    // calculate momenta needed for the line, circle and parabola fit
  for (int i=0; i<nhits; i++) {
    double x2 = measx[i]*measx[i];
	double y2 = measy[i]*measy[i];
    mean[0] += measx[i];
    mean[1] += measy[i];
    mean[2] += measx[i]*measy[i];
    mean[3] += x2;
    mean[4] += y2;
    mean[5] += x2 * measy[i];
    mean[6] += y2 * measx[i];
    mean[7] += x2 * measx[i];
    mean[8] += y2 * measy[i];	
	mean[9] += y2 * y2; 
  }  
  for (int i=0; i<10; i++)  mean[i] /= (double) nhits; 

  double A[6], discriminant, a[3]; 

  A[0] = mean[8]*mean[8]-mean[4]*mean[9];                    // parabola fit, no need for mean[3], [5], [7], se mi zdi
  A[1] = mean[1]*mean[9]-mean[4]*mean[8];
  A[2] = mean[4]*mean[4]-mean[1]*mean[8];
  A[3] = mean[4]*mean[4]-mean[9];
  A[4] = mean[8]-mean[1]*mean[4];
  A[5] = mean[1]*mean[1]-mean[4];
//  if (m_debug) for (int i=0; i<6; i++) printf("A[%d] = %lf\n", i, A[i]);
  discriminant = fabs( pow(mean[4], 3) + mean[8]*mean[8] + mean[1]*mean[1]*mean[9] - mean[4]*(2.*mean[1]*mean[8]+mean[9]) );
//  if (m_debug) printf("discriminant = %lf\n", discriminant);  

  a[0] = A[0] * mean[0] + A[1] * mean[2] + A[2] * mean[6];
  a[1] = A[1] * mean[0] + A[3] * mean[2] + A[4] * mean[6];
  a[2] = A[2] * mean[0] + A[4] * mean[2] + A[5] * mean[6];
  discriminant *= -1.;
  for (int i=0; i<3; i++) a[i] /= discriminant; 
  double Xparabola = a[0];
  double cotanParabola = a[1];
  double inverseSin3phi = 1. + a[1]*a[1]; inverseSin3phi *= sqrt(inverseSin3phi); // 1/sin^3phi
  double inverseR = 2. * a[2] / inverseSin3phi;
  double sinphi = sqrt(1./(1+a[1]*a[1])); 
  double cosphi = sqrt(1.-sinphi*sinphi); if (cotanParabola<0.) cosphi *= -1.;

  if (result) {
    for (int i=0; i<2; i++) result[i] = shift[i]; // pivot
	result[2] = Xparabola;
    result[3] = cotanParabola;
	result[4] = inverseR;
  }
  
  if (residuals) {
    for (int i=0; i<nhits; i++) {    
	  double tmp = (Xparabola-measx[i])*sinphi+measy[i]*cosphi;
      double res = (tmp+0.5*inverseR*(pow(measx[i]-Xparabola, 2.) + pow(measy[i], 2.) - pow(tmp, 2))); 
      residuals[i] = res;
    }
  }

  for (int i=0; i<nhits; i++) { measx[i] += shift[0]; measy[i] += shift[1]; }
  
  return;
}


//////////////////////////////////////////////////////////////////////////////////////////
// 
//   DEBUG cpu consumption: old find method
// 
//////////////////////////////////////////////////////////////////////////////////////////


void InDet::TRT_TrackSegmentsMaker_BarrelCosmics::findOld(){

  msg(MSG::DEBUG) << "InDet::TRT_TrackSegmentsMaker_BarrelCosmics::find()" << endmsg;

  if ((int)m_listHitCenter.size()<m_minHitsAboveTOT) return;

  if (m_segmentDriftCircles.size()) { // debug only
    msg(MSG::WARNING) << "find probably called twice per event? or newEvent / newRegion have not been called. check your program" << endmsg;
    clear(); return; 
  } 

  std::vector<double> x0, phi;
  std::vector<int> nHitsPosY, nHitsNegY;

  // first find seeds
  int nIterations(15), countCalls(0);
  while (countCalls<150) {

    double xRange(1000.), phiRange(M_PI_4);
    double par[] = {0., M_PI_2}; 
    int foundSeed(0);

    for (int i=0; i<nIterations; i++) {
	
      countCalls++;
      foundSeed = findSeed( par[0]-xRange, par[0]+xRange, par[1]-phiRange, par[1]+phiRange, par);
      if ( !foundSeed ) break;
								  								  
      xRange /= 3.; 
      phiRange /= 2.; 
      if ( xRange < 0.01 || phiRange < 0.00001) break;

    }
    if (!foundSeed) break;

// remove the hits associated with this region, save this region, search again
    int countAssociatedHits[] = {0, 0}; 
    double cosphi = cos(par[1]);
	double sinphi = sqrt(1.-cosphi*cosphi);
	
	for (std::vector< Amg::Vector3D >::iterator it = m_listHitCenter.begin(); it!=m_listHitCenter.end(); ) {

      if ( fabs( (par[0]-it->x())*sinphi + it->y()*cosphi ) > 2. ) { it++; continue; }
      countAssociatedHits[(it->y()>0)?0:1]++;
      it = m_listHitCenter.erase( it );
    } 

    if ( countAssociatedHits[0]+countAssociatedHits[1] < m_minHitsAboveTOT ) continue;

    x0.push_back( par[0] ); phi.push_back( par[1] ); nHitsPosY.push_back( countAssociatedHits[0] ); nHitsNegY.push_back( countAssociatedHits[1] );

  } // end for (int i=0; i<nIterations; i++) loop
// end finding seeds

// save all TRT hits for found segments
  int nFoundSegments = x0.size(); if (nFoundSegments>10) nFoundSegments = 10; // number of found segments
  { std::vector<const InDet::TRT_DriftCircle *> tmp; for (int i=0; i<nFoundSegments; i++) m_segmentDriftCircles.push_back(tmp); }

  for (unsigned int i=0; i<m_listHits.size(); i++) {
    const Amg::Vector3D &center = m_listHits[i]->detectorElement()->surface(m_listHits[i]->identify()).center();  
    for (int j=0; j<nFoundSegments; j++) {
      if (nHitsPosY[j]<5 && center.y()>0.) continue;
      if (nHitsNegY[j]<5 && center.y()<0.) continue;
      double cosphi = cos(phi[j]);
	  double sinphi = sqrt(1.-cosphi*cosphi);	  
      if ( fabs((x0[j]-center.x())*sinphi+center.y()*cosphi) < 2.) {
        m_segmentDriftCircles[j].push_back(m_listHits[i]);
	break;
      }
    }
  }
  // end saving TRT hits  

  if (m_debugLevel <= MSG::DEBUG) {
  // debug: check how many hits per found segments  
  msg(MSG::DEBUG) << "find() debug (" << nFoundSegments << ")" ;
  for (unsigned int i=0; i<m_segmentDriftCircles.size(); i++) msg(MSG::DEBUG) << " " << i << " " << m_segmentDriftCircles[i].size() ;
  msg(MSG::DEBUG) << endmsg;
  }
  
  for (unsigned int i=0; i<m_segmentDriftCircles.size(); i++) { // convert to segments
    if ((int)m_segmentDriftCircles[i].size()<m_minHitsForSegment) continue;
    double trackpar[] = {x0[i], phi[i]};
    convert(m_segmentDriftCircles[i], trackpar);
  }
  if (m_debugLevel <= MSG::DEBUG) msg(MSG::DEBUG) << "find(), number of converted segments: " << m_segments.size() << endmsg;

  return;
}






