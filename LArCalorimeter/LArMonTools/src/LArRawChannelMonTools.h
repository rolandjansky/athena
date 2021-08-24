/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONTOOLS_LARRAWCHANNELMONTOOLS_H
#define LARMONTOOLS_LARRAWCHANNELMONTOOLS_H

#include "CLHEP/Units/SystemOfUnits.h"

#include "TH1.h"
#include "TH2.h"

#include "LWHists/LWHist1D.h"
#include "LWHists/LWHist2D.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LArRawEvent/LArRawChannel.h"

#include <deque>
#include <map>
#include <string>
#include <vector>

class LArOnlineID;

// --- Some tools to help with LAr Description --- 
namespace LArMonTools {

  // --- Detector Navigation ---
  enum Detector {
    EMBA  = 0,
    EMBC  = 1,
    EMECA = 2,
    EMECC = 3,
    HECA  = 4,
    HECC  = 5,
    FCALA = 6,
    FCALC = 7,
    UNDEF = 8
  };

  enum Sampling {
    presampler     = 0,
    sampling_one   = 1,
    sampling_two   = 2,
    sampling_three = 3
  };

  // --- Histo Navigation ---
  enum RawChHisto {
    occ_h                                = 0,
    sig_h                                = 1,
    pos_noise_h                          = 2,
    neg_noise_h                          = 3,
    time_h                               = 4,
    quality_h                            = 5,
    burst_h                              = 6,
    noise_fraction_h                     = 7,
    noise_fraction_Neg_h                 = 8,
    noise_fraction_NoLArNoisyRO_h        = 9,
    pedestal_evolution_h                 = 10,
    sumE_vs_BCID_h                       = 11,
    mean_feb_time_h                      = 12,
    quality_nChannel_h                   = 13,
    quality_burst_h                      = 14,
    quality_nChannel_burst_h             = 15,
    E_v_bc_h                             = 16,
    noise_fraction_TimeVetoLArNoisyRO_h  = 17,
    timeVetoBurst_h                      = 18,
    noise_fraction_W_h                   = 19,//Modif done by Camincher C.
    gain_h                               = 20
  };

  std::string detector_str( const Detector& );
  std::string sampling_str( const Sampling& d );

  std::vector<double> detector_superslot_axis( Detector const& ,
					       const LArOnlineID * );

  template <class Container>
    std::vector<double> sparse_bins_vector(Container const &bins,
					   typename Container::value_type min=0,
					   typename Container::value_type max=0,
					   double const &bin_size = 1.0 );

  // --- Return number of slots in the feedthroughs in given detector:
  // --- 14 in the barrel (EMB?), 15 in the Endcaps (EMEC?, HEC?, FCAL?)
  int n_slots_in_feedthroughs( const LArMonTools::Detector& );


  // --- Return number of feedthroughs in given detector:
  // --- 32 in the barrel (EMB?), 25 in the Endcaps (EMEC?, HEC?, FCAL?)
  int n_feedthroughs( const LArMonTools::Detector& );

  //! Returns 1 for A side, -1 for C side, 0 as error
  /*!
    Tool to get the proper eta sign for the side of the detector from
    the Detector enum.
    \param detector Detector enum (EMBA, EMBC, etc. )
   */
  int pos_neg( const Detector &detector );

  const int n_slots_in_barrel = 14; //!< slots per feedthrough in barrel
  const int n_slots_in_endcap = 15; //!< slots per feedthrough in endcap
  const int n_feedthroughs_in_barrel = 32; //!< feedthroughs in barrel
  const int n_feedthroughs_in_endcap = 25; //!< feedthroughs in endcap
  
  struct Region;
  bool operator< ( const Region &, const Region& );
  struct Region {

    friend bool operator< ( const Region &, const Region& );

    double lower; //!< lower edge
    double upper; //!< upper edge
    double size;  //!< bin/cell/channel size

  };

