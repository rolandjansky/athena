/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetLowBetaFinder/LowBetaAlg.h"
#include "InDetLowBetaFinder/TrtToolsBetaLiklihood.h"

// Athena Control and Services
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

// Track headers
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

// Other headers
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
//#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetIdentifier/TRT_ID.h"

// Calib db access
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"

// ATLAS_NOT_THREAD_SAFE macro
#include "CxxUtils/checker_macros.h"

// Prints the input of TRT_FEbeta() to file (hard-set)
// This is a temporary debug method, and may be removed later
int printTrtToolBetaLiklihoodDebugFile(std::vector<int> TRT_bitpattern, std::vector<int> TRT_bec, std::vector<int> TRT_strawlayer, std::vector<int> TRT_layer,
                           std::vector<float> TRT_t0, std::vector<float> TRT_R, std::vector<float> TRT_R_track,
                           std::vector<float> TrackX, std::vector<float> TrackY, std::vector<float> TrackZ, float RecPt, float RecEta);
               
               
               
                           
StatusCode InDet::LowBetaAlg::initializeTrtToolBetaLiklihood()
{
	// Declare the track refit container
	// Create the TrtTool
	m_TrtTool = new TrtToolBetaLiklihood();
	
	// Load the default prior values
	m_TrtTool->TRT_LoadDefaultPriors();
/*
	// Register a function for obtaining priors from the TRT conditions database
	const DataHandle<CondAttrListCollection> collectionHandle;
	StatusCode SC = detStore()->regFcn(&InDet::LowBetaAlg::update,this,collectionHandle,"/TRT/Calib/MLbetaPriors");
	if (SC.isFailure())
	  ATH_MSG_WARNING("Callback registration failed for LowBetaAlg - priors.  Using default prior values.");
	else
	  ATH_MSG_INFO("Registered callback for updating LowBetaAlg - priors");
*/
	return StatusCode::SUCCESS;
}

int InDet::LowBetaAlg::finalizeTrtToolBetaLiklihood()
{
	// Done with the TrtTool
	delete m_TrtTool;
	
	return 0;
}


