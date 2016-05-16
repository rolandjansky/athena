/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
* Author: Piotr Sarna
*/

#ifndef L1Decoder_Monitored_h
#define L1Decoder_Monitored_h

#include <iostream>
#include <sstream>
#include <thread>

#include <string>
#include <tuple>
#include <map>
#include <memory>
#include <stdexcept>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "L1Decoder/TrigIdentifiers.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "SafeQueue.h"

template<class T>
class Monitored;

enum StoragePolicy
{
	StorageDisabled,
	StorageHistogram,
	//StorageFile
};

enum FillPolicy
{
	FillDisabled,
	FillExplicit,
	FillOnAssign,
	FillOnDestruction
};

class StorageProxy {
public:
	static std::shared_ptr<StorageProxy> no_storage();

	StorageProxy() { }
	virtual ~StorageProxy() { }

	virtual void fill(double) = 0;
private:
	StorageProxy(const StorageProxy&) = delete;
	StorageProxy& operator=(const StorageProxy&) = delete;
};

class MonitoredTool : public AthAlgTool {
public:
	typedef std::string monitored_id;
	typedef const std::string& monitored_id_arg;

	static const InterfaceID& interfaceID();

	MonitoredTool(const std::string& type, const std::string& name, const IInterface* parent);
	~MonitoredTool();

	StatusCode initialize();

	template<class T>
	Monitored<T> create_monitored(monitored_id_arg id, const T& default_value = {}, FillPolicy fillPolicy = FillDisabled) {
		return Monitored<T>(default_value, fillPolicy, this->find_proxy(id));
	}
private:
	std::shared_ptr<StorageProxy> find_proxy(monitored_id_arg id);

	std::map<monitored_id, std::shared_ptr<StorageProxy>> m_proxies;
	std::vector<std::string> m_monitored_variables_definitions;
	//ServiceHandle<ITHistSvc> m_histogram_service;
	ITHistSvc* m_histogram_service;

	class MonitoredDef {
	public:
		static bool try_parse(const std::string& definition, MonitoredDef& parsed_obj, std::string& parse_error);

		std::string alias;
		StoragePolicy policy;

		std::string path;
		std::string title;

		int xbins;
		double xmin;
		double xmax;
	private:
		static std::map<std::string, StoragePolicy> m_map_string_to_policy;
	};
};

template<class T>
class Monitored {
public:
	friend Monitored<T> MonitoredTool::create_monitored<T>(MonitoredTool::monitored_id_arg, const T&, FillPolicy);

	~Monitored() {
		if (m_is_assigned && m_fill_policy == FillOnDestruction) {
			m_storage->fill(static_cast<double>(m_data));
		}
	}

	void fill() {
		if (m_is_assigned && m_fill_policy == FillExplicit) {
			m_is_assigned = false;
			m_storage->fill(static_cast<double>(m_data));
		}
	}

	void operator=(T value) {
		m_is_assigned = true;
		m_data = value;

		if (m_fill_policy == FillOnAssign) {
			m_storage->fill(static_cast<double>(m_data));
		}
	}

	operator T() const {
		return m_data;
	}
private:
	Monitored(const T& default_value, FillPolicy policy, std::shared_ptr<StorageProxy> proxy)
		: m_is_assigned(false), m_data(default_value), m_fill_policy(policy), m_storage(proxy) { }

	bool m_is_assigned;
	T m_data;
	const FillPolicy m_fill_policy;
	std::shared_ptr<StorageProxy> m_storage;
};
#endif //L1Decoder_Monitored_h