  //! create eta and phi axis for different subdetectors of LArg Calo
  class lar_axis_maker {

  public:
    lar_axis_maker();

    lar_axis_maker( const std::map<Detector, std::map<Sampling, std::deque<Region> > >& );

    ~lar_axis_maker();

    const std::vector<double> operator() (const Detector&, const Sampling&);

    const std::map<Detector, std::map<Sampling, std::deque<Region> > > & regions() const;

    void regions( const std::map<Detector, std::map<Sampling, std::deque<Region> > >& ); 

  protected:
    std::map<Detector, std::map<Sampling, std::deque<Region> > >  m_regions; //!< regions
    std::map<Detector, std::map<Sampling, std::vector<double> > > m_bins;

    //! takes edges and bin sizes and creates bin vector.
    void generate();

  };

  //! virtual base class to define bhaviour of histogram proxy
  /*!
    This class defines the behavour of a proxy to ROOT's histograms. The
    motivation for the proxy is to allow one stl container to interact with
    TH1, TH2, and TH3 histograms. The class is implemented through HistoBase.
   */
  class IHistoProxyBase {

  public: 
    IHistoProxyBase() {}

    virtual ~IHistoProxyBase() {}

    virtual void Fill( double x ) = 0;
    virtual void Fill( double x, double y ) = 0;
    virtual void Fill( double x, double y, double z ) = 0;
    virtual void Fill( double x, double y, double z, double w ) = 0;

    virtual void Scale( double s = 1. ) = 0;
    virtual void Reset( ) = 0;

  };

  //! implementation of the behaviour defined by IHistoProxyBase
  /*!
    This class holds a pointer histogram (without claiming ownership)
    and calles it's Fill and Scale functions. Template specializations
    exist for the case of TH1 and TH2 histograms, since TH1 does not
    define a fill function with three doubles as arguments.
   */
  template <class HISTOGRAM>
    class HistoProxy : public IHistoProxyBase {

    private:
    HISTOGRAM * m_h_ptr;

    public:
    HistoProxy( )
      : m_h_ptr( 0 ) {}

    HistoProxy( HISTOGRAM * h_ptr )
      : m_h_ptr( h_ptr ) {}

    virtual void Fill( double x ) { m_h_ptr->Fill( x ); }
    virtual void Fill( double x, double y ) { m_h_ptr->Fill( x, y ); }
    virtual void Fill( double x, double y, double z ) { m_h_ptr->Fill( x, y, z ); }
    virtual void Fill( double x, double y, double z, double w ) { m_h_ptr->Fill( x, y, z, w ); }

    virtual void Scale( double s = 1. ) { m_h_ptr->Scale( s ); }
    virtual void Reset( ){ m_h_ptr->Reset( ); }

  };


