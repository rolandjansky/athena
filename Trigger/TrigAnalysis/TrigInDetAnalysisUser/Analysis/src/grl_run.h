/* emacs: this is -*- c++ -*- */
/**
 **     @file    grl_run.h
 **
 **     @basic   basic run class for the good runs list 
 **
 **              contsins the run number and a vector of 
 **              ( first, last ) pairs of valid lumi block 
 **              ranges                   
 **
 **     @author  mark sutton
 **     @date    Mon 24 Oct 2016 17:12:10 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  GRLRUN_H
#define  GRLRUN_H

#include <iostream>
#include <vector>
#include <map>


class grl_run : public std::vector<std::pair<int,int> >   {

public:

  grl_run( int run ) : mrun(run), msize(0), mfirst(0), mlast(0) { }

  grl_run( int run, int first, int last ) : mrun(run), msize(0), mfirst(0), mlast(0) {
    this->push_back( std::pair<int,int>( first, last ) );
    msize += last-first+1;
    mfirst = first;
    mlast  = last; 
  } 
  
  virtual ~grl_run() { } 

  void add_range( int first, int last ) { 
    this->push_back( std::pair<int,int>( first, last ) );
    msize += last-first+1;
    if ( last>mlast ) mlast = last;
    if ( mfirst==0 || first<mfirst )   mfirst = first;
 
  }

  int run()   const { return mrun; }
  int first() const { return mfirst; }
  int last()  const { return mlast; }

  int lbsize() const { return msize; }

  static  bool comparison( const grl_run* r1, const grl_run* r2 ) { 
    return r1->lbsize()>r2->lbsize(); 
  }


private:
  
  int mrun;
  
  int msize;
  
  int mfirst;
  int mlast;

};



inline std::ostream& operator<<( std::ostream& s, const grl_run& _g ) { 
  return s << "[run: " << _g.run() << "\t: " << _g.first() << "\t - " << _g.last() << "\t: size " << _g.lbsize() << " ]";
}


#endif  // GRLRUN_H 










