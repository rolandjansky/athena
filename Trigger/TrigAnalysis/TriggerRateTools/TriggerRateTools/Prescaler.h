/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerRateTools_Prescaler
#define TriggerRateTools_Prescaler

#include <vector>

class Prescaler {

private:
    std::vector<double> *i_list;
    std::vector<double> *i_res;
    double i_sum;
    int i_curr_level;
    int i_level_max;
    double i_total;
    
    long double applyPrescales(std::vector<long double> *list, std::vector<long double> *res, long double sum, int curr_level, int level_max, long double total);

public:
    Prescaler();
    Prescaler(std::vector<double> *_list, std::vector<double> *_res, double _sum, int _curr_level, int _level_max, double _total);
    virtual ~Prescaler();

    double getPnum();
        
};

#endif
