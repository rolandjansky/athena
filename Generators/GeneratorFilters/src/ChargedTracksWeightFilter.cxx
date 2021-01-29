/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#include "GeneratorFilters/ChargedTracksWeightFilter.h"
#include "TruthUtils/HepMCHelpers.h"
#include "CLHEP/Random/RandomEngine.h"
#include <algorithm>

using Point = ChargedTracksWeightFilter::Spline::Point;

ChargedTracksWeightFilter::ChargedTracksWeightFilter(const std::string& name, ISvcLocator* pSvcLocator)
    : GenFilter(name, pSvcLocator),
    m_randSvc("AtRndmGenSvc",name)
{
    declareProperty("Ptcut", m_Ptmin = 50.0);
    declareProperty("Etacut", m_EtaRange = 2.5);
    declareProperty("NchMin", m_nchmin = 0);
    declareProperty("NchMax", m_nchmax = 20);
    declareProperty("SplineX", m_weight_fun_x);
    declareProperty("SplineY", m_weight_fun_y);
}

StatusCode ChargedTracksWeightFilter::filterInitialize() {

    if(m_nchmin < 0) m_nchmin = 0;

    if(m_nchmax < m_nchmin || m_nchmin < 0) {
        ATH_MSG_ERROR("Wrong values of NchMin and NchMax");
        return StatusCode::FAILURE;
    }

    CHECK(spline.initialize(m_weight_fun_x, m_weight_fun_y));

    m_min_weight = spline.get_minimum(m_nchmin, m_nchmax);

    for(int i=m_nchmin; i<=m_nchmax; i++){
        ATH_MSG_DEBUG("nch = " << i 
                << " f(nch) = " << spline.value(i)
                << " weight(nch) = " << get_nch_weight(i));
    }

    if(m_min_weight<=0) {
        ATH_MSG_ERROR("Weight functions must be >0");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(name() << " Minimum weight = " << m_min_weight);
    return StatusCode::SUCCESS;
}

StatusCode ChargedTracksWeightFilter::filterFinalize() {

    double eff = m_nevents_accepted / m_nevents_selected;
    ATH_MSG_INFO(name() << " Weight-Filtering Only Efficiency = " << eff 
            << " [" << m_nevents_accepted << " / " << m_nevents_selected << "] \n");

    return StatusCode::SUCCESS;
}

StatusCode ChargedTracksWeightFilter::filterEvent() {
    int nChargedTracks = 0;

    // Loop over all events in McEventCollection
    for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
        HepMC::GenEvent* genEvt = *itr;

        // Loop over all particles in event
        HepMC::GenEvent::particle_const_iterator pitr;
        for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {

            // We only care about stable particles
            if (!MC::isGenStable(*pitr)) continue;

            // Particle's charge
            int pID = (*pitr)->pdg_id();
            double pCharge = MC::PID::charge(pID);

            // Count tracks in specified acceptance
            const double pT = (*pitr)->momentum().perp();
            const double eta = (*pitr)->momentum().pseudoRapidity();
            if (pT >= m_Ptmin && fabs(eta) <= m_EtaRange && pCharge != 0) {
                ATH_MSG_DEBUG("Found particle, " <<
                        " pT = " << pT <<
                        " eta = " << eta <<
                        " pdg id = " << pID <<
                        " charge = " << pCharge << " in acceptance");
                nChargedTracks += 1;
            }
        }
    }

    // Summarise event
    ATH_MSG_DEBUG("# of tracks " << nChargedTracks <<
            " with pT >= " << m_Ptmin <<
            " |eta| < " << m_EtaRange <<
            " nch_min= " << m_nchmin<<
            " nch_max= " << m_nchmax);

    // get event weight from the McEventCollection, used to determine efficiency of weight-filtering and cuts
    double orig_event_weight = 1;
    CHECK(event_weight(orig_event_weight));

    const auto selection = (((nChargedTracks <= m_nchmax)) && (nChargedTracks >= m_nchmin));


    if(!selection) {
        setFilterPassed(false);
        return StatusCode::SUCCESS;
    }

    m_nevents_selected += orig_event_weight;

    // weight-filtering weight
    const auto weight = get_nch_weight(nChargedTracks);

    bool accept = false;

    if(weight >=1){
        accept = true;
    } else {
        // weighting

        // Grab random number engine
        CLHEP::HepRandomEngine* m_rndmGen = m_randSvc->GetEngine(name());
        if (!m_rndmGen) {
            ATH_MSG_WARNING("Failed to retrieve random number engine " << name());
            setFilterPassed(false);
            return StatusCode::FAILURE;
        }

        double rnd = m_rndmGen->flat();
        if(weight>rnd) {
            accept = true;
        }
    }

    if(accept) {

        weight_event(weight);

        setFilterPassed(true);

        double final_event_weight = 1; 
        CHECK(event_weight(final_event_weight));

        ATH_MSG_DEBUG("Event accepted nch: " << nChargedTracks 
                << " nch weight: " << weight 
                << " orig_event_weight: " << orig_event_weight 
                << " new_event_weight: " << final_event_weight);
        m_nevents_accepted += orig_event_weight;
        return StatusCode::SUCCESS;
    }

    setFilterPassed(false);
    return StatusCode::SUCCESS;
}

