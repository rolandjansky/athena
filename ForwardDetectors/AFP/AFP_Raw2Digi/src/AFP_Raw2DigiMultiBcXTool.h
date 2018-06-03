/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAW2DIGIMULTIBCXTOOL_H
#define AFP_RAW2DIGIMULTIBCXTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AFP_Raw2Digi/IAFP_Raw2DigiTool.h"
#include "AFP_RawEv/AFP_RawContainer.h"
#include "xAODForward/AFPToFHit.h"
#include "xAODForward/AFPToFHitContainer.h"
#include "xAODForward/AFPSiHitContainer.h"
#include <string>
#include <vector>
#include "TF1.h"

class AFP_Raw2DigiMultiBcXTool : public extends<AthAlgTool, IAFP_Raw2DigiTool>
{
public:
  AFP_Raw2DigiMultiBcXTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing 
  virtual ~AFP_Raw2DigiMultiBcXTool() override;

  /// Does nothing
  virtual StatusCode initialize() override;

  /// Creates xAOD for silicon detector
  virtual StatusCode recoSiHits() override;

  /// Creates xAOD for time-of-flight detector
  virtual StatusCode recoToFHits() override;

  virtual StatusCode recoAll() override;

  /// Does nothing
  virtual StatusCode finalize() override;

  /// Method that decodes raw information about time-over-threshold to
  /// number of clock ticks
  unsigned int decodeTimeOverThresholdSi (const unsigned int input, const unsigned int discConfig) const;

  /// @brief Find the first bcID in the set taking into account circularity
  ///
  /// If the largest and smallest number in the set are 0 and #m_maxBcId it
  /// means that the bcIds span over the circularity of the
  /// numbering. In this case bcIds are checked one by one starting
  /// from the lowest untill the next one does not equal previous plus
  /// 1. The first number which does not fit this scheme is returned.
  ///
  /// If the largest and smallest number are not 0 and 3563 the
  /// smallest number is returned.
  unsigned int findFirstBcId (const std::set<unsigned int>& allBcIds) const;

  /// Returns difference between second and first. If difference is negative #m_maxBcId is added.
  unsigned int bcIdDiff (const int first, const int second) const;

  /// Remove elements which do not form continous block
  void removeNotContinues (std::set<unsigned int>& allBcIds) const;
  
protected:
  std::string m_rawDataContainerName;
  std::string m_AFPSiHitsContainerName;
  std::string m_AFPHitsContainerNameToF;

  /// @brief Factor converting signal to time
  ///
  /// The value of the factor is 25/1024 nanoseconds
  static constexpr double s_timeConversionFactor = 25./1024.;

  /// @brief Factor converting pulse length to time
  ///
  /// The value of the factor is 0.521 nanoseconds
  static constexpr double s_pulseLengthFactor = 0.521;

  /// @brief Function that transforms time-over-threshold to charge
  ///
  /// Transformation function can be set in steering cards
  TF1 m_totToChargeTransformation;	

  /// Method that creates a new AFPToFHit and sets it valus according to #data
  void newXAODHitToF (xAOD::AFPToFHitContainer* tofHitContainer, const AFP_ToFRawCollection& collection, const AFP_ToFRawData& data) const;
  
  /// Method that creates a new AFPSiHit and sets it valus according to #data
  void newXAODHitSi (xAOD::AFPSiHitContainer* xAODSiHit, const AFP_SiRawCollection& collection, const AFP_SiRawData& data) const;

  /// @brief Method mapping hptdcID and hptdcChannel to train ID and bar in train ID
  ///
  /// The method requires that hptdcID and hptdcChannel are set in the
  /// tofHit passed as argument.  Mapping is implemented according to
  /// https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC#Channel_Mapping
  void setBarAndTrainID(xAOD::AFPToFHit* tofHit) const;

