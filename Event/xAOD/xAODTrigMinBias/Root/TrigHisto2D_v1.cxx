/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigHisto2D_v1.h"

#include <iostream> 

//#define DBG
namespace xAOD {
  
  // Default ctor
  TrigHisto2D_v1::TrigHisto2D_v1()
  : SG::AuxElement(),m_binWidthX(0),m_binWidthY(0) {
  }
  
  TrigHisto2D_v1::TrigHisto2D_v1(unsigned int nbins_x, float min_x, float max_x, unsigned int nbins_y, float min_y, float max_y)
  : SG::AuxElement() {
    initialize(nbins_x, min_x, max_x, nbins_y, min_y, max_y);
  }
  
  TrigHisto2D_v1::~TrigHisto2D_v1(){}
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER(TrigHisto2D_v1, std::vector<float>, contents, setContents)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, unsigned int, nbinsX, setNbinsX)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, float, minX, setMinX)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, float, maxX, setMaxX)
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, unsigned int, nbinsY, setNbinsY)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, float, minY, setMinY)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrigHisto2D_v1, float, maxY, setMaxY)
  
  void TrigHisto2D_v1::initialize(unsigned int nbins_x, float min_x, float max_x, unsigned int nbins_y, float min_y, float max_y){
    
    //the aux store has to exist if we wont to write anything there
    if(!this->hasStore())
      this->makePrivateStore();
    
    /**compute contents_size **/
    unsigned int contents_size = (nbins_x+TrigHisto2D_v1::EXTRA_BINS)*(nbins_y+TrigHisto2D_v1::EXTRA_BINS);
#ifdef DBG
    std::cout<<"Creating Thist2D with :"<<contents_size<< " elements"<<std::endl<<"nbins_x+2="<< nbins_x+TrigHisto2D_v1::EXTRA_BINS<<std::endl<<"nbins_y+2="<<nbins_y+TrigHisto2D_v1::EXTRA_BINS<<std::endl;
#endif
    /**create empty contents vector**/
    setContents(std::vector<float>(contents_size,0.));
    setMinX(min_x);
    setMaxX(max_x);
    setNbinsX(nbins_x);
    setMinY(min_y);
    setMaxY(max_y);
    setNbinsY(nbins_y);
    
    /**compute binwdiths'**/
    if(nbins_x != 0) {
      m_binWidthX = (max_x - min_x)/((float)nbins_x); // Calculate bin size.
    }
    else {
      m_binWidthX = 0.;
    }
    
    if(nbins_y != 0) {
      m_binWidthY = (max_y - min_y)/((float)nbins_y); // Calculate bin size.
    }
    else {
      m_binWidthY = 0.;
    }   
  }
  
  void TrigHisto2D_v1::fill(float value_x, float value_y, float weight=1.){
    
    unsigned int ibin_x = findBinX(value_x);
    unsigned int ibin_y = findBinY(value_y);

    unsigned int ibin = ibin_y*(nbinsX()+TrigHisto2D_v1::EXTRA_BINS) + ibin_x;
   
    static Accessor< std::vector<float> > acc_contents( "contents" );

    acc_contents(*this).at(ibin)+= weight;
  }
  
  std::vector<float> TrigHisto2D_v1::profileX() const {
    std::vector<float> contentsX(nbinsX()+TrigHisto2D_v1::EXTRA_BINS,0.);
    
    static Accessor< std::vector<float> > acc_contents( "contents" );
    
    for(unsigned int ix = 0 ; ix < nbinsX() + TrigHisto2D_v1::EXTRA_BINS; ix++){
      for(unsigned int iy = 0; iy < nbinsY() + TrigHisto2D_v1::EXTRA_BINS; iy++){
	contentsX.at(ix) += acc_contents(*this).at(ix + iy*(nbinsX() + TrigHisto2D_v1::EXTRA_BINS));
      }
    }
    return contentsX;
  }
  
  std::vector<float> TrigHisto2D_v1::profileY() const {
    std::vector<float> contentsY(nbinsY()+TrigHisto2D_v1::EXTRA_BINS);
    
    static Accessor< std::vector<float> > acc_contents( "contents" );
    
    for(unsigned int iy = 0 ; iy < nbinsY() + TrigHisto2D_v1::EXTRA_BINS; iy++){
      for(unsigned int ix = 0; ix < nbinsX() + TrigHisto2D_v1::EXTRA_BINS; ix++){
	contentsY.at(iy) += acc_contents(*this).at(ix + iy*(nbinsX() + TrigHisto2D_v1::EXTRA_BINS));
      }
    }
    return contentsY;
  }
  
  double TrigHisto2D_v1::sumEntries(float value_x, float value_y, int cutType) const {
    
    unsigned int ibin, ibin_x, ibin_y, ibin_x_selected, ibin_y_selected;
    double entries;
    
    static Accessor< std::vector<float> > acc_contents( "contents" );
    
    // Find the x bin index that the cut corresponds to.
    ibin_x_selected = findBinX(value_x);
    
    // Find the y bin index that the cut corresponds to.
    ibin_y_selected = findBinY(value_y);
    entries = 0.;
    
    
    if( nbinsX()==0 || nbinsY()==0 ){
      return 0;
    }
    else{
      
      if(cutType == TrigHistoCutType::BELOW_X_BELOW_Y) {
	for(ibin_x = 0; ibin_x <= ibin_x_selected; ibin_x++) {
	  for(ibin_y = 0; ibin_y <= ibin_y_selected; ibin_y++) {
	    ibin = ibin_y*(nbinsX()+2) + ibin_x; // Find position in 1d data array
	    entries += acc_contents(*this).at(ibin);
	  }
	}
      }
      else if(cutType == TrigHistoCutType::ABOVE_X_BELOW_Y) {
	for(ibin_x = ibin_x_selected; ibin_x < nbinsX()+ TrigHisto2D_v1::EXTRA_BINS; ibin_x++) {
	  for(ibin_y = 0; ibin_y <= ibin_y_selected; ibin_y++) {
	    ibin = ibin_y*(nbinsX()+ TrigHisto2D_v1::EXTRA_BINS) + ibin_x; // Find position in 1d data array
	    entries += acc_contents(*this).at(ibin);
	  }
	}
      }
      else if(cutType == TrigHistoCutType::BELOW_X_ABOVE_Y) {
	for(ibin_x = 0; ibin_x <= ibin_x_selected; ibin_x++) {
	  for(ibin_y = ibin_y_selected; ibin_y < nbinsY()+ TrigHisto2D_v1::EXTRA_BINS; ibin_y++) {
	    ibin = ibin_y*(nbinsX()+ TrigHisto2D_v1::EXTRA_BINS) + ibin_x; // Find position in 1d data array
	    entries += acc_contents(*this).at(ibin);
	  }
	}
      }
      else if(cutType == TrigHistoCutType::ABOVE_X_ABOVE_Y) {   
	for(ibin_x = ibin_x_selected; ibin_x < nbinsX()+ TrigHisto2D_v1::EXTRA_BINS; ibin_x++) {
	  for(ibin_y = ibin_y_selected; ibin_y < nbinsY()+ TrigHisto2D_v1::EXTRA_BINS; ibin_y++) {
	    ibin = ibin_y*(nbinsX()+ TrigHisto2D_v1::EXTRA_BINS) + ibin_x; // Find position in 1d data array
	    entries += acc_contents(*this).at(ibin);
	  }
	}
      }
      else {
	return 0;
      }
    }// else of m_nbins!=0
    
    return entries;
  }
  
  unsigned int TrigHisto2D_v1::findBinX(float value) const {
    
    unsigned int ibin = 0;
    
    if(value < minX()) { // Underflow
      ibin = 0;
    }
    else if( !(value < maxX()) ) { // Overflow (catches NaN)
      ibin = nbinsX()+1;
    }
    else {
      while(value > (ibin*m_binWidthX+minX()) && ibin <= nbinsX()) { // None under/overflow from 1 to nbins
	ibin++;
      }
    }
    return ibin;
  }
  
  unsigned int TrigHisto2D_v1::findBinY(float value) const{
    
    unsigned int ibin = 0;
    
    if(value < minY()) { // Underflow
      ibin = 0;
    }
    else if( !(value < maxY()) ) { // Overflow (catches NaN)
      ibin = nbinsY()+1;
    }
    else {
      while(value > (ibin*m_binWidthY+minY()) && ibin <= nbinsY()) { // None under/overflow from 1 to nbins
	ibin++;
      }
    }
    return ibin;
  }
  
  void TrigHisto2D_v1::clear() {
    static Accessor< std::vector<float> > acc_contents( "contents" );
    
    for(std::vector<float>::iterator contents_iter = acc_contents(*this).begin(); contents_iter !=acc_contents(*this).end(); ++contents_iter)
      *contents_iter = 0;
  }
  
  void TrigHisto2D_v1::dump(){
    static Accessor< std::vector<float> > acc_contents( "contents" );
    
    std::cout<<"Dump contets vector of size:: "<<acc_contents(*this).size()<<std::endl;
    for( unsigned int i = 0 ; i < acc_contents(*this).size(); i++)
      std::cout<<acc_contents(*this).at(i)<<" ";
    std::cout<<std::endl;
    
    std::cout<<"NbinX:: "<< nbinsX()<<"\tRangeX:: ["<<minX()<<","<<maxX()<<"]"<<std::endl;
    std::cout<<"NbinY:: "<< nbinsY()<<"\tRangeY:: ["<<minY()<<","<<maxY()<<"]"<<std::endl;
    
    
    for(unsigned int iy = 0 ; iy < nbinsY() + TrigHisto2D_v1::EXTRA_BINS; iy++){
      std::cout<<std::endl;
      for(unsigned int ix = 0; ix < nbinsX() + TrigHisto2D_v1::EXTRA_BINS; ix++){
	std::cout<< acc_contents(*this).at(ix + iy*(nbinsX() + TrigHisto2D_v1::EXTRA_BINS))<<"\t";
      }
    }
    std::cout<<std::endl;
  }
}
