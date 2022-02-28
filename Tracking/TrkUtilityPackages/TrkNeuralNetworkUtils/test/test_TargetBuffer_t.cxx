/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#undef NDEBUG
#include "../src/TargetBuffer_t.h"
#include <utility>

#include <vector>
#include <iostream>
#include <limits>
#include <cassert>
#include <algorithm>
#include <cmath>

#include <cstdlib>
#include <numeric>

// missing with NDEBUG defined when compiling TargetBuffer_t.cxx
// so adding here and not linking TargetBuffer_t.o
#include <sstream>
#include "../src/TargetBuffer_t.icc"

namespace ndebug {
   // include the buffer definition again but now in NO DEBUG mode
   // to check that this can also be compiled and works
#  define NDEBUG
#  undef TargetBuffer_t_H
#  include "../src/TargetBuffer_t.h"
#  include "../src/TargetBuffer_t.icc"
#  undef NDEBUG
}


namespace TTN_internal{
    const static int MAX_LAYER_LENGTH = 1000;
    static double tmpdata[2*MAX_LAYER_LENGTH];
    static double * tmp_array[2] = {
      &(tmpdata[0]), &(tmpdata[MAX_LAYER_LENGTH]) };
}

class Test2 {
public:
  unsigned int mnInput=4;
  std::vector< unsigned int > mnHiddenLayerSize {8,3,1};

  std::vector< std::vector<double>  > mWeightMatrices;
  std::vector< std::vector<double>  > mThresholdVectors;

  unsigned int mnHidden;
  unsigned int mnOutput;
  unsigned int m_bufferSizeMax;

  double maxExpValue;

  Test2(unsigned int input=4, std::vector<unsigned int> hidden_layer_sizes = {8,3,1})
    : mnInput(input),
      mnHiddenLayerSize(std::move(hidden_layer_sizes))
  {
    maxExpValue = log(std::numeric_limits<double>::max());

    mnHidden=mnHiddenLayerSize.size();
    mWeightMatrices.resize(mnHidden);
    mThresholdVectors.resize(mnHidden);
    --mnHidden;
    mnOutput=mnHiddenLayerSize[mnHiddenLayerSize.size()-1];
    unsigned int n_input=mnInput;
    m_bufferSizeMax=n_input;

    double scale=2./RAND_MAX;

    for (unsigned int layer_i=0; layer_i<mnHiddenLayerSize.size(); ++layer_i) {
      mWeightMatrices[layer_i].resize(mnHiddenLayerSize[layer_i]*n_input);
      for (double &val:  mWeightMatrices[layer_i]) {
        val=rand()*scale-1;
      }
      mThresholdVectors[layer_i].resize(mnHiddenLayerSize[layer_i]);
      for (double &val:  mThresholdVectors[layer_i]) {
        val=rand()*scale-1;
      }
      if (m_bufferSizeMax < mnHiddenLayerSize[layer_i] ) {
          m_bufferSizeMax=mnHiddenLayerSize[layer_i];
      }
      n_input=mnHiddenLayerSize[layer_i];
    }
    ++m_bufferSizeMax;
  }

  double sigmoid(double x) const {
    if (-2*x >= maxExpValue){
      return 0.;
    }
    return 1./(1.+exp(-2*x));
  }

  template<class T_Buffer_t, class T_ConstBuffer_t, class T_DoubleBuffer_t>
  std::vector<double>
    calculateOutputValues(const std::vector<double>& input)
    const
  {
    if (input.size() != mnInput)
      {
        std::cerr << "TTrainedNetwork WARNING Input size: " << input.size()
                  << " does not match with network: " << mnInput << std::endl;
        return {};
      }

    T_DoubleBuffer_t tmp_array;
    tmp_array.clear(m_bufferSizeMax);          // make sure it is big enough and initialise with zero

    const unsigned nTargetLayers(mnHidden+1);
    const unsigned lastTargetLayer(mnHidden);
    unsigned nSource = mnInput, nTarget(0);
    T_ConstBuffer_t source(input);
    const double * weights(nullptr);
    const double * thresholds(nullptr);
    double nodeVal(0);

    for (unsigned iLayer = 0; iLayer < nTargetLayers; ++iLayer) {
      //Find data area for target layer:
      nTarget = ( iLayer == lastTargetLayer ?
                  mnOutput :
                  mnHiddenLayerSize[iLayer] );
      T_Buffer_t target( tmp_array[iLayer] );

      //Transfer the input nodes to the output nodes in this layer transition:
      weights = mWeightMatrices[iLayer].data();
      thresholds = mThresholdVectors[iLayer].data();
      for (unsigned inodeTarget=0;inodeTarget<nTarget;++inodeTarget) {
        nodeVal = 0.0;//Better would be "nodeVal = *thresholds++" and
        //remove the line further down, but this way we
        //get exactly the same results that an earlier
        //version of the package gave.
        const double * weights_tmp = weights++;
        const double * source_end(&(source.upper_bound_at(nSource)));
        for (const double* source_iter = &source[0];
             source_iter != source_end; ++source_iter)
          {
            nodeVal += (*weights_tmp) * (*source_iter);
            weights_tmp += nTarget;
          }
        nodeVal += *thresholds++;//see remark above.
        target[inodeTarget] = ( iLayer == lastTargetLayer )
          ? nodeVal : sigmoid(nodeVal);
      }
      //Prepare for next layer transition:
      source = target;
      nSource = nTarget;
    }

    // take the other half buffer to store the normalised output
    T_Buffer_t norm_target=tmp_array[nTargetLayers];
    T_Buffer_t target=tmp_array[(nTargetLayers-1)];
    const double sumLastLayer =
      std::accumulate(&target[0], &target[nTarget], 0.0 );
    const double normFact = sumLastLayer ? 1.0/sumLastLayer : 0.0;
    for (unsigned i = 0; i < nTarget; ++i)
      norm_target[i] = normFact * target[i];
    // copy the half buffer to the front of the full buffer
    // if necessary and shrink the array
    return tmp_array.releaseData(nTarget,nTargetLayers);

  }