  /// @brief Returns verified collection bcID
  ///
  /// The method checks if bcID of the current collection is the same
  /// as previous or next. If it is not it indicates there is a
  /// problem with current bcID. The correct value is being recovered in the following way:
  /// * if next and previous bcID are the same it means the current bcID should be the same
  /// * if next and previous bcIDs differ and one of them has more
  ///   entries than the other, the current bcID is added to the one
  ///   which has smaller number of entries
  /// * if next and previous bcIDs differ and have exactly the same
  ///   number of entries bcID of next is returned, because there is
  ///   no hint which is the correct one
  /// 
  /// @note Needs to be a template to work for silicon and time-of-flight
  ///
  /// @param currentIter iterator to the collection which bcID is to be tested
  /// @param previousIter iterator to the collection previous to the one which is tested
  /// @param nextIter iterator to the collection next to the one which is tested
  /// @param container container for accesing beginning and end of collections
  template <typename CollectionType>
  int correctBcId (const typename CollectionType::const_iterator& currentIter,
		   const typename CollectionType::const_iterator& previousIter,
		   const typename CollectionType::const_iterator& nextIter,
		   const CollectionType& container) const;

  /// @brief Returns verified collection bcID
  ///
  /// The method checks if bcID of the current collection is the same
  /// as next. If it is not it indicates there is a problem with
  /// current or next bcID. The third value is checked and the one
  /// that dominates is returend.
  ///
  /// @note Needs to be a template method in order to accept both
  /// const_iterator and reverse_iterator.
  ///
  /// @param currentIter iterator to the collection which bcID is to be tested
  /// @param nextIter iterator to the collection next to the one which is tested
  /// @param iterEnd iterator marking the end of collections
  template <typename IterType1, typename IterType2>
  int correctBcId (const IterType1& currentIter,
			const IterType2& nextIter,
			const IterType2& iterEnd) const;
					
  /// @brief Returns how many collections with the current bcID are in the list
  ///
  /// Iterates the list untill the end of the list or collection with
  /// different bcID than the one pointing by the iterator is
  /// encountered. Counts how many collections were iterated in this
  /// way.
  ///
  /// @note Needs to be a template method in order to accept both
  /// const_iterator and reverse_iterator.
  ///
  /// @param collectionIter iterator from which counting should start and go forward
  /// @param iterEnd iterator marking the end of collections
  template <typename IterType>
  int countBcIDOccuranceInBlock (IterType collectionIter, const IterType& iterEnd ) const;

  /// @brief Adds new xAOD objects to approperiate containers
  void siCollectionToXAOD (const AFP_SiRawCollection& collection, const int diff, std::vector<xAOD::AFPSiHitContainer*>& hitContainers) const;

  /// @brief Adds new xAOD objects to approperiate containers
  void tofCollectionToXAOD (const AFP_ToFRawCollection& collection, const int diff, std::vector<xAOD::AFPToFHitContainer*>& hitContainers) const;
  
  /// @brief Creates names of containers based on bcID information in the event
  ///
  /// @note In general should be called only for the first event.
  template<typename CollectionType>
  void createContainersNames (const std::list<CollectionType>& coll, const std::string& containerNameBase, std::vector<std::string>& containersNames);


  /// @brief Fills xAOD containers with information from collections
  ///
  /// @param coll list of collections providing information for filling xAOD containers
  /// @param hitContainers vector of xAOD containers to be filled
  /// @param collectionToXAOD method filling an xAOD container with information form a collection
  template<typename CollectionType, typename ContainerType>
  void fillXAODWithCollections (const typename std::list<CollectionType>& coll,
				typename std::vector<ContainerType*>& hitContainers,
				void (AFP_Raw2DigiMultiBcXTool::*collectionToXAOD) (const CollectionType& collection, const int diff, typename std::vector<ContainerType*>& hitContainers) const);

  /// Maximum value of a bcId sent by AFP
  static constexpr int m_maxBcId = 1023;

  /// @brief True if processing first event
  ///
  /// Needed for finding out how many bunch crossings are in data and
  /// setting up the xAOD containers. Used in tracking reconstruction.
  bool m_firstEventSi;

  /// @brief True if processing first event
  ///
  /// Needed for finding out how many bunch crossings are in data and
  /// setting up the xAOD containers. Used in time-of-flight reconstruction.
  bool m_firstEventToF;

  /// Vector of names of xAOD containers for silicon hits
  std::vector<std::string> m_containersNameSi;

  /// Vector of names of xAOD containers for time-of-flight hits
  std::vector<std::string> m_containersNameToF;
};

