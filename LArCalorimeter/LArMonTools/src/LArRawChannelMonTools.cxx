/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArMonTools/LArRawChannelMonTools.h"

#include <stdexcept>
#include <cmath>
#include <cfloat>

#include <boost/foreach.hpp>

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnlID_Exception.h"

#define foreach BOOST_FOREACH

using namespace LArMonTools;
using namespace std;

typedef vector<HWIdentifier>::const_iterator citer_vect_hwid;

typedef map<Detector, map<Sampling, deque<Region> > > det_region_map_t;
typedef map<Sampling, deque<Region> > sam_region_map_t;


/*----------------------------------------------------------------------------*/
string LArMonTools::detector_str( const Detector& d )
{

  switch ( d ) {

  case EMBA:
    return "EMBA";
    break;

  case EMBC:
    return "EMBC";
    break;

  case EMECA:
    return "EMECA";
    break;

  case EMECC:
    return "EMECC";
    break;

  case HECA:
    return "HECA";
    break;

  case HECC:
    return "HECC";
    break;

  case FCALA:
    return "FCALA";
    break;

  case FCALC:
    return "FCALC";
    break;

  default:
    return "UNDEF";

  }

}


/*----------------------------------------------------------------------------*/
string LArMonTools::sampling_str( const Sampling& d )
{

  switch ( d ) {

  case presampler:
    return "PS";
    break;

  case sampling_one:
    return "S1";
    break;

  case sampling_two:
    return "S2";
    break;

  case sampling_three:
    return "S3";
    break;

  default:
    return "UNDEF";

  }

}

int LArMonTools::n_slots_in_feedthroughs( const Detector& d )
{

  return ( d == EMBA || d == EMBC ) ? n_slots_in_barrel :
    n_slots_in_endcap ;

}


int LArMonTools::n_feedthroughs( const Detector& d )
{

  return ( d == EMBA || d == EMBC ) ? n_feedthroughs_in_barrel :
    n_feedthroughs_in_endcap ;

}


vector<double> LArMonTools::detector_superslot_axis( Detector const &detector,
						     const LArOnlineID *lar_online_id )
{

  // define detector_test type as a reference to a member function of
  // LArOnlineID taking a constant HWIdentifier as argument and
  // returning a bool:
  // --- Warning these only have meaning in conjunction with an
  // --- instance of the class:
  //
  // --- ((*lar_online_id).*is_in_detector_ptr)( some_bool ) // works
  //
  // --- ( lar_online_id->*is_in_detector_ptr )( some_bool ) // compiler
  // ---                                                     // error

  typedef bool ( LArOnlineID::*detector_test )( const HWIdentifier ) const;
  // Define such a beast ... to be set later.
  detector_test is_in_detector_ptr;

  // --- FEBs from which detector should we consider ---
  switch ( detector ){

  case EMBA:
  case EMBC:
    is_in_detector_ptr = &LArOnlineID::isEMBchannel;
    break;
  case EMECA:
  case EMECC:
    is_in_detector_ptr = &LArOnlineID::isEMECchannel;
    break;
  case HECA:
  case HECC:
    is_in_detector_ptr = &LArOnlineID::isHECchannel;
    break;
  case FCALA:
  case FCALC:
    is_in_detector_ptr = &LArOnlineID::isFCALchannel;
    break;

  case UNDEF:
  default:
    throw domain_error("Detector badly defined. Cannot determine position in LAr.");

  }

  // --- FEBS from which side should we consider ---
  int correct_side = 0;
  switch ( detector ){

  case EMBA:
  case EMECA:
  case HECA:
  case FCALA:
    correct_side = 1;
    break;
  case EMBC:
  case EMECC:
  case HECC:
  case FCALC:
    correct_side = 0;
    break;
  case UNDEF:
  default:
    throw std::domain_error("Detector badly defined. Cannot determine position in LAr.");

  }

  const int n_slots       = n_slots_in_feedthroughs( detector );
  const int superslot_max = n_slots * n_feedthroughs( detector );

  vector<int> super_slots_to_fill;
  
  // --- loop over FEBS ---
  citer_vect_hwid end_feb = lar_online_id->feb_end();
  for ( citer_vect_hwid itr_feb = lar_online_id->feb_begin();
	itr_feb != end_feb ; ++itr_feb) {

    // --- If the FEB isn't on the correct side skip it ---
    if ( lar_online_id->pos_neg( *itr_feb ) != correct_side ) continue;

    // --- If the FEB isn't in the correct detector skip it ---
    if ( !((*lar_online_id).*is_in_detector_ptr)( *itr_feb ) ) continue;
    
    // Calculate the FEB's superslot number and remeber it.
    int i_feedthrough = lar_online_id->feedthrough( *itr_feb );
    int i_slot        = lar_online_id->slot( *itr_feb );
    int i_super_slot  = i_feedthrough * n_slots + i_slot;

    super_slots_to_fill.push_back( i_super_slot );

  }

  // Return axis assigning only one bin for regions without FEBs
  return sparse_bins_vector( super_slots_to_fill, 1, superslot_max );

}