// Function for updating the TRT conditions database entries
// Adapted from TRT_DriftFunctionTool.cxx
StatusCode InDet::LowBetaAlg::update(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
	const bool doDebug = false;	// set to true to print to file below
	const char* DEBUGFILE_C = "/afs/cern.ch/user/w/wmills/testarea/16.0.2/InnerDetector/InDetRecAlgs/InDetLowBetaFinder/run/debugPriors.out";
	FILE* dFile = NULL;
	
	ATH_MSG_INFO("Updating priors for the LowBetaAlg likelihood beta estimator");
	
	// Callback function to update priors when condDB data changes:
	for(std::list<std::string>::const_iterator key=keys.begin(); key != keys.end(); ++key)
	ATH_MSG_DEBUG("IOVCALLBACK for key " << *key << " number " << I);
	
	// Read the priors
	const CondAttrListCollection* collection;
	StatusCode SC = detStore()->retrieve(collection, "/TRT/Calib/MLbetaPriors");
	if (SC.isFailure() || collection == 0)
	{
	  ATH_MSG_ERROR("A problem occurred while reading a conditions database object.");
	  return StatusCode::FAILURE;
	}
	else // Successfully retrieved
	{
	  // Ensure that the collection is the same size as the priors
	  // (Make sure we're replacing everything, not just a portion of the priors)
	  if (collection->size() != m_TrtTool->TRT_NumPriors())
	    ATH_MSG_WARNING("Unexpected number of priors retrieved from the condDB (got " << collection->size() << ", expected " << m_TrtTool->TRT_NumPriors() << ").  Using defaults.");
	  else
	  {
	    // If debugging, open the output file and print the header
	    if (doDebug)
	      dFile = fopen(DEBUGFILE_C,"w");
	    if (dFile != NULL)
	      fprintf(dFile,"#####\n##### prior[etaIndex][barrelOrEndcap][radiusIndex]\n#####\n\n");
	    
	    int channel;
	    char name[25];
	    double* bitValues;
	    
	    // Loop over the changes
	    for (CondAttrListCollection::const_iterator iter = collection->begin(); iter != collection->end(); ++iter)
	    {
	      channel = iter->first;
	      
	      // channel will be in one of two forms
	      // if channel >= 100000:
	      //	channel is in the form 1AABCC (each one is a single base-10 digit)
	      //	AA = eta bin index (0 to 99)		["i" in TRT_LoadPriors]
	      //	B = straw type, barrel(0) or endcap(1)	["k" in TRT_LoadPriors]
	      //	CC = radius bin index (0 to 99)		["l" in TRT_LoadPriors]
	      //  else if 0 <= channel < 100000:
	      //	channel is in the form ABC (each one is a single base-10 digit)
	      //	A = eta bin index (0 to 9)		["i" in TRT_LoadPriors]
	      //	B = straw type, barrel(0) or endcap(1)	["k" in TRT_LoadPriors]
	      //	C = radius bin index (0 to 9)		["l" in TRT_LoadPriors]
/*

	      if (channel >= 100000)
	      {
	        channel -= 100000;
	        etaIndex = channel/1000;
	        barrelOrEndcap = (channel%1000)/100;
	        radiusIndex = channel%100;
	      }
	      else
*/
	      if (channel >= 1000)
	      {
		ATH_MSG_FATAL("Using an old verion of the channel (> 100000).");
	  	return StatusCode::FAILURE;
	      }




	      int etaIndex		=  channel/100;
	      int barrelOrEndcap	= (channel%100)/10;
	      int radiusIndex		=  channel%10;



	      if (channel >= 100000)
	      {
		ATH_MSG_FATAL("Using an old verion of the channel (> 100000).");
	  	return StatusCode::FAILURE;
	      }

	      if (    ( (etaIndex       >= 0) && (etaIndex       <= TrtToolBetaLiklihood::NETABINS) )
	           && ( (barrelOrEndcap == 0) || (barrelOrEndcap == 1) )
	           && ( (radiusIndex    >= 0) && (radiusIndex    <= TrtToolBetaLiklihood::NRFEBINS) )
		)
	      {
	        const coral::AttributeList &list = iter->second;
	        bitValues = (double*)malloc(sizeof(double)*24);
	        
	        for (int i = 0; i < 24; i++)
	        {
	          sprintf(name,"TRT_bit_%d",i);
	          bitValues[i] = list[name].data<double>();
	          // If debug, print to file
	          if (dFile != NULL)
	            fprintf(dFile,"prior[%d][%d][%d][TRT_bit_%d] = %10f\n",etaIndex,barrelOrEndcap,radiusIndex,i,list[name].data<double>());
	        }
	        // If debug, print a new line to file
	        if (dFile != NULL)
	          fprintf(dFile,"\n");
	        
	        m_TrtTool->TRT_UpdatePriorValues(radiusIndex,etaIndex,barrelOrEndcap,bitValues);
	        free(bitValues);
	      }
	      else if (dFile != NULL) // If debug, print a warning line to file
	        fprintf(dFile,"Unexpected result!  Got channel of %d (eta=%d,BoE=%d,rad=%d)\n\n",channel,etaIndex,barrelOrEndcap,radiusIndex);
	      
	      // If debug, close the file, as we're done now
	      if (dFile != NULL)
	        fclose(dFile);
	    }
	  }
	}
	
	return StatusCode::SUCCESS;
}

