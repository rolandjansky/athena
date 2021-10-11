/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORAMPDATA_H
#define L1CALORAMPDATA_H

#include <vector>
#include <utility>
#include <cmath>

/**
 * Class to store the measured energy per step for either calorimeter or level-1.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */
class L1CaloRampStep 
{
 public:
    L1CaloRampStep() : m_nEntries(0) , m_w(0.), m_w2(0.) {}
    virtual ~L1CaloRampStep() {}
    
    void addEvent(double energy);
    double mean() const;
    double rms() const;

 private:
    unsigned int m_nEntries;
    double m_w;
    double m_w2;
};


/**
 * Transient class to store the RampData calculated from calibration runs.
 *
 * Steps start to count from 0.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */
class L1CaloRampData final
{
 public:
    // first L1CaloRampStep is calorimeter data, second is level-1 trigger data
    typedef std::pair<L1CaloRampStep, L1CaloRampStep> L1CaloRampDataPoint;
    
    L1CaloRampData() : m_rampDataVector(1), m_iStep(0) {}
    ~L1CaloRampData() = default;

    // adds calorimeter and level-1 trigger energiy to energy step i
    void addData(unsigned int step, const std::pair<double, double>& energy);
    void addData(unsigned int step, double calorimeter, double level1);
    void addData(unsigned int step, const L1CaloRampDataPoint& data);

    // adds calorimeter and level-1 trigger energy to the current energy step
    void addData(const std::pair<double, double>& l1calo);
    void addData(double calorimeter, double level1);
    
    // sets the current energy step
    void setStep(unsigned int step);

    // moves to the next energy step
    void nextStep();

    // returns the total number of energy steps
    unsigned int getNSteps() const;
    
    // returns the data of step i
    const L1CaloRampDataPoint* getStep(unsigned int step) const;
    
    // returns the calorimeter data of step i
    const L1CaloRampStep* getCalo(unsigned int step) const;
    
    // return the level-1 trigger data of step i
    const L1CaloRampStep* getLevel1(unsigned int step) const;

 private:
    typedef std::vector<L1CaloRampDataPoint> L1CaloRampDataVector;
    
    L1CaloRampDataVector m_rampDataVector;
    unsigned int m_iStep;
};

#endif //L1CALORAMPDATA_H