template <class Container>
vector<double> LArMonTools::sparse_bins_vector( Container const &bins,
						typename Container::value_type min,
						typename Container::value_type max,
						double const &bin_size )
{
  
  typename Container::value_type max_bin_given = *max_element( bins.begin(), bins.end() );
  typename Container::value_type min_bin_given = *min_element( bins.begin(), bins.end() );
  
  // --- If min and max were not set, determine them from the bins given
  if ( min == max && min == 0 ) {
    
    max = max_bin_given;
    min = min_bin_given;
    
  }
  
  // --- Above if statement should negate these, but just in case ---
  if ( max < max_bin_given )
    throw std::domain_error("Largest bin requested exceeds given maximum");
	
  if ( min > min_bin_given )
    throw std::domain_error("Smallest bin requested exceeds given minimum");

  // --- Determine Number of bins necessary ---
  vector<double>::size_type n_elements = max - min + static_cast<typename Container::value_type>( bin_size );
	
  vector<double> result( n_elements, min );
	
  foreach( typename Container::value_type x, bins )
    result.at( x - min ) = ( static_cast<double>( x ) - 0.5 ) * bin_size;
	
  if ( result.front() == min ) result.front() = min - 0.5 * bin_size;
  if ( result.back()  != min ) result.push_back( result.back() + bin_size );
  if ( result.back()  != max + 0.5 * bin_size ) result.push_back( max + 0.5 * bin_size );
	
  bool is_first_zero_in_row = false;
  bool is_first_entry = true;
  double last = static_cast<double>( min );
	
  foreach ( double &d, result ){
		
    if ( is_first_entry ) {
			
      is_first_entry = false;
      continue;
			
    }
		
    if ( d != min ) {
		
      is_first_zero_in_row = true;
      last = d;
      
    } else if ( is_first_zero_in_row ) {
		
      d = last + bin_size;
      is_first_zero_in_row = false;
      last = d;

    }
		
  }
	
	
  for ( vector<double>::iterator itr = result.begin(); itr != result.end(); itr++ ) {
		
    if ( *itr == min ) {
      
      itr = result.erase( itr ) - 1;
      
    }
		
  }
	
  return result;

}



int LArMonTools::pos_neg( const Detector &detector )
{

  int result = 0;

  switch ( detector ) {

  case EMBA:
  case EMECA:
  case HECA:
  case FCALA:
    result = 1;
    break;

  case EMBC:
  case EMECC:
  case HECC:
  case FCALC:
    result = -1;
    break;

  default:
    break;

  }

  return result;

}



