/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalPixelTool.cxx
 * Implementation of inner detector global hits monitoring tool
 *
 * @author Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>       
 *
 * $Id: InDetGlobalPixelTool.cxx,v 1.7 2009-04-24 11:53:54 kastanas Exp $
 *
 *********************************************************************************/


//Local
#include "InDetGlobalPixelTool.h"
//Framework
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//Root
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
using namespace TMath;
//Standard c++
#include <string>
//tracks
#include "InDetPrepRawData/PixelClusterContainer.h"         
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "InDetIdentifier/PixelID.h"
//#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkTrack/TrackCollection.h"            
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/PixelClusterContainer.h"         
#include "TrkTrackSummary/TrackSummary.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

InDetGlobalPixelTool::InDetGlobalPixelTool(
					   const std::string & type, 
					   const std::string & name,
					   const IInterface* parent)
  : InDetGlobalMotherMonTool(type, name, parent),
    m_detector("ID"),
    m_tracks(0),
    m_Pixel_track_clus_groupsize(0),
    m_Pixel_track_cluster_ToT(0),
    m_Pixel_track_res_phi(0),
    m_Pixel_track_pull_phi(0),
    m_Pixel_track_res_eta(0),
    m_Pixel_track_pull_eta(0),
    m_Pixel_track_ecA_cluster_occupancy(0),
    m_Pixel_track_ecC_cluster_occupancy(0),
    m_Pixel_track_bar_lay0_cluster_occupancy(0),
    m_Pixel_track_bar_lay1_cluster_occupancy(0),
    m_Pixel_track_bar_lay2_cluster_occupancy(0),
    m_Pixel_track_Lvl1ID(0),
    m_Pixel_track_Lvl1A(0),
    m_Pixel_track_chi2(0),
    m_Pixel_track_qOverP(0),
    m_Pixel_track_d0(0),
    m_Pixel_track_z0(0),
    m_Pixel_track_phi0(0),
    m_Pixel_track_theta(0),
    m_Pixel_track_eta(0)
{
  declareProperty("Detector", m_detector); 
  declareProperty("tracks", m_TracksName="CombinedInDetTracks");
}

