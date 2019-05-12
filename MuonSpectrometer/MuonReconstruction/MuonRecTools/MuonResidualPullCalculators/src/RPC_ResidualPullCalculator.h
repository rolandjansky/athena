/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RPC_ResidualPullCalculator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// W. Liebig <http://consult.cern.ch/xwho/people/54608>
// muon version adapted from S. Fleischmann's SCT tool
///////////////////////////////////////////////////////////////////

#ifndef MUONRPC_RESIDUALPULLCALCULATOR_H
#define MUONRPC_RESIDUALPULLCALCULATOR_H

// interfaces
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

// edm
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ResidualPull.h"

namespace Muon {  class MuonIdHelperTool;}

namespace Muon 
{

  /** @brief RPC-specific tool to calculate hit residual and pull from a RIO_OnTrack/TrackParameter pair

      Uses Muon-specific info to code the strip type (measuring phi/eta)
      in the 2nd coordinate of the residual.
      
      @author  Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */  

  class RPC_ResidualPullCalculator : virtual public Trk::IResidualPullCalculator, public AthAlgTool
    {
    public:
      //! standard AlgTool constructor
      RPC_ResidualPullCalculator(const std::string&,const std::string&,const IInterface*);

      //! default destructor
      virtual ~RPC_ResidualPullCalculator ();
      
      //! standard Athena-Algorithm method
      virtual StatusCode initialize();
      //! standard Athena-Algorithm method
      virtual StatusCode finalize  ();

    /**
     @brief This function returns (creates!) a Trk::ResidualPull object, which contains the values
            of residual and pull for the given measurement and track state.

     The track state can be an unbiased one (which can be retrieved by the Trk::IUpdator),
     a biased one (which contains the measurement), or a truth state.
     The enum ResidualType must be set according to this, otherwise the pulls will be wrong.
     Residuals differ in all three cases; please be aware of this!
     */
    virtual const Trk::ResidualPull* residualPull(
        const Trk::MeasurementBase* measurement,
        const Trk::TrackParameters* trkPar,
        const Trk::ResidualPull::ResidualType,
        const Trk::TrackState::MeasurementType) const;

      // this prevents the compiler warning about hiding functions
      using IResidualPullCalculator::residualPull;

    /** This function is a light-weight version of the function above, designed for track fitters
     * where speed is critical. The user has to provide a std::vector of size 5, which gets
     * filled with the residuals.
     */
    virtual void residuals(
        std::vector<double>& residuals,
        const Trk::MeasurementBase* measurement,
        const Trk::TrackParameters* trkPar,
        const Trk::ResidualPull::ResidualType,
        const Trk::TrackState::MeasurementType) const;

    private:
      
    //! MuonIdHelper
    ToolHandle<Muon::MuonIdHelperTool>  m_idHelper;

      //! internal structuring: common method to calculate the hit pull.
      double calcPull(const double residual,
                      const double locMesCov,
                      const double locTrkCov,
                      const Trk::ResidualPull::ResidualType& ) const;

    }; 
} // end of namespace

#endif 