lar_axis_maker::lar_axis_maker()
{
}

    
lar_axis_maker::lar_axis_maker( const map<Detector, map<Sampling, deque<Region> > > &regions )
  : _regions( regions )
{

  generate();

}


lar_axis_maker::~lar_axis_maker()
{
}


const vector<double> lar_axis_maker::operator() (const Detector &d, const Sampling &s)
{

  return _bins[d][s];

}


void lar_axis_maker::generate()
{

  _bins.clear();

  // for each detector
  foreach( const det_region_map_t::value_type &det_regions, _regions ) {

    const Detector &det = det_regions.first;

    // for each sampling
    foreach( const sam_region_map_t::value_type &sam_regions, det_regions.second ) {

      const Sampling &samp = sam_regions.first;
      _bins[det][samp] = vector<double>();

      // for each region
      foreach( const Region &region, sam_regions.second ) {

	double        lo   = region.lower;
	const double &up   = region.upper;
	const double &size = region.size;

	double diff = DBL_MAX;
	double last_diff = DBL_MAX;
	if ( size <= 0 ) break;
	while ( diff > 1.0e-10 ) { // float computaation =>
					    // 0 + 1 - 1 != 0 ...
	  _bins[det][samp].push_back( lo );
	  lo += size;
	  last_diff = diff;
	  diff = fabs( lo - up );
	  if ( diff > last_diff ) break;

	}

      }

      _bins[det][samp].push_back( sam_regions.second.back().upper );

    }

  }

}


const map<Detector, map<Sampling, deque<Region> > > &lar_axis_maker::regions() const { return _regions; }

void lar_axis_maker::regions( const map<Detector, map<Sampling, deque<Region> > >& regions )
{

  _regions = regions;
  generate();

} 

bool LArMonTools::operator< ( const Region &lhs, const Region &rhs )
{

  return lhs.lower < rhs.lower && lhs.upper < rhs.upper;

}



/* -------------------------------------------------------------------------- */
/*                           helper classes                                   */
/* -------------------------------------------------------------------------- */



SelectionContext::SelectionContext()
  : _positive_noise(0),
    _negative_noise(0),
    _quality( DBL_MAX ),
    _energy(0)
{}

SelectionContext::SelectionContext( const double& quality )
  : _positive_noise(0),
    _negative_noise(0),
    _quality( quality ),
    _energy(0)
{}

SelectionContext::~SelectionContext() {}


// Setters:
void SelectionContext::quality( const double& quality) { _quality = quality; }



QualitySelector::QualitySelector()
  : _pSelectionContext(0)
{}
      

// Getters:
const SelectionContext * QualitySelector::selectionContext() const { return _pSelectionContext; }


// Setters:
void QualitySelector::selectionContext( const SelectionContext * pSelectionContext ) {
  _pSelectionContext = pSelectionContext;
}


QualitySelector::~QualitySelector() {}




MeanCalculator::MeanCalculator()
  : _moment0( 0. )
  , _moment1( 0. )
  , _mean( 0. )
{}

    
MeanCalculator::~MeanCalculator() {}

double& MeanCalculator::Add( const double & x , const double & w ) {
  _moment0 += w;
  _moment1 += x * w;
  _mean = ( _moment0 > 0. )
    ? _moment1 / _moment0
    : 0.;
  return _mean;
}

double MeanCalculator::result() const { return _mean; }

void MeanCalculator::result( const double & mean) { _mean = mean; }



ResolutionCalculator::ResolutionCalculator()
  : _a ( 0. )
  , _b ( 0. )
{}


ResolutionCalculator::ResolutionCalculator( const double &a, const double &b )
  : _a ( a )
  , _b ( b )
{}

  
ResolutionCalculator::~ResolutionCalculator() {}


double ResolutionCalculator::operator() ( const double& x ){

  double numerator = x*x;
  double denominator = _a * _a + _b * _b * x * x;

  return (denominator > 0. )
    ? sqrt( numerator / denominator )
    : 0.;

}

