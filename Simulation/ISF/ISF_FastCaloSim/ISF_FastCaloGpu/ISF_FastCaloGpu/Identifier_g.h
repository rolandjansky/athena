/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_Identifier_GPU
#define ISF_FASTCALOGPU_Identifier_GPU


#  ifdef __CUDACC__
#    define CUDA_HOSTDEV __host__ __device__
#  else
#    define CUDA_HOSTDEV
#  endif

class Identifier_Gpu {
public:
  typedef long long value_type;

  typedef enum bit_defs_enum {
    NBITS    = sizeof( value_type ) * 8, // bits per byte
    MAX_BIT  = ( static_cast<value_type>( 1 ) << ( NBITS - 1 ) ),
    ALL_BITS = ~( static_cast<value_type>( 0 ) )
  } bit_defs;

  typedef enum max_value_type_enum {
    // max_value = 0xFFFFFFFFFFFFFFFFULL
    max_value = ~( static_cast<value_type>( 0 ) )
  } max_value_type;

  CUDA_HOSTDEV Identifier_Gpu() : m_id( max_value ){};
  CUDA_HOSTDEV Identifier_Gpu( const Identifier_Gpu& value ) : m_id( value.m_id ){};
  CUDA_HOSTDEV Identifier_Gpu( value_type value ) : m_id( value ){};

  CUDA_HOSTDEV operator value_type() const { return m_id; }

  CUDA_HOSTDEV Identifier_Gpu& operator=( const Identifier_Gpu& old ) {
    m_id = old;
    return ( *this );
  };
  CUDA_HOSTDEV Identifier_Gpu& operator=( value_type value ) {
    m_id = value;
    return ( *this );
  };
  CUDA_HOSTDEV bool operator==( const Identifier_Gpu& other ) const { return ( m_id == other.m_id ); }
  CUDA_HOSTDEV bool operator!=( const Identifier_Gpu& other ) const { return ( m_id != other.m_id ); }
  CUDA_HOSTDEV bool operator<( const Identifier_Gpu& other ) const { return ( m_id < other.m_id ); }
  CUDA_HOSTDEV bool operator>( const Identifier_Gpu& other ) const { return ( m_id > other.m_id ); }
  CUDA_HOSTDEV bool operator<=( const Identifier_Gpu& other ) const { return ( m_id <= other.m_id ); }
  CUDA_HOSTDEV bool operator>=( const Identifier_Gpu& other ) const { return ( m_id >= other.m_id ); }
  CUDA_HOSTDEV bool operator==( Identifier_Gpu::value_type other ) const { return ( m_id == other ); }
  CUDA_HOSTDEV bool operator!=( Identifier_Gpu::value_type other ) const { return ( m_id != other ); }

protected:
  value_type m_id;
};

#endif