double ChargedTracksWeightFilter::get_nch_weight(int nch) const {

    // interval with no weighting
    if( nch > m_nchmax || nch < m_nchmin ) return 1;

    auto w_fc = spline.value(nch);

    return m_min_weight/w_fc;
}


void ChargedTracksWeightFilter::weight_event(double weight) {

    for (auto event : *(events())){
        if(!event) continue;

        if (event->weights().empty()) {
            event->weights().push_back( 1/weight );
        } else {
            for(auto & w: event-> weights()) {
                w /= weight;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
// spline
/////////////////////////////////////////////////////////////////////

bool comp_x (const Point & lhs,  const Point &rhs){
    return lhs.x < rhs.x;
}

bool comp_y (const Point & lhs,  const Point &rhs){
    return lhs.y < rhs.y;
}

double ChargedTracksWeightFilter::Spline::value(int nch) const {

    const double dummy = -1;

    // find lowest weight in allowed range
    auto lower = std::upper_bound(points.begin(), points.end(), 
            Point(nch, dummy, dummy), comp_x);

    if(lower != points.begin()) lower--;

    const auto & x = lower-> x;
    const auto & y = lower-> y;;
    const auto & slope = lower-> slope;;

    // weight using linear spline
    const auto w_fc = (nch-x)*slope + y;

    if(w_fc<0) return 0;   // extrapolation gives negative value
    else return w_fc;
}

double ChargedTracksWeightFilter::Spline::get_minimum(double min, double max) const {
    // evaluate minimum of the spline over a defined range

    // spline must be initialized
    if(points.empty()) return -1;

    const double dummy = -1;

    // if min, max are in between points, or extrapolation needed 
    auto minimum = std::min(value(min), value(max));

    //  find boundary points and scan the allowed range
    auto min_item = std::lower_bound(points.begin(), points.end(), Point(min, dummy, dummy), comp_x);
    auto max_item = std::upper_bound(points.begin(), points.end(), Point(max, dummy, dummy), comp_x);

    if(min_item != max_item) {
        const auto smallest = std::min_element(min_item, max_item, comp_y);
        minimum = std::min(minimum, smallest-> y);
    }

    return minimum;
}

StatusCode ChargedTracksWeightFilter::Spline::initialize( std::vector<double> & x, std::vector<double> & y) {

    if(x.size() < 2 || x.size() != y.size()) {
        std::cout << "Spline not properly defined. SplineX, SplineY vectors are required to have at least 2 elements and same size.\n";
        return StatusCode::FAILURE;
    }  

    // calculate slope terms
    for(size_t i=0; i< x.size()-1; i++){

        const auto & x0 = x[i];
        const auto & y0 = y[i];
        const auto & x1 = x[i+1];
        const auto & y1 = y[i+1];

        const auto dx = x1 - x0;
        const auto dy = y1 - y0;

        if(dx <=0) {
            std::cout << "Spline not properly defined. SplineX is required to be monotonically increasing.\n";
            return StatusCode::FAILURE;
        }

        points.push_back( Point(x[i], y[i], dy/dx));
    }

    return StatusCode::SUCCESS;
}

StatusCode ChargedTracksWeightFilter::event_weight(double & event_weight) const {

    event_weight = 1;

    auto first_event = event_const();

    if(!first_event){
        ATH_MSG_ERROR("No events in McEventCollection");
        return StatusCode::FAILURE;
    }

    if(! first_event-> weights().empty()) {
        event_weight = first_event-> weights()[0]; 
    }

    return StatusCode::SUCCESS;
}
