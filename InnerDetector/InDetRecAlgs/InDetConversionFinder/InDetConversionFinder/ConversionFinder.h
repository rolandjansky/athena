/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ConversionFinder.h  -  Description
                             -------------------
    begin   : 10-11-2005
    authors : Tatjana Lenz
    email   : tatjana.lenz@cern.ch
    changes :
 ***************************************************************************/
#ifndef INDETCONVERSIONFINDER_CONVERSIONFINDER_H
#define INDETCONVERSIONFINDER_CONVERSIONFINDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
class IEMExtrapolationTools;


namespace InDet{

  class ConversionFinder : public AthAlgorithm {

  public:
    ConversionFinder(const std::string &name, ISvcLocator *pSvcLocator);
    ~ConversionFinder();
    StatusCode initialize();
    StatusCode finalize();
    StatusCode execute();
    void resetStatistics();  //<! Initialize the statistics vectors
    void analyzeResults(xAOD::VertexContainer*);

  protected:

    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tracksName;                //!< Name of track container in StoreGate
    SG::WriteHandleKey<xAOD::VertexContainer>       m_InDetConversionOutputName; //!< Name of output container to store results

    ToolHandle<IVertexFinder> m_VertexFinderTool{
      this,
      "VertexFinderTool",
      "InDet::InDetConversionFinderTools",
      "vertex finder tool"
    };
    ToolHandle<IEMExtrapolationTools> m_EMExtrapolationTool;

    bool m_doExtrapolation;

      /** Statistics  */
    long m_events_processed{};    //!< Number of events processed
    long m_Gamma_stored{};        //!< Number of conversion vertices stored
    long m_Double_Conversions{};  //!< Number of two-track conversions
    long m_Single_Conversions{};  //!< Number of single-track conversions
    long m_SiSi_Conversions{};    //!< Number of Si-Si track conversions
    long m_SiTrt_Conversions{};   //!< Number of Si-TRT track conversions
    long m_TrtTrt_Conversions{};  //!< Number of TRT-TRT track conversions
    long m_Si_Conversions{};      //!< Number of Si single-track conversions
    long m_Trt_Conversions{};     //!< Number of TRT single-track conversions

  };
}
#endif // INDETCONVERSIONFINDER_CONVERSIONFINDER_H
