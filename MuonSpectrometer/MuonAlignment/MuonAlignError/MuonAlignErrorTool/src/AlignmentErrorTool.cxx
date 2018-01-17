/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
////  Authors: Pierre-Francois Giraud, Camilla Maiani
////  Contacts: pierre-francois.giraud@cern.ch, cmaiani@cern.ch
////////////////////////////////////////////////////////////////////

#include "MuonAlignErrorTool/AlignmentErrorTool.h"
#include "MuonAlignErrorBase/AlignmentTranslationDeviation.h"
#include "MuonAlignErrorBase/AlignmentRotationDeviation.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "PathResolver/PathResolver.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

// For the call-back
#include "MuonCondInterface/IMuonAlignmentErrorDbTool.h"
#include "StoreGate/StoreGateSvc.h"

#include <fstream>
#include <sstream>

using namespace MuonAlign;

AlignmentErrorTool::AlignmentErrorTool(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_idTool("MuonCalib::IdToFixedIdTool"),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_pMuonAlignmentErrorDbSvc("MuonAlignmentErrorDbSvc", "MuonAlignmentErrorDbSvc")
{
  declareInterface<Trk::ITrkAlignmentDeviationTool>(this);
  declareProperty("idTool", m_idTool);
  declareProperty("IdHelper", m_idHelper );
  declareProperty("read_local_file", m_read_local_file=false);
  declareProperty("local_input_filename", m_local_input_filename="");
}

AlignmentErrorTool::~AlignmentErrorTool() {
}

//int AlignmentErrorTool::deviationSummary_t::i_instance = 0;


AlignmentErrorTool::deviationSummary_t::deviationSummary_t()
: traslation(0.), rotation(0.), stationName(""), sumP(Amg::Vector3D(0., 0., 0.)), sumU(Amg::Vector3D(0., 0., 0.)), sumV(Amg::Vector3D(0., 0., 0.)), sumW2(0.) { 
	//i_instance++;
} //
AlignmentErrorTool::deviationSummary_t::~deviationSummary_t() {
	//i_instance--;
} //

StatusCode AlignmentErrorTool::initialize() {

  ATH_MSG_INFO("*****************************************");
  ATH_MSG_INFO("AlignmentErrorTool::initialize()");

  ATH_CHECK( m_idTool.retrieve() );
  ATH_CHECK( m_idHelper.retrieve() );

  // MAP DEVIATION INITIALIZATION
  // from local file
  if ( m_read_local_file ) {
     // THESE METHODS SHOULD BE CALLED AT EACH CALL-BACK
     std::string full_input_filename = PathResolver::find_file( m_local_input_filename, "DATAPATH");
     std::ifstream indata(full_input_filename.c_str());
     initializeAlignmentDeviationsList( indata );
     ATH_MSG_INFO("filename " << m_local_input_filename );
     ATH_MSG_INFO("*****************************************");     
     //ATH_MSG_DEBUG("###########################################");
     //ATH_MSG_DEBUG("List of deviations updated");
     //ATH_MSG_DEBUG(data.str());
     //ATH_MSG_DEBUG("###########################################");
  } else {
  // from DB

    ATH_CHECK( m_pMuonAlignmentErrorDbSvc.retrieve() );

    // FIRST INITIALIZATION TO ENFORCE TOOL CORRECTLY CONFIGURED
    int I=0;
    std::list<std::string> keys;
    if (update(I,keys).isFailure()) {
      return StatusCode::FAILURE;
    }

    // INITIALIZE TOOL TO EXTRACT INFO FROM DB
    if ( detStore()->regFcn(&IMuonAlignmentErrorDbSvc::initInfo,
        &*m_pMuonAlignmentErrorDbSvc,
        &AlignmentErrorTool::update, this) != StatusCode::SUCCESS ) {
    
        ATH_MSG_INFO("Call-back to DB returned status failure, loading a new list of deviations from the DB");
    
    } else
        ATH_MSG_INFO("Call-back to DB returned status success");

  }
    
  return StatusCode::SUCCESS;

}



StatusCode AlignmentErrorTool::finalize() {

  ATH_MSG_INFO("AlignmentErrorTool::finalize()");

  AlignmentErrorTool::deleteAlignmentDeviationsList ();

  //ATH_MSG_DEBUG("Currently we have " << AlignmentErrorTool::deviationSummary_t::i_instance << " deviation vectors");

  return StatusCode::SUCCESS;
}

void AlignmentErrorTool::makeAlignmentDeviations (const Trk::Track& track, std::vector<Trk::AlignmentDeviation*>& deviations) const {

  ATH_MSG_DEBUG("AlignmentErrorTool::makeAlignmentDeviations()");

  // CLEAR HITS VECTOR //
  for ( unsigned int i = 0; i<m_deviationsVec.size(); i++) {
     m_deviationsVec[i]->hits.clear();
     Amg::Vector3D nullvec(0., 0., 0.);
     m_deviationsVec[i]->sumP = nullvec;
     m_deviationsVec[i]->sumU = nullvec;
     m_deviationsVec[i]->sumV = nullvec;
     m_deviationsVec[i]->sumW2 = 0.;
  }


  typedef DataVector< const Trk::TrackStateOnSurface > tsosc_t;
  const tsosc_t* tsosc = track.trackStateOnSurfaces();

  // LOOP ON HITS ON TRACK //
  unsigned int nPrecisionHits = 0;
  for (tsosc_t::const_iterator it=tsosc->begin(), end=tsosc->end(); it!=end; it++) {

    const Trk::TrackStateOnSurface* tsos = *it;

    if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {

      const Trk::MeasurementBase* meas = tsos->measurementOnTrack();
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (meas);

      if (!rot) {
	const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (meas);
	if (crot) {
	  unsigned int index = crot->indexOfMaxAssignProb();
	  rot = &(crot->rioOnTrack(index));
	}
      }
      if (!rot) continue;

      ::Identifier channelId = rot->identify();
      MuonCalib::MuonFixedId calibId = m_idTool->idToFixedId(channelId);
      if (!calibId.isValid()) {
	continue;
      }

      // GATHERING INFORMATION TO PUT TOGETHER THE STATION NAME //
      std::string completename = hardwareName(calibId);
      std::stringstream multilayer_stream;
      multilayer_stream << calibId.mdtMultilayer();
      std::string multilayer_sstring = multilayer_stream.str();

      if ( calibId.is_mdt() || ( calibId.is_csc() && calibId.cscMeasuresPhi() == 0 ) ) { 

        ATH_MSG_DEBUG("Hit is in station " << completename << " multilayer " << multilayer_sstring);
	++nPrecisionHits;
 
        // FOR CROSS-CHECK
        bool is_matched = false;

        // LOOP ON STATION DEVIATIONS EXTRACTED FROM INPUT FILE //
        for(unsigned int iDev=0; iDev < m_deviationsVec.size(); iDev++) {

           // find a match to the reg exp //
	   boost::regex tmp_stationName = m_deviationsVec[iDev]->stationName;

           if (  boost::regex_match(completename, tmp_stationName) ) {

              if( !boost::regex_match(multilayer_sstring, m_deviationsVec[iDev]->multilayer) && !calibId.is_csc() ) {
                 //ATH_MSG_DEBUG("Hit in multilayer " << multilayer_sstring << " couldn't match to " << (m_deviationsVec[iDev]->multilayer).str());
                 continue;
              }

              // ASSOCIATE EACH NUISANCE TO A LIST OF HITS
              m_deviationsVec[iDev]->hits.push_back(rot);
              
              // COMPUTE RELEVANT NUMBERS
              const Trk::PrepRawData* prd = rot->prepRawData();
              const Trk::Surface& sur = prd->detectorElement()->surface(prd->identify());
              double w2 = sqrt(rot->localCovariance()(Trk::loc1,Trk::loc1));
              w2 = 1./(w2*w2);
              m_deviationsVec[iDev]->sumW2 += w2;
              m_deviationsVec[iDev]->sumP += w2 * tsos->trackParameters()->position();
              m_deviationsVec[iDev]->sumU += w2 * tsos->trackParameters()->momentum().unit();
              
              // CHECK 1 //
              Amg::Vector3D zATLAS(0., 0., 1.);
              Amg::Vector3D v1 = (tsos->trackParameters()->position()).cross(zATLAS);
              v1 /= v1.mag();
              Amg::Vector3D v2 = sur.transform().rotation().col(2)/(sur.transform().rotation().col(2)).mag();
              double sign = (v1.dot(v2) > 0.) ? 1. : -1.;

              // ARTIFICIALLY ORIENTATE EVERYTHING TOWARDS THE SAME DIRECTION
              m_deviationsVec[iDev]->sumV += sign * w2 * sur.transform().rotation().col(2);

              // FOR CROSS-CHECK
              is_matched = true;
          
           } // match the station name to the deviation regexp

        } // LOOP ON DEVIATIONS

        if (is_matched == false)
           ATH_MSG_WARNING("The hits in the station " << completename << ", multilayer " << multilayer_sstring << " couldn't be matched to any deviation regexp in the list.");

     } //CHECK IF IT'S MDT OR CSC HIT

    } // CHECK TSOS OF TYPE 'MEASUREMENT'

  } //LOOP ON TSOS

  // Nuisance parameters covering the complete track are not wanted. (MS/ID
  // error treated differently for now). Removing the deviations covering the
  // full track in further processing.
  for (deviationSummary_t* dev : m_deviationsVec) {
    if (dev->hits.size() == nPrecisionHits)
      dev->hits.clear();
  }

  // CHECK HIT LISTS OVERLAP
  // FIRST CREATE AN INDEPENDENT COPY OF THE STRUCT VECTOR
  std::vector<deviationSummary_t*> new_deviationsVec;
  for(unsigned int iDev=0; iDev < m_deviationsVec.size(); iDev++) {
     deviationSummary_t* tmp = new deviationSummary_t();
     (*tmp) = (*m_deviationsVec[iDev]);
     new_deviationsVec.push_back(tmp);
  }
  
  for(unsigned int iDev=0; iDev < m_deviationsVec.size(); iDev++) {

     if ( new_deviationsVec[iDev] == NULL )
        continue;

     std::vector<const Trk::RIO_OnTrack*> v1 = m_deviationsVec[iDev]->hits;
     if ( v1.empty() )
        continue;

     std::stable_sort(v1.begin(), v1.end());

     for(unsigned int jDev=iDev+1; jDev < m_deviationsVec.size(); jDev++) {

        if ( new_deviationsVec[jDev] == NULL )
           continue;

        bool match = false;

        if ( m_deviationsVec[iDev]->hits.size() != m_deviationsVec[jDev]->hits.size() )
           continue;

        std::vector<const Trk::RIO_OnTrack*> v2 = m_deviationsVec[jDev]->hits;
        std::stable_sort(v2.begin(), v2.end());

        match = (v1 == v2); 

        if ( match ) {

           ATH_MSG_DEBUG("Found deviations " << iDev << " and " << jDev << " related to the same list of hits. Merging...");
           ATH_MSG_DEBUG("old (traslation, rotation) systematic uncertainties for " << iDev << ": " << new_deviationsVec[iDev]->traslation << ", " << new_deviationsVec[iDev]->rotation);
           ATH_MSG_DEBUG("old (traslation, rotation) systematic uncertainties for " << jDev << ": " << m_deviationsVec[jDev]->traslation << ", " << m_deviationsVec[jDev]->rotation);

           // MERGE THE TWO DEVIATIONS ASSOCIATED TO THE SAME LIST OF HITS //
           double new_traslation = sqrt(new_deviationsVec[iDev]->traslation*new_deviationsVec[iDev]->traslation + m_deviationsVec[jDev]->traslation*m_deviationsVec[jDev]->traslation);
           double new_rotation = sqrt(new_deviationsVec[iDev]->rotation*new_deviationsVec[iDev]->rotation + m_deviationsVec[jDev]->rotation*m_deviationsVec[jDev]->rotation);

           // NOW PREPARE TO ERASE ONE OF THE TWO COPIES //
	   delete new_deviationsVec[jDev];
           new_deviationsVec[jDev] = NULL;

           // ASSIGN NEW TRASLATION/ROTATION TO THE REMAINING COPY //
           new_deviationsVec[iDev]->traslation = new_traslation;
           new_deviationsVec[iDev]->rotation = new_rotation;
           ATH_MSG_DEBUG("New combined (traslation, rotation) systematic uncertainties: " << new_traslation << ", " << new_rotation);

        } // FIND AN OVERLAP IN THE HITS LISTS
     } // SECOND LOOP ON DEVIATIONS
  } // FIRST LOOP ON DEVIATIONS

  // NOW BUILD THE DEVIATIONS
  deviations.clear();
  ATH_MSG_DEBUG("************************************");
  ATH_MSG_DEBUG("FINAL LIST OF DEVIATIONS");
  for(unsigned int iDev=0; iDev < new_deviationsVec.size(); iDev++) {

     // THIS HAPPENS IF A MERGING HAD BEEN DONE
     if ( new_deviationsVec[iDev] == NULL ) {
        continue;
     }

     // SKIP IF NO HITS ARE ASSOCIATED TO THIS DEVIATION
     if ( new_deviationsVec[iDev]->hits.size() == 0) {
        //ATH_MSG_DEBUG("No hits found associated to the rule " << new_deviationsVec[iDev]->stationName.str() << ", skip");
        continue;
     }

     double rotation = new_deviationsVec[iDev]->rotation;
     double traslation = new_deviationsVec[iDev]->traslation;

     Amg::Vector3D sumP = new_deviationsVec[iDev]->sumP;
     Amg::Vector3D sumU = new_deviationsVec[iDev]->sumU;
     Amg::Vector3D sumV = new_deviationsVec[iDev]->sumV;
     double sumW2       = new_deviationsVec[iDev]->sumW2;

     sumP *= (1./sumW2);
     sumU *= (1./sumW2);
     sumV *= (1./sumW2);

     if ( traslation >= 0.001*Gaudi::Units::mm ) { 

        deviations.push_back(new AlignmentTranslationDeviation(sumU.cross(sumV), traslation*Gaudi::Units::mm, new_deviationsVec[iDev]->hits));

        ATH_MSG_DEBUG("A translation along (" << sumU.x() << ", " << sumU.y() << ", " << sumU.z() << ") with sigma=" << traslation*Gaudi::Units::mm << " mm was applied to " << new_deviationsVec[iDev]->hits.size() << " hits matching the station: " << new_deviationsVec[iDev]->stationName.str() << " and the multilayer " << new_deviationsVec[iDev]->multilayer.str());

     } if ( rotation >= 0.000001*Gaudi::Units::rad ) {

        deviations.push_back(new AlignmentRotationDeviation(sumP, sumV, rotation*Gaudi::Units::rad, new_deviationsVec[iDev]->hits));

        ATH_MSG_DEBUG("A rotation around the center = (" << sumP.x() << ", " << sumP.y() << ", " << sumP.z() << ") and axis = (" << sumV.x() << ", " << sumV.y() << ", " << sumV.z() << ") with sigma=" << rotation/Gaudi::Units::mrad << " mrad was applied to " << new_deviationsVec[iDev]->hits.size() << " hits matching the station " << new_deviationsVec[iDev]->stationName.str() << " and the multilayer " << new_deviationsVec[iDev]->multilayer.str());

     }

  } // LOOP ON NUISANCES

  ATH_MSG_DEBUG("******************************");
  ATH_MSG_DEBUG("FINAL CHECKUP");
  ATH_MSG_DEBUG("Found " << deviations.size() << " nuisances after duplicates merging");
  ATH_MSG_DEBUG("******************************");

  // NOW EMPTY THE LOCAL DEVIATIONS VECTOR //
  for(unsigned int iDev=0; iDev < new_deviationsVec.size(); iDev++) {
     delete new_deviationsVec[iDev];
  }

  //ATH_MSG_DEBUG("Currently we have " << AlignmentErrorTool::deviationSummary_t::i_instance << " deviation vectors");

  return;

}

////////////////////////////
// OTHER METHODS //
////////////////////////////
////////////////////////////
// INITIALIZE/DELETE NEW LIST OF DEVIATIONS, TO BE CALLED AT EACH CALL-BACK TO THE DB
////////////////////////////
void AlignmentErrorTool::initializeAlignmentDeviationsList (std::istream& indata) {

  // CLEANUP
  deleteAlignmentDeviationsList();

  if (!indata) {
    ATH_MSG_ERROR("Alignment error configuration invalid");
    return;
  }

  ATH_MSG_DEBUG("In AlignmentErrorTool::initializeAlignmentDeviationsList");

  ATH_MSG_DEBUG("***********************************");
  ATH_MSG_DEBUG("PARSING LIST OF DEVIATIONS...");
  std::string line;
  while ( getline(indata, line) ) {

    // READING COMMENTS
    if ( line.substr(0, 1) == "#" ) {
      // ATH_MSG_DEBUG("Reading a commented line saying " << line);
      continue;
    }

    // READING FROM INPUT FILE:                                //
    std::string flag("");
    double version_tag = -1.;
    std::string name_sstring("");
    std::string multilayer_sstring("");
    double traslation(0.);
    double rotation(0.);

    // GET INPUT FILE VERSION
    if ( line.substr(0, 7) == "version" ) {

      std::istringstream(line) >> flag >> version_tag;

      ATH_MSG_INFO("*****************************************");
      ATH_MSG_INFO("Input file version " << version_tag);
      ATH_MSG_INFO("*****************************************");

      continue;
    } 

    // A FLAG CHARACTERIZING THE TYPE OF ERROR
    // A REGULAR EXPRESSION FOR THE STATION NAME (EX: BIL.*)   //
    // TWO DOUBLES FOR THE TRANSLATION AND ROTATION DEVIATIONS //
    if (std::istringstream(line) >> flag >> name_sstring >> multilayer_sstring >> traslation >> rotation) {

      ATH_MSG_DEBUG(flag << " " << name_sstring << " " << multilayer_sstring << " " << traslation << " " << rotation);

      // TEMPORARY PER STATION DEVIATION STRUCT //
      deviationSummary_t* tmp = new deviationSummary_t;

      // SAVING A REGULAR EXPRESSION WITH THE STATION NAME //
      const boost::regex name_regexp(name_sstring);

      // SAVING THE STATION DEVIATIONS IN THE STRUCT //
      tmp->stationName = name_regexp;
      tmp->multilayer = multilayer_sstring;
      tmp->traslation = traslation;
      tmp->rotation = rotation;

      // FILLING THE VECTOR OF STRUCT CONTAINING THE STATION DEVIATIONS //
      m_deviationsVec.push_back(tmp);

    } //check stream is not at the end

  } // end of loop on input file lines

  if (m_deviationsVec.empty()) {
    ATH_MSG_WARNING("Could not read any alignment error configuration");
  }
}

void AlignmentErrorTool::deleteAlignmentDeviationsList () {

   ATH_MSG_DEBUG("In AlignmentErrorTool::deleteAlignmentDeviationsList");

   // NOW EMPTY THE LOCAL DEVIATIONS VECTOR //
   for(unsigned int iDev=0; iDev < m_deviationsVec.size(); iDev++) {
      delete m_deviationsVec[iDev];
   }
   m_deviationsVec.clear();

   return;
}

StatusCode AlignmentErrorTool::update(IOVSVC_CALLBACK_ARGS){
  ATH_MSG_DEBUG("Call Back for AlignmentErrorTool");

  // THESE METHODS SHOULD BE CALLED AT EACH CALL-BACK
  std::istringstream indata(m_pMuonAlignmentErrorDbSvc->ListErrorValue()); 
  initializeAlignmentDeviationsList(indata); 

  ATH_MSG_DEBUG("###########################################");
  ATH_MSG_DEBUG("List of deviations updated");
  ATH_MSG_DEBUG(m_pMuonAlignmentErrorDbSvc->ListErrorValue());
  ATH_MSG_DEBUG("###########################################");

  return StatusCode::SUCCESS;
}

////////////////////////////
// RECOGNIZE STATION NAME //
////////////////////////////
inline std::string AlignmentErrorTool::hardwareName(MuonCalib::MuonFixedId calibId) const {
  if (sector(calibId)==13) {
    if (calibId.eta()== 7 && calibId.stationName()==5) return "BOE1A13"; // BOE1A13 not BOL7A13
    if (calibId.eta()==-7 && calibId.stationName()==5) return "BOE1C13"; // BOE1C13 not BOL7C13
  }
  std::string etaString = "0";
  etaString[0] += abs(hardwareEta(calibId));
  return calibId.stationNameString() + etaString + side(calibId) + sectorString(calibId);
}

inline std::string AlignmentErrorTool::side(MuonCalib::MuonFixedId calibId) const {
  return calibId.eta()>0 ? "A" : calibId.eta()<0 ? "C" : "B";
}

inline std::string AlignmentErrorTool::sectorString(MuonCalib::MuonFixedId calibId) const  {
  std::string ret = "00";
  int sec = sector(calibId);
  if (sec >= 10) {
    ret[0] = '1';
    sec -= 10;
  }
  ret[1] += sec;
  return ret;
}

inline int AlignmentErrorTool::sector (MuonCalib::MuonFixedId calibId) const {
  return isSmallSector(calibId) ? 2*calibId.phi() : 2*calibId.phi()-1;
}

inline bool AlignmentErrorTool::isSmallSector (MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 2: return true;
    case 4: return true;
    case 6: return true;
    case 7: return true;
    case 9: return true;
    case 10: return true;
    case 11: return true;
    case 17: return true;
    case 20: return true;
    case 23: return true;
    case 24: return true;
    case 33: return true;
    default: return false;
  }
}