template <typename IterType>
int AFP_Raw2DigiMultiBcXTool::countBcIDOccuranceInBlock (IterType collectionIter, const IterType& iterEnd) const
{
  const int nominalBcId = collectionIter->bcId();
  int count = 1; // first count from current position
  while ( (collectionIter++) != iterEnd)
    if (nominalBcId == collectionIter->bcId())
      ++count;
    else
      break;

  return count;
}

template <typename IterType1, typename IterType2>
int AFP_Raw2DigiMultiBcXTool::correctBcId (const IterType1& currentIter,
				   const IterType2& nextIter,
				   const IterType2& iterEnd) const
{
  const int currentBcId = currentIter->bcId();
  
  // if current is the same as next everything is OK
  const int nextBcId = nextIter->bcId();
  if (currentBcId == nextBcId)
    return currentBcId;

  // --- current bcID is WRONG ---
  // test the next element
  IterType2 nextNextIter = nextIter;
  nextNextIter++;

  if (nextNextIter != iterEnd) {
    const int nextNextBcId = nextNextIter->bcId();

    if (nextNextBcId == currentBcId) {
      // the second bcID is incorrect return current one
      return currentBcId;
    }
    else if (nextNextBcId == nextBcId) {
      // the first bcID is incorrect return the next one
      return nextBcId;
    }
    else {
      // nothing matches no clue which one is correct
      ATH_MSG_WARNING("3 subsequent bcIDs differ. No idea which one is correct returning current one i.e. "<<currentBcId);
      return currentBcId;
    }
    
  }
  else {
    ATH_MSG_WARNING("BcIds differ, but there are no more elements to validate against. Returning current bcID i.e. "<<nextBcId);
    return currentBcId;
  }
  
}

template<typename CollectionType>
void AFP_Raw2DigiMultiBcXTool::createContainersNames (const std::list<CollectionType>& coll, const std::string& containerNameBase, std::vector<std::string>& containersNames) 
{
    // if first event create names of containers
    int maxBcIdDiff = 0;
    const typename std::list<CollectionType>::const_iterator iterEnd = coll.end();
    typename std::list<CollectionType>::const_iterator previousIter = coll.begin();
    const int firstBcID = previousIter->bcId();

    typename std::list<CollectionType>::const_iterator currentIter = previousIter;
    currentIter++;
    
    typename std::list<CollectionType>::const_iterator nextIter = currentIter;
    nextIter++;
    
    // skip first item, because obviously difference in bcId is 0
    // skip last item, because it should have the same bcId as second to last thus does not matter
    while (nextIter != iterEnd) {
      const int diff = bcIdDiff (firstBcID, correctBcId(currentIter, previousIter, nextIter, coll));
      if (maxBcIdDiff < diff)
	maxBcIdDiff = diff;

      currentIter++;
      previousIter++;
      nextIter++;
    }

    containersNames.clear();
    containersNames.reserve(maxBcIdDiff+1);
    // first item with default name
    containersNames.push_back(containerNameBase);
    // remaining items with difference suffix
    for (int diff = 1; diff <= maxBcIdDiff; ++diff) {
      std::stringstream name;
      name<<containerNameBase<<"BcIDplus"<<diff;
      containersNames.push_back(name.str());
    }

    ATH_MSG_DEBUG("Containers names created");
}

