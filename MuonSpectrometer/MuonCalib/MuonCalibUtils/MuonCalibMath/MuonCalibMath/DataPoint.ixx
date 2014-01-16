//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 07.06.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//:::::::::::::::::::::::::::::::::::::::::::::::::::
//:: INLINE METHODS DEFINED IN THE CLASS DataPoint ::
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

//::::::::::::::::
//:: OPERATOR < ::
//::::::::::::::::

inline bool DataPoint::operator < (const DataPoint & point) const {

    return m_vec[m_ref_comp]<point.dataVector()[m_ref_comp];

}

//*****************************************************************************

//::::::::::::::::
//:: OPERATOR > ::
//::::::::::::::::

inline bool DataPoint::operator > (const DataPoint & point) const {

    return m_vec[m_ref_comp]>point.dataVector()[m_ref_comp];

}

//*****************************************************************************

//:::::::::::::::::
//:: OPERATOR <= ::
//:::::::::::::::::

inline bool DataPoint::operator <= (const DataPoint & point) const {

    return m_vec[m_ref_comp]<=point.dataVector()[m_ref_comp];

}

//*****************************************************************************

//:::::::::::::::::
//:: OPERATOR >= ::
//:::::::::::::::::

inline bool DataPoint::operator >= (const DataPoint & point) const {

    return m_vec[m_ref_comp]>=point.dataVector()[m_ref_comp];

}

//*****************************************************************************

//:::::::::::::::::
//:: OPERATOR == ::
//:::::::::::::::::

inline bool DataPoint::operator == (const DataPoint & point) const {

    return m_vec[m_ref_comp]==point.dataVector()[m_ref_comp];

}
