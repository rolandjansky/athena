/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/BinnedEfficiency.h"
#include "MonteCarloReactUtils/EffExceptions.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TAxis.h>
#include <TDirectory.h>

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <vector>

using namespace MonteCarloReact;
using namespace std;

BinnedEfficiency::BinnedEfficiency( istream & ist,
				    const EffInfo* request) : Efficiency(), m_flowMode(Default)
{
  makeEfficiency( ist, request );
  makeEfficiencyHistogram( request );
}

BinnedEfficiency::BinnedEfficiency( const TH1 * h) : Efficiency(), m_flowMode(Default)
{
  // create the bin axis
  addTAxis(h->GetXaxis())  ;
  int nx  = h->GetNbinsX()+2;
  int ny  = 1, nz = 1 ;
  
  if (((string)h->ClassName()).find("TH2") != string::npos) {
    addTAxis(h->GetYaxis())  ;
    ny =  h->GetNbinsY()+2;
  }

  if (((string)h->ClassName()).find("TH3") != string::npos) {
    addTAxis(h->GetZaxis())  ;   
    nz =  h->GetNbinsZ()+2;
  }

  int binx = 0;
  int biny = 0;
  int binz = 0;
  vector<int> index ;
  index.push_back(0) ;
  if (getDimension()>1)  index.push_back(0) ;
  if (getDimension()>2)  index.push_back(0) ;
  // now fill them
  for( int i = 0; i < nx*ny*nz; ++i) {

    // eff_util and root convention about global bin number are not the same
    // exclude under and overflow bins from root
    binx = i%nx; 
    if (binx == 0 || binx > nx -2) continue ;
    index.at(0) = binx -1 ; // in eff_util the bin numbering starts at 0 

    if (getDimension() > 1 ) {
      biny = ((i-binx)/nx)%ny;
      if (biny == 0 || biny > ny -2) continue ;
      index.at(1) = biny -1 ; // in eff_util the bin numbering starts at 0 
    }

    if (getDimension() > 2 ) {
      binz = ((i-binx)/nx -biny)/ny;
      if (binz == 0 || binz > nz -2) continue ;
      index.at(2) = binz -1 ; // in eff_util the bin numbering starts at 0 
    }

    EffVal & bin = m_eff.at(getBinIndex(index));
    bin.setValue(h->GetBinContent( i ));
    bin.setErrorLow(h->GetBinError( i ));
    bin.setErrorHigh(h->GetBinError( i ));
    
  }
  m_spec.clear();

}

void BinnedEfficiency::defineBins(const vector< float > & loEdges)
{
  m_axisEdges.push_back(vector<float>(loEdges)) ;
  initEff() ;
}


void BinnedEfficiency::defineBins( int nbins, float xlo, float xhi)
{
  if( nbins <=0 ) 
    throw runtime_error("BinnedEfficiency::DefineBins ERROR: number of bins should be greater than 0") ;

  // make n bins opf equal size between lo and hi
  float xs = (xhi-xlo)/nbins;

  m_axisEdges.push_back(vector<float>()) ;
  for( int i = 0; i <= nbins; ++i) 
    m_axisEdges.back().push_back(xlo + ( i * xs )) ;
  initEff() ;
}


void BinnedEfficiency::addTAxis(const TAxis* axis) {
  m_axisEdges.push_back(vector<float>()) ;
  for( int i = 0; i < axis->GetNbins(); i++) 
    m_axisEdges.back().push_back(axis->GetBinLowEdge( i+1)) ;  
  m_axisEdges.back().push_back(axis->GetBinUpEdge( axis->GetNbins())) ;  
  initEff() ;
}

const EffVal& BinnedEfficiency::fill(const std::vector<float>& axis, bool passed) {
  int bin = getBinIndex(axis) ;
  if (bin <0 || bin >= int(m_eff.size())) {
    ostringstream st;
    st << "BinnedEfficiency::Fill ERROR: wrong bin number " << bin 
       << " maximum bin number is  " << m_eff.size() -1 ;
    throw runtime_error(st.str()) ;
  }
  EffVal& eff = m_eff.at(bin) ;
  eff.fill(passed) ;
  return eff ;
}


