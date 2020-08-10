/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @class  : Signature
 *
 * @brief  one signature, for instance
 *         2 muons AND 4 electrons AND 1 jet ...
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Signature.h,v 1.21 2007-09-06 16:29:44 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_SIGNATURE_H
#define TRIGSTEERING_SIGNATURE_H

#include <map>
#include <vector>
#include "TrigSteeringEvent/Enums.h"

#include "TrigSteering/Sequence.h"

#include <iosfwd>

namespace TrigConf {
  class HLTSignature;
}


class MsgStream;

namespace HLT {

  class ISequenceProvider;

  class AlgoConfig;
  class TriggerElement;

  /**
     @class Signature
     This class represents one signature, for instance 2 muons AND 4 electrons AND 1 jet

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class Signature
  {

  public:

    Signature(HLT::AlgoConfig* config); //!< minimal constructor

    Signature(const TrigConf::HLTSignature* configSig,
              HLT::ISequenceProvider* seqs, HLT::AlgoConfig* config); //!< constructor for normal running

    ~Signature() { } //!< destructor, does nothing

    bool reset(); //!< is called for every new event, resets everything to default.
    bool resetROBRequestPreparation();  //!< call at begin of event, calls resetPreparation for all sequences

    /** @brief try to satisfy this signature by running the sequences and then compare
     	       the result of the sequences with the requirements of this Signature

	@param pass was the execute successful ?
    */
    HLT::ErrorCode execute( bool& pass );

     /** @brief execute the ROB request preparation for all sequences of this signature
     - errors within the seqences are propagated upward
     */
    HLT::ErrorCode prepareRobRequests(); //!< execute the ROB request preparation for this signature

    void addItem(unsigned int reqTeCount, HLT::Sequence* reqSeq); //!< add a requirement to this Signature

    /** @brief templated print so that one can print to std::cout but also to the athena msg stream
     */
    template <class stream>
    void print(stream& os) const
    {
      os << " All Sequences of this Siganture ([# required TEs/# of TEs got, TE type]): ";
      // loop over all Sequences:
      for (unsigned int i = 0; i < m_requiredTEs.size(); ++i) {
	os << "[" << m_requiredTEs[i].multiplicity << "/" << m_requiredTEs[i].sequence->outputTEs().size() << " x " << m_requiredTEs[i].sequence->outputTEType()  << "], ";
      }
    }


    const TrigConf::HLTSignature* getConfigSignature() const { return m_configSig; } //!< const getter for underlying signature
    const std::vector< const HLT::Sequence* > getSequences() const;                  //!< const getter for vector of all Sequence objects
    bool getFoundOverlap() const { return m_foundOverlap; }                          //!< const getter of Overlap flag for one event

    bool isValid() const; //!< valid Signature ? ie do we have at least one Sequence

    const std::vector<std::vector< const TriggerElement*> >& makeAllTECombinations() const; //!< return a all active TEs ordered (outer vector) by type
    const std::vector< const TriggerElement* >& makeAllTEsFlat() const; //!< return a all active TEs ordered (outer vector) by type

    const std::vector<const TriggerElement*>& makeTECombination() const; //!< make sure the TEs that satisfy this Signature come from different RoI objects

  private:

    bool checkRoIOverlap() const; //!< helper method for TE combinations

    bool recurseRoICombs(std::vector< const TriggerElement* >& theCombs,
                         std::vector< std::vector< const TriggerElement* > >& remainingTEs,
                         std::vector< unsigned int >& numMissingTEs) const;  //!< helper method for TE combinations

    bool addIfNoOverlap(const TriggerElement* te,
                        std::vector<const TriggerElement*>& theComb) const;  //!< helper method for TE combinations


    /** @brief Requirements of this Signature.
	Vector holding: TE types - multiplicity.
	E.g. (hashID(e25i),2), (hashID(mu20),1) = 2 x e25i + 1 x mu20
	and additional decorations of thereof
    */
    struct RequiredTEs{
      RequiredTEs(int mult, HLT::Sequence* seq): multiplicity(mult), sequence(seq) {}
      RequiredTEs(const RequiredTEs& r) = default;
      RequiredTEs& operator=(const RequiredTEs& r) {
	const_cast<unsigned int&>((this->multiplicity)) = r.multiplicity; 
	sequence = r.sequence; 
	return *this;
      }
      const unsigned int multiplicity;
      HLT::Sequence* sequence;
    };
    std::vector<RequiredTEs> m_requiredTEs;


    HLT::AlgoConfig* m_config;                 //!< config object, holding common variables
    const TrigConf::HLTSignature* m_configSig; //!< Underlying config signature

    bool m_foundOverlap;                       //!< have we found an overlap ?

    mutable std::vector<const TriggerElement*> m_comb; //!< Remember one TE combination that actually satisfies this Signature (w/o overlap)

    mutable std::vector< std::vector< const TriggerElement* > > m_allCombinations;
    mutable std::vector< const TriggerElement* > m_allFlatCombinations;

  };

/** Overload of << operator for both, MsgStream and std::ostream */
MsgStream& operator << (MsgStream&, const HLT::Signature&);
std::ostream& operator << (std::ostream&, const HLT::Signature&);

} // end namespace

#endif
