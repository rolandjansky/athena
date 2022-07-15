/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNGENTOOLS_ALIGNRESIDUALCALCULATOR_H
#define TRKALIGNGENTOOLS_ALIGNRESIDUALCALCULATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthContainers/DataVector.h"

#include "TrkAlignInterfaces/IAlignResidualCalculator.h"

/**
   @file AlignResidualCalculator.h
   @class AlignResidualCalculator

   @brief Tool for calculating residuals for measurements and scattering 
   centers and storing on AlignTSOS.

   @author Robert Harrington <roberth@bu.edu>
   @date 8/28/09
*/

class AtlasDetectorID;

namespace Trk {

  class AlignTrack;
  class IUpdator;
  class IResidualPullCalculator;
  class ResidualPull;
  class Track;
  class AlignTSOS;
  class TrackStateOnSurface;

  class AlignResidualCalculator :  public AthAlgTool, virtual public IAlignResidualCalculator {
 
  public:

    AlignResidualCalculator(const std::string& type, const std::string& name,
		    const IInterface* parent);
    virtual ~AlignResidualCalculator();
    
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** sets residuals for AlignTSOS on AlignTrack and returns total chi2 */
    virtual double setResiduals(DataVector<AlignTSOS>* alignTSOSColl, 
                                const Track* track) override;
    
    /** sets residuals for AlignTSOS on AlignTrack and returns total chi2 */
    virtual double setResiduals(AlignTrack* alignTrack,
                                const Track* track) override;
    
    /** sets residuals for AlignTSOS on AlignTrack and returns total chi2 */
    virtual double setResiduals(DataVector<AlignTSOS>::iterator firstAtsos,
                                DataVector<AlignTSOS>::iterator lastAtsos,
                                const Track* track, bool newTrack) override;

    /** returns chi2 summed over hits of a particular MeasurementType */
    virtual double chi2ForMeasType(int measType) const override
    { return m_chi2ForMeasType[measType]; }

  private:

    double setResidualsOnATSOS(AlignTSOS* atsos, const TrackStateOnSurface* tsos);
    void   accumulateScattering(const TrackStateOnSurface* tsos);
    const TrackStateOnSurface* getMatchingTSOS(const AlignTSOS* atsos, const Track* track);

    ToolHandle <IResidualPullCalculator> m_pullCalculator;
    ToolHandle <IUpdator>                m_updator;

    int m_resType; //<! residual type as defined in the Trk::AlignResidualType enum

    bool   m_includeScatterers;
    double m_qOverP;
    double m_previousQOverP;
   
    int    m_nDoF;

    double* m_chi2ForMeasType; 

    std::vector<const TrackStateOnSurface*> m_matchedTSOS;

  };

  
}

#endif // TRKALIGNGENTOOLS_ALIGNRESIDUALCALCULATOR_H
