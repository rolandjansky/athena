/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_BINNEDEFFICIENCY_H
#define MONTECARLOREACTUTILS_BINNEDEFFICIENCY_H

#include "MonteCarloReactUtils/EffExceptions.h"
#include "MonteCarloReactUtils/Efficiency.h"
#include "MonteCarloReactUtils/EffVal.h"

#include <vector>
#include <string>
#include <iostream>

class TH1;
class TH1F;
class TAxis ;

namespace MonteCarloReact {
  
  
  /* BinnedEfficiency class is a class to used by MonteCarloReact package to keep histogram
     with any values (not only efficiency). It also can be used as a histogram class
     (the number of event in each bin could be increased one by one).
     Any number of dimentions with uniform or non-uniform bins are supported.
   */
  class BinnedEfficiency : public Efficiency {
    
  public:

    /* Different Mode for the treatment of underflow and overflow bins */
    enum Mode {Default, IgnoreUnderflow, IgnoreOverflow, IgnoreUnderflowOverflow} ;
    enum FindBinMode {IncludeLowBound, IncludeHighBound} ;


    /* Default constructor */ 
    BinnedEfficiency() : Efficiency(), m_flowMode(Default) {}

    /* Constructor using existing EffInfo inherited from base class*/
    BinnedEfficiency( std::istream & ist, const EffInfo* request=0 );

    /* Initialise efficiency with Root TH1, TH2 or TH3 histograms */
    BinnedEfficiency( const TH1 * h);


    /* add bin axis with non-uniform bin structure */
    void defineBins( const std::vector<float>& edges) ;

    /* add bin axis with uniform bins */
    void defineBins( int nbins, float xlo, float xhi) ;

    
    /* add bins from ROOT TAxis */
    void addTAxis(const TAxis* axis) ;

    
    /* increase event counter by one in the bins with axis values specified by the vector
       passed flag should be true for events which pass the selection or fire the trigger
       The return value is the efficiency object in the current bin */
    const EffVal& fill(const std::vector<float>& axis, bool passed) ;

    
    /* set bin content using internal bin number */
    void setVal( int ibin, float val, float elo, float ehi);

    /* set bin content using bins  number */
    void setVal(const std::vector<int>& ibins, float val, float elo, float ehi);

    /* set bin content using axis values */
    void setVal(const std::vector<float>& axis, float val, float elo, float ehi);

    /* set bin content from EffVal object */
    void setVal( int binno, const EffVal& eff) ;

    /* set bin content from Value object */
    void setVal(int binno, const Value& eff) ;

    
    /* number of bins of axis number n (starting from 0). 
       Default value -1 corresponds to the total number of bins */
    int getNbins(int n=-1) const ;

    /* get internal bin index using  axis values */
    int getBinIndex( const std::vector<float>& value) const;

    /* return internal bin index using axis bin index. 
       for example, for 3d index = z + nz*y + nz*ny*x */
    int getBinIndex( const std::vector<int>& value) const ;

    /* get bin edges for axis with number axis_n */
    float getLowBinEdge( int axis_n, int n) const ;
    float getHighBinEdge( int axis_n, int n) const ;
    
    /* get bin content by bin index, bin index started at 0 and finished at nbins-1
       bin index >= number of nbins throw overflow exeption */
    const EffVal&  getBinContent( int nx, int ny =-1, int nz = -1 ) const ;

    /* get bin content by axes value
       1D only */
    EffVal getEff(float x) const;

    /* Get Efficiency 2D only */
    EffVal getEff(float x, float y) const;

    /* Get Efficiency 3D only */
    EffVal getEff(float x, float y, float z) const;

    /* Get Efficiency for any dimension */
    EffVal getEff( const std::vector<float>&  value) const;

    
    /* remove all bin axis and bin contents information  */
    void clear() ;


    /* create 1D projection of nD histogram */
    TH1F* getProjection(int axis_number)  const ;

    /* create root histogram */
    void makeEfficiencyHistogram(const EffInfo* request, 
				 const std::string& hname="", bool useCurrentDir=false ) const;

    /* int find bin index (number) using axis value */ 
    int findBin( const std::vector< float >& loEdges, float x,
                 FindBinMode mode = IncludeLowBound) const; 

    /* int find bin index (number) using axis value and number  */
    int findBin(int axis_n, float x, FindBinMode mode = IncludeHighBound) const {
      return findBin(m_axisEdges.at(axis_n), x, mode);} 

    /* return historgram dimension (number of axis) */
    int getDimension() const { return m_axisEdges.size();} 

    /* Validate the EffInfo */
    bool validateInput() const { return m_eff.size() >0 ; } 

    /* if mode set to IgopreUnderflow will substitute the underflow with the lowest bin value
       if mode set to  IgnoreOverflow will substitute the overrflow with the highest bin value 
       IgnoreUnderflowOverflow will substitue both underflow and overflow */
    void setExeptionMode(Mode mode) const {m_flowMode = mode ;}
    
  protected:
    /* Read in Line of the efficiency file */
    bool m_parseInputLine( const std::string &key, const std::vector< std::string > & line);
    
  private:

    std::vector<std::vector< float > > m_axisEdges ; // The order of axes is x, y, z ...
    std::vector< EffVal > m_eff;

    mutable Mode m_flowMode ;

    /* write efficiency information to the output stream */
    void m_stream( std::ostream &) const;

    /* initialize efficiencies */
    void m_initEff() ;
  };
}
#endif
