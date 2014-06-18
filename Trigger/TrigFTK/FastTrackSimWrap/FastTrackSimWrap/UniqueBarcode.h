/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_UNIQUEBARCODE_H
#define WRAP_UNIQUEBARCODE_H

#include <cmath>
#include <boost/tuple/tuple.hpp>

class
UniqueBarcode
{

private:

  mutable bool _valid;
  int _event_index;
  int _barcode;

public:

  UniqueBarcode() : _valid( false ) {}

  UniqueBarcode( const int& event_index , const int& barcode ) 
  : _valid(true) 
  , _event_index( event_index )
  , _barcode( barcode )
  {}

  UniqueBarcode( const signed long& ucode )
  : _valid(true) 
  {
    const boost::tuple<int,int> tmp( barcode_and_event_index(ucode) );
    _barcode = tmp.get<0>();
    _event_index = tmp.get<1>();
  }

  virtual ~UniqueBarcode() {}

  static const signed long code( const int& event_index , const int& barcode ) { return( (100000l*event_index) + barcode ); }
  static const boost::tuple<int,int> barcode_and_event_index( const signed long& ucode ) { 
    ldiv_t tmp = std::div( ucode , 100000l );
    return boost::tuple<int,int>(tmp.rem,tmp.quot); 
  }

  const int event_index() const { return _event_index; }
  const int barcode() const { return _barcode; }

  const signed long operator()() const { return code(_event_index,_barcode); }
  const bool operator==(const UniqueBarcode& rhs) const { return( _valid && rhs._valid && _event_index==rhs._event_index && _barcode==rhs._barcode ); }
  
};

#endif // WRAP_UNIQUEBARCODE_H