inline int AlignmentErrorTool::hardwareEta(MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 3:
    {
       if (sector(calibId)==13) {
          switch (calibId.eta()) {
             case 4: return 5;
             case 5: return 6;
             case 6: return 7;
             case -4: return -5;
             case -5: return -6;
             case -6: return -7;
          }
       }
       return calibId.eta();
     }
     case 5:
     {
       if (sector(calibId)==13) {
       if (calibId.eta()== 7) return 1; // BOE1A13 not BOL7A13
       if (calibId.eta()==-7) return -1; // BOE1C13 not BOL7C13
     }
     return calibId.eta();
    }
    case 10: return calibId.eta()>0 ? calibId.eta()*2-1 : calibId.eta()*2+1;
    case 11: return calibId.eta()*2;
    case 15:
      {
        if ((sector(calibId) == 1) || (sector(calibId) == 9)) {
          switch (calibId.eta()) {
            case 4: return 5;
            case 5: return 4;
            case -4: return -5;
            case -5: return -4;
          }
        }
        return calibId.eta();
      }
    case 16:
      {
        if ((sector(calibId) == 5) && (calibId.eta() == 1)) return 2;
        if ((sector(calibId) == 5) && (calibId.eta() == -1)) return -2;
        return calibId.eta();
      }
    default: return calibId.eta();
  }
}
