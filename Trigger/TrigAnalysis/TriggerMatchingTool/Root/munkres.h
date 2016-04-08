/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUNKRES_H
#define MUNKRES_H

#include <iostream>
#include <vector>
#include <iomanip>
#include <map>

// adapted for C++ from http://csclab.murraystate.edu/bob.pilgrim/445/munkres.html
// standalone code at: https://github.com/lukasheinrich/munkres-cpp

class munkres{
public:
  typedef std::vector<double> vec_type;
  typedef std::vector<vec_type > matrix_type;
  typedef std::pair<int,int> coords;
  typedef std::vector<int> result_type;

  enum markertype {kStar = 1, kPrime = 2};

  munkres(matrix_type costs);
  void printcosts(){printmatrix(m_costmatrix);}
  result_type run(vec_type& costvector, bool debug = false);

private:
  void step_one();
  void step_two();
  void step_three();
  void step_four();
  void step_five();
  void step_six();

  void printmask() {printmatrix(m_maskmatrix);}
  void printmatrix(const matrix_type&);

  void    find_a_zero(int& row,int& col);
  int     find_in_row(const int row, markertype what);
  int     find_in_col(const int col, markertype what);
  double  find_min_uncov();

  void  augment_path(const std::vector<coords>& path);
  void  erase_primes_and_covers();

  matrix_type m_costmatrix;
  matrix_type m_costs_orig;
  matrix_type m_maskmatrix;
  int m_step;

  std::vector<bool>   m_rowIsCovered;
  std::vector<bool>   m_colIsCovered;
  const int m_dim;
};
#endif