// A wrapper intermediary for the TrtToolBetaLiklihood::TRT_FEbeta function
// Meant to be called from LowBetaAlg.cxx, via InDet::LowBetaAlg::ChargedSMPindicators
// Returns a vector of results from TRT_FEbeta where:
//	vector[0] = LikelihoodBeta
//	vector[1] = LikelihoodError
std::vector<float> InDet::LowBetaAlg::callTrtToolBetaLiklihood(const Trk::Track& track) const
{
	// Return variable
	std::vector<float> LikelihoodValues;
	
	// Variables to be determined before calling TRT_FEbeta
	//Raw bitpattern
	std::vector<int> TRT_bitpattern;
	//Barrel or endcap index
	std::vector<int> TRT_bec;
	//Strawlayer index
	std::vector<int> TRT_strawlayer;
	//Layer index
	std::vector<int> TRT_layer;
	//t0 values
	std::vector<float> TRT_t0;
        //time it took for ionization to drift to the anode, given the hit's r(t) and DCradius:
        std::vector<float> TRT_estDrift;
	//Drift circle radius
	std::vector<float> TRT_R;
	//Local track radius
	std::vector<float> TRT_R_track;
	//x position of the hit
	std::vector<float> TrackX;
	//y position of the hit
	std::vector<float> TrackY;
	//z position of the hit
	std::vector<float> TrackZ;
	//pt
	float RecPt;
	//eta
	float RecEta;
        //tube hit flags
        std::vector<int> TRT_isTube;
	
	
	// Load the priors from a file instead of using the conditions db or the defaults
	//const std::string fileName = "/afs/cern.ch/user/s/sschramm/testarea/16.0.2/InnerDetector/InDetRecAlgs/InDetLowBetaFinder/InDetLowBetaFinder/TRTpriors.MC.root";
	//m_TrtTool->TRT_LoadPriors(fileName);
	
	
	// Gather/parse all of the neccessary data
        //std::cout << "call parse data fxn:" << std::endl;
	StatusCode SC = parseDataForTrtToolBetaLiklihood(track,&TRT_bitpattern,&TRT_bec,&TRT_strawlayer,&TRT_layer,
				&TRT_t0,&TRT_estDrift,&TRT_R,&TRT_R_track,&TrackX,&TrackY,&TrackZ,&RecPt,&RecEta,&TRT_isTube);
	if (SC.isFailure())
	{
	  LikelihoodValues.clear();
	  LikelihoodValues.push_back(-997);
	  LikelihoodValues.push_back(-997);
	  return LikelihoodValues;
	}
	
	// We now have everything we need
	
	
/*
	// Create the debug file
	printTrtToolBetaLiklihoodDebugFile(TRT_bitpattern,TRT_bec,TRT_strawlayer,TRT_layer,TRT_t0,TRT_R,
				TRT_R_track,TrackX,TrackY,TrackZ,RecPt,RecEta);
*/
	// Now call the actual function we want
        //std::cout << "call FEbeta fxn:" << std::endl;
	LikelihoodValues = m_TrtTool->TRT_FEbeta(TRT_bitpattern,TRT_bec,TRT_strawlayer,TRT_layer,TRT_t0,TRT_estDrift,
						TRT_R,TRT_R_track,TrackX,TrackY,TrackZ,RecPt,RecEta,TRT_isTube);
        //std::cout << "FEbeta = " << LikelihoodValues.at(0) << std::endl;
/*
       //------debug---------------------------------------------------
        const char* FILENAME_C = "/scratchdisk2/wmills/unslimmedDump.log";
        FILE* outFile;
        outFile = fopen(FILENAME_C,"a");
        fprintf(outFile,"fit beta = %f \n",LikelihoodValues.at(0));
        //fprintf(outFile,"no. good hits = %i \n",TRT_bec.size());
        fclose(outFile);
	//-------------------end debug-----------------------
*/
	return LikelihoodValues;
}