void BinnedEfficiency::setVal( int ibin, float val, float elo, float ehi)
{
  if( ibin < 0) throw BinnedEffUnderflow() ;
  if (ibin >= (int)m_eff.size() )  throw BinnedEffOverflow() ;
  EffVal & bin = m_eff.at( ibin );
  bin.setValue(val);
  bin.setErrorLow(elo);
  bin.setErrorHigh(ehi);
}

void BinnedEfficiency::setVal( const vector<int>& ibins, float val, float elo, float ehi)
{
  setVal( getBinIndex(ibins), val, elo, ehi);
}

void BinnedEfficiency::setVal(const vector<float>& axis, float val, float elo, float ehi)
{
  setVal( getBinIndex(axis), val, elo, ehi);
}

void BinnedEfficiency::setVal(int binno,  const EffVal& eff) {
  if(binno < 0) throw BinnedEffUnderflow() ;
  if (binno >= (int)m_eff.size() )  throw BinnedEffOverflow() ;
  m_eff.at( binno ) = eff ;
}

void BinnedEfficiency::setVal( int binno, const Value& eff) {
  if(binno < 0) throw BinnedEffUnderflow() ;
  if (binno >= (int)m_eff.size() )  throw BinnedEffOverflow() ;
  m_eff.at( binno ) = eff  ;
}

int BinnedEfficiency::getNbins(int n) const {
  if (n >= (int) m_axisEdges.size()) {
    ostringstream st  ;
    st << "BinnedEfficiency::GetNbins ERROR: requested axis with number " << n
       << " is not existing. Only " << m_axisEdges.size() << " axes exist" ;
    throw runtime_error(st.str()) ;	
  }

  if (n>=0) return (m_axisEdges.begin()+n)->size() - 1;

  int nbins = 1 ; 
  for (vector<vector<float> >::const_iterator it = m_axisEdges.begin(); 
       it != m_axisEdges.end(); it++) nbins *= it->size() - 1 ;
  return nbins ;  
}

int BinnedEfficiency::getBinIndex( const vector<float>& value) const {
  if (value.size() != m_axisEdges.size()) {
    ostringstream st  ;
    st << "BinnedEfficiency::BinIndex ERROR: size of the value " <<  value.size()
       << " is not equal to number of Axis " << m_axisEdges.size() ;
    throw runtime_error(st.str()) ;
  }
  vector<int> axis ;
  for (int i = 0; i < int(value.size()) ; i++) {
    int bin = 0 ;
    try { 
      bin =  findBin( i, *(value.begin()+i)) ;
    }
    catch (BinnedEffUnderflow& e) {
      cerr << "BinnedEfficiency::BinIndex ERROR: value " << *(value.begin()+i) 
	   << " smaller than than the first bin edge " <<  (m_axisEdges.begin()+i)->front() 
	   << " of axis " << i << endl;
      throw BinnedEffUnderflow();
    }
    catch (BinnedEffOverflow& e) {
      cerr << "BinnedEfficiency::BinIndex ERROR: value " << *(value.begin()+i) 
	   << " larger than than the latest bin edge " <<  (m_axisEdges.begin()+i)->back() 
	   << " of axis " << i << endl;      
      throw BinnedEffOverflow();
    }
    axis.push_back(bin) ;
  }
  
  return getBinIndex(axis);
}

int BinnedEfficiency::getBinIndex( const vector<int>& value) const {
  if (value.size() != m_axisEdges.size())  {
    ostringstream st  ;
    st << "BinnedEfficiency::BinIndex ERROR: size of the value " <<  value.size()
       << " is not equal to number of Axis " << m_axisEdges.size() ;
    throw runtime_error(st.str()) ;
  }

  int index = 0 ;
  int index_size = 1 ;
  for (int i = value.size()-1; i >= 0 ; i--) {
    index += index_size*(*(value.begin()+i)) ;
    index_size *=  (m_axisEdges.begin()+i)->size() -1 ;
  }
  return index ;
}

