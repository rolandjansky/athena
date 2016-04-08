/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "munkres.h"
#include "boost/io/ios_state.hpp"

munkres::munkres(matrix_type costs):
  m_costmatrix(costs),
  m_costs_orig(costs),
  m_step(0),
  m_dim(costs.size())
{
    m_rowIsCovered = std::vector<bool>(m_dim,false);
    m_colIsCovered = std::vector<bool>(m_dim,false);
    vec_type maskrow(m_dim,0);
    for(int i=0;i<m_dim;++i) m_maskmatrix.push_back(maskrow);
}

munkres::result_type munkres::run(vec_type& costvector, bool debug){
  if(debug) printcosts();
  bool done = false;
  m_step = 1;
  while(!done){
    if(debug){
      std::cout << "doing step " << m_step << std::endl;
      printcosts();
      printmask();
    }
    switch(m_step){
      case 1:
        step_one();
        break;
      case 2:
        step_two();
        break;
      case 3:
        step_three();
        break;
      case 4:
        step_four();
        break;
      case 5:
        step_five();
        break;
      case 6:
        step_six();
        break;      
      case 7:
        done = true;
        break;
    }
  }
  if(debug) std::cout << "done running munkres algorithm: " << std::endl;
  result_type result;
  costvector.clear();

  for(int row=0;row<m_dim;++row){
    int col = find_in_row(row,kStar);
    result.push_back(col);
    costvector.push_back(m_costs_orig[row][result.back()]);
  }

  if(debug){
    printcosts();
    for(uint i=0;i<result.size();++i) std::cout << "row: " << i << " -> col: " << result[i] << " cost: " << m_costs_orig[i][result[i]]<< std::endl;
  }
  return result;
}

void munkres::step_one(){
  //subtract row minimum from each row element
  //then go to step 2
  for(int row=0;row<m_dim;++row){
    double min = m_costmatrix[row][0];
    for(int col=0;col<m_dim;++col){ // find minimum
      if(m_costmatrix[row][col] < min) min = m_costmatrix[row][col];
    }
    for(int col=0;col<m_dim;++col){ // subtract from all elements
      m_costmatrix[row][col] -= min;
    }
  }
  m_step = 2;
}

void munkres::step_two(){
  //star a zero matrix element if there is no other zero in it column or row
  //we will cover each row and column that have a zero so we know we don't have to 
  //check these rows and columns anymore for zeros to star
  //then go to step 3

  for(int row=0;row<m_dim;++row){
    for(int col=0;col<m_dim;++col){
      if(!m_rowIsCovered[row] && !m_colIsCovered[col] && m_costmatrix[row][col] == 0){
        m_rowIsCovered[row] = true;
        m_colIsCovered[col] = true;
        m_maskmatrix[row][col] = 1;
      }
    }
  }
  //reset cover vectors
  for(int i=0;i<m_dim;++i){
    m_rowIsCovered[i] = false;
    m_colIsCovered[i] = false;    
  }
  m_step = 3;
}

void munkres::step_three(){
  //cover each column with a starred zero
  //if we have m_dim coered columns we're done and exit
  //else we continue with step four

  for(int row=0;row<m_dim;++row){
    for(int col=0;col<m_dim;++col){
      if(m_maskmatrix[row][col] == 1) m_colIsCovered[col] = true;   // this is a starred zero cover column
    }
  }

  int nCoveredCols = 0;
  for(int col=0;col<m_dim;++col) if(m_colIsCovered[col]) nCoveredCols++;
  
  m_step = (nCoveredCols == m_dim) ? 7 : 4;
}

void munkres::find_a_zero(int& row, int& col){
  for(row=0;row<m_dim;++row){
    for(col=0;col<m_dim;++col){
      if(m_costmatrix[row][col] == 0 && !m_colIsCovered[col] && !m_rowIsCovered[row]) return;
    }
  }
  //not returned until now set flags
  row = -1;
  col = -1;
}

int munkres::find_in_row(const int row, markertype what){
  for(int col = 0;col<m_dim;++col) if(m_maskmatrix[row][col] == what) return col;
  //not returned until now
  return -1;
}

int munkres::find_in_col(const int col, markertype what){
  for(int row = 0;row<m_dim;++row) if(m_maskmatrix[row][col] == what) return row;
  //not returned until now
  return -1;  
}

