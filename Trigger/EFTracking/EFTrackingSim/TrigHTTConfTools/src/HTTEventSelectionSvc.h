/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCONFTOOLS_HTTEVENTSELECTIONSVC_H
#define HTTCONFTOOLS_HTTEVENTSELECTIONSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigHTTConfTools/IHTTEventSelectionSvc.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTObjects/HTTTypes.h"

class HTTHit;
class HTTTrack;
class HTTOfflineTrack;
class HTTTruthTrack;
class HTTRegionSlices;
class HTTEventInputHeader;
class HTTLogicalEventInputHeader;


class HTTEventSelectionSvc : public extends< AthService, IHTTEventSelectionSvc >
{
 public:

  HTTEventSelectionSvc(const std::string& name, ISvcLocator* svc);
  virtual ~HTTEventSelectionSvc() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  //static const InterfaceID& interfaceID();
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;

  virtual unsigned getRegionID() const override { return m_regionID; }
  virtual SampleType getSampleType() const override { return m_st; }
  virtual bool checkPU() const override { return m_withPU.value(); }
  virtual const HTTRegionSlices* getRegions() override;
  virtual int getLRTpdgID() const override { return m_LRT_pdgID; }

  virtual HTTTrackPars getMin() const override { return m_min; }
  virtual HTTTrackPars getMax() const override { return m_max; }

  virtual bool passCuts(const HTTHit&) const override;
  virtual bool passCuts(const HTTTrack&) const override;
  virtual bool passCuts(const HTTOfflineTrack&) const override;
  virtual bool passCuts(const HTTTruthTrack&) const override;

  virtual bool passQOverPt(const HTTTrack&) const override;
  virtual bool passEta(const HTTTrack&) const override;
  virtual bool passPhi(const HTTTrack&) const override;
  virtual bool passD0(const HTTTrack&) const override;
  virtual bool passZ0(const HTTTrack&) const override;
  virtual bool passQOverPt(const HTTOfflineTrack&) const override;
  virtual bool passEta(const HTTOfflineTrack&) const override;
  virtual bool passPhi(const HTTOfflineTrack&) const override;
  virtual bool passD0(const HTTOfflineTrack&) const override;
  virtual bool passZ0(const HTTOfflineTrack&) const override;
  
  virtual bool passMatching(HTTTrack const &) const override;
  virtual bool passMatching(HTTTruthTrack const &) const override;

  virtual bool selectEvent(HTTEventInputHeader*) const override;
  virtual bool selectEvent(HTTLogicalEventInputHeader*) const override;

 private:

  // Gaudi parameters:
  Gaudi::Property<unsigned int> m_regionID     { this, "regionID", 0, "current region under processing"};  // Current region of interest
  Gaudi::Property<std::string> m_regions_path  { this, "regions", "", "path of the slices file"};         // path to slices file
  Gaudi::Property<std::string> m_sampleType    { this, "sampleType", "singleMuons", "type of sample under processing (skipTruth, singleElectrons, singleMuons, singlePions, or LLPs)"};           // type of sample ("skipTruth", "singleElectrons", "singleMuons", "singlePions")
  Gaudi::Property<bool> m_withPU           { this, "withPU",  false, "flag to say if there is pile-up or not"};              // flag to say if there is pile-up or not
  Gaudi::Property<bool> m_LRT              { this, "doLRT",   false, "Change track selection to LRT quantities; hit selection unchanged"};         // flag to require cancelling of selections on d0 and z0 in the case of large-radius tracking
  Gaudi::Property<float> m_minLRTpT        { this, "minLRTpT", 5., "Minimum pT to use in LRT selection, in GeV"};         // minimum pT, in GeV, to use in LRT selection
  Gaudi::Property<int> m_LRT_pdgID         { this, "lrt_truthMatchPDGID", 0, "If we are running an LLP sample but want only some PDGID of output in the truth selection, set this"};        // If we are running an LLP sample but want only some PDGID of output in the truth selection, set this
  Gaudi::Property<bool> m_allowHighBarcode { this, "allowHighBarcode", false, "Whether or not to allow barcodes over 200000 in truth matching"}; // whether or not to allow barcodes over 200000 in truth matching


  
  SampleType m_st = SampleType::skipTruth;           // internal value for faster comparisons in selectEvent()
  HTTRegionSlices* m_regions = nullptr;  // pointer to RegionSlices class

  HTTTrackPars m_min;                    // min limits of current region
  HTTTrackPars m_max;                    // max limits of current region
  HTTTrackPars m_trackmin;               // min limits of tracks to be accepted in region
  HTTTrackPars m_trackmax;               // max limits of tracks to be accepted in region

 
  void createRegions();                  // helper function to create RegionSlices object
  bool checkTruthTracks(const std::vector<HTTTruthTrack>&) const; // helper function to check the truth tracks for selectEvent()
  bool checkTruthTracksLRT(const std::vector<HTTTruthTrack>&) const; // check the truth tracks for selectEvent() with LRT requirements
};

/*inline const InterfaceID& HTTEventSelectionSvc::interfaceID()
{
  static const InterfaceID IID_HTTEventSelectionSvc("HTTEventSelectionSvc", 1, 0);
  return IID_HTTEventSelectionSvc;
}*/

#endif  // HTTCONFTOOLS_HTTEVENTSELECTIONSVC_H