float BinnedEfficiency::getLowBinEdge(int axis_n, int n) const {
  if (axis_n >= (int)m_axisEdges.size()) {
    ostringstream st  ;
    st << "BinnedEfficiency::LowBinEdge ERROR: request axis with number " << axis_n
       << " is not existing. Only " << m_axisEdges.size() << " axis exist" ;
    throw runtime_error(st.str()) ;
  }

  if( n < 0 ) {
    ostringstream st ;
    st << "for " << axis_n << " axis" ;
    throw BinnedEffUnderflow(st.str());
  } else if( n >= getNbins(axis_n)) {
    ostringstream st ;
    st << "for " << axis_n << " axis" ;
    throw BinnedEffOverflow(st.str());
  }
  return (m_axisEdges.begin()+axis_n)->at(n) ;
}

float BinnedEfficiency::getHighBinEdge(int axis_n, int n) const {
  if (axis_n >= (int) m_axisEdges.size()) {
    ostringstream st  ;
    st << "BinnedEfficiency::HighBinEdge ERROR: request axis with number " << axis_n
       << " is not existing. Only " << m_axisEdges.size() << " axis exist" ;
    throw runtime_error( st.str()) ;	
  }

  if( n < 0 ) {
    ostringstream st ;
    st << "for " << axis_n << " axis" ;
    throw BinnedEffUnderflow(st.str());
  } else if( n >= getNbins(axis_n)) {
    ostringstream st ;
    st << "for " << axis_n << " axis" ;
    throw BinnedEffOverflow(st.str());
  }
  return (m_axisEdges.begin()+axis_n)->at(n+1) ;
}

const EffVal& BinnedEfficiency::getBinContent( int nx, int ny, int nz ) const {
  int dim = m_spec.getEffNVars();

  if( dim == 1 && (ny > 0 || nz > 0)) 
    throw BinnedBadDimension();
  else if( dim == 2 && nz > 0 ) 
    throw BinnedBadDimension();
  else if( dim >3 ) 
    throw BinnedBadDimension(); 

  if( nx < 0 ) throw BinnedEffUnderflow(" for x axis");
  else if( nx >= getNbins(0)) throw BinnedEffOverflow(" for x axis");

  vector<int> bins ;
  bins.push_back(nx) ;
  if (dim==1) return m_eff.at(getBinIndex(bins));

  if( ny < 0 ) throw BinnedEffUnderflow(" for y axis");
  else if( ny >= getNbins(1)) throw BinnedEffOverflow(" for y axis");

  bins.push_back(ny) ;
  if (dim==2) return m_eff.at(getBinIndex(bins));

  if( nz < 0 ) throw BinnedEffUnderflow(" for z axis");
  else if( nz >= getNbins(2)) throw BinnedEffOverflow(" for z axis");

  bins.push_back(nz) ;
  return m_eff.at(getBinIndex(bins));
}


EffVal BinnedEfficiency::getEff(float x) const {
  vector<float> bins ;
  bins.push_back(x) ;
  return getEff(bins) ;
}

EffVal BinnedEfficiency::getEff(float x, float y) const {
  vector<float> bins ;
  bins.push_back(x) ;
  bins.push_back(y) ;
  return getEff(bins) ;
}

EffVal BinnedEfficiency::getEff(float x, float y, float z) const {
  vector<float> bins ;
  bins.push_back(x) ;
  bins.push_back(y) ;
  bins.push_back(z) ;
  return getEff(bins) ;
}

EffVal BinnedEfficiency::getEff( const vector<float>& value ) const {
  int dim = m_spec.getEffNVars();
  if( dim != getDimension()) throw BinnedBadDimension();
  int index = getBinIndex(value);
  return m_eff.at( index );
}


void BinnedEfficiency::clear() {
  m_eff.clear() ;
  m_axisEdges.clear() ;
  Efficiency::clear() ;
}

