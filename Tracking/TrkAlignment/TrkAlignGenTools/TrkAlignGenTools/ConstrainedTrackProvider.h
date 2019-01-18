/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_CONSTRAINEDTRACKPROVIDER_H
#define TRKALIGNGENTOOLS_CONSTRAINEDTRACKPROVIDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "muonEvent/Muon.h"
#include "TrkAlignInterfaces/ITrackCollectionProvider.h"
#include "TrkFitterUtils/FitterTypes.h"  



class TFile;
class TH2F;
/**
  ConstrainedTrackProvider
  
  Will provide  a track coolection with momentum conatraint applied.  A 2d histogram read  and 
  corrections (delta) are extracted as function of eta and phi.
  The momentum is scaled as 
  corrected QoverP = original QoverP * (1.+ charge *pt *delta );

  The strength of eth constraint can be varied as well.
*/
namespace Trk {
  class IGlobalTrackFitter;

  class ConstrainedTrackProvider : virtual public ITrackCollectionProvider, public AthAlgTool {

  public:
    ConstrainedTrackProvider(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~ConstrainedTrackProvider();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    virtual StatusCode trackCollection(const TrackCollection*& tracks);

    virtual void printSummary();

  private :

    void  getCorrectedValues_P(const Trk::Perigee* mp, double& correctedQoverP,double& correctedQoverPError);
    void  getCorrectedValues_d0(const Trk::Perigee* mp, double& corrected_d0,double& corrected_d0Error);
    void  getCorrectedValues_z0(const Trk::Perigee* mp, double& corrected_z0,double& corrected_z0Error);
    bool  passTrackSelection(const Trk::Track *track);

    ToolHandle<IGlobalTrackFitter>        m_trackFitter;     //!< normal track fitter 
 
    RunOutlierRemoval     m_runOutlierRemoval;         //!< run outlier removal in the GX2 fitter 
    int           m_nRetrievalErrors;          //!< number of retrieval errors at the beginning of the job
    int           m_maxRetrievalErrors;        //!< maximum allowed number of retrieval errors at the beginning of the job (-1 for infinite)
    bool          m_useConstrainedTrkOnly;
    int           m_constrainedTracks;
    int           m_passconstrainedRequirements;
    int           m_unconstrainedTracks;

    bool          m_useConstraintError;
    double        m_reduceConstraintUncertainty;
    double        m_deltaScaling;

    int           m_minPIXHits;
    int           m_minSCTHits;
    int           m_minTRTHits;
    double        m_maxd0;
    double        m_maxz0;
    double        m_minPt;
    double        m_maxPt;
    

    std::string     m_inputTrackCollection;


    bool            m_CorrectMomentum;
    std::string     m_constraintFileName_P; 
    TFile*          m_constraintInputFile_P;
    std::string     m_constraintHistName_P; 
    TH2F*           m_etaphiMap_P;
      // Corrections expected to be in GeV-1

    bool            m_CorrectD0;
    std::string     m_constraintFileName_d0; 
    TFile*          m_constraintInputFile_d0;
    std::string     m_constraintHistName_d0; 
    TH2F*           m_etaphiMap_d0;
      // Corrections expected to be in mm

    bool            m_CorrectZ0;
    std::string     m_constraintFileName_z0; 
    TFile*          m_constraintInputFile_z0;
    std::string     m_constraintHistName_z0; 
    TH2F*           m_etaphiMap_z0;
      // Corrections expected to be in mm
    bool m_scalepmaptogev;

  }; // end class

} // end namespace

#endif // TRKALIGNGENTOOLS_CONSTRAINEDTRACKPROVIDER_H