// Gather all of the necessary data that the TRT_FEbeta function takes as inputs
// Sets all of the input arguments
StatusCode InDet::LowBetaAlg::parseDataForTrtToolBetaLiklihood(const Trk::Track& track,
			std::vector<int>* TRT_bitpattern,std::vector<int>* TRT_bec,std::vector<int>* TRT_strawlayer,std::vector<int>* TRT_layer,
			std::vector<float>* TRT_t0,std::vector<float>* TRT_estDrift,std::vector<float>* TRT_R,std::vector<float>* TRT_R_track,
			std::vector<float>* TrackX,std::vector<float>* TrackY,std::vector<float>* TrackZ,float* RecPt,float* RecEta, std::vector<int>* TRT_isTube) const
{
	// Variable for non-refit drift circles, to be used as a check
	std::vector<float> TRT_R_notFit;
	
	// Clear all the vectors
	TRT_bitpattern->clear();
	TRT_bec->clear();
	TRT_strawlayer->clear();
	TRT_layer->clear();
	TRT_t0->clear();
        TRT_estDrift->clear();
	TRT_R->clear();
	TRT_R_track->clear();
	TrackX->clear();
	TrackY->clear();
	TrackZ->clear();
	TRT_R_notFit.clear();
        TRT_isTube->clear();
	
	
	// Get the non-refit track particle drift circle radii for checking our refits later
	const DataVector<const Trk::TrackStateOnSurface>* hits = track.trackStateOnSurfaces();
	if (hits)
	{
	  // Loop over the hits
	  DataVector<const Trk::TrackStateOnSurface>::const_iterator hitIterEnd = hits->end();
	  DataVector<const Trk::TrackStateOnSurface>::const_iterator hitIter = hits->begin();
	  for (; hitIter != hitIterEnd; ++hitIter)
	  {
	    //only include this hit if we can call a TRT drift circle out of a measurementBase object based on it
	    const Trk::MeasurementBase *measurement = (*hitIter)->measurementOnTrack();
	    if (measurement)
	    {
	      // Get drift circle (ensures that hit is from TRT)
	      const InDet::TRT_DriftCircleOnTrack *driftCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
	      if (driftCircle)
	      {
	        // Check the raw data
	        if (driftCircle->prepRawData())
	        {
	          float driftCircleRadius = -999;
	          driftCircleRadius = driftCircle->localParameters()[Trk::driftRadius];
	          TRT_R_notFit.push_back(driftCircleRadius);
	        }
	      }
	    }
	  }
	}

	//std::cout << "break point 1 " << std::endl;
	
	// Determine pt, eta, phi
	// Perigee exists if we got here (checked for in InDet::LowBetaAlg::ChargedSMPindicators)
	const Trk::TrackParameters* perigee = track.perigeeParameters();
	const AmgVector(5)& parameterVector = perigee->parameters();
	double qOverP  = parameterVector[Trk::qOverP];
	double theta   = parameterVector[Trk::theta];
	double phi0    = parameterVector[Trk::phi0];
	if (tan(theta/2.0) < 0.0001)
	{
	  ATH_MSG_DEBUG("TrtToolBetaLiklihood aborting due to theta value (tan(theta/2) < 0.0001");
	  return StatusCode::FAILURE;
	}
	double eta = -log(tan(theta/2.0));
	if (qOverP == 0.0)
	{
	  ATH_MSG_DEBUG("TrtToolBetaLiklihood aborting due to momentum value (q/p == 0)");
	  return StatusCode::FAILURE;
	}
	double pt  = fabs(1.0/qOverP)*sin(theta);
	
	// Set pt and eta
	*RecPt  = pt;
	*RecEta = eta;
	//std::cout << "break point 2 " << std::endl;

	// Get the track refit container from storegate
	// (We've already checked that it's there)
	//const TrackCollection* refitTracks(0);
	//StatusCode SC = evtStore()->retrieve(refitTracks,m_RefittedTracksContainerName);
        SG::ReadHandle<TrackCollection> unslimmedTracks;
        const Trk::Track *best_unslimmed_match=nullptr;
        float bestCone = 9999;
        if (!m_UnslimmedTracksContainerName.key().empty()) {
          unslimmedTracks=SG::ReadHandle<TrackCollection>(m_UnslimmedTracksContainerName ) ;
          if (!unslimmedTracks.isValid()) {
            ATH_MSG_WARNING("Could not retrieve UnslimmedTracks container " << m_UnslimmedTracksContainerName.key());
            return StatusCode::FAILURE;
          }
          //std::cout << "break point 3 " << std::endl;

          // need to loop over refit container to find the right refit track, NOT 1:1 with track particles!
          // is this still true for the unslimmed container?
          //std::cout << "matching loop, eta = " << eta << ", phi0 = " << phi0 << std::endl;
          //float refitEta = -999;
          //float refitPhi = -999;
          float etaphiCone = 0;
          //int matchIndex = 0;
          //int hereIndex = 0;
          for(const Trk::Track *TR : *unslimmedTracks) {
            const Trk::TrackParameters *aMeasPer = TR->perigeeParameters();
            //etaphiCone = sqrt(pow( -log(tan(aMeasPer->parameters()[Trk::theta]/2))  - (*itr)->eta(), 2) + pow( aMeasPer->parameters()[Trk::phi0] - (*itr)->phi(),2));
            etaphiCone = sqrt(pow( -log(tan(aMeasPer->parameters()[Trk::theta]/2))  - eta, 2) + pow( aMeasPer->parameters()[Trk::phi0] - phi0,2));
            if( etaphiCone < bestCone ){
              bestCone = etaphiCone;
              //refitEta = -log(tan(aMeasPer->parameters()[Trk::theta]/2));
              //refitPhi = aMeasPer->parameters()[Trk::phi0];
              best_unslimmed_match = TR;
              //matchIndex = hereIndex;
            }
            //hereIndex++;
          }
          if (!best_unslimmed_match) {
            ATH_MSG_WARNING("No matching unslimmed track.");
            return StatusCode::FAILURE;
          }
        }
        else {
          best_unslimmed_match=&track;
        }
        //std::cout << "matching complete, bestCone = " << bestCone << ", matchIndex = " << matchIndex << std::endl;
	
	//std::cout << "break point 4 " << std::endl;
	// Check for refit hits (aka track states on surfaces, tsos)
	const DataVector<const Trk::TrackStateOnSurface>* refitHits = dynamic_cast<const DataVector<const Trk::TrackStateOnSurface>*>(best_unslimmed_match->trackStateOnSurfaces());
	if (refitHits)
	{
          for (const Trk::TrackStateOnSurface *hit : *refitHits )
	  {
	    //only include this hit if we can call a TRT drift circle out of a measurementBase object based on it
	    const Trk::MeasurementBase *measurement = hit->measurementOnTrack();
	    if (measurement)
	    {
	      // Get drift circle (ensures that hit is from TRT):
	      const InDet::TRT_DriftCircleOnTrack *driftCircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
	      if (driftCircle)
	      {
	        // Check the raw data
	        if (driftCircle->prepRawData())
	        {
	          // Raw bit pattern
	          TRT_bitpattern->push_back(driftCircle->prepRawData()->getWord());
	          
	          // TRT ID information
	          Identifier DCoTId = driftCircle->identify();
	          int bec = m_trtId->barrel_ec(DCoTId);
	          int strawLayer = m_trtId->straw_layer(DCoTId);
	          int layer = m_trtId->layer_or_wheel(DCoTId);
	          TRT_bec->push_back(bec);
	          TRT_strawlayer->push_back(strawLayer);
	          TRT_layer->push_back(layer);
		  //std::cout << "break point 5 " << std::endl;
	          // Get TRT calibration from the database, t0
	          Identifier TRTlocal = m_trtId->straw_id(bec,m_trtId->phi_module(DCoTId),layer,strawLayer,m_trtId->straw(DCoTId));
	          double t0 = m_trtconddbTool->getT0(TRTlocal);
	          TRT_t0->push_back(t0);

                  //is this a tube hit?  Not filling the dc rad with 0 for much longer, need to check directly:
                  float isTRTTube = 0;
                  double error2=measurement->localCovariance()(0,0);
                  if(error2>1) isTRTTube = 1;
                  TRT_isTube->push_back(isTRTTube);

	          // Get drift cirlce radius
	          float driftCircleRadius = -999;
	          driftCircleRadius = driftCircle->localParameters()[Trk::driftRadius];
	          TRT_R->push_back(driftCircleRadius);
		  //std::cout << "break point 6 " << std::endl;
                  // Get TRT r(t) from the db:
                  const TRTCond::BasicRtRelation* rtr = dynamic_cast<const TRTCond::BasicRtRelation*>(m_trtconddbTool->getRtRelation(TRTlocal)) ;
                  const float *pcal;
                  const float defaultpcal[] = {-0.179736,0.0366578,0.000699622,-0.000011811} ;
                  pcal = rtr ? rtr->cal() : defaultpcal ;

		  //std::cout << "break point 7.0" << std::endl;
		  double estDrift = -999;
                  //solve 3rd order poly for t using DC R (thank you http://www.1728.com/cubic2.htm :)
                  double a = pcal[3]; double b = pcal[2]; double c = pcal[1]; double d = pcal[0] - fabs(driftCircleRadius);
                  //std::cout << "r(t) = " << pcal[3] << "t^3 + " << pcal[2] << "t^2 + " << pcal[1] << "t + " << pcal[0] << "; t0 = " << t0 << std::endl;
		  //std::cout << "break point 7.1" << std::endl;
		  double f  =0.0;
		  double g  = 0.0;
		  //if (a!= 0.0){
		     f = (3.*c/a - b*b/a/a)/3.;
		     g = (2.*b*b*b/a/a/a - 9.*b*c/a/a + 27.*d/a)/27.;
		     //std::cout << "break point 7.2" << std::endl;
		     double h = g*g/4. + f*f*f/27.;
		     //std::cout << "break point 7.3" << std::endl;
		     double t1r, t2r, t3r;
		     //double t1r, t2r, t3r, t2i, t3i;
		     if(h<=0){
		       double I=0.0;
		       I = sqrt(g*g/4. - h);
		       double J = pow(I,1./3.);
		       double K =0.0;
		       if (fabs(-1.*g/2./I) < 1.0)K = acos(-1.*g/2./I);
		       
		       double L = -1.*J;
		       double M = cos(K/3.);
		       double N = sqrt(3)*sin(K/3.);
		       double P = -1.*b/3./a;
		       t1r = 2.*J*cos(K/3.) - b/3./a;
		       t2r = L*(M+N)+P;
		       t3r = L*(M-N)+P;
		       //std::cout << "break point 7.4a" << std::endl;
                       //std::cout << "h = " << h << "; I = " << I << "; J = " << J << "; K = " << K << "; L = " << L << "; M = " << M
                       //          << "; N = " << N << "; P = " << P << "; t1r = " << t1r << "; t2r = " << t2r << "; t3r = " << t3r << std::endl;
		     }
		     else{
		       double R = g/(-2.) + sqrt(h);
		       double S = pow(fabs(R),1./3.);
		       if(R<0) S = -1.*S;
		       double T = g/(-2.) - sqrt(h);
		       double U = pow(fabs(T),1./3.);
		       if(T<0) U = -1.*U;
		       t1r = S+U-b/3./a;
		       t2r = -1.*(S+U)/2. - b/3./a;  //t2i = (S-U)*sqrt(3.)/2.;
		       t3r = -1.*(S+U)/2. - b/3./a;  //t3i = -1.*(S-U)*sqrt(3.)/2.;
		       //std::cout << "break point 7.4b" << std::endl;
		     }
		  
		  //std::cout << "break point 8 " << std::endl;
                  int choose = 0; float smallest = 1000;
                  if(t1r>0 && t1r<smallest) { smallest = t1r; choose = 1; }
                  if(t2r>0 && t2r<smallest) { smallest = t2r; choose = 2; }
                  if(t3r>0 && t3r<smallest) { smallest = t3r; choose = 3; }
                  if(choose == 1) estDrift = t1r;
                  else if(choose == 2) estDrift = t2r;
                  else if(choose == 3) estDrift = t3r;
		  //}
                  TRT_estDrift->push_back(estDrift);
	          
	          // Get local track radius (track-anode distance)
	          const Trk::TrackParameters* hitParam=(hit->trackParameters());
	          float localTrackRadius = -999;
	          if(hitParam)
	            localTrackRadius = hitParam->parameters()[Trk::driftRadius];
	          TRT_R_track->push_back(localTrackRadius);
	          
	          
	          // Get the x, y, and z positions of the hit
	          TrackX->push_back(driftCircle->globalPosition().x());
	          TrackY->push_back(driftCircle->globalPosition().y());
	          TrackZ->push_back(driftCircle->globalPosition().z());
		
		//std::cout << "break point 9 " << std::endl;
		}
	      }
	    }
	  }  //std::cout << "end hit loop" << std::endl;
	}

        // unused
	// // Compare the non-refit and refit drift circle radii
	// if (TRT_R_notFit.size() != TRT_R->size())
	//   bestCone = 9999;
	// else
	//   for(unsigned int i = 0; i < TRT_R->size(); i++)
	//     if ((*TRT_R)[i] != TRT_R_notFit[i])
	//       bestCone = 9998;
	
	
	return StatusCode::SUCCESS;
}






