TH1F* BinnedEfficiency::getProjection(int axis_number)  const {

  int nx = (m_axisEdges.begin()+axis_number)->size() -1 ;
  std::vector<float> xedges (nx + 1, 0); 
  if( nx == 0)  throw  runtime_error("BinnedEfficiency::projection: number of bins for axis is 0!") ;
  for( int i = 0; i <=nx;++i) xedges[i]=m_axisEdges.at(axis_number).at(i);

  const EffInfo& info =  getInfo() ;
  string histname = info.getEffName() + "_" + info.getObjType() ;
  if (info.getObjQuality()!="UNDEFINED") histname += "_" + info.getObjQuality() ;
  histname += "_" + info.getEffVarNames().at(axis_number);

  if (info.getTriggerVersion() != "UNDEFINED")
    histname += "_trig_" + info.getTriggerVersion() ;
  else if (info.getTriggerVersionHigh() != "UNDEFINED" || info.getTriggerVersionLow() != "UNDEFINED") {
    histname += "_trig" ;
    if (info.getTriggerVersionLow() != "UNDEFINED")
      histname += "_" + info.getTriggerVersionLow() ;
    if (info.getTriggerVersionHigh() != "UNDEFINED")
      histname += "_" + info.getTriggerVersionHigh() ;
  }

  TH1F *hist = new TH1F(histname.c_str(),histname.c_str(),nx,xedges.data());
  hist->GetXaxis()->SetTitle(m_spec.getEffVarNames().at(axis_number).c_str());

  vector<EffVal> effval ;
  effval.resize(nx) ;
  for (vector< EffVal >::const_iterator it =  m_eff.begin(); it != m_eff.end(); it++) {
    // find bin number for requested axis
    int x = -1 ;
    int index = it - m_eff.begin() ;
    for (vector<vector<float> >::const_iterator jt =  m_axisEdges.begin();
	 jt != m_axisEdges.end() ; jt++) {          
      int n = 1 ;
      for (vector<vector<float> >::const_iterator nt = jt+1 ; nt != m_axisEdges.end() ; nt++) 
	n *= nt->size() - 1 ; 
      x =  index/n  ;
      index = index%n ;
      if (jt - m_axisEdges.begin() == axis_number) break ;
    }
    effval.at(x) += *it ; 
  }
  
  for (int j = 0; j < nx; j++) {
    const EffVal & val = effval.at(j) ;
    if (val.getTotalEvents()>0) {
      hist->SetBinContent(j+1,val.getValue());
      hist->SetBinError(j+1,(fabs(val.getErrorHigh())+fabs(val.getErrorLow()))/2);
    }
  }

  return hist ;
}

