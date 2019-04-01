/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVMODULE_H
#define LARHV_EMBHVMODULE_H

class EMBHVManager;
class EMBHVElectrode;

class EMBHVModule
{
  public:
    EMBHVModule(const EMBHVManager *manager
		, unsigned int iSide
		, unsigned int iEta
		, unsigned int iPhi
		, unsigned int iSector);

    // Destructor
    virtual ~EMBHVModule();

    // Get eta Index
    unsigned int getEtaIndex() const;

    // Get phi index
    unsigned int getPhiIndex() const;

    // Get sector index
    unsigned int getSectorIndex() const;

    // Get num electrodes
    unsigned int getNumElectrodes() const
    {
      return 64;
    }

    // Gets an electrode
    const EMBHVElectrode& getElectrode(unsigned int iElectrode) const;

    // Side Index (0=Negative, 1=Positive)
    unsigned int getSideIndex() const;

    double getEtaMin() const;
    double getEtaMax() const;
    double getPhiMin() const;
    double getPhiMax() const;

    const EMBHVManager& getManager() const;

  private:
    // Illegal operation
    EMBHVModule& operator=(const EMBHVModule& right);
    // Illegal operation
    EMBHVModule(const EMBHVModule& right);

    class Clockwork;
    Clockwork *m_c;

    friend class ImaginaryFriend;
};

#endif
