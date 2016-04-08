/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CELLPARTITIONING_H_HEADER_INCLUDED_D505A5F1
#define CELLPARTITIONING_H_HEADER_INCLUDED_D505A5F1
#include <vector>

//##ModelId=47A79B4E00C6
class CellPartitioning
{
  public:

    //##ModelId=47A79C30000C
    CellPartitioning(unsigned int firstDivisionNumber = 0);

    //##ModelId=47A79C30001C
    CellPartitioning(const CellPartitioning& right);

    //##ModelId=47A79C30005D
    virtual ~CellPartitioning();

    //##ModelId=47A79C5902EC
    CellPartitioning& operator=(const CellPartitioning& right);

    //##ModelId=47A79D8C032E
    void addValue(double xVal);

    //##ModelId=47A79DAB0286
    double binLower(unsigned int i) const
    {
      return m_partition[i-getFirstDivisionNumber()];
    }

    //##ModelId=47A79DD5004E
    double binUpper(unsigned int i) const
    {
      return m_partition[i-getFirstDivisionNumber()+1];
    }

    //##ModelId=47A79EA6012E
    double binCenter(unsigned int i) const
    {
      return 0.5*(binUpper(i)+binLower(i));
    }

    //##ModelId=47A79EC2021E
    unsigned int getNumDivisions() const
    {
      return m_partition.size()-1;
    }

    //##ModelId=47A79EF100D0
    unsigned int getFirstDivisionNumber() const
    {
      return m_firstDivisionNumber;
    }

    //##ModelId=47A79F0F03B6
    double getStart() const
    {
      return m_partition[0];
    }

    //##ModelId=47A79F220320
    double getEnd() const
    {
      return m_partition[m_partition.size()-1];
    }

  private:

    //##ModelId=47A7A0990065
    std::vector<double> m_partition;

    //##ModelId=47A7A20C024D
    unsigned int m_firstDivisionNumber;
};



#endif /* CELLPARTITIONING_H_HEADER_INCLUDED_D505A5F1 */