  std::vector<double>
    calculateOutputValuesOrig(const std::vector<double>& input)
    const
  {
    using namespace TTN_internal;
      // This method is now highly optimised (apart from the potential use
      // of a cheaper sigmoid function). Please be very careful changing
      // anything here since it is used heavily in reconstruction during
      // Pixel clusterization - Thomas Kittelmann, Oct 2011.

      if (input.size() != mnInput)
        {
          std::cerr << "TTrainedNetwork WARNING Input size: " << input.size()
                    << " does not match with network: " << mnInput << std::endl;
          return {};
        }

    const unsigned nTargetLayers(mnHidden+1);
    const unsigned lastTargetLayer(mnHidden);
    unsigned nSource = mnInput, nTarget(0);
    const double * source = &(input[0]);
    double * target(nullptr);
    const double * weights(nullptr);
    const double * thresholds(nullptr);
    double nodeVal(0);

    for (unsigned iLayer = 0; iLayer < nTargetLayers; ++iLayer) {
      //Find data area for target layer:
      nTarget = ( iLayer == lastTargetLayer ?
                  mnOutput :
                  mnHiddenLayerSize[iLayer] );
      target = tmp_array[iLayer%2];

      //Transfer the input nodes to the output nodes in this layer transition:
      weights = mWeightMatrices[iLayer].data();
      thresholds = mThresholdVectors[iLayer].data();
      for (unsigned inodeTarget=0;inodeTarget<nTarget;++inodeTarget) {
        nodeVal = 0.0;//Better would be "nodeVal = *thresholds++" and
        //remove the line further down, but this way we
        //get exactly the same results that an earlier
        //version of the package gave.
        const double * weights_tmp = weights++;
        const double * source_end(&(source[nSource]));
        for (const double* source_iter = source;
             source_iter != source_end; ++source_iter)
          {
            nodeVal += (*weights_tmp) * (*source_iter);
            weights_tmp += nTarget;
          }
        nodeVal += *thresholds++;//see remark above.
        target[inodeTarget] = ( iLayer == lastTargetLayer )
          ? nodeVal : sigmoid(nodeVal);
      }
      //Prepare for next layer transition:
      source = target;
      nSource = nTarget;
    }

    std::vector<double> result(nTarget);
    const double sumLastLayer =
      std::accumulate(&target[0], &target[nTarget], 0.0 );
    const double normFact = sumLastLayer ? 1.0/sumLastLayer : 0.0;
    for (unsigned i = 0; i < nTarget; ++i)
      result[i] = normFact * target[i];

    return result;
  }

};


// some test code for DoubleBuffer_t and Buffer_t
template<class T_Buffer_t,class T_ConstBuffer_t, class T_DoubleBuffer_t>
std::vector<double> exec(const std::vector<double> &input, unsigned int n_iter,unsigned int n_results) {
  assert( n_results <= input.size());
  std::vector<double>::size_type n=input.size();
  T_ConstBuffer_t source(input);
  T_DoubleBuffer_t tmp_array;
  tmp_array.clear(n);

  for (unsigned int i=0;i<n_iter; ++i) {
    T_Buffer_t target(tmp_array[i]);
    for (unsigned int j=0; j<n/2; ++j) {
      target[j+n-n/2]=(i+1)+source[j];
      target[j]=(i+1)+source[j+n-n/2];
    }
    // for (unsigned int j=0; j<n; ++j) {
    //   std::cout << target[j] << " ";
    // }
    // std::cout << std::endl;
    source=target;
  }
  {
  T_Buffer_t target(tmp_array[n_iter]);
  T_ConstBuffer_t source(tmp_array[(n_iter-1)]);
  for (unsigned int j=0; j<n_results; ++j) {
    target[j]=0;
  }
  for (unsigned int i=0; i<n/n_results; ++i) {
    for (unsigned int j=0; j<n_results; ++j) {
      target[j]+=source[i*n_results+j];
    }
  }
  // for(unsigned int j=0; j<n_results; ++j) {
  //   std::cout << source[j] << " ";
  // }
  // std::cout << std::endl;
  // for(unsigned int j=0; j<n_results; ++j) {
  //   std::cout << target[j] << " ";
  // }
  // std::cout << std::endl;

  return tmp_array.releaseData(n_results,n_iter);
  }
}



