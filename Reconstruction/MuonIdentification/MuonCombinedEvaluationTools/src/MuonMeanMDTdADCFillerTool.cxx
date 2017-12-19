///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonMeanMDTdADCFillerTool.cxx, Implementation file for class MuonMeanMDTdADCFillerTool
/////////////////////////////////////////////////////////////////// 

// MuonCombinedEvaluationTools includes
#include "MuonMeanMDTdADCFillerTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>

using CLHEP::GeV;

namespace Rec
{
    
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

MuonMeanMDTdADCFillerTool::MuonMeanMDTdADCFillerTool
(const std::string&      type,
 const std::string&      name, 
 const IInterface*       parent)
  : AthAlgTool              (type, name, parent),
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_idToFixedIdTool("MuonCalib::IdToFixedIdTool")

{
  declareInterface<IMuonMeanMDTdADCFiller>(this);
 // declareProperty("doMdtGasGainCorrectionForMc", m_doMdtGasGainCorrectionForMc = false);
}

MuonMeanMDTdADCFillerTool::~MuonMeanMDTdADCFillerTool (void) 
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode MuonMeanMDTdADCFillerTool::initialize()
{
  ATH_MSG_INFO( "Initializing MuonMeanMDTdADCFillerTool - package version " << PACKAGE_VERSION );

  return StatusCode::SUCCESS;
}

StatusCode MuonMeanMDTdADCFillerTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

double
MuonMeanMDTdADCFillerTool::meanMDTdADCFiller (const xAOD::Muon& muon) const
{
  // exclude statistical combination
  if( muon.author() == xAOD::Muon::STACO ) return -9999.;

	// Trk::Track* for trackParticle 
	const Trk::Track* theTrack = muon.trackParticle(xAOD::Muon::CombinedTrackParticle) ? 
	muon.trackParticle(xAOD::Muon::CombinedTrackParticle)->track() : 0;
   
	if(theTrack)
	{
		return meanMDTdADCFiller(*theTrack);
	}
	
	const Trk::Track* theTrack1 = muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ? 
	muon.trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->track() : 0;
	
	if(theTrack1)
	{
		return meanMDTdADCFiller(*theTrack1);
	}

	const Trk::Track* theTrack2 = muon.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle) ? 
	muon.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)->track() : 0;
	
 	if(theTrack2)
	{
		return meanMDTdADCFiller(*theTrack2);
	}

	const Trk::Track* theTrack3 = muon.trackParticle(xAOD::Muon::Primary) ? 
	muon.trackParticle(xAOD::Muon::Primary)->track() : 0;
	
 	if(theTrack3)
	{
		return meanMDTdADCFiller(*theTrack3);
	}
	
    ATH_MSG_DEBUG ("No primary author original track for refitted muon, stop calculation...");
    return -9999.;

}

 
double
MuonMeanMDTdADCFillerTool::meanMDTdADCFiller (const Trk::Track& track) const
{

	bool doMdtGasGainCorrectionForMc=false;// default value for DATA
	
	// Event information
	const xAOD::EventInfo* eventInfo = 0; 
	ATH_CHECK( evtStore()->retrieve( eventInfo), -9999. );

	// check if data or MC
	if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
		doMdtGasGainCorrectionForMc=true; // set "true" for MC 
	}

    // return mean Number of ADC counts for MDT tubes on the track
	
	const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if(states == NULL) {
        ATH_MSG_INFO("Cannot get track states on surface for TrackParticle");
        return -9999.;
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

    int nhitsadc = 0;
    double absR = 0;
    double datfit = 0;
    double mcfit = 0;
    double correction = 0;
    double testEta = 0;
    double maxhit = -999.;
    float PhiFit = 0;
	
    double meandADC = 0.;
    std::vector<double> dADCvec;
    dADCvec.clear();
         double meanMDTdADC = 0.;
	
    testEta = asinh(1. / tan(track.perigeeParameters()->parameters()[Trk::theta]));	
    double track_phi = track.perigeeParameters()->parameters()[Trk::phi];	

    for(int nhits = 0; tsit != tsit_end; ++tsit, ++nhits)  {
        // outliers can have type measurement, in Muid
        if(!(*tsit)->type(Trk::TrackStateOnSurface::Measurement) ||(*tsit)->type(Trk::TrackStateOnSurface::Outlier)) {
            continue;
        }
        //    if((*tsit)->type() == 3) continue; // scattering center..add count etc
        //    if((*tsit)->type() == 4) continue; // outlier..add count etc

        const Trk::MeasurementBase* measurement = (*tsit)->measurementOnTrack();
        if(measurement == NULL) {
            continue;
        }
        Identifier id = m_helperTool->getIdentifier(*measurement);
        if(!(m_idHelperTool->isMuon(id))) {
            continue;    // MS summary variables - don't need other technologies
        }
        if(!id.is_valid()) {
            //      std::cout<<"ID not valid\n";
            continue;
        }
        MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId(id);
        // mdt station counts
        // int statnam = fid.stationName();
        if(fid.is_mdt()) {
            const Muon::MdtDriftCircleOnTrack *mdtcirc = dynamic_cast<const Muon::MdtDriftCircleOnTrack *>(measurement);
            if(mdtcirc == NULL) {
              ATH_MSG_WARNING("cannot cast Trk::MeasurementBase to Muon::MdtDriftCircleOnTrack");
              continue;
            }
            const Muon::MdtPrepData *rawdata = mdtcirc->prepRawData();
            if(rawdata) {
                // Calculate deltaADC as difference of number of ADC counts for given hit and datfit(Rdrift),
                // where datfit(Rdrift) is the result of the fit of <ADC> in the given bin of Rdrift dependence on Rdrift
                absR = fabs(mdtcirc->driftRadius());

		bool isInBme = fabs(testEta)>=0.644 && fabs(testEta)<=0.772 && track_phi>=-1.72 && track_phi<=-1.42;
		
		if ((!isInBme && (absR <= 0.4 || absR >= 14.)) || (isInBme && (absR <= 0.4 || absR >= 6.5)))
			continue;
                nhitsadc++;


                if(fabs(testEta) < 1.) 
		{
			datfit = 105.088 + 27.0638*pow(absR,1) - 4.72089*pow(absR,2) + 0.110274*pow(absR,3) + 0.041508*pow(absR,4) - 0.00403678*pow(absR,5) + 0.000111072*pow(absR,6);
			mcfit = 73.8974 + 12.0642*pow(absR,1) + 0.975372*pow(absR,2) - 0.922337*pow(absR,3) + 0.140759*pow(absR,4) - 0.00881345*pow(absR,5) + 0.000202078*pow(absR,6);
                } 
		else 
		{
			datfit = 106.329 + 26.5296*pow(absR,1) - 4.07423*pow(absR,2) - 0.0594686*pow(absR,3) + 0.0608916*pow(absR,4) - 0.00506114*pow(absR,5) + 0.000131493*pow(absR,6);
			mcfit = 73.3596 + 12.9939*pow(absR,1) + 0.494472*pow(absR,2) - 0.812082*pow(absR,3) + 0.128345*pow(absR,4) - 0.00814268*pow(absR,5) + 0.000188203*pow(absR,6);
                }



                if(doMdtGasGainCorrectionForMc) 
		{
			correction=datfit/mcfit; //gas gain correction factor for MC
			meandADC += (correction*rawdata->adc() - datfit);
			dADCvec.push_back(correction*rawdata->adc() - datfit);

			maxhit = ((correction*rawdata->adc() - datfit) >= maxhit) ? (correction*rawdata->adc() - datfit) : maxhit;
                } 
		else 
		{
			meandADC += (rawdata->adc() - datfit);
			dADCvec.push_back(rawdata->adc() - datfit);

			maxhit = ((rawdata->adc() - datfit) >= maxhit) ? (rawdata->adc() - datfit) : maxhit;
                }
            }

        }
    } // end loop over hits
	
	
	if(nhitsadc == 0) 
	{
		meandADC = -9999;
    	}


    	if (nhitsadc == 1) 
	{
	        meandADC = meandADC / double(nhitsadc);
	}
	if (nhitsadc >= 2)
	{
		meandADC = double(meandADC - maxhit) / double(nhitsadc - 1);
	}

	

	if (doMdtGasGainCorrectionForMc)
	{
		if (track_phi > -3.2 && track_phi <= -2.87)
			PhiFit = -13.5471*pow(track_phi,2) - 39.0001*track_phi + 4.23613;
		if (track_phi > -2.87 && track_phi <= -2.13)
			PhiFit = 53.209*pow(track_phi,2) + 272.502*track_phi + 342.867;
		if (track_phi > -2.13 && track_phi <= -1.37)
			PhiFit = 72.4707*pow(track_phi,2) + 254.911*track_phi + 216.811;
		if (track_phi > -1.37 && track_phi <= -0.57)
			PhiFit = 77.218*pow(track_phi,2) + 149.676*track_phi + 65.371;
		if (track_phi > -0.57 && track_phi < 0.21)
			PhiFit = 86.977*pow(track_phi,2) + 29.0558*track_phi - 6.09313;
		if (track_phi >= 0.21 && track_phi <= 1.00)
			PhiFit = 79.203*pow(track_phi,2) - 92.9512*track_phi + 21.6361;
		if (track_phi > 1.00 && track_phi <= 1.79)
			PhiFit = 85.5711*pow(track_phi,2) - 239.068*track_phi + 161.918;
		if (track_phi > 1.79 && track_phi <= 2.60)
			PhiFit = 82.8996*pow(track_phi,2) - 362.665*track_phi + 391.419;
		if (track_phi > 2.60 && track_phi < 3.20)
			PhiFit = 73.8744*pow(track_phi,2) - 443.274*track_phi + 656.926;
 
		meandADC += PhiFit - 5.01 + 9.46;
	}
	
	dADCvec.clear();

	meanMDTdADC = meandADC; 
	    
    return meanMDTdADC;
} 
} //end namespace Rec

