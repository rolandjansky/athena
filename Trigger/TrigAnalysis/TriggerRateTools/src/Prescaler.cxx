/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <stdlib.h>
//#include "Riostream.h"
//#include "TROOT.h"
//#include "TClass.h"
#include <math.h>
#include <vector>
#include "TriggerRateTools/Prescaler.h"
//#include "TString.h"


//ClassImp(Prescaler)

/* Normal Constructor */
Prescaler::Prescaler(std::vector<double> *_list, std::vector<double> *_res, double _sum, int _curr_level, int _level_max, double _total) {
    i_list = _list;
    i_res = _res;
    i_sum = _sum;
    i_curr_level = _curr_level;
    i_level_max = _level_max;
    i_total = _total;
    
}

/* Detructor */
Prescaler::~Prescaler() {
}

/***********/
/* Private */
/***********/

long double Prescaler::applyPrescales(std::vector<long double> *list, std::vector<long double> *res, long double sum, int curr_level, int level_max, long double total) {
	if (curr_level == level_max) {
                return total;
        }

        std::vector<long double> n_res;

        for (int i = 0; i < level_max; i++) {
                n_res.push_back(0.0);
        }

        for (int i = 0; i < level_max; i++) {
                n_res[i] = sum;
                int j = i;

                while (j >= 0) {
                        n_res[i] = n_res.at(i) - res->at(j);
                        j--;
                }
                n_res[i] = list->at(i)*n_res.at(i);
        }
        long double new_sum = 0.0;

        for (int i = 0; i < level_max; i++) {
                new_sum = new_sum + n_res.at(i);
        }

        total = total + pow((-1),(curr_level))*new_sum;

        return applyPrescales(list, &n_res, new_sum, curr_level+1, level_max, total);

}

/***********/
/* Getters */
/***********/

double Prescaler::getPnum() {
    if (i_level_max > 5) {

	int level_1 = (int)(i_level_max/2);
	int level_2 = i_level_max - level_1;

        std::vector<double> a_list_1;
        std::vector<double> a_list_2;
        std::vector<double> a_res_1;
        std::vector<double> a_res_2;

        for (int i = 0; i < i_level_max; i++) {
            a_list_1.push_back(0.0);
            a_list_2.push_back(0.0);
            a_res_1.push_back(0.0);
            a_res_2.push_back(0.0);
        }

        for (int i = 0; i < level_1; i++) {
            a_list_1[i] = i_list->at(i);
            a_res_1[i] = i_res->at(i);
        }
        for (int i = 0; i < level_2; i++) {
            a_list_2[i] = i_list->at(i+level_1);
            a_res_2[i] = i_res->at(i+level_1);
        }

        std::vector<double> prescalers_res;
        std::vector<double> prescalers_tmp;

	prescalers_tmp.push_back(0.0);
	prescalers_tmp.push_back(0.0);

	//printf("Splitting in 2 prescalers\n");
	Prescaler *prescaler_1 = new Prescaler(&a_list_1, &a_res_1, 1.0, 0, level_1, 0.0);
	Prescaler *prescaler_2 = new Prescaler(&a_list_2, &a_res_2, 1.0, 0, level_2, 0.0);

	prescalers_res.push_back(prescaler_1->getPnum());
	prescalers_res.push_back(prescaler_2->getPnum());
	//printf("prescaler res: %f\t%f\n", prescalers_res[0], prescalers_res[1]);

	Prescaler *prescaler_comb = new Prescaler(&prescalers_res, &prescalers_tmp, 1.0, 0, 2, 0.0);

        return prescaler_comb->getPnum();


    } else {
        std::vector<long double> a_list;
        std::vector<long double> a_res;

        for (int i = 0; i < i_level_max; i++) {
            a_list.push_back( (long double) i_list->at(i));
            a_res.push_back( (long double) i_res->at(i));
        }
        long double a_sum = i_sum;
        long double a_total = i_total;
        return (double)applyPrescales(&a_list, &a_res, a_sum, i_curr_level, i_level_max, a_total);
    }
}