// the following method creates
// a histogram with the efficiency read from the mcr file.
void BinnedEfficiency::makeEfficiencyHistogram(const EffInfo* request, 
					       const string& hname, bool useCurrentDir) const
{

  if (getDimension()>3 || getDimension() ==0)  return ;
  
  int nx = m_axisEdges.begin()->size() -1 ;
  int ny = m_axisEdges.size() >= 2 ? (m_axisEdges.begin()+1)->size() -1 : 0 ;
  int nz = m_axisEdges.size() == 3 ? (m_axisEdges.begin()+2)->size() -1 : 0 ;

  // bin boundaries
  std::vector<float> xedges (nx + 1, 0); 
  std::vector<float> yedges (ny + 1, 0); 
  std::vector<float> zedges (nz + 1, 0); 
  if( nx >0) {
    for( int i = 0; i <=nx;++i) xedges[i]=m_axisEdges.at(0).at(i);
  }
  if( ny >0) {
    for( int i = 0; i <=ny;++i) yedges[i]=m_axisEdges.at(1).at(i);
  }
  if( nz >0) {
    for( int i = 0; i <=nz;++i) zedges[i]=m_axisEdges.at(2).at(i);
  }
  // now do data itself
  int dimension = m_spec.getEffNVars();
  if( dimension < 1 ) 
    return;
  if( dimension > 3)
    dimension = 3;
  char histname[200];
  if (!hname.empty()) {
    strncpy(histname,hname.c_str(),199);
  } else if (request) {
    // create histogram name from mcr file name
    strncpy(histname,request->makeFileName().c_str(),199);
    // cut off the file name extension (usually ".mcr")
    char *extension=strstr(histname,".");
    if (extension) strcpy(extension,"_");
    // append relativeto description
    strncat(histname,request->getObjRelativeTo().c_str(),199-strlen(histname));
  } else {
    strcpy(histname,"hist_BinnedEfficiency");
  }
  // create this histogram in root directory so it is easily accessible
  // for other CAF processors
  TDirectory* currdir=gDirectory;
  if (useCurrentDir) gDirectory->cd() ;
  else gDirectory->cd("root:/");
  // check whether histogram exists already (and don't attempt to recreate it)
  if (gDirectory->Get(histname)) {
    gDirectory=currdir;
    return;
  }
  // create the histogram
  if( dimension == 1) {
    TH1F *hist1 = new TH1F(histname,histname,nx,xedges.data());
    hist1->GetXaxis()->SetTitle(m_spec.getEffVarNames()[0].c_str());
    for (int i=0; i<nx; ++i) {
      const EffVal & val = getBinContent(i) ;
      hist1->SetBinContent(i+1,val.getValue());
      hist1->SetBinError(i+1,(fabs(val.getErrorHigh()) + fabs(val.getErrorLow()))/2);
    }
  } else if ( dimension == 2) {
    TH2F *hist2= new TH2F(histname,histname,nx,xedges.data(),ny,yedges.data());
    hist2->GetXaxis()->SetTitle(m_spec.getEffVarNames()[0].c_str());
    hist2->GetYaxis()->SetTitle(m_spec.getEffVarNames()[1].c_str());
    for (int i=0; i<nx; ++i) {
      for (int j=0; j<ny; ++j) {
	const EffVal & val = getBinContent(i,j) ;
	hist2->SetBinContent(i+1,j+1,val.getValue());
	hist2->SetBinError(i+1,j+1,(fabs(val.getErrorHigh()) + fabs(val.getErrorLow()))/2);
      }
    }
  } else if( dimension == 3) {
    TH3F *hist3= new TH3F(histname,histname,nx,xedges.data(),ny,yedges.data(),nz,zedges.data());
    hist3->GetXaxis()->SetTitle(m_spec.getEffVarNames()[0].c_str());
    hist3->GetYaxis()->SetTitle(m_spec.getEffVarNames()[1].c_str());
    hist3->GetZaxis()->SetTitle(m_spec.getEffVarNames()[2].c_str());
    for (int i=0; i<nx; ++i) {
      for (int j=0; j<ny; ++j) {
	for (int k=0; k<nz; ++k) {
	  const EffVal & val = getBinContent(i,j,k) ;
	  hist3->SetBinContent(i+1,j+1,k+1,val.getValue());
	  hist3->SetBinError(i+1,j+1,k+1,(fabs(val.getErrorHigh()) + fabs(val.getErrorLow()))/2);
	}
      }
    }
  }
  gDirectory=currdir;

}


int BinnedEfficiency::findBin( const vector< float >& loEdges, float x, FindBinMode mode) const
{
  if (loEdges.size() <= 1)
    throw runtime_error("BinnedEfficiency::FindBin ERROR: number of edges should be greater than 1") ;

  if( x < loEdges.front()) {
    if (m_flowMode != IgnoreUnderflowOverflow && 
	m_flowMode != IgnoreUnderflow )
      throw BinnedEffUnderflow();
    else return 0 ;
  }

  if (mode == IncludeLowBound && x == loEdges.front()) return 0 ;

  vector< float >::const_iterator it = lower_bound(loEdges.begin(), loEdges.end(), x)  ;

  if ( it == loEdges.end() ) {
    if (m_flowMode != IgnoreUnderflowOverflow && 
	m_flowMode != IgnoreOverflow ) 
      throw BinnedEffOverflow();
    else return loEdges.size() - 1 ;
  }

  if (mode == IncludeLowBound && x == *it) return  it - loEdges.begin() ;
  if (mode == IncludeHighBound && x == *it) return  it - loEdges.begin() - 1 ;
  
  return  it == loEdges.begin() ? 0 : it - loEdges.begin()  -1 ;
}


