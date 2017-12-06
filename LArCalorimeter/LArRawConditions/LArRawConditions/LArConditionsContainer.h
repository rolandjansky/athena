//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArConditionContainer.h
 *
 * @brief This file defines the template class used for transient
 * conditions data
 *
 * @author RD Schaffer  <R.D.Schaffer@cern.ch>
 * @author Hong Ma      <hma@bnl.gov>
 * @author Walter Lampl <wlampl@mail.cern.ch>
 *
 * $Id: LArConditionsContainer.h,v 1.19 2009-04-23 09:19:19 wlampl Exp $
 */

#ifndef LARRAWCONDITIONS_LARCONDITIONSCONTAINER_H
#define LARRAWCONDITIONS_LARCONDITIONSCONTAINER_H
/**
 * @class LArConditionContainer
 *
 * @brief template class for providing interface for accessing
 * conditions data.  
 *
 * This data class inherits from CondMultChanCollection which allows
 * to optionally split up the persistent data into multiple groups,
 * storing them separately and registering them as multichannels in
 * COOL. (See CondMultChanCollection interface - its methods are
 * inherited, but not reproduced here.)
 *
 * The different groups are stored in LArConditionsSubset. The
 * division into groups is handled in the (implementation) base -
 * LArConditionsContainerBase. 
 *
 * Uniform access to the whole set of conditions is provided by
 * LArConditionsContainerDB<T> which maps onto the different groups in
 * the LArConditionsSubsets.
 *
 * Usage:
 *   1) create container:
 *      LArConditionsContainer<MyObj>* cont = new LArConditionsContainer<MyObj>;
 *   2) set grouping type: 
 *      cont->setGroupingType(LArConditionsContainerBase::SubDetectorGrouping);
 *   3) initialize:
 *      cont->initialize();
 * 
**/

// LAr classes
#include "LArRawConditions/LArConditionsSubset.h"
#include "LArRawConditions/LArConditionsContainerBase.h"
#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "LArRawConditions/LArConditionsContainerDB.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"