  //The LWHistoProxy implementations:
  class LWHistoProxy1D : public IHistoProxyBase {
    private:
    LWHist1D * m_h_ptr;
    public:
    LWHistoProxy1D( ) : m_h_ptr( 0 ) {}
    LWHistoProxy1D( LWHist1D * h_ptr ) : m_h_ptr( h_ptr ) {}
    virtual void Fill( double x ) { m_h_ptr->Fill( x );}
    virtual void Fill( double x, double y ) { m_h_ptr->Fill( x, y ); }
    virtual void Fill( double, double, double) {}
    virtual void Fill( double, double,  double, double ) {}
    virtual void Scale( double s = 1. ) { m_h_ptr->scaleContentsAndErrors( s ); }
    virtual void Reset(){ m_h_ptr->Reset(); }
  };
  class LWHistoProxy2D : public IHistoProxyBase {
    private:
    LWHist2D * m_h_ptr;
    public:
    LWHistoProxy2D( ) : m_h_ptr( 0 ) {}
    LWHistoProxy2D( LWHist2D * h_ptr ) : m_h_ptr( h_ptr ) {}
    virtual void Fill( double ) {}
    virtual void Fill( double x, double y ) { m_h_ptr->Fill( x, y ); }
    virtual void Fill( double x, double y, double z ) { m_h_ptr->Fill( x, y, z ); }
    virtual void Fill( double, double, double, double ) {  }
    virtual void Scale( double s = 1.) { m_h_ptr->scaleContentsAndErrors( s ); }
    virtual void Reset(){ m_h_ptr->Reset(); }
  };
  class LWHistoProxyProfile : public IHistoProxyBase {
    private:
    TProfile_LW * m_h_ptr;
    public:
    LWHistoProxyProfile( ) : m_h_ptr( 0 ) {}
    LWHistoProxyProfile( TProfile_LW* h_ptr ) : m_h_ptr( h_ptr ) {}
    virtual void Fill( double ) {}
    virtual void Fill( double x, double y ) { m_h_ptr->Fill( x, y ); }
    virtual void Fill( double x, double y, double z ) { m_h_ptr->Fill( x, y, z ); }
    virtual void Fill( double, double, double, double ) {  }
    virtual void Scale( double) { }
    virtual void Reset(){ m_h_ptr->Reset(); }
  };
  class LWHistoProxyProfile2D : public IHistoProxyBase {
    private:
    TProfile2D_LW * m_h_ptr;
    public:
    LWHistoProxyProfile2D( ) : m_h_ptr( 0 ) {}
    LWHistoProxyProfile2D( TProfile2D_LW* h_ptr ) : m_h_ptr( h_ptr ) {}
    virtual void Fill( double ) {}
    virtual void Fill( double, double ) { }
    virtual void Fill( double x, double y, double z ) { m_h_ptr->Fill( x, y, z ); }
    virtual void Fill( double x, double y, double z, double w ) { m_h_ptr->Fill( x, y,z,w ); }
    virtual void Scale( double) { }
    virtual void Reset(){ m_h_ptr->Reset(); }
  };

  class SelectionContext{

  public:
    SelectionContext();
    SelectionContext( const double& );

    virtual ~SelectionContext();

    // Getters:
    int positive_noise() const;
    int negative_noise() const;
    inline double quality() const { return m_quality; }
    double energy() const;




    // Setters:
    void positive_noise( const int& );
    void negative_noise( const int& );
    void quality( const double& );
    void energy( const double& );

  private:
    int m_positive_noise;
    int m_negative_noise;

    double m_quality;
    double m_energy;

  };


  class QualitySelector {
    
  public:

    QualitySelector();

    //! Select a channel based on criteria given by _pSelectionContext
    /*!
      Selects a LArRawChannel based on q-factor.
      @See SelectionContext
     */
    inline bool Select( const LArRawChannel& a ) {
      return m_pSelectionContext &&
	a.energy() > 0.1 * CLHEP::MeV &&
	a.quality() > m_pSelectionContext->quality();
    }



    //! Getter function for context selection is based upon
    const SelectionContext * selectionContext() const;

    //! Setter function for context selection is based upon
    void selectionContext( const SelectionContext * pSelectionContext );

    // should be protected, but thanks to a compiler bug can't be...
    ~QualitySelector();


  private:

    //! Context holds information to select channels. Must be set properly for each channel
    const SelectionContext * m_pSelectionContext;

  };


  class MeanCalculator {

  public:
    MeanCalculator();
    ~MeanCalculator();

    double& Add( const double & x, const double & w = 1. );

    double result() const;
    void result( const double & );

  private:
    double m_moment0;
    double m_moment1;
    double m_mean;
  };


  class ResolutionCalculator {

  public:
    ResolutionCalculator();
    ResolutionCalculator( const double &, const double & );
    ~ResolutionCalculator();

    double operator() ( const double& ) const;

  private:

    double m_a;
    double m_b;

  };

}


#endif // LAR_RAW_CHANNEL_MON_TOOLS_H
