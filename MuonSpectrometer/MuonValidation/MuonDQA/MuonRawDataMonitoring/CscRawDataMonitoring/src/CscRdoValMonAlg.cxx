/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Athena include(s)
#include "AthenaMonitoring/AthenaMonManager.h"
#include "CscRdoValMonAlg.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"

//
// constructor ----------------------------------------------------------------
//
CscRdoValMonAlg::CscRdoValMonAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthMonitorAlgorithm(name, pSvcLocator)
  { }

//
// initialize ----------------------------------------------------------------
//
StatusCode CscRdoValMonAlg::initialize() 
{
  ATH_MSG_DEBUG ("CscRdoValMonAlg : in initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());
  ATH_CHECK(m_cscRdoKey.initialize());
  return AthMonitorAlgorithm::initialize();
}

//
// fillHistograms ----------------------------------------------------------------
//
StatusCode CscRdoValMonAlg::fillHistograms(const EventContext& ctx) const
{
  ATH_MSG_DEBUG ( "CscRdoValMonAlg :: in fillHistograms()"  );

  if(m_cscRdoKey.key() == ""){ //it seems that protection is needed for this case
    ATH_MSG_WARNING("CSC RDO key is blank, returning");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<CscRawDataContainer> CscRDO(m_cscRdoKey, ctx);

  // ==============================================================================
  // Field           Range               Notes
  // ==============================================================================
  // StationName     unsigned integer    maps to "CSS", "CSL", etc.
  // StationEta      [-1,1]              -1 for backward, 1 for forward endcap
  // StationPhi      [1,8]               increases with Phi
  // Technology      [1]                 maps to "CSC"
  // ChamberLayer    [1,2]               increases with |Z|
  // WireLayer       [1,4]               increases with |Z|
  // MeasuresPhi     [0,1]               0 if measures R, 1 if measures Phi
  // Strip           [1,n]               increases with R   for MeasuresPhi=0
  //                                     increases with Phi for MeasuresPhi=1
  // ==============================================================================

  ATH_MSG_DEBUG ( " Size of RDO Container  : " << CscRDO->size()  );

  // Begin Event ==================================================
  ATH_MSG_DEBUG ( " BEGIN  EVENT ========================================== "  );

  for (CscRawDataContainer::const_iterator it = CscRDO->begin(); it != CscRDO->end(); ++it)
  {
    const CscRawDataCollection * rdo = *it;
    ATH_MSG_DEBUG ( " Number of Samples      : " << rdo->numSamples()  );
    ATH_MSG_DEBUG ( " Size of Collection     : " << rdo->size()  );

    size_t nEtaClusWidthCnt = 0, nPhiClusWidthCnt = 0; //cluster position in each phi-layer
    Identifier stationId, channelId;

    int clusCount[33][9], sigclusCount[33][9];
    for(size_t kl = 0; kl < 33; kl++ )
    {
      for(size_t km = 0; km < 9; km++ )
      {
        clusCount[kl][km] = 0;
        sigclusCount[kl][km] = 0;
      }
    }

    // loop over ROD-clusters
    for(CscRawDataCollection::const_iterator ic   = (*it)->begin(); ic != (*it)->end(); ++ic)
    {
      const CscRawData *raw = (*ic);
      if(raw)
      {
        // Identify side(A/C), sector(1-16)/layer(1-4)
        stationId = m_cscRdoDecoderTool->stationIdentifier(raw,&m_idHelperSvc->cscIdHelper());
        channelId = m_cscRdoDecoderTool->channelIdentifier(raw,&m_idHelperSvc->cscIdHelper(),0);
        int stationName = m_idHelperSvc->cscIdHelper().stationName(channelId);
        int chamberType = m_idHelperSvc->cscIdHelper().stationNameIndex("CSS") == stationName ? 0 : 1;
        int stationEta = m_idHelperSvc->cscIdHelper().stationEta(channelId);
        int stationPhi = m_idHelperSvc->cscIdHelper().stationPhi(channelId);
        int wireLayer = m_idHelperSvc->cscIdHelper().wireLayer(channelId);
        int measuresPhi = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);
        auto measuresPhi_mon = Monitored::Scalar<int>("measuresPhi_mon", measuresPhi);
        auto measuresEta = Monitored::Scalar<int>("measuresEta", !measuresPhi );

        // determine the sector number
        int sectorNo = stationEta * (2 * stationPhi - chamberType);

        // compute the indices to store cluster count
        int ns = sectorNo < 0 ? sectorNo*(-1) : sectorNo+16; // [-16 -> -1] shifted to [1 -> 16] and [+1 -> +16] shifted to [+17 -> +32]
        int nl = (measuresPhi ? wireLayer : wireLayer+4);  // [ 1 -> 4] (phi-layers) and [5 -> 8] (eta-layers)
        clusCount[ns][nl]++;

        // indices for ns = [+1 -> +32]; 32 places (index '0' is not counted); allocated 33 places
        // indices for nl = [+1 -> +8]; 8 places (index '0' is not counted); allocated 9 places
        ATH_MSG_DEBUG(" ns = " << ns << "\tm_nl = " << nl << "\tm_sec = " << sectorNo << "\t m_lay= " << wireLayer << "\tmPhi = " << measuresPhi);

        // y-axis fill value
        // sector# +2 layer 1 maps to +2 + 0.2*(1-1) + 0.1 = +2.1
        // sector# +2 layer 2 maps to +2 + 0.2*(2-1) + 0.1 = +2.3
        // sector# +2 layer 3 maps to +2 + 0.2*(3-1) + 0.1 = +2.5
        // sector# +2 layer 4 maps to +2 + 0.2*(4-1) + 0.1 = +2.7
        auto secLayer = Monitored::Scalar<float>("secLayer",sectorNo + 0.2 * (wireLayer - 1) + 0.1);
        int xfac = measuresPhi ? -1 : 1;  // -48 / +192

        // this way we get 4 time samples per strip
        ATH_MSG_DEBUG (" Width of ROD cluster   : " << raw->width()  );
        uint16_t diff_max = 0., diff = 0.;
        std::vector<float> xVals;

        // loop over strips in ROD cluster
        auto raw_clus_width =  Monitored::Scalar<size_t>("raw_clus_width",raw->width());

        if (measuresPhi) nPhiClusWidthCnt++;
        else nEtaClusWidthCnt++;
        fill("CscRdoMonitor", raw_clus_width, secLayer, measuresPhi_mon, measuresEta);

        // loop over strips
        for (size_t n = 0; n < raw_clus_width; n++ )
        {
          // identify this strip
          Identifier chID = m_cscRdoDecoderTool->channelIdentifier(raw, &m_idHelperSvc->cscIdHelper(), n);
          int strip = m_idHelperSvc->cscIdHelper().strip(chID);
          auto stripId = Monitored::Scalar<float>("stripId",strip * xfac);         // x-axis fill value
          fill("CscRdoMonitor", stripId, secLayer);

          // for every strip that has a hit, store the X,Y values
          xVals.push_back(stripId);

          // extract the (four) time samples for this strip
          std::vector<uint16_t> samples;
          bool extractSamples = raw->samples(n, rdo->numSamples(), samples);
          uint16_t n_max=0, n_min = 9999;

          // if we have the time samples, identify the max/min sampling values i.e., ADC_max and ADC_min
          if (extractSamples)
          {
            for (size_t np = 0; np < samples.size(); np++)
            {
              if(samples[np] < n_min) n_min = samples[np];
              if(samples[np] > n_max) n_max = samples[np];
            }
            // the diff between max and min samplings Delta_ADC = (ADC_max - ADC_min)
            diff = n_max - n_min;

            // compute the max difference Max_Delta_ADC
            if(diff > diff_max) diff_max = diff;

            ATH_MSG_DEBUG ( n << " Max = " << n_max << "  Min = " << n_min << " Diff = " << diff  );
          } // end if extractSamples
        } // end for loop over strips in cluster

        ATH_MSG_DEBUG ( " End loop over strips======================" );

        ATH_MSG_DEBUG ( " Max difference                : " << diff_max  );

        auto diff_max_mon = Monitored::Scalar<uint16_t>("diff_max_mon",diff_max);
        fill("CscRdoMonitor", diff_max_mon);

        // determine of the cluster is a noise/signal cluster Max_Delta_ADC > NoiseCut
        bool signal = diff_max > m_cscNoiseCut ? true : false;
        std::vector<int> tmp_xVals_signal;
        std::vector<int> tmp_xVals_noise;
        std::vector<float> tmp_secLayer_signal;
        std::vector<float> tmp_secLayer_noise;

        if (signal)
        {
          sigclusCount[ns][nl]++; 
          for(size_t nf = 0; nf < xVals.size(); nf++)
          {
            tmp_xVals_signal.push_back(xVals[nf]);
            tmp_secLayer_signal.push_back(secLayer);
          }
        }
        else
        {
          for(size_t nf = 0; nf < xVals.size(); nf++)
          {
            tmp_xVals_noise.push_back(xVals[nf]);
            tmp_secLayer_noise.push_back(secLayer);
          }
        }
        auto isSignal = Monitored::Scalar<int>("isSignal", (int) signal);
        auto isNoise = Monitored::Scalar<int>("isNoise", (int) !signal);
        auto tmp_xVals_signal_mon = Monitored::Collection("tmp_xVals_signal_mon", tmp_xVals_signal);
        auto tmp_secLayer_sig_mon = Monitored::Collection("tmp_secLayer_sig_mon", tmp_secLayer_signal);
        auto tmp_secLayer_noise_mon = Monitored::Collection("tmp_secLayer_noise_mon", tmp_secLayer_noise);
        auto sideC_signal = Monitored::Scalar<int>("sideC_signal", (int) (signal && stationEta == -1));
        auto sideA_signal = Monitored::Scalar<int>("sideA_signal", (int) (signal && stationEta == 1));
        auto signal_measuresEta = Monitored::Scalar<int>("signal_measuresEta", (int)(signal && !measuresPhi));
        auto signal_measuresEta_sideC = Monitored::Scalar<int>("signal_measuresEta_sideC", (int)(signal && !measuresPhi && stationEta == -1));
        auto signal_measuresEta_sideA = Monitored::Scalar<int>("signal_measuresEta_sideA", (int)(signal && !measuresPhi && stationEta == 1));
        auto signal_measuresPhi = Monitored::Scalar<int>("signal_measuresPhi", (int)(signal && measuresPhi));
        auto signal_measuresPhi_sideC = Monitored::Scalar<int>("signal_measuresPhi_sideC", (int)(signal && measuresPhi && stationEta == -1));
        auto signal_measuresPhi_sideA = Monitored::Scalar<int>("signal_measuresPhi_sideA", (int)(signal && measuresPhi && stationEta == 1));
        auto tmp_xVals_noise_mon = Monitored::Collection("tmp_xVals_noise_mon", tmp_xVals_noise);
        auto noise_measuresEta = Monitored::Scalar<int>("noise_measuresEta", (int)(!signal && !measuresPhi ));
        auto noise_measuresPhi = Monitored::Scalar<int>("noise_measuresPhi", (int)(!signal && measuresPhi ));
        
        fill("CscRdoMonitor", isSignal, isNoise, secLayer, tmp_secLayer_sig_mon, tmp_secLayer_noise_mon, tmp_xVals_signal_mon, tmp_xVals_noise_mon, raw_clus_width, sideC_signal, sideA_signal, signal_measuresEta, signal_measuresEta_sideC, signal_measuresEta_sideA, signal_measuresPhi, signal_measuresPhi_sideC, signal_measuresPhi_sideA, noise_measuresEta, noise_measuresPhi);

      } // end if raw
    } // end loop over ROD-clusters

    ATH_MSG_DEBUG ( " End loop over clusters======================" );
    auto nPhiClusWidthCnt_mon = Monitored::Scalar<size_t>("nPhiClusWidthCnt_mon", nPhiClusWidthCnt);
    auto nEtaClusWidthCnt_mon = Monitored::Scalar<size_t>("nEtaClusWidthCnt_mon", nEtaClusWidthCnt);
    fill("CscRdoMonitor", nPhiClusWidthCnt_mon, nEtaClusWidthCnt_mon);

    int numeta = 0, numphi = 0, numetasignal = 0, numphisignal = 0;
    for(int kl = 1; kl < 33; kl++ )
    {
      // int m_sec = kl < 17 ? kl*(-1) : kl; // [1->16](-side)  [17-32] (+side)
      for(int km = 1; km < 9; km++ )
      {
        int lay = (km > 4 && km < 9) ? km-4 : km;  // 1,2,3,4 (phi-layers)     5-4, 6-4, 7-4, 8-4 (eta-layers)
        bool mphi = (km > 0 && km < 5) ? true : false; // 1,2,3,4 (phi-layers) 5,6,7,8 (eta-layers)
        std::string wlay = mphi ? "Phi-Layer " : "Eta-Layer: ";
        int count = clusCount[kl][km];
        int scount = sigclusCount[kl][km];
        int tmp_counts_phi = 0, tmp_counts_eta = 0;

        if(count > 0)
        {
          float secLayer = kl-16 + 0.2 * (lay - 1) + 0.1;
          if(mphi)
          {
            numphi += count;
            if(scount > 0)
            {
              numphisignal += scount;
              tmp_counts_phi = count - scount;
            }
            else tmp_counts_phi = count;
          }
          else
          {
            numeta += count;
            if(scount > 0)
            {
              numetasignal += scount;
              tmp_counts_eta = count - scount;
            }
            else tmp_counts_eta = count;
          }
          ATH_MSG_DEBUG ( wlay << "Counts sec: [" << kl-16 << "]\tlayer: [" << km << "] = " << secLayer << "\t = " << count << "\t" << scount);

          auto secLayer_count = Monitored::Scalar<float>("secLayer_count", secLayer);
          auto mphi_mon = Monitored::Scalar<int>("mphi_mon",(int) ( (count>0) && mphi));
          auto count_mon = Monitored::Scalar<int>("count_mon", count);
          auto scount_mon = Monitored::Scalar<int>("scount_mon", scount);
          auto mphi_scount = Monitored::Scalar<int>("mphi_scount",(int) ( (count>0) && mphi && (scount>0)));
          auto mphi_inv = Monitored::Scalar<int>("mphi_inv", (int) ( (count>0) && !mphi));
          auto mphi_inv_scount = Monitored::Scalar<int>("mphi_inv_scount", (int) ( (count>0) && !mphi && (scount>0)));
          auto tmp_counts_phi_mon = Monitored::Scalar<int>("tmp_counts_phi_mon",tmp_counts_phi);
          auto tmp_counts_eta_mon = Monitored::Scalar<int>("tmp_counts_eta_mon",tmp_counts_eta);

          fill("CscRdoMonitor", count_mon, secLayer_count, mphi_mon, mphi_scount, scount_mon, tmp_counts_phi_mon, mphi_inv, mphi_inv_scount, tmp_counts_eta_mon);
        } // end if count
      } 
    }

    auto numphi_mon = Monitored::Scalar<int>("numphi_mon",numphi);
    auto numeta_mon = Monitored::Scalar<int>("numeta_mon",numeta);
    auto numphisignal_mon = Monitored::Scalar<int>("numphisignal_mon",numphisignal);
    auto numetasignal_mon = Monitored::Scalar<int>("numetasignal_mon",numetasignal);
    auto diffnumphi = Monitored::Scalar<int>("diffnumphi", numphi-numphisignal);
    auto diffnumeta = Monitored::Scalar<int>("diffnumeta",numeta-numetasignal);

    fill("CscRdoMonitor", numphi_mon, numeta_mon, numphisignal_mon, numetasignal_mon, diffnumphi, diffnumeta);
  }

  ATH_MSG_DEBUG ( " END EVENT ============================================ "  );
  ATH_MSG_DEBUG ( "done collecting histograms"  );
  ATH_MSG_DEBUG ( "CSCRdoMon::fillHistograms reports success"  );

  return StatusCode::SUCCESS;
}