void munkres::step_four(){
  // Find a noncovered zero and prime it.
  // If there is no starred zero in the row containing this primed zero, Go to Step 5.
  // Otherwise, cover this row and uncover the column containing the starred zero.
  // Continue in this manner until there are no uncovered zeros left.
  // Save the smallest uncovered value and Go to Step 6.

  bool done = false;
  while(!done){
    int row, col;
    find_a_zero(row,col);
    if(row == -1){
      //there is no uncovered zero go to step 6
      done = true;
      m_step = 6;
    }
    else{
      // std::cout << "found uncovered zero at " << row << ", " << col << std::endl;
      //check if there is a starred zero in this row
      m_maskmatrix[row][col]=2;
      int starred0atcol = find_in_row(row,kStar);
      if(starred0atcol != -1){
        m_rowIsCovered[row] = true;
        m_colIsCovered[starred0atcol] = false;
        // printcosts();
        // printmask();
      }else{
        done = true;
        m_step = 5;
      }
    }
  }
}

void munkres::augment_path(const std::vector<coords>& p){
  for(unsigned int i = 0;i<p.size();++i){
    const int row = p[i].first;
    const int col = p[i].second;
    if(m_maskmatrix[row][col] == kStar){
      m_maskmatrix[row][col] = 0; //unstar each star;
    }
    else{//primed zeros
      m_maskmatrix[row][col] = kStar; //star each primed and unprime it;
    }
  }
}

void munkres::erase_primes_and_covers(){
  for(int row=0;row<m_dim;++row){
    m_rowIsCovered[row] = false;
    m_colIsCovered[row] = false;//queadratic matrix
    for(int col=0;col<m_dim;++col){
      if(m_maskmatrix[row][col] == kPrime) m_maskmatrix[row][col] = 0;
    }
  }
}

void munkres::step_five(){
  // Construct a series of alternating primed and starred zeros as follows.
  // Let Z0 represent the uncovered primed zero found in Step 4.
  // Let Z1 denote the starred zero in the column of Z0 (if any).
  // Let Z2 denote the primed zero in the row of Z1 (there will always be one). 
  // Continue until the series terminates at a primed zero that has no starred zero in its column.
  // Unstar each starred zero of the series, star each primed zero of the series,
  // erase all primes and uncover every line in the matrix.  Return to Step 3. 

  bool done = false;
  std::vector<coords> path;
  int row,col;
  find_a_zero(row,col);

  path.push_back(coords(row,col));

  int n = 0;
  while(!done && n<4){n++;
    int starred0atrow = find_in_col(path.back().second,kStar);
    if(starred0atrow > -1){
      path.push_back(coords(starred0atrow,path.back().second));
    }
    else{
      done = true;
    }
    if(!done){
      int primed0atcol = find_in_row(path.back().first,kPrime);
      path.push_back(coords(path.back().first,primed0atcol));
    }
  }
  // std::cout << "found path: " << std::endl;
  // for(unsigned int i=0;i<path.size();++i){
  //   std::cout << "(" << path[i].first << "," << path[i].second << ")" <<  ((i<path.size()-1) ? "->" : "" );
  // } std::cout << std::endl;
  
  augment_path(path);
  erase_primes_and_covers();
  
  m_step = 3;
}

double munkres::find_min_uncov(){
  double min = 1e10;
  for(int row=0;row<m_dim;++row){
    for(int col=0;col<m_dim;++col){
      if(!m_rowIsCovered[row] && !m_colIsCovered[col] && m_costmatrix[row][col] < min){
        min = m_costmatrix[row][col];
      }
    }
  }
  return min;
}

void munkres::step_six(){
  // Add the value found in Step 4 to every element of each covered row,
  // and subtract it from every element of each uncovered column.
  // Return to Step 4 without altering any stars, primes, or covered lines.
  // Notice that this step uses the smallest uncovered value in the cost matrix to modify the matrix. 

  double minuncov = find_min_uncov();
  for(int row=0;row<m_dim;++row){
    for(int col=0;col<m_dim;++col){
      if(m_rowIsCovered[row])   m_costmatrix[row][col] += minuncov;
      if(!m_colIsCovered[col])  m_costmatrix[row][col] -= minuncov;
    }
  }
  m_step = 4;
}

void munkres::printmatrix(const matrix_type& m){
  boost::io::ios_all_saver ias(std::cout);
  std::cout << std::setw(5) << std::setprecision(3) << "cov|";
  for(int col=0;col<m_dim;++col){
    std::cout << std::setw(7) << std::setprecision(3) << (m_colIsCovered[col] ? "+|" : "|");
  } std::cout << std::endl;

  for(int row=0;row<m_dim;++row){
    std::cout << std::setw(5) << std::setprecision(3) << (m_rowIsCovered[row] ? "+ |" : "|");
    for(int col=0;col<m_dim;++col){
      std::cout << std::setw(5) << std::setprecision(3) << m[row][col];
      if(m_maskmatrix[row][col] == kPrime)  std::cout << "'";
      if(m_maskmatrix[row][col] == kStar)   std::cout << "*";
      else                                  std::cout << " ";
      std::cout << "|";
    } std::cout << std::endl;
  }
}