bool BinnedEfficiency::parseInputLine( const string & key, const vector< string> & line)
{
  if( key.find("BinEdges") != string::npos ) {
    int npar = line.size();
    m_axisEdges.push_back(vector<float>()) ;
    for( int i =0; i < npar; ++i) 
      m_axisEdges.back().push_back(atof( (line[i]).c_str())) ;
    initEff() ;
    return true ;
  }
  
  if( key =="BinVal" ) {
    int npar = line.size();
    int dim = getDimension() ;
    if( dim!= npar-3 && dim != npar-5 ) 
      throw runtime_error("MonteCarloReactUtils::BinnedEfficiency::ParseInputLine ERROR: number of fields in the line does not corresponds to the efficiency dimension") ;
    if( m_eff.size() == 0) 
      throw runtime_error("MonteCarloReactUtils::BinnedEfficiency::ParseInputLine ERROR: number of efficiency bin is 0. Check mcr file structure and the bin  edges information.") ;    
    float val,elo,ehi;
    vector<int> bins ;
    for (int i=0; i< dim; i++) {
      bins.push_back(atoi( (line[i]).c_str()));
    }    
    int n=0, n0=0;
    if (dim == npar-5) {
      n0 = atoi( line[dim+3].c_str() ) ;
      n = atoi( line[dim+4].c_str() ) ;
    } 
    // during reading the number of event information has a priority on the caclulated values
    // the efficiency value and errors will be recalculated internally in EffVal object 
    // val, ehi, elo information will be used only if number of events is absent
    if (n0>0)  setVal( getBinIndex(bins), EffVal(n,n0)) ;
    else {
      val = atof( line[dim].c_str() );
      elo = atof( line[dim+1].c_str() );
      ehi = atof( line[dim+2].c_str() );
      setVal( bins, val, elo, ehi);
    }
    return true ;
  }
  return false;
}

void BinnedEfficiency::stream( ostream & os) const
{
  // first do bin edges
  for (vector<vector<float> >::const_iterator it =  m_axisEdges.begin();
       it != m_axisEdges.end() ; it++) {
    os << "BinEdges" << it - m_axisEdges.begin() +1 << " : " ;
    for (vector<float>::const_iterator jt = it->begin(); jt != it->end() ; jt++) 
      os << *jt << " " ;
    os << endl;
  }

  // now do data itself
  for(vector< EffVal >::const_iterator it = m_eff.begin(); 
      it != m_eff.end() ; it++) {
    // ignore bins with null information
    //if (it->getValue() == 0 && it->getErrorLow() == 0 && it->getErrorHigh() == 0) continue ;

    // save bin number
    os << "BinVal : " ;
    int index = it - m_eff.begin() ;
    for (vector<vector<float> >::const_iterator jt =  m_axisEdges.begin();
	 jt != m_axisEdges.end() ; jt++) {          
      int n = 1 ;
      for (vector<vector<float> >::const_iterator nt = jt+1 ; nt != m_axisEdges.end() ; nt++) 
	n *= nt->size() - 1 ; 
      int x =  index/n  ;
      index = index%n ;
      os << x << " " ;
    }
    // save efficiency value and errors
    os << "    " << it->getValue() << " " << it->getErrorLow() << " " << it->getErrorHigh() ;
    // save also number of events information. Will have a priority on the (val,ehi,elo) values during reading
    //os << " " << it->getTotalEvents() << " " << it->getPassedEvents() ; 
    os << endl;
  }
}


void BinnedEfficiency::initEff() {
  m_eff.resize(getNbins() ) ;  
}