//---------------------------------------------------------
StatusCode InDetGlobalPixelTool::bookHistograms()
{
  bool status = true;
  m_Pixel_track_clus_groupsize = new TH1F("m_Pixel_track_clus_groupsize","Number of pixels per cluster for clusters on track",10,0,10);
  status &= regHist(m_Pixel_track_clus_groupsize, "InDetGlobal/Pixel", run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_cluster_ToT = new TH1F("m_Pixel_track_cluster_ToT","Time over threshold for pixel clusters on track",100,0,300);
  status &= regHist(m_Pixel_track_cluster_ToT, "InDetGlobal/Pixel", run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_res_phi = new TH1F("m_Pixel_track_res_phi","Pixel Residual LocX",500,-5,5);
  status &= regHist(m_Pixel_track_res_phi, "InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_pull_phi = new TH1F("m_Pixel_track_pull_phi","Pixel pull LocX",500,-5,5);
  status &= regHist(m_Pixel_track_pull_phi, "InDetGlobal/Pixel", run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_res_eta = new TH1F("m_Pixel_track_res_eta","Pixel Residual LocY",500,-5,5);
  status &= regHist(m_Pixel_track_res_eta,"InDetGlobal/Pixel",  run ).isSuccess();
  m_Pixel_track_pull_eta = new TH1F("m_Pixel_track_pull_eta","Pixel Pull LocY",500,-5,5);
  status &= regHist(m_Pixel_track_pull_eta,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();   
  m_Pixel_track_ecA_cluster_occupancy = new TH2F("m_Pixel_track_ecA_cluster_occupancy","Pixel cluster occupancy in ECA",3,0,3,48,0,48);
  status &= regHist(m_Pixel_track_ecA_cluster_occupancy,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();     
  m_Pixel_track_ecC_cluster_occupancy = new TH2F("m_Pixel_track_ecC_cluster_occupancy","Pixel cluster occupancy in ECC",3,0,3,48,0,48);     
  status &= regHist(m_Pixel_track_ecC_cluster_occupancy,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();     
  m_Pixel_track_bar_lay0_cluster_occupancy = new TH2F("m_Pixel_track_bar_lay0_cluster_occupancy","Pixel cluster occupancy in the first barrel layer",13,-6.5,6.5,22,-0.5,21.5);
  status &= regHist(m_Pixel_track_bar_lay0_cluster_occupancy,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_bar_lay1_cluster_occupancy = new TH2F("m_Pixel_track_bar_lay1_cluster_occupancy","Pixel cluster occupancy in the second barrel layer",13,-6.5,6.5,38,-0.5,37.5);  
  status &= regHist(m_Pixel_track_bar_lay1_cluster_occupancy,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_bar_lay2_cluster_occupancy = new TH2F("m_Pixel_track_bar_lay2_cluster_occupancy","Pixel cluster occupancy in the third barrel layer",13,-6.5,6.5,52,-0.5,51.5);
  status &= regHist(m_Pixel_track_bar_lay2_cluster_occupancy,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_Lvl1A = new TH1F("m_Pixel_track_Lvl1A","Level 1 Accept on track",17,-0.5,16.5);
  status &= regHist(m_Pixel_track_Lvl1A ,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_chi2  = new TH1F("m_Pixel_track_chi2",  "chi2/DoF of reconstructed track (with >0 pixel hits)", 80,-0.,20.);
  status &= regHist(m_Pixel_track_chi2,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_qOverP  = new TH1F("m_Pixel_track_qOverP",  "Reconstructed inverse momentum of track (with >0 pixel hits)", 100,-5.,5.);
  TAxis *xaxis = m_Pixel_track_qOverP->GetXaxis();
  xaxis->SetTitle("Inverse momentum [GeV^{-1}]");
  status &= regHist(m_Pixel_track_qOverP,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_d0  = new TH1F("m_Pixel_track_d0",  "Reconstructed d_0 of track (with >0 pixel hits)", 400,-200.,200.);
  xaxis = m_Pixel_track_d0->GetXaxis();
  xaxis->SetTitle("d_0 [mm]");
  status &= regHist(m_Pixel_track_d0,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_z0  = new TH1F("m_Pixel_track_z0",  "Reconstructed z_0 of track (with >0 pixel hits)", 300,-1000.,1000.);
  xaxis = m_Pixel_track_z0->GetXaxis();
  xaxis->SetTitle("z_0 [mm]");
  status &= regHist(m_Pixel_track_z0,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_phi0  = new TH1F("m_Pixel_track_phi0",  "Reconstructed #phi_0 of track (with >0 pixel hits)", 40,-4.,4.);
  xaxis = m_Pixel_track_phi0->GetXaxis();
  xaxis->SetTitle("#phi_0");
  status &= regHist(m_Pixel_track_phi0,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_theta  = new TH1F("m_Pixel_track_theta",  "Reconstructed #theta of track (with >0 pixel hits)", 30,-0.,4.);
  xaxis = m_Pixel_track_d0->GetXaxis();
  xaxis->SetTitle("#theta");
  status &= regHist(m_Pixel_track_theta,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();
  m_Pixel_track_eta  = new TH1F("m_Pixel_track_eta",  "Reconstructed #eta of track (with >0 pixel hits)", 40,-4.,4.);
  xaxis = m_Pixel_track_eta->GetXaxis();
  xaxis->SetTitle("#eta");
  status &= regHist(m_Pixel_track_eta,"InDetGlobal/Pixel",  run, ATTRIB_MANAGED).isSuccess();

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode InDetGlobalPixelTool::fillHistograms()
{

  //Filling of histograms (loop over collections) :
  m_tracks=m_combined_tracks;
  int nPixelHits=0;


  std::vector<Identifier> myRDOIDs;
  for (int i=0; i<(int)m_tracks->size(); i++)
    {
      const Trk::Track *track=(*m_tracks)[i];
      if (track == 0) 
	{
	  if (msgLvl(MSG::ERROR) ) msg(MSG::ERROR) << "no pointer to track!!!" << endmsg;
	  break;
	}
      const Trk::TrackSummary* summary = m_trkSummaryTool->createSummary(*track);
      if ( summary->get(Trk::numberOfPixelHits) == 0 )
      {
	  delete summary;
	  continue;
      }
               
      else
      {
	  delete summary;
      }
      const Trk::TrackParameters *trkParameters = 0;

      //get the track state on surfaces (a vector, on element per surface) and loop over it
      const DataVector< const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
      DataVector< const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator_end=trackStates->end();
      for ( DataVector< const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurfaceIterator=trackStates->begin();
	    trackStateOnSurfaceIterator!= trackStateOnSurfaceIterator_end; ++trackStateOnSurfaceIterator)
	{
	  //Change the track state on 1 surface into the cluster it represents
	  const InDet::SiClusterOnTrack *clus = dynamic_cast< const InDet::SiClusterOnTrack*>((*trackStateOnSurfaceIterator)->measurementOnTrack());
	  if(!clus)continue;
	  const InDet::SiCluster *RawDataClus = dynamic_cast< const InDet::SiCluster*>(clus->prepRawData());
	  if(!RawDataClus)continue;

	  //if the cluster is a pixel cluster (not SCT) get the pixel information
	  if (RawDataClus->detectorElement()->isPixel())
	    {
	      nPixelHits++;//add another pixel hit 
	      m_Pixel_track_clus_groupsize->Fill(RawDataClus->rdoList().size());  //cluster sizea
	      for(unsigned int loopSize=0;loopSize < RawDataClus->rdoList().size(); loopSize++)
		{
		  myRDOIDs.push_back(RawDataClus->rdoList().at(loopSize));
		}

	      const InDet::PixelCluster * PixClus=dynamic_cast< const InDet::PixelCluster*>(RawDataClus);
	      if ( PixClus )
	      {
		  m_Pixel_track_cluster_ToT->Fill(PixClus->totalToT());         //cluster tot
		  m_Pixel_track_Lvl1A->Fill(PixClus->LVL1A());         //cluster tot
	      }
	      // Get track parameters for current surface
	      trkParameters = (*trackStateOnSurfaceIterator)->trackParameters();
	      if ( trkParameters && trkParameters->covariance() )
	      {
		  // Get local error matrix for hit and track and calc pull
		  const AmgSymMatrix(5) * trackErrMat = trkParameters->covariance();
		  const Amg::MatrixX clusErrMat = clus->localCovariance();

		  //pulls  and residuals Phi
		  double error_sum = sqrt(pow( Amg::error( *trackErrMat, Trk::u ), 2) + pow( Amg::error( clusErrMat, Trk::locX ), 2));
		  double res = clus->localParameters()[Trk::locX]-trkParameters->parameters()[Trk::locX];
		  m_Pixel_track_res_phi->Fill(res);
		  double pull =0;
		  if(error_sum != 0) pull = (res)/error_sum;
		  m_Pixel_track_pull_phi->Fill(pull);
		  
		  //pulls  and residuals Eta
		  error_sum = sqrt(pow( Amg::error( *trackErrMat, Trk::v ), 2) + pow( Amg::error( clusErrMat, Trk::locY ), 2));
		  res = clus->localParameters()[Trk::v]-trkParameters->parameters()[Trk::locY];
		  m_Pixel_track_res_eta->Fill(res);
		  if(error_sum != 0) pull = (res)/error_sum;
		  m_Pixel_track_pull_eta->Fill(pull);
	      }
	      //m_pixelid is the tool which translates the idenifier hash (clus->identify()) into more useful geometric information.
	      //It's initalized at the start of the code
	      double BEC = m_pixelID->barrel_ec(clus->identify());

	      if(BEC==2){
		m_Pixel_track_ecA_cluster_occupancy->Fill(m_pixelID->layer_disk(clus->identify()),m_pixelID->phi_module(clus->identify()));
	      }
	      if(BEC==-2){
		m_Pixel_track_ecC_cluster_occupancy->Fill(m_pixelID->layer_disk(clus->identify()),m_pixelID->phi_module(clus->identify()));
	      }
	      if(BEC==0){
		if(m_pixelID->layer_disk(clus->identify())==0) m_Pixel_track_bar_lay0_cluster_occupancy->Fill(m_pixelID->eta_module(clus->identify()),m_pixelID->phi_module(clus->identify()));
		if(m_pixelID->layer_disk(clus->identify())==1) m_Pixel_track_bar_lay1_cluster_occupancy->Fill(m_pixelID->eta_module(clus->identify()),m_pixelID->phi_module(clus->identify()));
		if(m_pixelID->layer_disk(clus->identify())==2) m_Pixel_track_bar_lay2_cluster_occupancy->Fill(m_pixelID->eta_module(clus->identify()),m_pixelID->phi_module(clus->identify()));
	      }
	    }
	}

      const  Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track->perigeeParameters());
      if ( measPerigee ){
	  m_Pixel_track_qOverP->Fill(measPerigee->parameters()[Trk::qOverP] * 1000.0);
	  m_Pixel_track_d0->Fill(measPerigee->parameters()[Trk::d0]);
	  m_Pixel_track_z0->Fill(measPerigee->parameters()[Trk::z0]);
	  m_Pixel_track_phi0->Fill(measPerigee->parameters()[Trk::phi0]);
	  m_Pixel_track_theta->Fill(measPerigee->parameters()[Trk::theta]);
	  m_Pixel_track_eta->Fill(measPerigee->eta());
      }
      if ( track->fitQuality()->numberDoF() != 0 ) {
	  m_Pixel_track_chi2->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
      }
      
    }
  
  
  return StatusCode::SUCCESS;
}