template <typename CollectionType>
int AFP_Raw2DigiMultiBcXTool::correctBcId (const typename CollectionType::const_iterator& currentIter,
				   const typename CollectionType::const_iterator& previousIter,
				   const typename CollectionType::const_iterator& nextIter,
				   const CollectionType& container) const
{
  const int currentBcId = currentIter->bcId();
  
  // if current is the same as previous everything is OK
  const int previousBcId = previousIter->bcId();
  if (currentBcId == previousBcId)
    return currentBcId;

  // if current is the same as next everything is OK
  const int nextBcId = nextIter->bcId();
  if (currentBcId == nextBcId)
    return currentBcId;
  
  // --- current bcID is WRONG ---
  // --- try to correct current bcID ---
  
  // if next and previous bcIDs are the same current must be of the same value
  if (previousBcId == nextBcId) {
    ATH_MSG_WARNING("Correcting bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<previousBcId);
    return previousBcId;
  }

  // tricky situation we are at the join of two bcIDs
  // calculate how many entries are with previous and next bcID and correct 
  const int nextEntries = countBcIDOccuranceInBlock (nextIter, container.end());
  const int previousEntries = countBcIDOccuranceInBlock (std::reverse_iterator<typename CollectionType::const_iterator>(previousIter),
							 container.rend()
    );
	
  // check if next and previous differ by only 1. This should happen if only one error is present
  const int entriesDiff = nextEntries - previousEntries;
  if (abs(entriesDiff) == 1) {
    // good - only one error, now correct
    if (entriesDiff > 0) { // previous are missing one entry so add current to previous
      ATH_MSG_WARNING("Correcting bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<previousBcId);
      return previousBcId;
    }
    else {// next are missing one entry so add current to next
      ATH_MSG_WARNING("Correcting bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<nextBcId);
      return nextBcId;
    }
  }
  // there are more errors trying to guess the correct bcID
  else if (entriesDiff > 0) {
    // previous are missing one entry so add current to previous,
    ATH_MSG_WARNING("Difference between bcID occurrences is different from 1.");
    ATH_MSG_WARNING("Guessing correction of bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<previousBcId);
    return previousBcId;
  }
  else if (entriesDiff < 0) {
    // next are missing one entry so add current to next
    ATH_MSG_WARNING("Difference between bcID occurrences is different from 1.");
    ATH_MSG_WARNING("Guessing correction of bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<nextBcId);
    return nextBcId;
  }
  else {
    // we are screwed up, no hints about correct bcID
    ATH_MSG_WARNING("Difference between bcID occurrences is 0. NO IDEA which bcId is correct.");
    ATH_MSG_WARNING("Blindly guessing correction of bcID in collection with lvl1Id="<<currentIter->lvl1Id()<<" robID="<<std::hex<<"0x"<<currentIter->robId()<<std::dec<<" and link="<<currentIter->link()<<" from "<<currentIter->bcId()<<" to "<<nextBcId);
    return nextBcId;
  }
}

template<typename CollectionType, typename ContainerType>
void AFP_Raw2DigiMultiBcXTool::fillXAODWithCollections (const typename std::list<CollectionType>& coll,
						typename std::vector<ContainerType*>& hitContainers,
						void (AFP_Raw2DigiMultiBcXTool::*collectionToXAOD) (const CollectionType& collection, const int diff, typename std::vector<ContainerType*>& hitContainers) const)
{
  if (coll.size() >= 3) {
    // prepare iterators
    const typename std::list<CollectionType>::const_iterator iterEnd = coll.end();
    typename std::list<CollectionType>::const_iterator previousIter = coll.begin();
    typename std::list<CollectionType>::const_iterator currentIter = previousIter;
    currentIter++;
    typename std::list<CollectionType>::const_iterator nextIter = currentIter;
    nextIter++;

    const unsigned int firstBcID = correctBcId(previousIter, currentIter, iterEnd);
  
    // fill first event
    const int diffFirst = bcIdDiff (firstBcID, correctBcId(previousIter, currentIter, iterEnd)); // to test is previousIter, currentIter is used as next
    (this->*collectionToXAOD) (*previousIter, diffFirst, hitContainers);

    // fill rest but last collections
    while (nextIter != iterEnd) {
      const int diff = bcIdDiff (firstBcID, correctBcId(currentIter, previousIter, nextIter, coll));
      (this->*collectionToXAOD) (*previousIter, diff, hitContainers);
      currentIter++;
      previousIter++;
      nextIter++;
    }

    // fill last collection
    const int diffLast = bcIdDiff (firstBcID, correctBcId(currentIter,
							  std::reverse_iterator<typename std::list<CollectionType>::const_iterator>(previousIter),
							  coll.rend()
							  )
				   ); // need to reverse iterator because testing the last element
    (this->*collectionToXAOD) (*currentIter, diffLast, hitContainers);
  }
  else
    ATH_MSG_WARNING("Less than 3 collections in container->collectionsXX(). Too few. Skipping event.");
  
}

inline unsigned int AFP_Raw2DigiMultiBcXTool::bcIdDiff (const int first, const int second) const
{
  int result = second - first;
  if (result < 0)
    result += m_maxBcId + 1;
  
  return result;
}
#endif 