template<class T_Buffer_t, class T_ConstBuffer_t, class T_DoubleBuffer_t>
std::vector<double> compute(unsigned int n, unsigned int n_iter, unsigned int n_results) {

  // n*=2;
  std::vector<double> input(n,1);

  std::fill(input.begin()+n/2,input.end(),0);
  return exec<T_Buffer_t, T_ConstBuffer_t, T_DoubleBuffer_t>(input,n_iter,n_results);
}

template <typename T>
bool fcomp(T a, T b) {
  return std::abs(a-b)/(a+b)<4*std::numeric_limits<T>::epsilon();
}

void testConstCorrectnessAndRangeCheck() {
// #define check_compile_error
#ifdef check_compile_error
  {
    std::vector<double> input {1,2,3,4};
    const std::vector<double> &const_input(input);
    TTN::Buffer_t target(const_input);     // ** compile error:  error: binding ..to reference ... discards qualifiers
    for (unsigned int i=0; i<4; ++i) {
      TTN::ConstBuffer_t source(const_input);
      source[i]=0;                         // ** compile error: error: assignment of read-only
    }
  }

#endif
  {
    std::vector<double> input {1,2,3,4};
    TTN::ConstBuffer_t source(input);
    {
      bool caught_expected_exception=false;

      try {
        if (source[4] == 1 ) {
          std::cerr << "Should not reach this." << std::endl;
        }
      }
      catch (std::exception &err) {
        std::cerr << "Caught expected exception: " << err.what() << std::endl;
        caught_expected_exception=true;
      }
      assert( caught_expected_exception == true );
    }

    TTN::DoubleBuffer_t tmp_array;
    tmp_array.clear(input.size());

    for (unsigned int i=0; i<4; ++i) {
      TTN::Buffer_t target(tmp_array[i]);
      bool caught_expected_exception=false;
      const double *end_ptr = &(target.upper_bound_at(4));
      try {
        for (unsigned int i=0; i<=4; ++i) {
          double *ptr=&(target[i]);
          assert( ptr < end_ptr ) ;
        }
      }
      catch (std::exception &err) {
        std::cerr << "Caught expected exception: " << err.what() << std::endl;
        caught_expected_exception=true;
      }
      assert( caught_expected_exception == true );
    }
  }

}

void check(std::vector<double> &&in, const std::vector<double> &expect) {
   // for (double val: in) {
   //   std::cout << val << " ";
   // }
   // std::cout << std::endl;
   assert( in.size() == expect.size());
   std::vector<double>::const_iterator expect_iter=expect.begin();
   for (double val: in) {
     assert( expect_iter != expect.end());
     assert( fcomp(val,*expect_iter));
     ++expect_iter;
   }
 }

int main()
{
  Test2 test(4,{16,8,3,2});
  // assembly test
  std::vector<double> result2( test.calculateOutputValues<TTN::Buffer_t,TTN::ConstBuffer_t, TTN::DoubleBuffer_t>({1.,2.,3.,4.})  );
  std::vector<double> result1( test.calculateOutputValuesOrig({1.,2.,3.,4.}) );
  std::vector<double> result3( test.calculateOutputValues<ndebug::TTN::Buffer_t,ndebug::TTN::ConstBuffer_t, ndebug::TTN::DoubleBuffer_t>({1.,2.,3.,4.})  );
  assert( result1 == result2 );
  assert( result1 == result3 );

  //std::cout <<"start " << __FILE__ << std::endl;
  // in  1100
  // [0] 1122
  // [1] 4433
  // [2] 6677
  // out 13 13
  check(compute<TTN::Buffer_t,TTN::ConstBuffer_t, TTN::DoubleBuffer_t>(4,3,2),{13.,13.});
  check(compute<ndebug::TTN::Buffer_t,ndebug::TTN::ConstBuffer_t,ndebug::TTN::DoubleBuffer_t>(4,3,2),{13.,13.});

  // out 13+13
  check(compute<TTN::Buffer_t,TTN::ConstBuffer_t,TTN::DoubleBuffer_t>(4,3,1),{26.});
  check(compute<ndebug::TTN::Buffer_t,ndebug::TTN::ConstBuffer_t,ndebug::TTN::DoubleBuffer_t>(4,3,1),{26.});

  // in  11000
  // [0] 11022
  // [1] 44033
  // [2] 66077
  // [3] 11 11 0 10 10
  // out 11 21
  check(compute<TTN::Buffer_t,TTN::ConstBuffer_t,TTN::DoubleBuffer_t>(5,4,2),{11,21});
  check(compute<ndebug::TTN::Buffer_t,ndebug::TTN::ConstBuffer_t,ndebug::TTN::DoubleBuffer_t>(5,4,2),{11,21});

  testConstCorrectnessAndRangeCheck();
  return 0;
}
