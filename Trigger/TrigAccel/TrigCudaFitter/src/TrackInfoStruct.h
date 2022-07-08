#pragma once

struct TrackInfoStruct
{
public:
	TrackInfoStruct(void);
	~TrackInfoStruct(void);
	float m_params[5];
	int m_nHits;
	long m_nOffsetG;
	long m_nOffsetH;
};
