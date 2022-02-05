/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this
#include "MdtCalibIOSvc/CalibrationOracleFileIOTool.h"

// MdtCalibUtils
#include "MdtCalibUtils/RtDataFromFile.h"
#include "MdtCalibUtils/TubeDataFromFile.h"

// MdtCalibData
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

// MdtCalibRt
#include "MdtCalibRt/RtCalibrationOutput.h"

// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

// MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

// c - c++
#include <dirent.h>
#include <sys/types.h>

#include "fstream"
#include "iostream"
#include "string"

namespace MuonCalib {

    CalibrationOracleFileIOTool::CalibrationOracleFileIOTool(const std::string &t, const std::string &n, const IInterface *p) :
        AthAlgTool(t, n, p), m_calib_dir("oracle_calib") {
        declareInterface<ICalibrationIOTool>(this);
        declareProperty("outputLocation", m_calib_dir);
    }

    StatusCode CalibrationOracleFileIOTool::WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int iov_start,
                                                    int iov_end) {
        // create directory
        system(("mkdir -p " + m_calib_dir + "/t0s").c_str());
        // open file
        std::string t0_dbfile_name(m_calib_dir + "/t0s/DB_t0" + station_id.regionId() + ".dat");
        FILE *db_file;
        db_file = fopen(t0_dbfile_name.c_str(), "w");
        if (!db_file) {
            ATH_MSG_FATAL("Cannot open output file!");
            return StatusCode::FAILURE;
        }
        TubeDataFromFile t0_file;
        t0_file.setNRegions(1);
        t0_file.addTubes(0, t0_output);
        t0_file.write_forDB(db_file, -9999, iov_start, iov_end);
        fclose(db_file);
        return StatusCode::SUCCESS;
    }

    StatusCode CalibrationOracleFileIOTool::WriteRt(const RtCalibrationOutput *rt_relation, std::shared_ptr<const IRtResolution> resolution,
                                                    const NtupleStationId &station_id, int iov_start, int iov_end, bool /*real_rt*/,
                                                    bool /*real_resolution*/) {
        RtDataFromFile rt_data;
        std::shared_ptr<const IRtRelation> new_rt = rt_relation->rt();
        std::unique_ptr<RtDataFromFile::RtRelation> rt = std::make_unique<RtDataFromFile::RtRelation>();
        int rt_region_id(0);
        rt_data.setNRts(1);
        rt_data.addRt(rt_region_id, rt.get(), rt_relation->fullInfo().get());
        if (!fill_rt(rt, new_rt, resolution)) return StatusCode::FAILURE;
        // write out the r-t relationship //
        std::string location = "location";
        int mdt_rt_id = 99;
        std::string mdt_rt_map_t_id = "mdt_rt_map_t_id";
        int mdt_rt_map_r_id = 99;
        int mdt_rt_map_s_id = 99;
        system(("mkdir -p " + m_calib_dir + "/rts").c_str());
        std::string rt_dbfile_name(m_calib_dir + "/rts/DBrt_" + station_id.regionId() + ".dat");
        std::string rtt_dbfile_name(m_calib_dir + "/rts/DBrt_" + station_id.regionId() + "_t.dat");
        std::string rtr_dbfile_name(m_calib_dir + "/rts/DBrt_" + station_id.regionId() + "_r.dat");
        std::string rts_dbfile_name(m_calib_dir + "/rts/DBrt_" + station_id.regionId() + "_s.dat");
        FILE *db_rt_file;
        db_rt_file = fopen(rt_dbfile_name.c_str(), "w");
        FILE *db_rtt_file;
        db_rtt_file = fopen(rtt_dbfile_name.c_str(), "w");
        FILE *db_rtr_file;
        db_rtr_file = fopen(rtr_dbfile_name.c_str(), "w");
        FILE *db_rts_file;
        db_rts_file = fopen(rts_dbfile_name.c_str(), "w");
        if (db_rt_file == nullptr || db_rtt_file == nullptr || db_rtr_file == nullptr || db_rts_file == nullptr) {
            ATH_MSG_FATAL("Cannot open output files");
            if (db_rt_file) fclose(db_rt_file);
            if (db_rtt_file) fclose(db_rtt_file);
            if (db_rtr_file) fclose(db_rtr_file);
            if (db_rts_file) fclose(db_rts_file);
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("Writing out r-t relationships in the files for calibration db.");
        // for the time being the variables:
        // mdt_rt_id,mdt_rt_map_t_id,mdt_rt_map_r_id,
        // mdt_rt_map_s_id and location
        // are dummy.
        fprintf(db_rt_file, " %d,%d,%d,%d,%d,%s,", mdt_rt_id, station_id.FixedId(), -9999, iov_start, iov_end, location.c_str());
        fprintf(db_rtt_file, " %s,%d,%d,", mdt_rt_map_t_id.c_str(), mdt_rt_id, mdt_rt_id);
        fprintf(db_rtr_file, " %d,%d,%s,", mdt_rt_map_r_id, mdt_rt_id, mdt_rt_map_t_id.c_str());
        fprintf(db_rts_file, " %d,%d,", mdt_rt_map_s_id, mdt_rt_id);
        rt_data.write_forDB(db_rt_file, db_rtt_file, db_rtr_file, db_rts_file, rt_region_id);
        ATH_MSG_INFO("r-t relationships wrote in the files for calibration db.");
        fclose(db_rt_file);
        fclose(db_rtt_file);
        fclose(db_rtr_file);
        fclose(db_rts_file);
        return StatusCode::SUCCESS;
    }

    bool CalibrationOracleFileIOTool::fill_rt(std::unique_ptr<RtDataFromFile::RtRelation> &rt, const std::shared_ptr<const IRtRelation>& new_rt,
                                              const std::shared_ptr<const MuonCalib::IRtResolution>& resolut) {
        ///////////////
        // VARIABLES //
        ///////////////
        const CalibFunc::ParVec &rt_param = new_rt->parameters();
        // parameters of the r-t relationship to be copied
        std::shared_ptr<const RtChebyshev> rt_Chebyshev = std::dynamic_pointer_cast<const RtChebyshev>(new_rt);
        std::shared_ptr<const RtRelationLookUp> rt_lookup = std::dynamic_pointer_cast<const RtRelationLookUp>(new_rt);

        ////////////////////////
        // FILL THE r-t CLASS //
        ////////////////////////

        // case 1: r-t relationship is stored in the class RtChebyshev //
        if (rt_Chebyshev) {
            unsigned int nb_points(100);
            double t_length(rt_Chebyshev->tUpper() - rt_Chebyshev->tLower());
            double bin_size(t_length / static_cast<double>(nb_points - 1));
            for (unsigned int k = 0; k < nb_points; k++) {
                double time(rt_Chebyshev->tLower() + k * bin_size);
                double radius(rt_Chebyshev->radius(time));
                double resol(0.0);
                resol = resolut->resolution(time);
                if (std::isnan(time) || std::isnan(radius) || std::isnan(resol)) {
                    ATH_MSG_FATAL("Filling nan into rt relation!");
                    //				return false;
                }
                rt->addEntry(time, radius, resol);
            }
        }

        // case 2: r-t relationship is stored in the class RtRelationLookUp //
        if (rt_lookup) {
            double t_min(rt_param[0]);
            double bin_size = rt_param[1];
            unsigned int nb_points(rt_lookup->nPar() - 2);
            for (unsigned int k = 0; k < nb_points; k++) {
                double radius(rt_param[k + 2]);
                double resol(0.0);

                resol = resolut->resolution(t_min + bin_size * k);
                if (std::isnan(radius) || std::isnan(resol)) {
                    ATH_MSG_FATAL("Filling nan into rt relation!");
                    //				return false;
                }
                rt->addEntry(t_min + bin_size * k, rt_param[k + 2], resol);
            }
        }

        ////////////////////////////////////////////
        // DECLARE THE NEW r-t RELATIONSHIP VALID //
        ////////////////////////////////////////////
        rt->isValid(1);

        return true;
    }  // CalibrationOracleFileIOTool::fill_rt

}  // namespace MuonCalib