// Prints the input of TRT_FEbeta() to file (hard-set)
// This is a temporary debug method, and may be removed later
int printTrtToolBetaLiklihoodDebugFile ATLAS_NOT_THREAD_SAFE // This method is not thread safe because static variable is used.
(std::vector<int> TRT_bitpattern, std::vector<int> TRT_bec, std::vector<int> TRT_strawlayer, std::vector<int> TRT_layer,
 std::vector<float> TRT_t0, std::vector<float> TRT_R, std::vector<float> TRT_R_track,
 std::vector<float> TrackX, std::vector<float> TrackY, std::vector<float> TrackZ, float RecPt, float RecEta)
{
    static int trackNum = 0; // static variable is not thread safe.
	const char* FILENAME_C = "/afs/cern.ch/user/s/sschramm/testarea/16.0.2/InnerDetector/InDetRecAlgs/InDetLowBetaFinder/debugFile.log";
	FILE* outFile;
	unsigned int i;
	
	if (trackNum == 0)
	  outFile = fopen(FILENAME_C,"w");
	else
	  outFile = fopen(FILENAME_C,"a");
	
	if (outFile == NULL)
	  return -1;
	
	fprintf(outFile,"#\n#Track Number %d\n#RecPt = %f, RecEta = %f\n#\n",trackNum++,RecPt,RecEta);
	
	fprintf(outFile,"%%Start TRT_bitpattern:\n");
	for (i = 0; i < TRT_bitpattern.size(); i++)
	  fprintf(outFile,"\t%d\n",TRT_bitpattern[i]);
	fprintf(outFile,"%%End TRT_bitpattern:\n");
	
	fprintf(outFile,"%%Start TRT_bec:\n");
	for (i = 0; i < TRT_bec.size(); i++)
	  fprintf(outFile,"\t%d\n",TRT_bec[i]);
	fprintf(outFile,"%%End TRT_bec:\n");
	
	fprintf(outFile,"%%Start TRT_strawlayer:\n");
	for (i = 0; i < TRT_strawlayer.size(); i++)
	  fprintf(outFile,"\t%d\n",TRT_strawlayer[i]);
	fprintf(outFile,"%%End TRT_strawlayer:\n");
	
	fprintf(outFile,"%%Start TRT_layer:\n");
	for (i = 0; i < TRT_layer.size(); i++)
	  fprintf(outFile,"\t%d\n",TRT_layer[i]);
	fprintf(outFile,"%%End TRT_layer:\n");
	
	fprintf(outFile,"%%Start TRT_t0:\n");
	for (i = 0; i < TRT_t0.size(); i++)
	  fprintf(outFile,"\t%f\n",TRT_t0[i]);
	fprintf(outFile,"%%End TRT_t0:\n");
	
	fprintf(outFile,"%%Start TRT_R:\n");
	for (i = 0; i < TRT_R.size(); i++)
	  fprintf(outFile,"\t%f\n",TRT_R[i]);
	fprintf(outFile,"%%End TRT_R:\n");
	
	fprintf(outFile,"%%Start TRT_R_track:\n");
	for (i = 0; i < TRT_R_track.size(); i++)
	  fprintf(outFile,"\t%f\n",TRT_R_track[i]);
	fprintf(outFile,"%%End TRT_R_track:\n");
	
	fprintf(outFile,"%%Start TrackX:\n");
	for (i = 0; i < TrackX.size(); i++)
	  fprintf(outFile,"\t%f\n",TrackX[i]);
	fprintf(outFile,"%%End TrackX:\n");
	
	fprintf(outFile,"%%Start TrackY:\n");
	for (i = 0; i < TrackY.size(); i++)
	  fprintf(outFile,"\t%f\n",TrackY[i]);
	fprintf(outFile,"%%End TrackY:\n");
	
	fprintf(outFile,"%%Start TrackZ:\n");
	for (i = 0; i < TrackZ.size(); i++)
	  fprintf(outFile,"\t%f\n",TrackZ[i]);
	fprintf(outFile,"%%End TrackZ:\n");
	
	fclose(outFile);
	
	return 0;
}