template<class T> 
class LArConditionsContainer : 
    public LArConditionsContainerBase,
    public CondMultChanCollection<LArConditionsSubset<T> >
{

public: 

    typedef T                                                    LArCondObj;
    /// Typedefs for subset
    typedef unsigned int                                         FebId; 
    typedef LArConditionsSubset<T>                               Subset; 
    typedef typename LArConditionsSubset<T>::Traits              Traits;
    typedef typename Subset::ChannelVector                       ChannelVector; 
    typedef typename Subset::FebPairReference                    FebPairReference;
    typedef typename ChannelVector::const_iterator               ChannelVectorIterator; 
    typedef typename Subset::size_type                           size_type; 
    typedef typename Subset::ConstCorrectionVecIt                ConstCorrectionIt;
    typedef typename Subset::Reference                           Reference;
    typedef typename Subset::ConstReference                      ConstReference;
    typedef typename Subset::Pointer                             Pointer;
    typedef typename Subset::ConstPointer                        ConstPointer;
    
    /// Typedefs for DB
    typedef LArConditionsContainerDB<T>                          ConditionsMap;
    typedef typename ConditionsMap::const_iterator               ConstConditionsMapIterator; 
    typedef typename ConditionsMap::iterator                     ConditionsMapIterator; 
    typedef typename std::map<unsigned int,  ConditionsMap >     GainMap;
    typedef typename std::pair<unsigned int, ConditionsMap >     GainPair;
    typedef typename GainMap::const_iterator                     ConstGainMapIterator; 
    typedef typename GainMap::iterator                           GainMapIterator; 

    /// Typedefs for MultChan access
    typedef CondMultChanCollection<Subset>                       MultChanCollection;
    typedef typename MultChanCollection::chan_const_iterator     chan_const_iterator;
    typedef typename MultChanCollection::iov_const_iterator      iov_const_iterator;
    typedef typename MultChanCollection::chan_size_type          chan_size_type;
    typedef typename MultChanCollection::iov_size_type           iov_size_type;


    /// default constructor
    LArConditionsContainer();

    /// constructor defining grouping type
    LArConditionsContainer(GroupingType type);

    /// destructor
    virtual ~LArConditionsContainer();


    /// put payload in persistent data 
    void                setPdata(const HWIdentifier id, 
				 const T& payload, 
				 unsigned int gain=0 );

    /// get data with online identifier
    ConstReference      get     (const HWIdentifier id, 
				 unsigned int gain=0) const; 

    /// non-const get data with online identifier
    Reference           get     (const HWIdentifier id, 
				 unsigned int gain=0); 

    /// get data with offline hash identifier
    ConstReference      get     (const IdentifierHash hashId, 
				 unsigned int gain=0) const; 

    /// get data with offline identifier
    ConstReference       get_offline     (const Identifier hashId, 
					  unsigned int gain=0) const; 

    /// Return the COOL channel number for a given online id and gain
    unsigned int        coolChannel (const HWIdentifier id,
				     unsigned int gain=0) const; 



    /// Type of grouping - defined in LArConditionContainerBase.h
    unsigned int        groupingType() const;

    /// Type of grouping - defined in LArConditionContainerBase.h
    std::string         groupingTypeToString() const;


    /// Return the group number of a COOL channel (0 - correction
    /// group, 1-N for FEB ID groups)
    unsigned int        groupNumber(unsigned int coolChannel) const;

    /// Number of groups - minimum is 2 (1 correction group, 1 FEB ID
    /// group)
    unsigned int        nGroups() const;

    /// Mininum gain value
    unsigned int        minGain() const;
    
    /// Number of gain values
    unsigned int        nGains() const;

    /// Access to Channel numbers via iterators - from MultChanCollection
    chan_const_iterator chan_begin() const;
    chan_const_iterator chan_end  () const;

    /// number of channels - from MultChanCollection
    chan_size_type      chan_size() const;
    
    /// Access to IOVs via iterators - from MultChanCollection
    iov_const_iterator  iov_begin() const;
    iov_const_iterator  iov_end  () const;
    
    /// number of IOVs - from MultChanCollection
    iov_size_type       iov_size() const;

    /// Statistics: number of conditions per COOL channel
    unsigned int        conditionsPerChannel(unsigned int coolChannel) const;

    /// Statistics: number of conditions per group
    unsigned int        conditionsPerGroup(unsigned int group) const;

    /// Statistics: number of conditions per gain
    unsigned int        conditionsPerGain(unsigned int gain) const;

    /// Statistics: total number of conditions 
    unsigned int        totalNumberOfConditions() const;

    /// Statistics: total number of corrections
    unsigned int        totalNumberOfCorrections() const;

    /// Have corrections been applied?
    bool                correctionsApplied() const;

    /// apply correction set
    StatusCode          applyCorrections();
    
    ///  undo corrections that have been already applied
    StatusCode          undoCorrections();
    

    ///  access to corrections - 
  /*  

  The following methods are commented out because they don't work any more and/or don't
  make sense any more since we have now the option to store corrections in the same subsets
  as the regular data. (And not used anywhere anyway).


    /// Access to a correction conditions object for a given channel
    /// id - the correction iterator must be compared to
    /// findCorrectionEnd iterator below
    ConstCorrectionIt   findCorrection   (HWIdentifier channelId,
					  unsigned int gain) const;
    ConstCorrectionIt   findCorrectionEnd() const;

    /// Access to the indexes for correction subsets given gain
    void                correctionIndexes(unsigned int gain,
                                          std::vector<unsigned int>& indexes) const;

    /// Iterators over channel set - must iterate over
    /// correctionIndexes from previous method
    ConstCorrectionIt   correctionsBegin(unsigned int index) const;
    ConstCorrectionIt   correctionsEnd  (unsigned int index) const;

    /// Size of channel set
    size_type           correctionsSize(unsigned int gain) const;


    /// Remove correction leaving the conditions - may be needed to
    /// only write out the conditions when both are present
    void                removeCorrections();
  */


    /// Insert a new channel id / T pair. If new channel id is the
    /// same as an existing one, the new T replaces the old T
    StatusCode          insertCorrection (HWIdentifier id, 
					  const T& cond, 
					  unsigned int gain,
					  bool corrChannel = true);


    ///get iterator over the Undo-Vector for a certain gain
    ConstCorrectionIt   undoCorrBegin(unsigned int gain) const;
    ConstCorrectionIt   undoCorrEnd(unsigned int gain) const;

    /// get iterator for all channels for a gain
    ConstConditionsMapIterator  begin(unsigned int gain) const ; 
    ConditionsMapIterator       begin(unsigned int gain); 

    /// get iterator for all channels for a gain, restricted to a list of FebIds
    ConstConditionsMapIterator  begin(unsigned int gain,
  				      const std::vector<FebId>& febIds) const ; 
    ConditionsMapIterator       begin(unsigned int gain,
                                      const std::vector<FebId>& febIds); 

    /// get iterator for all channels for a gain, restricted to one Feb
    ConstConditionsMapIterator  begin(unsigned int gain,
				      const HWIdentifier& febId) const ; 
    ConditionsMapIterator       begin(unsigned int gain,
				      const HWIdentifier& febId) ; 

    /// end of all channels for this gain
    ConstConditionsMapIterator  end  (unsigned int gain) const ; 
    ConditionsMapIterator       end  (unsigned int gain); 

    /// Remove conditions leaving the corrections - may be needed to
    /// only write out the corrections when both are present
    void                removeConditions();

    /// Remove the conditions for a specific cool channel - e.g. in
    /// order not to write it out
    void                removeConditionsChannel(unsigned int coolChannel);


    /// Fill all non-existing Correction Channels with emtpy subsets
    ///This is necessary to terminate the IOV of the previous correction
    //Returns the COOL channel numbers that have been added
    std::vector<unsigned> completeCorrectionChannels();


    /// Initialization done after creation or read back - derived
    /// classes may augment the functionality
    virtual StatusCode  initialize();
    

protected:

    /// Access to standard empty value
    Reference             empty() const;

    /// Map indexed by gain providing pointers into the data in the
    /// CondMultChanCollection. 
    GainMap               m_cached; 


private:
    ///Dummy iterator (returned if requested for invalid gain)
    ConstCorrectionIt m_dummyCorrIt;

    /// Update the ConditionsMap for a new FEB ID
    StatusCode            updateConditionsMap(FebId id, 
					      unsigned int gain, 
					      ConditionsMap& conditionsMap);
    /// Sort subsets by channel number
    void                  sortSubsets();


    /// initialize the vector for access via hashes
    void                  initHashVec() const; 

    class SubsetSort 
    {
    public:
	bool operator () (const LArConditionsSubset<T>* x, const LArConditionsSubset<T>* y) 
	    {
		return (x->channel() < y->channel());
	    }
    };
    
// Hide some types from Reflex so they do not get generated in the
// dictionary
#ifndef __REFLEX__
    typedef typename std::vector<ConstPointer>              TVector; 
    typedef typename TVector::const_iterator                TVectorIterator; 
    typedef typename std::map<unsigned int, TVector>        TVectorMap ; 
    typedef typename TVectorMap::const_iterator             TVectorMapIterator      ; 
    typedef typename Subset::CorrectionPair                 CorrectionPair;
    typedef typename Subset::CorrectionVec                  CorrectionVector;
    typedef typename Subset::ConstCorrectionVecIt           ConstCorrectionVecIt;
    typedef typename std::vector<CorrectionVector>          CorrectionVectors;
    typedef DataVector<Subset>                              SubsetDV;
    typedef typename SubsetDV::const_iterator               const_iterator;
    typedef typename SubsetDV::iterator                     iterator;
    

    mutable TVectorMap           m_hashVec  ;
    mutable CorrectionVectors    m_correctionsUndo;
    mutable bool                 m_hashVecValid ;
    mutable bool                 m_correctionsApplied;
    CorrectionVector             m_emptyCorrectionVec;
#endif 
};


#ifndef __REFLEX__
#include "LArConditionsContainer.icc"
#endif 

#endif 


