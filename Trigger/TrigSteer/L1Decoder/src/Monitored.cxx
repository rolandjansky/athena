/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "./SafeQueue.h"
#include "./Monitored.h"

using namespace std;

class NoStorage final : public StorageProxy {
public:
	NoStorage() { }
	virtual void fill(double) { }
private:
	NoStorage(const NoStorage&) = delete;
	NoStorage& operator=(const NoStorage&) = delete;
};

class HistogrammingStorage : public StorageProxy {
public:
	HistogrammingStorage(TH1* h) :
		m_hist(h),
		m_hist_queue(new SafeQueue<double>()),
		m_fill_hist_thread(new thread(&HistogrammingStorage::fillHistogramLoop, this)) { }

	~HistogrammingStorage() {
		while (!m_hist_queue->empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		delete m_hist_queue;

		m_fill_hist_thread->join();
		delete m_fill_hist_thread;
	}

	void fill(double t) {
		m_hist_queue->enqueue(t);
	}
private:
	void fillHistogramLoop() {
		try {
			while (true) {
				fillHistogram();
			}
		}
		catch (SafeQueueDeletedException& e) {
		}
	}

	void fillHistogram() {
		double value = m_hist_queue->dequeue();

		m_hist->Fill(value);
	}

	TH1* m_hist;
	SafeQueue<double>* m_hist_queue;
	thread* m_fill_hist_thread;
};

shared_ptr<StorageProxy> StorageProxy::no_storage() {
	static shared_ptr<StorageProxy> no_storage_instance = make_shared<NoStorage>();

	return no_storage_instance;
}

const InterfaceID& MonitoredTool::interfaceID() {
	static InterfaceID MonitoredTool_ID("MonitoredTool", 1, 0);

	return MonitoredTool_ID;
}

MonitoredTool::MonitoredTool(const string& type, const string& name, const IInterface* parent)
	: AthAlgTool(type, name, parent)/*,
	m_histogram_service("THistSvc", name)*/ {
	StatusCode status = service("THistSvc/THistSvc", m_histogram_service);

	if (status.isFailure()) {
		ATH_MSG_ERROR("Unable to locate THistSvc" << MSG::dec);
		return;
	}

	declareInterface<MonitoredTool>(this);
	declareProperty("MonitoredVariables", m_monitored_variables_definitions, 
		"Monitored variable definitions: alias(var_name), policy(,...). For StorageDisabled: null. For StorageHistogram: path, title, n_bins, x_min, x_max.");
	//declareProperty("THistSvc", m_histogram_service);
}

MonitoredTool::~MonitoredTool() { }

StatusCode MonitoredTool::initialize() {
	//CHECK(m_histogram_service.retrieve());

	for (auto definition : m_monitored_variables_definitions) {
		ATH_MSG_DEBUG("Parsing " << definition);
		MonitoredDef def;
		string parse_error;

		if (!MonitoredDef::try_parse(definition, def, parse_error)) {
			ATH_MSG_DEBUG(parse_error);
			ATH_MSG_DEBUG("Demand to monitor variable(s): " << def.alias << " can't be satisfied");
			continue;
		}
		
		if (def.policy == StorageDisabled) {
			m_proxies[def.alias] = StorageProxy::no_storage();
		}
		else if (def.policy == StorageHistogram) {
			TH1* histo = new TH1D(def.alias.c_str(), def.title.c_str(), def.xbins, def.xmin, def.xmax);
			StatusCode status = m_histogram_service->regHist(def.path + "/" + def.alias, histo); //this->parent()->interfaceID().id() hash of e.g. MonitoredTool string
			
			if (status.isFailure()) {
				ATH_MSG_ERROR("Unable to register histogram " << def.alias << MSG::dec);
				continue;
			}
			
			m_proxies[def.alias] = shared_ptr<StorageProxy>(new HistogrammingStorage(histo));
		}
		else {
			throw logic_error("NOT IMPLEMENTED!");
		}
	}

	return StatusCode::SUCCESS;
}

shared_ptr<StorageProxy> MonitoredTool::find_proxy(monitored_id_arg id) {
	auto it = m_proxies.find(id);

	if (it == m_proxies.end()) {
		throw invalid_argument("Missing property: " + id);
	}

	return it->second;
}

std::map<std::string, StoragePolicy> MonitoredTool::MonitoredDef::m_map_string_to_policy = {
	{ "StorageDisabled", StorageDisabled },
	{ "StorageHistogram", StorageHistogram },
	//{ "StorageFile", StorageFile }
};

bool MonitoredTool::MonitoredDef::try_parse(const string& definition, MonitoredDef& parsed_obj, std::string& parse_error) {
	vector<string> def_param;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
	boost::char_separator<char> sep(",");
	tokenizer_t tokens(definition, sep);

	for (tokenizer_t::iterator itr = tokens.begin(); itr != tokens.end(); ++itr) {
		string word = *itr;
		boost::trim(word);
		def_param.push_back(word);
	}
	try {
		parsed_obj.alias = def_param[0];
		parsed_obj.policy = m_map_string_to_policy[def_param[1]];

		if (parsed_obj.policy == StorageDisabled) {
			return true;
		}
		else if (parsed_obj.policy == StorageHistogram) {
			parsed_obj.path = def_param[2];
			parsed_obj.title = def_param[3];
			parsed_obj.xbins = stoi(def_param[4]);
			parsed_obj.xmin = stod(def_param[5]);
			parsed_obj.xmax = stod(def_param[6]);

			return true;
		}
		//else if (parsed_obj.policy == StorageFile) { return true; }
	}
	catch (exception e) {
		parse_error = string("Exception caught during parse definition \"") + definition + "\". Exception message: " + e.what();
	}

	return false;
}