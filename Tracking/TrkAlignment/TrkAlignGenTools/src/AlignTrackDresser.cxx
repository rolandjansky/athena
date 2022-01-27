/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "TrkTrack/Track.h"

#include "TrkAlignInterfaces/IDerivCalcTool.h"
#include "TrkAlignInterfaces/IFillNtupleTool.h"
#include "TrkAlignEvent/AlignTSOS.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignTrack.h"

#include "TrkAlignGenTools/AlignTrackDresser.h"

namespace Trk {

  //________________________________________________________________________
  AlignTrackDresser::AlignTrackDresser(const std::string & type, const std::string & name, const IInterface * parent)
    : AthAlgTool(type,name,parent)
    , m_derivCalcTool("Trk::AnalyticalDerivCalcTool", this)
    , m_numBadCovWMatrix(0)
  {
    declareInterface<IAlignTrackDresser>(this);

    declareProperty("DerivCalcTool",   m_derivCalcTool,   "tool for calculating derivatives");

    m_logStream = nullptr;
  }

  //________________________________________________________________________
  AlignTrackDresser::~AlignTrackDresser()
  {
  }

  //________________________________________________________________________
  StatusCode AlignTrackDresser::initialize()
  {
    ATH_MSG_DEBUG("in AlignTrackDresser initialize()");

    // Get DerivCalcTool (will have to be moved to AlignTool)
    if ( m_derivCalcTool.retrieve().isFailure() ) {
      msg(MSG::FATAL)<< "Failed to retrieve tool " << m_derivCalcTool << endmsg;
      return StatusCode::FAILURE;
    } 
    ATH_MSG_INFO("Retrieved tool " << m_derivCalcTool);

    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  void AlignTrackDresser::setLogStream(std::ostream * os)
  {
    m_logStream = os;
    m_derivCalcTool->setLogStream(m_logStream);
  } 

  //________________________________________________________________________
  StatusCode AlignTrackDresser::finalize()
  {
    if(m_numBadCovWMatrix>0)
      ATH_MSG_INFO("Total of "<<m_numBadCovWMatrix<<" tracks had invalid weight matrix.");
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool AlignTrackDresser::dressAlignTrack(AlignTrack* alignTrack)
  {
    // set hit residuals
    setResiduals(alignTrack);

    ATH_MSG_DEBUG("residuals set, getting partial derivatives from m_derivCalcTool="<<m_derivCalcTool);

    // set partial derivatives of hit residuals w.r.t. alignment parameters
    if (!m_derivCalcTool->setDerivatives(alignTrack))
      return false;

    // for global method we let the deriv.calc.tool calculate the weight matrix
    // get W = V^{-1} R V^{-1} (simply V^{-1} for numerical derivatives)
    bool Wisvalid = m_derivCalcTool->setResidualCovMatrix(alignTrack);

    if (!Wisvalid) {
      ATH_MSG_INFO("something is wrong with the cov matrix for the residuals.");
      ++m_numBadCovWMatrix;
    }

    return Wisvalid;
  }

  //________________________________________________________________________
  void AlignTrackDresser::setResiduals(AlignTrack* alignTrack) const
  {
    if (alignTrack->residualVector()) 
      ATH_MSG_ERROR("alignTrack already has residuals!");
    
    const int NMEAS = alignTrack->nAlignTSOSMeas();
    Amg::VectorX * residuals = new Amg::VectorX(NMEAS);
    
    int imeas = 0;
    AlignTSOSCollection::const_iterator atsosItr = alignTrack->firstAtsos();
    for ( ; atsosItr != alignTrack->lastAtsos(); ++atsosItr) {		

      if (!(*atsosItr)->isValid())
        continue;

      std::vector<Residual>::const_iterator itRes = (**atsosItr).firstResidual();
      for ( ; itRes != (**atsosItr).lastResidual() ; ++itRes, ++imeas) {
        (*residuals)[imeas] = itRes->residual();
        if (msgLvl(MSG::DEBUG)) {
          double z=(**atsosItr).measurementOnTrack()->globalPosition().z();	
          ATH_MSG_DEBUG("pos.z: "<<z<<", res["<<imeas<<"]="<<itRes->residual()<<"  res/sq["<<imeas<<"]="<<itRes->residualNorm());
        }
      }
    }

    if (imeas!=NMEAS) {
      msg(MSG::ERROR)<<"problem with nmeas.  imeas="<<imeas<<", NMEAS="<<NMEAS<<endmsg;
      exit(3);
    }

    alignTrack->setResidualVector(residuals); // residuals owned by alignTrack
    ATH_MSG_DEBUG("residuals set for AlignTrack");
 }
   
} // end namespace
  
