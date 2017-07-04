/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NtupleStationId_H
#define NtupleStationId_H


//c - c++
#include "sstream"

//MuonCalib
#include "MuonCalibIdentifier/MuonFixedId.h"


class MdtIdHelper;
namespace MuonGM {
class MuonDetectorManager;
}

namespace MuonCalib {

/** @class NtupleStationId
Station Identifier for sorting calibration data
@author rauscher@cern.ch
*/

class NtupleStationId
	{
	public:
	/** Default constructor */
		inline NtupleStationId(): m_station(-1), m_eta(-99), m_phi(-1), m_ml(0), m_author(0), m_region_hash(0), m_n_ml(-1), m_geom_ok(false), m_region_id_valid(false)
		 { ResetVectors(); }
	/** Initializing Constructor
		@param id MuonFixedId identifier
	*/
		inline NtupleStationId(const MuonFixedId &id): m_region_hash(0), m_n_ml(-1)
			{
			ResetVectors();
			Initialize(id);
			}
	/** Initializing Constructor
		@param station station name
		@param eta station eta
		@param phi station phi
	*/
		inline NtupleStationId(const std::string & station, const int & eta, const int & phi, const int & ml=0, const int &author=0): m_region_hash(0), m_n_ml(-1)
			{
			Initialize(station, eta, phi, ml, author);
			}
	/** initialize function
		@param id MuonFixedId identifier
	*/
		inline void Initialize(const MuonFixedId &id)
			{
			m_station=id.stationName();
			m_eta=id.eta();
			m_phi=id.phi();
			m_ml=id.mdtMultilayer();
			m_author=0;
			m_region_id_valid = false;
			m_geom_ok=false;
			}
		inline void Initialize(const int & station, const int & eta, const int & phi, const int & ml=0, const int &author=0)
			{
			m_station = station;
			m_eta = eta;
			m_phi = phi;
			m_ml = ml;
			m_author=author;
			m_region_id_valid = false;
			m_geom_ok=false;
			}
		inline void ResetVectors()
			{
			for(unsigned int ii=0; ii<2; ii++){
			  m_n_layer[ii] = -1;
			  m_n_tubes[ii] = -1;
			  m_layer_min[ii] = -1;
			  m_layer_max[ii] = -1;
			  m_tube_min[ii] = -1;
			  m_tube_max[ii] = -1;
			}
			}
	/** initialize function
		@param station station name
		@param eta station eta
		@param phi station phi
	*/
		bool Initialize(const std::string & station, const int & eta, const int & phi, const int & ml=0, const int & author=0);
	/** set station, eta or phi seperately */
		void SetStation(const std::string & station);
	/** set phi */
		inline void SetPhi(const int & phi)
			{
			m_phi=phi;
			m_region_id_valid = false;
			}
		inline void SetEta(const int & eta)
			{
			m_eta=eta;
			m_region_id_valid = false;
			}
		inline void SetMultilayer(const int &ml)
			{
			m_ml=ml;
			m_region_id_valid = false;
			}
		inline void SetAuthor(const int & author)
			{
			m_author=author;
			}
	/** comparision operators for the use as map ids*/
		inline bool operator < (const NtupleStationId &other) const
			{
			if(m_station<other.m_station) return true;
			if(m_station>other.m_station) return false;
			if(m_eta<other.m_eta) return true;
			if(m_eta>other.m_eta) return false;
			if(m_phi<other.m_phi) return true;
			if(m_phi>other.m_phi) return false;
			if(m_ml<other.m_ml) return true;
			if(m_ml>other.m_ml) return false;
			if(m_author<other.m_author) return true;
			return false;
			}
		inline bool operator > (const NtupleStationId &other) const
			{
			if(m_station>other.m_station) return true;
			if(m_station<other.m_station) return false;
			if(m_eta>other.m_eta) return true;
			if(m_eta<other.m_eta) return false;
			if(m_phi>other.m_phi) return true;
			if(m_phi<other.m_phi) return false;
			if(m_ml>other.m_ml) return true;
			if(m_ml<other.m_ml) return false;
			if(m_author>other.m_author) return true;
			return false;
			}
	/** return true if icdentifier matches selection */
		inline bool operator == (const MuonFixedId &id) const
			{
			if(m_station>=0)
				if(id.stationName() != m_station) return false;
			if(m_eta!=-99)
				if(id.eta() != m_eta) return false;
			if(m_phi>=0)
				if(id.phi() != m_phi) return false;
			if(m_ml>0)if(id.mdtMultilayer() != m_ml) return false;
			return true;
			}
	/** return true if the regions are the same */
		inline bool operator ==(const NtupleStationId &id) const
			{
			if(m_station != id.m_station) return false;
			if(m_eta != id.m_eta) return false;
			if(m_phi != id.m_phi) return false;
			if(m_ml && id.m_ml) if(m_ml!=id.m_ml) return false;
			if(m_author && id.m_author) if(m_author != id.m_author) return false;
			return true;
			}
	/** return the region id string */
		inline const std::string regionId() const
			{
			if(!m_region_id_valid) createRegionId();
			return m_region_id;
			}
	/** initialize geometry information */
		bool InitializeGeometry(const MdtIdHelper* mdtIdHelper, const MuonGM::MuonDetectorManager* detMgr);
	/** get station eta and phi*/
		inline int GetStation() const
			{
			return m_station;
			}
		inline int GetEta() const
			{
			return m_eta;
			}
		inline int GetPhi() const
			{
			return m_phi;
			}
		inline int GetMl() const
			{
			return m_ml;
			}
		inline const int & GetAuthor() const
			{
			return m_author;
			}
	/** return geometry information */
		inline int NMultilayers() const
			{
			if(!m_geom_ok) return -1;
			return m_n_ml;
			}
		inline int NLayers(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return m_n_layer[ml];
			}
		inline int NTubes(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return  m_n_tubes[ml];
			}
		inline int LayerMin(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return m_layer_min[ml];
			}
		inline int LayerMax(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return m_layer_max[ml];
			}
		inline int TubeMin(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return m_tube_min[ml];
			}
		inline int TubeMax(int ml) const
			{
			if(!m_geom_ok || ml >= m_n_ml) return -1;
			return m_tube_max[ml];
			}
		inline int RegionHash() const
			{
			if(!m_geom_ok) return -1;
			return m_region_hash;
			}
		int FixedId() const;
//=============================================================================
	private:
	//! id
		int m_station, m_eta, m_phi, m_ml;
		int m_author;
	//! geo model information
		int m_region_hash, m_n_ml, m_n_layer[2], m_n_tubes[2];
		int m_layer_min[2], m_layer_max[2];
		int m_tube_min[2], m_tube_max[2];
		bool m_geom_ok;
	//! region id
		mutable std::string m_region_id;
		mutable bool m_region_id_valid;
	/** create the region id string */
		void createRegionId() const;
	};


}


#endif
