/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class for compact and robust histogram handling         //
// Author: Zdenko van Kesteren                             //
// Date  : 16 january 2006                                 //
//*********************************************************//

#ifndef HistHandler_h
#define HistHandler_h

#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"

namespace MuonCalib{
  class MuonFixedId;
}

class HistSetting;

/**
   @class HistHandler
    a class reducing the amount of code in order to make sets of TH1F/TH2F histograms which similar features.
    - usages are:

    HistHandler( ADCspectra, binX, Xmin, Xmax, histSetting );

    MuonFixedId FID = mdtCalibHit.identify();

    HistHandler.fill( FID, MdtCalibHit.adcCount );  //Fills ADC counts of the histogram belonging to station with id FID.

    or

    if( FID.stationNumberToFixedStationString( FID.stationName() ) == "BOL" )

       HistHandler.fill( "BOL", MdtCalibHit.adcCount );  //Fills ADC counts of the histogram belonging to all BOL stations.

    @author Zdenko.Van.Kesteren@cern.ch
 */

class HistHandler{
 public:
  typedef std::map<std::string, TH1F*>  histMap;   //!< typedef of a map with strings and 1D histograms
  typedef std::map<std::string, TH2F*>  hist2Map;  //!< typedef of a map with strings and 2D histograms
  
  HistHandler( std::string title, Int_t binning, Float_t min, Float_t max, const HistSetting* setting=0); //!< constructor 1D HistHandler
  HistHandler( std::string title, Int_t binningX, Float_t minX, Float_t maxX,                           
	       Int_t binningY, Float_t minY, Float_t maxY, const HistSetting* setting=0);                 //!< constructor 2D HistHandler
  ~HistHandler() {}; //!< destructor


  void fill( const MuonCalib::MuonFixedId& Id, Float_t entry );                  //!< fill TH1F (keyed with Id) with entry
  void fill( const MuonCalib::MuonFixedId& Id, Float_t entryX, Float_t entryY ); //!< fill TH2F (keyed with Id) with entryX and entryY
  void write();                                                                  //!< write histograms in HistHandler
  TH1F* Hist( const MuonCalib::MuonFixedId& Id) const;                           //!< create 1D histogram keyed with id 
  TH2F* Hist2( const MuonCalib::MuonFixedId& Id) const;                          //!< create 2D histogram keyed with id 


  void fill( std::string tag, Float_t entry );                                   //!< fill TH1F (keyed with title) with entry             
  void fill( std::string tag, Float_t entryX, Float_t entryY );                  //!< fill TH2F (keyed with title) with entryX and entryY
  void fill( std::string tag, Float_t entryX, Float_t entryY, Float_t weight );  //!< fill TH2F (keyed with title) with entryX and entryY
  TH1F* Hist(  const std::string tag ) const;                                    //!< create 1D histogram keyed with tag
  TH2F* Hist2( const std::string tag ) const;                                    //!< create 2D histogram keyed with tag

  double  maximum( std::string tag );

  private:
  std::string m_title;          //!< common title of the histograms in HistHandler
  Int_t       m_bin;            //!< common binsize of the histograms in HistHandler
  Float_t     m_min, m_max;     //!< common min/max value of the histograms in HistHandler
  Int_t       m_binY;           //!< common binsize of the (yaxis of the) histograms in HistHandler
  Float_t     m_minY, m_maxY;   //!< common min/max value of the (yaxis of the) histograms in HistHandler
  histMap     m_myMap;          //!< map of 1D histograms used internally
  hist2Map    m_my2Map;         //!< map of 2D histograms used internally
  const HistSetting* m_setting; //!< graphical settings of the histograms
};

#endif
